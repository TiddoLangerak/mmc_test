/*****************************************************************************
 *
 *   Copyright(C) 2011, Embedded Artists AB
 *   All rights reserved.
 *
 ******************************************************************************
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * products. This software is supplied "AS IS" without any warranties.
 * Embedded Artists AB assumes no responsibility or liability for the
 * use of the software, conveys no license or title under any patent,
 * copyright, or mask work right to the product. Embedded Artists AB
 * reserves the right to make changes in the software without
 * notification. Embedded Artists AB also make no representation or
 * warranty that such application will be suitable for the specified
 * use without further testing or modification.
 *****************************************************************************/



/******************************************************************************
 * Includes
 *****************************************************************************/

#include "lpc_types.h"
#include "lpc177x_8x_timer.h"
#include "norflash.h"


/******************************************************************************
 * Defines and typedefs
 *****************************************************************************/

#define CMD_SWID      0x90
#define CMD_CFI_QRY   0x98
#define CMD_ID_EXIT   0xF0

#define CMD_ERASE_BLOCK  0x0050
#define CMD_ERASE_SECTOR 0x0030
#define CMD_ERASE_CHIP   0x0010

#define MAN_ID_SST         0x00BF
#define DEV_ID_SST39VF3201 0x235B 

/******************************************************************************
 * External global variables
 *****************************************************************************/

/******************************************************************************
 * Local variables
 *****************************************************************************/

static geometry_t chip_info;

/******************************************************************************
 * Local Functions
 *****************************************************************************/

