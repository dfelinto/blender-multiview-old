#!/bin/sh

find ~/develop/lib/darwin*/ -name '*\.a' -print -exec ranlib {} \;
