#!/bin/bash

LIB=$(LIBname $0)

. ${LIB}/convlib.sh

LNK=$1
R2IN=$2

inp()
{
    lnk2sym <${LNK} 
    echo "MARKER"
    cat ${R2IN}
}

inp | awk -f ${LIB}/conv.awk 




