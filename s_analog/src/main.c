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
#include "string.h"
#include "stdio.h"
#include "lpc177x_8x_pinsel.h"
#include "lpc177x_8x_uart.h"
#include "lpc177x_8x_adc.h"
#include "lpc177x_8x_timer.h"

#include "board.h"

/******************************************************************************
 * Typedefs and defines
 *****************************************************************************/

/******************************************************************************
 * Local Functions
 *****************************************************************************/


/******************************************************************************
 * Main method
 *****************************************************************************/

int main (void)
{
  uint32_t a2;
  char buff[30];
  TIM_TIMERCFG_Type timerCfg;
	
  // initialize timer
  TIM_ConfigStructInit(TIM_TIMER_MODE, &timerCfg);
  TIM_Init(LPC_TIM0, TIM_TIMER_MODE, &timerCfg);

  console_init();
  trimpot_init();

  while(1) {

		// Start conversion
		ADC_StartCmd(LPC_ADC, ADC_START_NOW);

		//Wait conversion complete
		while (!(ADC_ChannelGetStatus(LPC_ADC, ADC_CH_TRIMPOT, ADC_DATA_DONE)));

		a2 = ADC_ChannelGetData(LPC_ADC, ADC_CH_TRIMPOT);
  
    sprintf(buff, "a0.2=%d\r\n", (int)a2);
    console_sendString((uint8_t*)buff);

    TIM_Waitms(1000);

  }
}



