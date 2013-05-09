/*
 * Copyright 2011, Blender Foundation.
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
 * Contributor: 
 *		Jeroen Bakker 
 *		Monique Dewanchand
 *		Lukas Tönne
 */

#include "COM_OutputFileOperation.h"
#include "COM_SocketConnection.h"
#include <string.h>
#include "BLI_listbase.h"
#include "BLI_path_util.h"
#include "BLI_string.h"
#include "BKE_image.h"
#include "BKE_global.h"
#include "BKE_main.h"

#include "DNA_color_types.h"

extern "C" {
	#include "MEM_guardedalloc.h"
	#include "IMB_imbuf.h"
	#include "IMB_colormanagement.h"
	#include "IMB_imbuf_types.h"
}

static int get_datatype_size(DataType datatype)
{
	switch (datatype) {
		case COM_DT_VALUE:  return 1;
		case COM_DT_VECTOR: return 3;
		case COM_DT_COLOR:  return 4;
		default:            return 0;
	}
}

static float *init_buffer(unsigned int width, unsigned int height, DataType datatype)
{
	// When initializing the tree during initial load the width and height can be zero.
	if (width != 0 && height != 0) {
		int size = get_datatype_size(datatype);
		return (float *)MEM_callocN(width * height * size * sizeof(float), "OutputFile buffer");
	}
	else
		return NULL;
}

static void write_buffer_rect(rcti *rect, const bNodeTree *tree,
                              SocketReader *reader, float *buffer, unsigned int width, DataType datatype)
{
	float color[4];
	int i, size = get_datatype_size(datatype);

	if (!buffer) return;
	int x1 = rect->xmin;
	int y1 = rect->ymin;
	int x2 = rect->xmax;
	int y2 = rect->ymax;
	int offset = (y1 * width + x1) * size;
	int x;
	int y;
	bool breaked = false;

	for (y = y1; y < y2 && (!breaked); y++) {
		for (x = x1; x < x2 && (!breaked); x++) {
			reader->read(color, x, y, COM_PS_NEAREST);
			
			for (i = 0; i < size; ++i)
				buffer[offset + i] = color[i];
			offset += size;
			
			if (tree->test_break && tree->test_break(tree->tbh))
				breaked = true;
		}
		offset += (width - (x2 - x1)) * size;
	}
}


OutputSingleLayerOperation::OutputSingleLayerOperation(
        const RenderData *rd, const bNodeTree *tree, DataType datatype, ImageFormatData *format, const char *path,
        const ColorManagedViewSettings *viewSettings, const ColorManagedDisplaySettings *displaySettings)
{
	this->m_rd = rd;
	this->m_tree = tree;
	
	this->addInputSocket(datatype);
	
	this->m_outputBuffer = NULL;
	this->m_datatype = datatype;
	this->m_imageInput = NULL;
	
	this->m_format = format;
	BLI_strncpy(this->m_path, path, sizeof(this->m_path));

	this->m_viewSettings = viewSettings;
	this->m_displaySettings = displaySettings;
}

void OutputSingleLayerOperation::initExecution()
{
	this->m_imageInput = getInputSocketReader(0);
	this->m_outputBuffer = init_buffer(this->getWidth(), this->getHeight(), this->m_datatype);
}

void OutputSingleLayerOperation::executeRegion(rcti *rect, unsigned int tileNumber)
{
	write_buffer_rect(rect, this->m_tree, this->m_imageInput, this->m_outputBuffer, this->getWidth(), this->m_datatype);
}

void OutputSingleLayerOperation::deinitExecution()
{
	if (this->getWidth() * this->getHeight() != 0) {
		
		int size = get_datatype_size(this->m_datatype);
		ImBuf *ibuf = IMB_allocImBuf(this->getWidth(), this->getHeight(), this->m_format->planes, 0);
		Main *bmain = G.main; /* TODO, have this passed along */
		char filename[FILE_MAX];
		
		ibuf->channels = size;
		ibuf->rect_float = this->m_outputBuffer;
		ibuf->mall |= IB_rectfloat; 
		ibuf->dither = this->m_rd->dither_intensity;
		
		IMB_colormanagement_imbuf_for_write(ibuf, TRUE, FALSE, m_viewSettings, m_displaySettings,
		                                    this->m_format);

		BKE_makepicstring(filename, this->m_path, bmain->name, this->m_rd->cfra, this->m_format,
		                  (this->m_rd->scemode & R_EXTENSION), true);
		
		if (0 == BKE_imbuf_write(ibuf, filename, this->m_format))
			printf("Cannot save Node File Output to %s\n", filename);
		else
			printf("Saved: %s\n", filename);
		
		IMB_freeImBuf(ibuf);
	}
	this->m_outputBuffer = NULL;
	this->m_imageInput = NULL;
}


