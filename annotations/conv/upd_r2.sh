#!/bin/bash

LIB=$(dirname $0)

. ${LIB}/convlib.sh

LNK=$1
R2IN=$2

inp()
{
    lnk2sym-unthumb <${LNK} | sort
    echo "MARKER"
    cat ${R2IN}
}

inp | awk -f ${LIB}/upd_r2.awk 




