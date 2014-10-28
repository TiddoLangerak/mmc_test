/*****************************************************************************
 *
 *   Copyright(C) 2012, Embedded Artists AB
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
#include "lpc177x_8x_dac.h"
#include "lpc177x_8x_pinsel.h"
#include "lpc177x_8x_timer.h"
#include "lpc177x_8x_i2c.h"
#include "lpc177x_8x_uart.h"
#include "lpc177x_8x_gpdma.h"

#include "acc.h"
#include "board.h"
#include "memreg.h"
#include "pca9532.h"
#include "ea_lcd_board.h"
#include "sdram.h"
#include "lpc_swim.h"
#include "lpc_swim_font.h"
#include "lpc_swim_image.h"
#include "truly_display.h"
#include "lodepng.h"
#include "cfgmem.h"
#include "lpc_heap.h"
#include "speaker.h"
#include "joystick.h"

/******************************************************************************
 * Typedefs and defines
 *****************************************************************************/

#define I2C_PORT (I2C_0)
#define MAIN_BUF_SZ 100

/******************************************************************************
 * Global variables
 *****************************************************************************/

extern const unsigned char demo_image[];
extern int demo_image_sz;

#define IMAGE_TO_SHOW  &demo_image[0]
#define IMAGE_SIZE     demo_image_sz

/******************************************************************************
 * Local variables
 *****************************************************************************/

static char main_buf[MAIN_BUF_SZ];
static uint16_t running_mode = 0;

static uint16_t sequence_0[] = {
    LED1, LED5, LED2, LED6, LED3, LED7, LED4,
    LED8, LED4, LED7, LED3, LED6, LED2, LED5,
};
static uint16_t sequence_1[] = {
    LED1|LED8, LED1|LED8, LED5|LED4, LED5|LED4,
    LED2|LED7, LED6|LED3, LED2|LED7, LED6|LED3,
    LED2|LED7, LED5|LED4, LED2|LED7, LED5|LED4,
};
static uint16_t sequence_2[] = {
    LED1|LED8, LED5|LED4, LED2|LED8, LED6|LED4, LED2|LED8, LED5|LED4,
    LED1|LED8, LED5|LED4, LED2|LED8, LED6|LED4, LED2|LED8, LED5|LED4,
};
static uint16_t sequence_3[] = {
    LED1|LED5|LED2|LED6, LED1|LED5|LED2|LED6,
    LED1|LED5|LED2|LED6, LED1|LED5|LED2|LED6,
    LED3|LED7|LED4|LED8, LED3|LED7|LED4|LED8,
    LED3|LED7|LED4|LED8, LED3|LED7|LED4|LED8,
};
static const uint16_t* const sequences[4] = {
    &sequence_0[0],
    &sequence_1[0],
    &sequence_2[0],
    &sequence_3[0],
};
static uint16_t sequence_lengths[4] = {14, 12, 12, 8};

static int systick_counter = 0;

/******************************************************************************
 * Local Functions
 *****************************************************************************/

static void greeting(void)
{
  console_sendString((uint8_t*)"\r\n***************************************************\r\n");
  console_sendString((uint8_t*)"*                                                 *\r\n");
  console_sendString((uint8_t*)"* Demo Application for the LPC4088 OEM Board...   *\r\n");
  console_sendString((uint8_t*)"* (C) Embedded Artists AB 2001-2012               *\r\n");
  console_sendString((uint8_t*)"*                                                 *\r\n");
  console_sendString((uint8_t*)"*                                                 *\r\n");
  console_sendString((uint8_t*)"*     Thank you for buying Embedded Artists'      *\r\n");
  console_sendString((uint8_t*)"*     LPC4088 Developer's Kit                     *\r\n");
  console_sendString((uint8_t*)"*                                                 *\r\n");
  console_sendString((uint8_t*)"*     ... and congratulations on your choice      *\r\n");
  console_sendString((uint8_t*)"*     of microcontroller!                         *\r\n");
  console_sendString((uint8_t*)"*                                                 *\r\n");
  console_sendString((uint8_t*)"***************************************************\r\n");
  console_sendString((uint8_t*)"Version: 1.2, (build " __DATE__ ")\r\n");

/*
 * Version History:
 *
 * 1.0  First version. No delay after SDRAM init.
 * 1.1  Added 100ms delay after SDRAM init.
 * 1.2  Fixed SDRAM bug so the delay added in 1.1 has been removed.
 */
}

