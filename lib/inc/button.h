/**
* @file		button.h
* @brief	Contains API header for reading buttons.
*
* @version	1.0
* @date		3 Nov. 2016
* @author	Hugo Reis
**********************************************************************/

#ifndef _button_h_
#define _button_h_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum bevt_e {
    BUTTON_NONE = 0,
    BUTTON_PRESSED,
    BUTTON_TIMING,
    BUTTON_HOLD,
    BUTTON_RELEASED
}bevt_t;

/**
* @brief Faz a iniciação do sistema para permitir o acesso aos botões
* @param ht : mask bitmap com os bits correspondentes aos pinos ligados nos botões
**/
void BUTTON_Init(uint32_t (*scan)(void));

/**
* @brief Devolve o estado dos botões, pressionado, largado, mantido ou em contagem.
* Não é bloqueante.
**/
bevt_t BUTTON_Read(void);

/**
* @brief Devolve o código (bitmap) do botão pressionado. É bloqueante.
*
**/
uint32_t BUTTON_Get(void);

/**
* @brief Devolve o ultimo evento registado:
* 		 pressionado (transição),
*        libertado (transição),
*        repetição (mantido pressionado)
**/
bevt_t BUTTON_GetEvents(void);

/**
 * @brief Devolve o codigo (bitmap) da tecla corrente
 *        BUTTON_EMPTY se nenhuma tecla pressionada
 **/
uint32_t BUTTON_GetValue(void);

/**
* @brief Efectua uma espera pelo evento dado no parametro*
**/
void BUTTON_WaitEvent(bevt_t event);

/**
* @brief configura o tempo minimo para que uma tecla
*        seja considerada como mantida pressionada
**/
void BUTTON_SetHoldTime(uint32_t t);

/**
* @brief verifica se o botao passado no parametro foi premido
*
**/
uint32_t BUTTON_Pressed(uint32_t mask);

#ifdef __cplusplus
}
#endif

#endif

