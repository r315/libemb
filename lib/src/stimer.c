#include <stddef.h>
#include <stdint.h>
#include "stimer.h"

static stimer_t *tlist = NULL;
static uint32_t last_tick = 0;

/**
 * @brief Configures a timer and adds it to internal timer list
 *
 * @param timer         pointer to stimer_t
 * @param interval      Expiration interval in handler call quantum, [ms] typical
 * @param callback      Callback function on expiration
 * @return
 */
void STIMER_Config(stimer_t *timer)
{
    if(timer == NULL || timer->interval == 0  || timer->callback == NULL){
        return;
    }

    timer->count = 0;

    if(tlist == NULL){
        // Empty list, add head
        timer->next = NULL;
        tlist = timer;
        return;
    }

    stimer_t *head = tlist;

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
 * @param timer     pointer to stimer_t
 * @return
 */
void STIMER_Cancel(stimer_t *timer)
{
    stimer_t *head = tlist;

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
 * @param timer     pointer to stimer_t
 * @param interval  New expiration interval in handler call quantum, [ms] typical
 */
void STIMER_SetInterval(stimer_t *timer, uint32_t interval)
{
    if(timer){
        timer->interval = interval;
    }
}

/**
 * @brief Starts a timer, if timer in parameter is not
 * on internal list, it will not start
 *
 * @param timer     pointer to stimer_t
 *
*/
void STIMER_Start(stimer_t *timer)
{
    if(timer){
        timer->count = timer->interval;
    }
}

/**
 * @brief Stops timer
 *
 * @param timer     pointer to stimer_t
*/
void STIMER_Stop(stimer_t *timer)
{
    if(timer){
        timer->count = 0;
    }
}

/**
 * @brief Check is a timer is running
 * TODO: FIX by checking if timer is in list
 * @param timer
 *
 * @return 0: if inactive, otherwise remaining ticks to timeout
 */
uint32_t STIMER_IsActive(stimer_t *timer)
{
    return timer ? timer->count != 0 : 0;
}

/**
 * @brief Simple timer handler, called periodically
 * with a fixed quantum usually 1ms, advances active timers by one tick.
 *
 * Note: callback may be called from interrupt context.
 *
*/
void STIMER_Handler(void)
{
    stimer_t *head = tlist;

    while(head){
        if(head->count > 0){
            if((--head->count) == 0){
                head->count = head->callback(head);
            }
        }
        head = head->next;
    }
}

/**
 * @brief stimer tick, called by application loop
 * to advance active timers.
 *
 * TODO: Maybe change to use simple callbacks without parameters
 * and return
 * @param tick current system tick
 */
void STIMER_Tick(uint32_t tick)
{
    stimer_t *head = tlist;
    uint32_t diff = tick - last_tick;

    while(head){
        if(head->count != 0){
            head->count -= diff;
            if(head->count  <= 0){
                uint32_t count = head->callback(head);
                // Compensate for delayed call
                head->count = count ? count + head->count : 0;
            }
        }
        head = head->next;
    }

    last_tick = tick;
}