#!/bin/sh

g++ -o ubit_test -g -O2 -Wall -fno-strict-aliasing -ffriend-injection -I/usr/include/freetype2 \
 -L/usr/X11R6/lib ubit_test.cpp -lubit -L/home/dan/dev/deliveries/ubit-99.99.99/lib \
 -L/usr/X11R6/lib -lXpm -L/usr/X11R6/lib  -lSM -lICE -L/usr/X11R6/lib -lXmu -lX11 -lfreetype -lm -lnsl -lpthread


