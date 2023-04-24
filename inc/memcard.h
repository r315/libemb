#ifndef _memcard_h_
#define _memcard_h_

// Specification:
// https://www.sdcard.org/cms/wp-content/themes/sdcard-org/dl.php?f=Part1_Physical_Layer_Simplified_Specification_Ver8.00.pdf
// https://www.sdcard.org/cms/wp-content/themes/sdcard-org/dl.php?f=PartA5_SD_Extensions_API_Simplified_Specification_Ver1.00.pdf

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

uint8_t SDGetCSD(uint8_t *CSDRegister);
uint8_t SDGetCID(uint8_t *CIDRegister);
uint8_t SDGetSCR(uint8_t *SCRRegister);
uint8_t SDGetOCR(uint8_t *OCRRegister);

#ifdef __cplusplus
}
#endif

#endif