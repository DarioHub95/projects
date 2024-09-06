#!/bin/bash

mydir=`dirname $0`
cd $mydir
cwd=`pwd`
name=`basename $cwd`
upper=`echo $name|tr '[:lower:]' '[:upper:]'`

#echo "#ifndef __${upper}_H__"
#echo "#define __${upper}_H__"
echo

for file in `ls */*.c`
do

name=`basename $file .c`
upper=`echo $name|tr '[:lower:]' '[:upper:]'`

echo "/************************************ $upper ************************************/"
sed -n "/#ifndef __${upper}_H__/,/#ifndef __HEADERS__/p" $file|grep -v "#ifndef __HEADERS__"
echo "#endif"

done

#echo "#endif"
