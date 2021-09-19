/**
* @file		rng.h
* @brief	Contains random number generator api
*     		
* @version	1.0
* @date		10 Sep. 2021
* @author	Hugo Reis
**********************************************************************/

#ifndef _RNG_H_
#define _RNG_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>


void RNG_Init(void);
uint32_t RNG_Get();

#ifdef __cplusplus
}
#endif

#endif /* _RNG_H_ */
