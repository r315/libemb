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

volatile uint8_t *I2STXBuffer = (uint8_t *)(DMA_SRC); 
volatile uint8_t *I2SRXBuffer = (uint8_t *)(DMA_DST);
volatile uint32_t I2SReadLength = 0;
volatile uint32_t I2SWriteLength = 0;
volatile uint32_t I2SRXDone = 0, I2STXDone = 0;

static void clock_config(i2sbus_t *i2s){
    CLOCK_SetPCLK(PCLK_I2S, PCLK_4);
    //Y divider
    //X divider
    //bitrate MCLK divider

    LPC_I2S->I2STXRATE = 0x241;
    LPC_I2S->I2SRXRATE = 0x241;

}

/**
 * @brief I2S initialization, bus member selects pins to be used
 * 
 * @param   i2s:    i2sbus structure for initialization
 * */
void I2S_Init(i2sbus_t *i2s){

    PCONP_I2S_ENABLE();

    clock_config(i2s);

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

    //GPIO_Function(P4_28, P4_28_RX_MCLK);
    //GPIO_Function(P4_29, P4_29_TX_MCLK);

    //LPC_I2S->I2STXMODE = I2S_TXMODE_TXMCENA;
    //LPC_I2S->I2SRXMODE = I2S_RXMODE_RXMCENA;

    NVIC_EnableIRQ(I2S_IRQn);
// DMA
    //PCONP_GPDMA_ENABLE();
    //LPC_GPDMA->DMACIntTCClear = 0x03;
    //LPC_GPDMA->DMACIntErrClr = 0x03;

    I2S_Stop();
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

void I2S_DMA_IRQHandler(i2sbus_t *spidev){
uint32_t RxCount = 0;

  if ( LPC_I2S->I2SSTATE & 0x01 )
  {
	RxCount = (LPC_I2S->I2SSTATE >> 8) & 0xFF;
	if ( (RxCount != RXFIFO_EMPTY) && !I2SRXDone )
	{
	  while ( RxCount > 0 )
	  {
		if ( I2SReadLength == BUFSIZE )
		{
		  LPC_I2S->I2SDAI |= ((0x01 << 3) | (0x01 << 4));
		  LPC_I2S->I2SIRQ &= ~(0x01 << 0);	/* Disable RX */	
		  I2SRXDone = 1;
		  break;
		}
		else
		{
		  I2SRXBuffer[I2SReadLength++] = LPC_I2S->I2SRXFIFO;
		}
		RxCount--;
	  }
	}
  }
  return;
}