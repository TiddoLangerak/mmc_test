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

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "lpc_types.h"
#include "lpc177x_8x_pinsel.h"
#include "lpc177x_8x_uart.h"
#include "lpc177x_8x_ssp.h"
#include "lpc177x_8x_i2c.h"
#include "lpc177x_8x_timer.h"

#include "sdram.h"
#include "lcd_driver.h"
#include "lpc_swim.h"
#include "lpc_swim_font.h"
#include "ea_lcd_board.h"
#include "pca9532.h"
#include "tsc2046_touch.h"
#include "truly_display.h"
#include "lcdb_eeprom.h"
#include "board.h"


/******************************************************************************
 * Typedefs and defines
 *****************************************************************************/


#define I2C_PORT (I2C_0)


#define MAIN_BUF_SZ 100
static char main_buf[MAIN_BUF_SZ];
static char buff[512];

static int touch_calibrated = 0;

void calibrateStart(SWIM_WINDOW_T *win);

/******************************************************************************
 * Local Functions
 *****************************************************************************/

/**
 * Draw introduction text to the display
 */
static void drawIntro(SWIM_WINDOW_T* win1)
{

  swim_clear_screen(win1, BLACK);
  swim_set_pen_color(win1, WHITE);


  swim_put_text_xy(win1, "Display: ", 10, 10);  

  if (ea_lcdb_getDisplayName(main_buf, MAIN_BUF_SZ) != LCDB_RESULT_OK) {
    swim_put_text_xy(win1, "TFT-G240320LTSW-118W-E",     60, 10);        
  }
  else {
    swim_put_text_xy(win1, (const CHAR*)main_buf,     60, 10);  
  }

  swim_put_text_xy(win1, "Mfg: ", 10, 20);  

  if (ea_lcdb_getDisplayMfg(main_buf, MAIN_BUF_SZ) != LCDB_RESULT_OK) {
    swim_put_text_xy(win1, "TRULY SEMICONDUCTORS",     60, 20);        
  }
  else {
    swim_put_text_xy(win1, (const CHAR*)main_buf, 60, 20);  
  }


  swim_put_text_xy(win1, "1. Press SW2 to for this info screen ", 10, 60);  
  swim_put_text_xy(win1, "2. Press SW3 to draw color bars ", 10, 75);  
  swim_put_text_xy(win1, "3. Press SW4 to test backlight ", 10, 90);  
  swim_put_text_xy(win1, "4. Press SW5 to test touch ", 10, 105);  

}

/**
 * Draw color bars to the display
 */
static void drawBars(SWIM_WINDOW_T* win1, uint32_t width, uint32_t height) 
{
  COLOR_T clr;
  UNS_16 xgs, ygs, curx, cury, curym, xidx;
  int idx;

  /* Compute vertical size for bars */
  ygs = height / 3;

  /* Draw Red bars */
  cury = 0;
  curx = 0;
  curym = ygs - 1;
  xgs = width / RED_COLORS;
  clr = BLACK;
  for (xidx = 0; xidx < RED_COLORS; xidx++)
  {
      swim_set_pen_color(win1, clr);
      for (idx = 0; idx <= xgs; idx++)
      {
          swim_put_line(win1, curx, cury, curx, curym);
          curx++;
      }
      clr = clr + 0x0800;
  }

  /* Draw green bars */
  cury = cury + ygs;
  curx = 0;
  curym = cury + (ygs - 1);
  xgs = width / GREEN_COLORS;
  clr = BLACK;
  for (xidx = 0; xidx < GREEN_COLORS; xidx++)
  {
      swim_set_pen_color(win1, clr);
      for (idx = 0; idx <= xgs; idx++)
      {
          swim_put_line(win1, curx, cury, curx, curym);
          curx++;
      }
      clr = clr + 0x0020;
  }

  /* Draw blue bars */
  cury = cury + ygs;
  curx = 0;
  curym = cury + (ygs - 1);
  xgs = width / BLUE_COLORS;
  clr = BLACK;
  for (xidx = 0; xidx < BLUE_COLORS; xidx++)
  {
      swim_set_pen_color(win1, clr);
      for (idx = 0; idx <= xgs; idx++)
      {
          swim_put_line(win1, curx, cury, curx, curym);
          curx++;
      }
      clr = clr + 0x0001;
  }
}

/**
 * Fade out and in by using backlight contrast
 */
