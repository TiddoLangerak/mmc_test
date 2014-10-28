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
#include "lpc177x_8x_pinsel.h"
#include "lpc177x_8x_timer.h"
#include "lpc177x_8x_uart.h"
#include "joystick.h"
#include "board.h"



/******************************************************************************
 * Typedefs and defines
 *****************************************************************************/

#define UART_PORT (LPC_UART0)


/******************************************************************************
 * Local Functions
 *****************************************************************************/


/******************************************************************************
 * Main method
 *****************************************************************************/

int main (void)
{
	TIM_TIMERCFG_Type timerCfg;

  uint8_t joyState = 0;

  /* Initialize devices */

  // initialize timer
  TIM_ConfigStructInit(TIM_TIMER_MODE, &timerCfg);
  TIM_Init(LPC_TIM0, TIM_TIMER_MODE, &timerCfg);

  console_init();

  joystick_init();        

  while(1) {
    joyState = joystick_read();

    if (joyState & JOYSTICK_UP) {
      console_sendString((uint8_t*)"Up ");
    }

    if (joyState & JOYSTICK_DOWN) {
      console_sendString((uint8_t*)"Down ");
    }

    if (joyState & JOYSTICK_LEFT) {
      console_sendString((uint8_t*)"Left ");
    }

    if (joyState & JOYSTICK_RIGHT) {
      console_sendString((uint8_t*)"Right ");
    }

    if (joyState & JOYSTICK_CENTER) {
      console_sendString((uint8_t*)"Center ");
    }

    if (joyState != 0) {
      console_sendString((uint8_t*)"\r\n");
    }

    TIM_Waitms(200);

  }
}


