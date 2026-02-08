#include <stddef.h>
#include "lpc17xx_hal.h"
#include "dac.h"
#include "dma.h"


static dmatype_t dma_dac;

static void dacInt(void){
    CLEAR_BIT(LPC_DAC->CTRL, CTRL_INT_DMA_REQ | CTRL_DMA_ENA | CTRL_CNT_ENA);
}

/**
 * @brief Power up DAC and enable AOUT pin, also inits private dma structure
 *
 *
 * @param dac structure with dac parameters. Note some fields are reseted
 */
void DAC_Init(dactype_t *dac){
    CLOCK_SetPCLK(PCLK_DAC, PCLK_1);
    /* Select AOUT function for P0.26, this also power up DAC */
    LPC_PINCON->PINSEL1 = (LPC_PINCON->PINSEL1 & ~(3 << 20)) | ( 2 << 20);
    LPC_DAC->CR = DAC_CR_BIAS;

    dac->per = LPC_DAC;
    dac->len = 0;
    dac->loop = 0;

    DMA_Config(&dma_dac, 0); // Check dma config
}

void DAC_DeInit(dactype_t *dac){
    DAC_Stop(dac);
}

void DAC_Config(dactype_t *dac){

    if(!dac->len)
        return;

    dma_dac.src = dac->buf;
    dma_dac.dst = (void*)&LPC_DAC->CR;
    dma_dac.len = dac->len;
    dma_dac.dir = DMA_DIR_M2P;
    dma_dac.ssize = DMA_CONTROL_WIDTH16;
    dma_dac.dsize = DMA_CONTROL_WIDTH16;

    if(dac->loop){
        dma_dac.eot = NULL;
        dma_dac.single = 0;
    }else{
        dma_dac.eot = dacInt;
        dma_dac.single = 1;
    }

    LPC_DAC->CNTVAL = dac->rate;
}

void DAC_Write(dactype_t *dac, uint16_t value){
    (void)dac;
    LPC_DAC->CR = (LPC_DAC->CR & 0x10000) | value; // six lsb bits are discarted
}

void DAC_Stop(dactype_t *dac){
    (void)dac;
    DMA_Cancel(&dma_dac);
    LPC_DAC->CTRL = 0;
}

void DAC_Start(dactype_t *dac){
    (void)dac;
    DMA_Config(&dma_dac, DMA_REQ_DAC);
    DMA_Start(&dma_dac);

    SET_BIT(LPC_DAC->CTRL, CTRL_DMA_ENA | CTRL_CNT_ENA);
}

void DAC_UpdateRate(dactype_t *dac, uint32_t rate){
    dac->rate = rate;
    LPC_DAC->CNTVAL = dac->rate;
}