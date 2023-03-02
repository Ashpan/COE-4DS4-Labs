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

#include <ctype.h>

#define TRUE (1u)
#define FALSE (0u)
#define NUMBER_OF_SEMAPHORES (4u)
#define USER_INPUT_SIZE (50u)

#define LEFT_SEMAPHORE (1 << 0)
#define RIGHT_SEMAPHORE (1 << 1)
#define UP_SEMAPHORE (1 << 2)
#define DOWN_SEMAPHORE (1 << 3)

char userInputLower[USER_INPUT_SIZE];
char userInputUpper[USER_INPUT_SIZE];
uint8_t userInputFlag = FALSE;

void producer_semaphore(void *pvParameters)
{
	SemaphoreHandle_t *semaphores = (SemaphoreHandle_t *)pvParameters;
	SemaphoreHandle_t leftSemaphore = semaphores[0];
	SemaphoreHandle_t rightSemaphore = semaphores[1];
	SemaphoreHandle_t upSemaphore = semaphores[2];
	SemaphoreHandle_t downSemaphore = semaphores[3];

	PRINTF("PRODUCER_SEMAPHORE\n");

    char c;

    while (1)
    {
    	scanf("%s", &c);
        switch (c)
        {
            case 'a':
                xSemaphoreGive(leftSemaphore);
                break;
            case 's':
                xSemaphoreGive(downSemaphore);
                break;
            case 'd':
                xSemaphoreGive(rightSemaphore);
                break;
            case 'w':
                xSemaphoreGive(upSemaphore);
                break;
        }
	vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}

void consumer_semaphore(void *pvParameters)
{
	SemaphoreHandle_t *semaphores = (SemaphoreHandle_t *)pvParameters;
	SemaphoreHandle_t leftSemaphore = semaphores[0];
	SemaphoreHandle_t rightSemaphore = semaphores[1];
	SemaphoreHandle_t upSemaphore = semaphores[2];
	SemaphoreHandle_t downSemaphore = semaphores[3];

	PRINTF("CONSUMER_SEMAPHORE\n");
//    vTaskDelay(100 / portTICK_PERIOD_MS);
    while (1)
    {
        if (xSemaphoreTake(leftSemaphore, portMAX_DELAY) == pdPASS)
        {
            PRINTF("Left\r\n");
        }
        if (xSemaphoreTake(rightSemaphore, portMAX_DELAY) == pdPASS)
        {
            PRINTF("Right\r\n");
        }
        if (xSemaphoreTake(upSemaphore, portMAX_DELAY) == pdPASS)
        {
            PRINTF("Up\r\n");
        }
        if (xSemaphoreTake(downSemaphore, portMAX_DELAY) == pdPASS)
        {
            PRINTF("Down\r\n");
        }
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

	SemaphoreHandle_t *semaphores = (SemaphoreHandle_t *)malloc(NUMBER_OF_SEMAPHORES * sizeof(SemaphoreHandle_t));
    semaphores[0] = xSemaphoreCreateBinary();
    semaphores[1] = xSemaphoreCreateBinary();
    semaphores[2] = xSemaphoreCreateBinary();
    semaphores[3] = xSemaphoreCreateBinary();

    status = xTaskCreate(producer_semaphore, "producer", 200, (void *)semaphores, 3, NULL);
    if (status != pdPASS)
    {
        PRINTF("Task creation failed!.\r\n");
        while (1)
            ;
    }

    status = xTaskCreate(consumer_semaphore, "consumer", 200, (void *)semaphores, 3, NULL);
    if (status != pdPASS)
    {
        PRINTF("Task creation failed!.\r\n");
        while (1)
            ;
    }

    vTaskStartScheduler();
    while (1)
    {
        // Nothing
    }

    return 0;
}
