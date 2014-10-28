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
#include "lpc177x_8x_timer.h"
#include "lpc177x_8x_ssp.h"

#include "lcd_driver.h"
#include "lcd_params.h"
#include "truly_display.h"

/******************************************************************************
 * Defines and typedefs
 *****************************************************************************/

#define CS_ON    (LPC_GPIO5->CLR |= (1<<3)) // p5.3 -> low    /* LPC4088 */
#define CS_OFF   (LPC_GPIO5->SET |= (1<<3)) // p5.3 -> high   /* LPC4088 */
#define DC_CMD   (LPC_GPIO0->CLR |= (1<<19)) // p0.19 -> low
#define DC_DATA  (LPC_GPIO0->SET |= (1<<19)) // p0.19 -> high

#define SSP_PORT  (LPC_SSP2)   /* LPC4088 */
#define SSP_CLOCK 3000000

#define BACKLIGHT_PIN  (1 << 18)   // P1.18

/******************************************************************************
 * External global variables
 *****************************************************************************/

/******************************************************************************
 * Local variables
 *****************************************************************************/

static const LCD_PARAM_T truly_g240320ltsw =
{
   28,       /* Horizontal back porch */
   10,       /* Horizontal front porch */
    2,       /* HSYNC pulse width */
  240,       /* Pixels per line */
    2,       /* Vertical back porch */
    1,       /* Vertical front porch */
    2,       /* VSYNC pulse width */
  320,       /* Lines per panel */
    0,        /* Invert output enable */
    1,        /* Do not invert panel clock */
    1,        /* Invert HSYNC */
    1,        /* Invert VSYNC */
    1,        /* AC bias frequency (not used) */
   16,        /* Bits per pixel */
  8200000,    /* Optimal clock rate (Hz) between 1-8.22 MHz according to SSD1289 datasheet */
  TFT,        /* LCD panel type */
    0,        /* Single panel display */
};



/******************************************************************************
 * Local Functions
 *****************************************************************************/

