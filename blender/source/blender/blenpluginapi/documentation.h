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
 *
 * @mainpage plugin API - the access point for texture and sequence
 * plugins
 *
 * @section about About the plugin API
 *
 * This API wraps functions that are used by texture and sequence
 * plugins. They are explicitly wrapped in order to make the
 * dependencies to the rest of the system clear.
 *
 * @section issues Known issues with the plugin API
 *
 * - It can be difficult at times to get access to the API functions
 * linked into the final executable. On gcc, the -fpic and -shared
 * flags take care of this. On Irix, -shared is needed, but you also
 * need a reference to the function to get the handle. This has
 * momentarily been taken care of by pluginapi_force_ref().
 *
 * - Plugins need to define three functions that are needed for
 * version bookkeeping and information. The plugin loading code
 * explicitly checks for these functions. The nanes depend on whether
 * it is a texture or sequence plugin.
 *
 * - The plugin loading occurs in sequence.c and texture.c. The
 * following functions are involved:
 *   - open_plugin_seq() (used in readfile.c, editseq.c, sequence.c)
 *   - add_plugin_seq()  (used in editseq.c, sequence.c)
 *   - free_plugin_seq() (used in editseq.c, sequence.c)
 *   - open_plugin_tex() (used in texture.c, readfile.c)
 *   - add_plugin_tex()  (used in texture.c, buttons.c)
 *   - free_plugin_tex() (used in texture.c, buttons.c)
 *   - test_dlerr()      (used in texture.c, sequence.c)
 * Since the plugins are about to phase out, we will not sanitize this
 * code. It will be removed as soon as the replacing system is in
 * place.
 * 
 * @section dependencies Dependencies
 *
 * The plugins wraps functions from IMB and BLI. In addition, they
 * define some useful variables.
 * */
