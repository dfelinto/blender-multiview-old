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
#ifndef _H_IMG_BrushRGBA32
#define _H_IMG_BrushRGBA32

#include "IMG_PixmapRGBA32.h"


/**
 * A brush used for drawing in pixmaps.
 * The brush is a pixmap as well.
 * Drawing can be implemented by blending the brush into the pixmap.
 * 
 * @author	Maarten Gribnau
 * @date	March 7, 2001
 * @todo	Check the algorithm
 */

class IMG_BrushRGBA32 : public IMG_PixmapRGBA32 {
public:
	/**
	 * Constructs a brush image with the dimensions and color given.
	 * @param	w	width of the brush image
	 * @param	h	height of the brush image
	 * @param	c	color of the brush
	 * @param	a	transparency in the center of the brush
	 */
	IMG_BrushRGBA32(TUns32 w, TUns32 h, const IMG_ColorRGB& c, float a = 0.8f);

	/**
	 * Sets the color of the brush.
	 * @param	c	color of the brush
	 */
	virtual	void setColor(const IMG_ColorRGB& c);

	/**
	 * Returns the color of the brush.
	 * @param	c	color of the brush
	 */
	inline virtual void getColor(IMG_ColorRGB& c) const;

	/**
	 * Sets the transparency of the brush.
	 * @param	a	transparency of the brush
	 */
	virtual	void setTransparency(float a);

	/**
	 * Returns the transparency of the brush.
	 * @param	a	transparency of the brush
	 */
	inline virtual float getTransparency() const;

	/**
	 * Sets the shape of the brush from two radii (inner and outer).
	 * This creates a brush with alpha==0 inside the inner radius
	 * and gradually fades to alpha==1 at the outer radius.
	 * Inner radius should be smaller or equal to the outer radius.
	 * If not, the outer radius is made equal to the inner radius.
	 * If the either radius is larger than the bounds of the brush,
	 * they are clipped.
	 * @param	rI	inner radius
	 * @param	rO	outer radius
	 */
	virtual	void setRadii(TUns32 rI, TUns32 rO);

	/**
	 * Returns the radii that determine the shape of the brush.
	 * @param	rI	inner radius
	 * @param	rO	outer radius
	 */
	inline virtual void setRadii(TUns32& rI, TUns32& rO) const;

protected:
	/**
	 * Updates the image of this brush.
	 * The image is created using the current color, transparency and shape radii.
	 */
	virtual void	updateImage();

	IMG_ColorRGB m_color;	/** The color of the brush */
	float m_alpha;			/** The transparency of the brush */
	TUns32 m_ri;			/** The inner radius of the brush */
	TUns32 m_ro;			/** The outer radius of the brush */
};


inline void IMG_BrushRGBA32::setRadii(TUns32& rI, TUns32& rO) const
{
	rI = m_ri;
	rO = m_ro;
}

inline float IMG_BrushRGBA32::getTransparency() const
{
	return m_alpha;
}

inline void IMG_BrushRGBA32::getColor(IMG_ColorRGB& c) const
{
	c = m_color;
}


#endif // _H_IMG_BrushRGBA32
