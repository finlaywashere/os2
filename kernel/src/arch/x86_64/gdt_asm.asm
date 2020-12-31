extern gdt_ptr
global load_gdt
load_gdt:
        lgdt [gdt_ptr]
        ret
