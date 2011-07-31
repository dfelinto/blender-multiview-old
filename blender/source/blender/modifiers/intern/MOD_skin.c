/*
* $Id$
*
* ***** BEGIN GPL LICENSE BLOCK *****
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software  Foundation,
* Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*
* ***** END GPL LICENSE BLOCK *****
*
*/

/** \file blender/modifiers/intern/MOD_skin.c
 *  \ingroup modifiers
 */


#include <stddef.h>

#include "MEM_guardedalloc.h"

#include "BLI_editVert.h"
#include "BLI_math.h"

#include "BKE_cdderivedmesh.h"
#include "BKE_DerivedMesh.h"
#include "BKE_modifier.h"

#include "DNA_mesh_types.h"
#include "DNA_meshdata_types.h"
#include "DNA_object_types.h"

#include "MOD_util.h"

/* probably bad level call? */
#include "BIF_gl.h"
#include "UI_resources.h"

#include "GPU_buffers.h"

static void initData(ModifierData *md)
{
	SkinModifierData *smd = (SkinModifierData*)md;

	smd->threshold = 0;
	smd->subdiv = 1;
	smd->flag = MOD_SKIN_DRAW_NODES;
}

static void copyData(ModifierData *md, ModifierData *target)
{
	SkinModifierData *smd = (SkinModifierData*) md;
	SkinModifierData *tsmd = (SkinModifierData*) target;

	tsmd->threshold = smd->threshold;
	tsmd->subdiv = smd->subdiv;
	tsmd->flag = smd->flag;
}

typedef struct {
	DerivedMesh dm;
	EditMesh *em;
} SkinDM;

static void skindm_draw(SkinDM *dm)
{
	float col_vert[3], col_vertsel[3];
	float col_elemact[3];
	EditMesh *em;
	EditVert *eve, *eve_act;
	EditEdge *eed, *eed_act;
	SkinNode *n1, *n2;

	em = dm->em;

	/* get colors */
	UI_GetThemeColor3fv(TH_VERTEX, col_vert);
	UI_GetThemeColor3fv(TH_VERTEX_SELECT, col_vertsel);
	UI_GetThemeColor3fv(TH_EDITMESH_ACTIVE, col_elemact);

	/* check for active vertex */
	if(em->selected.last) {
		EditSelection *ese = em->selected.last;
		if(ese->type == EDITEDGE)
			eed_act = ese->data;
		else if(ese->type == EDITVERT)
			eve_act = ese->data;
	}

	/* set up GL state */
	glShadeModel(GL_SMOOTH);
	glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);

	/* draw nodes */
	for(eve = em->verts.first; eve; eve = eve->next) {
		n1 = CustomData_em_get(&em->vdata, eve->data, CD_SKIN_NODE);

		glPushMatrix();
		glTranslatef(eve->co[0], eve->co[1], eve->co[2]);
		glScalef(n1->radius, n1->radius, n1->radius);
		glColor3fv(eve == eve_act ? col_elemact :
			   eve->f & SELECT ? col_vertsel : col_vert);
		GPU_draw_smooth_sphere();
		glPopMatrix();
	}

	/* draw edges */
	for(eed = em->edges.first; eed; eed = eed->next) {
		float f, t, col[3], *col1, *col2, co[3], radius;
		int i, totstep;

		n1 = CustomData_em_get(&em->vdata, eed->v1->data, CD_SKIN_NODE);
		n2 = CustomData_em_get(&em->vdata, eed->v2->data, CD_SKIN_NODE);

		/* XXX: assuming vert sel mode here? */
		col1 = eed->v1->f & SELECT ? col_vertsel : col_vert;
		col2 = eed->v2->f & SELECT ? col_vertsel : col_vert;

		/* factor to adjust steps by... just guessing at a nice value here */
		f = (n1->radius + n2->radius) / 6;

		totstep = len_v3v3(eed->v1->co, eed->v2->co) / f;
		if(totstep <= 2) totstep = 3;

		for(i = 1; i < totstep-1; i++) {
			t = i / (totstep-1.0f);

			interp_v3_v3v3(col, col1, col2, t);
			interp_v3_v3v3(co, eed->v1->co, eed->v2->co, t);
			radius = interpf(n1->radius, n2->radius, t);

			glPushMatrix();

			glColor3fv(col);
			glTranslatef(co[0], co[1], co[2]);
			glScalef(radius, radius, radius);
			GPU_draw_smooth_sphere();

			glPopMatrix();
		}
	}

	glShadeModel(GL_FLAT);
	glDisable(GL_COLOR_MATERIAL);
}

static void skindm_getMinMax(DerivedMesh *dm, float min_r[3], float max_r[3])
{
	/* TODO */
	zero_v3(min_r);
	zero_v3(max_r);
}

static void skindm_drawEdges(DerivedMesh *dm, int drawLooseEdges, int drawAllEdges)
{
	skindm_draw((SkinDM*)dm);
}

static void skindm_drawMappedFaces(DerivedMesh *dm,
				   int (*setDrawOptions)(void*, int, int*),
				   void *userData, int useColors,
				   int (*setMaterial)(int, void*))
{
	skindm_draw((SkinDM*)dm);
}

static void skindm_release(DerivedMesh *dm)
{
	if(DM_release(dm))
		MEM_freeN(dm);
}

static DerivedMesh *applyModifierEM(ModifierData *md, Object *ob,
				    EditMesh *em,
				    DerivedMesh *dm)
{
	SkinDM *skin;

	skin = MEM_callocN(sizeof(SkinDM), "SkinDM");

	/* standard DerivedMesh stuff */
	skin->dm.drawEdges = skindm_drawEdges;
	skin->dm.drawMappedFaces = skindm_drawMappedFaces;
	skin->dm.getMinMax = skindm_getMinMax;
	skin->dm.release = skindm_release;

	skin->em = em;

	return (DerivedMesh*)skin;
}


static DerivedMesh *applyModifier(ModifierData *md, Object *ob, DerivedMesh *dm,
				  int useRenderParams, int isFinalCalc)
{
	Mesh *me;

	/* XXX: temporary hack to make sure there's some customdata
	   for us to use, later can do a proper interface for this */
	me = ob->data;
	if(!CustomData_get_layer(&me->vdata, CD_SKIN_NODE))
		CustomData_add_layer(&me->vdata, CD_SKIN_NODE, CD_DEFAULT, NULL, me->totvert);

	return dm;
}


ModifierTypeInfo modifierType_Skin = {
	/* name */              "Skin",
	/* structName */        "SkinModifierData",
	/* structSize */        sizeof(SkinModifierData),
	/* type */              eModifierTypeType_Constructive,
	/* flags */             eModifierTypeFlag_AcceptsMesh|eModifierTypeFlag_SupportsEditmode,

	/* copyData */          copyData,
	/* deformVerts */       NULL,
	/* deformMatrices */    NULL,
	/* deformVertsEM */     NULL,
	/* deformMatricesEM */  NULL,
	/* applyModifier */     applyModifier,
	/* applyModifierEM */   applyModifierEM,
	/* initData */          initData,
	/* requiredDataMask */  NULL,
	/* freeData */          NULL,
	/* isDisabled */        NULL,
	/* updateDepgraph */    NULL,
	/* dependsOnTime */     NULL,
	/* dependsOnNormals */	NULL,
	/* foreachObjectLink */ NULL,
	/* foreachIDLink */     NULL,
};
