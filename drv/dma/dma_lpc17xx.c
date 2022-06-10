#include <stddef.h>
#include "lpc17xx_hal.h"
#include "dma.h"

typedef struct{
    void *src;
    void *dst;
    uint32_t lli;
    uint32_t ctl;
}dmalli_t;

typedef void (*eot_t)(void);

static const void *m_dmachs[8] = {LPC_GPDMACH0, LPC_GPDMACH1, LPC_GPDMACH2, LPC_GPDMACH3, LPC_GPDMACH4, LPC_GPDMACH5, LPC_GPDMACH6, LPC_GPDMACH7};
static dmalli_t m_lli[DMA_MAX_CHANNELS];
static eot_t ch_eot[DMA_MAX_CHANNELS];

/**
 * @brief 
 * 
 * @param dma 
 * @param req   Peripheral Identification, low 4 bit dst, upper 4 bit src for P2P
 */
void DMA_Config(dmatype_t *dma, uint32_t req){
    LPC_GPDMACH_TypeDef *dmach = NULL;
    uint8_t ch_num = DMA_MAX_CHANNELS - 1;

    if(dma->stream == NULL){
        do{
            if((LPC_GPDMA->EnbldChns & (1 << ch_num)) == 0){
                break;
            }
        }while((ch_num--) != 0);
        
        if(ch_num == 255){
            return; // no channels available
        }
    }else{        
        for (ch_num = 0; ch_num < DMA_MAX_CHANNELS; ch_num++){
            if(m_dmachs[ch_num] == dma->stream){
                break;
            }
        }

        if((LPC_GPDMA->EnbldChns & (1 << ch_num))){
            return; // channel in use
        }
    }
    
    dmach = (LPC_GPDMACH_TypeDef*)m_dmachs[ch_num];

    /* Clear any error */
    LPC_GPDMA->IntTCClear = (1 << ch_num);
    LPC_GPDMA->IntErrClr = (1 << ch_num);

	dmach->CSrcAddr = (uint32_t)dma->src;
	dmach->CDestAddr = (uint32_t)dma->dst;   

	/* Configure control, Note transfer size not visible
    if channel is disabled */
	uint32_t control, config;

    control =   DMA_CONTROL_SET_DWIDTH(dma->dsize) | 
                DMA_CONTROL_SET_SWIDTH(dma->ssize) |
                DMA_CONTROL_SET_TRANSFER(dma->len);

    config =    0;    
    
    switch(dma->dir){
        case DMA_DIR_P2P:
            config |=   DMA_CONFIG_SET_TYPE(DMA_CONFIG_P2P) |
                        DMA_CONFIG_SET_SRC_PER(req >> 4) |
                        DMA_CONFIG_SET_DST_PER(req);
            break;

        case DMA_DIR_P2M:
            control |= DMA_CONTROL_DI;
            config |=   DMA_CONFIG_SET_TYPE(DMA_CONFIG_P2M) |
                        DMA_CONFIG_SET_SRC_PER(req) |
                        DMA_CONFIG_SET_DST_PER(0);
            break;

        case DMA_DIR_M2P:
            control |= DMA_CONTROL_SI;
            config |=   DMA_CONFIG_SET_TYPE(DMA_CONFIG_M2P) |
                        DMA_CONFIG_SET_SRC_PER(0) |
                        DMA_CONFIG_SET_DST_PER(req);
            break;

        case DMA_DIR_M2M:
            config |=   DMA_CONFIG_SET_TYPE(DMA_CONFIG_M2M);
            break;
    }

    ch_eot[ch_num] = dma->eot;    
    
    if(dma->single == 0){
        m_lli[ch_num].src = dma->src;
        m_lli[ch_num].dst = dma->dst;
        m_lli[ch_num].lli = (uint32_t)&m_lli[ch_num];
        m_lli[ch_num].ctl = control;
        dmach->CLLI = (uint32_t)&m_lli[ch_num];
    }else{
        if(dma->eot != NULL){
            control |= DMA_CONTROL_I;
            config |= DMA_CONFIG_ITC;
        }
        dmach->CLLI = 0;
    }

    dmach->CControl = control; 
    dmach->CConfig = config;
    dma->stream = dmach;
}

void DMA_Init(dmatype_t *dma){
    PCONP_GPDMA_ENABLE;
    if(!(LPC_GPDMA->Config & DMAC_CONFIG_E)){
        LPC_GPDMA->Config = DMAC_CONFIG_E; // Enable DMA
        NVIC_EnableIRQ(DMA_IRQn);
    }

    dma->len = 0;
}

void DMA_Start(dmatype_t *dma){
    if(dma->len > 0){
        ((LPC_GPDMACH_TypeDef *)dma->stream)->CConfig |= DMA_CONFIG_E;
    }
}

void DMA_Stop(dmatype_t *ch){
    LPC_GPDMACH_TypeDef *stream = (LPC_GPDMACH_TypeDef *)ch->stream;

    #if 0 // forced stop
    ((LPC_GPDMACH_TypeDef *)ch->stream)->CConfig &= ~DMA_CONFIG_E;
    #else
    
    if(!(stream->CConfig & DMA_CONFIG_E)){
        return;
    }

    stream->CConfig |= DMA_CONFIG_H;
    while(stream->CConfig & DMA_CONFIG_A);
    stream->CConfig = 0;
    #endif
}

void DMA_IRQHandler(void){
    for(uint8_t ch = 0, ch_msk = 1; ch < DMA_MAX_CHANNELS; ch++, ch_msk <<= 1){
        if(LPC_GPDMA->IntStat & ch_msk){
            if(LPC_GPDMA->IntTCStat & ch_msk){
                if(ch_eot[ch] != NULL){
                    ch_eot[ch]();
                }
                LPC_GPDMA->IntTCClear = ch_msk;
            }

            if(LPC_GPDMA->IntErrStat & ch_msk){
                LPC_GPDMA->IntErrClr = ch_msk;
            }
        }
    }    
}