set -e errexit	#otherwise bash continues after error!
make all
make clean
cd applet
make -f Makefile.d02.032
cd ..
cd db
make all