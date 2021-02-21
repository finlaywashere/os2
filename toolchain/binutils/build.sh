#!/bin/bash
binutils=binutils-2.35.90

rm -rf ${binutils}.tar.xz $binutils
wget "ftp://sourceware.org/pub/binutils/snapshots/${binutils}.tar.xz"
tar -xvf ${binutils}.tar.xz

patch -p0 -i binutils.patch
cd ${binutils}/ld
automake
cd ../../
mkdir -p build
cd build
../$binutils/configure --target=x86_64-fos --prefix=/usr/local --with-sysroot=../ --disable-werror
make -j4

# Make install is left up to user because it requires root
echo "Run cd build && make install as root to install binaries to /usr/local"

