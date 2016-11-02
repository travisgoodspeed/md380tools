#!/bin/bash

LIB=$(dirname $0)

. ${LIB}/convlib.sh

ra2sym | sym_strip | sort 



