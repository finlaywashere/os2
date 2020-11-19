ENTRY(_start)

SECTIONS{
        . = 0x9000;
        .text : {
                *(.text)
        }
	.rodata : {
                *(.rodata)
        }

        _stage2_end = .;
}

