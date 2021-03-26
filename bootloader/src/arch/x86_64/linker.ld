ENTRY(_start)

SECTIONS{
	/*
		Code starts at 0x9000 where the stage 1 loader loaded it
	*/
	. = 0x9000;
	.text : {
		*(.text)
	}
	.rodata : {
		*(.rodata)
	}
	
	_stage2_end = .;
}

