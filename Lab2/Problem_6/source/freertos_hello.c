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

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* Task priorities. */
#define hello_task_PRIORITY (configMAX_PRIORITIES - 1)
/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static void hello_task(void *pvParameters);

/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * @brief Application entry point.
 */

SemaphoreHandle_t xSemaphore;

static void hello_task(void *pvParameters)
{
	BaseType_t status;

    while(1)
    {
    	status = xSemaphoreTake(xSemaphore, portMAX_DELAY);
		if (status != pdPASS)
		{
			PRINTF("Failed to acquire semaphore\r\n");
			while (1);
		}
        PRINTF("Hello world.\r\n");
    }
}

void timerCallbackFunction2(TimerHandle_t timer_handle)
{
	BaseType_t status;

	xSemaphoreGive(xSemaphore);
}

int main(void)
{
	BaseType_t status;
    /* Init board hardware. */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitDebugConsole();

    xSemaphore = xSemaphoreCreateBinary();

    status = xTaskCreate(hello_task, "Hello_task", 200, NULL, 2, NULL);
    if (status != pdPASS)
    {
		PRINTF("Task creation failed!.\r\n");
		while (1);
    }

    TimerHandle_t timer_handle2 = xTimerCreate("Periodic timer",
											1000 / portTICK_PERIOD_MS,
											pdTRUE,
											NULL,
											timerCallbackFunction2);

    status = xTimerStart(timer_handle2, 0);
    if(status != pdPASS)
    {
		PRINTF("Couldn’t start the timer!.\r\n");
		while (1);
    }


    vTaskStartScheduler();
    while (1)
    {}
}