static void manipulateRunningLights(void)
{
  uint16_t state = 0;
  uint16_t new_mode = running_mode;
  static int pos = 0;

  state = (pca9532_getLedState(FALSE));

  if (state & KEY1) {
    new_mode = 0;
  } else if (state & KEY2) {
    new_mode = 1;
  } else if (state & KEY3) {
    new_mode = 2;
  } else if (state & KEY4) {
    new_mode = 3;
  } else {
    state = joystick_read();
    if (state & JOYSTICK_LEFT) {
      new_mode = (new_mode + 3) % 4; // select previous mode
    } else if (state & JOYSTICK_RIGHT) {
      new_mode = (new_mode + 1) % 4; // select next mode
    }
  }

  if (new_mode != running_mode) {
    running_mode = new_mode;
    pos = 0;
  }

  /* Update running light */
  pca9532_setLeds(sequences[running_mode][pos], (~sequences[running_mode][pos]&LED_MASK));

  pos++;
  if (pos >= sequence_lengths[running_mode]) {
    pos = 0;
  }
}

static void runLights(void)
{
  static Bool firstTime = TRUE;
  static int pos = 0;
  static uint16_t* memreg = (uint16_t*)MEMREG_BASE;
  static int32_t yoff;

  int8_t x;
  int8_t y;
  int8_t z;

  /* should be called every 100ms */
  if (firstTime) {
    memreg_init();
    acc_init();
    joystick_init();

    acc_read(&x, &y, &z);
    yoff = 0-y;
    firstTime = FALSE;
  }

  /* Update memory mapped LEDs based on accelerator position */
  if (pos&1) {
    acc_read(&x, &y, &z);
    y = y+yoff;
    if (y < 0) {
      *memreg = (1<<((y+75)/10)) << 0;
    } else {
      *memreg = (1<<(y/10)) << 8;
    }
  }

  /* Update running light */
  pos++;
  manipulateRunningLights();
}

void SysTick_Handler(void)
{
  systick_counter++;
}


/*
 * The decoded image produced by LodePNG is in 24 bit format but the display wants it in 16 bit
 * format (565). This function converts the image to 565 format.
 */
static void decodePNG(unsigned char** ppOut, unsigned* pW, unsigned* pH, const unsigned char* pIn, size_t insize)
{
  unsigned char* pTmp;
  unsigned error = LodePNG_decode24(&pTmp, pW, pH, pIn, insize);

  if (error == 0)
  {
    int x, y;
    uint16_t* pConverted;
    uint8_t r;
    uint8_t g;
    uint8_t b;
    int height = *pH;
    int width = *pW;
    int off = 0;

    *ppOut = lpc_new(width * height * 2);
    pConverted = (uint16_t*)*ppOut;

    for (y = 0; y < height; y++) {
      for (x = 0; x < width; x++) {
        r = pTmp[off    ];
        g = pTmp[off + 1];
        b = pTmp[off + 2];
        *pConverted = (((unsigned short)r & 0xF8) << 8) |
                       (((unsigned short)g & 0xFC) << 3) |
                       (((unsigned short)b & 0xF8) >> 3);
        pConverted++;
        off += 3;
      }
    }
    lpc_free(pTmp);
  }
  else
  {
    sprintf(main_buf, "Failed to decode PNG. Error %d\r\n", error);
    console_sendString((uint8_t*)main_buf);
  }
}

/**
 * Draw introduction text to the display
 */
