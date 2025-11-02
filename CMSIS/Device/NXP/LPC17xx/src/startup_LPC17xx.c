/****************************************************************************//**
 * @file :    startup_LPC17xx.c
 * @brief : CMSIS Cortex-M3 Core Device Startup File
 * @version : V1.01
 * @date :    4. Feb. 2009
 *
 *----------------------------------------------------------------------------
 *
 * Copyright (C) 2009 ARM Limited. All rights reserved.
 *
 * ARM Limited (ARM) is supplying this software for use with Cortex-Mx
 * processor based microcontrollers.  This file can be freely distributed
 * within development tools that are supporting such ARM based processors.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 * OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * ARM SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 ******************************************************************************/

// Mod by nio for the .fastcode part
// Mod by Sagar G V for &_stack part. Stack Pointer was not getting initialized to RAM0 top..was hanging in the middle
// Modified by Sagar G V on Mar 11 2011. added __libc_init_array()

#include <stdint.h>
#include "LPC17xx.h"

#define WEAK            __attribute__ ((weak))
#define DEFAULT_HANDLER __attribute__ ((weak, alias("Default_Handler")))

/* Exported types --------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
extern uint32_t _etext;
extern uint32_t _sidata;        /* start address for the initialization values of the .data section. defined in linker script */
extern uint32_t _sdata;         /* start address for the .data section. defined in linker script */
extern uint32_t _edata;         /* end address for the .data section. defined in linker script */

extern uint32_t _sifastcode;    /* start address for the initialization values of the .fastcode section. defined in linker script */
extern uint32_t _sfastcode;     /* start address for the .fastcode section. defined in linker script */
extern uint32_t _efastcode;     /* end address for the .fastcode section. defined in linker script */

extern uint32_t _sbss;          /* start address for the .bss section. defined in linker script */
extern uint32_t _ebss;          /* end address for the .bss section. defined in linker script */

extern uint32_t _stack;         /* init value for the stack pointer. defined in linker script */

//*****************************************************************************
//
// Forward declaration of the default fault handlers.
//
//*****************************************************************************
/* System exception vector handler */
//void WEAK         Reset_Handler(void);             /* Reset Handler */
void NMI_Handler(void)          DEFAULT_HANDLER;    /* NMI Handler */
void HardFault_Handler(void)    DEFAULT_HANDLER;    /* Hard Fault Handler */
void MemManage_Handler(void)    DEFAULT_HANDLER;    /* MPU Fault Handler */
void BusFault_Handler(void)     DEFAULT_HANDLER;    /* Bus Fault Handler */
void UsageFault_Handler(void)   DEFAULT_HANDLER;    /* Usage Fault Handler */
void SVC_Handler(void)          DEFAULT_HANDLER;    /* SVCall Handler */
void DebugMon_Handler(void)     DEFAULT_HANDLER;    /* Debug Monitor Handler */
void PendSV_Handler(void)       DEFAULT_HANDLER;    /* PendSV Handler */
void SysTick_Handler(void)      DEFAULT_HANDLER;    /* SysTick Handler */

/* External interrupt vector handler */
void WDT_IRQHandler(void)       DEFAULT_HANDLER;    /* Watchdog Timer */
void TIMER0_IRQHandler(void)    DEFAULT_HANDLER;    /* Timer0 */
void TIMER1_IRQHandler(void)    DEFAULT_HANDLER;    /* Timer1 */
void TIMER2_IRQHandler(void)    DEFAULT_HANDLER;    /* Timer2 */
void TIMER3_IRQHandler(void)    DEFAULT_HANDLER;    /* Timer3 */
void UART0_IRQHandler(void)     DEFAULT_HANDLER;    /* UART0 */
void UART1_IRQHandler(void)     DEFAULT_HANDLER;    /* UART1 */
void UART2_IRQHandler(void)     DEFAULT_HANDLER;    /* UART2 */
void UART3_IRQHandler(void)     DEFAULT_HANDLER;    /* UART3 */
void PWM1_IRQHandler(void)      DEFAULT_HANDLER;    /* PWM1 */
void I2C0_IRQHandler(void)      DEFAULT_HANDLER;    /* I2C0 */
void I2C1_IRQHandler(void)      DEFAULT_HANDLER;    /* I2C1 */
void I2C2_IRQHandler(void)      DEFAULT_HANDLER;    /* I2C2 */
void SPI_IRQHandler(void)       DEFAULT_HANDLER;    /* SPI */
void SSP0_IRQHandler(void)      DEFAULT_HANDLER;    /* SSP0 */
void SSP1_IRQHandler(void)      DEFAULT_HANDLER;    /* SSP1 */
void PLL0_IRQHandler(void)      DEFAULT_HANDLER;    /* PLL0 (Main PLL) */
void RTC_IRQHandler(void)       DEFAULT_HANDLER;    /* Real Time Clock */
void EINT0_IRQHandler(void)     DEFAULT_HANDLER;    /* External Interrupt 0 */
void EINT1_IRQHandler(void)     DEFAULT_HANDLER;    /* External Interrupt 1 */
void EINT2_IRQHandler(void)     DEFAULT_HANDLER;    /* External Interrupt 2 */
void EINT3_IRQHandler(void)     DEFAULT_HANDLER;    /* External Interrupt 3 */
void ADC_IRQHandler(void)       DEFAULT_HANDLER;    /* A/D Converter */
void BOD_IRQHandler(void)       DEFAULT_HANDLER;    /* Brown Out Detect */
void USB_IRQHandler(void)       DEFAULT_HANDLER;    /* USB */
void CAN_IRQHandler(void)       DEFAULT_HANDLER;    /* CAN */
void DMA_IRQHandler(void)       DEFAULT_HANDLER;    /* GP DMA */
void I2S_IRQHandler(void)       DEFAULT_HANDLER;    /* I2S */
void ENET_IRQHandler(void)      DEFAULT_HANDLER;    /* Ethernet */
void RIT_IRQHandler(void)       DEFAULT_HANDLER;    /* Repetitive Interrupt Timer */
void MCPWM_IRQHandler(void)     DEFAULT_HANDLER;    /* Motor Control PWM */
void QEI_IRQHandler(void)       DEFAULT_HANDLER;    /* Quadrature Encoder Interface */
void PLL1_IRQHandler(void)      DEFAULT_HANDLER;    /* PLL1 (USB PLL) */


