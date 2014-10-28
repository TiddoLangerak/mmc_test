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

#include "lpc_types.h"
#include "lpc177x_8x_dac.h"
#include "lpc177x_8x_timer.h"
#include "lpc177x_8x_gpdma.h"

#include "math.h"

/******************************************************************************
 * Typedefs and defines
 *****************************************************************************/

/* Speed factor */
#define PLAYBACK_SPEED 1

/* DMA wait forever */
#define WAIT_FOREVER  0xffff

/* DMA size of transfer */
#define LUT_SIZE 60

#define TO_NOTE(__val) (uint32_t)(PeripheralClock / (__val * LUT_SIZE))

// Notes Frequency from http://www.phy.mtu.edu/~suits/notefreqs.html
// The Original frequency value (decimal) is converted to the integer value
#define C3  131
#define E3  165
#define Gc3 208
#define A3  220
#define B3  247

#define C4  262
#define Cc4 277
#define D4  294
#define Dc4 311
#define E4  330
#define F4  349
#define Fc4 370
#define G4  392
#define Gc4 415
#define A4  440
#define Ac4 466
#define B4  494

#define C5  523
#define Cc5 554
#define D5  587
#define Dc5 622
#define E5  659
#define F5  698
#define Fc5 740
#define G5  783
#define Gc5 831
#define A5  880
#define Ac5 932
#define B5  988
#define C6  1047

/*
 * Multiplier to increase/decrease the speed that the melody
 * is played with
 */
#define TEMPO  1.5

/*
 * Adds three entries to the array that holds the melody:
 *
 *   frequency, duration, gap
 *
 * The duration is multiplied with TEMPO so that the speed
 * can be corrected without having to modify each note.
 *
 * The gap (silence period after the note) is a fifth of
 * the duration plus any extra gap time.
 */
#define NOTE_EXTRA_GAP(__freq,__duration, __extra_gap) \
              (__freq),(__duration)*TEMPO,((__duration)*TEMPO)/5 + (__extra_gap)
#define NOTE(__freq,__duration) \
              NOTE_EXTRA_GAP(__freq,__duration, 0)

/*
 * Tokens inserted into the melody to be able to do loops etc.
 * Don't use directly when composing the song.
 */
#define TOKEN_SET         0xfefe
#define TOKEN_GOTO        0xfafa
#define TOKEN_LABEL       0xfbfb
#define TOKEN_GOTO_STATE  0xfcfc
#define TOKEN_END         0xffff

/*
 * Place holders to use when composing the song to produce
 * loops.
 */
#define END_OF_SONG      TOKEN_END,        TOKEN_END,  TOKEN_END
#define SET_STATE(__val) TOKEN_SET,        (__val),    0
#define LABEL(__number)  TOKEN_LABEL,      (__number), 0
#define GOTO(__number)   TOKEN_GOTO,       (__number), 0
#define GOTO_STATE       TOKEN_GOTO_STATE, 0,          0

/******************************************************************************
 * Local variables
 *****************************************************************************/

static GPDMA_Channel_CFG_Type  GPDMACfg;
static GPDMA_LLI_Type          DMA_LLI_Struct;
static DAC_CONVERTER_CFG_Type  DAC_ConverterConfigStruct;

static uint32_t LUT_BUFFER[LUT_SIZE];

