/**
 LD script for ram load 31 jan 2017
 for jlink use version 6.11d jlinkARM.dll file
 when using isp for loading programs mind that the first 512bytes are used by loader
**/

ENTRY(Reset_Handler)

MEMORY {
	/* ram0 (rx) : ORIGIN = 0x00002000, LENGTH = 32k - 512 /* for isp reserv 512bytes of ram */
	ram0 (rwx) : ORIGIN = 0x10000000, LENGTH = 32k  
	ram1 (rwx) : ORIGIN = 0x2007C000, LENGTH = 32k
}

SECTIONS {
	.text : {		
		_stext = LOADADDR(.text);
		KEEP(*(.startup .startup.*))
		*(.text)
		*(.rodata .rodata.*)		
		. = ALIGN(4);
		_etext = .;
		_estartup = LOADADDR(.text);
	} > ram0	
	
	.data : {				/* note: running libc on ram ocupies almost all ram */
		. = ALIGN(4);
		_sidata = .;
		_sdata = _sidata;
		*(.data)		
		. = ALIGN(4);
		_edata = .;
	} > ram0

	.bss : {
		. = ALIGN(4);		
		_sbss = .;
		__bss_start__ = .;
		*(.bss)	
		*(COMMON)
		. = ALIGN(4);
		_ebss = .;
		__bss_end__ = .;
		. = ALIGN(8);
		_end = .;		
	} > ram0	

	.stack : {
		. = ALIGN(4);
		*(.stack*)	
		. = ALIGN(4);		
	} > ram0	
	
	_stack = ORIGIN(ram0) + LENGTH(ram0);
	
	_siramcode = .;  /* avoid ram copy on startup */
	_sramcode  = .;
	_eramcode  = .;

	/* Stabs debugging sections.  */
  	.stab          0 : { *(.stab) }
  	.stabstr       0 : { *(.stabstr) }
  	.stab.excl     0 : { *(.stab.excl) }
  	.stab.exclstr  0 : { *(.stab.exclstr) }
  	.stab.index    0 : { *(.stab.index) }
  	.stab.indexstr 0 : { *(.stab.indexstr) }
  	.comment       0 : { *(.comment) }	
}
