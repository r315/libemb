#include "rc6.h"

/**
 * @brief
 * @param dst
 * @param data
 * @return
 */
static uint8_t RC6_Data(uint16_t *dst, uint8_t data)
{
    for(uint8_t i = 0; i < 8; i++, data <<= 1){
        if(data & 0x80){
            *dst++ = IR_MARK_ON(RC6_1T);
            *dst++ = IR_MARK_OFF(RC6_1T);
        }else{
            *dst++ = IR_MARK_OFF(RC6_1T);
            *dst++ = IR_MARK_ON(RC6_1T);
        }
    }

    return 16;
}

/**
 * @brief Generates a RC6 frame to be transmitted
 *
 * @param dst   destination buffer, minimum size is 44
 * @param tr    toggle bit
 * @param addr  addresss, usually 0
 * @param data  key code
 *
 * @return size of frame
 */
uint16_t RC6_Frame(uint16_t *dst, uint8_t tr, uint8_t addr, uint8_t data)
{
    // Leader one symbol
    dst[0] = IR_MARK_ON(RC6_6T);
    dst[1] = IR_MARK_OFF(RC6_2T);
    // Start bit one symbol
    dst[2] = IR_MARK_ON(RC6_1T);
    dst[3] = IR_MARK_OFF(RC6_1T);
    // field bits 3 symbols
    dst[4] = IR_MARK_OFF(RC6_1T);
    dst[5] = IR_MARK_ON(RC6_1T);
    dst[6] = IR_MARK_OFF(RC6_1T);
    dst[7] = IR_MARK_ON(RC6_1T);
    dst[8] = IR_MARK_OFF(RC6_1T);
    dst[9] = IR_MARK_ON(RC6_1T);
    // Trailer/toggle bit one symbol
    if(tr & 1){
        dst[10] = IR_MARK_ON(RC6_2T);
        dst[11] = IR_MARK_OFF(RC6_2T);
    }else{
        dst[10] = IR_MARK_OFF(RC6_2T);
        dst[11] = IR_MARK_ON(RC6_2T);
    }

    uint8_t offset = 12;

    offset += RC6_Data(dst + offset, addr);
    offset += RC6_Data(dst + offset, data);

    return offset;
}
