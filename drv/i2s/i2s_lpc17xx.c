#include "board.h"
#include "i2s.h"

/**
 * Signals      PINS_1      PIN_2
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
    GPIO_Function(P0_4, P0_4_I2SRX_CLK); \
    GPIO_Function(P0_5, P0_5_I2SRX_WS);  \
    GPIO_Function(P0_6, P0_6_I2SRX_SDA);

#define I2S_PINS_RX2 \
    GPIO_Function(P0_23, P0_23_I2SRX_CLK); \
    GPIO_Function(P0_24, P0_24_I2SRX_WS);  \
    GPIO_Function(P0_25, P0_25_I2SRX_SDA);

#define I2S_PINS_TX1 \
    GPIO_Function(P0_7, P0_7_I2STX_CLK); \
    GPIO_Function(P0_8, P0_8_I2STX_WS);  \
    GPIO_Function(P0_9, P0_9_I2STX_SDA);

#define I2S_PINS_TX2 \
    GPIO_Function(P2_11, P2_11_I2STX_CLK); \
    GPIO_Function(P2_12, P2_12_I2STX_WS);  \
    GPIO_Function(P2_13, P2_13_I2STX_SDA);

/* USB RAM is used for GPDMA operation. */
//#define DMA_SRC			0x7FD00000	
//#define DMA_DST			0x7FD01000		
//#define DMA_I2S_TX_FIFO	0xE0088008
//#define	DMA_I2S_RX_FIFO	0xE008800C

#define DMA_SRC			0x20080000	
#define DMA_DST			0x20081000
#define DMA_I2S_TX_FIFO	0x400A8008
#define	DMA_I2S_RX_FIFO	0x400A800C
 
#define DMA_SIZE		0x200

/* DMA mode */
#define M2M				0x00
#define M2P				0x01
#define P2M				0x02
#define P2P				0x03

#define I2S_DMA_ENABLED		1

#define BUFSIZE				0x200
#define RXFIFO_EMPTY		0
#define TXFIFO_FULL			8

/**
 * @brief Configure MCLK according bit rate.
 * Bit rate is given by sample rate * number of bits of one sample * number of channels.
 * MCLK is given by multiplying TX_CLK with N (1 to 64).
 * 
 * Find an MCLK value in such way that is integer divisible to obtain TX_CLK and
 * a result of PCLK by fraction.
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
    uint32_t pclk = CLOCK_GetPCLK(PCLK_I2S);
    LPC_I2S_TypeDef *i2sx = (LPC_I2S_TypeDef*)i2s->regs;

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
	 * 		I2S_MCLK = Freq * channel*wordwidth * (I2STXBITRATE+1);
	 * So: (X/Y) = (Freq * channel*wordwidth * (I2STXBITRATE+1))*2/PCLK_I2S
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
		
        if(dif>0x8000){
            err = 0x10000-dif;
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

    i2sx->I2STXBITRATE = N-1;
    i2sx->I2SRXBITRATE = N-1;
    i2sx->I2STXRATE = y_divide | (x_divide << 8);

}

/**
 * @brief I2S Configuration
 * 
 * @param   i2s:    Configuration structure
 * */
void I2S_Config(i2sbus_t *i2s){
    uint32_t value = I2S_DAO_STOP | I2S_DAO_RESET;
    
    SET_BIT(LPC_I2S->I2SDAO, I2S_DAO_RESET | I2S_DAO_STOP);
    SET_BIT(LPC_I2S->I2SDAI, I2S_DAI_RESET | I2S_DAI_STOP);
    
    clock_config(i2s);

    switch(i2s->data_size){
        case 8:
            value |= I2S_DAO_WIDTH_8B |
                    (7 << 6);  /* Bits per slot 1 to 64*/
            break;

        default:
        case 16:
            value |= I2S_DAO_WIDTH_16B | (15 << 6);
            break;

        case 32:
            value |= I2S_DAO_WIDTH_32B | (31 << 6);
            break;
    }

    if(i2s->channels == 1){
        value |= I2S_DAO_MONO;
    }  

    /* Configure transmitter */
    if(!(i2s->mode & I2S_TX_MASTER)){
        LPC_I2S->I2SDAO = value | I2S_DAO_WS_SEL; /* Slave */
    }else{
        LPC_I2S->I2SDAO = value;
    }

    if(i2s->mute){
        SET_BIT(LPC_I2S->I2SDAO, I2S_DAO_MUTE);
    }

    /* Configure same parameters for receiver */
    if(!(i2s->mode & I2S_TX_MASTER)){
        LPC_I2S->I2SDAI = value | I2S_DAI_WS_SEL;
    }else{
        LPC_I2S->I2SDAI = value;
    }
}

