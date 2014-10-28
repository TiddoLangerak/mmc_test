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
#include "lcdb_eeprom.h"
#include "board.h"
#include "tda19988.h"
#include "tfp410.h"


/******************************************************************************
 * Typedefs and defines
 *****************************************************************************/

#define I2C_PORT (I2C_0)
#define MAIN_BUF_SZ 100


typedef struct
{
  const LCD_PARAM_T* lcd_params;
  osc_e              oscillator;
  tda19988_res       tda_res;
  char*              description;
} setting_t;

#define NUM_RESOLUTIONS  (sizeof(resolutions)/sizeof(setting_t))

/******************************************************************************
 * Local Variables
 *****************************************************************************/

static char main_buf[MAIN_BUF_SZ];
static char buff[512];
static LCD_PARAM_T dyn_params;


//VGA@60Hz on HDMI/DVI display
static const LCD_PARAM_T vga_60hz =
{
    48,       /* Horizontal back porch = 1.92 us */
    16,       /* Horizontal front porch = 640 ns */
    96,       /* HSYNC pulse width = 3.84us */
    640,      /* Pixels per line */

    28,//29, //31,       /* Vertical back porch */
    11,//10, //11,       /* Vertical front porch */
    2,        /* VSYNC pulse width */
    480,      /* Lines per panel */

    0,        /* Do not invert output enable */
    1,        /* Invert panel clock */
    1,        /* Invert HSYNC */
    1,        /* Invert VSYNC */
    1,        /* AC bias frequency (not used) */
    16,       /* Bits per pixel */
    0,        /* Optimal clock rate (Hz). Use external clock in (JP1 must be 7-8 to enable the 25.175MHz external oscillator) */
    TFT,      /* LCD panel type */
    0,        /* Single panel display */
};

//SVGA@56Hz on HDMI/DVI display
static const LCD_PARAM_T svga_800x600_36MHz_56Hz =
{
    64,//128,       /* Horizontal back porch */
    40,//24,       /* Horizontal front porch */
    168,//72,      /* HSYNC pulse width */
    800,      /* Pixels per line */

    23,//22,       /* Vertical back porch */
    1,        /* Vertical front porch */
    4,//2,        /* VSYNC pulse width */
    600,      /* Lines per panel */

    0,        /* Do not invert output enable */
    1,        /* Invert panel clock */
    0,//1,        /* Invert HSYNC */
    0,//1,        /* Invert VSYNC */
    1,        /* AC bias frequency (not used) */
    16,       /* Bits per pixel */
    0,        /* Optimal clock rate (Hz). Use external clock in (JP1 must be 3-4 to enable the 36MHz external oscillator) */
    TFT,      /* LCD panel type */
    0,        /* Single panel display */
};

//720x480p@60Hz on HDMI/DVI display
static const LCD_PARAM_T _720x480_27_027MHz_60Hz =
{
    60,       /* Horizontal back porch */
    16,       /* Horizontal front porch */
    62,      /* HSYNC pulse width */
    720,      /* Pixels per line */

    31,//30,       /* Vertical back porch */
    8,//9,        /* Vertical front porch */
    6,        /* VSYNC pulse width */
    480,      /* Lines per panel */

    0,        /* Do not invert output enable */
    1,        /* Invert panel clock */
    1,        /* Invert HSYNC */
    1,        /* Invert VSYNC */
    1,        /* AC bias frequency (not used) */
    16,       /* Bits per pixel */
    0,        /* Optimal clock rate (Hz). Use external clock in (JP1 must be 5-6 to enable the 27MHz external oscillator) */
    TFT,      /* LCD panel type */
    0,        /* Single panel display */
};

//720x576p@50Hz on HDMI/DVI display
static const LCD_PARAM_T _720x576_27MHz_50Hz =
{
/*144*/
	66,       /* Horizontal back porch */
    16,       /* Horizontal front porch */
    62,      /* HSYNC pulse width */
    720,      /* Pixels per line */

/*49*/
    33,//34,       /* Vertical back porch */
    10,//9,        /* Vertical front porch */
    6,        /* VSYNC pulse width */
    576,      /* Lines per panel */

    0,        /* Do not invert output enable */
    1,        /* Invert panel clock */
    1,        /* Invert HSYNC */
    1,        /* Invert VSYNC */
    1,        /* AC bias frequency (not used) */
    16,       /* Bits per pixel */
    0,        /* Optimal clock rate (Hz). Use external clock in (JP1 must be 5-6 to enable the 27MHz external oscillator) */
    TFT,      /* LCD panel type */
    0,        /* Single panel display */
};

static int current_resolution = 0;

