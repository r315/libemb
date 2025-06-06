#include "lpc17xx_hal.h"
#include "i2s.h"
#include "dma.h"

/**
 * Signals      PIN_1      PIN_2
 *
 * RX_CLK       P0.4        P0.23
 * RX_WS        P0.5        P0.24
 * RX_SDA       P0.6        P0.25
 * RX_MCLK      P4.28
 *
 * TX_CLK       P0.7        P2.11
 * TX_WS        P0.8        P2.12
 * TX_SDA       P0.9        P2.13
 * TX_MCLK      P4.29
 * */

#define I2S_PINS_RX1 \
    LPC_PINCON->PINSEL0 = (LPC_PINCON->PINSEL0 & ~((3 << 12) | (3 << 10) | (3 << 8))) | (( 1 << 12) | ( 1 << 10) | ( 1 << 8))

    //GPIO_Function(P0_4, P0_4_I2SRX_CLK);
    //GPIO_Function(P0_5, P0_5_I2SRX_WS);
    //GPIO_Function(P0_6, P0_6_I2SRX_SDA);

#define I2S_PINS_RX2 \
    LPC_PINCON->PINSEL1 = (LPC_PINCON->PINSEL1 & ~((3 << 18) | (3 << 16) | (3 << 14))) | (( 2 << 18) | ( 2 << 16) | ( 2 << 14))

    //GPIO_Function(P0_23, P0_23_I2SRX_CLK);
    //GPIO_Function(P0_24, P0_24_I2SRX_WS);
    //GPIO_Function(P0_25, P0_25_I2SRX_SDA);

#define I2S_PINS_TX1 \
    LPC_PINCON->PINSEL0 = (LPC_PINCON->PINSEL0 & ~((3 << 18) | (3 << 16) | (3 << 14))) | (( 1 << 18) | ( 1 << 16) | ( 1 << 14))

    //GPIO_Function(P0_7, P0_7_I2STX_CLK);
    //GPIO_Function(P0_8, P0_8_I2STX_WS);
    //GPIO_Function(P0_9, P0_9_I2STX_SDA);

#define I2S_PINS_TX2 \
    LPC_PINCON->PINSEL4 = LPC_PINCON->PINSEL4 | ( 3 << 26) | ( 3 << 24) | ( 3 << 22)

    //GPIO_Function(P2_11, P2_11_I2STX_CLK);
    //GPIO_Function(P2_12, P2_12_I2STX_WS);
    //GPIO_Function(P2_13, P2_13_I2STX_SDA);
#ifndef I2S_NO_DMA
static dmatype_t s_i2s_tx_dma;
static dmalli_t s_lli;
void I2S_DMAHandler(void);
#endif

static i2sCallback s_txCallback;
static i2sCallback s_rxCallback;

/**
 * @brief Configure MCLK according configuration.
 *
 *                                        +--------------> TX_MCLK
 *                                        |
 * PCLK --->[Frac divider (X/Y)]--->[/2]--+->[/N 1-64]---> TX_CLK (Bit clock)
 *
 * @param   i2s:    Configuration structure
 * */
