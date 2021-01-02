#!/bin/bash

dd if=/dev/zero of=ffs.iso bs=1024 count=32

# This script is kinda terrible atm because it doesn't auto generate the important filesystem structures but shhhh
