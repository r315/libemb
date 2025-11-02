#include <stddef.h>
#include "lpc17xx_hal.h"
#include "dma.h"



typedef void (*eot_t)(void);

static const void *s_dmachs[8] = {LPC_GPDMACH0, LPC_GPDMACH1, LPC_GPDMACH2, LPC_GPDMACH3, LPC_GPDMACH4, LPC_GPDMACH5, LPC_GPDMACH6, LPC_GPDMACH7};
static dmalli_t s_lli[DMA_MAX_CHANNELS];
static eot_t s_eot[DMA_MAX_CHANNELS];


/**
 * @brief
 *
 * @param dma
 * @param req   Peripheral Identification, low 4 bit dst, upper 4 bit src for P2P
 */
uint32_t DMA_Config(dmatype_t *dma, uint32_t req){
    LPC_GPDMACH_TypeDef *dmach = NULL;
    uint8_t ch_num = DMA_MAX_CHANNELS - 1;

    if(!(LPC_SC->PCONP & SC_PCONP_PCGPDMA)){
        LPC_SC->PCONP |= SC_PCONP_PCGPDMA;
        LPC_GPDMA->Config = DMAC_CONFIG_E; // Enable DMA
        NVIC_EnableIRQ(DMA_IRQn);
    }

    if(dma->stream == NULL){
        do{
            // Select a channel starting from the lowest priority one
            if((LPC_GPDMA->EnbldChns & (1 << ch_num)) == 0){
                break;
            }
        }while((ch_num--) != 0);

        if(ch_num == 255){
            return 0; // no channels available
        }
    }else{
        // Channel has been configured
        for (ch_num = 0; ch_num < DMA_MAX_CHANNELS; ch_num++){
            if(s_dmachs[ch_num] == dma->stream){
                break;
            }
        }
        if((LPC_GPDMA->EnbldChns & (1 << ch_num))){
            return 0; // channel in use
        }
    }

    dmach = (LPC_GPDMACH_TypeDef*)s_dmachs[ch_num];

    /* Clear any error */
    LPC_GPDMA->IntTCClear = (1 << ch_num);
    LPC_GPDMA->IntErrClr = (1 << ch_num);

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

    s_eot[ch_num] = dma->eot;

    if(dma->eot != NULL){
        control |= DMA_CONTROL_I;
        config |= DMA_CONFIG_ITC;
    }

    if(dma->single == 0){
        s_lli[ch_num].src = (uint32_t)dma->src;
        s_lli[ch_num].dst = (uint32_t)dma->dst;
        s_lli[ch_num].lli = (uint32_t)&s_lli[ch_num];
        s_lli[ch_num].ctl = control;
        dmach->CLLI = s_lli[ch_num].lli;
    }else{
        dmach->CLLI = 0;
    }

    dmach->CSrcAddr = (uint32_t)dma->src;
	dmach->CDestAddr = (uint32_t)dma->dst;

    dmach->CControl = control;
    dmach->CConfig = config;
    dma->stream = dmach;

    return 1;
}

/**
 * @brief
 *
 * @param dma
 */
void DMA_Start(dmatype_t *dma)
{
    LPC_GPDMACH_TypeDef *stream = (LPC_GPDMACH_TypeDef *)dma->stream;
    uint32_t ctrl = (stream->CControl & 0xFFFFF000) | dma->len;

    if(dma->dir == DMA_DIR_P2P){
        ctrl = ctrl & ~(DMA_CONTROL_SI | DMA_CONTROL_DI);
    }

    stream->CSrcAddr = (uint32_t)dma->src;
    stream->CControl = ctrl;
    stream->CConfig |= DMA_CONFIG_E;
}

/**
 * @brief
 *
 * @param ch
 */
void DMA_Cancel(dmatype_t *ch)
{
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

/**
 * @brief Return number of bytes already transferred into buffer
 * or in other words dma offset within destination
 * @param dma
 * @return
 */
uint32_t DMA_GetTransfers(dmatype_t *dma)
{
    LPC_GPDMACH_TypeDef *stream = (LPC_GPDMACH_TypeDef *)dma->stream;

    return dma->len - (stream->CControl & 0xFFF);
}

/**
 * @brief
 *
 */
void DMA_IRQHandler(void){
    for(uint8_t ch = 0, ch_msk = 1; ch < DMA_MAX_CHANNELS; ch++, ch_msk <<= 1){
        if(LPC_GPDMA->IntStat & ch_msk){
            if(LPC_GPDMA->IntTCStat & ch_msk){
                if(s_eot[ch] != NULL){
                    s_eot[ch]();
                }
                LPC_GPDMA->IntTCClear = ch_msk;
                // DMA stream is disable automatically if DMACCCxLL is NULL
            }

            if(LPC_GPDMA->IntErrStat & ch_msk){
                LPC_GPDMA->IntErrClr = ch_msk;
            }
        }
    }
}