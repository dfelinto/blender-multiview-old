/* $Id$ 
/*
/* ***** BEGIN GPL/BL DUAL LICENSE BLOCK *****
/*
/* The contents of this file may be used under the terms of either the GNU
/* General Public License Version 2 or later (the "GPL", see
/* http://www.gnu.org/licenses/gpl.html ), or the Blender License 1.0 or
/* later (the "BL", see http://www.blender.org/BL/ ) which has to be
/* bought from the Blender Foundation to become active, in which case the
/* above mentioned GPL option does not apply.
/*
/* The Original Code is Copyright (C) 2002 by NaN Holding BV.
/* All rights reserved.
/*
/* The Original Code is: all of this file.
/*
/* Contributor(s): none yet.
/*
/* ***** END GPL/BL DUAL LICENSE BLOCK *****
 */


#define KEY_GETPTR(x) (g_ptrtab ? g_ptrtab[x] : 0)

/* these are the defines for the keyed functions:

	 #define key_func<n>  <function name to be behind key>

   This function must be of type "int func(void*)"

   To prevent symbol table dumpers from retrieving certain key
   functions too easily, some of those functions have nonsense names.
*/

#define key_func1 make_beautiful_animation
#define key_func2 key_return_true
#define key_func3 calc_memleak
/* add the corresponding function pointer defines here.
   Example:

	   #define key_func4 my_protected_function_name
	   #define MY_PROTECTED_FUNCTION_PTR  KEY_GETPTR(KEY_FUNC3)

   KEY_GETPTR(KEY_FUNC3) corresponds to the function pointer to function
   key_func3 after the python key code unscrambled the function pointer tables.
   Also add pointer initializations to these functions in 
   license_key.c:init_ftable() if necessary.
*/

#define KEY_WRITE_RUNTIME		KEY_GETPTR(KEY_FUNC1)
#define KEY_RETURN_TRUE			KEY_GETPTR(KEY_FUNC2)
#define KEY_NLA_EVENT			KEY_GETPTR(KEY_FUNC3)

/* PROTOS */
int make_beautiful_animation(void *vp);
int calc_memleak (void* ptr);


