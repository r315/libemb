#include <stddef.h>
#include <stdint.h>
#include "stimer.h"

static simpletimer_t *tlist = NULL;


/**
 * @brief Configures a timer and adds it to internal timer list
 *
 * @param timer         pointer to simpletimer_t
 * @param interval      Expiration interval in handler call quantum, [ms] typical
 * @param callback      Callback function on expiration
 * @return
 */
void STIMER_Config(simpletimer_t *timer, uint32_t interval, uint32_t (*callback)(simpletimer_t *timer))
{
    if(interval == 0 || timer == NULL || callback == NULL){
        return;
    }

    timer->interval = interval;
    timer->callback = callback;
    timer->countdown = 0;

    if(tlist == NULL){
        // Empty list, add head
        timer->next = NULL;
        tlist = timer;
        return;
    }

    simpletimer_t *head = tlist;

    do{
        if(head == timer){
            // timer is already on list, all done
            return;
        }

        if(head->next == NULL){
            head->next = timer;
            return;
        }
        head = head->next;
    }while(1);
}

/**
 * @brief Removes timer from timer linked list
 *
 * @param timer     pointer to simpletimer_t
 * @return
 */
void STIMER_Remove(simpletimer_t *timer)
{
    simpletimer_t *head = tlist;

    if(!timer){
        return;
    }

    // Remove from head
    if(head == timer){
        if(head->next != NULL){
            tlist = head->next;
        }else{
            tlist = NULL;
        }
        return;
    }

    // Go through list
    while(head){
        if(head->next == timer){
            head->next = timer->next;
            return;
        }
        head = head->next;
    }
}

/**
 * @brief Changes the interval of a timer.
 * This affects a running timer on next call of callback if
 * returns STIMER_GetInterval() or STIMER_Start()
 *
 * @param timer     pointer to simpletimer_t
 * @param interval  New expiration interval in handler call quantum, [ms] typical
 */
void STIMER_SetInterval(simpletimer_t *timer, uint32_t interval)
{
    if(!timer){
        return;
    }

    timer->interval = interval;
}

/**
 * @brief Starts a timer, if timer in parameter is not
 * on internal list, it will not start
 *
 * @param timer     pointer to simpletimer_t
 *
*/
void STIMER_Start(simpletimer_t *timer)
{
    if(!timer){
        return;
    }

    timer->countdown = timer->interval;
}

/**
 * @brief Stops timer
 *
 * @param timer     pointer to simpletimer_t
*/
void STIMER_Stop(simpletimer_t *timer)
{
    if(!timer){
        return;
    }
    timer->countdown = 0;
}

/**
 * @brief Simple timer handler, this must be called periodically
 * on a fixed quantum usually 1ms.
 * callback returns next value for interval or 0 to stop timer
 *
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