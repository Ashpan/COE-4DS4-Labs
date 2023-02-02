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
#include "fsl_uart.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define FTM_MOTOR               FTM0
#define FTM_CHANNEL_DC_MOTOR    kFTM_Chnl_0
#define FTM_CHANNEL_SERVO_MOTOR kFTM_Chnl_3

#define TARGET_UART UART4
#define MSG_BUFF_SIZE (100u)

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

	ftmParam_Servo_Motor.chnlNumber = FTM_CHANNEL_SERVO_MOTOR;
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

char speedBuffer[MSG_BUFF_SIZE];
char angleBuffer[MSG_BUFF_SIZE];

typedef enum {
	MSG_UNKNOWN = 0,
	MSG_SPEED_READY,
	MSG_ANGLE_READY,
	MSG_IN_PROGRESS
} TeMessageReady;

typedef enum {
	SELECT_UNKNOWN = 0,
	SELECT_SPEED,
	SELECT_ANGLE
} TeMessageSelector;

TeMessageReady constructMessage(char* newChar) {
	// Flag used to reset message buffers on next function invocations
	static uint8_t resetBuffers = 0;
	// messageSelector:
	// 0 -> speed section of message
	// 1 -> angle section of message
	static TeMessageSelector messageSelector = SELECT_SPEED;

	if (resetBuffers == 1) {
		resetBuffers = 0;
		memset(speedBuffer, 0, MSG_BUFF_SIZE);
		memset(angleBuffer, 0, MSG_BUFF_SIZE);
	}

	if (*newChar == '.') {
		resetBuffers = 1;
		messageSelector = SELECT_SPEED;
		return MSG_ANGLE_READY;
	}

	if (*newChar == ',') {
		messageSelector = SELECT_ANGLE;
		return MSG_SPEED_READY;
	}

	if (messageSelector == SELECT_SPEED) {
		strncat(speedBuffer, newChar, 1);
	}

	if (messageSelector == SELECT_ANGLE) {
		strncat(angleBuffer, newChar, 1);
	}

	return MSG_IN_PROGRESS;
}
/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * @brief Main function
 */
int main(void)
{
    char ch;
    char txbuff[] = "CAN I GET A HOOOYYYAAAA\r\n";
    memset(&speedBuffer, 0, MSG_BUFF_SIZE);
    memset(&angleBuffer, 0, MSG_BUFF_SIZE);

    int DCMotorSpeed, ServoMotorAngle;
    float DCMotorDutyCycle, ServoMotorDutyCycle;
    float servoInputStart = -100.0;
    float servoInputEnd = 100.0;
    float servoOutputStart = 0.05;
    float servoOutputEnd = 0.1;

    TeMessageReady msgState;

    /* Init board hardware. */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitDebugConsole();

    setupUART();
    setupPWM();
    // DELAY -------------------
    for (volatile int i = 0U; i < 1000000; i++) {
    	__asm("NOP");
    }

    UART_WriteBlocking(TARGET_UART, txbuff, sizeof(txbuff)-1);
    printf("%s\n", txbuff);

    updatePWM_dutyCycle(FTM_CHANNEL_DC_MOTOR, 0.0615);
    FTM_SetSoftwareTrigger(FTM_MOTOR, true);

    while (1)
	{
		UART_ReadBlocking(TARGET_UART, &ch, 1);
		printf("%c\r\n", ch);
		msgState = constructMessage(&ch);

		if (msgState == MSG_SPEED_READY) {
			printf("Speed string: (%s)\n", speedBuffer);
			DCMotorSpeed = atoi(speedBuffer);
			printf("Speed: (%i)\n", DCMotorSpeed);
		    DCMotorDutyCycle = DCMotorSpeed * 0.025f/100.0f + 0.0615;
			updatePWM_dutyCycle(FTM_CHANNEL_DC_MOTOR, DCMotorDutyCycle);
		    FTM_SetSoftwareTrigger(FTM_MOTOR, true);
		}

		if (msgState == MSG_ANGLE_READY) {
			printf("Angle string: (%s)\n", angleBuffer);
			ServoMotorAngle = atoi(angleBuffer);
			printf("Angle: (%i)\n", ServoMotorAngle);
			ServoMotorDutyCycle = servoOutputStart + ((servoOutputEnd - servoOutputStart) / (servoInputEnd - servoInputStart)) * (ServoMotorAngle - servoInputStart);
			updatePWM_dutyCycle(FTM_CHANNEL_SERVO_MOTOR, ServoMotorDutyCycle);
		    FTM_SetSoftwareTrigger(FTM_MOTOR, true);
		}
	}
}
