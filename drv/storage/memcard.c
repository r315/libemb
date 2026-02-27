#include "board.h"
#include "diskio.h"
#include "memcard.h"

/* Card type flags (CardType) */
#define CT_NONE				0x00
#define CT_MMC				0x01	/* MMC ver 3 */
#define CT_SD1				0x02	/* SD ver 1 */
#define CT_SD2				0x04	/* SD ver 2 */
#define CT_SDC				(CT_SD1|CT_SD2)	/* SD */
#define CT_BLOCK			0x08	/* Block addressing */

/* Definitions for MMC/SDC commands (supported on SPI mode) */
#define CMD0				(0x40+0)	/* GO_IDLE_STATE */
#define CMD1				(0x40+1)	/* SEND_OP_COND (MMC) */
#define CMD6				(0x40+6)	/* SWITCH_FUNC */
#define CMD8				(0x40+8)	/* SEND_IF_COND */
#define CMD9                (0x40+9)    /* SEND_CSD */
#define CMD10               (0x40+10)   /* SEND_CID */
#define CMD12               (0x40+12)	/* STOP_TRANSMISSION */
#define CMD13               (0x40+13)	/* SEND_STATUS */
#define CMD16				(0x40+16)	/* SET_BLOCKLEN */
#define CMD17				(0x40+17)	/* READ_SINGLE_BLOCK */
#define CMD18				(0x40+18)	/* READ_MULTIPLE_BLOCK */
#define CMD24				(0x40+24)	/* WRITE_BLOCK */
#define CMD25				(0x40+25)	/* WRITE_MULTIPLE_BLOCK */
#define CMD27				(0x40+27)	/* PROGRAM_CSD */
#define CMD55				(0x40+55)	/* APP_CMD */
#define CMD58				(0x40+58)	/* READ_OCR */
#define CMD59				(0x40+59)	/* CRC_ON_OFF */
#define ACMD41              (0xC0+41)	/* SEND_OP_COND (SDC) */

#define ACMD_FLAG           (1 << 7)

/* Response */
#define R1_IDLE             (1 << 0)
#define R1_ERASE_RESET      (1 << 1)
#define R1_ILLEGAL_CMD      (1 << 2)
#define R1_CRC_ERROR        (1 << 3)
#define R1_ERASE_ERROR      (1 << 4)
#define R1_ADDR_ERROR       (1 << 5)
#define R1_PARAM_ERROR      (1 << 6)
#define R1_MSB              (1 << 7)

#define SD_START_BLOCK_SINGLE       0xFE
#define SD_START_BLOCK_MULTIPLE     0xFD

#define SD_OCR_CCS          (1 << 30)   /* Card Capacity Status */
#define SD_OCR_CPS          (1 << 31)   /* Card Power up Status */
#define SD_HCS              (1 << 30)   /* Host Capacity Support */

#define ENABLE_SECTOR_WRITE	0
#define ENABLE_CARD_DETECT	0

#define SD_SPI_SLOW         0

/*-----------------------------------------------------------------------
   Module Private variables
-------------------------------------------------------------------------*/
static uint8_t CardType;
static spibus_t *s_spi = NULL;
static uint8_t spi_dummy;
/*-----------------------------------------------------------------------*/
/* Send a command packet to MMC                                          */
/*-----------------------------------------------------------------------*/
/**
 * @brief Sends a data packet to memory card, note that card is selected on exit
 *
 * @param cmd 		: command code
 * @param arg 		: command argument
 * @return uint8_t  : command response
 */
