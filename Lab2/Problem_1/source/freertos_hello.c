/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

/* Freescale includes. */
#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"

#define TRUE  (1u)
#define FALSE (0u)

char userInput[50] = "";
uint8_t inputFlag = FALSE;

void userInputTask(void* pvParameters) {
	printf("Entered user input: ");
	scanf("%s", userInput);
	inputFlag = TRUE;

	vTaskDelete(NULL);
}

void printfTask(void* pvParameters) {
	while(1) {
		if (inputFlag) {
			PRINTF("%s.\r\n", userInput);
			vTaskDelay(1000 / portTICK_PERIOD_MS);
		}

		// Force scheduler to move to other tasks if inputFlag != TRUE
		vTaskDelay(10 / portTICK_PERIOD_MS);
	}

	vTaskDelete(NULL);
}

int main(void)
{
	BaseType_t status;

	/* Init board hardware. */
	BOARD_InitBootPins();
	BOARD_InitBootClocks();
	BOARD_InitDebugConsole();

	status = xTaskCreate(userInputTask, "userInputTask", 200, NULL, 2, NULL);
	if (status != pdPASS)
	{
		PRINTF("Task creation failed!.\r\n");
		while (1);
	}

	status = xTaskCreate(printfTask, "printfTask", 200, NULL, 3, NULL);
	if (status != pdPASS) {
		PRINTF("Task creation failed!.\r\n");
		while (1);
	}

	vTaskStartScheduler();

	for (;;);
	
	return 0;
}