static void pinConfig(void)
{
  LPC_IOCON->P3_0 |= 1; /* D0 @ P3.0 */
  LPC_IOCON->P3_1 |= 1; /* D1 @ P3.1 */
  LPC_IOCON->P3_2 |= 1; /* D2 @ P3.2 */
  LPC_IOCON->P3_3 |= 1; /* D3 @ P3.3 */

  LPC_IOCON->P3_4 |= 1; /* D4 @ P3.4 */
  LPC_IOCON->P3_5 |= 1; /* D5 @ P3.5 */
  LPC_IOCON->P3_6 |= 1; /* D6 @ P3.6 */
  LPC_IOCON->P3_7 |= 1; /* D7 @ P3.7 */

  LPC_IOCON->P3_8 |= 1; /* D8 @ P3.8 */
  LPC_IOCON->P3_9 |= 1; /* D9 @ P3.9 */
  LPC_IOCON->P3_10 |= 1; /* D10 @ P3.10 */
  LPC_IOCON->P3_11 |= 1; /* D11 @ P3.11 */

  LPC_IOCON->P3_12 |= 1; /* D12 @ P3.12 */
  LPC_IOCON->P3_13 |= 1; /* D13 @ P3.13 */
  LPC_IOCON->P3_14 |= 1; /* D14 @ P3.14 */
  LPC_IOCON->P3_15 |= 1; /* D15 @ P3.15 */

  LPC_IOCON->P3_16 |= 1; /* D16 @ P3.16 */
  LPC_IOCON->P3_17 |= 1; /* D17 @ P3.17 */
  LPC_IOCON->P3_18 |= 1; /* D18 @ P3.18 */
  LPC_IOCON->P3_19 |= 1; /* D19 @ P3.19 */

  LPC_IOCON->P3_20 |= 1; /* D20 @ P3.20 */
  LPC_IOCON->P3_21 |= 1; /* D21 @ P3.21 */
  LPC_IOCON->P3_22 |= 1; /* D22 @ P3.22 */
  LPC_IOCON->P3_23 |= 1; /* D23 @ P3.23 */

  LPC_IOCON->P3_24 |= 1; /* D24 @ P3.24 */
  LPC_IOCON->P3_25 |= 1; /* D25 @ P3.25 */
  LPC_IOCON->P3_26 |= 1; /* D26 @ P3.26 */
  LPC_IOCON->P3_27 |= 1; /* D27 @ P3.27 */

  LPC_IOCON->P3_28 |= 1; /* D28 @ P3.28 */
  LPC_IOCON->P3_29 |= 1; /* D29 @ P3.29 */
  LPC_IOCON->P3_30 |= 1; /* D30 @ P3.30 */
  LPC_IOCON->P3_31 |= 1; /* D31 @ P3.31 */

  LPC_IOCON->P4_0 |= 1; /* A0 @ P4.0 */
  LPC_IOCON->P4_1 |= 1; /* A1 @ P4.1 */
  LPC_IOCON->P4_2 |= 1; /* A2 @ P4.2 */
  LPC_IOCON->P4_3 |= 1; /* A3 @ P4.3 */

  LPC_IOCON->P4_4 |= 1; /* A4 @ P4.4 */
  LPC_IOCON->P4_5 |= 1; /* A5 @ P4.5 */
  LPC_IOCON->P4_6 |= 1; /* A6 @ P4.6 */
  LPC_IOCON->P4_7 |= 1; /* A7 @ P4.7 */

  LPC_IOCON->P4_8 |= 1; /* A8 @ P4.8 */
  LPC_IOCON->P4_9 |= 1; /* A9 @ P4.9 */
  LPC_IOCON->P4_10 |= 1; /* A10 @ P4.10 */
  LPC_IOCON->P4_11 |= 1; /* A11 @ P4.11 */

  LPC_IOCON->P4_12 |= 1; /* A12 @ P4.12 */
  LPC_IOCON->P4_13 |= 1; /* A13 @ P4.13 */
  LPC_IOCON->P4_14 |= 1; /* A14 @ P4.14 */
  LPC_IOCON->P4_15 |= 1; /* A15 @ P4.15 */

  LPC_IOCON->P4_16 |= 1; /* A16 @ P4.16 */
  LPC_IOCON->P4_17 |= 1; /* A17 @ P4.17 */
  LPC_IOCON->P4_18 |= 1; /* A18 @ P4.18 */
  LPC_IOCON->P4_19 |= 1; /* A19 @ P4.19 */

  LPC_IOCON->P4_20 |= 1; /* A20 @ P4.20 */
  LPC_IOCON->P4_21 |= 1; /* A21 @ P4.21 */
  LPC_IOCON->P4_22 |= 1; /* A22 @ P4.22 */
  LPC_IOCON->P4_23 |= 1; /* A23 @ P4.23 */

  LPC_IOCON->P4_24 |= 1; /* OEN @ P4.24 */
  LPC_IOCON->P4_25 |= 1; /* WEN @ P4.25 */
  LPC_IOCON->P4_26 |= 1; /* BLSN[0] @ P4.26 */
  LPC_IOCON->P4_27 |= 1; /* BLSN[1] @ P4.27 */

  LPC_IOCON->P4_28 |= 1; /* BLSN[2] @ P4.28 */
  LPC_IOCON->P4_29 |= 1; /* BLSN[3] @ P4.29 */
  LPC_IOCON->P4_30 |= 1; /* CSN[0] @ P4.30 */
  LPC_IOCON->P4_31 |= 1; /* CSN[1] @ P4.31 */

  LPC_IOCON->P2_14 |= 1; /* CSN[2] @ P2.14 */
  LPC_IOCON->P2_15 |= 1; /* CSN[3] @ P2.15 */
}

#if 0
static void getIdString(uint16_t idString[11])
{
  int i = 0;

  *(GET_ADDR(0x5555)) = 0x00AA;
  *(GET_ADDR(0x2aaa)) = 0x0055;
  *(GET_ADDR(0x5555)) = CMD_CFI_QRY;

  for (i = 0; i < 11; i++) {
    idString[i] = *(GET_ADDR(0x10 + i));
  }

  *(GET_ADDR(0x5555)) = 0x00AA;
  *(GET_ADDR(0x2aaa)) = 0x0055;
  *(GET_ADDR(0x5555)) = CMD_ID_EXIT;
}
#endif

