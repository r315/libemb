#ifndef CLOCK_H
#define CLOCK_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

enum clocknr {
    CLOCK_CPU = 0,
    CLOCK_CLK1,
    CLOCK_CLK2,
    CLOCK_CLK3
};

typedef struct sysclock {
    uint32_t cpu;      // Cpu clock
    uint32_t clk1;     // Peripheral bus1
    uint32_t clk2;     // Peripheral bus2
    uint32_t clk3;     // Peripheral bus3/adc
}sysclock_t;

void CLOCK_GetAll(sysclock_t *clk);
uint32_t CLOCK_Get(enum clocknr clock);

#ifdef __cplusplus
}
#endif

#endif // CLOCK_H