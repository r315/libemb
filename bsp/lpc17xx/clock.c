
#include <board.h>
#include <clock.h>


#ifdef __USE_SYSTICK
volatile unsigned int _systicks;

//-----------------------------------------------------									   
// SysTick Interrupt Handler (1ms)   
//-----------------------------------------------------
void SysTick_Handler(void){
	_systicks++;
}

void CLOCK_DelayMs(uint32_t ms){
unsigned int ticks = _systicks + ms;
	while(ticks > _systicks);
}

unsigned int CLOCK_GetTicks(void){
	return _systicks;
}

void systickInit(){
	SysTick_Config(SystemCoreClock/1000);
}
#else

void systickInit(){
	LPC_TIM0->TCR = 0;
	LPC_TIM0->MCR = 0;
	LPC_SC->PCLKSEL0 |= (PCLK_1<<PCLK_TIMER0);
	LPC_TIM0->CTCR = 0;
	LPC_TIM0->PR = SystemCoreClock/1000;
	LPC_TIM0->TCR |= 1;
}

unsigned int CLOCK_GetTicks(void){
	return LPC_TIM0->TC;
}

void CLOCK_DelayMs(uint32_t ms){
unsigned int ticks = LPC_TIM0->TC + ms;
	while(ticks > LPC_TIM0->TC);
}
#endif

unsigned int CLOCK_ElapsedTicks(unsigned int ticks){
	return CLOCK_GetTicks() - ticks;
}

unsigned int CLOCK_CalculateCCLK(void){
/* Determine clock frequency according to clock register values             */
  if (((LPC_SC->PLL0STAT >> 24)&3)==3) {/* If PLL0 enabled and connected      */
    switch (LPC_SC->CLKSRCSEL & 0x03) {
      default:
      case 0:                           /* Internal RC oscillator => PLL0     */
      case 3:                           /* Reserved, default to Internal RC   */
        return (IRC_OSC * 
               (((2 * ((LPC_SC->PLL0STAT & 0x7FFF) + 1))) /
               (((LPC_SC->PLL0STAT >> 16) & 0xFF) + 1))   /
               ((LPC_SC->CCLKCFG & 0xFF)+ 1));

      case 1:                           /* Main oscillator => PLL0            */
        return (OSC_CLK * 
               (((2 * ((LPC_SC->PLL0STAT & 0x7FFF) + 1))) /
               (((LPC_SC->PLL0STAT >> 16) & 0xFF) + 1))   /
               ((LPC_SC->CCLKCFG & 0xFF)+ 1));

      case 2:                           /* RTC oscillator => PLL0             */
        return (RTC_CLK * 
               (((2 * ((LPC_SC->PLL0STAT & 0x7FFF) + 1))) /
               (((LPC_SC->PLL0STAT >> 16) & 0xFF) + 1))   /
               ((LPC_SC->CCLKCFG & 0xFF)+ 1));
    }
  } else {
    switch (LPC_SC->CLKSRCSEL & 0x03) {
      default:
      case 0:                           /* Internal RC oscillator => PLL0     */
      case 3:                           /* Reserved, default to Internal RC   */
        return IRC_OSC / ((LPC_SC->CCLKCFG & 0xFF)+ 1);
      case 1:                           /* Main oscillator => PLL0            */
        return OSC_CLK / ((LPC_SC->CCLKCFG & 0xFF)+ 1);
      case 2:                           /* RTC oscillator => PLL0             */
        return RTC_CLK / ((LPC_SC->CCLKCFG & 0xFF)+ 1);
    }
  }
}

__attribute__ ((weak)) void SystemCoreClockUpdate(void){
	SystemCoreClock = CLOCK_CalculateCCLK();
}

