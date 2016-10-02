#!/bin/bash
set -e errexit	#otherwise bash continues after error!
make clean
make all # d13.020
make clean
#cd applet
#make -f Makefile.d02.032
#cd ..
make image_D02
make clean
#cd applet
#make -f Makefile.s13.020
#cd ..
make image_S13
cd db
make all