/**
 * @brief I2S initialization and configuration.
 *      PINS are configured according bus number
 * 
 * @param   i2s:    i2sbus structure for initialization
 * */
void I2S_Init(i2sbus_t *i2s){
    PCONP_I2S_ENABLE();

    i2s->regs = LPC_I2S;
        
    I2S_Config(i2s);

    i2s->txbuffer = (uint32_t *)(DMA_SRC); 
    i2s->rxbuffer = (uint32_t *)(DMA_DST);

    i2s->wridx = 0;
    i2s->rdidx = 0;

    switch(i2s->bus){
        case I2S_BUS0:
            I2S_PINS_RX1 
            I2S_PINS_TX1
            break;

        case I2S_BUS1:
            I2S_PINS_RX2
            I2S_PINS_TX1
            break;

        case I2S_BUS2:
            I2S_PINS_RX1 
            I2S_PINS_TX2
            break;

        case I2S_BUS3:
            I2S_PINS_RX2 
            I2S_PINS_TX2
            break;

        default:
            return;
    }

    GPIO_Function(P4_28, P4_28_RX_MCLK);
    GPIO_Function(P4_29, P4_29_TX_MCLK);

    LPC_I2S->I2STXMODE = I2S_TXMODE_TXMCENA; /* Enable MCLK output */
    LPC_I2S->I2SRXMODE = I2S_RXMODE_RXMCENA;


    //LPC_I2S->I2STXRATE = 0x241;
    //LPC_I2S->I2SRXRATE = 0x241;
    I2S_Stop();

    NVIC_EnableIRQ(I2S_IRQn);
}

void I2S_Start( void ){
    /* Audio output is the master, audio input is the slave, */
    /* 16 bit data, stereo, master mode, not mute. */
    CLEAR_BIT(LPC_I2S->I2SDAO, I2S_DAO_RESET | I2S_DAO_STOP);
    /* 16 bit data, stereo, reset, slave mode, not mute. */
    CLEAR_BIT(LPC_I2S->I2SDAI, I2S_DAI_RESET | I2S_DAI_STOP);
}

void I2S_Stop(void){
    /* Stop the I2S to start. Audio output is master, audio input is the slave. */
    /* 16 bit data, set STOP and RESET bits to reset the channels */
    SET_BIT(LPC_I2S->I2SDAO, I2S_DAO_RESET | I2S_DAO_STOP);
    /* Switch to master mode, TX channel, no mute */
    CLEAR_BIT(LPC_I2S->I2SDAO, I2S_DAO_MUTE | I2S_DAO_WS_SEL);  /* Master */
    /* 16 bit data, set STOP and RESET bits to reset the channels */
    SET_BIT(LPC_I2S->I2SDAI, I2S_DAI_RESET | I2S_DAI_STOP);     /* Slave */
}

void I2S_DMA_IRQHandler(i2sbus_t *i2s){
    uint32_t RxCount, WrIndex;
    if ( LPC_I2S->I2SSTATE & I2S_STATE_IRQ ){
        RxCount = (LPC_I2S->I2SSTATE >> 8) & 0x0F;
        WrIndex = i2s->wridx;
        while ( RxCount > 0 ){
            i2s->rxbuffer[WrIndex++] = LPC_I2S->I2SRXFIFO;            
            if ( WrIndex == BUFSIZE ){
               WrIndex = 0;
            }
            RxCount--;
        }
        i2s->wridx = WrIndex;
    }
}