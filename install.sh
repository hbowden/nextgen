#!/bin/sh

root=${PWD}

os=`uname`

echo $os

if [ $os = 'FreeBSD' ]; then
    cd $root/../deps/capstone-3.0.4 && gmake install && \
    cd $root/../deps/ck-0.5.1 && gmake install && \
    cd $root/../deps/libressl-2.4.1 && gmake install;
else
    cd $root/../deps/capstone-3.0.4 && make install && \
    cd $root/../deps/ck-0.5.1 && make install && \
    cd $root/../deps/libressl-2.4.1 && make install;
fi