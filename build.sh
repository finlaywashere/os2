#!/bin/bash
gcc -I../gnu-efi/inc -I../gnu-efi/inc/x86_64 -fpic -ffreestanding -fno-stack-protector -fno-stack-check -fshort-wchar -mno-red-zone -maccumulate-outgoing-args -c src/main.c -o src/main.o
ld -shared -Bsymbolic -L../gnu-efi/x86_64/gnuefi -L../gnu-efi/x86_64/lib -T../gnu-efi/gnuefi/elf_x86_64_efi.lds ../gnu-efi/x86_64/gnuefi/crt0-efi-x86_64.o src/main.o -o main.so -lgnuefi -lefi
objcopy -j .text -j .sdata -j .data -j .dynamic -j .dynsym  -j .rel -j .rela -j .rel.* -j .rela.* -j .reloc --target efi-app-x86_64 --subsystem=10 main.so main.efi

