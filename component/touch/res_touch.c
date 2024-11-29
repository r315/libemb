/*

*/
#include "res_touch.h" 
#include "touch.h"
//#include <statistic.h>

//--------------------------------------------
//
//--------------------------------------------
static void sort(uint32_t *data, uint16_t length)
{
	uint16_t a = length, i;
	uint32_t aux;
     while(a--)
     {
        for(i = 0; i < length - 1; i++)
		{
			if((data[i] < data[i + 1]))
			{
				aux = data[i];
				data[i] = data[i + 1];
				data[i + 1] = aux;				
			}
		}     
    }
}     
//--------------------------------------------
//
//--------------------------------------------     
uint32_t median(uint32_t *data, uint16_t length){
    
    sort(data, length);

    if(length & 1){
        return data[length / 2];	// Return center value
    }

    return (data[(length / 2) - 1] + data[length / 2]) / 2; // return average of central values           
}
//--------------------------------------------
//
//--------------------------------------------
uint32_t average(uint32_t *data, uint16_t length)
{
	uint64_t sum=0;

    for(uint16_t i = 0; i < length; i++)
        sum += data[i];
        
    return sum / length;
}


//---------------------------------------------------*/									   
//
//-----------------------------------------------------			
void TS_Init(void){
	ADC_Init();
	LPC_PINCON->PINMODE_OD0 = Xn | Yn;	// Configure open drain
}
//-----------------------------------------------------
// 	    
//-----------------------------------------------------
static uint16_t TS_Sample(uint8_t ch){
#if 0
 unsigned int n=0; 
 unsigned char i=8;  
 while(i--) 
 	n += ADIN(ch);	  
 return n / 8;
#else
	uint16_t samples[8];  

	for(uint8_t i = 0; i < sizeof(samples); i++){
		samples[i] = ADC_Convert(ch);
	}

	return median(samples, sizeof(samples));
#endif
}
//---------------------------------------------------------------*/
//(t-min)/((max-min)/range)
//-----------------------------------------------------------------
uint16_t mapp(uint16_t v, uint16_t min, uint16_t max, uint16_t range)
{
	max -= min;
	max /= range;		
	return (v-min) / max;
}
//-----------------------------------------------------
//
//-----------------------------------------------------
uint8_t TS_Get(TouchEvent *ts)
{
#if 0 //metodo 1
	// READ  POS X
	LPC_PINCON->PINSEL1 &= setgpio;   // Fun��o GPIO nos pins Xp,Yp,Xn,Yn
	LPC_PINCON->PINSEL1 |= XpIN;      // Fun��o anologica no pino Xp
	LPC_GPIO0->FIODIR   &= ~Xp;       // Direc��o entrada Xp
	LPC_GPIO0->FIODIR   |= Yp|Xn|Yn;  // Direc��o saida	  Yp,Xn,Yn
	LPC_GPIO0->FIOCLR    = Xn;        // Xn = gnd
	LPC_GPIO0->FIOSET    = Yp|Yn;     // Yp = Yn = vcc	  
	ts->x = touchadc(XpAIN);

	// READ  POS Y
	LPC_PINCON->PINSEL1 &= setgpio;   // Fun��o GPIO nos pins Xp,Yp,Xn,Yn
	LPC_PINCON->PINSEL1 |= YpIN;      // Fun��o anologica no pino Yp
	LPC_GPIO0->FIODIR   &= ~Yp;       // Entrada Yp
	LPC_GPIO0->FIODIR   |= Xp|Xn|Yn;  // Saida Xp,Xn,Yn
	LPC_GPIO0->FIOCLR    = Yn;        // Yn = gnd
	LPC_GPIO0->FIOSET    = Xp|Xn;     // Xp = Xn = vcc	  
	ts->y = touchadc(YpAIN);

#else //metodo 2
	#if 1
	// READ  POS X
	LPC_PINCON->PINSEL1 &= setgpio;   // Fun��o GPIO nos pins Xp,Yp,Xn,Yn
	LPC_PINCON->PINSEL1 |= YpIN|YnIN; // Fun��o anologica nos pinos Yp,Yn
	LPC_GPIO0->FIODIR   &= ~(Yp|Yn);  // Direc��o entrada Yp,Yn
	LPC_GPIO0->FIODIR   |= Xp|Xn;     // Direc��o saida	  Xp,Xn
	LPC_GPIO0->FIOCLR    = Xn;        // Xn = gnd
	LPC_GPIO0->FIOSET    = Xp;        // Xp = vcc	  
	ts->x = TS_Sample(YpAIN);
	#endif

	#if 1
	// READ  POS Y
	LPC_PINCON->PINSEL1 &= setgpio;   // Fun��o GPIO nos pins Xp,Yp,Xn,Yn
	LPC_PINCON->PINSEL1 |= XpIN|XnIN; // Fun��o anologica nos pinos Xp,Xn
	LPC_GPIO0->FIODIR   &= ~(Xp|Xn);  // Direc��o entrada Xp,Xn
	LPC_GPIO0->FIODIR   |= Yp|Yn;     // Direc��o saida	  Yp,Yn			 
	LPC_GPIO0->FIOCLR    = Yn;        // Yn = gnd
	LPC_GPIO0->FIOSET    = Yp;        // Yp = vcc	  
	ts->y = TS_Sample(XpAIN);
	#endif

#endif

// ts->x tem de ser testado dpx de ler eixo y
// estado dos pins intrefere com leituras
if(ts->x >3300)
	return 0;

ts->x = mapp(ts->x, 660, 3300,2 40);

// fuck TS avariado.....
// TODO: mapear eixo Y
if(ts->x < 50)
	ts->y = mapp(ts->y,1700,3700,320);
else if(ts->x < 90)
	ts->y = mapp(ts->y,2000,3700,320);
else if(ts->x < 130)
	ts->y = mapp(ts->y,2100,3700,320);
else 
	ts->y = mapp(ts->y,2200,3700,320);

if( ts->x > 0 && ts->x < 240 && ts->y > 0 && ts->y < 320)
{
#ifdef ORIENTATION90
	unsigned int aux;
	aux = ts->x;
	ts->x = ts->y;
	ts->y = 240-aux;
#endif
	return 1;
}
else
	return 0;
}
