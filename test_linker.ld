ENTRY(_start)

SECTIONS{
	. = 0x1000;
	.text ALIGN (4K) : AT (ADDR (.text)){
                *(.text)
        }
}
