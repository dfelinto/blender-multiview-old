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
 * Implementation of the application hooks between browser and app, 
 * where app = Ketjsi.
 */

#ifdef _WIN32

#include "SCA_IInputDevice.h"
//#include "SCA_ISystem.h"
#include "GPC_MouseDevice.h"
#include "GPW_KeyboardDevice.h"
#include "GPW_System.h"
#include "GPW_Canvas.h"
#include "GPC_RenderTools.h"
#include "GPC_RawImage.h"
#include "GPC_RawLoadDotBlendArray.h"
#include "GPC_RawLogoArrays.h"
#include "SND_DeviceManager.h"
#include "RAS_OpenGLRasterizer.h"
#include "NG_LoopBackNetworkDeviceInterface.h"

//#include "PLG_plugin_handles.h"
#include "NZC_application_callback.h"
#include "NZC_application_hooks.h"
#include "KXH_MSWindows_services.h"
#include "PLG_MSWindows_windowing.h"


#define GENERATE_LOG
/* Logging: */
static void
log_entry(char* msg);


void
KXH_create_devices(
	ketsji_engine_data* k
	)
{
	int width = 0;
	int height = 0;

// 	log_entry("KXH_create_devices");

	k->logic_system = new GPW_System();
	
	// devices
	k->keyboarddevice = new GPW_KeyboardDevice();
	k->mousedevice  = new GPC_MouseDevice();
	k->net_dev = new NG_LoopBackNetworkDeviceInterface();
	
	// setup audio
  	SND_DeviceManager::Subscribe();
	SND_DeviceManager::SetDeviceType(snd_e_dummydevice);
	k->audiodevice = SND_DeviceManager::Instance();

	/* Apart from the sizes, we don't need to do anything
	 * here. Proper initialization is not an issue here... without
	 * correct canvas, we'd never have ended up here. Why is this
	 * so much different from the win solution ? Because the win
	 * version swaps buffers etc. in the canvas code. There is
	 * less strict separation of concerns on windows...*/
 	width = PLA_get_display_width(k->plugin);
 	height = PLA_get_display_height(k->plugin);
	HWND winhandle = PLA_get_HWND(k->plugin);

 	HDC ctxhandle = PLA_get_HDC(k->plugin);



	GPW_Canvas* temp_canv = new GPW_Canvas(winhandle,
					       ctxhandle,
					       width, 
					       height);
	// does the actual init
// 	log_entry("KXH_create_devices:: initialize canvas:");
	temp_canv->Init();

	k->canvas_device = temp_canv;

	// create a rasterizer
	k->rasterizer = new RAS_OpenGLRasterizer(k->canvas_device);

	// Well, rendertools only wraps a few gl-calls...
	k->rendertools = new GPC_RenderTools();

}

void
KXH_add_banners(
	ketsji_engine_data* k
	)
{

// 	log_entry("KXH_add_banners");
	
	GPW_Canvas* c = (GPW_Canvas*) k->canvas_device;
	
	k->blender_logo = new GPC_RawImage();
	k->blender_url_logo = new GPC_RawImage();
	
	if(!k->blender_logo->Load("BlenderLogo", 
				  128, 128, 
				  GPC_RawImage::alignTopLeft, 
				  8, 8)) {
		// Out of memory?
		k->blender_logo = 0;
	} else {
		c->AddBanner(k->blender_logo->Width(), 
			     k->blender_logo->Height(),
			     k->blender_logo->Width(), 
			     k->blender_logo->Height(),
			     k->blender_logo->Data(), 
			     GPC_Canvas::alignTopLeft);
	}
	
	if(!k->blender_url_logo->Load("Blender3DLogo", 
				      256, 256, 
				      GPC_RawImage::alignBottomRight, 
				      8, 8)) {
		// Out of memory?
		k->blender_url_logo = 0;
	} else {
		c->AddBanner(k->blender_url_logo->Width(), 
			     k->blender_url_logo->Height(),
			     k->blender_url_logo->Width(), 
			     k->blender_url_logo->Height(),
			     k->blender_url_logo->Data(), 
			     GPC_Canvas::alignBottomRight);
	}
	
	c->SetBannerDisplayEnabled(true);
}


static void
log_entry(char* msg)
{
#ifdef GENERATE_LOG
	FILE* fp = fopen("/tmp/plugin_log","a");
	if (!fp) return;
  	fprintf(fp, "--> KXH_MSWindows_services::(%p) %s\n", 
		PR_GetCurrentThread(),
		msg); 
	fflush(fp);
	fclose (fp);
#endif
}

#endif

