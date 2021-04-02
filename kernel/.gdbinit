target remote localhost:1234
symbol-file bin/kernel.elf
hbreak kernel_start

layout split