static void pinConfig(void)
{
  LPC_IOCON->P0_4 |= 7; /* LCD_VD_0 @ P0.4 */
  LPC_IOCON->P0_5 |= 7; /* LCD_VD_1 @ P0.5 */
  LPC_IOCON->P0_6 |= 7; /* LCD_VD_8 @ P0.6 */
  LPC_IOCON->P0_7 |= 7; /* LCD_VD_9 @ P0.7 */
  LPC_IOCON->P0_8 |= 7; /* LCD_VD_16 @ P0.8 */
  LPC_IOCON->P0_9 |= 7; /* LCD_VD_17 @ P0.9 */
  LPC_IOCON->P0_10 |= 7; /* LCD_VD_5 @ P0.10 */  /* LPC4088 */

  LPC_IOCON->P1_20 |= 7; /* LCD_VD_10 @ P1.20 */
  LPC_IOCON->P1_21 |= 7; /* LCD_VD_11 @ P1.21 */
  LPC_IOCON->P1_22 |= 7; /* LCD_VD_12 @ P1.22 */
  LPC_IOCON->P1_23 |= 7; /* LCD_VD_13 @ P1.23 */
  LPC_IOCON->P1_24 |= 7; /* LCD_VD_14 @ P1.24 */
  LPC_IOCON->P1_25 |= 7; /* LCD_VD_15 @ P1.25 */
  LPC_IOCON->P1_26 |= 7; /* LCD_VD_20 @ P1.26 */
  LPC_IOCON->P1_27 |= 7; /* LCD_VD_21 @ P1.27 */
  LPC_IOCON->P1_28 |= 7; /* LCD_VD_22 @ P1.28 */
  LPC_IOCON->P1_29 |= 7; /* LCD_VD_23 @ P1.29 */

  LPC_IOCON->P2_0 |= 7; /* LCD_PWR @ P2.0 */
  LPC_IOCON->P2_1 |= 7; /* LCD_LE  @ P2.1 */
  LPC_IOCON->P2_2 |= 7; /* LCD_DCLK @ P2.2 */
  LPC_IOCON->P2_3 |= 7; /* LCD_FP @ P2.3 */
  LPC_IOCON->P2_4 |= 7; /* LCD_ENAB_M @ P2.4 */
  LPC_IOCON->P2_5 |= 7; /* LCD_LP @ P2.5 */
  LPC_IOCON->P2_6 |= 7; /* LCD_VD_4 @ P2.6 */
  //LPC_IOCON->P2_7 |= 7; /* LCD_VD_5 @ P2.7 */  /* LPC4088 */
  LPC_IOCON->P2_8 |= 7; /* LCD_VD_6 @ P2.8 */
  LPC_IOCON->P2_9 |= 7; /* LCD_VD_7 @ P2.9 */
  
  LPC_IOCON->P2_11 |= 7; /* LCD_CLKIN @ P2.11 */
  LPC_IOCON->P2_12 |= 7; /* LCD_VD_18 @ P2.12 */
  LPC_IOCON->P2_13 |= 7; /* LCD_VD_19 @ P2.13 */

  LPC_IOCON->P4_28 |= 7; /* LCD_VD_2 @ P4.28 */
  LPC_IOCON->P4_29 |= 7; /* LCD_VD_3 @ P4.29 */

  /* p1.18 (backlight) pin as output p2.0 */
  LPC_GPIO1->DIR |= BACKLIGHT_PIN; 
  LPC_GPIO1->CLR  = BACKLIGHT_PIN; 

  /* p5.3 pin as output (CS) */     /* LPC4088 */
  LPC_GPIO5->DIR |= (1<<3);  	    /* LPC4088 */

  /* p0.19 pin as output (DC) */
  LPC_GPIO0->DIR |= (1<<19);

  // PIN config SSP 2
  PINSEL_ConfigPin (5, 2, 2);   /* LPC4088 */
  PINSEL_ConfigPin (5, 3, 0);   /* LPC4088 */
  PINSEL_ConfigPin (5, 1, 2);   /* LPC4088 */
  PINSEL_ConfigPin (5, 0, 2);   /* LPC4088 */

  // set P1.18 as PWM output (PWM1.1, second alternative function)
  PINSEL_ConfigPin(1, 18, 2);   /* LPC4088 */
  LPC_PWM1->PR  = 0x00;     //no prescaling
  LPC_PWM1->MCR = 0x02;     //reset counter if MR0 match
  LPC_PWM1->MR0 = 0x3000;   //period time equal about 5 ms
  LPC_PWM1->MR1 = 0x0000;
  LPC_PWM1->LER = 0x03;     //latch MR0 and MR1
  LPC_PWM1->PCR = 0x0200;   //enable PWMENA1
  LPC_PWM1->TCR = 0x09;     //enable counter and PWM
  truly_lcd_setBacklight(0);
}

static void
writeToReg(uint16_t addr, uint16_t data)
{
  uint8_t buf[2];
  SSP_DATA_SETUP_Type sspCfg;
  DC_CMD;
  CS_ON;

  buf[0] = 0;
  buf[1] = (addr & 0xff);

  sspCfg.tx_data = buf;
  sspCfg.rx_data = NULL;
  sspCfg.length  = 2; 

  SSP_ReadWrite (SSP_PORT, &sspCfg, SSP_TRANSFER_POLLING);

  CS_OFF;  

  DC_DATA;
  CS_ON;
  buf[0] = (data >> 8);
  buf[1] = (data & 0xff);
  SSP_ReadWrite (SSP_PORT, &sspCfg, SSP_TRANSFER_POLLING);
  CS_OFF; 

  DC_CMD;
  CS_ON;

  buf[0] = (0);
  buf[1] = (0x22);
  SSP_ReadWrite (SSP_PORT, &sspCfg, SSP_TRANSFER_POLLING);
  CS_OFF; 
}

