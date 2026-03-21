/******************************************************************************
 * @file     startup_ARMCM23.c
 * @brief    CMSIS-Core Device Startup File for a Cortex-M23 Device
 * @version  V2.1.0
 * @date     16. December 2020
 ******************************************************************************/
/*
 * Copyright (c) 2009-2020 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdint.h>

#include "gd32e23x.h"


/* Cortex-M23 Fault Register Addresses */
#define HFSR    (*((volatile uint32_t *)0xE000ED2C))
#define DFSR    (*((volatile uint32_t *)0xE000ED30))

/* HFSR bit definitions */
#define HFSR_VECTTBL    (1UL << 1)   /* Bus fault on vector table read     */
#define HFSR_FORCED     (1UL << 30)  /* Forced HardFault (escalated fault) */
#define HFSR_DEBUGEVT   (1UL << 31)  /* Debug event                        */

/* DFSR bit definitions */
#define DFSR_HALTED     (1UL << 0)
#define DFSR_BKPT       (1UL << 1)
#define DFSR_DWTTRAP    (1UL << 2)
#define DFSR_VCATCH     (1UL << 3)
#define DFSR_EXTERNAL   (1UL << 4)

#define NORETURN   __attribute__((naked, __noreturn__))

/*----------------------------------------------------------------------------
  External References
 *----------------------------------------------------------------------------*/
extern uint32_t __INITIAL_SP;
extern uint32_t __STACK_LIMIT;
#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
extern uint32_t __STACK_SEAL;
#endif

extern __NO_RETURN void __PROGRAM_START(void);

//extern void main(void);

//void _start(void) { main (); while(1){}}

/*----------------------------------------------------------------------------
  Internal References
 *----------------------------------------------------------------------------*/
__NO_RETURN void Reset_Handler  (void);
            void Default_Handler(void);

/*----------------------------------------------------------------------------
  Exception / Interrupt Handler
 *----------------------------------------------------------------------------*/
/* Exceptions */
void NMI_Handler            (void) __attribute__ ((weak, alias("Default_Handler")));
void HardFault_Handler      (void) __attribute__ ((weak));
void SVC_Handler            (void) __attribute__ ((weak, alias("Default_Handler")));
void PendSV_Handler         (void) __attribute__ ((weak, alias("Default_Handler")));
void SysTick_Handler        (void) __attribute__ ((weak, alias("Default_Handler")));

void WWDGT_IRQHandler       (void) __attribute__ ((weak, alias("Default_Handler")));
void LVD_IRQHandler         (void) __attribute__ ((weak, alias("Default_Handler")));
void RTC_IRQHandler         (void) __attribute__ ((weak, alias("Default_Handler")));
void FMC_IRQHandler         (void) __attribute__ ((weak, alias("Default_Handler")));
void RCU_IRQHandler         (void) __attribute__ ((weak, alias("Default_Handler")));
void EXTI0_1_IRQHandler     (void) __attribute__ ((weak, alias("Default_Handler")));
void EXTI2_3_IRQHandler     (void) __attribute__ ((weak, alias("Default_Handler")));
void EXTI4_15_IRQHandler    (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA_Channel0_IRQHandler (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA_Channel1_2_IRQHandler (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA_Channel3_4_IRQHandler     (void) __attribute__ ((weak, alias("Default_Handler")));
void ADC_CMP_IRQHandler      (void) __attribute__ ((weak, alias("Default_Handler")));
void TIMER0_BRK_UP_TRG_COM_IRQHandler     (void) __attribute__ ((weak, alias("Default_Handler")));
void TIMER0_Channel_IRQHandler     (void) __attribute__ ((weak, alias("Default_Handler")));
void TIMER2_IRQHandler      (void) __attribute__ ((weak, alias("Default_Handler")));
void TIMER5_IRQHandler      (void) __attribute__ ((weak, alias("Default_Handler")));
void TIMER13_IRQHandler     (void) __attribute__ ((weak, alias("Default_Handler")));
void TIMER14_IRQHandler     (void) __attribute__ ((weak, alias("Default_Handler")));
void TIMER15_IRQHandler     (void) __attribute__ ((weak, alias("Default_Handler")));
void TIMER16_IRQHandler     (void) __attribute__ ((weak, alias("Default_Handler")));
void I2C0_EV_IRQHandler     (void) __attribute__ ((weak, alias("Default_Handler")));
void I2C1_EV_IRQHandler     (void) __attribute__ ((weak, alias("Default_Handler")));
void SPI0_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));
void SPI1_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));
void USART0_IRQHandler      (void) __attribute__ ((weak, alias("Default_Handler")));
void USART1_IRQHandler      (void) __attribute__ ((weak, alias("Default_Handler")));
void I2C0_ER_IRQHandler     (void) __attribute__ ((weak, alias("Default_Handler")));
void I2C1_ER_IRQHandler     (void) __attribute__ ((weak, alias("Default_Handler")));


/*----------------------------------------------------------------------------
  Exception / Interrupt Vector table
 *----------------------------------------------------------------------------*/

