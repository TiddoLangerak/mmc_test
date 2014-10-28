/*****************************************************************************
 *
 *   Copyright(C) 2012, Embedded Artists AB
 *   All rights reserved.
 *
 *   This example is writing and reading data from the eeprom. The example
 *   can be configured to access the eeprom on the OEM Board or on the
 *   base board.
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
#include "lpc177x_8x_dac.h"
#include "lpc177x_8x_timer.h"

#include "board.h"

/******************************************************************************
 * Typedefs and defines
 *****************************************************************************/


/******************************************************************************
 * External Functions
 *****************************************************************************/

extern const unsigned char sound_8k[];
extern int sound_sz;

/******************************************************************************
 * Local Functions
 *****************************************************************************/


/******************************************************************************
 * Main method
 *****************************************************************************/

int main (void)
{
  uint32_t cnt = 0;
  uint32_t off = 0;
  uint32_t sampleRate = 0;
  uint32_t delay = 0;

	TIM_TIMERCFG_Type timerCfg;

  /* Initialize devices */

  // initialize timer
  TIM_ConfigStructInit(TIM_TIMER_MODE, &timerCfg);
  TIM_Init(LPC_TIM0, TIM_TIMER_MODE, &timerCfg);

  console_init();
  
  DAC_Init(0);

  /* ChunkID */
  if (sound_8k[cnt] != 'R' && sound_8k[cnt+1] != 'I' &&
      sound_8k[cnt+2] != 'F' && sound_8k[cnt+3] != 'F')
  {
    console_sendString((uint8_t*)"Wrong format (RIFF)\r\n");
    return 0;
  }
  cnt+=4;

  /* skip chunk size*/
  cnt += 4;

  /* Format */
  if (sound_8k[cnt] != 'W' && sound_8k[cnt+1] != 'A' &&
      sound_8k[cnt+2] != 'V' && sound_8k[cnt+3] != 'E')
  {
    console_sendString((uint8_t*)"Wrong format (WAVE)\r\n");
    return 0;
  }
  cnt+=4;

  /* SubChunk1ID */
  if (sound_8k[cnt] != 'f' && sound_8k[cnt+1] != 'm' &&
      sound_8k[cnt+2] != 't' && sound_8k[cnt+3] != ' ')
  {
    console_sendString((uint8_t*)"Missing fmt\r\n");
    return 0;
  }
  cnt+=4;

  /* skip chunk size, audio format, num channels */
  cnt+= 8;

  sampleRate = (sound_8k[cnt] | (sound_8k[cnt+1] << 8) |
          (sound_8k[cnt+2] << 16) | (sound_8k[cnt+3] << 24));

  if (sampleRate != 8000) {
    console_sendString((uint8_t*)"Only 8kHz supported\r\n");
    return 0;
  }

  delay = 1000000 / sampleRate;

  cnt+=4;

  /* skip byte rate, align, bits per sample */
  cnt += 8;

  /* SubChunk2ID */
  if (sound_8k[cnt] != 'd' && sound_8k[cnt+1] != 'a' &&
      sound_8k[cnt+2] != 't' && sound_8k[cnt+3] != 'a')
  {
    console_sendString((uint8_t*)"Missing data\r\n");
    return 0;
  }
  cnt += 4;

  /* skip chunk size */
  cnt += 4;

  off = cnt;

  while(1)
  {
    cnt = off;
    while(cnt++ < sound_sz)
    {
      DAC_UpdateValue(0, (sound_8k[cnt]&0x3FF));

      TIM_Waitus(delay);
    }
  }

}



