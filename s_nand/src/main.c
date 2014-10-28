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

#include "string.h"
#include "stdio.h"
#include "lpc_types.h"
#include "lpc177x_8x_pinsel.h"
#include "lpc177x_8x_uart.h"
#include "lpc177x_8x_timer.h"

#include "nand.h"
#include "board.h"

/******************************************************************************
 * Typedefs and defines
 *****************************************************************************/

/******************************************************************************
 * Local Functions
 *****************************************************************************/



static char buff[2112];

/******************************************************************************
 * Main method
 *****************************************************************************/

int main (void)
{
  int i = 0;
  int blockNum = 100;
  TIM_TIMERCFG_Type timerCfg;

  /* Initialize devices */

  // initialize timer
  TIM_ConfigStructInit(TIM_TIMER_MODE, &timerCfg);
  TIM_Init(LPC_TIM0, TIM_TIMER_MODE, &timerCfg);

  console_init();


  nand_init();
  
  console_sendString((uint8_t*)"Checking blocks\r\n");      
  for (i = 0; i < NAND_NUM_BLOCKS; i++) {
    if (!nand_isBlockValid(i)) {
      sprintf(buff, "Block %d is BAD\r\n", i);
      console_sendString((uint8_t*)buff);      
    }
  }
  console_sendString((uint8_t*)"DONE\r\n\r\n"); 
  
  for (i = 0; i < nand_getPageSize(); i++) {
    buff[i] = (char)(i+1);
  }

  console_sendString((uint8_t*)"Erasing...\r\n");        
  
  if (!nand_eraseBlock(blockNum)) {
    console_sendString((uint8_t*)"Failed to erase block\r\n");        
  }

  console_sendString((uint8_t*)"Writing...\r\n");          
  if (!nand_writePage(blockNum, 0, (uint8_t*) buff)) {
    console_sendString((uint8_t*)"Failed to write to NAND\r\n");        
    return 1;
  }

  memset(buff, 0, 2112);

  console_sendString((uint8_t*)"Reading...\r\n");          
  if (!nand_readPage(blockNum, 0, (uint8_t*) buff)) {
    console_sendString((uint8_t*)"Failed to read from NAND\r\n");        
    return 1;
  }

  console_sendString((uint8_t*)"Verifing...\r\n");        
  for (i = 0; i < nand_getPageSize(); i++) {
    if (buff[i] != (char)(i+1)) {
      console_sendString((uint8_t*)"Verify failed\r\n");        
      return 1;
    }
  }
  console_sendString((uint8_t*)"Done\r\n");        


  return 0;
}



