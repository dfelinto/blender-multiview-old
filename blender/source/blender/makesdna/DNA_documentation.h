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
 *
 * @mainpage DNA- Makesdna modules 
 *
 * @section about About the DNA module
 *
 * The DNA module holds all type definitions that are serialized in a
 * blender file. There is an executable that scans all files, looking
 * for struct-s to serialize (hence sdna: Struct DNA). From this
 * information, it builds a file with numbers that encode the format,
 * the names of variables, and the plce to look for them.
 *
 * @section issues Known issues with DNA
 *
 * - Function pointers:
 *
 *   Because of historical reasons, some function pointers were
 *   untyped. The parser/dna generator has been modified to explicitly
 *   handle these special cases. Most pointers have been given proper
 *   proto's by now. DNA_space_types.h::Spacefile::returnfuncmay still
 *   be badly defined. The reason for this is that is is called with
 *   different types of arguments. It takes a char* at this moment...
 *
 * - Path to the header files
 *
 *   Also because of historical reasons, there is a path prefix to the
 *   headers that need to be scanned. This is the BASE_HEADER
 *   define. If you change the file-layout for DNA, you will probably
 *   have to change this (Not very flexible, but it is hardly ever
 *   changed. Sorry.).
 *
 * @section dependencies Dependencies
 *
 * DNA has no external dependencies (except for a few system
 * includes).
 *
 **/

/* This file has intentionally no definitions or implementation. */