/* Private typedef -----------------------------------------------------------*/
/* function prototypes ------------------------------------------------------*/
void Reset_Handler(void) __attribute__((__interrupt__));
extern int main(void);
extern void  __libc_init_array();

//*****************************************************************************
//
// This is the code that gets called when the processor receives an unexpected
// interrupt.  This simply enters an infinite loop, preserving the system state
// for examination by a debugger.
//
//*****************************************************************************
void Default_Handler(void)
{
    volatile int isr_number = (SCB->ICSR & 0x1FF) - 16;
    // See IRQ number on Exceptions Table
    (void)isr_number;

    asm volatile
    (
        "bkpt #01 \n"
        "b . \n"
    );
}


/*******************************************************************************
* Function Name  : Reset_Handler
* Description    : This is the code that gets called when the processor first starts execution
*               following a reset event.  Only the absolutely necessary set is performed,
*               after which the application supplied main() routine is called.
* Input          :
* Output         :
* Return         :
*******************************************************************************/
void Reset_Handler(void)
{
    uint32_t *pulDest;
    uint32_t *pulSrc;

    //
    // Copy the data segment initializers from flash to SRAM in ROM mode
    //

    if (&_sidata != &_sdata) {    // only if needed
        pulSrc = &_sidata;
        for(pulDest = &_sdata; pulDest < &_edata; ) {
            *(pulDest++) = *(pulSrc++);
        }
    }

    // Copy the .fastcode code from ROM to SRAM

    if (&_sifastcode != &_sfastcode) {    // only if needed
        pulSrc = &_sifastcode;
        for(pulDest = &_sfastcode; pulDest < &_efastcode; ) {
            *(pulDest++) = *(pulSrc++);
        }
    }

    //
    // Zero fill the bss segment.
    //
    for(pulDest = &_sbss; pulDest < &_ebss; )
    {
        *(pulDest++) = 0;
    }

    //Initialize static constructors.

    __libc_init_array();
    //
    // Call the application's entry point.
    //

    main();
    while(1);
}

/******************************************************************************
*
* The minimal vector table for a Cortex M3.  Note that the proper constructs
* must be placed on this to ensure that it ends up at physical address
* 0x0000.0000.
*
******************************************************************************/
__attribute__ ((section(".isr_vector")))
const void *g_pfnVectors[] = {
        &_stack,                   /* The initial stack pointer */
        Reset_Handler,             /* Reset Handler */
        NMI_Handler,               /* NMI Handler */
        HardFault_Handler,         /* Hard Fault Handler */
        MemManage_Handler,         /* MPU Fault Handler */
        BusFault_Handler,          /* Bus Fault Handler */
        UsageFault_Handler,        /* Usage Fault Handler */
        0,                         /* Reserved */
        0,                         /* Reserved */
        0,                         /* Reserved */
        0,                         /* Reserved */
        SVC_Handler,               /* SVCall Handler */
        DebugMon_Handler,          /* Debug Monitor Handler */
        0,                         /* Reserved */
        PendSV_Handler,            /* PendSV Handler */
        SysTick_Handler,           /* SysTick Handler */

        // External Interrupts
        WDT_IRQHandler,            /* Watchdog Timer */
        TIMER0_IRQHandler,         /* Timer0 */
        TIMER1_IRQHandler,         /* Timer1 */
        TIMER2_IRQHandler,         /* Timer2 */
        TIMER3_IRQHandler,         /* Timer3 */
        UART0_IRQHandler,          /* UART0 */
        UART1_IRQHandler,          /* UART1 */
        UART2_IRQHandler,          /* UART2 */
        UART3_IRQHandler,          /* UART3 */
        PWM1_IRQHandler,           /* PWM1 */
        I2C0_IRQHandler,           /* I2C0 */
        I2C1_IRQHandler,           /* I2C1 */
        I2C2_IRQHandler,           /* I2C2 */
        SPI_IRQHandler,            /* SPI */
        SSP0_IRQHandler,           /* SSP0 */
        SSP1_IRQHandler,           /* SSP1 */
        PLL0_IRQHandler,           /* PLL0 (Main PLL) */
        RTC_IRQHandler,            /* Real Time Clock */
        EINT0_IRQHandler,          /* External Interrupt 0 */
        EINT1_IRQHandler,          /* External Interrupt 1 */
        EINT2_IRQHandler,          /* External Interrupt 2 */
        EINT3_IRQHandler,          /* External Interrupt 3 */
        ADC_IRQHandler,            /* A/D Converter */
        BOD_IRQHandler,            /* Brown Out Detect */
        USB_IRQHandler,            /* USB */
        CAN_IRQHandler,            /* CAN */
        DMA_IRQHandler,            /* GP DMA */
        I2S_IRQHandler,            /* I2S */
        ENET_IRQHandler,           /* Ethernet */
        RIT_IRQHandler,            /* Repetitive Interrupt Timer */
        MCPWM_IRQHandler,          /* Motor Control PWM */
        QEI_IRQHandler,            /* Quadrature Encoder Interface */
        PLL1_IRQHandler,           /* PLL1 (USB PLL) */
};
