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
#include "lpc177x_8x_timer.h"
#include "memreg.h"


/******************************************************************************
 * Defines and typedefs
 *****************************************************************************/


/******************************************************************************
 * External global variables
 *****************************************************************************/

/******************************************************************************
 * Local variables
 *****************************************************************************/

static void pinConfig(void)
{
  LPC_IOCON->P3_0 |= 1; /* D0 @ P3.0 */
  LPC_IOCON->P3_1 |= 1; /* D1 @ P3.1 */
  LPC_IOCON->P3_2 |= 1; /* D2 @ P3.2 */
  LPC_IOCON->P3_3 |= 1; /* D3 @ P3.3 */

  LPC_IOCON->P3_4 |= 1; /* D4 @ P3.4 */
  LPC_IOCON->P3_5 |= 1; /* D5 @ P3.5 */
  LPC_IOCON->P3_6 |= 1; /* D6 @ P3.6 */
  LPC_IOCON->P3_7 |= 1; /* D7 @ P3.7 */

  LPC_IOCON->P3_8 |= 1; /* D8 @ P3.8 */
  LPC_IOCON->P3_9 |= 1; /* D9 @ P3.9 */
  LPC_IOCON->P3_10 |= 1; /* D10 @ P3.10 */
  LPC_IOCON->P3_11 |= 1; /* D11 @ P3.11 */

  LPC_IOCON->P3_12 |= 1; /* D12 @ P3.12 */
  LPC_IOCON->P3_13 |= 1; /* D13 @ P3.13 */
  LPC_IOCON->P3_14 |= 1; /* D14 @ P3.14 */
  LPC_IOCON->P3_15 |= 1; /* D15 @ P3.15 */

  LPC_IOCON->P3_16 |= 1; /* D16 @ P3.16 */
  LPC_IOCON->P3_17 |= 1; /* D17 @ P3.17 */
  LPC_IOCON->P3_18 |= 1; /* D18 @ P3.18 */
  LPC_IOCON->P3_19 |= 1; /* D19 @ P3.19 */

  LPC_IOCON->P3_20 |= 1; /* D20 @ P3.20 */
  LPC_IOCON->P3_21 |= 1; /* D21 @ P3.21 */
  LPC_IOCON->P3_22 |= 1; /* D22 @ P3.22 */
  LPC_IOCON->P3_23 |= 1; /* D23 @ P3.23 */

  LPC_IOCON->P3_24 |= 1; /* D24 @ P3.24 */
  LPC_IOCON->P3_25 |= 1; /* D25 @ P3.25 */
  LPC_IOCON->P3_26 |= 1; /* D26 @ P3.26 */
  LPC_IOCON->P3_27 |= 1; /* D27 @ P3.27 */

  LPC_IOCON->P3_28 |= 1; /* D28 @ P3.28 */
  LPC_IOCON->P3_29 |= 1; /* D29 @ P3.29 */
  LPC_IOCON->P3_30 |= 1; /* D30 @ P3.30 */
  LPC_IOCON->P3_31 |= 1; /* D31 @ P3.31 */

  LPC_IOCON->P4_0 |= 1; /* A0 @ P4.0 */
  LPC_IOCON->P4_1 |= 1; /* A1 @ P4.1 */
  LPC_IOCON->P4_2 |= 1; /* A2 @ P4.2 */
  LPC_IOCON->P4_3 |= 1; /* A3 @ P4.3 */

  LPC_IOCON->P4_4 |= 1; /* A4 @ P4.4 */
  LPC_IOCON->P4_5 |= 1; /* A5 @ P4.5 */
  LPC_IOCON->P4_6 |= 1; /* A6 @ P4.6 */
  LPC_IOCON->P4_7 |= 1; /* A7 @ P4.7 */

  LPC_IOCON->P4_8 |= 1; /* A8 @ P4.8 */
  LPC_IOCON->P4_9 |= 1; /* A9 @ P4.9 */
  LPC_IOCON->P4_10 |= 1; /* A10 @ P4.10 */
  LPC_IOCON->P4_11 |= 1; /* A11 @ P4.11 */

  LPC_IOCON->P4_12 |= 1; /* A12 @ P4.12 */
  LPC_IOCON->P4_13 |= 1; /* A13 @ P4.13 */
  LPC_IOCON->P4_14 |= 1; /* A14 @ P4.14 */
  LPC_IOCON->P4_15 |= 1; /* A15 @ P4.15 */

  LPC_IOCON->P4_16 |= 1; /* A16 @ P4.16 */
  LPC_IOCON->P4_17 |= 1; /* A17 @ P4.17 */
  LPC_IOCON->P4_18 |= 1; /* A18 @ P4.18 */
  LPC_IOCON->P4_19 |= 1; /* A19 @ P4.19 */

  LPC_IOCON->P4_20 |= 1; /* A20 @ P4.20 */
  LPC_IOCON->P4_21 |= 1; /* A21 @ P4.21 */
  LPC_IOCON->P4_22 |= 1; /* A22 @ P4.22 */
  LPC_IOCON->P4_23 |= 1; /* A23 @ P4.23 */

  LPC_IOCON->P4_24 |= 1; /* OEN @ P4.24 */
  LPC_IOCON->P4_25 |= 1; /* WEN @ P4.25 */
  LPC_IOCON->P4_26 |= 1; /* BLSN[0] @ P4.26 */
  LPC_IOCON->P4_27 |= 1; /* BLSN[1] @ P4.27 */

  LPC_IOCON->P4_28 |= 1; /* BLSN[2] @ P4.28 */
  LPC_IOCON->P4_29 |= 1; /* BLSN[3] @ P4.29 */
  LPC_IOCON->P4_30 |= 1; /* CSN[0] @ P4.30 */
  LPC_IOCON->P4_31 |= 1; /* CSN[1] @ P4.31 */

  LPC_IOCON->P2_14 |= 1; /* CSN[2] @ P2.14 */
  LPC_IOCON->P2_15 |= 1; /* CSN[3] @ P2.15 */
}



/******************************************************************************
 * Local Functions
 *****************************************************************************/



/******************************************************************************
 * Public Functions
 *****************************************************************************/



/******************************************************************************
 *
 * Description:
 *    Initialize the NOR Flash
 *
 *****************************************************************************/
uint32_t memreg_init (void)
{
  LPC_SC->PCONP      |= 0x00000800;

  LPC_EMC->Control = 0x00000001;

  LPC_EMC->Config  = 0x00000000;


  pinConfig();

  LPC_EMC->StaticConfig2   = 0x00000001;
#if 0
  LPC_EMC->StaticWaitWen2  = 0x00000003; /* ( n + 1 ) -> 4 clock cycles */
  LPC_EMC->StaticWaitOen2  = 0x00000003; /* ( n ) -> 0 clock cycles */
  LPC_EMC->StaticWaitRd2   = 0x00000006; /* ( n + 1 ) -> 7 clock cycles */
  LPC_EMC->StaticWaitPage2 = 0x00000003; /* ( n + 1 ) -> 4 clock cycles */
  LPC_EMC->StaticWaitWr2   = 0x00000005; /* ( n + 2 ) -> 7 clock cycles */
  LPC_EMC->StaticWaitTurn2 = 0x00000003; /* ( n + 1 ) -> 4 clock cycles */
#endif



  return FALSE;
}


