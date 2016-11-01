#!/bin/bash

DIR=$(dirname $0)

. ${DIR}/convlib.sh

inp()
{
    lnk2sym <../../applet/src/symbols_d13.020 
    echo "MARKER"
    cat flash.r
}

inp | awk -f ${DIR}/conv.awk >flash2.r




