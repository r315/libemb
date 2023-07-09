#include <stddef.h>
#include <stdint.h>
#include "stimer.h"

static simpletimer_t *tlist = NULL;

/**
 * Configures a timer and adds it to internal timer list
*/
void STIMER_Config(simpletimer_t *timer, uint32_t interval, uint32_t (*callback)(simpletimer_t *timer))
{
    if(interval == 0 || timer == NULL || callback == NULL){
        return;
    }

    timer->interval = interval;
    timer->callback = callback;
    timer->countdown = 0;
    timer->next = NULL;

    if(tlist == NULL){
        tlist = timer;
        return;
    }

    simpletimer_t *head = tlist;

    while(head->next != NULL){
        head = head->next;
    }

    head->next = timer;
}

/**
 * Removes timer from list
*/
void STIMER_Remove(simpletimer_t *timer)
{
    simpletimer_t *head = tlist;

    if(head == timer){
        if(head->next != NULL){
            tlist = head->next;
        }else{
            tlist = NULL;
        }
        return;
    }

    while(head){
        if(head->next == timer){
            head->next = timer->next;
            return;
        }        
        head = head->next;
    }
}

/**
 * Starts a timer, if timer in parameter is not
 * on internal list, it will not start
*/
void STIMER_Start(simpletimer_t *timer)
{
    timer->countdown = timer->interval;
}

/**
 * Stops timer 
*/
void STIMER_Stop(simpletimer_t *timer)
{
    timer->countdown = 0;
}

/**
 * Timer handler, this must be called periodically
 * on a fixed quantum usually 1ms
*/
void STIMER_Handler(void)
{
    simpletimer_t *head = tlist;

    while(head){
        if(head->countdown > 0){
            if((--head->countdown) == 0){
                head->countdown = head->callback(head);
            }
        }
        head = head->next;
    }
}