#include <stddef.h>
#include "rtttl.h"
#include "tone.h"

typedef struct{
	const char *notes;         // notes to play
	uint8_t o;	               // default octave
	uint8_t d;                 // default note duration
	uint16_t b;                // music rhythm
    tone_t tone;
	uint16_t fullnoteduration; // duration of whole note in ms
}rtttl_t;

static const uint16_t rtttl_notes [] = {
		// 4th Octave
		220,  // A
		247,  // B
		262,  // C
		294,  // D
		330,  // E
		349,  // F
		392,  // G
		233,  // A#
		0,
		277,  // C#
		311,  // D#
		0,
		370,  // F#
		415,  // G#
};

static rtttl_t rtttl;

static tone_t *RTTTL_NextNote(rtttl_t *prtttl);

static void rtttl_callback(const tone_t **next_tone)
{
    *next_tone = RTTTL_NextNote(&rtttl);
}

/**
 * @brief Private helper to convert string to integer
 */
static uint16_t str2int(const char *str){
	uint16_t num = 0;
	while((*str > 0x2f) && (*str < 0x3a)){
		num = (num * 10) + (*str++ - '0');
	}
	return num;
}

static tone_t *RTTTL_NextNote(rtttl_t *prtttl)
{
//static uint16_t count = 0;
	uint16_t tmp;

	// Check if song has ended
	if(!(*prtttl->notes)){
		prtttl->tone.d = 0;
		return NULL;
	}

	// Get duration (1,2,4,8,16,32)
	uint16_t duration = 0;
	while((*prtttl->notes > 0x2f) && (*prtttl->notes < 0x3a)){
		duration = (duration * 10) + (*prtttl->notes++ - '0');
	}

	if(!duration){
		duration = prtttl->d;
	}

	prtttl->tone.d = rtttl.fullnoteduration / duration;  // set duration in ms

	// Get pitch [a-f](#)
	if((*prtttl->notes) == 'p'){
		prtttl->notes++;
		prtttl->tone.f = 0;
	}else{
		tmp = *prtttl->notes - 'a';

		rtttl.notes++;

		if( (*prtttl->notes) == '#'){
			prtttl->notes++;
			tmp += 7;  // # notes are placed after normal notes in frequency table
		}
		prtttl->tone.f = rtttl_notes[tmp];  // set base note frequency
	}

	// Check for dotted note, can be before octave
	if( (*prtttl->notes) == '.'){
		prtttl->notes++;
		prtttl->tone.d += (prtttl->tone.d >> 1);
	}

	// Get octave (4,5,6,7)
	tmp = 0;
	if((*prtttl->notes > 0x2f) && (*prtttl->notes < 0x3a)){
		tmp = *(prtttl->notes++) - '0';
	}

	if(!tmp){
		tmp = rtttl.o;
	}

	while(tmp > RTTTL_BASE_OCTAVE){
		rtttl.tone.f <<= 1;  // Adjust note frequency
		tmp--;
	}

	//skip comma
	if(*prtttl->notes == ','){
		prtttl->notes++;
	}

	//count ++;

    return &prtttl->tone;
}

/**
 * @brief
 *
 * */
uint8_t RTTTL_Play(const char *melody)
{
    if(!melody) {
        return RTTTL_ERR;
    }
	// Skip name section
	do{
		if(!(*melody)){
			return RTTTL_ERR_HDR;
		}
	}while( (*melody++) != ':');

	// Set default values
	rtttl.d = 4;
	rtttl.o = 6;
	rtttl.b = 63;

    // Parse control section
	do{
		if(!(*melody)){
			return RTTTL_ERR_DEF;
        }

		if( *melody == 'd'){
			melody += 2; // skip "d="
			rtttl. d = str2int(melody);
		}

		if(*melody == 'o'){
			melody += 2; // skip "o="
			rtttl.o = *melody - '0';
		}

		if(*melody == 'b'){
			melody += 2; // skip "b="
			rtttl.b = str2int(melody);
		}

	}while( (*melody++) != ':');

	rtttl.notes = melody;
	rtttl.fullnoteduration = RTTTL_BPM_TO_MS(rtttl.b) * 4;

	RTTTL_NextNote(&rtttl);     // Get first note

    TONE_SetCallback(rtttl_callback);

    return TONE_Play(&rtttl.tone) == TONE_ERR ? RTTTL_ERR : RTTTL_OK;
}
