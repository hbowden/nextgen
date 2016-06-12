#!/bin/sh

root=${PWD}

cd $root/deps/capstone-3.0.4 && make && \
cd $root/deps/ck-0.5.1 && ./configure && make && \
cd $root/deps/sqlite && make && \
cd $root/deps/libressl-2.4.0 && ./configure && make;