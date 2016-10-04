#!/bin/sh

root=${PWD}

os=`uname`
ck='ck-0.5.2'

echo $os

if [ $os = 'FreeBSD' ]; then
    cd $root/deps/capstone-3.0.4 && gmake && \
    cd $root/deps/$ck && ./configure && gmake && \
    cd $root/deps/sqlite && gmake && \
    cd $root/deps/libressl-2.5.0 && ./configure && gmake;
else
    cd $root/deps/capstone-3.0.4 && make && \
    cd $root/deps/$ck && ./configure && make && \
    cd $root/deps/sqlite && make && \
    cd $root/deps/libressl-2.5.0 && ./configure && make;
fi