static void drawIntro(SWIM_WINDOW_T* win1, int displayWidth, int displayHeight)
{
  SWIM_WINDOW_T win2;
  unsigned char* pOut = NULL;
  unsigned w, h;

  swim_clear_screen(win1, WHITE);
  swim_set_pen_color(win1, WHITE);

  decodePNG(&pOut, &w, &h, IMAGE_TO_SHOW, IMAGE_SIZE);

  if (displayWidth >= w && displayHeight >= h)
  {
    // Image is small enough to fit inside the physical boundaries of the
    // screen. Make sure it is centered
    swim_window_open(&win2, displayWidth, displayHeight, (COLOR_T*)(int32_t)(MEM_FB_BASE),
        (displayWidth-w)/2, (displayHeight-h)/2,
        (displayWidth-w)/2 + w - 1, (displayHeight-h)/2 + h - 1,
        0, WHITE, BLACK, BLACK);

    swim_put_image(&win2,(COLOR_T *)pOut,w,h);
  }
  else
  {
    // Image is small enough to fit inside the physical boundaries of the
    // screen but not in the correct orientation. Rotate it and make sure it is centered
    swim_window_open(&win2, displayWidth, displayHeight, (COLOR_T*)(int32_t)(MEM_FB_BASE),
        (displayWidth-h)/2, (displayHeight-w)/2,
        (displayWidth-h)/2 + h - 1, (displayHeight-w)/2 + w - 1,
        0, WHITE, BLACK, BLACK);

    swim_put_right_image(&win2,(COLOR_T *)pOut,w,h);
  }

  lpc_free(pOut);
}

/******************************************************************************
 * Main method
 *****************************************************************************/

int main (void)
{
  TIM_TIMERCFG_Type timerCfg;
  SWIM_WINDOW_T win1;
  int32_t dev_lcd = 0;
  int32_t framebuf = (int32_t)(MEM_FB_BASE);
  int32_t width = 0;
  int32_t height = 0;
  lcdb_result_t result;
  LCD_PARAM_T lp;
  int delay = 20;

  /* Initialize devices */

  /* initialize timer */
  TIM_ConfigStructInit(TIM_TIMER_MODE, &timerCfg);
  TIM_Init(LPC_TIM0, TIM_TIMER_MODE, &timerCfg);

  console_init();
  greeting();
  i2c0_pinConfig();

  /* Initialize I2C */
  I2C_Init(I2C_PORT, 100000);
  I2C_Cmd(I2C_PORT, ENABLE);

  /* Generate system tick at 10Hz */
  SysTick_Config(SystemCoreClock/100);

  sdram_init();
  lpc_heap_init((void *)MEM_HEAP_BASE, MEM_HEAP_SIZE);

  do {
    if ((result = ea_lcdb_open(NULL, NULL, &dev_lcd)) == LCDB_RESULT_OK) {

      if ((result = ea_lcdb_getLcdParams(&lp)) != LCDB_RESULT_OK) {
        sprintf(main_buf, "ea_lcdb_getLcdParams FAILED (%d)\r\n", result);
        console_sendString((uint8_t*)main_buf);
        break;
      }

      width  = lp.pixels_per_line;
      height = lp.lines_per_panel;

      memset((void*)(framebuf), 0x00, width*height*2);

      if ((result = ea_lcdb_enable(dev_lcd, framebuf)) != LCDB_RESULT_OK) {
        sprintf(main_buf, "ea_lcdb_enable FAILED (%d)\r\n", result);
        console_sendString((uint8_t*)main_buf);
        break;
      }
    }
    else {
      /* Assuming that the unidentified display is a QVGA display */
      width  = 240;
      height = 320;
      memset((void*)(framebuf), 0x00, width*height*2);

      truly_lcd_open(framebuf);
    }

    /* Create a SWIM window */
    swim_window_open(&win1, width, height, (COLOR_T*)framebuf, 0, 0,
      width-1, height-1, 1, WHITE, BLACK, BLACK);

    drawIntro(&win1, width, height);

  } while(0);

  initSpeaker();
  playSound();

  while (1) {
    /* Wait forever */
    if (systick_counter > 10) {
      uint16_t state = joystick_read();
      if (state & JOYSTICK_UP) {
        delay = 10;
      } else if (state & JOYSTICK_CENTER) {
        delay = 20;
      } else if (state & JOYSTICK_DOWN) {
        delay = 40;
      }
    }
    if (systick_counter > delay) {
      runLights();
      systick_counter = 0;
    }
  }
}

