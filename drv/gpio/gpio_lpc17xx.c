#include "gpio_lpc17xx.h"

#define NAME_TO_POS(name)       name & (32 - 1)
#define NAME_TO_OFS(name)       (name >> 2) & 0x38
#define NAME_TO_POS2(name)      (name & 0x0f) << 1
#define NAME_TO_OFS2(name)      (name >> 4)

/**
 * @brief GPIO pin configuration.
 * 
 * @param   name:  Pin name, see pinName_e
 * @param   cfg:    Configuration
 *                  PIN_FUNC0,
 *                  PIN_FUNC1,
 *                  PIN_FUNC2,
 *                  PIN_FUNC3,
 *                  PIN_OUT_PP,
 *                  PIN_OUT_OD,
 *                  PIN_IN_PU,
 *                  PIN_IN_REPEATER,
 *                  PIN_IN_FLOAT,
 *                  PIN_IN_PD,
 * */
void GPIO_Init(pinName_e name, uint8_t cfg){    
    GPIO_Function(name, cfg);
    GPIO_Mode(name, cfg >> PIN_MODE_POS);
    GPIO_Direction(name, cfg & PIN_OUT_PP);
}

/**
 * @brief GPIO pin alternative function, pin direction is
 *          controlled automaticaly.
 * @param   bit_pos : 0 - 31
 * @param   ofs     : reg offset 0- 10
 * @param   func    : Pin alternative function
 *                      PIN_FUNC0,
 *                      PIN_FUNC1,
 *                      PIN_FUNC2,
 *                      PIN_FUNC3,
 * */
void GPIO_Function(pinName_e name, uint8_t func){
    uint8_t bit_pos = NAME_TO_POS2(name);
    uint8_t ofs = NAME_TO_OFS2(name);

    __IO uint32_t *pinsel = &LPC_PINCON->PINSEL0 + ofs;
    func = func & PIN_FUNC_MASK;
    *pinsel = (*pinsel & ~(3 << bit_pos)) | (func << bit_pos);
}

/**
 * @brief 
 * 
 * @param bit_pos   : 0 - 31
 * @param ofs       : reg offset 0- 10
 * @param mode      : Pin mode
 *                  PIN_MODE_PU,
 *                  PIN_MODE_REPEATER,
 *                  PIN_MODE_FLOAT,
 *                  PIN_MODE_PD,
 *                  PIN_MODE_PP,
 *                  PIN_MODE_OD
 */
void GPIO_Mode(pinName_e name, uint8_t mode){
    uint8_t bit_pos = NAME_TO_POS2(name);
    uint8_t ofs = NAME_TO_OFS2(name);
    __IO uint32_t *reg = &LPC_PINCON->PINMODE0 + ofs;    

    *reg = (*reg & ~(3 << bit_pos)) | ((mode & PIN_FUNC_MASK) << bit_pos);

    if(mode & PIN_MODE_OD){
        ofs = ofs >> 1;
        reg = &LPC_PINCON->PINMODE_OD0 + ofs;
        *reg |= (1 << bit_pos);
    }
}

/**
 * @brief Controls pin direction for GPIO function
 * 
 * @param name      : Pin name
 * @param dir       : 0: input, output otherwise
 */
void GPIO_Direction(pinName_e name, uint8_t dir){
    uint8_t bit_pos = NAME_TO_POS(name);
    uint8_t ofs = NAME_TO_OFS(name);
    __IO uint32_t *reg = (uint32_t*)&LPC_GPIO0->FIODIR + ofs;

    *reg = (dir == 0) ? *reg & ~(1 << bit_pos) : *reg | (1 << bit_pos);
}

/**
 * @brief 
 * 
 * @param name 
 * @param state 
 */
void GPIO_Write(pinName_e name, uint8_t state){
    uint8_t bit_pos = NAME_TO_POS(name);
    uint8_t ofs = NAME_TO_OFS(name);
    __IO uint32_t *reg = ((state == GPIO_LOW) ? (uint32_t*)&LPC_GPIO0->FIOCLR : (uint32_t*)&LPC_GPIO0->FIOSET) + ofs;

    *reg = (1 << bit_pos);
}

/**
 * @brief 
 * 
 * @param name 
 * @return uint32_t 
 */
uint32_t GPIO_Read(pinName_e name){
    uint8_t bit_pos = NAME_TO_POS(name);
    uint8_t ofs = NAME_TO_OFS(name);
    __IO uint32_t *reg = (uint32_t*)&LPC_GPIO0->FIOPIN + ofs;

    return *reg & (1 << bit_pos);
}

/**
 * @brief 
 * 
 * @param name 
 */
void GPIO_Toggle(pinName_e name){
    uint8_t bit_pos = NAME_TO_POS(name);
    uint8_t ofs = NAME_TO_OFS(name);
    __IO uint32_t *reg = (uint32_t*)&LPC_GPIO0->FIOPIN + ofs;

    *reg ^= (1 << bit_pos);
}
