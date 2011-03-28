# ##### BEGIN GPL LICENSE BLOCK #####
#
#  This program is free software; you can redistribute it and/or
#  modify it under the terms of the GNU General Public License
#  as published by the Free Software Foundation; either version 2
#  of the License, or (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software Foundation,
#  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
#
# ##### END GPL LICENSE BLOCK #####

# <pep8 compliant>

'''
looks for demo.py textblock:
# --- example
config = [
    dict(file="./demo.blend", animate=1, screen_switch=1),
    dict(file="./demo_2.blend", render=1),
    dict(file="./demo_2.blend", animate=1, screen_switch=1),
    ]
# ---
'''



import bpy
import sys
import time
import tempfile
import os

# populate from script
global_config_files = []

global_config = {
    "animate": 0,  # seconds
    "render": 0,
    "screen_switch": 0,
}

# switch to the next file in 2 sec.
global_config_fallback = {
    "animate": 2,  # seconds
    "render": 0,
    "screen_switch": 0,
}

global_state = {
    "init_time": 0.0,
    "last_switch": 0.0,
    "reset_anim": False,
    "render_out": "",
    "render_time": "",  # time render was finished.
    "timer": None,
    "basedir": "",  # demo.py is stored here
    "demo_index": 0,
}


def demo_mode_next_file():
    global_state["demo_index"] += 1

    if global_state["demo_index"] >= len(global_config_files):
        global_state["demo_index"] = 0

    print("func:demo_mode_next_file", global_state["demo_index"])
    filepath = global_config_files[global_state["demo_index"]]["file"]
    bpy.ops.wm.open_mainfile(filepath=filepath)


def demo_mode_timer_add():
    global_state["timer"] = bpy.context.window_manager.event_timer_add(0.8, bpy.context.window)


def demo_mode_timer_remove():
    if global_state["timer"]:
        bpy.context.window_manager.event_timer_remove(global_state["timer"])
        global_state["timer"] = None


def demo_mode_load_file():
    """ Take care, this can only do limited functions since its running
        before the file is fully loaded.
        Some operators will crash like playing an animation.
    """
    print("func:demo_mode_load_file")
    DemoMode.first_run = True
    bpy.ops.wm.demo_mode('EXEC_DEFAULT')


def demo_mode_init():
    print("func:demo_mode_init")
    DemoKeepAlive.ensure()

    if 1:
        global_config.clear()
        global_config.update(global_config_files[global_state["demo_index"]])

    print(global_config)

    demo_mode_timer_add()

    if global_config["animate"]:
        bpy.ops.screen.animation_play()

    elif global_config["render"]:
        print("  render")
        global_state["render_out"] = tempfile.mkstemp()[1]

        bpy.context.scene.render.filepath = global_state["render_out"]
        bpy.context.scene.render.use_file_extension = False
        bpy.context.scene.render.use_placeholder = False
        if os.path.exists(global_state["render_out"]):
            print("  render!!!")
            os.remove(global_state["render_out"])

        bpy.ops.render.render('INVOKE_DEFAULT', write_still=True)

    global_state["init_time"] = global_state["last_switch"] = time.time()
    global_state["render_time"] = -1.0


def demo_mode_update():
    time_current = time.time()
    time_delta = time_current - global_state["last_switch"]
    time_total = time_current - global_state["init_time"]

    # --------------------------------------------------------------------------
    # ANIMATE MODE
    if global_config["animate"]:
        # check for exit
        if time_total > global_config["animate"]:
            demo_mode_next_file()
            return

        # run update funcs
        if global_state["reset_anim"]:
            global_state["reset_anim"] = False
            bpy.ops.screen.animation_cancel(restore_frame=False)
            bpy.ops.screen.animation_play()

        if global_config["screen_switch"]:
            # print(time_delta, 1)
            if time_delta > global_config["screen_switch"]:

                screen = bpy.context.window.screen
                index = bpy.data.screens.keys().index(screen.name)
                screen_new = bpy.data.screens[(index if index > 0 else len(bpy.data.screens)) - 1]
                bpy.context.window.screen = screen_new

                global_state["last_switch"] = time_current

                if global_config["animate"]:
                    global_state["reset_anim"] = True

    # --------------------------------------------------------------------------
    # RENDER MODE
    elif global_config["render"]:
        if os.path.exists(global_state["render_out"]):
            # wait until the time has passed
            if global_state["render_time"] == -1.0:
                global_state["render_time"] = time.time()
            else:
                if time.time() - global_state["render_time"] > global_config["render"]:
                    os.remove(global_state["render_out"])
                    demo_mode_next_file()
                    return


