target remote localhost:1234
symbol-file kernel/bin/kernel.elf
hbreak *0x7c00
hbreak *0x9000
hbreak kernel_start

#symbol-file test_programs/src/test_c.o
#hbreak main

layout split