static void getGeoInfo(uint16_t info[14])
{
  int i = 0;

  *(GET_ADDR(0x5555)) = 0x00AA;
  *(GET_ADDR(0x2aaa)) = 0x0055;
  *(GET_ADDR(0x5555)) = CMD_CFI_QRY;

  for (i = 0; i < 14; i++) {
    info[i] = *(GET_ADDR(0x27 + i));
  }

  *(GET_ADDR(0x5555)) = 0x00AA;
  *(GET_ADDR(0x2aaa)) = 0x0055;
  *(GET_ADDR(0x5555)) = CMD_ID_EXIT;
}

static uint32_t getProductId(void)
{
  uint16_t manuid = 0;
  uint16_t devid = 0;
  uint32_t result = 0;

  *(GET_ADDR(0x5555)) = 0x00AA;
  *(GET_ADDR(0x2aaa)) = 0x0055;
  *(GET_ADDR(0x5555)) = CMD_SWID;

  manuid = *(GET_ADDR(0x00));
  devid  = *(GET_ADDR(0x01));

  result = ((manuid << 16) | devid);

  *(GET_ADDR(0x5555)) = 0x00AA;
  *(GET_ADDR(0x2aaa)) = 0x0055;
  *(GET_ADDR(0x5555)) = CMD_ID_EXIT;

  return result;
}

/******************************************************************************
 *
 * Description:
 *    When the SST39VF160x/320x are in the internal Program operation, any 
 *    attempt to read DQ7 will produce the complement of the true data. Once 
 *    the Program operation is completed, DQ7 will produce true data. Note 
 *    that even though DQ7 may have valid data immediately following the 
 *    completion of an internal Write operation, the remaining data outputs 
 *    may still be invalid: valid data on the entire data bus will appear in 
 *    subsequent successive Read cycles after an interval of 1 µs. During 
 *    internal Erase operation, any attempt to read DQ7 will produce a '0'. 
 *    Once the internal Erase operation is completed, DQ7 will produce a '1'.
 *
 * Parameters:
 *    addr      The device address 
 *    data      The original (true) data
 *    timeout   Maximum number of loops to delay
 *
 * Returns:
 *    TRUE if success
 *
 *****************************************************************************/
static uint16_t check_data_polling(uint32_t addr, uint16_t data, uint32_t timeout)
{
  volatile uint16_t *p = (uint16_t*) addr;
  uint16_t true_data = data & 0x80;
  int i;

  for (i = 0; i < timeout; i++)
  {
    if ( true_data == (*p &0x80) )
    {
      TIM_Waitus(1);
      return (TRUE);
    } 
  }  
  return (FALSE);
}

/******************************************************************************
 *
 * Description:
 *    During the internal Program or Erase operation, any consecutive attempts 
 *    to read DQ6 will produce alternating “1”s and “0”s, i.e., toggling 
 *    between 1 and 0. When the internal Program or Erase operation is 
 *    completed, the DQ6 bit will stop toggling. The device is then ready 
 *    for the next operation. 
 *
 * Parameters:
 *    addr      The device address 
 *    timeout   Maximum number of loops to delay
 *
 * Returns:
 *    TRUE if success
 *
 *****************************************************************************/
static uint16_t check_toggle_ready(uint32_t addr, uint32_t timeout)
{
  volatile uint16_t *p = (uint16_t*) addr;
  uint16_t predata, currdata;
  int i;

  predata = *p & 0x40;
  for (i = 0; i < timeout; i++)
  {
    currdata = *p & 0x40;
    if (predata == currdata)
    {
      TIM_Waitus(1);
      return (TRUE);
    }
    predata = currdata;
  }
  return (FALSE);
}

/******************************************************************************
 * Public Functions
 *****************************************************************************/



/******************************************************************************
 *
 * Description:
 *    Initialize the NOR Flash
 *
 *****************************************************************************/
