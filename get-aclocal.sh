#!/bin/sh

wget http://ftp.gnu.org/gnu/automake/automake-1.15.tar.gz && \
tar -xvf automake-1.15.tar.gz && cd automake-1.15 && ./configure &&
make -j2 && sudo make install && aclocal --version