static void clock_config(i2sbus_t *i2s){
    uint32_t x, y, N;
    uint16_t err, ErrorOptimal = 0xFFFF;
    uint16_t x_divide, y_divide, dif;
    uint32_t pclk;
    LPC_I2S_TypeDef *i2sx = (LPC_I2S_TypeDef*)i2s->regs;

    pclk = CLOCK_GetPCLK(PCLK_I2S);

    /**
     * Source: Marlin 3D Printer Firmware
     *
     * Calculate X and Y divider
	 * The MCLK rate for the I2S transmitter is determined by the value
	 * in the I2STXRATE/I2SRXRATE register. The required I2STXRATE/I2SRXRATE
	 * setting depends on the desired audio sample rate desired, the format
	 * (stereo/mono) used, and the data size.
	 * The formula is:
	 * 		I2S_MCLK = PCLK_I2S * (X/Y) / 2
     * In that, Y must be greater than or equal to X. X should divides evenly
     * into Y.
	 * We have:
	 * 		I2S_MCLK = Freq * channel * wordwidth * (I2STXBITRATE+1);
	 * So: (X/Y) = I2S_MCLK * 2 / PCLK_I2S
	 * We use a loop function to chose the most suitable X,Y value
	 */

    /* divider is a fixed point number with 16 fractional bits */
    uint64_t divider = (((uint64_t)i2s->sample_rate * i2s->channels * i2s->data_size * 2) << 16) / pclk;

	/* find N that make x/y <= 1 -> divider <= 2^16 */
	for(N = 64; N > 0; N--){
		if((divider * N) < ( 1 << 16))
            break;
	}

	if(N == 0)
        return; /* Error */

	divider = divider * N;

	for (y = 255; y > 0; y--) {
		x = y * divider;

        if(x & (0xFF000000)){
            continue;
        }

        dif = x & 0xFFFF;

        if(dif > 0x8000){
            err = 0x10000 - dif;
        }else{
            err = dif;
        }

		if (err == 0){
			y_divide = y;
			break;
		}else if (err < ErrorOptimal){
			ErrorOptimal = err;
			y_divide = y;
		}
	}

    x_divide = ((uint64_t)y_divide * i2s->sample_rate *(i2s->channels * i2s->data_size)* N * 2) / pclk;

	if(x_divide >= 256) x_divide = 0xFF;
	if(x_divide == 0) x_divide = 1;

    i2sx->TXRATE = y_divide | (x_divide << 8);

    if(i2s->mode & I2S_EN_TX)
        i2sx->TXBITRATE = N - 1;

    if(i2s->mode & I2S_EN_RX)
        i2sx->RXBITRATE = N - 1;
}

/**
 * @brief I2S Interface configuration
 *
 * @param   i2s:    i2sbus structure
 */
void I2S_Config(i2sbus_t *i2s){
    uint32_t value = DAO_STOP | DAO_RESET;

    clock_config(i2s);

    /* Bits per slot 1 to 64*/
    switch(i2s->data_size){
        //case 8: value |= DAO_WIDTH_8B | (7 << 6); break;

        default:
        case I2S_DT16_SL16:
            value |= DAO_WIDTH_16B | (15 << 6);
            break;

        case I2S_DT32_SL32:
            value |= DAO_WIDTH_32B | (31 << 6);
            break;
    }

    if(i2s->mode & I2S_EN_RX){
        if(!(i2s->mode & I2S_MASTER_RX)){
            value |= DAI_WS_SEL;
        }

        if(i2s->channels == 1){
            value |= DAI_MONO;
        }

        LPC_I2S->DAI = value;
    }

    if(i2s->mode & I2S_EN_TX){
        if(!(i2s->mode & I2S_MASTER_TX)){
            value |= DAO_WS_SEL; /* Slave */
        }

        if(i2s->channels == 1){
            value |= DAO_MONO;
        }

        if(i2s->mute){
            value |= DAO_MUTE;
        }

        LPC_I2S->DAO = value;
    }
}

/**
 * @brief I2S Controller and IO PINS initialization and configuration.
 * DMA is also configured to perform 32bit transfers to TX_FIFO
 *
 * @param   i2s:    i2sbus structure
 * */
