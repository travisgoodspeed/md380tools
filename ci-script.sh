#!/bin/bash
set -e errexit	#otherwise bash continues after error!
#make -C applet ci
cd firmware ; ./test.sh
