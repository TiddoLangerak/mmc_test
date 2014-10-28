/*****************************************************************************
 *
 *   Copyright(C) 2012, Embedded Artists AB
 *   All rights reserved.
 *
 *   This example is using the PCA9532 driver to control the LEDs on
 *   the base board and monitor the states of the buttons. LED1 to LED4
 *   are turned on/off when pressing button 1 to 4 respectively
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


#include "lpc177x_8x_pinsel.h"
#include "lpc177x_8x_timer.h"
#include "lpc177x_8x_i2c.h"
#include "lpc_types.h"

#include "system_lpc177x_8x.h"

#include "pca9532.h"
#include "board.h"

#define I2C_PORT (I2C_0)

/******************************************************************************
 * Local Functions
 *****************************************************************************/

static void updateVerticalLeds(void) 
{
  static int vState = 0;
  
  switch(vState) {
    case 0:
        vState = 1;
       pca9532_setLeds(LED5, (~LED5&LED_MASK));    
        break;
    case 1:
        vState = 2;
        pca9532_setLeds(LED6, (~LED6&LED_MASK));    
        break;
    case 2:
        vState = 3;            
        pca9532_setLeds(LED7, (~LED7&LED_MASK));    
        break;
    case 3:
        vState = 4;
        pca9532_setLeds(LED8, (~LED8&LED_MASK));    
        break;
    case 4:
        vState = 5;
        pca9532_setLeds(LED7, (~LED7&LED_MASK));    
        break;
    case 5:
        vState = 0;
         pca9532_setLeds(LED6, (~LED6&LED_MASK));    
        break;
  }        
}

static void updateHorizontalLeds(void) 
{
  uint16_t state = 0;
  
  state = (pca9532_getLedState(FALSE));

  if (state & KEY1) {
    pca9532_setLeds((LED1 | (state & LED_MASK)), 
      (~(state & LED_MASK)&LED_MASK));
      state |= LED1;      
  }

  if (state & KEY2) {
    pca9532_setLeds((LED2 | (state & LED_MASK)), 
      (~(state & LED_MASK)&LED_MASK));    
    state |= LED2;
  }

  if (state & KEY3) {
    pca9532_setLeds((LED3 | (state & LED_MASK)), 
      (~(state & LED_MASK)&LED_MASK));    
  state |= LED3;
  }

  if (state & KEY4) {
    pca9532_setLeds((LED4 | (state & LED_MASK)), 
      (~(state & LED_MASK)&LED_MASK));    
    state |= LED4;
  }  
        
}

void SysTick_Handler(void)
{
	return;
}

/******************************************************************************
 * Main method
 *****************************************************************************/

int main (void)
{

	TIM_TIMERCFG_Type timerCfg;
  

  // initialize timer
  TIM_ConfigStructInit(TIM_TIMER_MODE, &timerCfg);
  TIM_Init(LPC_TIM0, TIM_TIMER_MODE, &timerCfg);
	
	/* Generate system tick at 10Hz */
	SysTick_Config(SystemCoreClock/10);
	
  i2c0_pinConfig();

  /* Initialize I2C */
  I2C_Init(I2C_PORT, 100000);
  I2C_Cmd(I2C_PORT, ENABLE);

	while(1) {

    updateVerticalLeds();
    updateHorizontalLeds();

		
		/* Sleep until SysTick interrupt is generated */
    __WFI();
    __WFI();
	}

}


