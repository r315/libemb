#include "at32f4xx.h"
#include "at32f4xx_rcc.h"
#include "dma_at32f4xx.h"
#include "dma.h"
#include "i2s.h"

#define SPI_I2SCTRL_I2SMSEL     ((uint16_t)0x0800)
#define SPI_I2SCTRL_I2SEN       ((uint16_t)0x0400)
#define SPI_I2SCTRL_I2SCBN      ((uint16_t)0x0001)
#define SPI_I2SCTRL_I2SCLK_POL  (1 << 3)
#define SPI_I2SCLKP_I2SDIV_DEFAULT  2   // I2SDIV[9:0] must be >1

#define SPI_I2SCTRL_I2SDBN_16   ((0 << 1) | SPI_I2SCTRL_I2SCBN)
#define SPI_I2SCTRL_I2SDBN_24   ((1 << 1) | SPI_I2SCTRL_I2SCBN)
#define SPI_I2SCTRL_I2SDBN_32   ((2 << 1) | SPI_I2SCTRL_I2SCBN)

#define SPI_I2S_DMA_TX          ((uint16_t)0x0002)
#define SPI_I2S_DMA_RX          ((uint16_t)0x0001)

#define I2S_AUDIOFREQ_DEFAULT   ((uint32_t)2)
#define I2S_DIV_VALUE_MAX       ((uint16_t)0x03FF)
#define I2S_DIV_EXT_VALUE_MASK              ((uint16_t)0x0300)
#define I2S_DIV_EXT_VALUE_LSHIFT_OFFSET     ((uint16_t)2)

static dmatype_t i2s1_dma, i2s2_dma;

static void i2s_peripheral_enable(SPI_Type* SPIx, FunctionalState NewState){
    if (NewState != DISABLE) {    
        SPIx->I2SCTRL |= SPI_I2SCTRL_I2SEN;
    }else {
        SPIx->I2SCTRL &= ~SPI_I2SCTRL_I2SEN;
    }
}

static void i2s_peripheral_reset(SPI_Type* SPIx){
    if (SPIx == SPI1){       
        RCC_APB2PeriphResetCmd(RCC_APB2PERIPH_SPI1, ENABLE);
        RCC_APB2PeriphResetCmd(RCC_APB2PERIPH_SPI1, DISABLE);
    }else {
        RCC_APB1PeriphResetCmd(RCC_APB1PERIPH_SPI2, ENABLE);
        RCC_APB1PeriphResetCmd(RCC_APB1PERIPH_SPI2, DISABLE);
    }
}

void i2s_peripheral_dma_enable(SPI_Type* SPIx, uint16_t SPI_I2S_DMAReq, FunctionalState NewState)
{
    if (NewState != DISABLE) {
        SPIx->CTRL2 |= SPI_I2S_DMAReq;
    } else {
        SPIx->CTRL2 &= (uint16_t)~SPI_I2S_DMAReq;
    }
}

