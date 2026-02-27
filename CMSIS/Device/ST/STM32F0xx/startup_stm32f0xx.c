#include <stdint.h>
#include "stm32f0xx.h"

#define WEAK __attribute__((weak))
#define WEAK_ALIAS(sym, target) __attribute__((weak, alias(#target))) void sym (void)
#define NAKED __attribute__((noreturn, naked, aligned(4)))
#define ISR __attribute__((section(".isr_vector")))
#define WEAK_ALIAS_HANDLER(_handler_) WEAK_ALIAS(_handler_, defaultHandler)

extern uint32_t _sidata, _sdata, _edata, _sbss, _ebss, _stack, _estack;
extern uint32_t _sstack, _estack;

extern void __libc_init_array(void);
WEAK int main(void){}

NAKED void Reset_Handler(void)
{
    volatile uint32_t *src, *dst;

    /* Copy initialize variables with .data section values*/
    src = &_sidata;
    dst = &_sdata;

    while (dst < &_edata){
        *dst++ = *src++;
    }

    /* Clear .bss */
    dst = &_sbss;

    while (dst < &_ebss)
        *dst++ = 0;

#if 0
    /* Copy .ram_code section to ram*/
    for (src = &_siram_code, dest = &_sram_code; dest < &_eram_code; src++, dest++)
    {
       *dest = *src;
    }
#endif

    /* Fill stack */
    dst = &_sstack;

    while (dst < &_estack)
        *dst++ = *(uint32_t*)"STAK";

    /* Configure system clocks */
    SystemInit();
    /* Call c++ constructors */
    __libc_init_array();

    main();

    /* case returns... */
    __asm volatile("b .");
}

typedef struct {
    uint32_t r0, r1, r2, r3, r12, lr, pc, psr;
}stackframe_t;

void RegistersDump(stackframe_t *regs)
{
    (void)regs;

    __asm volatile
    (
        "bkpt #01 \n"
        "b . \n"
    );
}

NAKED void HardFault_Handler(void){
    __asm volatile (
        " mov   r2, lr            \n"   // copy LR into a low register
        " movs  r0, #4            \n"   // r0 = 4
        " tst   r2, r0            \n"   // test EXC_RETURN bit 2
        " beq   msp_active        \n"   // 0 = MSP, 1 = PSP
        " mrs   r0, psp           \n"   // r0 = PSP
        " b     get_pc            \n"
    "msp_active:                  \n"
        " mrs   r0, msp           \n"   // r0 = MSP
    "get_pc:                      \n"
        " ldr   r1, [r0, #24]     \n"   // stacked PC
        " ldr   r2, =RegistersDump\n"   // function pointer
        " bx    r2                \n"
    );
}


void defaultHandler(void){
    volatile uint8_t isr_number = (SCB->ICSR & 255) - 16;
    // See position number on Table 61 from RM0008
    (void)isr_number;

    __asm volatile
    (
        "bkpt #01 \n"
        "b .      \n"
    );
}