uint32_t norflash_init()
{
  uint32_t prodId = 0;

  LPC_SC->PCONP      |= 0x00000800;

  LPC_EMC->Control = 0x00000001;

  LPC_EMC->Config  = 0x00000000;

  //Disable Auto-Byte Addressing (on boards designed for LPC24xx)
  LPC_SC->SCS |= 0x00000001;

  pinConfig();

  LPC_EMC->StaticConfig0   = 0x00000081;

  LPC_EMC->StaticWaitWen0  = 0x00000003; /* ( n + 1 ) -> 4 clock cycles */
  LPC_EMC->StaticWaitOen0  = 0x00000003; /* ( n ) -> 0 clock cycles */
  LPC_EMC->StaticWaitRd0   = 0x00000006; /* ( n + 1 ) -> 7 clock cycles */
  LPC_EMC->StaticWaitPage0 = 0x00000003; /* ( n + 1 ) -> 4 clock cycles */
  LPC_EMC->StaticWaitWr0   = 0x00000005; /* ( n + 2 ) -> 7 clock cycles */
  LPC_EMC->StaticWaitTurn0 = 0x00000003; /* ( n + 1 ) -> 4 clock cycles */


  prodId = getProductId();

  if (prodId == ((MAN_ID_SST << 16) | DEV_ID_SST39VF3201)) {

    uint16_t info[14];
    getGeoInfo(info);
    chip_info.device_size = 1 << info[0];
    chip_info.num_sectors = ((info[7] << 8) | info[6]) + 1;
    chip_info.sector_size = ((info[9] << 8) | info[8]) * 256;
    chip_info.num_blocks =  ((info[11] << 8) | info[10]) + 1;
    chip_info.block_size = ((info[13] << 8) | info[12]) * 256;

    return TRUE;
  }

  return FALSE;
}

void norflash_getGeometry(geometry_t* geometry)
{
  *geometry = chip_info;
}


uint32_t norflash_eraseSector(uint32_t addr)
{
  volatile uint16_t* p;

  *(GET_ADDR(0x5555)) 	= 0x00AA;
  *(GET_ADDR(0x2AAA)) 	= 0x0055;
  *(GET_ADDR(0x5555)) 	= 0x0080;
  *(GET_ADDR(0x5555)) 	= 0x00AA;
  *(GET_ADDR(0x2AAA)) 	= 0x0055;

  p  = (uint16_t*) addr;
  *p = CMD_ERASE_SECTOR;

  return check_data_polling(addr, 0xffff, 500000);
}

uint32_t norflash_eraseBlock(uint32_t addr)
{
  volatile uint16_t* p;

  *(GET_ADDR(0x5555)) 	= 0x00AA;
  *(GET_ADDR(0x2AAA)) 	= 0x0055;
  *(GET_ADDR(0x5555)) 	= 0x0080;
  *(GET_ADDR(0x5555)) 	= 0x00AA;
  *(GET_ADDR(0x2AAA)) 	= 0x0055;

  p  = (uint16_t*) addr;
  *p = CMD_ERASE_BLOCK;

  return check_toggle_ready(addr, 500000);
}

uint32_t norflash_eraseEntireChip(void)
{
  *(GET_ADDR(0x5555)) 	= 0x00AA;
  *(GET_ADDR(0x2AAA)) 	= 0x0055;
  *(GET_ADDR(0x5555)) 	= 0x0080;
  *(GET_ADDR(0x5555)) 	= 0x00AA;
  *(GET_ADDR(0x2AAA)) 	= 0x0055;
  *(GET_ADDR(0x5555)) 	= CMD_ERASE_CHIP;

  return check_toggle_ready(NORFLASH_BASE, 500000);
}

uint32_t norflash_writeWord(uint32_t addr, uint16_t data)
{
  volatile uint16_t *p;

  *(GET_ADDR(0x5555)) = 0x00AA;
  *(GET_ADDR(0x2AAA)) = 0x0055;
  *(GET_ADDR(0x5555)) = 0x00A0;

  p  = (uint16_t*) addr;
  *p = data;

  return check_toggle_ready(addr, 500000);
}

uint32_t norflash_writeBuff(uint32_t addr, uint16_t* data, uint16_t len)
{
  uint16_t i;
  for (i = 0; i < len; i++)
  {
    if (!norflash_writeWord(addr, data[i]))
    {
      return (FALSE);
    }
  }
  return (TRUE);
}

