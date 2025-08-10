#include "board.h"


static uint32_t seed;
static uint32_t Lehmer32(void){
	seed += 0xE120FC15;
	uint64_t tmp = (uint64_t)seed * 0x4A39B70D;
	uint32_t m1 = (tmp >> 32) ^ tmp;
	tmp = (uint64_t)m1 * 0x12FAD5C9;
	uint32_t m2 = (tmp >> 32) ^ tmp;
	return m2;
}

void RNG_Init(void){
    seed = 0;
}

uint32_t RNG_Get(){
    return Lehmer32();
}

void RNG_Seed(uint32_t s){
    seed = s;
}