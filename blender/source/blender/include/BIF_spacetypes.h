/**
 * $Id$
 *
 * ***** BEGIN GPL/BL DUAL LICENSE BLOCK *****
 *
 * The contents of this file may be used under the terms of either the GNU
 * General Public License Version 2 or later (the "GPL", see
 * http://www.gnu.org/licenses/gpl.html ), or the Blender License 1.0 or
 * later (the "BL", see http://www.blender.org/BL/ ) which has to be
 * bought from the Blender Foundation to become active, in which case the
 * above mentioned GPL option does not apply.
 *
 * The Original Code is Copyright (C) 2002 by NaN Holding BV.
 * All rights reserved.
 *
 * The Original Code is: all of this file.
 *
 * Contributor(s): none yet.
 *
 * ***** END GPL/BL DUAL LICENSE BLOCK *****
 */
struct ScrArea;

typedef struct _SpaceType	SpaceType;

typedef	void	(*SpaceDrawFP)		(void);
typedef	void	(*SpaceChangeFP)	(void);
typedef	void	(*SpaceHandleFP)	(unsigned short evt, short val, char ascii);

	/***/

SpaceType*	spacetype_new			(char *name);

void		spacetype_set_winfuncs	(SpaceType *st, SpaceDrawFP draw, SpaceChangeFP change, SpaceHandleFP handle);

	/***/

SpaceType *spaceaction_get_type		(void);
SpaceType *spacebuts_get_type		(void);
SpaceType *spacefile_get_type		(void);
SpaceType *spaceimage_get_type		(void);
SpaceType *spaceimasel_get_type		(void);
SpaceType *spaceinfo_get_type		(void);
SpaceType *spaceipo_get_type		(void);
SpaceType *spacenla_get_type		(void);
SpaceType *spaceoops_get_type		(void);
SpaceType *spaceseq_get_type		(void);
SpaceType *spacesound_get_type		(void);
SpaceType *spacetext_get_type		(void);
SpaceType *spaceview3d_get_type		(void);
