#!/bin/bash
set -e

bash=bash-2.0
target=x86_64-elf-fos
prefix=/

rm -rf ${bash}.tar.gz $bash build/
wget "https://ftp.wayne.edu/gnu/bash/${bash}.tar.gz"
tar -xvf ${bash}.tar.gz

mkdir -p build
cd build
../$bash/configure --prefix=$prefix --disable-werror --target=$target
make $makeflags

