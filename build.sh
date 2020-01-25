#!/bin/sh

gcc -g     -o chelp.o -c chelp.c -Wno-pointer-to-int-cast
gcc -g -O2 -o chelp      chelp.o