void i2s_peripheral_init(SPI_Type* SPIx, i2sbus_t *i2s, uint8_t opersel)
{
    uint16_t tmpctrl, i2sdiv = 2, i2sodd = 0, slotsize = 64;   

    SPIx->I2SCTRL = 0;
    SPIx->I2SCLKP = SPI_I2SCLKP_I2SDIV_DEFAULT;

    switch(i2s->data_size){
        case I2S_DT16_SL16:
        default:
            tmpctrl = 0;
            slotsize = 32;
            break;

        case I2S_DT16_SL32:
            tmpctrl = SPI_I2SCTRL_I2SDBN_16;
            break;

        case I2S_DT24_SL32:
            tmpctrl = SPI_I2SCTRL_I2SDBN_24;
            break;

        case I2S_DT32_SL32:
            tmpctrl = SPI_I2SCTRL_I2SDBN_32;
            break;
    }

    if(i2s->sample_rate == I2S_AUDIOFREQ_DEFAULT) {
        i2sodd = (uint16_t)0;
        i2sdiv = (uint16_t)2;
    } else {      
        RCC_ClockType RCC_Clocks; 
        RCC_GetClocksFreq(&RCC_Clocks);

        uint32_t tmp;
        /* Compute the Real divider depending on the MCLK output state with a floating point */
        if(i2s->mode & I2S_MCLK_OUT) {
            tmp = (uint16_t)(((((RCC_Clocks.AHBCLK_Freq / 256) * 10) / i2s->sample_rate)) + 5);
        } else {
            tmp = (uint16_t)((((RCC_Clocks.AHBCLK_Freq / slotsize) * 10 ) / i2s->sample_rate) + 5);
        }
        /* Remove floating point */
        tmp = tmp / 10;
        /* Check the parity of the divider */
        i2sodd = (uint16_t)(tmp & (uint16_t)0x0001);
        /* Compute the i2sdiv prescaler */
        i2sdiv = (uint16_t)((tmp - i2sodd) / 2);

        /* Test if the divider is 1 or 0 or greater than 1023 */
        if ((i2sdiv < 2) || (i2sdiv > I2S_DIV_VALUE_MAX)) {
            /* Set the default values */
            i2sdiv = 2;
            i2sodd = 0;
        }
        /* Shift bits [9:8] if necessary*/
        else if (i2sdiv & I2S_DIV_EXT_VALUE_MASK) {            
            i2sdiv |= ((i2sdiv & I2S_DIV_EXT_VALUE_MASK) << I2S_DIV_EXT_VALUE_LSHIFT_OFFSET);
            i2sdiv &= ~I2S_DIV_EXT_VALUE_MASK;
        }
    }

    /* Write to SPIx I2SPR register the computed value */
    SPIx->I2SCLKP = (uint16_t)(i2sdiv | (i2sodd << 8) | ((i2s->mode & I2S_MCLK_OUT) ? SPI_I2SCLKP_I2SMCLKOE : 0));

    /* Configure the I2S with the SPI_InitStruct values */
    tmpctrl |= (uint16_t)(SPI_I2SCTRL_I2SMSEL |
                        ((i2s->mode >> 1) & 0x30)      // Set audio protocol
                        //|SPI_I2SCTRL_I2SCLK_POL       // Default idles at 0
    );

    if(opersel & I2S_MASTER_TX){
        tmpctrl |= (1 << 9);      // Operate as master
    }

    if(!(opersel & I2S_EN_TX)){   // flag used as RX/TX selection
        tmpctrl |= (1 << 8);      // Reception mode
    }

    /* Write to SPIx I2SCFGR */
    SPIx->I2SCTRL = tmpctrl;
}

/**
 * @brief 
 * 
 * @param i2s 
 */
static void i2s_init_peripheral(i2sbus_t *i2s)
{   
    if(i2s->mode & I2S_EN_TX) {
        RCC->APB2EN |= RCC_APB2EN_SPI1EN;
        i2s_peripheral_reset(SPI1);
    }

    if(i2s->mode & I2S_EN_RX) {
        RCC->APB1EN |= RCC_APB1EN_SPI2EN;
        i2s_peripheral_reset(SPI2);
    }

    i2s_peripheral_init(SPI1, i2s, I2S_MASTER_TX | I2S_EN_TX);
    i2s_peripheral_init(SPI2, i2s, I2S_MASTER_TX);
}

/**
 * @brief 
 * 
 * @param i2s 
 */
