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
#define FTM_MOTOR               FTM0
#define FTM_CHANNEL_DC_MOTOR    kFTM_Chnl_0
#define FTM_CHANNEL_SERVO_MOTOR kFTM_Chnl_3
/*******************************************************************************
 * Prototypes
 ******************************************************************************/
void setupPWM() {
	ftm_config_t ftmInfo;
	ftm_chnl_pwm_signal_param_t ftmParam_DC_Motor, ftmParam_Servo_Motor;
	ftm_pwm_level_select_t pwmLevel = kFTM_HighTrue;

	ftmParam_DC_Motor.chnlNumber = FTM_CHANNEL_DC_MOTOR;
	ftmParam_DC_Motor.level = pwmLevel;
	ftmParam_DC_Motor.dutyCyclePercent = 7;
	ftmParam_DC_Motor.firstEdgeDelayPercent = 0U;
	ftmParam_DC_Motor.enableComplementary   = false;
	ftmParam_DC_Motor.enableDeadtime        = false;


	ftmParam_Servo_Motor.chnlNumber = FTM_CHANNEL_DC_MOTOR;
	ftmParam_Servo_Motor.level = pwmLevel;
	ftmParam_Servo_Motor.dutyCyclePercent = 7;
	ftmParam_Servo_Motor.firstEdgeDelayPercent = 0U;
	ftmParam_Servo_Motor.enableComplementary   = false;
	ftmParam_Servo_Motor.enableDeadtime        = false;



	FTM_GetDefaultConfig(&ftmInfo);
	ftmInfo.prescale = kFTM_Prescale_Divide_128;
	FTM_Init(FTM_MOTOR, &ftmInfo);
	FTM_SetupPwm(FTM_MOTOR, &ftmParam_DC_Motor, 1U, kFTM_EdgeAlignedPwm, 50U, CLOCK_GetFreq(kCLOCK_BusClk));
	FTM_SetupPwm(FTM_MOTOR, &ftmParam_Servo_Motor, 1U, kFTM_EdgeAlignedPwm, 50U, CLOCK_GetFreq(kCLOCK_BusClk));
	FTM_StartTimer(FTM_MOTOR, kFTM_SystemClock);
}

void updatePWM_dutyCycle(ftm_chnl_t channel, float dutyCycle){
	uint32_t cnv, cnvFirstEdge = 0, mod;

	/* The CHANNEL_COUNT macro returns -1 if it cannot match the FTM instance */
	assert(-1 != FSL_FEATURE_FTM_CHANNEL_COUNTn(FTM_MOTOR));

	mod = FTM_MOTOR->MOD;
	if (dutyCycle == 0U) {
		/* Signal stays low */
		cnv = 0;
	} else {
		cnv = mod * dutyCycle;
		/* For 100% duty cycle */
		if (cnv >= mod) {
			cnv = mod + 1U;
		}
	}
	FTM_MOTOR->CONTROLS[channel].CnV = cnv;
}
/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * @brief Main function
 */
int main(void)
{
    uint8_t ch;
    int DCMotorSpeed, ServoMotorAngle;
    float dutyCycle;

    /* Init board hardware. */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitDebugConsole();

    setupPWM();
    // DELAY -------------------
    for (volatile int i = 0U; i < 1000000; i++) {
    	__asm("NOP");
    }

    updatePWM_dutyCycle(FTM_CHANNEL_DC_MOTOR, 0.0615);
    FTM_SetSoftwareTrigger(FTM_MOTOR, true);

    scanf("DC Motor Speed = %d", &DCMotorSpeed);
    scanf("Servo Motor Angle = %d", &ServoMotorAngle);
    dutyCycle = DCMotorSpeed * 0.025f/100.0f + 0.0615;
    updatePWM_dutyCycle(FTM_CHANNEL_DC_MOTOR, dutyCycle);

    FTM_SetSoftwareTrigger(FTM_MOTOR, true);


    while (1)
    {}
}
