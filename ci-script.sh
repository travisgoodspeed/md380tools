set -e errexit	#otherwise bash continues after error!
make clean
make all # d13.020
make clean
cd applet
make -f Makefile.d02.032
cd ..
make clean
cd applet
make -f Makefile.s13.020
cd ..
cd db
make all
