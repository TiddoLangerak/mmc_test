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
#ifndef __CFGMEM_H
#define __CFGMEM_H

/*
 * Loading the application to 0x80000000.
 * Reserving 1MB to the application.
 */
#define SDRAM_END  (SDRAM_BASE + SDRAM_SIZE)

#define MEM_ETH_BASE (SDRAM_BASE)
#define MEM_ETH_SIZE (0x00010000)

#define MEM_GUI_BASE (MEM_ETH_BASE + MEM_ETH_SIZE)
#define MEM_GUI_SIZE (0x01000000)

#define MEM_FB_BASE (MEM_GUI_BASE + MEM_GUI_SIZE)
#define MEM_FB_SIZE (0x00100000)

#define MEM_HEAP_BASE (MEM_FB_BASE + MEM_FB_SIZE)
#define MEM_HEAP_SIZE (SDRAM_END - MEM_HEAP_BASE + 1)

#endif /* end __CFGMEM_H */

