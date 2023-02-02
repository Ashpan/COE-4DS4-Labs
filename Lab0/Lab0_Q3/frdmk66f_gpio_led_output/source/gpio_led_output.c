/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"
#include "fsl_debug_console.h"
#include "fsl_gpio.h"
#include "gpio_driver.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define BOARD_RED_LED_GPIO        (GPIOD)
#define BOARD_GREEN_BLUE_LED_GPIO (GPIO_C)

#define BOARD_RED_LED_GPIO_PIN   (1u)
#define BOARD_BLUE_LED_GPIO_PIN  (8u)
#define BOARD_GREEN_LED_GPIO_PIN (9u)

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/*!
 * @brief delay a while.
 */
void delay(void);

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/
void delay(void)
{
    volatile uint32_t i = 0;
    for (i = 0; i < 800000; ++i)
    {
        __asm("NOP"); /* delay */
    }
}

/*!
 * @brief Main function
 */
int main(void)
{
	gpio_pin_config_t led_config = {
			kGPIO_DigitalOutput,
			0,
	};

    /* Board pin, clock, debug console init */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitDebugConsole();

    /* Print a note to terminal. */
    PRINTF("\r\n GPIO Driver example\r\n");
    PRINTF("\r\n The LED is blinking.\r\n");

    /* Init output LED GPIO. */
	GPIO_PinInit(BOARD_RED_LED_GPIO, BOARD_RED_LED_GPIO_PIN, &led_config);
	GPIO_PinInit(BOARD_GREEN_BLUE_LED_GPIO, BOARD_GREEN_LED_GPIO_PIN, &led_config);
	GPIO_PinInit(BOARD_GREEN_BLUE_LED_GPIO, BOARD_BLUE_LED_GPIO_PIN, &led_config);

    while (1)
    {
    	GPIO_PortToggle(BOARD_GREEN_BLUE_LED_GPIO, 1 << BOARD_BLUE_LED_GPIO_PIN);
        delay();
        GPIO_PortToggle(BOARD_GREEN_BLUE_LED_GPIO, 1 << BOARD_BLUE_LED_GPIO_PIN);

        GPIO_PortToggle(BOARD_GREEN_BLUE_LED_GPIO, 1 << BOARD_GREEN_LED_GPIO_PIN);
        delay();
        GPIO_PortToggle(BOARD_GREEN_BLUE_LED_GPIO, 1 << BOARD_GREEN_LED_GPIO_PIN);

        GPIO_PortToggle(BOARD_RED_LED_GPIO, 1 << BOARD_RED_LED_GPIO_PIN);
        delay();
        GPIO_PortToggle(BOARD_RED_LED_GPIO, 1 << BOARD_RED_LED_GPIO_PIN);
        delay();
    }
}
