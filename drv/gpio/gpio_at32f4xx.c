#include "at32f4xx.h"
#include "gpio_at32f4xx.h"

#if !defined (AT32F403xx)
#define GPIOE_BASE 0
#endif

static const uint32_t ports[] = {GPIOA_BASE, GPIOB_BASE, GPIOC_BASE, GPIOD_BASE, GPIOE_BASE, GPIOF_BASE};

/**
 * @brief 
 * 
 * @param name 
 * @param mode 
 */
void GPIO_Config(uint32_t name, uint32_t mode) {
    GPIO_Type *port;
    uint8_t pin = GPIO_NAME_TO_PORT(name);
    uint8_t iom, iof;

    if (pin > sizeof(ports)/sizeof(ports[0])) {
        return;
    }
    
    port = (GPIO_Type*)ports[pin];
    pin = GPIO_NAME_TO_PIN(name);

    iom = mode & 0x03;
    iof = mode & (3 << 2);

    if(iom == GPIO_IOM_IN){
        if(iof & GPIO_IOF_PD){
            port->BSRE = (1 << pin); // Activate pull-up
            mode &= ~GPIO_IOF_FLT;   // clear reserved bit
        }else{
            port->BRE = (1 << pin);  // Activate pull-down
        }
    }

    if(pin <  8){ 
        port->CTRLL = (port->CTRLL & ~(15 << (pin << 2))) | (mode << (pin << 2));
    }else{ 
        port->CTRLH = (port->CTRLH & ~(15 << ((pin - 8) << 2))) | (mode << ((pin - 8) << 2)); 
    }
}

/**
 * @brief 
 * 
 * @param name 
 * @param mux 
 */
void GPIO_Function(uint32_t name, uint32_t fn)
{
   
}

/**
 * @brief 
 * 
 * @param name 
 * @return uint32_t 
 */
uint32_t GPIO_Read(uint32_t name){
    GPIO_Type *port;
    uint8_t pin = GPIO_NAME_TO_PORT(name);

    if (pin > sizeof(ports)/sizeof(ports[0])) {
        return 0xFFFFFFFF;
    }
    
    port = (GPIO_Type*)ports[pin];
    pin = GPIO_NAME_TO_PIN(name);

    return !!(port->IPTDT & (1 << pin));
}

/**
 * @brief 
 * 
 * @param name 
 * @param state 
 */
void GPIO_Write(uint32_t name, uint32_t state){
    GPIO_Type *port;
    uint8_t pin = GPIO_NAME_TO_PORT(name);

    if (pin > sizeof(ports)/sizeof(ports[0])) {
        return;
    }
    
    port = (GPIO_Type*)ports[pin];
    pin = GPIO_NAME_TO_PIN(name);

    uint32_t mask = (1 << pin);

    if(state & 1){ 
        port->BSRE = mask;
    }else{ 
        port->BRE = mask;
    }  
}

/**
 * @brief 
 * 
 * @param name 
 */
void GPIO_Toggle(uint32_t name){
    GPIO_Type *port;
    uint8_t pin = GPIO_NAME_TO_PORT(name);

    if (pin > sizeof(ports)/sizeof(ports[0])) {
        return;
    }
    
    port = (GPIO_Type*)ports[pin];
    pin = GPIO_NAME_TO_PIN(name);


    port->OPTDT ^= (1 << pin);
}
