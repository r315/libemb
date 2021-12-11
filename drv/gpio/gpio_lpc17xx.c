#include "gpio_lpc17xx.h"

/**
 * @brief GPIO pin alternative function, pin direction is
 *          controlled automaticaly.
 * @param   name:   Pin name, see pinName_e
 * @param   func:   Pin alternative function
 * */
void GPIO_Function(pinName_e pin, uint8_t func){
    uint8_t bit_pos = (pin & 0x0f) << 1;
    __IO uint32_t *pinsel;

    if(pin < P0_16){
        pinsel = & LPC_PINCON->PINSEL0;
    }else if(pin < P1_0){
        pinsel = & LPC_PINCON->PINSEL1;
    }else if(pin < P1_16){
        pinsel = & LPC_PINCON->PINSEL2;
    }else if(pin < P2_0){
        pinsel = & LPC_PINCON->PINSEL3;
    }else if(pin < P3_25){
        pinsel = & LPC_PINCON->PINSEL4;
    }else if(pin == P3_25 || pin == P3_26){
        pinsel = & LPC_PINCON->PINSEL7;
    }else if(pin == P4_28 || pin == P4_29){
        pinsel = & LPC_PINCON->PINSEL9;
    }else{
        return;
    }

    *pinsel = (*pinsel & ~(3 << bit_pos)) | (func << bit_pos);
}

/**
 * @brief GPIO pin configuration.
 * 
 * @param   name:  Pin name, see pinName_e
 * @param   cfg:    Configuration
 *                  PIN_OUT_PP,
 *                  PIN_OUT_OD,
 *                  PIN_IN_PU,
 *                  PIN_IN_REPEATER,
 *                  PIN_IN_FLOAT,
 *                  PIN_IN_PD,
 * */
void GPIO_Config(pinName_e name, uint8_t mode){

}