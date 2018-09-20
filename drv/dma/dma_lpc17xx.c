
#include <common.h>
#include <dma.h>

#define DMA_MAX_CHANNELS    8
#define PCGPDMA             (1 << 29)
#define DMA_POWER_ON        LPC_SC->PCONP |= PCGPDMA;

#define DMACCONFIG_E        (1 << 0)
#define DMACCONFIG_M        (1 << 1)
#define DMA_CH0             (1 << 0) 

#define DMA_CONTROL_WIDTH8  0
#define DMA_CONTROL_WIDTH16 1
#define DMA_CONTROL_WIDTH32 2
#define DMA_CONTROL_SI      (1 << 26)   // Source Increment
#define DMA_CONTROL_I       (1 << 31)   // Terminal count interrupt enable
#define DMA_CONTROL_SWIDTH(x) (x << 18)
#define DMA_CONTROL_DWIDTH(x) (x << 21)
#define DMA_CONTROL_MAX_TRANSFER_MASK 0xFFFF

#define DMA_CONFIG_M2M      (0 << 11)
#define DMA_CONFIG_M2P      (1 << 11)
#define DMA_CONFIG_P2M      (2 << 11)
#define DMA_CONFIG_P2P      (3 << 11)
#define DMA_CONFIG_SRC(x)   (x << 1)
#define DMA_CONFIG_DST(x)   (x << 6)

#define DMACBREQ_DAC        7

#define DMA_CON_SSP0_TX     0
#define DMA_CON_SSP0_RX     1
#define DMA_CON_SSP1_TX     2
#define DMA_CON_SSP1_RX     3
#define DMA_CON_ADC         4
#define DMA_CON_I2S0        5
#define DMA_CON_I2S1        6
#define DMA_CON_DAC         7
#define DMA_CON_UART0_TX    8
#define DMA_CON_UART0_RX    9
#define DMA_CON_UART1_TX   10
#define DMA_CON_UART1_RX   11
#define DMA_CON_UART2_TX   12
#define DMA_CON_UART2_RX   13
#define DMA_CON_UART3_TX   14
#define DMA_CON_UART3_RX   15

typedef struct{
    volatile uint32_t src;
    volatile uint32_t dst;
    volatile uint32_t lli;
    volatile uint32_t ctl;
}DmaLinkedListItem;

static uint8_t dma_channels_in_use;
static const LPC_GPDMACH_TypeDef *m_dmachs[8] = {LPC_GPDMACH0, LPC_GPDMACH1, LPC_GPDMACH2, LPC_GPDMACH3, LPC_GPDMACH4, LPC_GPDMACH5, LPC_GPDMACH6, LPC_GPDMACH7};
static DmaLinkedListItem m_lli[DMA_MAX_CHANNELS];
/**
 * @brief get DMA Connection based on address
 * */
uint8_t DMA_GetConnection(uint32_t address){
    switch(address){      
        case 0x40034004:        //AD0GDR
            return DMA_CON_ADC;
        case 0x4008C000:        //DACR
            return DMA_CON_I2S0;
        default: 
            return 0;
    }
}

/**
 * @brief Gets the dma structure correspondent to the dma channel number
 * */
LPC_GPDMACH_TypeDef *DMA_GetChannel(uint8_t num){
   return m_dmachs[num & 7];
}

/**
 * @brief return the first available channel starting
 * from specified parameter. channels with lowest number
 * have the highest priority
 * 
 * @param[in]   from  Start priority 0-7
 * @return      channel number or DMA_MAX_CHANNELS if no channel available
 * */
uint8_t DMA_GetAvailableChannel(uint8_t from){
uint8_t i = from & 7;

    do{
        if((dma_channels_in_use & (1 << i)) == 0){
            dma_channels_in_use |= (1 << i);            
            return i;
        }
    }while((i--) != 0);
    
    return DMA_MAX_CHANNELS;
}

void DMA_Setup(Dmachannel *ch){
LPC_GPDMACH_TypeDef *dmach;
uint32_t control;

    if(ch->gpdma == NULL){        
        ch->number = (void*) DMA_GetAvailableChannel(DMA_MAX_CHANNELS - 1 - ch->priority);
        if(ch->number == DMA_MAX_CHANNELS){
            return;
        }
        ch->gpdma = DMA_GetChannel(ch->number);
    }

    /* Clear any error */
    LPC_GPDMA->DMACIntTCClear = (1 << ch->number);
    LPC_GPDMA->DMACIntErrClr = (1 << ch->number);

    // Get channel HW structure
    dmach = (LPC_GPDMACH_TypeDef*)ch->gpdma;

	dmach->DMACCSrcAddr = ch->src;
	dmach->DMACCDestAddr = ch->dst;   

	/* Configure control, Note transfer size not visible
    if channel is disabled */
	control =   DMA_CONTROL_SI |
                DMA_CONTROL_DWIDTH(DMA_CONTROL_WIDTH16) | 
                DMA_CONTROL_SWIDTH(DMA_CONTROL_WIDTH16) |
                (ch->len & DMA_CONTROL_MAX_TRANSFER_MASK);

    if(ch->circular){
        m_lli[ch->number].src = ch->src;
        m_lli[ch->number].dst = ch->dst;
        m_lli[ch->number].lli = &m_lli[ch->number];
        m_lli[ch->number].ctl = control;
        dmach->DMACCLLI = &m_lli[ch->number];
    }

    dmach->DMACCControl = control; 
    dmach->DMACCConfig = DMA_CONFIG_DST(DMA_CON_DAC) | DMA_CONFIG_SRC(0) | DMA_CONFIG_M2P;
}

void DMA_Init(void){
    dma_channels_in_use = 0;
    DMA_POWER_ON;
    LPC_GPDMA->DMACConfig = DMACCONFIG_E; // Enable DMA
}

void DMA_Start(Dmachannel *ch){
    ((LPC_GPDMACH_TypeDef *)ch->gpdma)->DMACCConfig |= DMACCONFIG_E;
}

void DMA_Stop(Dmachannel *ch){
    ((LPC_GPDMACH_TypeDef *)ch->gpdma)->DMACCConfig &= ~DMACCONFIG_E;
}