OutputOpenExrLayer::OutputOpenExrLayer(const char *name_, DataType datatype_)
{
	BLI_strncpy(this->name, name_, sizeof(this->name));
	this->datatype = datatype_;
	/* these are created in initExecution */
	this->outputBuffer = 0;
	this->imageInput = 0;
}

OutputOpenExrMultiLayerOperation::OutputOpenExrMultiLayerOperation(
        const RenderData *rd, const bNodeTree *tree, const char *path, char exr_codec, int actview)
{
	this->m_rd = rd;
	this->m_tree = tree;
	
	BLI_strncpy(this->m_path, path, sizeof(this->m_path));
	this->m_exr_codec = exr_codec;
	this->m_actview = actview;
}

void *OutputOpenExrMultiLayerOperation::get_handle(const char* filename)
{
	unsigned int width = this->getWidth();
	unsigned int height = this->getHeight();

	if (width != 0 && height != 0) {

		void *exrhandle;
		SceneRenderView *srv;
		int nr;

		exrhandle = IMB_exr_get_handle_name(filename);
		if (this->m_actview > 0) return exrhandle;

		/* MV are are doing very similar in
		 * render_result.c::render_result_new
		 * it could be an external shared function */

		/* check renderdata for amount of views */
		for (nr = 0, srv= (SceneRenderView *) this->m_rd->views.first; srv; srv = srv->next, nr++) {

			if ((this->m_rd->scemode & R_SINGLE_VIEW) && nr != this->m_rd->actview)
				continue;

			if (srv->viewflag & SCE_VIEW_DISABLE)
				continue;

			IMB_exr_add_view(exrhandle, srv->name);

			for (unsigned int i = 0; i < this->m_layers.size(); ++i) {
				char channelname[EXR_TOT_MAXNAME];
				BLI_strncpy(channelname, this->m_layers[i].name, sizeof(channelname) - 2);
				char *channelname_ext = channelname + strlen(channelname);

				/* create channels */
				switch (this->m_layers[i].datatype) {
					case COM_DT_VALUE:
						strcpy(channelname_ext, ".V");
						IMB_exr_add_channel(exrhandle, 0, channelname, srv->name, 1, width, NULL);
						break;
					case COM_DT_VECTOR:
						strcpy(channelname_ext, ".X");
						IMB_exr_add_channel(exrhandle, 0, channelname, srv->name, 3, 3 * width, NULL);
						strcpy(channelname_ext, ".Y");
						IMB_exr_add_channel(exrhandle, 0, channelname, srv->name, 3, 3 * width, NULL);
						strcpy(channelname_ext, ".Z");
						IMB_exr_add_channel(exrhandle, 0, channelname, srv->name, 3, 3 * width, NULL);
						break;
					case COM_DT_COLOR:
						strcpy(channelname_ext, ".R");
						IMB_exr_add_channel(exrhandle, 0, channelname, srv->name, 4, 4 * width, NULL);
						strcpy(channelname_ext, ".G");
						IMB_exr_add_channel(exrhandle, 0, channelname, srv->name, 4, 4 * width, NULL);
						strcpy(channelname_ext, ".B");
						IMB_exr_add_channel(exrhandle, 0, channelname, srv->name, 4, 4 * width, NULL);
						strcpy(channelname_ext, ".A");
						IMB_exr_add_channel(exrhandle, 0, channelname, srv->name, 4, 4 * width, NULL);
						break;
					default:
						break;
				}
			}
		}

		BLI_make_existing_file(filename);

		/* prepare the file with all the channels */
		if(IMB_exrmultipart_begin_write(exrhandle, filename, width, height, this->m_exr_codec, TRUE) == 0)
		{
			/* TODO, get the error from openexr's exception */
			/* XXX nice way to do report? */
			printf("Error Writing Render Result, see console\n");
			IMB_exr_close(exrhandle);
		}
		else {
			/* the actual writing */
			return exrhandle;
		}
	}
	return NULL;
}

