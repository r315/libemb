/**
	Minimal support linker file, this file defines 
	sections that matchs usb bootloader.
**/

ENTRY(Reset_Handler)

MEMORY
{
	flash (rx) : ORIGIN = 0x00002000, LENGTH = 504k
	ram0 (rwx) : ORIGIN = 0x10000000, LENGTH = 32k
	ram1 (rwx) : ORIGIN = 0x2007C000, LENGTH = 32k
}

_stack_top = ORIGIN(ram0) + LENGTH(ram0);
_end = _stack_top;  /*needed for syscalls.c */
_ram0 = ORIGIN(ram0);
_ram1 = ORIGIN(ram1);


SECTIONS
{
	.text :
	{	. = ALIGN(4);
		_start_text = .;
		*(.isrvectors)
		*(.text)
		*(.rodata .rodata.*)		
		. = ALIGN(4);
		_end_text = .;
	} > flash
	
	__exidx_start = .;	/*section for libc exceptions */
	.ARM.exidx :
	{		
		*(.ARM.exidx* .gnu.linkonce.armexidx.*)		
	} > flash
	__exidx_end = .;	
	
	.data :
	{		
		. = ALIGN(4);
		_sidata = LOADADDR (.data); /* get the address where data content is stored on flash*/
		_start_data = .;			/* get the actual data address on ram */
		*(.data)		
		. = ALIGN(4);
		_end_data = .;
	} >ram0 AT >flash  /* instructs that the content of data section is stored no flash */

	.bss :
	{
		. = ALIGN(4);
		_start_bss = .;
		*(.bss)		
		. = ALIGN(4);
		_end_bss = .;
	} >ram0		
}