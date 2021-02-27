#!/bin/bash
set -e

autoconf=autoconf-2.69
prefix=/usr/local
makeflags=-j16

rm -rf build/ $autoconf ${autoconf}.tar.gz
wget https://ftp.gnu.org/gnu/autoconf/${autoconf}.tar.gz
tar -xvf ${autoconf}.tar.gz

mkdir -p build/
cd build/
../${autoconf}/configure --prefix=$prefix
make $makeflags
sudo make install