static void fadeOutInDisplay(uint8_t usingTrulyDisplay)
{
  int32_t value = 100;
  
  for (value = 100; value > 0; value--) {
    if (usingTrulyDisplay)
      truly_lcd_setBacklight(value);
    else
      ea_lcdb_ctrl_backlightContrast(value);
    TIM_Waitms(3);
  }  

  for (value = 0; value < 100; value++) {
    if (usingTrulyDisplay)
      truly_lcd_setBacklight(value);
    else
      ea_lcdb_ctrl_backlightContrast(value);
    TIM_Waitms(3);
  }  
}


/**
 * Run the touch test
 */
static void runTouch(SWIM_WINDOW_T* win1, uint32_t width, uint32_t height)
{
  int32_t x = 0;
  int32_t y = 0;
  int32_t z = 0;

  touch_init();  

  if (!touch_calibrated) {
    calibrateStart(win1);
    touch_calibrated = 1;
    swim_clear_screen(win1, BLACK);
  }    

  touch_xyz(&x, &y, &z);
  if (z > 0) {
    swim_put_pixel(win1, x, y);
  }
  TIM_Waitms(2);

}

/******************************************************************************
 * Main method
 *****************************************************************************/



int main (void)
{
  SWIM_WINDOW_T win1;
  
  int32_t dev_lcd = 0;

  uint16_t state = 0;
  uint16_t lastKey = 0;
  
  int32_t framebuf = (int32_t)(SDRAM_BASE+0x10000);
  int32_t width = 0;
  int32_t height = 0;
  uint8_t usingTrulyDisplay = 0;
  lcdb_result_t result;
  LCD_PARAM_T lp;

	TIM_TIMERCFG_Type timerCfg;


  // initialize timer
  TIM_ConfigStructInit(TIM_TIMER_MODE, &timerCfg);
  TIM_Init(LPC_TIM0, TIM_TIMER_MODE, &timerCfg);

  console_init();
  i2c0_pinConfig();


  /* Initialize I2C */
  I2C_Init(I2C_PORT, 100000);
  I2C_Cmd(I2C_PORT, ENABLE);

  sdram_init();

  do {
                         
    if ((result = ea_lcdb_open(NULL, NULL, &dev_lcd)) == LCDB_RESULT_OK) {
  
      if ((result = ea_lcdb_getLcdParams(&lp)) != LCDB_RESULT_OK) {
        sprintf(buff, "ea_lcdb_getLcdParams FAILED (%d)\r\n", result);
        console_sendString((uint8_t*)buff);
        break;  
      }

      width  = lp.pixels_per_line;
      height = lp.lines_per_panel;
  
      memset((void*)(framebuf), 0x00, width*height*2);
    
      if ((result = ea_lcdb_enable(dev_lcd, framebuf)) != LCDB_RESULT_OK) {
        sprintf(buff, "ea_lcdb_enable FAILED (%d)\r\n", result);
        console_sendString((uint8_t*)buff);
        break;        
      }
    } else {
      /* Assuming that the unidentified display is a QVGA display */
      usingTrulyDisplay = 1;
      width  = 240;
      height = 320;
      memset((void*)(framebuf), 0x00, width*height*2);

      truly_lcd_open(framebuf);
    }
      
    /* Create a SWIM window */
    swim_window_open(&win1, width, height, (COLOR_T*)framebuf, 0, 0, 
      width-1, height-1, 1, WHITE, BLACK, BLACK);

    drawIntro(&win1);

    while (1) {
      state = pca9532_getLedState(FALSE);
  
      if ((state & KEY1)) {
        lastKey = KEY1;
        drawIntro(&win1);
      }
  
      else if ((state & KEY2)) {
        lastKey = KEY2;
        drawBars(&win1, width, height);
      }

      else if ((state & KEY3)) {
        lastKey = KEY3;
        fadeOutInDisplay(usingTrulyDisplay);
      }
  
      else if ((state & KEY4) || lastKey == KEY4) {
  
        if (lastKey != KEY4) {
          swim_clear_screen(&win1, BLACK);
          swim_set_pen_color(&win1, WHITE);
          swim_put_text_xy(&win1, "Start drawing ", 10, 10); 
        }
  
        lastKey = KEY4;
        runTouch(&win1, width, height);      
      }
    }

  } while(0);

  while(1) {
    ;
  }

  //truly_lcd_close(dev_lcd);
}


