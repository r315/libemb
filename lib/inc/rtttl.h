#ifndef _rtttl_h_
#define _rtttl_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

//wholenote duration = (60 seconds * 1000ms * 4 beats full note) / (bpm of song)
#define RTTTL_FULL_NOTE_DURATION 	240000UL
#define RTTTL_BPM_TO_MS(_bpm)		(60000/_bpm)
#define RTTTL_BASE_OCTAVE 			4  //4th octave

enum rtttl_e {
    RTTTL_OK,
    RTTTL_ERR,
    RTTTL_ERR_HDR,
    RTTTL_ERR_DEF,
    RTTTL_ERR_INIT
};

/**
 * @brief
 *
 */
uint8_t RTTTL_Play(const char *melody);

#ifdef __cplusplus
};
#endif
#endif