#if defined ( __GNUC__ )
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#endif

extern const VECTOR_TABLE_Type __VECTOR_TABLE[240];
       const VECTOR_TABLE_Type __VECTOR_TABLE[240] __VECTOR_TABLE_ATTRIBUTE = {
  (VECTOR_TABLE_Type)(&__INITIAL_SP),       /*     Initial Stack Pointer */
  Reset_Handler,                            /*     Reset Handler */
  NMI_Handler,                              /* -14 NMI Handler */
  HardFault_Handler,                        /* -13 Hard Fault Handler */
  0,                                        /*     Reserved */
  0,                                        /*     Reserved */
  0,                                        /*     Reserved */
  0,                                        /*     Reserved */
  0,                                        /*     Reserved */
  0,                                        /*     Reserved */
  0,                                        /*     Reserved */
  SVC_Handler,                              /*  -5 SVCall Handler */
  0,                                        /*     Reserved */
  0,                                        /*     Reserved */
  PendSV_Handler,                           /*  -2 PendSV Handler */
  SysTick_Handler,                          /*  -1 SysTick Handler */

  /* Interrupts */
  /* external interrupts handler */
  WWDGT_IRQHandler,                 //16:Window Watchdog Timer
  LVD_IRQHandler  ,                 //17:LVD through EXTI Line detect
  RTC_IRQHandler  ,                 //18:RTC through EXTI Line
  FMC_IRQHandler  ,                 //19:FMC
  RCU_IRQHandler  ,                 //20:RCU
  EXTI0_1_IRQHandler,               //21:EXTI Line 0 and EXTI Line 1
  EXTI2_3_IRQHandler,               //22:EXTI Line 2 and EXTI Line 3
  EXTI4_15_IRQHandler,              //23:EXTI Line 4 to EXTI Line 15
  0               ,                 //Reserved
  DMA_Channel0_IRQHandler,          //25:DMA Channel 0
  DMA_Channel1_2_IRQHandler,        //26:DMA Channel 1 and DMA Channel 2
  DMA_Channel3_4_IRQHandler,        //27:DMA Channel 3 and DMA Channel 4
  ADC_CMP_IRQHandler,               //28:ADC and Comparator
  TIMER0_BRK_UP_TRG_COM_IRQHandler, //29:TIMER0 Break,Update,Trigger and Commutation
  TIMER0_Channel_IRQHandler,        //30:TIMER0 Channel Capture Compare
  0               ,                 //Reserved
  TIMER2_IRQHandler ,               //32:TIMER2
  TIMER5_IRQHandler ,               //33:TIMER5
  0               ,                 //Reserved
  TIMER13_IRQHandler,               //35:TIMER13
  TIMER14_IRQHandler,               //36:TIMER14
  TIMER15_IRQHandler,               //37:TIMER15
  TIMER16_IRQHandler,               //38:TIMER16
  I2C0_EV_IRQHandler,               //39:I2C0 Event
  I2C1_EV_IRQHandler,               //40:I2C1 Event
  SPI0_IRQHandler ,                 //41:SPI0
  SPI1_IRQHandler ,                 //42:SPI1
  USART0_IRQHandler ,               //43:USART0
  USART1_IRQHandler ,               //44:USART1
  0               ,                 //Reserved
  0               ,                 //Reserved
  0               ,                 //Reserved
  I2C0_ER_IRQHandler,               //48:I2C0 Error
  0               ,                 //Reserved
  I2C1_ER_IRQHandler
};

#if defined ( __GNUC__ )
#pragma GCC diagnostic pop
#endif

/*----------------------------------------------------------------------------
  Reset Handler called on controller reset
 *----------------------------------------------------------------------------*/
__NO_RETURN void Reset_Handler(void)
{
  __set_PSP((uint32_t)(&__INITIAL_SP));

#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
  __set_MSPLIM((uint32_t)(&__STACK_LIMIT));
  __set_PSPLIM((uint32_t)(&__STACK_LIMIT));

  __TZ_set_STACKSEAL_S((uint32_t *)(&__STACK_SEAL));
#endif

  SystemInit();                             /* CMSIS System Initialization */
  __PROGRAM_START();                        /* Enter PreMain (C library entry point) */
}


#if defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Wmissing-noreturn"
#endif

/*----------------------------------------------------------------------------
  Hard Fault Handler
 *----------------------------------------------------------------------------*/
typedef struct {
    uint32_t r0;
    uint32_t r1;
    uint32_t r2;
    uint32_t r3;
    uint32_t r12;
    uint32_t lr;
    uint32_t pc;
    union{
        uint32_t xpsr;
        struct {
            uint32_t exception : 9;
            uint32_t _0_0 : 1;
            uint32_t ici_itl : 6;
            uint32_t ge : 4;
            uint32_t _0_1 : 1;
            uint32_t b : 1;
            uint32_t res0 : 2;
            uint32_t t : 1;
            uint32_t ici_ith : 2;
            uint32_t q : 1;
            uint32_t v : 1;
            uint32_t c : 1;
            uint32_t z : 1;
            uint32_t n : 1;
        }xpsr_bit;
    };
} ExceptionFrame_t;

