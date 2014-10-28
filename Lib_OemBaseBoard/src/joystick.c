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
#include "lpc177x_8x_gpio.h"
#include "joystick.h"

/******************************************************************************
 * Defines and typedefs
 *****************************************************************************/

/******************************************************************************
 * External global variables
 *****************************************************************************/

/******************************************************************************
 * Local variables
 *****************************************************************************/


/******************************************************************************
 * Local Functions
 *****************************************************************************/


/******************************************************************************
 * Public Functions
 *****************************************************************************/

/******************************************************************************
 *
 * Description:
 *    Initialize the Joystick Driver
 *
 *****************************************************************************/
void joystick_init (void)
{
  /* set to GPIO function for the 5 pins used with the joystick */
  LPC_IOCON->P2_27 &= ~(0x07);
  LPC_IOCON->P2_26 &= ~(0x07);
  LPC_IOCON->P2_25 &= ~(0x07);
  LPC_IOCON->P2_22 &= ~(0x07);
  LPC_IOCON->P2_23 &= ~(0x07);

  /* set the pins as inputs */
  LPC_GPIO2->DIR &= ~(1<<22|1<<23|1<<25|1<<26|1<<27);
}

/******************************************************************************
 *
 * Description:
 *    Read the joystick status
 *
 * Returns:
 *   The joystick status. The returned value is a bit mask. More than one
 *   direction may be active at any given time (e.g. UP and RIGHT)
 *
 *****************************************************************************/
uint8_t joystick_read(void)
{
    uint8_t status = 0;
    uint32_t pinVal = 0;

    pinVal = LPC_GPIO2->PIN; 
    pinVal = pinVal;

    if ((pinVal & (1 << 27)) == 0) {
      status |= JOYSTICK_DOWN;
    }

    if ((pinVal & (1 << 26)) == 0) {
      status |= JOYSTICK_RIGHT;
    }

    if ((pinVal & (1 << 25)) == 0) {
      status |= JOYSTICK_UP;
    }

    if ((pinVal & (1 << 23)) == 0) {
      status |= JOYSTICK_LEFT;
    }

    if ((pinVal & (1 << 22)) == 0) {
      status |= JOYSTICK_CENTER;
    }

    return status;
}





