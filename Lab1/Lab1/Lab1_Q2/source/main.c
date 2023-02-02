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
#include "fsl_uart.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define TARGET_UART UART4
#define MSG_BUFF_SIZE (100u)

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
void setupUART() {
	uart_config_t config;

	UART_GetDefaultConfig(&config);
	config.baudRate_Bps = 57600;
	config.enableTx = true;
	config.enableRx = true;
	config.enableRxRTS = true;
	config.enableTxCTS = true;

	UART_Init(TARGET_UART, &config, CLOCK_GetFreq(kCLOCK_BusClk));
}
/*******************************************************************************
 * Code
 ******************************************************************************/
char msgBuffer[MSG_BUFF_SIZE];

void constructMessage(char* newChar) {
	if (*newChar == '.') {
		memset(&msgBuffer, 0, MSG_BUFF_SIZE);
		return;
	}

	strncat(msgBuffer, newChar, 1);

	return;
}

/*!
 * @brief Main function
 */
int main(void)
{
    char ch;
    char txbuff[] = "If anyone is connected, please scream\r\n";
    memset(&msgBuffer, 0, MSG_BUFF_SIZE);

    /* Init board hardware. */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitDebugConsole();

    setupUART();

    for (volatile int i = 0U; i < 10000000; i++){
    	__asm("NOP");
    }

    PRINTF("%s", txbuff);

    UART_WriteBlocking(TARGET_UART, txbuff, sizeof(txbuff)-1);
    while (1)
    {
    	UART_ReadBlocking(TARGET_UART, &ch, 1);
    	printf("%c\r\n", ch);
    	constructMessage(&ch);
    	printf("%s\n", msgBuffer);
    }
}
