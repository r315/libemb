/**
* @file		button.c
* @brief	Contains API source code for reading buttons.
*     		
* @version	1.0
* @date		3 Nov. 2016
* @author	Hugo Reis
**********************************************************************/
#include <button.h>
#include <clock.h>

#include <lpc1768.h>

static BUTTON_Controller __button;

void BUTTON_Init(int ht){    
    __button.cur  = BUTTON_EMPTY;
    __button.last = BUTTON_EMPTY;
    __button.events = BUTTON_EMPTY;
    __button.htime = ht;

    BUTTON_SetInput(BUTTON_MASK);

/*
#ifdef __LPC17xx_H__
    LPC_GPIO1->FIODIR |= BUTTON_MASK;
#else
    GPIO1->FIODIR |= BUTTON_MASK;
#endif*/
}

int BUTTON_Hit(void){
uint32_t cur;

    cur = BUTTON_Capture();

    switch(__button.events){

        case BUTTON_EMPTY:
            if(cur == BUTTON_EMPTY)
                break;            
            __button.cur = cur;
            __button.events = BUTTON_PRESSED;
            break;

        case BUTTON_PRESSED:
            if(cur == BUTTON_EMPTY){
                __button.events = BUTTON_RELEASED;
                break;
            }
            if(cur == __button.cur){             // same key still pressed
                __button.events = BUTTON_TIMING; // start timer
                __button.counter = CLOCK_GetTicks();
                break;
            }
            __button.cur = cur; // another key was pressed 
            break;              // TODO: optionaly implement if one key is relesed

        case BUTTON_TIMING:
            if(cur == BUTTON_EMPTY){
                __button.events = BUTTON_RELEASED;
                break;
            }            
            if(cur == __button.cur){
                if(CLOCK_ElapsedTicks(__button.counter) > __button.htime){
                    __button.events = BUTTON_HOLD;
                }   
                break;
            }
            __button.cur = cur; // another key was pressed 
            __button.events = BUTTON_PRESSED;
            break;

        case BUTTON_HOLD:
            if(cur == BUTTON_EMPTY){
                __button.events = BUTTON_RELEASED;
            }
            if(cur == __button.cur)
                break;
            __button.cur = cur; // another key was pressed 
            __button.events = BUTTON_PRESSED;
            break;
            
        case BUTTON_RELEASED:
            __button.last= __button.cur;
            __button.cur = BUTTON_EMPTY;
            __button.events = BUTTON_EMPTY;
            break;
            
        default: break;
    }            

#if 0
	
	// check if any buttons pressed
	if(cur == BUTTON_EMPTY){		
		//update lastkey pressed and events
		if(__button.cur != BUTTON_EMPTY){
			__button.last= __button.cur;
			__button.cur = BUTTON_EMPTY;
			__button.events = BUTTON_RELEASED;
		}
		return BUTTON_EMPTY;
	}
	//update lastkey pressed
	__button.last = __button.cur;
	__button.cur = cur;

	// if not the same key pressed, start timer
	if(__button.cur != __button.last){		
		__button.counter = TIMER0_GetValue();
		__button.events = BUTTON_PRESSED;
		return __button.cur;
	}
	
	//check for timeout and activate hold flag
	if(TicksToMs(TIMER0_Elapse(__button.counter)) > __button.htime){
		__button.events = BUTTON_HOLD;
	}
	else
		__button.events = BUTTON_TIMING;
#endif

    return __button.events; //__button.cur;
}

void BUTTON_WaitEvent(int event){
 do{
     BUTTON_Hit();
 }while(__button.events != event);    
}

int BUTTON_Read(void){
	BUTTON_WaitEvent(BUTTON_PRESSED);
	return __button.cur; 
}

int BUTTON_GetEvents(void){	
	return __button.events;
}

int BUTTON_GetValue(void){
	return __button.cur;
}

void BUTTON_SetHoldTime(int t){
    __button.htime = t;
}

char BUTTON_Pressed(int button){
	return (__button.cur & button) != 0 ? 1 : 0;
}



