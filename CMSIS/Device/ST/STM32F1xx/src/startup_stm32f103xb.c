#include <stdint.h>
#include "stm32f1xx.h"

#define WEAK __attribute__((weak))
#define NAKED __attribute__((noreturn, naked, aligned(4)))
#define ISR __attribute__((section(".isr_vector")))

ISR void *g_pfnVectors[];
extern uint32_t _sidata, _sdata, _edata, _sbss, _ebss, _stack, _estack;
extern uint32_t _siram_code, _sram_code, _eram_code;

extern void __libc_init_array(void);
WEAK int main(void){}

NAKED void Reset_Handler(void)
{
    volatile uint32_t *src, *dest;

    /* Copy initialize variables with .data section values*/
    for (src = &_sidata, dest = &_sdata; dest < &_edata; src++, dest++){
        *dest = *src;
    }

    /* Copy .ram_code section to ram*/
    for (src = &_siram_code, dest = &_sram_code; dest < &_eram_code; src++, dest++)
    {
       *dest = *src;
    }

    /* Clear .bss */
    dest = &_sbss;

    while (dest < &_ebss)
        *dest++ = 0;

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

NAKED void HardFault_Handler(void)
{
    __asm volatile
    (
        " tst lr, #4                                 \n"        // Check current stack
        " ite eq                                     \n"
        " mrseq r0, msp                              \n"        // Move msp to r0 ??
        " mrsne r0, psp                              \n"        // Move psp to r0 ??
        " ldr r1, [r0, #24]                          \n"        // Get address were exception happen ?
        " ldr r2, dumpHandler_address                \n"
        " bx r2                                      \n"
        " dumpHandler_address: .word RegistersDump   \n"
    );
}

void Default_Handler(void)
{
    volatile uint8_t isr_number = (SCB->ICSR & 255) - 16;
    // See position number on Table 61 from RM0008
    (void)isr_number;

    __asm volatile
    (
        "bkpt #01 \n"
        "b .      \n"
    );
}

void NMI_Handler(void);                 __attribute__ ((weak, alias("Default_Handler")));
//WEAK void HardFault_Handler(void);
void MemManage_Handler(void);           __attribute__ ((weak, alias("Default_Handler")));
void BusFault_Handler(void);            __attribute__ ((weak, alias("Default_Handler")));
void UsageFault_Handler(void);          __attribute__ ((weak, alias("Default_Handler")));
void SVC_Handler(void);                 __attribute__ ((weak, alias("Default_Handler")));
void DebugMon_Handler(void);            __attribute__ ((weak, alias("Default_Handler")));
void PendSV_Handler(void);              __attribute__ ((weak, alias("Default_Handler")));
void SysTick_Handler(void);             __attribute__ ((weak, alias("Default_Handler")));
void WWDG_IRQHandler(void);             __attribute__ ((weak, alias("Default_Handler")));
void PVD_IRQHandler(void);              __attribute__ ((weak, alias("Default_Handler")));
void TAMPER_IRQHandler(void);           __attribute__ ((weak, alias("Default_Handler")));
void RTC_IRQHandler(void);              __attribute__ ((weak, alias("Default_Handler")));
void FLASH_IRQHandler(void);            __attribute__ ((weak, alias("Default_Handler")));
void RCC_IRQHandler(void);              __attribute__ ((weak, alias("Default_Handler")));
void EXTI0_IRQHandler(void);            __attribute__ ((weak, alias("Default_Handler")));
void EXTI1_IRQHandler(void);            __attribute__ ((weak, alias("Default_Handler")));
void EXTI2_IRQHandler(void);            __attribute__ ((weak, alias("Default_Handler")));
void EXTI3_IRQHandler(void);            __attribute__ ((weak, alias("Default_Handler")));
void EXTI4_IRQHandler(void);            __attribute__ ((weak, alias("Default_Handler")));
void DMA1_Channel1_IRQHandler(void);    __attribute__ ((weak, alias("Default_Handler")));
void DMA1_Channel2_IRQHandler(void);    __attribute__ ((weak, alias("Default_Handler")));
void DMA1_Channel3_IRQHandler(void);    __attribute__ ((weak, alias("Default_Handler")));
void DMA1_Channel4_IRQHandler(void);    __attribute__ ((weak, alias("Default_Handler")));
void DMA1_Channel5_IRQHandler(void);    __attribute__ ((weak, alias("Default_Handler")));
void DMA1_Channel6_IRQHandler(void);    __attribute__ ((weak, alias("Default_Handler")));
void DMA1_Channel7_IRQHandler(void);    __attribute__ ((weak, alias("Default_Handler")));
void ADC1_2_IRQHandler(void);           __attribute__ ((weak, alias("Default_Handler")));
void USB_HP_CAN1_TX_IRQHandler(void);   __attribute__ ((weak, alias("Default_Handler")));
void USB_LP_CAN1_RX0_IRQHandler(void);  __attribute__ ((weak, alias("Default_Handler")));
void CAN1_RX1_IRQHandler(void);         __attribute__ ((weak, alias("Default_Handler")));
void CAN1_SCE_IRQHandler(void);         __attribute__ ((weak, alias("Default_Handler")));
void EXTI9_5_IRQHandler(void);          __attribute__ ((weak, alias("Default_Handler")));
void TIM1_BRK_IRQHandler(void);         __attribute__ ((weak, alias("Default_Handler")));
void TIM1_UP_IRQHandler(void);          __attribute__ ((weak, alias("Default_Handler")));
void TIM1_TRG_COM_IRQHandler(void);     __attribute__ ((weak, alias("Default_Handler")));
void TIM1_CC_IRQHandler(void);          __attribute__ ((weak, alias("Default_Handler")));
void TIM2_IRQHandler(void);             __attribute__ ((weak, alias("Default_Handler")));
void TIM3_IRQHandler(void);             __attribute__ ((weak, alias("Default_Handler")));
void TIM4_IRQHandler(void);             __attribute__ ((weak, alias("Default_Handler")));
void I2C1_EV_IRQHandler(void);          __attribute__ ((weak, alias("Default_Handler")));
void I2C1_ER_IRQHandler(void);          __attribute__ ((weak, alias("Default_Handler")));
void I2C2_EV_IRQHandler(void);          __attribute__ ((weak, alias("Default_Handler")));
void I2C2_ER_IRQHandler(void);          __attribute__ ((weak, alias("Default_Handler")));
void SPI1_IRQHandler(void);             __attribute__ ((weak, alias("Default_Handler")));
void SPI2_IRQHandler(void);             __attribute__ ((weak, alias("Default_Handler")));
void USART1_IRQHandler(void);           __attribute__ ((weak, alias("Default_Handler")));
void USART2_IRQHandler(void);           __attribute__ ((weak, alias("Default_Handler")));
void USART3_IRQHandler(void);           __attribute__ ((weak, alias("Default_Handler")));
void EXTI15_10_IRQHandler(void);        __attribute__ ((weak, alias("Default_Handler")));
void RTC_Alarm_IRQHandler(void);        __attribute__ ((weak, alias("Default_Handler")));
void USBWakeUp_IRQHandler(void);        __attribute__ ((weak, alias("Default_Handler")));

ISR void *g_pfnVectors[] = {
    &_estack,
    Reset_Handler,
    NMI_Handler,
    HardFault_Handler,
    MemManage_Handler,
    BusFault_Handler,
    UsageFault_Handler,
    0,
    0,
    0,
    0,
    SVC_Handler,
    DebugMon_Handler,
    0,
    PendSV_Handler,
    SysTick_Handler,
    WWDG_IRQHandler,
    PVD_IRQHandler,
    TAMPER_IRQHandler,
    RTC_IRQHandler,
    FLASH_IRQHandler,
    RCC_IRQHandler,
    EXTI0_IRQHandler,
    EXTI1_IRQHandler,
    EXTI2_IRQHandler,
    EXTI3_IRQHandler,
    EXTI4_IRQHandler,
    DMA1_Channel1_IRQHandler,
    DMA1_Channel2_IRQHandler,
    DMA1_Channel3_IRQHandler,
    DMA1_Channel4_IRQHandler,
    DMA1_Channel5_IRQHandler,
    DMA1_Channel6_IRQHandler,
    DMA1_Channel7_IRQHandler,
    ADC1_2_IRQHandler,
    USB_HP_CAN1_TX_IRQHandler,
    USB_LP_CAN1_RX0_IRQHandler,
    CAN1_RX1_IRQHandler,
    CAN1_SCE_IRQHandler,
    EXTI9_5_IRQHandler,
    TIM1_BRK_IRQHandler,
    TIM1_UP_IRQHandler,
    TIM1_TRG_COM_IRQHandler,
    TIM1_CC_IRQHandler,
    TIM2_IRQHandler,
    TIM3_IRQHandler,
    TIM4_IRQHandler,
    I2C1_EV_IRQHandler,
    I2C1_ER_IRQHandler,
    I2C2_EV_IRQHandler,
    I2C2_ER_IRQHandler,
    SPI1_IRQHandler,
    SPI2_IRQHandler,
    USART1_IRQHandler,
    USART2_IRQHandler,
    USART3_IRQHandler,
    EXTI15_10_IRQHandler,
    RTC_Alarm_IRQHandler,
    USBWakeUp_IRQHandler,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
};
