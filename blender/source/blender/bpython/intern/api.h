/* $Id$ 
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
 *
 *
 * This headerfile defines the API status.
 * Parts of the API can be compiled as dynamic module for testing -- 
 * see Makefile
 */

#undef EXPERIMENTAL  /* undefine this for release, please :-) */

/* Uncomment this if you want to have the new blender module
compiled static into Blender :*/
/*  #define SHAREDMODULE  -- put into Makefile */

/* API configuration -- define in Makefile */

#ifdef SHAREDMODULE 
	#define BLENDERMODULE _Blender
#elif defined(CURRENT_PYTHON_API)
	#define BLENDERMODULE Blender
#elif defined(FUTURE_PYTHON_API)
	#define BLENDERMODULE _Blender
#else // FALLBACK
	#define BLENDERMODULE Blender
#endif	

#define SUBMODULE(mod) (MODNAME(BLENDERMODULE) "." #mod)

/* this macro defines the init routine for dynamically loaded modules;
example:

void INITMODULE(BLENDERMODULE) -> void initBlender(void)
*/

#define _INITMODULE(x) init##x
#define INITMODULE(x) _INITMODULE(x)

/* MODNAME(MODULE) stringifies the module definition, example:
MODNAME(BLENDERMODULE) -> "_Blender"
*/

#define _MODNAME(x) #x
#define MODNAME(x) _MODNAME(x)

// module configuration -- TODO: this should be set later from the Makefile...
#if defined(__FreeBSD__) || defined(__linux__) || defined (__sgi) || defined(__sparc)
#define STATIC_TEXTTOOLS 1
#endif


#define USE_NMESH 1     // still use NMesh structure for <mesh object>.data
#define CLEAR_NAMESPACE // undefine this if you still want the old dirty global 
                        // namespace shared by ALL scripts. 


// experimental sh*t:
#ifdef EXPERIMENTAL
	#undef USE_NMESH
#endif