void CLOCK_setCCLK(unsigned int targetcclk){
	
	LPC_SC->CLKSRCSEL = 1;                  /* Main oscillator as Clock Source    */

	switch(targetcclk){
		default:
		case 48:
			LPC_SC->CCLKCFG = 6-1;          /* CCLKDivValue */
			LPC_SC->PLL0CFG = (1-1) << 16 | (12-1); /* (NValue-1) << 16 | (MValue-1); */
			break;
		case 72:
			LPC_SC->CCLKCFG = 4-1;
			LPC_SC->PLL0CFG = (2-1) << 16 | (24-1);
			break;
		case 80:
			LPC_SC->CCLKCFG = 5-1;
			LPC_SC->PLL0CFG  = (3-1) << 16 | (20-1);
			break;
		case 100:
			LPC_SC->CCLKCFG = 3-1;
			LPC_SC->PLL0CFG = (2-1) << 16 | (25-1);
			break;
	}
	
	LPC_SC->PLL0CON   = 0x01;               /* PLL0 Enable                        */
  	LPC_SC->PLL0FEED  = 0xAA;
  	LPC_SC->PLL0FEED  = 0x55;
  	while (!(LPC_SC->PLL0STAT & (1<<26)));  /* Wait for PLOCK0                    */				   
  	LPC_SC->PLL0CON   = 0x03;               /* PLL0 Enable & Connect              */
  	LPC_SC->PLL0FEED  = 0xAA;
  	LPC_SC->PLL0FEED  = 0x55;																		   
  	//LPC_SC->PCLKSEL0  = 0xAAAAAAAA;       /* Peripheral Clock Selection CCLK/2, PCLK should be configured on peripheral config */
  	//LPC_SC->PCLKSEL1  = 0xAAAAAAAA;		/* 00=CCLK/4 01=CCLK 10=CCLK/2 11=CCLK/8 */
	LPC_SC->PCONP     = 0x042887DE;         /* Power up default peripherals */
}

/* Configuration of PLL1 for providing 48Mhz to USB sub system from main oscillator*/
void CLOCK_InitUSBCLK(void){
  LPC_SC->PLL1CFG   = 0x23;             /* P = 1, M = 3: Multiplier(MSEL) and divider(NSEL) select */
  LPC_SC->PLL1CON   = 0x01;             /* PLL1 Enable                        */
  LPC_SC->PLL1FEED  = 0xAA;
  LPC_SC->PLL1FEED  = 0x55;
  while (!(LPC_SC->PLL1STAT & (1<<10)));/* Wait for PLOCK1                    */

  LPC_SC->PLL1CON   = 0x03;             /* PLL1 Enable & Connect              */
  LPC_SC->PLL1FEED  = 0xAA;
  LPC_SC->PLL1FEED  = 0x55;
  
  //LPC_SC->USBCLKCFG = 0;               /* Setup USB Clock Divider from PLL0   when not using PLL1*/
}


void CLOCK_Init(unsigned int cclk){

	LPC_SC->SCS       = 0x20;               /* Main Oscillator enable             */
  	while (!(LPC_SC->SCS & 0x40));          /* Wait for Oscillator to be ready    */  
	
	if( cclk < 48 ){
	/* Configuration of the 4Mhz internal osccilator */
	  LPC_SC->CLKSRCSEL = 0;               /* Select Internal oscillator as PLL0 CLK source */ 
	  LPC_SC->CCLKCFG   = 0;               /* NO division of PLLCLK, CCLK = PLLCLK */
	  LPC_SC->PCLKSEL0  = 0;               /* Peripheral Clock (PCLK) = CCLK/4 */
	  LPC_SC->PCLKSEL1  = 0;
	  SystemCoreClock = IRC_OSC;
	}else{
		CLOCK_setCCLK(cclk);
	}
	
	LPC_SC->FLASHCFG  = 0x0000303A;    /* Flash Accelerator Configuration */ 
 
	SystemCoreClock = CLOCK_CalculateCCLK();
	systickInit();
}

int CLOCK_GetCCLK(void){
	if(!SystemCoreClock)
		SystemCoreClock = IRC_OSC;
	return SystemCoreClock;
}

