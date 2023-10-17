#include "gpio_lpc17xx.h"

#define NAME_TO_BIT(name)        (name & 0x1f)          // One bit masking
#define NAME_TO_OFS(name)        ((name & 0x60) >> 2)   // offset divide by 4 since is added as uint32_t
#define NAME_TO_BIT_2(name)      ((name & 0x0f) << 1)   // Two bit masking
#define NAME_TO_OFS_2(name)      (name >> 4)

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
void GPIO_Config(pinName_e name, uint32_t cfg){    
    uint8_t bit_pos = NAME_TO_BIT_2(name);
    uint8_t ofs = NAME_TO_OFS_2(name);

    // Config function
    __IO uint32_t *reg = &LPC_PINCON->PINSEL0 + ofs;
    uint8_t tmp = PIN_MASK_FUNC(cfg);
    *reg = (*reg & ~(3 << bit_pos)) | (tmp << bit_pos);
    // Mode
    reg = &LPC_PINCON->PINMODE0 + ofs;
    tmp = PIN_MASK_MODE(cfg);
    *reg = (*reg & ~(3 << bit_pos)) | (tmp << bit_pos);

    if(cfg & PIN_TYPE_OD){
        ofs = ofs >> 1;
        reg = &LPC_PINCON->PINMODE_OD0 + ofs;
        *reg |= (1 << bit_pos);
    }
    
    // Direction
    bit_pos = NAME_TO_BIT(name);
    ofs = NAME_TO_OFS(name);
    reg = (uint32_t*)&LPC_GPIO0->FIODIR + ofs;
    *reg = (cfg & PIN_DIR_OUT) ? *reg | (1 << bit_pos) : *reg & ~(1 << bit_pos);
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
void GPIO_Function(pinName_e name, uint32_t func){
    uint8_t bit_pos = NAME_TO_BIT_2(name);
    uint8_t ofs = NAME_TO_OFS_2(name);

    __IO uint32_t *pinsel = &LPC_PINCON->PINSEL0 + ofs;
    func = PIN_MASK_FUNC(func);
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
void GPIO_Mode(pinName_e name, uint32_t mode){
    uint8_t bit_pos = NAME_TO_BIT_2(name);
    uint8_t ofs = NAME_TO_OFS_2(name);
    __IO uint32_t *reg = &LPC_PINCON->PINMODE0 + ofs;    

    *reg = (*reg & ~(3 << bit_pos)) | (PIN_MASK_MODE(mode) << bit_pos);

    if(mode & PIN_TYPE_OD){
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
void GPIO_Direction(pinName_e name, uint32_t dir){
    uint8_t bit_pos = NAME_TO_BIT(name);
    uint8_t ofs = NAME_TO_OFS(name);
    __IO uint32_t *reg = (uint32_t*)&LPC_GPIO0->FIODIR + ofs;

    *reg = (dir & PIN_DIR_OUT) ? *reg | (1 << bit_pos) : *reg & ~(1 << bit_pos);
}

/**
 * @brief 
 * 
 * @param name 
 * @param state 
 */
void GPIO_Write(pinName_e name, uint32_t state){
    uint8_t bit_pos = NAME_TO_BIT(name);
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
    uint8_t bit_pos = NAME_TO_BIT(name);
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
    uint8_t bit_pos = NAME_TO_BIT(name);
    uint8_t ofs = NAME_TO_OFS(name);
    __IO uint32_t *reg = (uint32_t*)&LPC_GPIO0->FIOPIN + ofs;

    *reg ^= (1 << bit_pos);
}
