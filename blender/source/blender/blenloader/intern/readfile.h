/*
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
 * blenloader readfile private function prototypes
 */
#ifndef READFILE_H
#define READFILE_H

struct OldNewMap;

typedef struct FileData {
	// linked list of BHeadN's
	ListBase listbase;
	int flags;
	int eof;
	int buffersize;
	int seek;
	int (*read)(struct FileData *filedata, void *buffer, int size);

	// variables needed for reading from memory / stream
	char * buffer;

	// variables needed for reading from file
	int filedes;

	// variables needed for reading from stream
	char headerdone;
	int inbuffer;
	
	// general reading variables
	struct SDNA *filesdna;
	struct SDNA *memsdna;
	char *compflags;
	
	int fileversion;
	
	struct OldNewMap *datamap;
	struct OldNewMap *globmap;
	struct OldNewMap *libmap;
	
	ListBase mainlist;
	
		/* ick ick, used to return
		 * data through streamglue.
		 */
	BlendFileData **bfd_r;
	BlendReadError *error_r;
} FileData;

typedef struct BHeadN {
	struct BHeadN *next, *prev;
	struct BHead bhead;
} BHeadN;

#define FD_FLAGS_SWITCH_ENDIAN             (1<<0)
#define FD_FLAGS_FILE_POINTSIZE_IS_4       (1<<1)
#define FD_FLAGS_POINTSIZE_DIFFERS         (1<<2)
#define FD_FLAGS_FILE_OK                   (1<<3)
#define FD_FLAGS_NOT_MY_BUFFER			   (1<<4)
#define FD_FLAGS_NOT_MY_LIBMAP			   (1<<5)

#define SIZEOFBLENDERHEADER 12

	/***/

void blo_join_main(ListBase *mainlist);
void blo_split_main(ListBase *mainlist);

	BlendFileData*
blo_read_file_internal(
	FileData *fd, 
	BlendReadError *error_r);


	FileData*
blo_openblenderfile(
	char *name);

	FileData*
blo_openblendermemory(
	void *buffer,
	int buffersize);

	void
blo_freefiledata(
	FileData *fd);


	BHead*
blo_firstbhead(
	FileData *fd);

	BHead*
blo_nextbhead(
	FileData *fd, 
	BHead *thisblock);

	BHead*
blo_prevbhead(
	FileData *fd, 
	BHead *thisblock);
	
#endif
