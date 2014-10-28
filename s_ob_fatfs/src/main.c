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
#include "lpc177x_8x_pinsel.h"
#include "lpc177x_8x_timer.h"
#include "lpc177x_8x_uart.h"
#include "lpc177x_8x_ssp.h"

#include "system_LPC177x_8x.h"

#include "sdram.h"
#include "diskio.h"
#include "ff.h"

#include "board.h"



/******************************************************************************
 * Typedefs and defines
 *****************************************************************************/



/* UART buffer */
static char buff[512];

/******************************************************************************
 * Local Functions
 *****************************************************************************/

void SysTick_Handler(void)
{
  disk_timerproc();
	return;
}

static FATFS lFatfs[1];
static FILINFO Finfo;

/******************************************************************************
 * Main method
 *****************************************************************************/

int main (void)
{
  DIR dir;
  BYTE res;
  FIL file;
  UINT numRead = 0;
  UINT numWritten = 0;
  TIM_TIMERCFG_Type timerCfg;
    

  // initialize timer
  TIM_ConfigStructInit(TIM_TIMER_MODE, &timerCfg);
  TIM_Init(LPC_TIM0, TIM_TIMER_MODE, &timerCfg);

  console_init();

  SysTick_Config(SystemCoreClock/10);


  console_sendString((uint8_t*)"FatFS Demo\r\n");


  res = f_mount(0, &lFatfs[0]);
  if (res != FR_OK) {
    sprintf(buff, "Failed to mount 0: %d \r\n", res);
    console_sendString((uint8_t*)buff);
    return 0;
  }

  console_sendString((uint8_t*)"Opening '/'\r\n");
  res = f_opendir(&dir, "/");
  if (res) {
    sprintf(buff, "Failed to open /: %d \r\n", res);
    console_sendString((uint8_t*)buff);
    return 0;
  }


  console_sendString((uint8_t*)"Listing content of '/'\r\n");
  for(;;) {
    res = f_readdir(&dir, &Finfo);
    if ((res != FR_OK) || !Finfo.fname[0]) break;
    
    console_sendString((uint8_t*)&(Finfo.fname[0]));
    console_sendString((uint8_t*)"\r\n");
  
  }

  console_sendString((uint8_t*)"\r\nOpening a file\r\n");
  res = f_open (&file, "ip.txt", FA_READ);
  if (res) {
    sprintf(buff, "Failed to open ip.txt: %d \r\n", res);
    console_sendString((uint8_t*)buff);
    return 0;
  }

  console_sendString((uint8_t*)"Reading content of the file\r\n");
  res =  f_read (&file, buff, 100, &numRead);			/* Read data from a file */
  if (res || numRead <= 0) {
    sprintf(buff, "Failed to read ip.txt: %d \r\n", res);
    console_sendString((uint8_t*)buff);
    return 0;
  }

  buff[numRead] = '\0';
  console_sendString((uint8_t*)buff);

  console_sendString((uint8_t*)"\r\nClosing file\r\n");
  res =  f_close (&file);
  if (res) {
    sprintf(buff, "Failed to close ip.txt: %d \r\n", res);
    console_sendString((uint8_t*)buff);
    return 0;
  }

  console_sendString((uint8_t*)"\r\nOpening a file for writing\r\n");
  res = f_open (&file, "new.txt", (FA_CREATE_ALWAYS|FA_READ|FA_WRITE));
  if (res) {
    sprintf(buff, "Failed to open new.txt: %d \r\n", res);
    console_sendString((uint8_t*)buff);
    return 0;
  }

  console_sendString((uint8_t*)"Writing  to the file\r\n");
  res = f_write (&file, "Data written to file\r\n", 22, &numWritten);
  if (res) {
    sprintf(buff, "Failed to write to new.txt: %d \r\n", res);
    console_sendString((uint8_t*)buff);
    return 0;
  }

  console_sendString((uint8_t*)"Closing file\r\n");
  res =  f_close (&file);
  if (res) {
    sprintf(buff, "Failed to close new.txt: %d \r\n", res);
    console_sendString((uint8_t*)buff);
    return 0;
  }


   while(1);

}


