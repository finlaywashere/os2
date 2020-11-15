#!/bin/bash
rm -rf iso/
rm -f os.iso fat.img

dd if=/dev/zero of=fat.img bs=1k count=1440
mformat -i fat.img -f 1440 ::
mmd -i fat.img ::/EFI
mmd -i fat.img ::/EFI/BOOT
mcopy -i fat.img main.efi ::/EFI/BOOT/BOOTX64.EFI

mkdir iso
cp fat.img iso
xorriso -as mkisofs -R -f -e fat.img -no-emul-boot -o os.iso iso
