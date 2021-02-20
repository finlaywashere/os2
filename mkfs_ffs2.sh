#!/bin/bash

rm -f ffs.iso test.o test.l.o test2.o test2.l.o

nasm -felf64 test.asm -o test.o 
x86_64-elf-ld -T test_linker.ld test.o -o test.l.o

nasm -felf64 test2.asm -o test2.o
x86_64-elf-ld -T test_linker.ld test2.o -o test2.l.o

../FFS2/ffs2_utils.o mkfs ffs.iso 64
../FFS2/ffs2_utils.o copyfile ffs.iso test.l.o test.elf
../FFS2/ffs2_utils.o copyfile ffs.iso test2.l.o test2.elf
