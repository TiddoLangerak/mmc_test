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
#include "lpc177x_8x_pinsel.h"
#include "lpc177x_8x_i2c.h"
#include "lpc177x_8x_uart.h"
#include "lpc177x_8x_timer.h"

#include "oem_eeprom.h"
#include "base_eeprom.h"
#include "string.h"
#include "board.h"


/******************************************************************************
 * Typedefs and defines
 *****************************************************************************/

/*
 * Set this define to 1 if the EEPROM on the OEM board should be used.
 * If set to 0 the EEPROM on the base board will be used instead.
 */
#define USE_OEM_EEPROM 0

#define I2C_PORT (I2C_0)

#define E_WRITE_LEN 200

#define OEM_EEPROM_DEV OEM_EEPROM_256

/******************************************************************************
 * Local Functions
 *****************************************************************************/

static void doInit (void)
{
#if (USE_OEM_EEPROM)
  oem_eeprom_init();
#else
  base_eeprom_init();
#endif  
}

static int16_t doRead(uint8_t* buf, uint16_t offset, uint16_t len)
{
#if (USE_OEM_EEPROM)
  return oem_eeprom_read(OEM_EEPROM_DEV, buf, offset, len);
#else
  return base_eeprom_read(buf, offset, len);
#endif  
}

static int16_t doWrite(uint8_t* buf, uint16_t offset, uint16_t len)
{
#if (USE_OEM_EEPROM)
  return oem_eeprom_write(OEM_EEPROM_DEV, buf, offset, len);
#else
  return base_eeprom_write(buf, offset, len);
#endif  
}


/******************************************************************************
 * Main method
 *****************************************************************************/

int main (void)
{
  int i = 0;
  int16_t len = 0;
  uint16_t offset = 240;
  uint8_t b[E_WRITE_LEN];
  TIM_TIMERCFG_Type timerCfg;

  // initialize timer
  TIM_ConfigStructInit(TIM_TIMER_MODE, &timerCfg);
  TIM_Init(LPC_TIM0, TIM_TIMER_MODE, &timerCfg);

  i2c0_pinConfig();

  /* Initialize I2C */
  I2C_Init(I2C_PORT, 100000);
  I2C_Cmd(I2C_PORT, ENABLE);

  console_init();

  doInit();

  for (i = 0; i < E_WRITE_LEN; i++) {
    b[i] = (uint8_t)(i+1);
  }

  len = doWrite(b, offset, E_WRITE_LEN);

  if (len != E_WRITE_LEN) {
    console_sendString((uint8_t*)"EEPROM: Failed to write data\r\n");
    return 1;
  }

  console_sendString((uint8_t*)"EEPROM: Data written\r\n");

  memset(b, 0, E_WRITE_LEN);

  console_sendString((uint8_t*)"EEPROM: Reading\r\n");
  len = doRead(b, offset, E_WRITE_LEN);

  if (len != E_WRITE_LEN) {
    console_sendString((uint8_t*)"EEPROM: Failed to read all data\r\n");
    return 1;
  }

  console_sendString((uint8_t*)"EEPROM: Verifing\r\n");
  for (i = 0; i < E_WRITE_LEN; i++) {
    if (b[i] != (uint8_t)(i+1)) {
      console_sendString((uint8_t*)"EEPROM: Invalid data\r\n");

      return 1;
    }
  }

  console_sendString((uint8_t*)"EEPROM: OK\r\n");


  while(1);
}


