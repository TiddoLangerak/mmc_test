/*-----------------------------------------------------------------------*/
/* Low level disk I/O module                                             */
/* (C) Copyright 2007,2008                                               */
/* Martin Thomas, Lorenz Aebi                                            */
/*-----------------------------------------------------------------------*/
/* This is a LPC23xx/24xx MCI disk I/O module                            */
/*-----------------------------------------------------------------------*/

// Add Multi Block Write by Lorenz Aebi 2008
// Bugfix from Marco Ziegert 20090823

// #define DISKIO_DEBUG
// #define DISKIO_DUMPSTAT
// #define DISKIO_DEBUGR
// #define DISKIO_DEBUGW


#include <string.h>
#include "lpc_types.h"
//#include "irq.h"
//#include "target.h"
#include "lpc177x_8x_mci.h"
#include "lpc177x_8x_gpdma.h"


#include "diskio.h"

#ifdef __GNUC__
#define myprintf xprintf
#else
#include <stdio.h>
#define myprintf printf
#endif

#ifdef __ARMCC_VERSION
// RealView
#define INLINE __inline
#else
// gcc
#define INLINE inline
#endif


#ifdef DISKIO_DEBUG
#define diskio_printf myprintf
#else
#define diskio_printf( fmt, args... )
#endif

#ifdef DISKIO_DEBUGR
#define diskio_printfr myprintf
#else
#define diskio_printfr( fmt, args... )
#endif

#ifdef DISKIO_DEBUGW
#define diskio_printfw myprintf
#else
// #define diskio_printfw( fmt, args... )
#define diskio_printfw( fmt, ... )
#endif




#ifdef __GNUC__
//volatile uint8_t *WriteBlock __attribute__ ((aligned (4))) = (volatile uint8_t *)(DMA_SRC); /* treat WriteBlock as a constant address */
//volatile uint8_t *ReadBlock __attribute__ ((aligned (4))) = (volatile uint8_t *)(DMA_DST); /* treat ReadBlock as a constant address */
#else
//volatile uint8_t *WriteBlock = (uint8_t *)(DMA_SRC); /* treat WriteBlock as a constant address */
//volatile uint8_t *ReadBlock = (uint8_t *)(DMA_DST); /* treat ReadBlock as a constant address */
#endif

static uint8_t WriteBlock[BLOCK_LENGTH];
static uint8_t ReadBlock[BLOCK_LENGTH];


static volatile
DWORD Timer1, Timer2;	/* 1000Hz decrement timer */

static volatile
DSTATUS Stat = STA_NOINIT;	/* Disk status */

static
uint32_t CardType;			/* b0:MMC, b1:SDC, b2:Block addressing */

static 
uint8_t csd[16]; /* cached csd */

static 
uint8_t sd_status[16]; /* cached 16 MS-uint8_ts from SD_STATUS (ACMD13) */

#define	DWORD_TO_uint8_tS(uint8_t_ptr_,dw_val_)	\
do { \
	*(uint8_t*)(uint8_t_ptr_)     = (uint8_t)((DWORD)(dw_val_)>>24); \
	*(uint8_t*)((uint8_t_ptr_)+1) = (uint8_t)((DWORD)(dw_val_)>>16); \
	*(uint8_t*)((uint8_t_ptr_)+2) = (uint8_t)((WORD)(dw_val_)>>8);   \
	*(uint8_t*)((uint8_t_ptr_)+3) = (uint8_t)(dw_val_); \
} while (0)


extern volatile DWORD MCI_CardType;




#define CURR_STATE_IDLE     0
#define CURR_STATE_READY    1
#define CURR_STATE_IDENT    2
#define CURR_STATE_STBY     3
#define CURR_STATE_TRAN     4
#define CURR_STATE_DATA     5
#define CURR_STATE_RCV      6
#define CURR_STATE_PRG      7
#define CURR_STATE_DIS      8

#ifdef DISKIO_DUMPSTAT

