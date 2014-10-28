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
#include "lpc177x_8x_adc.h"
#include "lpc177x_8x_timer.h"
#include "lpc177x_8x_clkpwr.h"



/******************************************************************************
 * Typedefs and defines
 *****************************************************************************/


#define UART_RS232_PORT ((LPC_UART_TypeDef*)(LPC_UART2))


/******************************************************************************
 * Local Functions
 *****************************************************************************/


/******************************************************************************
 * Main method
 *****************************************************************************/

int main (void)
{

  char buff[30];

  TIM_TIMERCFG_Type timerCfg;
  UART_CFG_Type UARTConfigStruct;

  /* Initialize devices */

  // initialize timer
  TIM_ConfigStructInit(TIM_TIMER_MODE, &timerCfg);
  TIM_Init(LPC_TIM0, TIM_TIMER_MODE, &timerCfg);


  // pinsel for UART2
  PINSEL_ConfigPin(4,22,2); /* LPC4088 P4.22 as U2_TXD*/
  PINSEL_ConfigPin(4,23,2); /* LPC4088 P4.23 as U2_RXD*/

  UART_ConfigStructInit(&UARTConfigStruct);
  UARTConfigStruct.Baud_rate = 115200;

  // Initialize UART peripheral
  UART_Init(UART_RS232_PORT, &UARTConfigStruct);

  // Enable UART Transmit
  UART_TxCmd(UART_RS232_PORT, ENABLE);



  while(1) {

    sprintf(buff, "hepp\r\n");
    UART_Send(UART_RS232_PORT, (uint8_t*)buff, strlen(buff), BLOCKING);

    TIM_Waitms(1000);

  }
}



