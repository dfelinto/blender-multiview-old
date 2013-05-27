/*
 * Copyright 2013, Blender Foundation.
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
 *		Dalai Felinto
 */

#ifndef _COM_OutputFileMultiViewOperation_h
#define _COM_OutputFileMultiViewOperation_h
#include "COM_NodeOperation.h"

#include "BLI_rect.h"
#include "BLI_path_util.h"

#include "DNA_color_types.h"

#include "intern/openexr/openexr_multi.h"

/* Writes the image to a single-layer file. */
class OutputSingleLayerMultiViewOperation : public OutputSingleLayerOperation {
private:
	const RenderData *m_rd;
	const bNodeTree *m_tree;
	
	ImageFormatData *m_format;
	char m_path[FILE_MAX];
	
	float *m_outputBuffer;
	DataType m_datatype;
	SocketReader *m_imageInput;

	const ColorManagedViewSettings *m_viewSettings;
	const ColorManagedDisplaySettings *m_displaySettings;

	int m_actview;
public:
	OutputSingleLayerMultiViewOperation(const RenderData *rd, const bNodeTree *tree, DataType datatype, ImageFormatData *format, const char *path,
	                           const ColorManagedViewSettings *viewSettings, const ColorManagedDisplaySettings *displaySettings);
	
	void executeRegion(rcti *rect, unsigned int tileNumber);
	bool isOutputOperation(bool rendering) const { return true; }
	void initExecution();
	void deinitExecution();
	const CompositorPriority getRenderPriority() const { return COM_PRIORITY_LOW; }

	bool isFileOutputOperation() { return true; }
};

/* Writes inputs into OpenEXR multilayer channels. */
class OutputOpenExrMultiLayerMultiViewOperation : public OutputOpenExrMultiLayerOperation {
private:
	typedef std::vector<OutputOpenExrLayer> LayerList;
	
	const RenderData *m_rd;
	const bNodeTree *m_tree;
	
	char m_path[FILE_MAX];
	char m_exr_codec;
	LayerList m_layers;

	int m_actview;

public:
	OutputOpenExrMultiLayerMultiViewOperation(const RenderData *rd, const bNodeTree *tree, const char *path, char exr_codec, int actview);

	void *get_handle(const char *filename);
	void add_layer(const char *name, DataType datatype);
	
	void executeRegion(rcti *rect, unsigned int tileNumber);
	bool isOutputOperation(bool rendering) const { return true; }
	void initExecution();
	void deinitExecution();
	const CompositorPriority getRenderPriority() const { return COM_PRIORITY_LOW; }

	bool isFileOutputOperation() { return true; }
};

#endif