void I2S_Init(i2sbus_t *i2s){
    PCONP_I2S_ENABLE;

    CLOCK_SetPCLK(PCLK_I2S, PCLK_4);

    i2s->regs = LPC_I2S;

    // Reset I2C Controller
    LPC_I2S->DAO = 0x87E1;
    LPC_I2S->DAI = 0x07E1;
    for (uint32_t i = 0, *ptr = (uint32_t*)&LPC_I2S->DMA1; i < 9; i++){
        *ptr++ = 0;
    }

    I2S_Config(i2s);

    // BUS defines used pins
    switch(i2s->bus){
        case I2S_BUS0:
            if(i2s->mode & I2S_EN_RX) {I2S_PINS_RX1;}
            if(i2s->mode & I2S_EN_TX) {I2S_PINS_TX1;}
            break;

        case I2S_BUS1:
            if(i2s->mode & I2S_EN_RX) {I2S_PINS_RX2;}
            if(i2s->mode & I2S_EN_TX) {I2S_PINS_TX1;}
            break;

        case I2S_BUS2:
            if(i2s->mode & I2S_EN_RX) {I2S_PINS_RX1;}
            if(i2s->mode & I2S_EN_TX) {I2S_PINS_TX2;}
            break;

        case I2S_BUS3:
            if(i2s->mode & I2S_EN_RX) {I2S_PINS_RX2;}
            if(i2s->mode & I2S_EN_TX) {I2S_PINS_TX2;}
            break;

        default:
            return;
    }

    if(i2s->mode & I2S_MCLK_OUT){
        if(i2s->mode & I2S_EN_TX){
            LPC_I2S->TXMODE = TXMODE_TXMCENA; /* Enable MCLK output */
            LPC_PINCON->PINSEL9 = (LPC_PINCON->PINSEL9 & (3 << 26)) | ( 1 << 26);
        }

        if(i2s->mode & I2S_EN_RX){
            LPC_I2S->RXMODE = RXMODE_RXMCENA;
            LPC_PINCON->PINSEL9 = (LPC_PINCON->PINSEL9 & (3 << 24)) | ( 1 << 24);
        }
    }

#ifndef I2S_NO_DMA
    s_i2s_tx_dma.dir = DMA_DIR_M2P;
    s_i2s_tx_dma.dst = (void*)&LPC_I2S->TXFIFO;
    s_i2s_tx_dma.dsize = DMA_CONTROL_WIDTH32;
    s_i2s_tx_dma.ssize = DMA_CONTROL_WIDTH32;
    DMA_Config(&s_i2s_tx_dma, 0); // Check dma config
    i2s->dma = &s_i2s_tx_dma; // Not sure if useful..
#endif

}

/**
 * @brief Start I2S interface.
 *
 * @param i2s
 */
void I2S_Start(i2sbus_t *i2s){

#ifndef I2S_NO_DMA
    if(i2s->mode & I2S_EN_RX) {
        s_txCallback = i2s->txcp;

        s_i2s_tx_dma.src = i2s->txbuffer;
        s_i2s_tx_dma.len = i2s->tx_buf_len / 2; // divide buffer into two lists
        s_i2s_tx_dma.eot = I2S_DMAHandler;
        DMA_Config(&s_i2s_tx_dma, DMA_REQ_PER_I2S_CH0);

        // get configured lli then reconfigure it to use two linked lists
        dmalli_t *cfg_lli = (dmalli_t*)((LPC_GPDMACH_TypeDef *)s_i2s_tx_dma.stream)->CLLI;

        s_lli.ctl = cfg_lli->ctl;
        s_lli.dst = cfg_lli->dst;
        cfg_lli->lli = (uint32_t)&s_lli;
        s_lli.lli = (uint32_t)cfg_lli;

        cfg_lli->src = (uint32_t)i2s->txbuffer;   // first half
        s_lli.src = (uint32_t)i2s->txbuffer + (s_i2s_tx_dma.len << 2); // second half

        LPC_I2S->DAO = LPC_I2S->DAO & ~(DAO_RESET | DAO_STOP);
        LPC_I2S->DMA1 = I2S_DMA_TX_EN | I2S_DMA_TX_DEPTH2;
        DMA_Start(&s_i2s_tx_dma);
    }

    if(i2s->mode & I2S_EN_RX) {
        LPC_I2S->DAI = LPC_I2S->DAI & ~(DAI_RESET | DAI_STOP);
        LPC_I2S->DMA2 = I2S_DMA_RX_EN | I2S_DMA_RX_DEPTH2;
        s_rxCallback = i2s->rxcp;
    }

#else
    uint32_t irq = 0;
    if(i2s->mode & I2S_EN_RX) {
        LPC_I2S->DAI = LPC_I2S->DAI & ~(DAI_RESET | DAI_STOP);
        irq |= IRQ_RX_IRQ_EN | ((TXFIFO_SIZE - 6) << IRQ_RX_DEPTH_POS);
    }else{
        irq |= IRQ_RX_DEPTH_MSK;
    }

    if(i2s->mode & I2S_EN_TX) {
        LPC_I2S->DAO = LPC_I2S->DAO & ~(DAO_RESET | DAO_STOP);
        irq |= IRQ_TX_IRQ_EN | ((TXFIFO_SIZE - 6) << IRQ_TX_DEPTH_POS);
    }else{
        irq |= IRQ_TX_DEPTH_MSK;
    }
    LPC_I2S->IRQ = irq;
#endif

    if(s_txCallback || s_rxCallback){
        NVIC_EnableIRQ(I2S_IRQn);
    }
}

