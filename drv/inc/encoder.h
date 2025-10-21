#ifndef ENCODER_H
#define ENCODER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef struct enctype{
    void *handle;
    uint32_t count;
    int32_t value;
    int32_t max;
    int32_t min;
}enctype_t;

uint32_t ENC_Init(enctype_t *enc, uint32_t cfg);
// Get the current encoder count
uint32_t ENC_CountGet(enctype_t *enc);
// Get howm much counter has changed from last call
int32_t ENC_IncrementGet(enctype_t *enc);
// Update value in enctype structure
int32_t ENC_Update(enctype_t *enc);

#ifdef __cplusplus
}
#endif

#endif // ENCODER_H