/******************************************************************************
 *
 * Description:
 *    Reads the security information from the chip. For an explanation
 *    see the user manual. 
 *
 * Parameters:
 *    SST_SecID   The factory programmed security segment 
 *    User_SecID  The user defined security segment
 *
 *****************************************************************************/
void norflash_secid_read(uint16_t SST_SecID[8], uint16_t User_SecID[8])
{
  uint16_t i;

  *(GET_ADDR(0x5555)) = 0x00AA;
  *(GET_ADDR(0x2AAA)) = 0x0055;
  *(GET_ADDR(0x5555)) = 0x0088;

  for (i = 0; i < 7; i++)
  {
    SST_SecID[i] = *(GET_ADDR(i));          // SST security is 0x00 - 0x07
    User_SecID[i] = *(GET_ADDR(i + 0x10)); // User security is 0x10 - 0x17
  }

  // exit command
  *(GET_ADDR(0x5555)) = CMD_ID_EXIT;
}

/******************************************************************************
 *
 * Description:
 *    Checks if the user defined security segment has been locked or not.
 *    See the user manual for more information. 
 *
 * Returns:
 *    TRUE if the segment is locked
 *
 *****************************************************************************/
uint32_t norflash_secid_getLockStatus(void)
{
  uint16_t status;

  *(GET_ADDR(0x5555)) = 0x00AA;
  *(GET_ADDR(0x2AAA)) = 0x0055;
  *(GET_ADDR(0x5555)) = 0x0088;

  // read status
  status = *(GET_ADDR(0xff));
  status &= 0x0008;

  *(GET_ADDR(0x5555)) = 0x00AA;
  *(GET_ADDR(0x2AAA)) = 0x0055;
  *(GET_ADDR(0x5555)) = CMD_ID_EXIT;

  if (!status)
    return TRUE; // locked
  return FALSE;  // not locked
}

/******************************************************************************
 *
 * Description:
 *    Lock the user security segment. CANNOT BE UNDONE. 
 *    See the user manual for more information. 
 *
 * Returns:
 *    TRUE if the segment is locked after programming
 *
 *****************************************************************************/
uint32_t norflash_secid_lockOut()
{
  // Code not verified. Use at own risk
#if 0
  *(GET_ADDR(0x5555)) = 0x00AA;
  *(GET_ADDR(0x2AAA)) = 0x0055;
  *(GET_ADDR(0x5555)) = 0x0085;
  *(GET_ADDR(0x0   )) = 0x0000;  // Write 0x0000 to any address
  
  if (check_toggle_ready(GET_ADDR(0x0), 500000)) 
  {
    return norflash_secid_getLockStatus();
  }
#endif    
  return FALSE;  
}

/******************************************************************************
 *
 * Description:
 *    Writes data to the user security segment (0x0010 - 0x0017). 
 *    See the user manual for more information. 
 *
 * Parameters:
 *    target   Must be in the range 0x10 to 0x17 
 *    data     The data to write
 *    len      The number of words to write
 * 
 * Returns:
 *    TRUE if the programming was successful
 *
 *****************************************************************************/
uint32_t norflash_secid_writeWord(uint16_t target, uint16_t* data, uint16_t len)
{
  // Code not verified. Use at own risk
#if 0
  uint16_t i;

  if ((target < 0x10) || (target > 0x17))
    return FALSE;

  if ((len > 8) || ((target + len) > 0x17))
    return FALSE;

  for (i = 0; i < len; i++)
  {
    *(GET_ADDR(0x5555)) = 0x00AA;
    *(GET_ADDR(0x2AAA)) = 0x0055;
    *(GET_ADDR(0x5555)) = 0x00A5;
    *(GET_ADDR(target + i)) = data;

    data++;

    /* Read the toggle bit to detect end-of-programming for User Sec ID.
       Do Not use Data# Polling for User_SecID_Word_Program!! */
    if (!check_toggle_ready(GET_ADDR(target + i), 500000))
      return FALSE;
  }
#endif

  return TRUE;
}

