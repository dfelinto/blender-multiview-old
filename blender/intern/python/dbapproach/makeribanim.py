#! /usr/bin/env python

#######################
# (c) Jan Walter 2000 #
#######################

# CVS
# $Author$
# $Date$
# $RCSfile$
# $Revision$

import sys
import posix
import string

def usage():
    print 'usage: python makeribanim.py "staframe" "endframe"'

if __name__ == "__main__":
    if len(sys.argv) != 3:
        usage()
    else:
        staframe = string.atoi(sys.argv[1])
        endframe = string.atoi(sys.argv[2])
        for i in xrange(staframe, endframe + 1):
            filename = "test.%04d.rib" % i
            execString = "rendrib %s" % filename
            print
            print "#" * 79
            print execString
            posix.system(execString)
