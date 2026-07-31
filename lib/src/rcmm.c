#include "rcmm.h"

/**
 * @brief
 * @param dst
 * @param data
 * @return
 */
static uint8_t RCMM_Data(uint16_t *dst, uint8_t data)
{
    for(uint8_t i = 0; i < 4; i++, data <<= 2){
        switch(data & 0xC0){
            case 0x00:
                *dst++ = IR_MARK_ON(RCMM_0_6T);
                *dst++ = IR_MARK_OFF(RCMM_1T);
                break;
            case 0x40:
                *dst++ = IR_MARK_ON(RCMM_0_6T);
                *dst++ = IR_MARK_OFF(RCMM_1_6T);
                break;
            case 0x80:
                *dst++ = IR_MARK_ON(RCMM_0_6T);
                *dst++ = IR_MARK_OFF(RCMM_2_2T);
                break;
            case 0xC0:
                *dst++ = IR_MARK_ON(RCMM_0_6T);
                *dst++ = IR_MARK_OFF(RCMM_2_8T);
                break;
        }
    }

    return 8;
}

/**
 * @brief Generates a RC-MM frame to be transmitted
 *
 * @param dst   destination buffer, minimum size is 44
 * @param tr    toggle bit
 * @param addr  addresss, usually 0
 * @param data  key code
 *
 * @return size of frame
 */
uint16_t RCMM_Frame(uint16_t *dst, uint8_t control, uint8_t addr, uint8_t code)
{
    uint8_t offset = 2;
    // Header
    dst[0] = IR_MARK_ON(RCMM_1_5T);
    dst[1] = IR_MARK_OFF(RCMM_1T);
    // Mode bits
    offset += RCMM_Data(dst + offset, 0x30);
    // Address bits
    offset += RCMM_Data(dst + offset, addr);
    // Control bits
    offset += RCMM_Data(dst + offset, control);
    // Key code bits
    offset += RCMM_Data(dst + offset, code);
    // end pulse
    dst[offset++] = IR_MARK_ON(RCMM_0_6T);
    dst[offset++] = IR_MARK_OFF(RCMM_1T);

    return offset;
}
