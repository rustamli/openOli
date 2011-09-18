#!/bin/sh

# GCJ: Java -> Machine Code

SRC="$1"
DST="$2"

/home/turan/jdk1.6.0_14/bin/javac -g -d $DST $SRC

exit $?
