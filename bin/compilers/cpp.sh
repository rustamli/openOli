#!/bin/sh

# from DOMJudge

SRC="$1"
DST="$2"

# -Wall:	Report all warnings
# -O2:		Level 2 optimizations (default for speed)
# -static:	Static link with all libraries

g++ -Wall -O2 -static -o $DST $SRC
exit $?