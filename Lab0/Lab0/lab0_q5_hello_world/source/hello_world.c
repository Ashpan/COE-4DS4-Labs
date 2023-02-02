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
#include "fsl_ftm.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/


/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * @brief Main function
 */

void pwm_setup() {
	ftm_config_t ftmInfo;
	ftm_chnl_pwm_signal_param_t ftmParamRed;
	ftm_chnl_pwm_signal_param_t ftmParamGreen;
	ftm_chnl_pwm_signal_param_t ftmParamBlue;
	ftmParamRed.chnlNumber = kFTM_Chnl_1;
	ftmParamRed.level = kFTM_HighTrue;
	ftmParamRed.dutyCyclePercent = 0;
	ftmParamRed.firstEdgeDelayPercent = 0U;
	ftmParamRed.enableComplementary = false;
	ftmParamRed.enableDeadtime = false;

	ftmParamGreen.chnlNumber = kFTM_Chnl_5;
	ftmParamGreen.level = kFTM_HighTrue;
	ftmParamGreen.dutyCyclePercent = 0;
	ftmParamGreen.firstEdgeDelayPercent = 0U;
	ftmParamGreen.enableComplementary = false;
	ftmParamGreen.enableDeadtime = false;

	ftmParamBlue.chnlNumber = kFTM_Chnl_4;
	ftmParamBlue.level = kFTM_HighTrue;
	ftmParamBlue.dutyCyclePercent = 0;
	ftmParamBlue.firstEdgeDelayPercent = 0U;
	ftmParamBlue.enableComplementary = false;
	ftmParamBlue.enableDeadtime = false;

	FTM_GetDefaultConfig(&ftmInfo);

	FTM_Init(FTM3, &ftmInfo);
	FTM_SetupPwm(FTM3, &ftmParamRed, 1U, kFTM_EdgeAlignedPwm, 5000U, CLOCK_GetFreq(
	kCLOCK_BusClk));
	FTM_SetupPwm(FTM3, &ftmParamGreen, 1U, kFTM_EdgeAlignedPwm, 5000U, CLOCK_GetFreq(
	kCLOCK_BusClk));
	FTM_SetupPwm(FTM3, &ftmParamBlue, 1U, kFTM_EdgeAlignedPwm, 5000U, CLOCK_GetFreq(
	kCLOCK_BusClk));
	FTM_StartTimer(FTM3, kFTM_SystemClock);
}

int main(void) {
	char ch;
	unsigned long hex_code = 0;

	printf("Hello world11!11!\n");

	/* Init board hardware. */
	BOARD_InitBootPins();
	BOARD_InitBootClocks();
	BOARD_InitDebugConsole();

	pwm_setup();

	scanf("%x", &hex_code);

	printf("Duty cycles (%X)\n", hex_code);

	FTM_UpdatePwmDutycycle(FTM3, kFTM_Chnl_1, kFTM_EdgeAlignedPwm, (((hex_code >> 16) & (0xFF))/255)*100);
	FTM_UpdatePwmDutycycle(FTM3, kFTM_Chnl_5, kFTM_EdgeAlignedPwm, (((hex_code >> 8) & (0xFF))/255)*100);
	FTM_UpdatePwmDutycycle(FTM3, kFTM_Chnl_4, kFTM_EdgeAlignedPwm, (((hex_code) & (0xFF))/255)*100);
	FTM_SetSoftwareTrigger(FTM3, true);

	while (1)
	{
	}
}
