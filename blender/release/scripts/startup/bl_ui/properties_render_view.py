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
import bpy
from bpy.types import Menu, Panel, UIList


class RenderViewButtonsPanel():
    bl_space_type = 'PROPERTIES'
    bl_region_type = 'WINDOW'
    bl_context = "render_view"
    # COMPAT_ENGINES must be defined in each subclass, external engines can add themselves here

    @classmethod
    def poll(cls, context):
        scene = context.scene
        return scene and (scene.render.engine in cls.COMPAT_ENGINES)


class RENDERVIEW_UL_renderviews(UIList):
    def draw_item(self, context, layout, data, item, icon, active_data, active_propname, index):
        # assert(isinstance(item, bpy.types.SceneRenderView)
        view = item
        if self.layout_type in {'DEFAULT', 'COMPACT'}:
            layout.label(view.name, icon_value=icon + (not view.use))
            layout.prop(view, "use", text="", index=index)
        elif self.layout_type in {'GRID'}:
            layout.alignment = 'CENTER'
            layout.label("", icon_value=icon + (not view.use))


class RENDERVIEW_PT_views(RenderViewButtonsPanel, Panel):
    bl_label     = "View List"
    bl_options = {'HIDE_HEADER'}
    COMPAT_ENGINES = {'BLENDER_RENDER'}

    def draw(self, context):
        layout = self.layout

        scene = context.scene
        rd = scene.render

        row = layout.row()
        row.template_list("RENDERVIEW_UL_renderviews", "", rd, "views", rd.views, "active_index", rows=2)

        col = row.column(align=True)
        col.operator("scene.render_view_add", icon='ZOOMIN', text="")
        col.operator("scene.render_view_remove", icon='ZOOMOUT', text="")

        row = layout.row()
        rv = rd.views.active
        if rv and rv.name not in ('left', 'right'):
            row.prop(rv, "name")
        else:
            row.label()
        row.prop(rd, "use_single_view", text="", icon_only=True)


class RENDERVIEW_PT_view_options(RenderViewButtonsPanel, Panel):
    bl_label = "View"
    COMPAT_ENGINES = {'BLENDER_RENDER'}

    def draw(self, context):
        layout = self.layout

        scene = context.scene
        rd = scene.render
        rv = rd.views.active

        split = layout.split()

        col = split.column()
        col.prop(rv, "camera")

        col.separator()
        col.prop(rv, "use_custom_suffix")

        sub = col.column()
        sub.active =  rv.use_custom_suffix
        sub.prop(rv, "file_suffix")


if __name__ == "__main__":  # only for live edit.
    bpy.utils.register_module(__name__)
