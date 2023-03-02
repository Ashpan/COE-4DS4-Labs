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

char userInputLower[USER_INPUT_SIZE];
char userInputUpper[USER_INPUT_SIZE];
uint8_t userInputFlag = FALSE;

void userInputTask(void *pvParameters) {
	SemaphoreHandle_t *semaphores = (SemaphoreHandle_t *)pvParameters;
	SemaphoreHandle_t printingLowerSema = semaphores[0];
	SemaphoreHandle_t printingUpperSema = semaphores[1];
	BaseType_t status;
	int i = 0;

	printf("Entered user input: ");
	fgets(userInputLower, USER_INPUT_SIZE, stdin);
	userInputLower[strcspn(userInputLower, "\n")] = 0; //Strip newline char
	while(userInputLower[i] != '\0') {
		userInputUpper[i] = toupper(userInputLower[i]);
		i++;
	}

	userInputFlag = TRUE;
	xSemaphoreGive(printingLowerSema);
	vTaskDelete(NULL);
}

void printfLowerTask(void *pvParameters) {
	SemaphoreHandle_t *semaphores = (SemaphoreHandle_t *)pvParameters;
	SemaphoreHandle_t printingLowerSema = semaphores[0];
	SemaphoreHandle_t printingUpperSema = semaphores[1];
	BaseType_t status;

	while (1)
	{
		status = xSemaphoreTake(printingLowerSema, portMAX_DELAY);
		// If the printingSema is taken, that means the other printf task is currently running
		if (status == pdPASS) {
			PRINTF("%s\r\n", userInputLower);
			xSemaphoreGive(printingUpperSema);
		}
	}

	vTaskDelete(NULL);
}

void printfUpperTask(void *pvParameters) {
	SemaphoreHandle_t *semaphores = (SemaphoreHandle_t *)pvParameters;
	SemaphoreHandle_t printingLowerSema = semaphores[0];
	SemaphoreHandle_t printingUpperSema = semaphores[1];
	BaseType_t status;

	while (1)
	{
		status = xSemaphoreTake(printingUpperSema, portMAX_DELAY);
		// If the userInputSema is taken, that means the user input has been stored and we can continue with printing
		if (status == pdPASS) {
			PRINTF("%s\r\n", userInputUpper);
			xSemaphoreGive(printingLowerSema);
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

	SemaphoreHandle_t *semaphores = (SemaphoreHandle_t *)malloc(NUMBER_OF_SEMAPHORES * sizeof(SemaphoreHandle_t));
	semaphores[0] = xSemaphoreCreateBinary(); // Lower and Upper case printing semaphore
	semaphores[1] = xSemaphoreCreateBinary(); // Lower and Upper case printing semaphore



	// Take the user input semaphore immediately so the printf tasks, which run first due to
	// priority, do not take the semaphore

	status = xTaskCreate(userInputTask, "userInput", 200, (void *)semaphores, 2, NULL);
	if (status != pdPASS)
	{
		PRINTF("Task creation failed!.\r\n");
		while (1);
	}
	status = xTaskCreate(printfLowerTask, "printLower", 200, (void *)semaphores, 3, NULL);
	if (status != pdPASS)
	{
		PRINTF("Task creation failed!.\r\n");
		while (1);
	}

	status = xTaskCreate(printfUpperTask, "printUpper", 200, (void *)semaphores, 3, NULL);
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
