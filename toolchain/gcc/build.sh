#!/bin/bash
gcc=gcc-10.2.0
makeflags=-j16

rm -rf ${gcc}.tar.xz $gcc build/
wget "https://bigsearcher.com/mirrors/gcc/releases/${gcc}/${gcc}.tar.xz"
tar -xvf ${gcc}.tar.xz

patch -p0 -i gcc.patch
sysroot=${pwd}
cd $gcc/libstdc++-v3/
/usr/local/bin/autoconf
cd ../../
mkdir -p build
cd build
../$gcc/configure --target=x86_64-elf-fos --prefix=/usr/local --with-sysroot=$sysroot --with-native-system-header-dir=/usr/include -disable-nls --disable-libssp --disable-werror --enable-languages=c,c++
make all-gcc all-target-libgcc $makeflags
sudo make install-gcc install-target-libgcc