//WEAK void NMI_Handler(void);
WEAK_ALIAS_HANDLER(NMI_Handler);
WEAK_ALIAS_HANDLER(SVC_Handler);
WEAK_ALIAS_HANDLER(PendSV_Handler);
WEAK_ALIAS_HANDLER(SysTick_Handler);
WEAK_ALIAS_HANDLER(WWDG_IRQHandler);
WEAK_ALIAS_HANDLER(PVD_VDDIO2_IRQHandler);
WEAK_ALIAS_HANDLER(TAMPER_IRQHandler);
WEAK_ALIAS_HANDLER(RTC_IRQHandler);
WEAK_ALIAS_HANDLER(FLASH_IRQHandler);
WEAK_ALIAS_HANDLER(RCC_CRS_IRQHandler);
WEAK_ALIAS_HANDLER(EXTI0_1_IRQHandler);
WEAK_ALIAS_HANDLER(EXTI2_3_IRQHandler);
WEAK_ALIAS_HANDLER(EXTI4_15_IRQHandler);
WEAK_ALIAS_HANDLER(TSC_IRQHandler);
WEAK_ALIAS_HANDLER(DMA1_Channel1_IRQHandler);
WEAK_ALIAS_HANDLER(DMA1_Channel2_3_IRQHandler);
WEAK_ALIAS_HANDLER(DMA1_Channel4_5_6_7_IRQHandler);
WEAK_ALIAS_HANDLER(ADC1_COMP_IRQHandler);
WEAK_ALIAS_HANDLER(TIM1_BRK_UP_TRG_COM_IRQHandler);
WEAK_ALIAS_HANDLER(TIM1_CC_IRQHandler);
WEAK_ALIAS_HANDLER(TIM2_IRQHandler);
WEAK_ALIAS_HANDLER(TIM3_IRQHandler);
WEAK_ALIAS_HANDLER(TIM6_DAC_IRQHandler);
WEAK_ALIAS_HANDLER(TIM7_IRQHandler);
WEAK_ALIAS_HANDLER(TIM14_IRQHandler);
WEAK_ALIAS_HANDLER(TIM15_IRQHandler);
WEAK_ALIAS_HANDLER(TIM16_IRQHandler);
WEAK_ALIAS_HANDLER(TIM17_IRQHandler);
WEAK_ALIAS_HANDLER(I2C1_IRQHandler);
WEAK_ALIAS_HANDLER(I2C2_IRQHandler);
WEAK_ALIAS_HANDLER(SPI1_IRQHandler);
WEAK_ALIAS_HANDLER(SPI2_IRQHandler);
WEAK_ALIAS_HANDLER(USART1_IRQHandler);
WEAK_ALIAS_HANDLER(USART2_IRQHandler);
WEAK_ALIAS_HANDLER(USART3_4_IRQHandler);
WEAK_ALIAS_HANDLER(CEC_CAN_IRQHandler);

ISR void *g_pfnVectors[48] = {
    &_estack,
    Reset_Handler,
    NMI_Handler,
    HardFault_Handler,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    SVC_Handler,
    0,
    0,
    PendSV_Handler,
    SysTick_Handler,
    WWDG_IRQHandler,                   /* Window WatchDog              */
    PVD_VDDIO2_IRQHandler,             /* PVD and VDDIO2 through EXTI Line detect */
    RTC_IRQHandler,                    /* RTC through the EXTI line    */
    FLASH_IRQHandler,                  /* FLASH                        */
    RCC_CRS_IRQHandler,                /* RCC and CRS                  */
    EXTI0_1_IRQHandler,                /* EXTI Line 0 and 1            */
    EXTI2_3_IRQHandler,                /* EXTI Line 2 and 3            */
    EXTI4_15_IRQHandler,               /* EXTI Line 4 to 15            */
    TSC_IRQHandler,                    /* TSC                          */
    DMA1_Channel1_IRQHandler,          /* DMA1 Channel 1               */
    DMA1_Channel2_3_IRQHandler,        /* DMA1 Channel 2 and Channel 3 */
    DMA1_Channel4_5_6_7_IRQHandler,    /* DMA1 Channel 4, Channel 5, Channel 6 and Channel 7*/
    ADC1_COMP_IRQHandler,              /* ADC1, COMP1 and COMP2         */
    TIM1_BRK_UP_TRG_COM_IRQHandler,    /* TIM1 Break, Update, Trigger and Commutation */
    TIM1_CC_IRQHandler,                /* TIM1 Capture Compare         */
    TIM2_IRQHandler,                   /* TIM2                         */
    TIM3_IRQHandler,                   /* TIM3                         */
    TIM6_DAC_IRQHandler,               /* TIM6 and DAC                 */
    TIM7_IRQHandler,                   /* TIM7                         */
    TIM14_IRQHandler,                  /* TIM14                        */
    TIM15_IRQHandler,                  /* TIM15                        */
    TIM16_IRQHandler,                  /* TIM16                        */
    TIM17_IRQHandler,                  /* TIM17                        */
    I2C1_IRQHandler,                   /* I2C1                         */
    I2C2_IRQHandler,                   /* I2C2                         */
    SPI1_IRQHandler,                   /* SPI1                         */
    SPI2_IRQHandler,                   /* SPI2                         */
    USART1_IRQHandler,                 /* USART1                       */
    USART2_IRQHandler,                 /* USART2                       */
    USART3_4_IRQHandler,               /* USART3 and USART4            */
    CEC_CAN_IRQHandler,                /* CEC and CAN                  */
    0                                  /* Reserved  */
};