static const setting_t resolutions[] =
{
  { NULL,                     OSC_25_175MHZ, RES_VGA,   "Read from EEPROM"       },
  { &vga_60hz,                OSC_25_175MHZ, RES_VGA,   "VGA (640x480) at 60Hz"  },
  { &svga_800x600_36MHz_56Hz, OSC_36MHZ,     RES_SVGA,  "SVGA (800x600) at 56Hz" },
  { &_720x480_27_027MHz_60Hz, OSC_27MHZ,     RES_480P60,"480p (720x480) at 60Hz" },
  { &_720x576_27MHz_50Hz,     OSC_27MHZ,     RES_576P50,"576p (720x576) at 50Hz" },
};

/******************************************************************************
 * Local Functions
 *****************************************************************************/

static void PrintGreeting(void)
{
  console_sendString((uint8_t*)"\r\n***************************************************\r\n");
  console_sendString((uint8_t*)"*                                                 *\r\n");
  console_sendString((uint8_t*)"* Display Expansion Board program for LPC1788...  *\r\n");
  console_sendString((uint8_t*)"* (C) Embedded Artists 2001-2012                  *\r\n");
  console_sendString((uint8_t*)"*                                                 *\r\n");
  console_sendString((uint8_t*)"*                           " __DATE__ ", " __TIME__ " *\r\n");
  console_sendString((uint8_t*)"***************************************************\r\n");
}

static void selectOscillator(osc_e oscillator)
{
  if (ea_lcdb_ctrl_selectOscillator(oscillator) != LCDB_RESULT_OK) {
    sprintf(buff, "Invalid oscillator (%d)\r\n", oscillator);
    console_sendString((uint8_t*)buff);
  }
}

static lcdb_result_t changeResolution(SWIM_WINDOW_T* pWin, int32_t* pDev)
{
  int32_t framebuf = (int32_t)(SDRAM_BASE+0x10000);
  lcdb_result_t result;
  int32_t width = 0;
  int32_t height = 0;
  LCD_PARAM_T* lp;
  
  current_resolution++;
  if (current_resolution == NUM_RESOLUTIONS)
  {
    current_resolution = 0;
  }

  sprintf(buff, "Switching to %s\r\n", resolutions[current_resolution].description);
  console_sendString((uint8_t*)buff);
  
  ea_lcdb_close(*pDev);
  
  lp = (LCD_PARAM_T*)resolutions[current_resolution].lcd_params;
  
  selectOscillator(resolutions[current_resolution].oscillator);
  
  tda19988_setRes(resolutions[current_resolution].tda_res);
  
  if ((result = ea_lcdb_open(lp, NULL, pDev)) != LCDB_RESULT_OK) {
    sprintf(buff, "Failed to switch to %s, ea_lcdb_open FAILED (%d)\r\n", resolutions[current_resolution].description, result);
    console_sendString((uint8_t*)buff);
    return result;  
  }

  if (lp == NULL)
  {
    lp = &dyn_params;
    if ((result = ea_lcdb_getLcdParams(lp)) != LCDB_RESULT_OK) {
      sprintf(buff, "Failed to switch to %s, ea_lcdb_getLcdParams FAILED (%d)\r\n", resolutions[current_resolution].description, result);
      console_sendString((uint8_t*)buff);
      return result;
    }
  }
  
  width  = lp->pixels_per_line;
  height = lp->lines_per_panel;

  memset((void*)(framebuf), 0x00, width*height*2);


  if ((result = ea_lcdb_enable(*pDev, framebuf)) != LCDB_RESULT_OK) {
    sprintf(buff, "Failed to switch to %s, ea_lcdb_enable FAILED (%d)\r\n", resolutions[current_resolution].description, result);
    console_sendString((uint8_t*)buff);
    return result;  
  }
  
  swim_window_close(pWin);
  
  /* Create a SWIM window */
  swim_window_open(pWin, width, height, (COLOR_T*)framebuf, 0, 0, 
    width-1, height-1, 1, WHITE, BLACK, BLACK);
  
  return LCDB_RESULT_OK;
}

/**
 * Draw introduction text to the display
 */
