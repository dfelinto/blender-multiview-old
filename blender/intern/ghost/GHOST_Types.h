/**
 * $Id$
 * ***** BEGIN GPL/BL DUAL LICENSE BLOCK *****
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version. The Blender
 * Foundation also sells licenses for use in proprietary software under
 * the Blender License.  See http://www.blender.org/BL/ for information
 * about this.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * The Original Code is Copyright (C) 2001-2002 by NaN Holding BV.
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
 * @author	Maarten Gribnau
 * @date	May 31, 2001
 */

#ifndef _GHOST_TYPES_H_
#define _GHOST_TYPES_H_

typedef	char				GHOST_TInt8;
typedef	unsigned char		GHOST_TUns8;
typedef short				GHOST_TInt16;
typedef unsigned short		GHOST_TUns16;
typedef	int					GHOST_TInt32;
typedef	unsigned int		GHOST_TUns32;

#ifdef WIN32
typedef __int64				GHOST_TInt64;
typedef unsigned __int64	GHOST_TUns64;
#else
typedef long long			GHOST_TInt64;
typedef unsigned long long	GHOST_TUns64;
#endif

typedef void*				GHOST_TUserDataPtr;

typedef enum
{
	GHOST_kFailure = 0,
	GHOST_kSuccess
} GHOST_TSuccess;


typedef enum {
	GHOST_kNotVisible = 0,
	GHOST_kPartiallyVisible,
	GHOST_kFullyVisible
} GHOST_TVisibility;


typedef enum {
	GHOST_kFireTimeNever = 0xFFFFFFFF
} GHOST_TFireTimeConstant;

typedef enum {
    GHOST_kModifierKeyLeftShift = 0,
    GHOST_kModifierKeyRightShift,
    GHOST_kModifierKeyLeftAlt,
    GHOST_kModifierKeyRightAlt,
    GHOST_kModifierKeyLeftControl,
    GHOST_kModifierKeyRightControl,
    GHOST_kModifierKeyCommand,		// APPLE only
    GHOST_kModifierKeyNumMasks
} GHOST_TModifierKeyMask;


typedef enum {
	GHOST_kWindowStateNormal = 0,
	GHOST_kWindowStateMaximized,
	GHOST_kWindowStateMinimized,
	GHOST_kWindowStateFullScreen
} GHOST_TWindowState;


typedef enum {
	GHOST_kWindowOrderTop = 0,
	GHOST_kWindowOrderBottom
} GHOST_TWindowOrder;


typedef enum {
	GHOST_kDrawingContextTypeNone = 0,
	GHOST_kDrawingContextTypeOpenGL
} GHOST_TDrawingContextType;


typedef enum {
	GHOST_kButtonMaskLeft = 0,
	GHOST_kButtonMaskMiddle,
	GHOST_kButtonMaskRight,
	GHOST_kButtonNumMasks
} GHOST_TButtonMask;


typedef enum {
	GHOST_kEventUnknown = 0,

	GHOST_kEventCursorMove,
	GHOST_kEventButtonDown,
	GHOST_kEventButtonUp,

	GHOST_kEventKeyDown,
	GHOST_kEventKeyUp,
//	GHOST_kEventKeyAuto,

	GHOST_kEventQuit,

	GHOST_kEventWindowClose,
	GHOST_kEventWindowActivate,
	GHOST_kEventWindowDeactivate,
	GHOST_kEventWindowUpdate,
	GHOST_kEventWindowSize,

	GHOST_kNumEventTypes
} GHOST_TEventType;


typedef enum {
	GHOST_kStandardCursorFirstCursor = 0,
	GHOST_kStandardCursorDefault = 0,
	GHOST_kStandardCursorRightArrow,
	GHOST_kStandardCursorLeftArrow,
	GHOST_kStandardCursorInfo, 
	GHOST_kStandardCursorDestroy,
	GHOST_kStandardCursorHelp,    
	GHOST_kStandardCursorCycle,
	GHOST_kStandardCursorSpray,
	GHOST_kStandardCursorWait,
	GHOST_kStandardCursorText,
	GHOST_kStandardCursorCrosshair,
	GHOST_kStandardCursorUpDown,
	GHOST_kStandardCursorLeftRight,
	GHOST_kStandardCursorTopSide,
	GHOST_kStandardCursorBottomSide,
	GHOST_kStandardCursorLeftSide,
	GHOST_kStandardCursorRightSide,
	GHOST_kStandardCursorTopLeftCorner,
	GHOST_kStandardCursorTopRightCorner,
	GHOST_kStandardCursorBottomRightCorner,
	GHOST_kStandardCursorBottomLeftCorner,
	GHOST_kStandardCursorCustom, 
	GHOST_kStandardCursorNumCursors
} GHOST_TStandardCursor;


