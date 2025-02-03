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
void HardFault_Handler(void)
{
  while(1);
}

/*----------------------------------------------------------------------------
  Default Handler for Exceptions / Interrupts
 *----------------------------------------------------------------------------*/
void Default_Handler(void)
{
  while(1);
}

#if defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
  #pragma clang diagnostic pop
#endif

