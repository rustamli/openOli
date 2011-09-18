#!/bin/sh

# from DOMJudge

SRC="$1"
DST="$2"

gcc -Wall -O2 -static -o $DST $SRC -lm
exit $?