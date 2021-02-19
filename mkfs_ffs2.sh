#!/bin/bash

rm -f ffs.iso test.o test.l.o

nasm -felf64 test.asm -o test.o 
x86_64-elf-ld -T test_linker.ld test.o -o test.l.o

../FFS2/ffs2_utils.o mkfs ffs.iso 64
../FFS2/ffs2_utils.o copyfile ffs.iso test.l.o test.elf