typedef enum {
	GHOST_kKeyUnknown = -1,
	GHOST_kKeyBackSpace,
	GHOST_kKeyTab,
	GHOST_kKeyLinefeed,
	GHOST_kKeyClear,
	GHOST_kKeyEnter  = 0x0D,
	
	GHOST_kKeyEsc    = 0x1B,
	GHOST_kKeySpace  = ' ',
	GHOST_kKeyQuote  = 0x27,
	GHOST_kKeyComma  = ',',
	GHOST_kKeyMinus  = '-',
	GHOST_kKeyPeriod = '.',
	GHOST_kKeySlash  = '/',

	// Number keys
	GHOST_kKey0 = '0',
	GHOST_kKey1,
	GHOST_kKey2,
	GHOST_kKey3,
	GHOST_kKey4,
	GHOST_kKey5,
	GHOST_kKey6,
	GHOST_kKey7,
	GHOST_kKey8,
	GHOST_kKey9,

	GHOST_kKeySemicolon = ';',
	GHOST_kKeyEqual     = '=',

	// Character keys
	GHOST_kKeyA = 'A',
	GHOST_kKeyB,
	GHOST_kKeyC,
	GHOST_kKeyD,
	GHOST_kKeyE,
	GHOST_kKeyF,
	GHOST_kKeyG,
	GHOST_kKeyH,
	GHOST_kKeyI,
	GHOST_kKeyJ,
	GHOST_kKeyK,
	GHOST_kKeyL,
	GHOST_kKeyM,
	GHOST_kKeyN,
	GHOST_kKeyO,
	GHOST_kKeyP,
	GHOST_kKeyQ,
	GHOST_kKeyR,
	GHOST_kKeyS,
	GHOST_kKeyT,
	GHOST_kKeyU,
	GHOST_kKeyV,
	GHOST_kKeyW,
	GHOST_kKeyX,
	GHOST_kKeyY,
	GHOST_kKeyZ,

	GHOST_kKeyLeftBracket  = '[',
	GHOST_kKeyRightBracket = ']',
	GHOST_kKeyBackslash    = 0x5C,
	GHOST_kKeyAccentGrave  = '`',

	
	GHOST_kKeyLeftShift = 0x100,
	GHOST_kKeyRightShift,
	GHOST_kKeyLeftControl,
	GHOST_kKeyRightControl,
	GHOST_kKeyLeftAlt,
	GHOST_kKeyRightAlt,
    GHOST_kKeyCommand,				// APPLE only!

	GHOST_kKeyCapsLock,
	GHOST_kKeyNumLock,
	GHOST_kKeyScrollLock,

	GHOST_kKeyLeftArrow,
	GHOST_kKeyRightArrow,
	GHOST_kKeyUpArrow,
	GHOST_kKeyDownArrow,

	GHOST_kKeyPrintScreen,
	GHOST_kKeyPause,

	GHOST_kKeyInsert,
	GHOST_kKeyDelete,
	GHOST_kKeyHome,
	GHOST_kKeyEnd,
	GHOST_kKeyUpPage,
	GHOST_kKeyDownPage,

	// Numpad keys
	GHOST_kKeyNumpad0,
	GHOST_kKeyNumpad1,
	GHOST_kKeyNumpad2,
	GHOST_kKeyNumpad3,
	GHOST_kKeyNumpad4,
	GHOST_kKeyNumpad5,
	GHOST_kKeyNumpad6,
	GHOST_kKeyNumpad7,
	GHOST_kKeyNumpad8,
	GHOST_kKeyNumpad9,
	GHOST_kKeyNumpadPeriod,
	GHOST_kKeyNumpadEnter,
	GHOST_kKeyNumpadPlus,
	GHOST_kKeyNumpadMinus,
	GHOST_kKeyNumpadAsterisk,
	GHOST_kKeyNumpadSlash,

	// Function keys
	GHOST_kKeyF1,
	GHOST_kKeyF2,
	GHOST_kKeyF3,
	GHOST_kKeyF4,
	GHOST_kKeyF5,
	GHOST_kKeyF6,
	GHOST_kKeyF7,
	GHOST_kKeyF8,
	GHOST_kKeyF9,
	GHOST_kKeyF10,
	GHOST_kKeyF11,
	GHOST_kKeyF12,
	GHOST_kKeyF13,
	GHOST_kKeyF14,
	GHOST_kKeyF15,
	GHOST_kKeyF16,
	GHOST_kKeyF17,
	GHOST_kKeyF18,
	GHOST_kKeyF19,
	GHOST_kKeyF20,
	GHOST_kKeyF21,
	GHOST_kKeyF22,
	GHOST_kKeyF23,
	GHOST_kKeyF24
} GHOST_TKey;


typedef void* GHOST_TEventDataPtr;

typedef struct {
	/** The x-coordinate of the cursor position. */
	GHOST_TInt32 x;
	/** The y-coordinate of the cursor position. */
	GHOST_TInt32 y;
} GHOST_TEventCursorData;

typedef struct {
	/** The mask of the mouse button. */
	GHOST_TButtonMask button;
} GHOST_TEventButtonData;

typedef struct {
	/** The key code. */
	GHOST_TKey		key;
	/** The ascii code for the key event ('\0' if none). */
	char			ascii;
} GHOST_TEventKeyData;

typedef struct {
	/** Number of pixels on a line. */
	GHOST_TUns32	xPixels;
	/** Number of lines. */
	GHOST_TUns32	yPixels;
	/** Numberof bits per pixel. */
	GHOST_TUns32	bpp;
	/** Refresh rate (in Hertz). */
	GHOST_TUns32	frequency;
} GHOST_DisplaySetting;


/**
 * A timer task callback routine.
 * @param task The timer task object.
 * @param time The current time.
 */
#ifdef __cplusplus
class GHOST_ITimerTask;
typedef void (*GHOST_TimerProcPtr)(GHOST_ITimerTask* task, GHOST_TUns64 time);
#else
struct GHOST_TimerTaskHandle__;
typedef void (*GHOST_TimerProcPtr)(struct GHOST_TimerTaskHandle__* task, GHOST_TUns64 time);
#endif


#endif // _GHOST_TYPES_H_
