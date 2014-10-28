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
#include "lpc_types.h"
#include "lpc177x_8x_timer.h"
#include "lpc177x_8x_pinsel.h"
#include "lpc177x_8x_uart.h"

#include "sdram.h"
#include "board.h"


/******************************************************************************
 * Typedefs and defines
 *****************************************************************************/


/******************************************************************************
 * Local Functions
 *****************************************************************************/

static void led_blinky(void)
{
  volatile uint32_t i;
  LPC_GPIO2->SET |= 1<<10;
  for(i = 1000000; i > 0; i--);

  LPC_GPIO2->CLR |= 1<<10;
  for(i = 1000000; i > 0; i--);
}

/******************************************************************************
 * Main method
 *****************************************************************************/

int main (void)
{
  volatile uint8_t *char_wr_ptr;
  volatile uint16_t *short_wr_ptr;
  volatile uint32_t *wr_ptr; 

  uint32_t data;
  uint32_t lfsr;
  uint32_t i, j;

  TIM_TIMERCFG_Type timerCfg;



  // initialize timer
  TIM_ConfigStructInit(TIM_TIMER_MODE, &timerCfg);
  TIM_Init(LPC_TIM0, TIM_TIMER_MODE, &timerCfg);

  console_init();

  sdram_init();

  //LED as output
  LPC_GPIO2->DIR |= 1<<10;

  while (1)
  {
    wr_ptr = (uint32_t *)SDRAM_BASE;
    char_wr_ptr = (uint8_t *)wr_ptr;

    /* Clear content before 8 bit access test */
    for (i = 0; i < SDRAM_SIZE/4; i++)
    {
      *wr_ptr = 0;
      *wr_ptr = 0xFFFFFFFF; //Toggle

      *wr_ptr++ = 0;  //Reset
    }

    console_sendString((uint8_t*)"Testing  8-bit access...");

    /* 8 bit write */
    for (i = 0; i < SDRAM_SIZE/0x40000; i++)
    {
      for (j = 0; j < 0x10000; j++)
      {
        *char_wr_ptr++ = (i + j) & 0xFF;
        *char_wr_ptr++ = ((i + j) & 0xFF00) >> 8;
        *char_wr_ptr++ = ((i + j) + 1) & 0xFF;
        *char_wr_ptr++ = (((i + j) + 1) & 0xFF00) >> 8;
      }
      led_blinky();
    }

    /* verifying */
    wr_ptr = (uint32_t *)SDRAM_BASE;
    for (i = 0; i < SDRAM_SIZE/0x40000; i++)
    {
      for (j = 0; j < 0x10000; j++)
      {
        data = *wr_ptr;          
        if (data != (((((i + j) + 1) & 0xFFFF) << 16) | ((i + j) & 0xFFFF)))	/* be aware of endianess */
        {
          while (1);  /* fatal error */
        }
        wr_ptr++;
      }
      led_blinky();
    }

    console_sendString((uint8_t*)"OK\r\n");

    wr_ptr = (uint32_t *)SDRAM_BASE;
    short_wr_ptr = (uint16_t *)wr_ptr;
    /* Clear content before 16 bit access test */
    for (i = 0; i < SDRAM_SIZE/4; i++)
    {
      *wr_ptr++ = 0;
    }

    console_sendString((uint8_t*)"Testing 16-bit access...");

    /* 16 bit write */
    for (i = 0; i < SDRAM_SIZE/0x40000; i++)
    {
      for (j = 0; j < 0x10000; j++)
      {
        *short_wr_ptr++ = (i + j);
        *short_wr_ptr++ = (i + j) + 1;
      }
      led_blinky();
    }

    /* Verifying */
    wr_ptr = (uint32_t *)SDRAM_BASE;
    for (i = 0; i < SDRAM_SIZE/0x40000; i++)
    {
      for (j = 0; j < 0x10000; j++)
      {
        data = *wr_ptr;          
        if (data != (((((i + j) + 1) & 0xFFFF) << 16) | ((i + j) & 0xFFFF)))	/* be aware of endianess */
        {
          while (1);	/* fatal error */
        }
        wr_ptr++;
      }
      led_blinky();
    }

    console_sendString((uint8_t*)"OK\r\n");

    wr_ptr = (uint32_t *)SDRAM_BASE;
    short_wr_ptr = (uint16_t *)wr_ptr;
    /* Clear content before 32 bit access test */
    for (i = 0; i < SDRAM_SIZE/4; i++)
    {
      *wr_ptr++ = 0;
    }

    console_sendString((uint8_t*)"Testing 32-bit access...");

    wr_ptr = (uint32_t *)SDRAM_BASE;
    lfsr = 1;
    for (i = 0; i < (SDRAM_SIZE/4); i++) //4 bytes per word
    {
      *wr_ptr++ = lfsr;	  
      lfsr = (lfsr >> 1) ^ (unsigned int)(0 - (lfsr & 1u) & 0xd0000001u);
    }
    led_blinky();

    //Verify
    wr_ptr = (uint32_t *)SDRAM_BASE;
    lfsr = 1;
    for (i = 0; i < (SDRAM_SIZE/4); i++) //4 bytes per word
    {
      if (*wr_ptr++ != lfsr)
      {
        while(1);/* fatal error */
      }
      lfsr = (lfsr >> 1) ^ (unsigned int)(0 - (lfsr & 1u) & 0xd0000001u); 
    }

    console_sendString((uint8_t*)"OK\r\n\r\n");

  }
}