# -----------------------------------------------------------------------------
# modal operator

class DemoKeepAlive:
    secret_attr = "_keepalive"

    @staticmethod
    def ensure():
        if DemoKeepAlive.secret_attr not in bpy.app.driver_namespace:
            bpy.app.driver_namespace[DemoKeepAlive.secret_attr] = DemoKeepAlive()

    @staticmethod
    def remove():
        if DemoKeepAlive.secret_attr in bpy.app.driver_namespace:
            del bpy.app.driver_namespace[DemoKeepAlive.secret_attr]

    def __del__(self):
        """ Hack, when the file is loaded the drivers namespace is cleared.
        """
        if DemoMode.enabled:
            demo_mode_load_file()


class DemoMode(bpy.types.Operator):
    bl_idname = "wm.demo_mode"
    bl_label = "Demo"

    enabled = False

    first_run = True

    def cleanup(self, disable=False):
        demo_mode_timer_remove()
        self.__class__.first_run = True

        if disable:
            self.__class__.enabled = False
            DemoKeepAlive.remove()

    def modal(self, context, event):
        print("DemoMode.modal")
        if event.type == 'ESC':
            self.cleanup(disable=True)
            # disable here and not in cleanup because this is a user level disable.
            # which should stay disabled until explicitly enabled again.
            return {'CANCELLED'}

        # print(event.type)
        if self.__class__.first_run:
            self.__class__.first_run = False

            demo_mode_init()
        else:
            demo_mode_update()

        return {'PASS_THROUGH'}

    def execute(self, context):
        print("func:DemoMode.execute")
        self.__class__.enabled = True
        context.window_manager.modal_handler_add(self)
        return {'RUNNING_MODAL'}

    def cancel(self, context):
        print("func:DemoMode.cancel")
        # disable here means no running on file-load.
        self.cleanup()
        return None


def menu_func(self, context):
    print("func:menu_func - DemoMode.enabled:", DemoMode.enabled, "bpy.app.driver_namespace:", DemoKeepAlive.secret_attr not in bpy.app.driver_namespace, 'global_state["timer"]:', global_state["timer"])
    if not DemoMode.enabled:
        pass
    if 1:
        layout = self.layout
        layout.operator_context = 'EXEC_DEFAULT'
        layout.operator("wm.demo_mode", icon='PLAY')


def register():
    bpy.utils.register_class(DemoMode)
    bpy.types.INFO_HT_header.append(menu_func)


def unregister():
    bpy.utils.unregister_class(DemoMode)


# -----------------------------------------------------------------------------
# parse args

def load_config(cfg_name="demo.py"):
    namespace = {}
    text = bpy.data.texts.get(cfg_name)
    if text is None:
        basedir = os.path.dirname(bpy.data.filepath)
        demo_path = os.path.join(basedir, "demo.py")
        demo_file = open(demo_path, "r")
        demo_data = demo_file.read()
    else:
        demo_data = text.as_string()
        demo_path = os.path.join(bpy.data.filepath, cfg_name)  # fake

    namespace["__file__"] = demo_path

    exec(demo_data, namespace, namespace)

    demo_file.close()

    global_config_files[:] = []

    for filecfg in namespace["config"]:

        # defaults
        filecfg["render"] = filecfg.get("render", 0)
        filecfg["animate"] = filecfg.get("animate", 0)
        filecfg["screen_switch"] = filecfg.get("screen_switch", 0)

        if not os.path.exists(filecfg["file"]):
            filepath_test = os.path.join(basedir, filecfg["file"])
            if not os.path.exists(filepath_test):
                print("Cant find %r or %r, skipping!")
                continue
            filecfg["file"] = os.path.normpath(filepath_test)

        # sanitize
        filecfg["file"] = os.path.abspath(filecfg["file"])
        filecfg["file"] = os.path.normpath(filecfg["file"])
        print("  Adding:", filecfg["file"])
        global_config_files.append(filecfg)

    print("found %d files" % len(global_config_files))

    global_state["basedir"] = basedir


if __name__ == "__main__":
    load_config()
    register()

    # starts the operator
    demo_mode_load_file()
