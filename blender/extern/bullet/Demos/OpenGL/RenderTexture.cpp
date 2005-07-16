#include "RenderTexture.h"
#include <memory.h>
#include "BMF_FontData.h"

RenderTexture::RenderTexture(int width,int height)
:m_width(width),m_height(height)
{
	m_buffer = new unsigned char[m_width*m_height*4];
	
	//clear screen
	memset(m_buffer,0,m_width*m_height*4);

	//clear screen version 2
	for (int x=0;x<m_width;x++)
	{
		for (int y=0;y<m_height;y++)
		{
			SetPixel(x,y,SimdVector4(float(x),float(y),0.f,1.f));
		}

	}

}

void RenderTexture::Printf(char* str,	BMF_FontData* fontData, int startx,int starty)
{
	unsigned char c;
	int rasterposx = startx;
	int rasterposy = starty;
	while (c = (unsigned char) *str++) {
		BMF_CharData & cd = fontData->chars[c];
		
		if (cd.data_offset!=-1) {
			unsigned char* bitmap = &fontData->bitmap_data[cd.data_offset];
			for (int y=0;y<cd.height;y++)
			{
				int bit = 128;
				for (int x=0;x<cd.width;x++)
				{
					char packedColor = bitmap[y];
					float colorf = packedColor & bit ? 1.f : 0.f;
					SimdVector4 rgba(colorf,colorf,colorf,1.f);
					SetPixel(rasterposx+x,rasterposy+8-y-1,rgba);
					bit >>=1;
				}
			}
		}
		rasterposx+= cd.advance;
	}
}

RenderTexture::~RenderTexture()
{
	delete [] m_buffer;
}



