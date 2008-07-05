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
 * Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * The Original Code is Copyright (C) 2001-2002 by NaN Holding BV.
 * All rights reserved.
 *
 * The Original Code is: all of this file.
 *
 * Contributor(s): none yet.
 *
 * ***** END GPL LICENSE BLOCK *****
 * Specific implementations for linking the browser, plugin
 * and gl together.
 */

#ifndef PLB_SCRIPT_BINDINGS_H
#define PLB_SCRIPT_BINDINGS_H

#ifdef __cplusplus
extern "C" {
#endif
	
	
	/** Hook for Javascript blenderURL function */
	void
	PLB_native_blenderURL_func(void* inst,
				   const char* url);
	
	/** Hook for Javascript SendMessage function */
	void
	PLB_native_SendMessage_func(
		void* inst,
		const char* to,
		const char* from,
		const char* subject,
		const char* body
		);
	
#ifdef __cplusplus
}
#endif

# endif

