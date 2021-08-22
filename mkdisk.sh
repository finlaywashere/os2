#!/bin/bash

parted --script disk.iso \
mkpart primary fat32 65KiB, 40MiB \
set 1 boot on

mkfs.fat -F 32 --offset=130 disk.iso
