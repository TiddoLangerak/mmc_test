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


#include <string.h>
#include <stdio.h>
#include "lpc_types.h"
#include "lpc177x_8x_i2c.h"
#include "lpc177x_8x_uart.h"
#include "lpc177x_8x_timer.h"
#include "norflash.h"
#include "board.h"

/******************************************************************************
 * Typedefs and defines
 *****************************************************************************/


/******************************************************************************
 * Local Functions
 *****************************************************************************/

/*
 * Erased sectors are filled with 0xff so to store a counter in NOR flash
 * a zero is added each time for a maximum of 128 zeros.
 *
 * This function counts the number of stored zeroes. 
 */
static int countWrites()
{
  uint16_t data;
  uint16_t* mem = (uint16_t*)NORFLASH_BASE;
  int i, words;
  int count = 0;

  for (words = 0; words < 8; words++)
  {
    data = *mem++;
    for (i = 0; i < 16; i++)
    {
      if ((data & 0x1))
      {
        // Found last zero
        return count;
      }
      count++;
      data = data>>1;
    }
  }
  return count;
}

static void storeWrites()
{
  uint16_t data;
  uint16_t* mem = (uint16_t*)NORFLASH_BASE;
  int words;

  for (words = 0; words < 8; words++)
  {
    data = *mem;
    if (data > 0)
    {
      data = (data << 1) & 0xffff;
      norflash_writeWord((uint32_t)mem, data);
      return;
    }
    *mem++;
  }
}

/******************************************************************************
 * Main method
 *****************************************************************************/

int main (void)
{
  geometry_t geometry;
	TIM_TIMERCFG_Type timerCfg;
  char buff[60];
  int numWrites;
  uint16_t sec[8];
  uint16_t user[8];

  /* Initialize devices */

  // initialize timer
  TIM_ConfigStructInit(TIM_TIMER_MODE, &timerCfg);
  TIM_Init(LPC_TIM0, TIM_TIMER_MODE, &timerCfg);

  console_init();  

  norflash_init();
  
  /* Read and print information about the NOR flash */
  norflash_getGeometry(&geometry);
  sprintf(buff, "NOR: chip size %d bytes\r\n", geometry.device_size);
  console_sendString((uint8_t*)buff);
  sprintf(buff, "NOR: %d sectors with %d bytes/sector\r\n", geometry.num_sectors, geometry.sector_size);
  console_sendString((uint8_t*)buff);
  sprintf(buff, "NOR: %d blocks with %d bytes/block\r\n", geometry.num_blocks, geometry.block_size);
  console_sendString((uint8_t*)buff);

  /* Read and print the security information on the NOR flash */
  norflash_secid_read(sec, user);
  sprintf(buff, "NOR: Factory Security Info: 0x%x 0x%x 0x%x 0x%x  0x%x 0x%x 0x%x 0x%x\r\n", 
          sec[0], sec[1], sec[2], sec[3],
          sec[4], sec[5], sec[6], sec[7]);
  console_sendString((uint8_t*)buff);
  sprintf(buff, "NOR: User Security Info: 0x%x 0x%x 0x%x 0x%x  0x%x 0x%x 0x%x 0x%x\r\n", 
          user[0], user[1], user[2], user[3],
          user[4], user[5], user[6], user[7]);
  console_sendString((uint8_t*)buff);
  sprintf(buff, "NOR: User Security Info locked: %s\r\n", (norflash_secid_getLockStatus()?"YES":"NO"));
  console_sendString((uint8_t*)buff);

#if 1
  /*
   * Reads the first 8 bytes to see how many times this program has been
   * executed and then increases that number. If the execution counter 
   * has reached 128 times the counter is no longer updated to prevent
   * excessive writes.
   */
  numWrites = countWrites();
  if (numWrites == 0)
  {
    sprintf(buff, "This program has never been executed\r\n");
    console_sendString((uint8_t*)buff);
    norflash_eraseSector(NORFLASH_BASE);

    storeWrites();
  }
  else if (numWrites < 128)
  {
    sprintf(buff, "This program has been executed %d times before\r\n", numWrites);
    console_sendString((uint8_t*)buff);

    storeWrites();
  }
  else
  {
    console_sendString((uint8_t*)"This program has been executed >= 128 times\r\n");
  }
#else
  /* 
   * To reset the execution counter
   */
  norflash_eraseSector(NORFLASH_BASE);
  console_sendString((uint8_t*)"The program execution counter has been resetted\r\n");
#endif
}



