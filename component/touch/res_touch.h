/*======================================================== 	

    ________________________
   |   __________________   |
   |          Y+            |
   | |                    | |
   | |                    | |
   | |                    | |
   | |                    | |
   | |                    | |
   | |                    | |
   | |X+                X-| |
   | |                    | |
   | |                    | |
   | |                    | |
   | |                    | |
   | |                    | |
   | |                    | |
   | |         Y-         | |
   |   __________________   |
   |________________________|


X+ (XL) Touch Panel Left Pin
Y+ (YU) Touch Panel Up Pin
X- (XR) Touch Panel Right Pin
Y- (YD) Touch Panel Down Pin

TP    MCU PINS
X+ -> P0.23(AD0)
Y+ -> P0.24(AD1)
X- -> P0.25(AD2)
Y- -> P0.26(AD3)

-----------------------------------------------------------
# Metodo 1
leitura instavel 
consumo de corrente menor
estavel se n�o for precionado

        Read Y                        Read X
           Y+ (YIN)                     Y+ (vcc)
           |                            | 
           \                            \
           /                            /
X+ _/\/\/\_|_/\/\/\_ X-      X+ _/\/\/\_|_/\/\/\_ X-
(Vcc)      /        (vcc)    (XIN)      /			(gnd)
           \                            \	
           /                            /
           |                            |
           Y- (gnd)                     Y- (vcc)

-----------------------------------------------------------
# Metodo 2
leitura estavel e precisa 
consome mais corrente
muito instavel se n�o for precionado

        Read Y                        Read X
           Y+ (vcc)                     Y+ (XIN)
           |                            | 
           \                            \
           /                            /
X+ _/\/\/\_|_/\/\/\_ X-      X+ _/\/\/\_|_/\/\/\_ X-
(YIN)      /        (open)   (vcc)      /			(gnd)
           \                            \	
           /                            /
           |                            |
           Y- (gnd)                     Y- (open)

==========================================================*/

#ifndef _touch_h_
#define _touch_h_

#include <stdint.h>

#define XpIN (1<<14) // Function bis for P0.23 PINSEL1-15:14	
#define YpIN (1<<16) // Function bis for P0.24 PINSEL1-17:16
#define XnIN (1<<18) // Function bis for P0.25 PINSEL1-19:18
#define YnIN (1<<20) // Function bis for P0.26 PINSEL1-21:20
#define XpAIN 0      // canal analogico 0
#define YpAIN 1      // canal analogico 1
#define XnAIN 2
#define YnAIN 3

#define Xp  (1<<23)
#define Yp  (1<<24)
#define Xn  (1<<25)
#define Yn  (1<<26)

#define setgpio ~(0xFF<<14)	//2bit per GPIO

typedef struct{
        uint16_t x;
        uint16_t y;	
}TouchEvent;

uint8_t TS_Get(TouchEvent *ts);
void TS_Init(void);

#endif