const char* states[] = {
	"idle", "ready", "ident", "stby", 
	"tran", "data", "rcv", "prg", "dis"
};



static volatile void dump_status( const char* info )
{
	DWORD st;
	DWORD currst;

	st = MCI_Send_Status();
	if ( st == INVALID_RESPONSE ) {
		diskio_printf("dump_status: Send_Status INVALID_RESPONSE\n");
		return;
	}

	diskio_printf("------ Status register %s\n -----", info);
	diskio_printf("AKE_SEQ_ERROR    : %s\n",  (st & (1 <<  3)) ? "error"     : "no error" );
	diskio_printf("APP_CMD          : %s\n",  (st & (1 <<  5)) ? "enabled"   : "disabled" );
	diskio_printf("READY_FOR_DATA   : %s\n",  (st & (1 <<  8)) ? "not ready" : "ready"    );

	currst = (st >> 9) & 0x0f;
	if ( currst > 8 ) {
		diskio_printf("CURR_STATE       : reserved\n");
	}
	else {
		diskio_printf("CURR_STATE       : %s   <---\n", states[currst]);
	}
	// TODO bits 13-18
	diskio_printf("ERROR            : %s\n", (st & (1<<19)) ? "error"  : "no error" );
	diskio_printf("CC_ERROR         : %s\n", (st & (1<<20)) ? "error"  : "no error" );
	diskio_printf("CARD_ECC_FAILED  : %s\n", (st & (1<<21)) ? "failure": "success"  );
	diskio_printf("ILLEGAL_COMMAND  : %s\n", (st & (1<<22)) ? "error"  : "no error" );
	diskio_printf("COM_CRC_ERROR    : %s\n", (st & (1<<23)) ? "error"  : "no error" );
	diskio_printf("LOCK_UNLOCK_FAIL : %s\n", (st & (1<<24)) ? "error"  : "no error" );
	diskio_printf("CARD_IS_LOCKED   : %s\n", (st & (1<<25)) ? "locked" : "unlocked" );
	// TODO bits 26-28
	diskio_printf("BLOCK_LEN_ERROR  : %s\n", (st & (1<<29)) ? "error"  : "no error" );
	diskio_printf("ADDRESS_ERROR    : %s\n", (st & (1<<30)) ? "error"  : "no error" );
	diskio_printf("OUT_OF_RANGE     : %s\n", (st & (1<<31)) ? "error"  : "no error" );
}
#else 
#define dump_status(s__)
#endif

static INLINE USHORT currstate_from_status( DWORD st )
{
	return ( (st >> 9 ) & 0xff );
}

static INLINE uint8_t status_ready_bit( DWORD st )
{
	return ( st & ( 1<<8 ) ) ? 1 : 0;
}

// state to trans (SD Card State Diagram: V2.2 P4-7)
static void SELECT(void)
{
	uint32_t currst, st;
	dump_status("before select");

	Timer2 = 1000;
	do {
		// cmd7 until state is trans
    MCI_GetCardStatus(&st);
		currst = currstate_from_status(st);
		if ( currst != CURR_STATE_TRAN ) {
			if ( MCI_Cmd_SelectCard() != MCI_FUNC_OK ) {
				diskio_printf("SELECT Select_Card failed - retry\n");
			}
		}
	} while ( ( currst != CURR_STATE_TRAN ) && Timer2 );

	if ( Timer2 == 0 ) {
		diskio_printf("SELECT timeout changeing to trans\n");
	}
	dump_status("after select");
}

// state to stby
static void DESELECT(void)
{
	uint32_t cardtype_save, st, currst;

	Timer2 = 500;
	do {
    MCI_GetCardStatus(&st);
		
		currst = currstate_from_status(st);
	} while ( ( currst != CURR_STATE_TRAN ) && 
	          ( currst != CURR_STATE_STBY  ) && 
	          Timer2 );

	if ( Timer2 == 0 ) {
		diskio_printf("DESELCT timeout waiting trans or stby\n");
	}
	else if ( currst == CURR_STATE_TRAN ) {
		cardtype_save = MCI_CardType;
		// CMD7: trans to stby (RCA 0) / de-select
		MCI_CardType = 0;
		if ( MCI_Cmd_SelectCard() != MCI_FUNC_OK ) {
			diskio_printf("DESELECT failed\n");
		}
		MCI_CardType = cardtype_save;
	}
	else {
		//diskio_printf("DSELECT already in stby\n");
	}
	
	return;
}

