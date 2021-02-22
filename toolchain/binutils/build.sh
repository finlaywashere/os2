#!/bin/bash
binutils=binutils-2.35.90
prefix=/usr/local/
sysroot=$(realpath ../../sysroot)
makeflags=-j16

rm -rf ${binutils}.tar.xz $binutils build/
wget "ftp://sourceware.org/pub/binutils/snapshots/${binutils}.tar.xz"
tar -xvf ${binutils}.tar.xz

patch -p0 -i binutils.patch
cd ${binutils}/ld
automake
cd ../../
mkdir -p build
cd build
../$binutils/configure --target=x86_64-elf-fos --prefix=$prefix --with-sysroot=$sysroot --disable-werror
make $makeflags

sudo make install
