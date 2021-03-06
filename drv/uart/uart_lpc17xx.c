#include <stdint.h>
#include <stdio.h>
#include <LPC17xx.h>
#include <clock_lpc17xx.h>
#include <uart_lpc17xx.h>

static uart_t *uart0;
static uart_t *uart1;
static uart_t *uart2;
static uart_t *uart3;


//Fractional Divider setting look-up table
static const struct FdrPair _frdivTab[]={
//FR, DivAddVal|MulVal<<4
{1000,0|(1<<4)},
{1067,1|(15<<4)},
{1071,1|(14<<4)},
{1077,1|(13<<4)},
{1083,1|(12<<4)},
{1091,1|(11<<4)},
{1100,1|(10<<4)},
{1111,1|(9<<4)},
{1125,1|(8<<4)},
{1133,2|(15<<4)},
{1143,1|(7<<4)},
{1154,2|(13<<4)},
{1167,1|(6<<4)},
{1182,2|(11<<4)},
{1200,1|(5<<4)},
{1214,3|(14<<4)},
{1222,2|(9<<4)},
{1231,3|(13<<4)},
{1250,1|(4<<4)},
{1267,4|(15<<4)},
{1273,3|(11<<4)},
{1286,2|(7<<4)},
{1300,3|(10<<4)},
{1308,4|(13<<4)},
{1333,1|(3<<4)},
{1357,5|(14<<4)},
{1364,4|(11<<4)},
{1375,3|(8<<4)},
{1385,5|(13<<4)},
{1400,2|(5<<4)},
{1417,5|(12<<4)},
{1429,3|(7<<4)},
{1444,4|(9<<4)},
{1455,5|(11<<4)},
{1462,6|(13<<4)},
{1467,7|(15<<4)},
{1500,1|(2<<4)},
{1533,8|(15<<4)},
{1538,7|(13<<4)},
{1545,6|(11<<4)},
{1556,5|(9<<4)},
{1571,4|(7<<4)},
{1583,7|(12<<4)},
{1600,3|(5<<4)},
{1615,8|(13<<4)},
{1625,5|(8<<4)},
{1636,7|(11<<4)},
{1643,9|(14<<4)},
{1667,2|(3<<4)},
{1692,9|(13<<4)},
{1700,7|(10<<4)},
{1714,5|(7<<4)},
{1727,8|(11<<4)},
{1733,11|(15<<4)},
{1750,3|(4<<4)},
{1769,10|(13<<4)},
{1778,7|(9<<4)},
{1786,11|(14<<4)},
{1800,4|(5<<4)},
{1818,9|(11<<4)},
{1833,5|(6<<4)},
{1846,11|(13<<4)},
{1857,6|(7<<4)},
{1867,13|(15<<4)},
{1875,7|(8<<4)},
{1889,8|(9<<4)},
{1900,9|(10<<4)},
{1909,10|(11<<4)},
{1917,11|(12<<4)},
{1923,12|(13<<4)},
{1929,13|(14<<4)},
{1933,14|(15<<4)}
};

//TODO: Fix bug, add end condition for pointer on cycle do while
unsigned char frdivLookup(struct FdrPair *frdiv, unsigned int baudrate, uint64_t pclk){
unsigned int DLest,FRest;
struct FdrPair *pfrdvtab = (struct FdrPair *)_frdivTab;

	do{		
		DLest = (pclk * 1000)/ (16 * baudrate * (pfrdvtab->fr));
		FRest = (pclk * 1000)/ (16 * baudrate * DLest);
		pfrdvtab += 1;
	}while(FRest < 1000 || FRest > 1900);
	
	frdiv->fr = pfrdvtab->fr;
	frdiv->FdrVal = pfrdvtab->FdrVal;
return DLest & 255;
}

void UART_Cfg(unsigned char number, unsigned int baudrate, uint64_t pclk){
struct FdrPair frdiv;	
unsigned char DLest;
	
	// Turn on power to UART0
	LPC_SC->PCONP |=  PCONP_UART0;
		
	// Turn on UART0 peripheral clock
	CLOCK_SetPCLK(PCLK_UART0, PCLK_4);
	//SC->PCLKSEL0 |=  (0 << PCLK_UART0);	
	
	// Set PINSEL0 so that P0.2 = TXD0, P0.3 = RXD0
	LPC_PINCON->PINSEL0 &= ~0xf0;
	LPC_PINCON->PINSEL0 |= ((1 << 4) | (1 << 6));
	
	LPC_UART0->LCR = 0x83;		// 8 bits, no Parity, 1 Stop bit, DLAB=1
    
	#ifndef DEBUG
	DLest = frdivLookup(&frdiv, baudrate, pclk);	
    LPC_UART0->DLM = DLest >> 8;
    LPC_UART0->DLL = DLest & 0xFF;
	LPC_UART0->FDR = frdiv.FdrVal;
	#else	
    LPC_UART0->DLM = 0; //config for CCLK=4Mhz, 9600bps
    LPC_UART0->DLL = 6;
	LPC_UART0->FDR = 0xC1;
	#endif
	
    LPC_UART0->LCR = 0x03;		// 8 bits, no Parity, 1 Stop bit DLAB = 0
    LPC_UART0->FCR = 0x07;		// Enable and reset TX and RX FIFO
}

