#!/bin/sh

root=${PWD}

os=`uname`
ck='ck-0.5.2'

echo $os

if [ $os = 'FreeBSD' ]; then
    cd $root/../deps/capstone-3.0.4 && gmake install && \
    cd $root/../deps/$ck && gmake install && \
    cd $root/../deps/libressl-2.4.2 && gmake install;
else
    cd $root/../deps/capstone-3.0.4 && make install && \
    cd $root/../deps/$ck && make install && \
    cd $root/../deps/libressl-2.4.2 && make install;
fi