static void ssd1289_init(void)
{
  writeToReg (0x00,0x0001);
  TIM_Waitms(15);
  writeToReg (0x03,0xAEAC);
  writeToReg (0x0C,0x0007);
  writeToReg (0x0D,0x000F);
  writeToReg (0x0E,0x2900);
  writeToReg (0x1E,0x00B3);
  TIM_Waitms(15);
  writeToReg (0x01,0x2B3F);
  writeToReg (0x02,0x0600);
  writeToReg (0x10,0x0000);
  writeToReg (0x11,0x65b0);//60B0  rgb i/r
  TIM_Waitms(20);
  writeToReg (0x05,0x0000);
  writeToReg (0x06,0x0000);
  writeToReg (0x16,0xEF1C);
  writeToReg (0x17,0x0003);
  writeToReg (0x07,0x0233);
  writeToReg (0x0B,0x5312);
  writeToReg (0x0F,0x0000);
  writeToReg (0x1E,0x00AE);
  TIM_Waitms(20);
  writeToReg (0x41,0x0000);
  writeToReg (0x42,0x0000);
  writeToReg (0x48,0x0000);
  writeToReg (0x49,0x013F);
  writeToReg (0x44,0xEF00);
  writeToReg (0x45,0x0000);
  writeToReg (0x46,0x013F);
  writeToReg (0x4A,0x0000);
  writeToReg (0x4B,0x0000);
  TIM_Waitms(20);
  writeToReg (0x30,0x0707);
  writeToReg (0x31,0x0704);
  writeToReg (0x32,0x0204);
  writeToReg (0x33,0x0201);
  writeToReg (0x34,0x0203);
  writeToReg (0x35,0x0204);
  writeToReg (0x36,0x0204);
  writeToReg (0x37,0x0502);
  writeToReg (0x3A,0x0302);
  writeToReg (0x3B,0x0500);
  TIM_Waitms(20);
  writeToReg (0x22,0x0000);
}

/******************************************************************************
 * Public Functions
 *****************************************************************************/

/******************************************************************************
 *
 * Description:
 *    Initialize the display
 *
 *****************************************************************************/
int32_t truly_lcd_open (int32_t framebuf)
{
  int32_t dev = 0;
  SSP_CFG_Type SSP_ConfigStruct;

  pinConfig();

	// initialize SSP configuration structure to default
	SSP_ConfigStructInit(&SSP_ConfigStruct);

  // set clock rate
  SSP_ConfigStruct.ClockRate = SSP_CLOCK;

	// Initialize SSP peripheral with parameter given in structure above
	SSP_Init(SSP_PORT, &SSP_ConfigStruct);

	// Enable SSP peripheral
	SSP_Cmd(SSP_PORT, ENABLE);

  /* turn on backlight -> active low */
  //LPC_GPIO1->SET |= BACKLIGHT_PIN;
  truly_lcd_setBacklight(100);

  dev = lcd_open((int32_t)&truly_g240320ltsw);

  lcd_ioctl(dev, LCD_PWR_ON, 1);
  lcd_ioctl(dev, LCD_SET_BUFFER, framebuf);

  TIM_Waitms(200);

  /* initialize LCD controller */
  ssd1289_init();

  SSP_Cmd(SSP_PORT, DISABLE);

  SSP_DeInit(SSP_PORT);


  return dev;
}

int32_t truly_lcd_close (int32_t devid)
{
  /* turn off backlight  */
  //LPC_GPIO1->CLR |= BACKLIGHT_PIN;
  truly_lcd_setBacklight(0);

  return lcd_close(devid);
}

void truly_lcd_setBacklight (uint8_t percent)
{
//  LPC_PWM1->MR1 = ((unsigned long)0x3000 * (unsigned long)(100-percent)) / (unsigned long)100;
  LPC_PWM1->MR1 = ((unsigned long)0x3000 * (unsigned long)(percent)) / (unsigned long)100;
  LPC_PWM1->LER = 0x02;
}
