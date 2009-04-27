#!/bin/sh
DIR="$1"
shift 1
gcc -iquote "$DIR" -MM -MG "$@" | sed -e 's@^\(.*\)\.o:@\1.d \1.o:@'