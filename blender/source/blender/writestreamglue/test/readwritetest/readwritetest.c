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

/* Test for the streamglue read/write components
 *
 * The streamglue functions connect dataprocessors. 
 *
 * Tested functions
 *
 * - streamGlueWrite (from BLO_streamglue.h)
 * - streamGlueRead (from BLO_streamglue.h)
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>

#include "BLO_writeStreamGlue.h"

#ifndef _WIN32
#define O_BINARY 0
#endif

struct streamGlueControlStruct *Global_streamGlueControl;

/* Some stubs here, because otherwise we have to link all of blender... */

//FILE* mywfile     = NULL;
int mywfile     = 0;

    void *
BLO_readstreamfile_begin(
	void)
{
	fprintf(stderr, "|--> BLO_readstreamfile_begin: local loopback\n");
	return NULL;
}

	int
BLO_readstreamfile_process(
	void *filedataVoidPtr,
	unsigned char *data,
	unsigned int dataIn)
{
	fprintf(stderr, "|--> BLO_readstreamfile_process: local loopback\n");
	return 0;
}


	int
BLO_readstreamfile_end(
	void *filedataVoidPtr)
{
	fprintf(stderr, "|--> BLO_readstreamfile_end: local loopback\n");
	return 0;
}

int main (int argc, char *argv[])
{
	int verbose       = 0;
	int error_status  = 0;
	int retval        = 0;
	int i             = 0;
	
	int datachunksize = 12345;
	char* datachunk   = NULL;
	char* dataptr     = NULL;
	struct writeStreamGlueStruct *sgp = NULL;

   	switch (argc) {
	case 2:		
		verbose = atoi(argv[1]);
		if (verbose < 0) verbose = 0;
		break;		
	case 1:
	default:
		verbose = 0;
	}

	/* ----------------------------------------------------------------- */

	if (verbose > 0) {
		fprintf(stderr,"\n*** Streamglue read/write test\n");
	}

	Global_streamGlueControl = streamGlueControlConstructor();

//	mywfile = fopen("readwritetestdump","wb");
	mywfile = open("readwritetestdump",O_BINARY+O_WRONLY+O_CREAT+O_TRUNC, 0666);

	error_status = (mywfile == NULL);
//	error_status |= fprintf(mywfile,"|\n|-- Opened file for testing: %d\n|\n", mywfile);

	
	if (verbose > 1) {
		fprintf(stderr,"|\n|-- Opened file for testing: %d with \n|\n", mywfile);
	}

/*    	streamGlueControlAppendAction(Global_streamGlueControl, DUMPFROMMEMORY);  */
/*  		streamGlueControlAppendAction(Global_streamGlueControl, DEFLATE); */
/*  		streamGlueControlAppendAction(Global_streamGlueControl, ENCRYPT); */
/*  	streamGlueControlAppendAction(Global_streamGlueControl, SIGN); */
	streamGlueControlAppendAction(Global_streamGlueControl, WRITEBLENFILE);
	
	if (verbose >1) {
		fprintf(stderr,"|\n|-- Created and initialized streamGlueControl thingy \n");
		fflush(stderr);
	}
	
	/* 2: the size */
	datachunksize = 12345;

	/* 1: a data chunk. We fill it with some numbers */
	datachunk = (char*) malloc(datachunksize);

	/* an ascending-ish thingy */
	dataptr = datachunk;
	for (i = 0 ;
		 i < datachunksize;
		 i++, dataptr++) {
		*dataptr = (i % 0xFF);
	}

	if (verbose >1) {
		fprintf(stderr,"|\n|-- Calling streamGlueWrite\n");
		fflush(stderr);
	}
	
	retval = 
		writeStreamGlue(
			Global_streamGlueControl,  // general controller
			&sgp,                      // ie. construct this for me
			datachunk,                 // raw data
			datachunksize,             // data size
			1);                        // i.e. finalize this write

	if (verbose >1) {
		fprintf(stderr,"|\n|-- streamGlueWrite returned with %d\n", retval);
	}

	/* ----------------------------------------------------------------- */	

	if (close(mywfile)) {
		error_status = 1;
		if (verbose > 1) {
			fprintf(stderr,"|\n|-- file close failed.\n");
		}
	}
	
	if (verbose > 0) {
		fprintf(stderr,"|\n*** Finished test\n\n");
	}
	exit(error_status);
}
	

/* eof */
