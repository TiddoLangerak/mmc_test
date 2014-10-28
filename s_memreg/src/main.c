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
#include "memreg.h"


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
  int i;
  uint16_t* memreg = (uint16_t*)MEMREG_BASE;


  memreg_init();

  // write a bit mask which enables/disables 
  // LED1 to LED16 on the OEM Base Board
  *memreg = 0x55AA;

  while(1)
  {
    *memreg = 0xAA55;
    for(i = 0; i < 0x200000; i++);
    *memreg = 0x55AA;
    for(i = 0; i < 0x200000; i++);
  }
}