// wait for stby or trans with timeout
static uint8_t wait_ready( void ) 
{
	uint8_t res;
	uint32_t st;
	// DWORD currst;

	res = 0xaa;
	Timer2 = 500;	/* Wait for ready in timeout of 500ms */
	do {
		
    MCI_GetCardStatus(&st);
		if ( st == INVALID_RESPONSE ) {
			diskio_printf("wait_ready: Send_Status INVALID_RESPONSE\n");
		}
		else {
			// currst = currstate_from_status(st);
			// if ( currst == 3 /* stby */ || currst == 4 /* trans */) {
			if ( status_ready_bit( st ) ) {
				res = 0xff;
			}
		}
	}
	while ( (res != 0xFF) && Timer2 );

	if ( Timer2 == 0 ) {
		diskio_printf("wait_ready: timeout stby or trans\n");
	}

	return res;
}

// well, no SPI used here but will keep the interface compatible with
// Chan's examples
static void release_spi(void)
{
	DESELECT();
}

static void power_off (void)
{
	SELECT();
	wait_ready();
	release_spi();

// TODO	MCI_Power_Off();

	Stat |= STA_NOINIT;		/* Set STA_NOINIT */
}



static int MCI_disk_initialize(void)
{
	int err = 0, i;
	uint32_t csddw[4];
  st_Mci_CardId cid;

	diskio_printf("\n");
#if MCI_DMA_ENABLED
	/* on DMA channel 0, source is memory, destination is MCI FIFO. */
	/* On DMA channel 1, source is MCI FIFO, destination is memory. */
	GPDMA_Init();
	diskio_printf("MCI DMA enabled\n");
#endif

	if ( MCI_Init(0) != MCI_FUNC_OK )
	{
		diskio_printf("MCI_Init failed\n");
		err++; /* fatal error */
	}


	if ( err || MCI_GetCID(&cid) != MCI_FUNC_OK )
	{
		diskio_printf("Check_CID failed\n");
		err++; /* fatal error */
	}
	
	if ( err || MCI_SetCardAddress() != MCI_FUNC_OK )
	{
		diskio_printf("Set_Address failed\n");
		err++; /* fatal error */
	}
	
	if ( err || MCI_GetCSD( csddw ) != MCI_FUNC_OK )
	{
		diskio_printf("Send_CSD failed\n");
		err++;
	}
	else 
	{
		diskio_printf("diskio: CSD %08x %08x %08x %08x\n",
			csddw[0], csddw[1], csddw[2], csddw[3]);
		for ( i=0; i<4; i++ ) {
			DWORD_TO_uint8_tS( &csd[i*4], csddw[i] );
		}
	}
	
	if ( err || MCI_Cmd_SelectCard() != MCI_FUNC_OK )
	{
		diskio_printf("Select_Card failed\n");
		err++;
	}

	if ( !err ) 
	{
		if ( ( MCI_CardType == MCI_SD_CARD ) /*|| 
		     ( MCI_CardType == SDHC_CARD )*/ )
		{
			MCI_Set_MCIClock( MCI_NORMAL_RATE );
			if (MCI_SetBusWidth( SD_4_BIT ) != MCI_FUNC_OK )
			{
				diskio_printf("set 4 bit mode failed\n");
				err++;
			}
		}
	}

	
	if ( err || MCI_SetBlockLen( BLOCK_LENGTH ) != MCI_FUNC_OK )
	{
		diskio_printf("Set_BlockLen failed\n");
		err++;
	}
	diskio_printf("Set_BlockLen o.k.\n");

	if (!err) diskio_printf("MMC_disk_initialize ok\n");

	return err;
}


