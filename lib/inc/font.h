#ifndef _font_h_
#define _font_h_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct font_s{
    uint8_t w;          // Character width in pixel including spacing
    uint8_t h;          // Character height in pixel including spacing
    const uint8_t *data;// Character data
    uint16_t data_size; // Total bytes of all characters on font
    uint8_t bpl;        // Bytes per line
    uint8_t offset;     // offset from ascii table
    uint8_t vspace;     // Vertical spacing
}font_t;

#if !defined(USE_MULTIPLE_FONTS)
#define USE_DEFAULT_FONT
#endif

#ifdef USE_DEFAULT_FONT
    extern const font_t defaultFont; // 8x8
#endif

#ifdef USE_DEFAULT_BOLD_FONT
	extern const font_t defaultBoldFont; //8x7
#endif

#ifdef USE_PIXELDUST_FONT
	extern const font_t pixelDustFont; //7x7
#endif

#ifdef USE_LCD_FONT
	extern const font_t lcdFont; // 8x7
#endif

#ifdef USE_COURIER_FONT
	extern const font_t courierFont; //8x14
#endif

#ifdef USE_GROTESKBOLD_FONT
    extern const font_t GroteskBold16x32;
#endif

#ifdef USE_UBUNTU_FONT
    extern const font_t ubuntuFont; //24x32
#endif


#ifdef __cplusplus
}
#endif

#endif
