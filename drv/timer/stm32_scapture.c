#include "board.h"
#include "scapture.h"

#if defined(STM32L412xx)

#define CAP_TIM2_CH1

#define TIM_CR2_MMS_COMPARE_PULSE   (3U << TIM_CR2_MMS_Pos)
#define TIM_CCMR1_CC1S_TI1          (1U << TIM_CCMR1_CC1S_Pos)
#define TIM_CCER_TI1FP1_BOTH        (10U)
#define TIM_SMCR_SMS_TRIGGER_MODE   (6U << TIM_SMCR_SMS_Pos)

#define TIMEOUT_TIM                 TIM15
#define TIMEOUT_TIM_IRQn            TIM1_BRK_TIM15_IRQn
#define TIMEOUT_TIM_ENABLE          \
    __HAL_RCC_TIM15_CLK_ENABLE();   \
    __HAL_RCC_TIM15_FORCE_RESET();  \
    __HAL_RCC_TIM15_RELEASE_RESET();
#define TIMEOUT_TIM_HANDLER         void TIM1_BRK_TIM15_IRQHandler(void)

#if defined(CAP_TIM1_CH1)
#define CAP_TIM                     TIM1
#define CAP_DMA                     DMA1_Channel2
#define CAP_TIM_DMA_CSELR           (DMA1_CSELR->CSELR & ~DMA_CSELR_C2S_Msk) | (7U << DMA_CSELR_C2S_Pos)
#define CAP_TIM_IRQn                TIM1_CC_IRQn
#define CAP_DMA_IRQn                DMA1_Channel2_IRQn
#define CAP_DMA_ISR                 DMA_ISR_GIF2
#define CAP_DMA_IFCR                (DMA_IFCR_CGIF2 | DMA_IFCR_CTCIF2 | DMA_IFCR_CTEIF2)
#define CAP_TIM_ENABLE              \
    __HAL_RCC_TIM1_CLK_ENABLE();    \
    __HAL_RCC_TIM1_FORCE_RESET();   \
    __HAL_RCC_TIM1_RELEASE_RESET();
#define CAP_DMA_HANDLER             void DMA1_Channel2_IRQHandler(void)
#define CAP_TIM_HANDLER             void TIM1_CC_IRQHandler(void)
#elif defined(CAP_TIM2_CH1)
#define CAP_TIM                     TIM2
#define CAP_TIM_DMA_CSELR           (DMA1_CSELR->CSELR & ~DMA_CSELR_C5S_Msk) | (4U << DMA_CSELR_C5S_Pos)  // Map TIM2_CH1 to DMA1 channel 5
#define CAP_TIM_IRQn                TIM2_IRQn
#define CAP_DMA                     DMA1_Channel5
#define CAP_DMA_IRQn                DMA1_Channel5_IRQn
#define CAP_DMA_ISR                 DMA_ISR_GIF5
#define CAP_DMA_IFCR                (DMA_IFCR_CGIF5 | DMA_IFCR_CTCIF5 | DMA_IFCR_CTEIF5)
#define CAP_TIM_ENABLE              \
    __HAL_RCC_TIM2_CLK_ENABLE();    \
    __HAL_RCC_TIM2_FORCE_RESET();   \
    __HAL_RCC_TIM2_RELEASE_RESET();
#define CAP_DMA_HANDLER             void DMA1_Channel5_IRQHandler(void)
#define CAP_TIM_HANDLER             void TIM2_IRQHandler(void)
#endif



static void scapInitGpio(void){
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    __HAL_RCC_GPIOA_CLK_ENABLE();
#if defined(CAP_TIM1_CH1)
    /**TIM1 GPIO Configuration
    PA8 (Pin D9)    ------> TIM1_CH1
    */
    GPIO_InitStruct.Pin = GPIO_PIN_8;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
#elif defined(CAP_TIM2_CH1)    
    /**TIM2 GPIO Configuration
    PA0-CK_IN (Pin A0)    ------> TIM2_CH1
    */
    GPIO_InitStruct.Pin = GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
#endif
}

/**
 * @brief Initialize signal capture engine
 * 
 * Timer 1 is configured to operate in capture mode, counter value is captured 
 * to CCR1 upon edge change on PA8 and transferred to memory by DMA. The captured values 
 * are the time between egdges in us and can be used to obtain a bit stream if bit time is known.
 *
 * The capture is ended by two means, timeout or destination buffer full. To end the capture by timeout
 * Timer 15 is configured to count to the specified timeout, since is configured to count ms. Because Timer 15
 * is configured to operate as slave and one pulse mode, it will start upon first capture of Timer 1 and stop 
 * when reaches timeout value.
 * 
 * */
