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
	uint32_t state;
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
    __button.state = BUTTON_NONE;
    __button.htime = BUTTON_DEFAULT_HOLD_TIME;
    __button.scan = scan;
}

bevt_t BUTTON_Read(void){
    uint32_t scanned;

    if( __button.scan == NULL){
        return BUTTON_NONE;
    }

    scanned = __button.scan();

    switch(__button.state){
        case BUTTON_NONE:
            if(scanned){
                __button.state = BUTTON_PRESSED;
            }
            __button.pressed = scanned;
            break;

        case BUTTON_PRESSED:
            if(scanned == 0){
                __button.state = BUTTON_RELEASED;
                break;
            }
            if(scanned == __button.pressed){           // same key still pressed
                __button.state = BUTTON_TIMING;    // start timer
                __button.counter = GetTick();
                break;
            }
            __button.pressed = scanned;                // another key was pressed
            break;                                  // TODO: optionally implement if one key is released

        case BUTTON_TIMING:
            if(scanned == 0){
                __button.state = BUTTON_RELEASED;
                break;
            }
            if(scanned == __button.pressed){
                if(GetTick() - __button.counter > __button.htime){
                    __button.state = BUTTON_HOLD;
                }
                break;
            }
            __button.pressed = scanned;                // another key was pressed
            __button.state = BUTTON_PRESSED;
            break;

        case BUTTON_HOLD:
            if(scanned == 0){
                __button.state = BUTTON_RELEASED;
                break;
            }
            if(scanned == __button.pressed)
                break;
            __button.pressed = scanned;                // another key was pressed
            __button.state = BUTTON_PRESSED;
            break;

        case BUTTON_RELEASED:{
            uint32_t released = __button.pressed ^ scanned;
            __button.last = __button.pressed;
            __button.pressed = released;
            __button.state = BUTTON_NONE;
            break;
        }

        default: break;
    }
    return __button.state;
}

void BUTTON_WaitEvent(bevt_t event){
    do{
        BUTTON_Read();
    }while(__button.state != event);
}

uint32_t BUTTON_Get(void){
	BUTTON_WaitEvent(BUTTON_PRESSED);
	return __button.pressed;
}

bevt_t BUTTON_GetEvents(void){
	return __button.state;
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



