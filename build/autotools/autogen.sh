#!/bin/sh

libtoolize -c -f
aclocal
autoheader
automake -a -c -f
autoconf -f
