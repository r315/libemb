#include <pinName.h>


/**
 * @brief Configure GPIO pin
 * @param port : pin port
 * @param pin : pin number 0 - 15
 * @param mode : Output
 *                  GPO_2MHZ
 *                  GPO_10MHZ
 *                  GPO_50MHZ
 *                  GPO_AF | GPO_xxMHZ
 *                  GPO_AF_OD | GPO_xxMHZ
 *               Input
 *                  GPI_FLOAT
 *                  GPI_ANALOG
 *                  GPI_PD
 *                  GPI_PU
 * */
void PIN_Init(pinName_e name, uint8_t mode) {
    
    if(mode == GPI_PD){
        port->BRR = (1 << pin);
    }

    if(mode == GPI_PU){
        port->BSRR = (1 << pin);
    }    

    mode &= 0x0f;

    if(pin <  8){ 
        port->CRL = (port->CRL & ~(15 << (pin << 2))) | (mode << (pin << 2));
    }else{ 
        port->CRH = (port->CRH & ~(15 << ((pin - 8) << 2))) | (mode << ((pin - 8) << 2)); 
    }
}

void PIN_Write(pinName_e name, uint8_t state){
}

void PIN_Toggle(pinName_e name){
}

void PIN_PORT_Write(pinName_e name, uint32_t value){
}
uint32_t PIN_Read(pinName_e name){
}