#!/bin/bash
set -e

automake=automake-1.15.1
prefix=/usr/local
makeflags=-j16

rm -rf build/ $automake ${automake}.tar.gz
wget https://ftp.gnu.org/gnu/automake/${automake}.tar.gz
tar -xvf ${automake}.tar.gz

mkdir -p build/
cd build/
../${automake}/configure --prefix=$prefix
make $makeflags
sudo make install
