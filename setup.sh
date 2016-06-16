#!/bin/sh

root=${PWD}

os=`uname`

echo $os

if [ $os = 'FreeBSD' ]; then
    cd $root/deps/capstone-3.0.4 && gmake && \
    cd $root/deps/ck-0.5.1 && ./configure && gmake && \
    cd $root/deps/sqlite && gmake && \
    cd $root/deps/libressl-2.4.1 && ./configure && gmake;
else
    cd $root/deps/capstone-3.0.4 && make && \
    cd $root/deps/ck-0.5.1 && ./configure && make && \
    cd $root/deps/sqlite && make && \
    cd $root/deps/libressl-2.4.1 && ./configure && make;
fi

