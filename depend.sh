#!/bin/sh
DIR="$1"
eval "LAST=\${$#}"
shift 1
gcc -iquote "$DIR" -MT `echo "$LAST" | sed 's/\.c/\.o/'` -MM -MG "$@" | sed -e 's@^\(.*\)\.o:@\1.d \1.o:@'