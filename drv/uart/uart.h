#ifndef _uart_h_
#define _uart_h_
#include <stdint.h>

#define PCUART0_POWERON (1 << 3)

#define PCLK_UART0 6
#define PCLK_UART0_MASK (3 << 6)

#define IER_RBR		0x01
#define IER_THRE	0x02
#define IER_RLS		0x04

#define IIR_PEND	0x01
#define IIR_RLS		0x03
#define IIR_RDA		0x02
#define IIR_CTI		0x06
#define IIR_THRE	0x01

#define LSR_RDR		0x01
#define LSR_OE		0x02
#define LSR_PE		0x04
#define LSR_FE		0x08
#define LSR_BI		0x10
#define LSR_THRE	0x20
#define LSR_TEMT	0x40
#define LSR_RXFE	0x80


struct FdrPair{
	unsigned short fr;
	unsigned char FdrVal; //(MulVal<<4) | MulVal	
};

typedef struct{
	void (*xputc)(char);		//for use on xstdout
	char (*xgetchar)(void);
}Uart;

 /**
 * @breif returns a reference for using on stdout
 *        UART_Init Must be called before!!
 **/
void *UART_GetReference(void);

void UART_Init(unsigned char number, unsigned int baudrate, uint64_t pclk);
void UART_SendChar(char c);
char UART_GetChar(void);

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



#endif