/**
 * @brief Stop I2S Interface, TX_SDA and TX_WS are held low only TX_CLK continues to be generated.
 * All DMA transference's related with I2S are cancelled.
 *
 * @param i2s   : I2S state structure
 */
void I2S_Stop(i2sbus_t *i2s){

    if(i2s->mode & I2S_EN_RX) {
        LPC_I2S->IRQ &= ~IRQ_RX_IRQ_EN;
        LPC_I2S->DAI = LPC_I2S->DAI | (DAI_RESET | DAI_STOP);
    }

    if(i2s->mode & I2S_EN_TX) {
        LPC_I2S->IRQ &= ~IRQ_TX_IRQ_EN;
        LPC_I2S->DAO = LPC_I2S->DAO | (DAO_RESET | DAO_STOP);
        #ifndef I2S_NO_DMA
        DMA_Cancel(&s_i2s_tx_dma);
        #endif
    }
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
    if(i2s->mode & I2S_EN_TX) {
        if(mute){
            LPC_I2S->DAO = LPC_I2S->DAO | DAO_MUTE;
        }else{
            LPC_I2S->DAO = LPC_I2S->DAO & ~DAO_MUTE;
        }
        i2s->mute = mute;
    }
}

/**
 * @brief Interrupt handlers
 *
 *
 */
#if I2S_NO_DMA
void I2S_IRQHandler(void){
    uint32_t State, Count;

    if ( LPC_I2S->STATE & STATE_IRQ ){
        State = LPC_I2S->STATE;
#if 0
        Count = I2S_RXFIFO_SIZE - (State >> I2SSTATE_RX_LEVEL_pos) & 0xF;

        if(Count){
            Index = i2s->wridx;
            len = i2s->buf_len;
            while ( Count > 0 ){
                i2s->rxbuffer[Index++] = LPC_I2S->RXFIFO;
                if (Index == len){
                    Index = 0;
                }
                Count--;
            }
            i2s->wridx = Index;
        }
#endif
        Count =  TXFIFO_SIZE - ((State >> STATE_TX_LEVEL_pos) & 0xF);
        if(Count){
            s_txCallback((uint32_t*)&LPC_I2S->TXFIFO, Count);
        }
    }
}
#else
void I2S_DMAHandler(void){
    if(s_txCallback){
        // At this point DMA Controller hasn't moved yet to next LLI,
        // src buffer just finished to be sent to i2s controller
        dmalli_t *lli = (dmalli_t*)((LPC_GPDMACH_TypeDef *)s_i2s_tx_dma.stream)->CLLI;
        s_txCallback((uint32_t*)lli->src, s_i2s_tx_dma.len);
    }
}
#endif