static uint8_t cardCmd (uint8_t cmd, uint32_t arg){
    uint8_t r1;
    uint8_t data_packet[6];

    if (cmd & ACMD_FLAG) {	/* ACMD<n> is the command sequence of CMD55-CMD<n> */
        r1 = cardCmd(CMD55, 0);
        if (r1 > 1)
            return r1;
        cmd = cmd & ~ACMD_FLAG;
    }

    BOARD_SDCARD_DESELECT;

    /* Form command packet */
    data_packet[0] = cmd;
    data_packet[1] = (uint8_t)(arg >> 24);
    data_packet[2] = (uint8_t)(arg >> 16);
    data_packet[3] = (uint8_t)(arg >> 8);
    data_packet[4] = (uint8_t)(arg >> 0);

    if (cmd == CMD0){
        data_packet[5] = 0x95;			/* Valid CRC for CMD0(0) */
    }else if(cmd == CMD8){
        data_packet[5] = 0x87;			/* Valid CRC for CMD8(0x1AA) */
    }else{
        data_packet[5] = 0x01;			/* Dummy CRC + Stop */
    }

    BOARD_SDCARD_SELECT;
    SPI_Xchg(s_spi, &spi_dummy);
    SPI_Transfer(s_spi, data_packet, sizeof(data_packet));

    /* Receive a command response */
    /* Wait for a valid response within timeout, responses are not fixed to index*/
    for (uint8_t n = 0; n < sizeof(data_packet); n++){
        r1 = SPI_Xchg(s_spi, &spi_dummy);
        if((r1 & R1_MSB) == 0){ /* MSB is cleared, a valid response was received */
            break;
        }
    }
    return r1;
}

/*--------------------------------------------------------------------------

   Public Functions

---------------------------------------------------------------------------*/

/**
 * @brief Retrieve Card Specific Data
 *
 * @param CSDRegister   16-Byte buffer
 * @return uint8_t      0: Success, otherwise error
 */
uint8_t SDGetCSD(uint8_t *CSDRegister){
    uint8_t res = 1;

    if (cardCmd(CMD9, 0) == 0) {
        uint8_t retry = 32;
        do {/* Wait for data packet with timeout */
            if (SPI_Xchg(s_spi, &spi_dummy) == SD_START_BLOCK_SINGLE) {
                for (uint8_t n = 0; n < 16; n++){
                   CSDRegister[n] = SPI_Xchg(s_spi, &spi_dummy);
                }
                res = 0;
                break;
            }
        }while(--retry);
    }

    BOARD_SDCARD_DESELECT;
    return res;
}

/**
 * @brief Retrieve Card Identification Register.
 *
 * @param CIDRegister   16-Byte buffer
 * @return uint8_t      0: Success, otherwise error
 */
uint8_t SDGetCID(uint8_t *CIDRegister){
    uint8_t res = 1;

    if (cardCmd(CMD10, 0) == 0) {
        uint8_t retry = 32;
        do {/* Wait for data packet with timeout */
            if (SPI_Xchg(s_spi, &spi_dummy) == SD_START_BLOCK_SINGLE) {
                for (uint8_t n = 0; n < 16; n++){
                   CIDRegister[n] = SPI_Xchg(s_spi, &spi_dummy);
                }
                res = 0;
                break;
            }
        }while(--retry);
    }

    BOARD_SDCARD_DESELECT;
    return res;
}

/**
 * @brief Retrieve SD Card Configuration Register
 *
 * @param SCRRegister   8-Byte buffer
 * @return uint8_t      0: Success, otherwise error
 */
uint8_t SDGetSCR(uint8_t *SCRRegister){
    (void)SCRRegister;
    return 1;
}

/**
 * @brief Retrieve Operation Condition Register
 *
 * @param OCRRegister   4-byte buffer
 * @return uint8_t      0: Success, otherwise error
 */
uint8_t SDGetOCR(uint8_t *OCRRegister){
    uint8_t retry = 32;
    uint8_t res = 1;

    do{
        if (cardCmd(CMD58, 0) == 0) {
            for (uint8_t n = 0; n < 4; n++){
                OCRRegister[3 - n] = SPI_Xchg(s_spi, &spi_dummy); // store as little-endian
            }

            if(*(uint32_t*)OCRRegister & SD_OCR_CPS){
                res = 0;
                break;
            }
        }
    }while(--retry);

    BOARD_SDCARD_DESELECT;
    return res;
}
/**
 * @brief Initialize Disk Drive in SPI mode
 *
 * @return STA_OK on success, STA_NOINIT otherwise
 */