static void i2s_init_dma(i2sbus_t *i2s)
{
    if(i2s->mode & I2S_EN_TX) {
        DMA_Cancel(&i2s1_dma);
        i2s1_dma.src = i2s->txbuffer;
        i2s1_dma.dst = (void*)&SPI1->DT;
        i2s1_dma.len = i2s->tx_buf_len;
        i2s1_dma.ssize = DMA_CCR_MSIZE_32;
        i2s1_dma.dsize = DMA_CCR_MSIZE_32;
        i2s1_dma.dir = DMA_DIR_M2P;
        DMA_Config(&i2s1_dma, DMA1_REQ_I2S1_TX);
        //DMA_INTConfig(DMA1_Channel3, DMA_INT_TC | DMA_INT_HT, ENABLE);
        //NVIC_EnableIRQ(DMA1_Channel3_IRQn);
    }

    if(i2s->mode & I2S_EN_RX) {
        DMA_Cancel(&i2s2_dma);
        i2s2_dma.src = (void*)&SPI2->DT;
        i2s2_dma.dst = i2s->rxbuffer;
        i2s2_dma.len = i2s->rx_buf_len;
        i2s2_dma.ssize = DMA_CCR_MSIZE_32;
        i2s2_dma.dsize = DMA_CCR_MSIZE_32;
        i2s2_dma.dir = DMA_DIR_P2M;
        DMA_Config(&i2s2_dma, DMA1_REQ_I2S2_RX); 
        //DMA_INTConfig(DMA1_Channel4, DMA_INT_TC | DMA_INT_HT, ENABLE);
        //NVIC_EnableIRQ(DMA1_Channel4_IRQn);
    }

}

/**
 * @brief I2S Interface configuration
 * 
 * @param   i2s:    i2sbus structure
 */
void I2S_Config(i2sbus_t *i2s){
    
}

/**
 * @brief  I2S Peripheral init
 * 
 * Pin combinations, initialize at board level
 * 
 *          Default     Remap
 * I2S1_SD   PA_7       PB_5
 * I2S1_WS   PA_4       PA_15
 * I2S1_CK   PA_5       PB_3
 * I2S1_MCK  PB_0       PB_6
 * 
 * I2S2_SD   PB_15      PB_5
 * I2S2_WS   PB_12      PA_15
 * I2S2_CK   PB_13      PB_3
 * I2S2_MCK  PC_6       PC_7
 * 
 * 
 * @param   i2s:    i2sbus structure
 * */
void I2S_Init(i2sbus_t *i2s){
    i2s_init_peripheral(i2s);
    i2s_init_dma(i2s);
}

/**
 * @brief Start I2S interface.
 * 
 * @param i2s 
 */
void I2S_Start(i2sbus_t *i2s){
  
    if(i2s->mode & I2S_EN_TX) {
        i2s_peripheral_dma_enable(SPI1, SPI_I2S_DMA_TX, ENABLE);
        i2s_peripheral_enable(SPI1, ENABLE);
        DMA_Start(&i2s1_dma);
    }

    if(i2s->mode & I2S_EN_RX) {
        i2s_peripheral_dma_enable(SPI2, SPI_I2S_DMA_RX, ENABLE);
        i2s_peripheral_enable(SPI2, ENABLE);
        DMA_Start(&i2s2_dma);
    }
}

/**
 * @brief Stop I2S Interface, TX_SDA and TX_WS are held low only TX_CLK continues to be generated.
 * All DMA transference's related with I2S are cancelled.
 * 
 * @param i2s   : I2S state structure
 */
void I2S_Stop(i2sbus_t *i2s){
    i2s_peripheral_enable(SPI1, DISABLE);
    i2s_peripheral_enable(SPI2, DISABLE);
    //DMA_ChannelEnable(DMA1_Channel3, DISABLE);
    //DMA_ChannelEnable(DMA1_Channel4, DISABLE);
}

/**
 * @brief Mute output audio by holding TX_SDA pin low, 
 * all remaining signals continue to be generated and DMA 
 * transference's continue to be performed.
 * 
 * @param i2s   : I2S state structure
 * @param mute  : 0 - Unmute, otherwise muted
 */
void I2S_Mute(i2sbus_t *i2s, uint8_t mute){
  
}