static const uint16_t song[] = {
    // A version of Fur Elise
    NOTE(E4, 125),
    NOTE(Dc4, 125),
    NOTE(E4, 125),
    NOTE(Dc4, 125),
    NOTE(E4, 125),
    NOTE(B3, 125),
    NOTE(D4, 125),
    NOTE(C4, 125),
    NOTE_EXTRA_GAP(A3, 125, 125),
    NOTE(C3, 125),
    NOTE(E3, 125),
    NOTE(A3, 125),
    NOTE_EXTRA_GAP(B3, 250, 125),
    NOTE(E3, 125),
    NOTE(Gc3, 125),
    NOTE(B3, 125),
    NOTE_EXTRA_GAP(C4, 250, 125),
    NOTE(E3, 125),
    NOTE(E3, 125),
    NOTE(Dc4, 125),
    NOTE(E4, 125),
    NOTE(Dc4, 125),
    NOTE(E4, 125),
    NOTE(B3, 125),
    NOTE(D4, 125),
    NOTE(C4, 125),
    NOTE_EXTRA_GAP(A3, 125, 125),
    NOTE(C3, 125),
    NOTE(E3, 125),
    NOTE(A3, 125),
    NOTE_EXTRA_GAP(B3, 250, 125),
    NOTE(E3, 125),
    NOTE(C4, 125),
    NOTE(B3, 125),
    NOTE(A3, 250),
#if 0
    SET_STATE(0),
    LABEL(5),
      NOTE(G4,300),
      NOTE(A4,100),
      NOTE(C5,500),
      NOTE(C5,450),
      NOTE(G5,1150),
      NOTE(A5,350),
      NOTE(A5,350),
      NOTE(A5,150),
      NOTE(G5,1150),
      NOTE(F5,450),
      NOTE(F5,300),
      NOTE(F5,250),
      NOTE(E5,1150),
      NOTE(D5,600),
      NOTE(E5,175),
      NOTE(D5,100),
      NOTE(C5,1050),
      NOTE(C5,550),
      NOTE(C5,175),
      NOTE(C5,100),
      NOTE(C5,100),
      NOTE(C5,150),
      NOTE(C5,1300),
      NOTE(C5,600),
      NOTE(B4,200),
      NOTE(C5,200),
      NOTE(D5,200),
      GOTO_STATE,

    LABEL(0),
#if 0
      NOTE(E5,1100),
      NOTE(E5,800),
      NOTE(D5,1600),
      SET_STATE(1),
      GOTO(5),

    LABEL(1),
      NOTE(C5,1600),
      SET_STATE(2),
      GOTO(5),

    LABEL(2),
      NOTE_EXTRA_GAP(C5,1600,100),
      NOTE(C5,450),
      NOTE(D5,150),
      NOTE(D5,50),
      NOTE(D5,50),
      NOTE(D5,1),
      NOTE(D5,1000),
      NOTE(G4,450),
      NOTE(E5,150),
      NOTE(E5,50),
      NOTE(E5,50),
      NOTE(E5,1),
      NOTE(E5,1000),
      NOTE(C5,350),
      NOTE(D5,250),
      NOTE(D5,100),
      NOTE(D5,75),
      NOTE(D5,350),
      NOTE(C5,150),
      NOTE(D5,250),
      NOTE(E5,1000),
      NOTE(E5,250),
      NOTE(G5,175),
      NOTE(A5,450),
      NOTE(A5,100),
      NOTE(E5,150),
      NOTE(G5,1000),
      NOTE(A5,100),
      NOTE(A5,50),
      NOTE(E5,150),
      NOTE(G5,1000),
      NOTE(A5,100),
      NOTE(A5,50),
      NOTE(E5,150),
      NOTE(G5,1000),
      NOTE(F5,450),
      NOTE(E5,650),
      NOTE(D5,1300),
      SET_STATE(3),
      GOTO(5),

    LABEL(3),
      NOTE_EXTRA_GAP(C5,1600, 100),
      NOTE(C5,550),
      NOTE(C5,175),
      NOTE(C5,100),
      NOTE(C5,100),
      NOTE(C5,150),
      NOTE(C5,1300),
      NOTE(C5,800),
      NOTE(B4,400),
      NOTE(C5,300),
      NOTE(D5,300),
      NOTE(C5,2300),
#endif
#endif
      END_OF_SONG,
};

/******************************************************************************
 * Local Functions
 *****************************************************************************/

static void playNote(uint32_t value, uint32_t time, uint32_t gap)
{
  /* play note for "time" ms and then silence for "gap" ms */
  DAC_SetDMATimeOut(0, value);
  LPC_DAC->CTRL  |= DAC_CNT_ENA;
  TIM_Waitms(time);
  LPC_DAC->CTRL    &= ~DAC_CNT_ENA;
  TIM_Waitms(gap);
}

