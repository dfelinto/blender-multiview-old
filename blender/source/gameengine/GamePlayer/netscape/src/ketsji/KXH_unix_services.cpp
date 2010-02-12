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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#if !defined(_WIN32) && !defined(__APPLE__)

#include "SCA_IInputDevice.h"
//#include "SCA_ISystem.h"
#include "GPC_MouseDevice.h"
#include "GPU_KeyboardDevice.h"
#include "GPU_System.h"
#include "GPU_Canvas.h"
#include "GPC_RenderTools.h"
#include "GPC_RawImage.h"
#include "GPC_RawLoadDotBlendArray.h"
#include "GPC_RawLogoArrays.h"
#include "SND_DeviceManager.h"
#include "RAS_OpenGLRasterizer.h"
#include "NG_LoopBackNetworkDeviceInterface.h"

#include "NZC_application_callback.h"
#include "KXH_unix_services.h"


void
KXH_create_devices(
	ketsji_engine_data* k
	)
{
	int width = 0;
	int height = 0;

	k->logic_system = new GPU_System();
	
	// devices
	k->keyboarddevice = new GPU_KeyboardDevice();
	k->mousedevice  = new GPC_MouseDevice();
	k->net_dev = new NG_LoopBackNetworkDeviceInterface();
	
	// setup audio
  	SND_DeviceManager::Subscribe();
	SND_DeviceManager::SetDeviceType(snd_e_dummydevice);
	k->audiodevice = SND_DeviceManager::Instance();

	/* Apart from the sizes, we don't need to do anything here. Proper
	 * initialization is not an issue here... without correct canvas,
	 * we'd never have ended up here. Why is this so much different
	 * from the win solution ? */
	width = PLA_get_display_width(k->plugin);
	height = PLA_get_display_height(k->plugin);
	k->canvas_device = new GPU_Canvas((KXH_plugin_handle) k, 
					  width, 
					  height);

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
	
	GPU_Canvas* c = (GPU_Canvas*) k->canvas_device;
	
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


#endif

