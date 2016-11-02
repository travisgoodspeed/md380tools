#!/bin/bash

LIB=$(dirname $0)

. ${LIB}/convlib.sh

RA=$1
R2IN=$2

ra2sym <${RA} | sym2lnk | sort 



