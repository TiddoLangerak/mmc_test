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


#include <stdlib.h> 
#include <string.h>
#include "lpc_types.h"
#include "lpc177x_8x_timer.h"

#include "tsc2046_touch.h"
#include "calibrate.h"

#include "lpc_swim_font.h"
#include "lpc_swim.h"



/******************************************************************************
 * Defines, macros, and typedefs
 *****************************************************************************/

#define BACK_COLOR WHITE
#define FORE_COLOR BLACK

/******************************************************************************
 * Public variables
 *****************************************************************************/


/******************************************************************************
 * Local variables
 *****************************************************************************/


static int calibPoint = 0;
static tTouchPoint refPoint1 = { 15,  242};
static tTouchPoint refPoint2 = {240, 80};
static tTouchPoint refPoint3 = {460, 222};
static tTouchPoint scr1 = {0, 0};
static tTouchPoint scr2 = {0, 0};
static tTouchPoint scr3 = {0, 0};
static BOOL_32 releaseNeeded = FALSE;
static BOOL_32 touchReleased = FALSE;


/******************************************************************************
 * Local functions
 *****************************************************************************/

static void
paintMessage(SWIM_WINDOW_T *win, char* pMsg)
{
  uint16_t msgLen = strlen((char *)pMsg);
  uint16_t x = 0;
  uint16_t y = 20;
  
  swim_set_pen_color(win, BLACK);
  swim_put_box(win, 0, 0, 480, 40);
  swim_set_pen_color(win, WHITE);
  
  // each character is 6 pixels wide
  x = (240 / 2) - (msgLen*6/2);
      
  swim_put_text_xy(win, (const CHAR*)pMsg, x, y);
}


static void
drawCalibPoint(SWIM_WINDOW_T *win, UNS_16 x, UNS_16 y)
{  
  swim_clear_screen(win, BLACK);
  paintMessage(win, "Calibrating, touch marked box...");

  swim_put_box(win, x-5, y-5, x+5, y+5);

}

/******************************************************************************
 * Implementation of public functions
 *****************************************************************************/

void
calibrateStart(SWIM_WINDOW_T *win) 
{
  INT_32 x, y, z = 0;

  BOOL_32 calibrated = FALSE;
  releaseNeeded = FALSE;
  calibPoint = 0;
  
  drawCalibPoint(win, refPoint1.x, refPoint1.y);

  touch_xyz(&x, &y, &z);
    
  while (!calibrated) {

    TIM_Waitms(100);

    touch_xyz(&x, &y, &z);


    if (z == 0)
    {
      touchReleased = TRUE;
      continue;
    } 
    
    if (releaseNeeded && !touchReleased)
      continue;
    
    touchReleased = FALSE;
    
    
    switch (calibPoint++)
    {
    case 0:
      scr1.x = x;
      scr1.y = y;
      drawCalibPoint(win, refPoint2.x, refPoint2.y);

      releaseNeeded = TRUE;
      break;
    case 1:
      scr2.x = x;
      scr2.y = y;
      drawCalibPoint(win, refPoint3.x, refPoint3.y);

      releaseNeeded = TRUE;
      break;
    case 2:
      scr3.x = x;
      scr3.y = y;
      paintMessage(win, "Calibrated!");

      releaseNeeded = TRUE;
      touch_calibrate(refPoint1, refPoint2, refPoint3, scr1, scr2, scr3);

      calibrated = TRUE;
      break;
      
    }
  }

}

