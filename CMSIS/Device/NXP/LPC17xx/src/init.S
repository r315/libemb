/*
 * @file init.S
 * @brief this is the first code executed after hard reset
 *        it clears bss section and jump to main  
 *
 *  Created on: 30 Jan 2017
 *      Author: hugo reis
 */

/*****************************************************************
	Bare Bone assemble Init
 ******************************************************************/

.section .startup, "ax"

.global _start
.global Reset_Handler

.word	_stack
.word   _start

Reset_Handler:
_start:	

		mov     r0, #0
		mov     r1, #0
		ldr     r2, =main
		blx     r2
		mov		pc, lr
		
      .end   