void OutputOpenExrMultiLayerOperation::add_layer(const char *name, DataType datatype)
{
	this->addInputSocket(datatype);
	this->m_layers.push_back(OutputOpenExrLayer(name, datatype));
}

void OutputOpenExrMultiLayerOperation::initExecution()
{
	for (unsigned int i = 0; i < this->m_layers.size(); ++i) {
		this->m_layers[i].imageInput = getInputSocketReader(i);
		this->m_layers[i].outputBuffer = init_buffer(this->getWidth(), this->getHeight(), this->m_layers[i].datatype);
	}
}

void OutputOpenExrMultiLayerOperation::executeRegion(rcti *rect, unsigned int tileNumber)
{
	for (unsigned int i = 0; i < this->m_layers.size(); ++i) {
		write_buffer_rect(rect, this->m_tree, this->m_layers[i].imageInput, this->m_layers[i].outputBuffer, this->getWidth(), this->m_layers[i].datatype);
	}
}

void OutputOpenExrMultiLayerOperation::deinitExecution()
{
	unsigned int width = this->getWidth();
	unsigned int height = this->getHeight();

	if (width != 0 && height != 0) {
		void *exrhandle;
		char view[64];
		Main *bmain = G.main; /* TODO, have this passed along */
		char filename[FILE_MAX];

		BKE_makepicstring_from_type(filename, this->m_path, bmain->name, this->m_rd->cfra, R_IMF_IMTYPE_MULTILAYER,
									(this->m_rd->scemode & R_EXTENSION), true);

		exrhandle = this->get_handle(filename);

		IMB_exr_get_multiView_name(exrhandle, this->m_actview, view);
		IMB_exr_clear_channels(exrhandle);

		for (unsigned int i = 0; i < this->m_layers.size(); ++i) {
			char channelname[EXR_TOT_MAXNAME];
			BLI_strncpy(channelname, this->m_layers[i].name, sizeof(channelname) - 2);
			char *channelname_ext = channelname + strlen(channelname);

			float *buf = this->m_layers[i].outputBuffer;

			/* create channels */
			switch (this->m_layers[i].datatype) {
				case COM_DT_VALUE:
					strcpy(channelname_ext, ".V");
					IMB_exr_add_channel(exrhandle, 0, channelname, view, 1, width, buf);
					break;
				case COM_DT_VECTOR:
					strcpy(channelname_ext, ".X");
					IMB_exr_add_channel(exrhandle, 0, channelname, view, 3, 3 * width, buf);
					strcpy(channelname_ext, ".Y");
					IMB_exr_add_channel(exrhandle, 0, channelname, view, 3, 3 * width, buf + 1);
					strcpy(channelname_ext, ".Z");
					IMB_exr_add_channel(exrhandle, 0, channelname, view, 3, 3 * width, buf + 2);
					break;
				case COM_DT_COLOR:
					strcpy(channelname_ext, ".R");
					IMB_exr_add_channel(exrhandle, 0, channelname, view, 4, 4 * width, buf);
					strcpy(channelname_ext, ".G");
					IMB_exr_add_channel(exrhandle, 0, channelname, view, 4, 4 * width, buf + 1);
					strcpy(channelname_ext, ".B");
					IMB_exr_add_channel(exrhandle, 0, channelname, view, 4, 4 * width, buf + 2);
					strcpy(channelname_ext, ".A");
					IMB_exr_add_channel(exrhandle, 0, channelname, view, 4, 4 * width, buf + 3);
					break;
				default:
					break;
			}
		}

		/* the actual writing */
		IMB_exrmultipart_write_channels(exrhandle, this->m_actview);

		for (unsigned int i = 0; i < this->m_layers.size(); ++i) {
			if (this->m_layers[i].outputBuffer) {
				MEM_freeN(this->m_layers[i].outputBuffer);
				this->m_layers[i].outputBuffer = NULL;
			}
			
			this->m_layers[i].imageInput = NULL;
		}

		/* ready to close the file */
		if (this->m_actview >= IMB_exr_get_multiView_count(exrhandle) - 1)
			IMB_exr_close(exrhandle);
	}
}
