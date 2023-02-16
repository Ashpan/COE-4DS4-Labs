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

#define TRUE (1u)
#define FALSE (0u)

uint8_t inputFlag = FALSE;

void userInputTask(void *pvParameters) {
	char userInput[50] = "";
	QueueHandle_t queue1 = (QueueHandle_t)pvParameters;
	BaseType_t status;

	printf("Entered user input: ");
	scanf("%s", userInput);

	status = xQueueSendToBack(queue1, (void *)userInput, portMAX_DELAY);
	if (status != pdPASS)
	{
		PRINTF("Queue Send failed!.\r\n");
		while(1);
	}

	inputFlag = TRUE;

	vTaskDelete(NULL);
}

void printfTask(void *pvParameters) {
	char receivedInput[50] = "";
	QueueHandle_t queue1 = (QueueHandle_t)pvParameters;
	BaseType_t status;

	status = xQueueReceive(queue1, (void *)&receivedInput, portMAX_DELAY);
	if (status != pdPASS) {
		PRINTF("Queue Receive failed!.\r\n");
		while(1);
	}

	while (1) {
		if (inputFlag) {
			PRINTF("%s\r\n", receivedInput);
			vTaskDelay(1000 / portTICK_PERIOD_MS);
		}

		// Force scheduler to move to other tasks if inputFlag != TRUE
		vTaskDelay(10 / portTICK_PERIOD_MS);
	}

	vTaskDelete(NULL);
}

int main(void) {
	BaseType_t status;

	/* Init board hardware. */
	BOARD_InitBootPins();
	BOARD_InitBootClocks();
	BOARD_InitDebugConsole();

	QueueHandle_t queue1 = xQueueCreate(1, sizeof(int));
	if (queue1 == NULL) {
		PRINTF("Queue creation failed!.\r\n");
		while(1);
	}

	status = xTaskCreate(userInputTask, "userInput", 200, (void *)queue1, 2, NULL);
	if (status != pdPASS) {
		PRINTF("Task creation failed!.\r\n");
		while(1);
	}

	status = xTaskCreate(printfTask, "printf", 200, (void *)queue1, 2, NULL);
	if (status != pdPASS) {
		PRINTF("Task creation failed!.\r\n");
		while(1);
	}

	vTaskStartScheduler();

	for (;;);

	return 0;
}
