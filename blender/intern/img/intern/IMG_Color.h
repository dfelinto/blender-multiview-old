/**
 * $Id$
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

/**

 * $Id$
 * Copyright (C) 2001 NaN Technologies B.V.
 * Declaration of an RGB color with red, green and blue components.
 * Declaration of an RGBA color with red, green, blue and alpha components.
 * Components are stored as floats.
 * @author	Maarten Gribnau
 * @date	March 7, 2001
 */

#ifndef _H_IMG_Color
#define _H_IMG_Color

class IMG_ColorRGBA;


class IMG_ColorRGB {
public:
	/**
	 * Constructs a color with the given values.
	 * @param	r	requested red component of the color
	 * @param	g	requested green component of the color
	 * @param	b	requested blue component of the color
	 */
	IMG_ColorRGB(float r=0, float g=0, float b=0)
		: m_r(r), m_g(g), m_b(b) {}

	/**
	 * Copy constructor.
	 * @param	c	the color to copy.
	 */
	IMG_ColorRGB(const IMG_ColorRGB& c)
		: m_r(c.m_r), m_g(c.m_g), m_b(c.m_b) {}

	/**
	 * Constructs a color without alpha from one with.
	 * @param	c	the color to copy.
	 */
	inline IMG_ColorRGB(const IMG_ColorRGBA& c);

	/** Red component of the color */
	float m_r;
	/** Green component of the color */
	float m_g;
	/** Blue component of the color */
	float m_b;
};


class IMG_ColorRGBA {
public:
	/**
	 * Constructs a color with the given values.
	 * @param	r	requested red component of the color
	 * @param	g	requested green component of the color
	 * @param	b	requested blue component of the color
	 * @param	a	requested alpha component of the color
	 */
	IMG_ColorRGBA(float r=0, float g=0, float b=0, float a=0)
		: m_r(r), m_g(g), m_b(b), m_a(a) {}

	/**
	 * Copy constructor.
	 * @param	c	the color to copy.
	 */
	IMG_ColorRGBA(const IMG_ColorRGBA& c)
		: m_r(c.m_r), m_g(c.m_g), m_b(c.m_b), m_a(c.m_a) {}

	/**
	 * Constructs a color with alpha from one without.
	 * @param	c	the color to copy.
	 */
	IMG_ColorRGBA(const IMG_ColorRGB& c)
		: m_r(c.m_r), m_g(c.m_g), m_b(c.m_b), m_a(0) {}

	/**
	 * Blends the given color with this color.
	 * Uses the alpha of the given color for blending.
	 * The alpha of this color is left untouched.
	 * @param	c	the color to blend
	 */
	inline void	blendColor(const IMG_ColorRGBA& c);

	/** Red component of the color */
	float m_r;
	/** Green component of the color */
	float m_g;
	/** Blue component of the color */
	float m_b;
	/** Alpha component of the color */
	float m_a;
};

inline IMG_ColorRGB::IMG_ColorRGB(const IMG_ColorRGBA& c)
	: m_r(c.m_r), m_g(c.m_g), m_b(c.m_b) {}

inline void	IMG_ColorRGBA::blendColor(const IMG_ColorRGBA& c)
{
	float r1 = 1 - c.m_a; // The reverse of alpha
#if IMG_REVERSED_ALPHA
	m_r = c.m_a * m_r + r1 * c.m_r;
	m_g = c.m_a * m_g + r1 * c.m_g;
	m_b = c.m_a * m_b + r1 * c.m_b;
#else
	m_r = r1 * m_r + c.m_a * c.m_r;
	m_g = r1 * m_g + c.m_a * c.m_g;
	m_b = r1 * m_b + c.m_a * c.m_b;
#endif
}


#endif // _H_IMG_Color