char UART_GetChar(void){
char c;
	while( (LPC_UART0->LSR & LSR_RDR) == 0 );  // Nothing received so just block 	
	c = LPC_UART0->RBR; // Read Receiver buffer register
	return c;
}

void UART_SendChar(char c){	
	while( (LPC_UART0->LSR & LSR_THRE) == 0 );	// Block until tx empty	
	LPC_UART0->THR = c;	
}


void UART_Init(Uart *uart, uint8_t number){
LPC_UART0_TypeDef *puart;
struct FdrPair frdiv;	
unsigned char DLest;

    switch(number){
        case 0: 
            puart = LPC_UART0;
            // Turn on power to UART0
	        LPC_SC->PCONP |=  PCONP_UART0;
            // Turn on UART0 peripheral clock
	        CLOCK_SetPCLK(PCLK_UART0, PCLK_4);
	        //SC->PCLKSEL0 |=  (0 << PCLK_UART0);	
	
	        // Set PINSEL0 so that P0.2 = TXD0, P0.3 = RXD0
	        LPC_PINCON->PINSEL0 &= ~0xf0;
	        LPC_PINCON->PINSEL0 |= ((1 << 4) | (1 << 6));            
            break;

        //case 1: puart = LPC_UART1;
        //case 2: puart = LPC_UART2;
        //case 3: puart = LPC_UART3;
        default: return;
    }

    puart->LCR = 0x83;		// 8 bits, 1 Parity, 2 Stop bit, DLAB=1
    DLest = frdivLookup(&frdiv, uart->baudrate, CLOCK_GetPCLK(PCLK_UART0));	
    puart->DLM = DLest >> 8;
    puart->DLL = DLest & 0xFF;
	puart->FDR = frdiv.FdrVal;


    puart->LCR = 0x1F;		// 8 bits, no Parity, 1 Stop bit DLAB = 0
    puart->FCR = 0x07;		// Enable and reset TX and RX FIFO

    uart->dev = puart;
}

void UART_Send(Uart *uart, uint8_t *data, uint32_t len){
LPC_UART0_TypeDef *puart = (LPC_UART0_TypeDef*)(uart->dev);
    while(len--){
        while( (puart->LSR & LSR_THRE) == 0 ){}	// Block until tx empty	
	        puart->THR = *(data++);
    }
}

uint8_t UART_Receive(Uart *uart, uint8_t *data, uint32_t len){
	return 0;
}

void UART_SendDMA(Uart *uart, uint8_t *data, uint32_t len){

}

void UART_ReceiveIT(Uart *uart, uint8_t *data, uint32_t len){

		if(uart->rxcb == NULL){
			return;
		}

		((LPC_UART_TypeDef*)uart->dev)->IER |= IER_RBR;

		switch((uint32_t)uart->dev){
			case (uint32_t)LPC_UART0:
				uart0 = uart;
				//NVIC_SetPriority(UART0_IRQn, ((0x01<<3)|0x01));
				((LPC_UART_TypeDef*)uart->dev)->FCR = 0x07;
				NVIC_EnableIRQ(UART0_IRQn);
				break;

			case (uint32_t)LPC_UART1:
				uart1 = uart;
				NVIC_EnableIRQ(UART1_IRQn);
				break;

			case (uint32_t)LPC_UART2:
				uart2 = uart;
				NVIC_EnableIRQ(UART2_IRQn);
				break;
			
			case (uint32_t)LPC_UART3:
				uart3 = uart;
				NVIC_EnableIRQ(UART3_IRQn);
				break;
		}

		

}

void UARTx_IRQHandler(uart_t *uart){
uint32_t int_status = ((LPC_UART_TypeDef*)uart->dev)->IIR;
	if(!(int_status & IIR_PEND)){
		// Check intid
		switch((int_status>>1) & 7){
			case 0: // Modem interrupt
				break;
			case 1: // THRE Interrupt
				break;
			case 2: // Receive Data Available (RDA)
				//if(uart->rxcb != NULL)
				{					
					uint32_t status;
					while((status = ((LPC_UART_TypeDef*)uart->dev)->LSR) & LSR_RDR){
						uint32_t data = ((LPC_UART_TypeDef*)uart->dev)->RBR;					
						data = ((status & LSR_OE) == 0)? data : data | (LSR_OE << 16); // Put flag on upper 16bits
						uart->rxcb(data);
					}
				}
				break;
			case 3: // Receive Line Status (RLS)
				break;
			case 6:	// Character Time-out Indicator (CTI)
				break;
		}
	}
}

void UART0_IRQHandler(void){
	UARTx_IRQHandler(uart0);
}

void UART1_IRQHandler(void){
	UARTx_IRQHandler(uart1);
}

void UART2_IRQHandler(void){
	UARTx_IRQHandler(uart1);
}

void UART3_IRQHandler(void){
	UARTx_IRQHandler(uart3);
}