static void executeSong(uint16_t *pSong)
{
  uint32_t note;
  uint32_t length;
  uint32_t gap;

  int pos;
  int i;
  int state = 0;
  for (pos = 0; ; pos+=3)
  {
    switch (song[pos]) {
    case TOKEN_END:
      // Disable DAC to make sure no lingering sound
      LPC_DAC->CTRL    &= ~DAC_CNT_ENA;
      return;
    case TOKEN_GOTO:
      i = 0;
      while (TRUE) {
        if (song[i] == TOKEN_LABEL && song[i+1]==song[pos+1]) {
          pos = i;
          break;
        }
        i+=3;
      }
      break;
    case TOKEN_SET:
      state = song[pos+1];
      break;
    case TOKEN_LABEL:
      break;
    case TOKEN_GOTO_STATE:
      i = 0;
      while (TRUE) {
        if (song[i] == TOKEN_LABEL && song[i+1]==state) {
          pos = i;
          break;
        }
        i+=3;
      }
      break;
    default:
      note = song[pos];
      length = song[pos+1];
      gap = song[pos+2];
      playNote(TO_NOTE(note), length*PLAYBACK_SPEED, gap*PLAYBACK_SPEED);
      break;
    }
  }
}

static void initLUT()
{
  short s;
  float volume = 0.15;
  for (s = 0; s < 360; s+=6)
  {
    float rad = (s * 3.141519) / 180.0;
    float sin = sinf(rad);

    /* Reducing amplitude to be +- 0.15 instead of +-1 */
    short val = volume * (1 + sin) * (1<<9); // +1 to get 0<->2 instead of -1<->1 and *1<<9 to use all 10 bits

    if (val >= 1024) {
      val = 1023;
    } else if (val < 0) {
      val = 0;
    }
    LUT_BUFFER[s/6] = DAC_BIAS_EN | DAC_VALUE(val);
  }
}

/******************************************************************************
 * Public functions
 *****************************************************************************/

void initSpeaker(void)
{
  initLUT();

  DMA_LLI_Struct.SrcAddr= (uint32_t)LUT_BUFFER;
  DMA_LLI_Struct.DstAddr= (uint32_t)&(LPC_DAC->CR);
  DMA_LLI_Struct.NextLLI= (uint32_t)&DMA_LLI_Struct;
  DMA_LLI_Struct.Control= (LUT_SIZE)
                          | (2<<18) /* Source transfer width is 32 bit */
                          | (2<<21) /* Destination transfer width is 32-bit */
                          | (1<<26) /* Source address is incremented after each transfer */
                          ;

  /* Configure GPDMA */
  GPDMA_Init();
  GPDMACfg.ChannelNum = 0;
  GPDMACfg.SrcMemAddr = (uint32_t)(LUT_BUFFER);
  GPDMACfg.DstMemAddr = 0;                        /* Not used */
  GPDMACfg.TransferSize = LUT_SIZE;
  GPDMACfg.TransferWidth = 0;                     /* Not used */
  GPDMACfg.TransferType = GPDMA_TRANSFERTYPE_M2P; /* Memory to peripheral */
  GPDMACfg.SrcConn = 0;                           /* Not used */
  GPDMACfg.DstConn = GPDMA_CONN_DAC;              /* Destination is the DAC */
  GPDMACfg.DMALLI = (uint32_t)&DMA_LLI_Struct;
  GPDMA_Setup(&GPDMACfg);

  /* Configure DAC */
  DAC_Init(0);
  DAC_ConverterConfigStruct.DBLBUF_ENA = RESET;
  DAC_ConverterConfigStruct.CNT_ENA    = SET;
  DAC_ConverterConfigStruct.DMA_ENA    = SET;
  DAC_ConfigDAConverterControl(0, &DAC_ConverterConfigStruct);

  DAC_SetDMATimeOut(0, WAIT_FOREVER);
  GPDMA_ChannelCmd(0, ENABLE);
}

void playSound()
{
  executeSong((uint16_t*)&song[0]);
}

