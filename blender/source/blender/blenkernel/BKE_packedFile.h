/**
 * blenlib/BKE_packedFile.h (mar-2001 nzc)
 *	
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
#ifndef BKE_PACKEDFILE_H
#define BKE_PACKEDFILE_H

struct PackedFile;
struct VFont;
struct bSample;
struct bSound;
struct Image;

struct PackedFile * newPackedFile(char * filename);
struct PackedFile * newPackedFileMemory(void *mem, int memlen);

int seekPackedFile(struct PackedFile * pf, int offset, int whence);
void rewindPackedFile(struct PackedFile * pf);
int readPackedFile(struct PackedFile * pf, void * data, int size);
int countPackedFiles(void);
void freePackedFile(struct PackedFile * pf);
void packAll(void);
int writePackedFile(char * filename, struct PackedFile *pf);
int checkPackedFile(char * filename, struct PackedFile * pf);
char * unpackFile(char * abs_name, char * local_name, struct PackedFile * pf, int how);
int unpackVFont(struct VFont * vfont, int how);
void create_local_name(char *localname, char *prefix, char *filename);
int unpackSample(struct bSample *sample, int how);
int unpackImage(struct Image * ima, int how);
void unpackAll(int how);
	
#endif
