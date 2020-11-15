#!/bin/bash
qemu-system-x86_64 -L /usr/share/edk2-ovmf/x64/ -bios OVMF.fd -cdrom os.iso
