#ifndef _font_h_
#define _font_h_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _font_t{
    uint8_t w;
    uint8_t h;
    const uint8_t *data;
    uint16_t data_len;
    uint8_t bpl;        // Bytes per line
    uint8_t offset;     // offset from ascii table
    uint8_t spacing;
}font_t;

extern font_t defaultFont;

#if !defined(SINGLE_FONT)	
	extern font_t defaultBoldFont;
	extern font_t lcdFont;
	extern font_t pixelDustFont;
#ifdef USE_COURIER_FONT
	extern font_t courierFont;
#endif
#ifdef USE_GROTESKBOLD_FONT    	
    extern font_t GroteskBold16x32;
#endif
#ifdef USE_UBUNTU_FONT
    extern font_t ubuntuFont;
#endif
#endif /* SINGLE_FONT */

#ifdef __cplusplus
}
#endif

#endif
