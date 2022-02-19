#include <board.h>
#include <dac.h>
#include <dma.h>
#include <string.h>

#define DAC_MAX_VALUE 0x3FF
#define DAC_CFG_PIN         LPC_PINCON->PINSEL1 = (LPC_PINCON->PINSEL1 & ~(3 << 20)) | ( 2 << 20)  // P0.26 AOUT function
#define DAC_WRITE(x)        LPC_DAC->DACR = (x & 0x7FF) << 6 // The 11th bit is DAC_BIAS

#define DACCTRL_CNT_ENA     (1 << 2)
#define DACCTRL_DMA_ENA     (1 << 3)


void DAC_Init(){

    CLOCK_SetPCLK(PCLK_DAC, PCLK_8);
    DAC_CFG_PIN;
    //memset(&dacdma, 0, sizeof(Dmachannel));
    //DMA_Init();
   
}

void DAC_SetRate(uint32_t fq){

}

void DAC_Write(uint16_t value){
    DAC_WRITE(value);
}

void DAC_WriteBufer(uint16_t *buf, uint32_t len, uint16_t rate){
    LPC_DAC->DACCNTVAL = rate;
    LPC_DAC->DACCTRL = 0;
/*
    dacdma.src = (uint32_t)buf;
    dacdma.dst = (uint32_t)&LPC_DAC->DACR;
    dacdma.len = len;
    dacdma.circular = 1;
    dacdma.priority = 7;

    DMA_Setup(&dacdma);   
    DMA_Start(&dacdma);   

    LPC_DAC->DACCTRL = DACCTRL_DMA_ENA | DACCTRL_CNT_ENA;
*/
    //DAC_Stop();
}

void DAC_Stop(void){
    LPC_DAC->DACCTRL = 0;
    //DMA_Stop(&dacdma);
}
