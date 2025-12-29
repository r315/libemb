/**
* @file		button.c
* @brief	Contains API source code for reading buttons.
*
* @version	1.0
* @date		3 Nov. 2016
* @author	Hugo Reis
**********************************************************************/
#include <stddef.h>
#include "button.h"

#define BUTTON_DEFAULT_HOLD_TIME 2000   //2 seconds

typedef struct{
	uint32_t pressed;
	uint32_t last;
	uint32_t counter;
	uint32_t events;
    uint32_t htime;
    uint32_t (*scan)(void); /* return bitmask with pressed buttons */
}bctrl_t;

extern uint32_t GetTick(void);

static bctrl_t __button;

/**
 * @brief Initialise internal state variables
 *
 * @param ht : time in ms before HOLD state
 */
void BUTTON_Init(uint32_t (*scan)(void))
{
    __button.pressed = 0;
    __button.last = 0;
    __button.events = BUTTON_NONE;
    __button.htime = BUTTON_DEFAULT_HOLD_TIME;
    __button.scan = scan;
}

bevt_t BUTTON_Read(void){
    uint32_t mask;

    if( __button.scan == NULL){
        return BUTTON_NONE;
    }

    mask = __button.scan();

    switch(__button.events){
        case BUTTON_NONE:
            if(mask == 0)
                break;
            __button.pressed = mask;
            __button.events = BUTTON_PRESSED;
            break;

        case BUTTON_PRESSED:
            if(mask == 0){
                __button.events = BUTTON_RELEASED;
                break;
            }
            if(mask == __button.pressed){           // same key still pressed
                __button.events = BUTTON_TIMING;    // start timer
                __button.counter = GetTick();
                break;
            }
            __button.pressed = mask;                // another key was pressed
            break;                                  // TODO: optionally implement if one key is released

        case BUTTON_TIMING:
            if(mask == 0){
                __button.events = BUTTON_RELEASED;
                break;
            }
            if(mask == __button.pressed){
                if(GetTick() - __button.counter > __button.htime){
                    __button.events = BUTTON_HOLD;
                }
                break;
            }
            __button.pressed = mask;                // another key was pressed
            __button.events = BUTTON_PRESSED;
            break;

        case BUTTON_HOLD:
            if(mask == 0){
                __button.events = BUTTON_RELEASED;
                break;
            }
            if(mask == __button.pressed)
                break;
            __button.pressed = mask;                // another key was pressed
            __button.events = BUTTON_PRESSED;
            break;

        case BUTTON_RELEASED:
            __button.last= __button.pressed;
            __button.pressed = 0;
            __button.events = BUTTON_NONE;
            break;

        default: break;
    }
    return __button.events;
}

void BUTTON_WaitEvent(bevt_t event){
 do{
     BUTTON_Read();
 }while(__button.events != event);
}

uint32_t BUTTON_Get(void){
	BUTTON_WaitEvent(BUTTON_PRESSED);
	return __button.pressed;
}

bevt_t BUTTON_GetEvents(void){
	return __button.events;
}

uint32_t BUTTON_GetValue(void){
	return __button.pressed;
}

void BUTTON_SetHoldTime(uint32_t t){
    __button.htime = t;
}

uint32_t BUTTON_Pressed(uint32_t mask){
	return !!(__button.pressed & mask);
}