/*-----------------------------------------------------------------------*/
/* Initialize a Drive                                                    */

DSTATUS disk_initialize (
	uint8_t drv				/* Physical drive nmuber (0..) */
)
{
	if ( drv != 0 ) return STA_NOINIT;

	if ( MCI_disk_initialize() ) {
		// error occured 
		Stat |= STA_NOINIT;
		power_off();
	}
	else {
		Stat &= ~STA_NOINIT;
		// cardtype mci.h to chan cardtype - (maybe redundant)
		switch ( MCI_CardType ) {
//		case SDHC_CARD :
//			CardType = 4;
//			break;
		case MCI_MMC_CARD :
			CardType = 1;
			break;
		case MCI_SD_CARD :
			CardType = 2;
			break;
		default :
			CardType = 0;
		}
	}
	
	return Stat;
}



/*-----------------------------------------------------------------------*/
/* Return Disk Status                                                    */

DSTATUS disk_status (
	uint8_t drv		/* Physical drive nmuber (0..) */
)
{
	if ( drv != 0 ) return STA_NOINIT;

	return Stat;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */

DRESULT disk_read (
	uint8_t drv,		/* Physical drive number (0..) */
	uint8_t *buff,		/* Data buffer to store read data */
	DWORD s,	/* Sector number (LBA) */
	uint8_t count		/* Sector count (1..255) */
)
{
	// DRESULT res;
	//WORD k;
  uint32_t sector = (uint32_t)s;

	if (drv || !count) return RES_PARERR;
	if (Stat & STA_NOINIT) return RES_NOTRDY;

	// done in MCI driver if (!(CardType & 4)) sector *= 512;	/* Convert to uint8_t address if needed */

	/* Select the card and wait for ready */
	/* mthomas - not sure if this is needed for SD */
	DESELECT();
	//if (wait_ready() != 0xFF) return (DRESULT)(0xFF);
	SELECT();

	// TODO: Multi-block
	do {
		diskio_printfr("Read_Block start...\n");

		if ( MCI_ReadBlock((uint8_t*)ReadBlock, sector, 1) != MCI_FUNC_OK ) {
			diskio_printf("MCI_Read_Block failed\n");
			break;
		}
		else {    
			
			diskio_printfr("Read_Block wait...\n");

      while(MCI_GetDataXferEndState() != 0);
			diskio_printfr("Read_Block end wait\n");
			// laebi: memcpy is faster than the for-Loop
			// TODO: avoid copy
			memcpy(buff, (char*)ReadBlock, 512);
			// mziegert 20090823: Bugfix, memcpy does not increase buff pointer
			buff = &buff[512];
			sector++;
		}
	} while ( --count );
	
	release_spi();

	return count ? RES_ERROR : RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */

#if _READONLY == 0
DRESULT disk_write (
	uint8_t drv,			/* Physical drive number (0..) */
	const uint8_t *buff,	/* Data to be written */
	DWORD sector,		/* Sector number (LBA) */
	uint8_t count			/* Sector count (1..255) */
)
{
	//WORD k;

	if (drv || !count)
		return RES_PARERR;
	if (Stat & STA_NOINIT)
		return RES_NOTRDY;
	if (Stat & STA_PROTECT)
		return RES_WRPRT;

	// done in mci-driver: if (!(CardType & 4)) sector *= 512;	/* Convert to uint8_t address if needed */

	/* Select the card and wait for ready */
	/* mthomas - not sure if this is needed for SD */
	DESELECT();
	//if (wait_ready() != 0xFF) return (DRESULT)0xFF;
	SELECT();

	//do {
	diskio_printfw("Write_Block %d start...\n", sector );

  if (1) {
		// Single Block write
		// TODO: avoid copy
		memcpy((char*)WriteBlock, buff, 512);
		if (MCI_WriteBlock((uint8_t*)buff, sector, 1) != MCI_FUNC_OK) {
			diskio_printf("MCI_Write_Block failed\n");
			return RES_ERROR;
			//break;
		} else {
			diskio_printfw("Write_Block wait...\n");

      while(MCI_GetDataXferEndState() != 0);
			diskio_printfw("Write_Block end wait\n");
		}
	}

	// laebi: not necessary
	//release_spi();

	//return count ? RES_ERROR : RES_OK;
	return RES_OK;
}

#endif /* _READONLY */



/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */

DRESULT disk_ioctl (
	uint8_t drv,		/* Physical drive nmuber (0..) */
	uint8_t ctrl,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res;
	uint8_t n; // buffered csd[16];
	DWORD csize;

	if (drv) return RES_PARERR;
	if (Stat & STA_NOINIT) return RES_NOTRDY;

	res = RES_ERROR;

	switch (ctrl) {

		case CTRL_SYNC :	/* Flush dirty buffer if present */
			SELECT();
			if (wait_ready() == 0xFF) {
				res = RES_OK;
			}
			break;

		case GET_SECTOR_SIZE :	/* Get sectors on the disk (WORD) */
			*(WORD*)buff = 512;
			res = RES_OK;
			break;

		case GET_SECTOR_COUNT :	/* Get number of sectors on the disk (WORD) */
			if ((csd[0] >> 6) == 1) {	/* SDC ver 2.00 */
				csize = csd[9] + ((WORD)csd[8] << 8) + 1;
				*(DWORD*)buff = (DWORD)csize << 10;
			} 
			else {					/* MMC or SDC ver 1.XX */
				n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
				csize = (csd[8] >> 6) + ((WORD)csd[7] << 2) + ((WORD)(csd[6] & 3) << 10) + 1;
				*(DWORD*)buff = (DWORD)csize << (n - 9);
			}
			res = RES_OK;
			break;

		
		case GET_BLOCK_SIZE :	/* Get erase block size in unit of sectors (DWORD) */
			if (CardType & 4) {   /* SDC ver 2.00 - use cached  Read SD status */
				// TODO - untested!
				*(DWORD*)buff = 16UL << (sd_status[10] >> 4);
				res = RES_OK;
			} 
			else {   /* SDC ver 1.XX or MMC */
				if (CardType & 2) {			/* SDC ver 1.XX */
					*(DWORD*)buff = (((csd[10] & 63) << 1) + ((WORD)(csd[11] & 128) >> 7) + 1) << ((csd[13] >> 6) - 1);
				} else {					/* MMC */
					*(DWORD*)buff = ((WORD)((csd[10] & 124) >> 2) + 1) * (((csd[11] & 3) << 3) + ((csd[11] & 224) >> 5) + 1);
				}
				res = RES_OK;
			}
			break;

		default:
			res = RES_PARERR;
	}

	release_spi(); 

	return res;
}


/*-----------------------------------------------------------------------*/
/* Device Timer Interrupt Procedure  (Platform dependent)                */
/*-----------------------------------------------------------------------*/
/* This function must be called in period of 1ms                         */


void disk_timerproc (void)
{
	DWORD n;

	n = Timer1;						/* 1000Hz decrement timer */
	if (n) Timer1 = --n;
	n = Timer2;
	if (n) Timer2 = --n;

#if 0
	static uint8_t pv;
	uint8_t s, p;

	p = pv;
	pv = SOCKPORT & (SOCKWP | SOCKINS);	/* Sample socket switch */

	if (p == pv) {					/* Have contacts stabled? */
		s = Stat;

		if (p & SOCKWP)				/* WP is H (write protected) */
			s |= STA_PROTECT;
		else						/* WP is L (write enabled) */
			s &= ~STA_PROTECT;

		if (p & SOCKINS)			/* INS = H (Socket empty) */
			s |= (STA_NODISK | STA_NOINIT);
		else						/* INS = L (Card inserted) */
			s &= ~STA_NODISK;

		Stat = s;
	}
#endif
}

DWORD get_fattime (void)
{
    return 0;
}