DSTATUS disk_initialize (void){
    #warning "memcard driver requires fixing"
    #if 0 // TODO: FIX
    uint8_t n, cmd, ocr[4];
    uint16_t tmr;

#if ENABLE_CARD_DETECT
    if(BOARD_CARD_IS_DETECTED == 0){
        return STA_NODISK;
    }
#endif

#if ENABLE_SECTOR_WRITE
    if (BOARD_CARD_IS_SELECTED)
        disk_writep(0, 0);		/* Finalize write process if it is in progress */
#endif

#if SD_SPI_SLOW
    if(s_spi == NULL){
        s_spi = BOARD_SD_GET_SPI;
        s_spi->freq = 100000;
        SPI_Init(s_spi);
    }
#else

    if(s_spi == NULL){
        s_spi = BOARD_SD_GET_SPI;
    }

    uint32_t freq = s_spi->freq;
    s_spi->freq = 100000;
    spi_dummy = 0xFF;     // Dummy variable for single transfers
    SPI_Init(s_spi);
#endif
    BOARD_SDCARD_DESELECT;

    n = 10;
    do{ /* Dummy clocks for enter SPI mode */
        SPI_Xchg(s_spi, &spi_dummy);
    }while(--n);

    CardType = CT_NONE;

    if (cardCmd(CMD0, 0) == R1_IDLE) {          /* Enter Idle state */
        if (cardCmd(CMD8, 0x1AA) == R1_IDLE) {
            /* Card is SDv2 */
            /* Read R7 remaining 4-bytes */
            for (uint8_t n = 0; n < 4; n++){
                ocr[n] = SPI_Xchg(s_spi, &spi_dummy);
            }

            if (ocr[2] == 0x01 && ocr[3] == 0xAA) {				/* Check if card supports vdd range of 2.7-3.6V */

                /* Wait for leaving idle state (ACMD41 with HCS bit) */
                for (tmr = 12000; tmr; tmr--){
                    if(cardCmd(ACMD41, SD_HCS) != R1_IDLE){
                        break;
                    }
                }

                if(SDGetOCR(ocr) == 0){
                    /* SDv2, HC or SC? */
                    CardType = (*(uint32_t*)ocr & SD_OCR_CCS) ? CT_SD2 | CT_BLOCK : CT_SD2;
                }
            }
            // Reject card
        } else {
            if (cardCmd(ACMD41, 0) <= R1_IDLE) 	{
                CardType = CT_SD1; cmd = ACMD41;    /* SDv1 */
            } else {
                CardType = CT_MMC; cmd = CMD1;	    /* MMCv3 */
            }

            for (tmr = 25000; tmr && cardCmd(cmd, 0); tmr--) ;	/* Wait for leaving idle state */

            if (!tmr || cardCmd(CMD16, 512) != 0){			    /* Set R/W block length to 512 */
                CardType = CT_NONE;
            }
        }
    }

    BOARD_SDCARD_DESELECT;

#if SD_SPI_SLOW == 0
    s_spi->freq = freq;
    SPI_Init(s_spi);
#endif
#endif
    return (CardType != CT_NONE) ? STA_OK : STA_NOINIT;
}


/**
 * @brief Read partial sector
 *
 * @param buff		: Pointer to the read buffer (NULL:Read uint8_ts are forwarded to the stream)
 * @param lba		: Sector number (LBA)
 * @param ofs		: offset to read from (0..511)
 * @param cnt 		: Number of uint8_ts to read (ofs + cnt must be <= 512)
 * @return DRESULT 	:
 * */
