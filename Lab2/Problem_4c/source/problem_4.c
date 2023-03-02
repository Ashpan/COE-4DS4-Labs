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
#include "semphr.h"
#include "event_groups.h"

/* Freescale includes. */
#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"

// User includes
#include <ctype.h>

#define TRUE 				 (1u)
#define FALSE           	 (0u)
#define NUMBER_OF_SEMAPHORES (2u)
#define USER_INPUT_SIZE		 (50u)
#define TAKE_INPUT_BIT       (1 << 0)
#define PRINT_UPPER_BIT      (1 << 1)
#define PRINT_LOWER_BIT 	 (1 << 2)

char userInputLower[USER_INPUT_SIZE];
char userInputUpper[USER_INPUT_SIZE];
uint8_t userInputFlag = FALSE;

void userInputTask(void *pvParameters) {
	EventGroupHandle_t event_group = (EventGroupHandle_t)pvParameters;
	BaseType_t status;
	int i = 0;

	EventBits_t bits = xEventGroupWaitBits(event_group, TAKE_INPUT_BIT, pdTRUE, pdFALSE, portMAX_DELAY);

	if (bits == TAKE_INPUT_BIT) {
		printf("Enter user input: ");
		fgets(userInputLower, USER_INPUT_SIZE, stdin);
		userInputLower[strcspn(userInputLower, "\n")] = 0; //Strip newline char

		while(userInputLower[i] != '\0') {
			userInputUpper[i] = toupper(userInputLower[i]);
			i++;
		}

		xEventGroupSetBits(event_group, PRINT_LOWER_BIT);
	}
	vTaskDelete(NULL);
}

void printfLowerTask(void *pvParameters) {
	EventGroupHandle_t event_group = (EventGroupHandle_t)pvParameters;
	BaseType_t status;
	EventBits_t bits;

	while (1)
	{
		bits = xEventGroupWaitBits(event_group, PRINT_LOWER_BIT, pdTRUE, pdFALSE, portMAX_DELAY);
		if (bits == PRINT_LOWER_BIT) {
			PRINTF("%s\n", userInputLower);
			xEventGroupSetBits(event_group, PRINT_UPPER_BIT);
		}
	}

	vTaskDelete(NULL);
}

void printfUpperTask(void *pvParameters) {
	EventGroupHandle_t event_group = (EventGroupHandle_t)pvParameters;
	BaseType_t status;
	EventBits_t bits;


	while (1)
	{
		bits = xEventGroupWaitBits(event_group, PRINT_UPPER_BIT, pdTRUE, pdFALSE, portMAX_DELAY);
		if (bits == PRINT_UPPER_BIT) {
			PRINTF("%s\n", userInputUpper);
			xEventGroupSetBits(event_group, PRINT_LOWER_BIT);
		}
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


	EventGroupHandle_t event_group = xEventGroupCreate();
	xEventGroupSetBits(event_group, TAKE_INPUT_BIT);



	status = xTaskCreate(userInputTask, "userInput", 200, (void *)event_group, 2, NULL);
	if (status != pdPASS)
	{
		PRINTF("Task creation failed!.\r\n");
		while (1);
	}
	status = xTaskCreate(printfLowerTask, "printLower", 200, (void *)event_group, 3, NULL);
	if (status != pdPASS)
	{
		PRINTF("Task creation failed!.\r\n");
		while (1);
	}

	status = xTaskCreate(printfUpperTask, "printUpper", 200, (void *)event_group, 3, NULL);
	if (status != pdPASS)
	{
		PRINTF("Task creation failed!.\r\n");
		while (1);
	}

	vTaskStartScheduler();

	while (1) {
		// Nothing
	}

	return 0;
}