void fault_print_char(char c)
{
    USART_TDATA(USART0) = c;
    while(!(USART_STAT(USART0) & USART_STAT_TBE));
}

void fault_print(const char *reason)
{
    while (*reason)
    {
        fault_print_char(*reason++);
    }
}

void fault_print_hex(const char *label, uint32_t value)
{
    fault_print(label);
    char *pval = (char*)&value + 3;
    for(int8_t i = 0; i < 4; i++, pval--){
        uint8_t byte = *pval >> 4;
        // print hex byte
        fault_print_char(byte + ((byte > 9) ? '7' : '0'));
        byte = *pval & 15;
        fault_print_char(byte + ((byte > 9) ? '7' : '0'));
    }
}

void CoreDump(ExceptionFrame_t *frame)
{
    uint32_t hfsr = HFSR;
    uint32_t dfsr = DFSR;

    RCU_APB2EN |= RCU_APB2EN_USART0EN;
    RCU_APB2RST |= RCU_APB2RST_USART0RST;
    RCU_AHBEN |= RCU_AHBEN_PAEN;
    RCU_APB2RST &= ~RCU_APB2RST_USART0RST;
    GPIO_CTL(GPIOA) = 0x28080000;   // Cfg PA9 as AF
    GPIO_AFSEL1(GPIOA) = (1 << 4);  // AF1_UART1_TX

    USART_BAUD(USART0) = 0x271;
    USART_CTL0(USART0) = 0x09;

    fault_print("\n========== HARDFAULT ==========\n");

    /* --- Stacked register dump --- */
    fault_print("-- Stacked Registers --\n");
    fault_print_hex(" R0 : 0x", frame->r0);
    fault_print_hex("  R1 : 0x", frame->r1);
    fault_print_hex("  R2 : 0x", frame->r2);
    fault_print_hex("  R3 : 0x", frame->r3);
    fault_print_char('\n');
    fault_print_hex(" R12: 0x", frame->r12);
    fault_print_hex("  LR : 0x", frame->lr);
    fault_print_hex("  PC : 0x", frame->pc);   /* <-- faulting address */
    fault_print_hex("  xPSR: 0x", frame->xpsr);

    /* --- HFSR analysis --- */
    fault_print("\n-- HardFault Status --\n");
    fault_print_hex(" HFSR ", hfsr);

    if (hfsr & HFSR_VECTTBL) {
        fault_print("  [!] VECTTBL: Bus fault on vector table read\n");
    }
    if (hfsr & HFSR_FORCED) {
        fault_print("  [!] FORCED: Escalated fault (usage/bus/mem fault)\n");
        fault_print("      -> Check PC for the faulting instruction\n");
    }
    if (hfsr & HFSR_DEBUGEVT) {
        fault_print("  [!] DEBUGEVT: Debug event caused HardFault\n");
    }

    /* --- DFSR analysis --- */
    fault_print("\n-- Debug Fault Status --\n");
    fault_print_hex(" DFSR ", dfsr);

    if (dfsr & DFSR_HALTED)   fault_print("  [!] HALTED: C_HALT or C_STEP debug halt\n");
    if (dfsr & DFSR_BKPT)     fault_print("  [!] BKPT: Breakpoint instruction\n");
    if (dfsr & DFSR_DWTTRAP)  fault_print("  [!] DWTTRAP: DWT watchpoint match\n");
    if (dfsr & DFSR_VCATCH)   fault_print("  [!] VCATCH: Vector catch triggered\n");
    if (dfsr & DFSR_EXTERNAL) fault_print("  [!] EXTERNAL: External debug request\n");

    fault_print("\n================================\n");

    /* Clear fault status registers by writing 1s */
    HFSR = hfsr;
    DFSR = dfsr;
}

NORETURN void HardFault_Handler(void)
{
    /* Determine which stack was active: MSP or PSP */
    /* EXC_RETURN in LR: bit 2 = 1 means PSP was used  */
    asm("mov r0, lr");
    asm("movs r1, #4");
    asm("tst r0,r1");
    asm("beq use_msp");
    asm("mrs r0, psp");
    asm("b CoreDump");
    asm("use_msp:");
    asm("mrs r0, msp");
    asm("push {lr}");   /* save lr for later */
    asm("bl CoreDump");
    asm("pop {r0}");
    asm("mov lr, r0");  /* restore lr to maintain stack */

    asm("bkpt #01");
    asm("b .");
}

/*----------------------------------------------------------------------------
  Default Handler for Exceptions / Interrupts
 *----------------------------------------------------------------------------*/
void Default_Handler(void)
{
    volatile uint8_t isr_number = (SCB->ICSR & 255) - 16;
    // See position number on __VECTOR_TABLE
    (void)isr_number;

    asm("bkpt #01");
    asm("b .");
}

#if defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
  #pragma clang diagnostic pop
#endif

