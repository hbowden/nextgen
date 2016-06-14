#!/bin/sh

wget http://ftp.gnu.org/gnu/automake/automake-1.5.tar.gz && \
tar -xvf automake-1.5.tar.gz && cd automake-1.5 && ./configure &&
make -j2 && sudo make install && echo aclocal --version