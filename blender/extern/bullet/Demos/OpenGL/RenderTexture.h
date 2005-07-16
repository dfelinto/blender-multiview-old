#ifndef RENDER_TEXTURE_H
#define RENDER_TEXTURE_H

#include "SimdVector3.h"
#include "BMF_FontData.h"

///
///RenderTexture provides a software-render context (setpixel/printf)
///
class RenderTexture
{
	int m_height;
	int m_width;
	unsigned char*	m_buffer;

public:

	RenderTexture(int width,int height);
	~RenderTexture();

	inline void	SetPixel(int x,int y,const SimdVector4& rgba)
	{
		unsigned char* pixel = &m_buffer[ (x+y*m_width) * 4];

		pixel[0] = unsigned char(255*rgba.getX());
		pixel[1] = unsigned char(255*rgba.getY());
		pixel[2] = unsigned char(255*rgba.getZ());
		pixel[3] = unsigned char(255*rgba.getW());
	}

	const unsigned char*	GetBuffer() const { return m_buffer;}
	int	GetWidth() const { return m_width;}
	int	GetHeight() const { return m_height;}
	void Printf(char* str,	BMF_FontData* fontData, int startx = 0,int starty=0);

};

#endif //RENDER_TEXTURE_H