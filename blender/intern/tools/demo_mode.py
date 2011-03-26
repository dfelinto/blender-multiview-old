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

# WIP, this needs a timer to run before python:
# C code, WM_event_add_timer(CTX_wm_manager(C), CTX_wm_window(C), 0x0110, 0.1f);
#
# play for 8 seconds then quick, switch screens every 4 sec.
# ./blender.bin demo.blend --python demo_mode.py -- --animate=8 --screen_switch=4
#
# render and pause for 3 seconds.
# ./blender.bin demo.blend --python demo_mode.py -- --render 3


import bpy
import sys
import time
import tempfile
import os


global_config = {
    "animate": 0,  # seconds
    "render": 0,
    "screen_switch": 0,
    "_init_time": 0.0,
    "_last_switch": 0.0,
    "_reset_anim": False,
    "_render_out": "",
}


def demo_mode_init():
    if global_config["animate"]:
        bpy.ops.screen.animation_play()

    if global_config["render"]:
        global_config["_render_out"] = tempfile.mkstemp()[1]

        bpy.context.scene.render.filepath = global_config["_render_out"]
        bpy.context.scene.render.use_file_extension = False
        bpy.context.scene.render.use_placeholder = False
        if os.path.exists(global_config["_render_out"]):
            os.remove(global_config["_render_out"])

        bpy.ops.render.render('INVOKE_DEFAULT', write_still=True)

    global_config["_init_time"] = global_config["_last_switch"] = time.time()


def demo_mode_update():
    time_current = time.time()
    time_delta = time_current - global_config["_last_switch"]
    time_total = time_current - global_config["_init_time"]

    # check for exit
    if global_config["animate"] and time_total > global_config["animate"]:
        sys.exit(0)

    if global_config["render"] and os.path.exists(global_config["_render_out"]):
        # sleep and quit
        os.remove(global_config["_render_out"])
        time.sleep(global_config["render"])
        sys.exit(0)

    # run update funcs
    if global_config["_reset_anim"]:
        global_config["_reset_anim"] = False
        f = bpy.context.scene.frame_current
        bpy.ops.screen.animation_cancel()
        bpy.context.scene.frame_set(f)
        bpy.ops.screen.animation_play()

    if global_config["screen_switch"]:
        print(time_delta, 1)
        if time_delta > global_config["screen_switch"]:
            screen = bpy.context.window.screen
            index = bpy.data.screens.keys().index(screen.name)
            screen_new = bpy.data.screens[(index if index > 0 else len(bpy.data.screens)) - 1]
            bpy.context.window.screen = screen_new

            global_config["_last_switch"] = time_current

            if global_config["animate"]:
                global_config["_reset_anim"] = True


# -----------------------------------------------------------------------------
# modal operator

class DemoMode(bpy.types.Operator):
    bl_idname = "wm.demo_mode"
    bl_label = "Simple Modal Operator"

    _first_run = True

    def modal(self, context, event):
        if event.type in ('RIGHTMOUSE', 'ESC'):
            return {'CANCELLED'}
        # print(event.type)
        if self.__class__._first_run:
            self.__class__._first_run = False

            demo_mode_init()
        else:
            demo_mode_update()

        return {'PASS_THROUGH'}

    def execute(self, context):
        context.window_manager.modal_handler_add(self)
        return {'RUNNING_MODAL'}


def register():
    bpy.utils.register_class(DemoMode)


def unregister():
    bpy.utils.unregister_class(DemoMode)


def main_model_op():
    register()
    bpy.ops.wm.demo_mode('EXEC_DEFAULT')


# -----------------------------------------------------------------------------
# parse args


def main():
    import argparse

    # get the args passed to blender after "--", all of which are ignored by blender specifically
    # so python may receive its own arguments
    argv = sys.argv

    if "--" not in argv:
        argv = []  # as if no args are passed
    else:
        argv = argv[argv.index("--") + 1:]  # get all args after "--"

    # When --help or no args are given, print this help
    usage_text = "Run blender in with this script:"
    usage_text += "  blender BLENDFILE --python " + __file__ + " -- [options]"

    parser = argparse.ArgumentParser(description=usage_text)

    # Example background utility, add some text and renders or saves it (with options)
    # Possible types are: string, int, long, choice, float and complex.
    parser.add_argument("-a", "--animate", dest="animate", help="Play animations on load", type=int, metavar="SECONDS")
    parser.add_argument("-r", "--render", dest="render", help="Render on load", type=int, metavar="SECONDS")
    parser.add_argument("-s", "--screen_switch", dest="screen_switch", help="Switch screen every N seconds", type=int, metavar="SECONDS")

    options = parser.parse_args(argv)  # In this example we wont use the args

    if not argv:
        parser.print_help()
        return

    if not options.animate and not options.render:
        print("Error: --render / --animate not given")
        parser.print_help()
        return

    # Run the example function
    global_config["animate"] = options.animate
    global_config["render"] = options.render
    global_config["screen_switch"] = options.screen_switch

    print("batch job finished, exiting")


if __name__ == "__main__":
    main()
    main_model_op()