DRESULT disk_readp (BYTE *buff,	DWORD lba, UINT ofs, UINT cnt){
    DRESULT res;
    WORD bc;

    if(CardType == CT_NONE){
        return RES_NOTRDY;
    }

    if (!(CardType & CT_BLOCK)){
        lba *= 512;		/* Convert to uint8_t address if needed */
    }

    res = RES_ERROR;

#if ENABLE_DISK_ACTIVITY_LED
    BOARD_CARD_ACTIVE;
#endif

    /* READ_SINGLE_BLOCK */
    if (cardCmd(CMD17, lba) == 0) {
        bc = 30000;
        do {/* Wait for data packet with timeout */
            if (SPI_Xchg(s_spi, &spi_dummy) == SD_START_BLOCK_SINGLE) {
                /* A data packet arrived */
                bc = 514 - ofs - cnt; /* Read 512 bytes + 2 CRC bytes */

                /* Skip leading uint8_ts */
                if (ofs) {
                    do SPI_Xchg(s_spi, &spi_dummy); while (--ofs);
                }

                /* Receive a part of the sector */
                do
                    *buff++ = SPI_Xchg(s_spi, &spi_dummy);
                while (--cnt);

                /* Skip trailing uint8_ts and CRC */
                do SPI_Xchg(s_spi, &spi_dummy); while (--bc);

                res = RES_OK;
                break;
            }
        } while (--bc);
    }

    BOARD_SDCARD_DESELECT;

#if ENABLE_DISK_ACTIVITY_LED
    BOARD_CARD_NOT_ACTIVE;
#endif

    return res;
}

/*-----------------------------------------------------------------------*/
/* Write partial sector                                                  */
/*-----------------------------------------------------------------------*/
/**
 * @brief
 *
 * @param buff 		: Pointer to the uint8_ts to be written (NULL:Initiate/Finalize sector write)
 * @param sa 		: Number of uint8_ts to send, Sector number (LBA) or zero
 * @return DRESULT
 */
DRESULT disk_writep (const uint8_t *buff, DWORD sa ){
#if ENABLE_SECTOR_WRITE
    (void)buff;
    (void)sa;
    DRESULT res;
    WORD bc;
    static WORD wc;

    res = RES_ERROR;

    if(CardType == CT_NONE){
        return RES_NOTRDY;
    }

#if ENABLE_DISK_ACTIVITY_LED
    BOARD_CARD_ACTIVE;
#endif

    if (buff) {		/* Send data uint8_ts */
        bc = (WORD)sa;
        while (bc && wc) {		/* Send data uint8_ts to the card */
            SPI_Send(s_spi, *buff++);
            wc--; bc--;
        }
        res = RES_OK;
    } else {
        if (sa) {	/* Initiate sector write process */
            if (!(CardType & CT_BLOCK)) sa *= 512;	/* Convert to uint8_t address if needed */
            if (cardCmd(CMD24, sa) == 0) {			/* WRITE_SINGLE_BLOCK */
                SPI_Send(s_spi, 0xFF);
                SPI_Send(s_spi, SD_START_BLOCK_SINGLE);		/* Data block header */
                wc = 512;							/* Set uint8_t counter */
                res = RES_OK;
            }
        } else {	/* Finalize sector write process */
            bc = wc + 2;
            while (bc--) SPI_Send(s_spi, 0);	/* Fill left uint8_ts and CRC with zeros */
            if ((SPI_Send(s_spi, 0xFF) & 0x1F) == 0x05) {	/* Receive data resp and wait for end of write process in timeout of 300ms */
                for (bc = 65000; SPI_Send(s_spi, 0xFF) != 0xFF && bc; bc--) ;	/* Wait ready */
                if (bc) res = RES_OK;
            }
            BOARD_SDCARD_DESELECT;
        }
    }

#if ENABLE_DISK_ACTIVITY_LED
    BOARD_CARD_NOT_ACTIVE;
#endif

    return res;
#else
    (void)buff;
    (void)sa;

    return RES_NOTRDY;
#endif
}
