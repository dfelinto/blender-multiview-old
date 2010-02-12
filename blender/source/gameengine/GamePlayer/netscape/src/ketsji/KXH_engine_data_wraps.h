/**
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
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * The Original Code is Copyright (C) 2001-2002 by NaN Holding BV.
 * All rights reserved.
 *
 * The Original Code is: all of this file.
 *
 * Contributor(s): none yet.
 *
 * ***** END GPL LICENSE BLOCK *****
 * Data wraps for the engine.
 */

#ifndef KXH_ENGINE_DATA_WRAPS_H
#define KXH_ENGINE_DATA_WRAPS_H


#include "PLG_plugin_handles.h"
#include "prthread.h"

/**********************************
 * Begin Blender include block
 **********************************/
#ifdef __cplusplus
extern "C"
{
#endif  // __cplusplus
#include "BLI_blenlib.h"
#include "BLO_readfile.h"
#include "DNA_scene_types.h"
#include "BKE_blender.h"
#include "BKE_report.h"
#ifdef __cplusplus
}
#endif // __cplusplus
/**********************************
 * End Blender include block
 **********************************/


#ifdef __cplusplus
extern "C" {
#endif

	/* forwards: */
	class KX_KetsjiEngine;
	class GPC_System;
	class GPC_Canvas;
	class RAS_IRasterizer;
	class RAS_IRenderTools;
	class GPC_KeyboardDevice;
	class GPC_MouseDevice;
	class NG_NetworkDeviceInterface;
	class SND_IAudioDevice;
	class KX_BlenderSceneConverter;
	class GPC_RawImage;
	class GPC_RawImage;
	struct BlendFileData;
	struct _thread_data_wrap;

	enum KXH_STATE {
		KXH_NO_STATE = 0,
		KXH_INIT,                     // 1
		KXH_SHOW_FG_COLOR,            // 2
		KXH_TEST_LOADING_FILE_TYPE,   // 3
		KXH_PLAY_LOADING_ANIM,        // 4
		KXH_TEST_MAIN_FILE,           // 5
		KXH_ADD_LOGOS,                // 6
		KXH_PLAY_MAIN_FILE,           // 7
		KXH_AWAITING_NEW_STREAM,      // 8
		KXH_GL_ERROR_STATE,           // 9
		KXH_CLEAN_UP,                 // 10
		KXH_MAX_STATE                 // 11
	};


	enum KXH_MAIN_FILE_STATE {
		KXH_MAIN_FILE_NO_STATE,
		KXH_MAIN_FILE_IS_PUBLISHER_FILE,
		KXH_MAIN_FILE_IS_NOT_PUBLISHER_FILE,
		KXH_MAIN_FILE_CORRUPT,
		KXH_MAIN_FILE_MAX_STATE	
	};


	typedef struct _ketsji_engine_data {

		/** Flag that terminates the plugin. */	
		bool terminate;

		/** Flag to raise a request for termination */
		bool termination_request;

		/** Reference to the plugin resources (gl and x contexts). */
		PLA_plugin_handle plugin;

		/** Reference for getting the references right. */
		struct _thread_data_wrap* thread_data;

		/** The active gameengine. */
		KX_KetsjiEngine* kx_engine;

		/** The engine's logic system. */
		GPC_System* logic_system;

		/** The canvas. We sometimes need to send resizes and such. */
		GPC_Canvas* canvas_device;

		/** The rasterizer */
		RAS_IRasterizer* rasterizer;
	
		/** The Rendertools */
		RAS_IRenderTools* rendertools;
	
		/** The keyboarddevice */
		GPC_KeyboardDevice* keyboarddevice;
	
		/** The mousedevice */
		GPC_MouseDevice* mousedevice;
	
		/** The network device */
		NG_NetworkDeviceInterface* net_dev;

		/** The audiodevice */
		SND_IAudioDevice* audiodevice;
	
		/** a blend-to-kx scene converter */
		KX_BlenderSceneConverter* converter;

		/** State of the gameengine. Refer to the spec to see the
		 * meaning of the names. */
		KXH_STATE state;

		/** Is the gameengine initialised? */
		bool kx_initialized;

		/** The target blend file */
		struct BlendFileData *blendfile;

		/** Did the main file retrieval fail for some reason? */
		bool blendfile_failed;

		/** The loading anim resource. */
		struct BlendFileData *loading_anim;
		/** Did the loading anim file retrieval fail for some reason? */
		bool loading_anim_failed;

		/** Placeholder for blender logo, if needed */
		GPC_RawImage* blender_logo;

		/** Placeholder for url blender logo, if needed */
		GPC_RawImage* blender_url_logo;

		/** Time in 10e-6 seconds to the next redraw request. */
		int sleep_time;

		/** Restart with new streaming url? */
		bool replacing_url_requested;

		bool new_url_requested;

	} ketsji_engine_data;


	typedef struct _thread_data_wrap {
		/** Thread for the plugin. This is retained, so we can properly
		    cancel and join on termination. */
		PRThread* t;

		/** Handle to the game data, so we can send the terminate signal. */
		ketsji_engine_data* ked;
	
		/** Handle to the plugin, for resources. */
		PLA_plugin_handle ph;

	} thread_data_wrap;

#ifdef __cplusplus
}
#endif

#endif