void SCAP_Init(void){
    __HAL_RCC_DMA1_CLK_ENABLE();

    CAP_TIM_ENABLE;
    TIMEOUT_TIM_ENABLE;
    
    CAP_TIM->PSC = (SystemCoreClock / 1000000) - 1;     // Count us
    CAP_TIM->ARR = 65535;                               // Max count 65535us
    //CAP_TIM->CR2 = TIM_CR2_MMS_COMPARE_PULSE;         // Configure master mode, valid for TIM1 and TIM15
    CAP_TIM->CCMR1 = TIM_CCMR1_CC1S_TI1;                // Configure channel 1 capture mode for TI1 input
    CAP_TIM->CCER = TIM_CCER_TI1FP1_BOTH;               // Capture on both edges
    CAP_TIM->DIER = TIM_DIER_CC1DE;                     // Enable DMA Request

    TIMEOUT_TIM->CR1 = TIM_CR1_OPM;                     // Enable one pulse mode
    TIMEOUT_TIM->PSC = (SystemCoreClock / 1000) - 1;    // Count ms
    TIMEOUT_TIM->ARR = 65535;                           // Count to max
    //TIMEOUT_TIM->SMCR = TIM_SMCR_SMS_TRIGGER_MODE;    // Configure slave mode, valid for TIM1 and TIM15
    TIMEOUT_TIM->DIER = TIM_DIER_UIE;                   // Generate interrupt on update event

    scapInitGpio();

    CAP_DMA->CCR =
            DMA_CCR_MSIZE_0 |                           // 16bit Dst size
			DMA_CCR_PSIZE_0 |                           // 16bit src size
			DMA_CCR_TCIE;                               // Enable Transfer Complete interrupt
    CAP_DMA->CPAR = (uint32_t)&CAP_TIM->CCR1;           // Peripheral source
    CAP_DMA->CCR |= DMA_CCR_MINC;                       // Enable memory increment
    DMA1_CSELR->CSELR = CAP_TIM_DMA_CSELR;
    
    HAL_NVIC_EnableIRQ(CAP_DMA_IRQn);
    HAL_NVIC_EnableIRQ(TIMEOUT_TIM_IRQn);
    HAL_NVIC_EnableIRQ(CAP_TIM_IRQn);
}


/**
 * @brief Starts a capture
 * 
 * \param dst       : Pointer for destination of captured values
 * \param size      : Buffer size
 * \param duration  : Duration of capture in ms
 *  */
void SCAP_Start(uint16_t *dst, uint32_t size, uint32_t duration){
    SCAP_Stop();
    CAP_DMA->CMAR = (uint32_t)dst;
    CAP_DMA->CNDTR = size;
    CAP_DMA->CCR |= DMA_CCR_EN;             // Enable DMA Channel
    
    TIMEOUT_TIM->ARR = duration - 1;        // Configure max count as timeout
    TIMEOUT_TIM->EGR = TIM_EGR_UG;          // Reset timer and update registers
    TIMEOUT_TIM->SR &= ~TIM_SR_UIF;         // Ignore initial update event

    CAP_TIM->EGR = TIM_EGR_UG;              // Reset timer and update registers
    CAP_TIM->DIER |= TIM_DIER_CC1IE;        // Enable capture interrupt
    CAP_TIM->CR1 |= TIM_CR1_CEN;            // Start capture timer
    CAP_TIM->CCER |= TIM_CCER_CC1E;         // Enable channel 1 capture
}

/**
 * @brief Stop a capture
 * 
 *  */
void SCAP_Stop(void){
    CAP_TIM->CCER &= ~TIM_CCER_CC1E;        // Disable channel 1 capture
    CAP_TIM->CR1 &= ~TIM_CR1_CEN;           // Stop capture timer
    CAP_DMA->CCR &= ~DMA_CCR_EN;            // Disable DMA
}

/**
 * @brief Waits for a capture to end
 * 
 * \retval : number of remaining capture values
 *  */
uint32_t SCAP_Wait(void){
    while(CAP_TIM->CR1 & TIM_CR1_CEN);
    return CAP_DMA->CNDTR;
}

/** 
 * Interrupt Handlers
 * */

CAP_DMA_HANDLER{
    if(DMA1->ISR & CAP_DMA_ISR){
        SCAP_Stop();
        DMA1->IFCR = CAP_DMA_IFCR;
    }
}

// Necessary for TIM2 and TIM15,
// on first capture, handler is called to enable TIM15.
// If using TIM1 master/slave mode can be configured to start TIM15
CAP_TIM_HANDLER{
    uint16_t st = CAP_TIM->SR;
    //CC1IF when read is never set because DMA clears it, however
    // UIF is set on first capture
    if(st & TIM_SR_UIF){
        CAP_TIM->DIER &= ~TIM_DIER_CC1IE;   // Disable update interrupt
        TIMEOUT_TIM->CR1 |= TIM_CR1_CEN;    // Start timeout timer
        LED_ON;
    }
    CAP_TIM->SR ^= st;
}

TIMEOUT_TIM_HANDLER{
    if((TIMEOUT_TIM->SR & TIM_SR_UIF) != 0){
        SCAP_Stop();
        TIMEOUT_TIM->SR &= ~TIM_SR_UIF;     // Clear update event flag
        LED_OFF;
    }
}
#endif