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
 * FreeBSD 3.4 does not yet have pthread_cancel (3.5 and above do)
 */
#ifdef __FreeBSD__

#include <osreldate.h>

#if (__FreeBSD_version < 350000)
#include <pthread.h>

#define FD_READ             0x1
#define _FD_LOCK(_fd,_type,_ts)         _thread_fd_lock(_fd, _type, _ts)
#define _FD_UNLOCK(_fd,_type)		_thread_fd_unlock(_fd, _type)

int pthread_cancel(pthread_t pthread) {
    pthread_exit(NULL);
    return 0;
}

long fpathconf(int fd, int name)
{
    long            ret;

    if ((ret = _FD_LOCK(fd, FD_READ, NULL)) == 0) {
	ret = _thread_sys_fpathconf(fd, name);
	_FD_UNLOCK(fd, FD_READ);
    }
    return ret;
}

#endif

int pthread_atfork(void *a, void *b, void *c) {
    return 0;
}

#endif
