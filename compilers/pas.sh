#!/bin/sh

# from DOMJudge

SRC="$1"
DST="$2"

# -viwn:	Verbose warnings, notes and informational messages
# -02:		Level 2 optimizations (default for speed)
# -Sg:		Support label and goto commands (for those who need it ;-)
# -XS:		Static link with all libraries
fpc -viwn -O2 -Sg -XS -o$DST $SRC
exitcode=$?

# clean created object files:
rm -f $DST.o

exit $exitcode