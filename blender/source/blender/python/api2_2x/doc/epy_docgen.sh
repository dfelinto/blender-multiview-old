#!/bin/bash
# epy_docgen.sh
# generates blender python doc using epydoc
# requires epydoc in your PATH.
# run from the doc directory containing the .py files
# usage: ./epy_docgen.sh

# force posix locale so [A-Z]*.py glob uses ASCII ordering
LC_COLLATE=POSIX

EPY_VER=$( epydoc --version | sed -e "s/^.* version //" )
case $EPY_VER in
    3*)
        EPY_PARAMS="--top API_intro --name \"Blender\""
        ;;

    [12]*)
        EPY_PARAMS="-t API_intro.py -n \"Blender\""
        ;;

    *)
        echo ERROR: Unknown epydoc version
        exit 1
        ;;

esac

epydoc -o BPY_API --url "http://www.blender.org" -n "Blender" \
 --no-private --no-frames $EPY_PARAMS [A-Z]*.py
