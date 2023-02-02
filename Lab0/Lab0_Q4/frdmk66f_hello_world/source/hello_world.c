/*
 * Copyright (c) 2013 - 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"

#include <stdint.h>
#include <stdio.h>

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define LOC_1 (0x20001000u)
#define LOC_2 (0x20001001u)
#define LOC_3 (0x20001005u)
#define LOC_4 (0x20001007u)

void lab0_setValues() {
	(*(uint8_t*)LOC_1) = 0xAC;
	(*(uint32_t*)LOC_2) = 0xAABBCCDD;
	(*(uint16_t*)LOC_3) = 0xABCD;
	(*(uint32_t*)LOC_4) = 0xAABBCCDD;

	return;
}

typedef struct __attribute__((__packed__)) {
	int location_1;
	char location_2;
	int location_3;
}ARBITRARY_MODULE;

#define MODULE ((ARBITRARY_MODULE*)0x20001000)

void testFunction2() {
	MODULE->location_1 = 0xAAAAAAAA;
	MODULE->location_2 = 0xBB;
	MODULE->location_3 = 0xCCCCCCCC;
}



/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * @brief Main function
 */
int main(void)
{
    char ch;

    /* Init board hardware. */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitDebugConsole();

    PRINTF("hello world.\r\n");

    testFunction2();

    while (1)
    {
        ch = GETCHAR();
        PUTCHAR(ch);
    }
}
