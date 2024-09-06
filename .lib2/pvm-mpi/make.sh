#!/bin/bash

mydir=`dirname $0|xargs realpath`
cd $mydir

CC=mpiCC
CC_OPTS="-O3 -D__PVM_LIB__ -D__USE_MPI__"
LIB2_DIR=$HOME/.lib2/

./head.sh > $LIB2_DIR/pvm.h
$CC $CC_OPTS -I$LIB2_DIR -c */*.c
rm -f $LIB2_DIR/libpvm.a
ar rs $LIB2_DIR/libpvm.a *.o
rm -f *.o