static void drawIntro(SWIM_WINDOW_T* win1)
{

  swim_clear_screen(win1, BLACK);
  swim_set_pen_color(win1, WHITE);


  swim_put_text_xy(win1, "Display: ", 10, 10);  

  if (ea_lcdb_getDisplayName(main_buf, MAIN_BUF_SZ) != LCDB_RESULT_OK) {
    swim_put_text_xy(win1, "Unknown",     70, 10);        
  }
  else {
    swim_put_text_xy(win1, (const CHAR*)main_buf,     70, 10);  
  }

  swim_put_text_xy(win1, "Mfg: ", 10, 20);  

  if (ea_lcdb_getDisplayMfg(main_buf, MAIN_BUF_SZ) != LCDB_RESULT_OK) {
    swim_put_text_xy(win1, "Unknown",     70, 20);        
  }
  else {
    swim_put_text_xy(win1, (const CHAR*)main_buf, 70, 20);  
  }

  swim_put_text_xy(win1, "Resolution:", 10, 30);  
  if (resolutions[current_resolution].lcd_params == NULL)
  {
    sprintf(buff, "%dx%d, %s", win1->xpsize, win1->ypsize, resolutions[current_resolution].description);
    swim_put_text_xy(win1, buff, 70, 30);  
  }
  else
  {
    swim_put_text_xy(win1, resolutions[current_resolution].description, 70, 30);  
  }


  swim_put_text_xy(win1, "1. Press SW2 to for this info screen ", 10, 70);  
  swim_put_text_xy(win1, "2. Press SW3 to draw color bars ", 10, 85);  
  swim_put_text_xy(win1, "3. Press SW4 to cycle through resolutions ", 10, 100);  
  //swim_put_text_xy(win1, "4. Press SW5 to test touch ", 10, 115);  

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
  
  swim_set_pen_color(win1, 0x8000);
  swim_put_line(win1, 10, 10, width-10, height/3 - 10);
  swim_set_pen_color(win1, 0x000f);
  swim_put_line(win1, 10, height*2/3 + 10, width-10, height-10);
}




/******************************************************************************
 * Main method
 *****************************************************************************/

int main (void)
{
  SWIM_WINDOW_T win1;
  
  int32_t dev_lcd = 0;

  uint16_t state = 0;

  int32_t framebuf = (int32_t)(SDRAM_BASE+0x10000);
  int32_t width = 0;
  int32_t height = 0;
  lcdb_result_t result;
  LCD_PARAM_T lp;
  
	TIM_TIMERCFG_Type timerCfg;

  // initialize timer
  TIM_ConfigStructInit(TIM_TIMER_MODE, &timerCfg);
  TIM_Init(LPC_TIM0, TIM_TIMER_MODE, &timerCfg);

  console_init();
  
  PrintGreeting();

  i2c0_pinConfig();

  /* Initialize I2C */
  I2C_Init(I2C_PORT, 100000);
  I2C_Cmd(I2C_PORT, ENABLE);

  sdram_init();
  
  TIM_Waitms(50);
  
  do {
    if ((result = ea_lcdb_open(NULL, NULL, &dev_lcd)) != LCDB_RESULT_OK) {
      sprintf(buff, "ea_lcdb_open FAILED (%d)\r\n", result);
      console_sendString((uint8_t*)buff);
      break;  
    }
  
    if ((result = ea_lcdb_getLcdParams(&lp)) != LCDB_RESULT_OK) {
      sprintf(buff, "ea_lcdb_getLcdParams FAILED (%d)\r\n", result);
      console_sendString((uint8_t*)buff);
      break;  
    }

    if (tda19988_init() != SUCCESS)
    {
      sprintf(buff, "tda19988_init FAILED, Output on connectors J10 and J11 disabled.\r\n");
      console_sendString((uint8_t*)buff);
      //break;
    }
    if (tfp410_init() != SUCCESS)
    {
      sprintf(buff, "tfp410_init FAILED, Output on connectors J7 and J8 disabled.\r\n");
      console_sendString((uint8_t*)buff);
      //break;
    }
    
    width  = lp.pixels_per_line;
    height = lp.lines_per_panel;

    memset((void*)(framebuf), 0x00, width*height*2);

    if ((result = ea_lcdb_enable(dev_lcd, framebuf)) != LCDB_RESULT_OK) {
      sprintf(buff, "ea_lcdb_enable FAILED (%d)\r\n", result);
      console_sendString((uint8_t*)buff);
      break;        
    }
      
    /* Create a SWIM window */
    swim_window_open(&win1, width, height, (COLOR_T*)framebuf, 0, 0, 
      width-1, height-1, 1, WHITE, BLACK, BLACK);

    console_sendString((uint8_t*)"Initialization complete\r\n");
    drawIntro(&win1);

    while (1) {
      state = pca9532_getLedState(FALSE);
  
      if ((state & KEY1)) {
        drawIntro(&win1);
      }
  
      else if ((state & KEY2)) {
        drawBars(&win1, win1.xpsize, win1.ypsize);
      }

      else if ((state & KEY3)) {
        while (changeResolution(&win1, &dev_lcd) != LCDB_RESULT_OK)
        {
          sprintf(buff, "Failed to change resolution, testing next\r\n");
          console_sendString((uint8_t*)buff);
        }
        drawIntro(&win1);
        TIM_Waitms(300);
      }
    }

  } while(0);

  while(1) {
    ;
  }
}


