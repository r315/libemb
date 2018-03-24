/*-----------------------------------------------------------------------*/
/* PFF - Low level disk control module for ATtiny85     (C)ChaN, 2009    */
/*-----------------------------------------------------------------------*/

#define _WRITE_FUNC	1

#include <blueboard.h>
#include <spi.h>
#include "diskio.h"

/* Definitions for MMC/SDC command */
#define CMD0	(0x40+0)	/* GO_IDLE_STATE */
#define CMD1	(0x40+1)	/* SEND_OP_COND (MMC) */
#define	ACMD41  (0xC0+41)	/* SEND_OP_COND (SDC) */
#define CMD8	(0x40+8)	/* SEND_IF_COND */
#define CMD16	(0x40+16)	/* SET_BLOCKLEN */
#define CMD17	(0x40+17)	/* READ_SINGLE_BLOCK */
#define CMD24	(0x40+24)	/* WRITE_BLOCK */
#define CMD55	(0x40+55)	/* APP_CMD */
#define CMD58	(0x40+58)	/* READ_OCR */

/*-----------------------------------------------------------------------
   Module Private Functions                                              
-------------------------------------------------------------------------*/
uint8_t CardType;
/*-----------------------------------------------------------------------*/
/* Deselect the card and release SPI bus                                 */
/*-----------------------------------------------------------------------*/
static void release_spi (void)
{
	DESELECT_CARD();
	SPI(0xFF);
}
/*-----------------------------------------------------------------------*/
/* Send a command packet to MMC                                          */
/*-----------------------------------------------------------------------*/	
static uint8_t send_cmd (
	uint8_t cmd,		/* Command uint8_t */
	DWORD arg)			/* Argument */
{
	uint8_t n, res;


	if (cmd & 0x80) {	/* ACMD<n> is the command sequense of CMD55-CMD<n> */
		cmd &= 0x7F;
		res = send_cmd(CMD55, 0);
		if (res > 1) return res;
	}

	/* Select the card */
	DESELECT_CARD();
	SPI(0xFF);
	SELECT_CARD();
	SPI(0xFF);

	/* Send a command packet */
	SPI(cmd);						/* Start + Command index */
	SPI((uint8_t)(arg >> 24));		/* Argument[31..24] */
	SPI((uint8_t)(arg >> 16));		/* Argument[23..16] */
	SPI((uint8_t)(arg >> 8));			/* Argument[15..8] */
	SPI((uint8_t)arg);				/* Argument[7..0] */
	n = 0x01;							/* Dummy CRC + Stop */
	if (cmd == CMD0) n = 0x95;			/* Valid CRC for CMD0(0) */
	if (cmd == CMD8) n = 0x87;			/* Valid CRC for CMD8(0x1AA) */
	SPI(n);

	/* Receive a command response */
	n = 10;								/* Wait for a valid response in timeout of 10 attempts */
	do {
		res = SPI(0xFF);
	} while ((res & 0x80) && --n);

	return res;			/* Return with the response value */
}
/*--------------------------------------------------------------------------

   Public Functions

---------------------------------------------------------------------------*/  
/*-----------------------------------------------------------------------*/
/* Initialize Disk Drive                                                 */
/*-----------------------------------------------------------------------*/
DSTATUS disk_initialize (void)
{
	uint8_t n, cmd, ty, ocr[4];
	WORD tmr;	
    
#if _WRITE_FUNC
	if (MMC_SEL) disk_writep(0, 0);		/* Finalize write process if it is in progress */
#endif
	SELECT_CARD();
	for (n = 100; n; n--) SPI(0xFF);	/* Dummy clocks */

	ty = 0;
	if (send_cmd(CMD0, 0) == 1) {			/* Enter Idle state */
		if (send_cmd(CMD8, 0x1AA) == 1) {	/* SDv2 */
			for (n = 0; n < 4; n++) ocr[n] = SPI(0xFF);		/* Get trailing return value of R7 resp */
			if (ocr[2] == 0x01 && ocr[3] == 0xAA) {				/* The card can work at vdd range of 2.7-3.6V */
				for (tmr = 12000; tmr && send_cmd(ACMD41, 1UL << 30); tmr--) ;	/* Wait for leaving idle state (ACMD41 with HCS bit) */
				if (tmr && send_cmd(CMD58, 0) == 0) {		/* Check CCS bit in the OCR */
					for (n = 0; n < 4; n++) ocr[n] = SPI(0xFF);
					ty = (ocr[0] & 0x40) ? CT_SD2 | CT_BLOCK : CT_SD2;	/* SDv2 (HC or SC) */
				}
			}
		} else {							/* SDv1 or MMCv3 */
			if (send_cmd(ACMD41, 0) <= 1) 	{
				ty = CT_SD1; cmd = ACMD41;	/* SDv1 */
			} else {
				ty = CT_MMC; cmd = CMD1;	/* MMCv3 */
			}
			for (tmr = 25000; tmr && send_cmd(cmd, 0); tmr--) ;	/* Wait for leaving idle state */
			if (!tmr || send_cmd(CMD16, 512) != 0)			/* Set R/W block length to 512 */
				ty = 0;
		}
	}
	CardType = ty;
	release_spi();
	
	#if (_osc==1)
	SPI_HI_SPEED(TRUE);
	#endif

	return ty ? 0 : STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Read partial sector                                                   */
/*-----------------------------------------------------------------------*/

DRESULT disk_readp (
	uint8_t *buff,		/* Pointer to the read buffer (NULL:Read uint8_ts are forwarded to the stream) */
	DWORD lba,		/* Sector number (LBA) */
	WORD ofs,		/* uint8_t offset to read from (0..511) */
	WORD cnt		/* Number of uint8_ts to read (ofs + cnt mus be <= 512) */
)
{
	DRESULT res;
	uint8_t rc;
	WORD bc;


	if (!(CardType & CT_BLOCK)) lba *= 512;		/* Convert to uint8_t address if needed */

	res = RES_ERROR;
	if (send_cmd(CMD17, lba) == 0) {		/* READ_SINGLE_BLOCK */

		bc = 30000;
		do {							/* Wait for data packet in timeout of 100ms */
			rc = SPI(0xFF);
		} while (rc == 0xFF && --bc);

		if (rc == 0xFE) {				/* A data packet arrived */
			bc = 514 - ofs - cnt;

			/* Skip leading uint8_ts */
			if (ofs) {
				do SPI(0xFF); while (--ofs);
			}

			/* Receive a part of the sector */
			do
				*buff++ = SPI(0xFF);
			while (--cnt);
		

			/* Skip trailing uint8_ts and CRC */
			do SPI(0xFF); while (--bc);

			res = RES_OK;
		}
	}

	release_spi();

	return res;
}



/*-----------------------------------------------------------------------*/
/* Write partial sector                                                  */
/*-----------------------------------------------------------------------*/
#if _WRITE_FUNC

DRESULT disk_writep (
	const uint8_t *buff,	/* Pointer to the uint8_ts to be written (NULL:Initiate/Finalize sector write) */
	DWORD sa			/* Number of uint8_ts to send, Sector number (LBA) or zero */
)
{
	DRESULT res;
	WORD bc;
	static WORD wc;


	res = RES_ERROR;

	if (buff) {		/* Send data uint8_ts */
		bc = (WORD)sa;
		while (bc && wc) {		/* Send data uint8_ts to the card */
			SPI(*buff++);
			wc--; bc--;
		}
		res = RES_OK;
	} else {
		if (sa) {	/* Initiate sector write process */
			if (!(CardType & CT_BLOCK)) sa *= 512;	/* Convert to uint8_t address if needed */
			if (send_cmd(CMD24, sa) == 0) {			/* WRITE_SINGLE_BLOCK */
				SPI(0xFF); SPI(0xFE);		/* Data block header */
				wc = 512;							/* Set uint8_t counter */
				res = RES_OK;
			}
		} else {	/* Finalize sector write process */
			bc = wc + 2;
			while (bc--) SPI(0);	/* Fill left uint8_ts and CRC with zeros */
			if ((SPI(0xFF) & 0x1F) == 0x05) {	/* Receive data resp and wait for end of write process in timeout of 300ms */
				for (bc = 65000; SPI(0xFF) != 0xFF && bc; bc--) ;	/* Wait ready */
				if (bc) res = RES_OK;
			}
			release_spi();
		}
	}

	return res;
}
#endif
