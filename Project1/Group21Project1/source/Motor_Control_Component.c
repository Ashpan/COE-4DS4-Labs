#include "Motor_Control_Component.h"

QueueHandle_t motor_queue;
QueueHandle_t angle_queue;

void setupMotorComponent()
{
	setupMotorPins();
	
	ftm_config_t ftmInfo;
	FTM_GetDefaultConfig(&ftmInfo);
	ftmInfo.prescale = kFTM_Prescale_Divide_128;
	FTM_Init(FTM_MOTORS, &ftmInfo);

	setupDCMotor();
	setupServo();

	FTM_StartTimer(FTM_MOTORS, kFTM_SystemClock);

    /*************** Motor Task ***************/
	//Create Motor Queue
	QueueHandle_t motor_queue = xQueueCreate(1, sizeof(TsMotorData));
	if (motor_queue == NULL) {
		PRINTF("Motor queue creation failed!.\r\n");
		while(1);
	}

	//Create Motor Task
	BaseType_t status = xTaskCreate(motorTask, "motorTask", 200, NULL, 3, NULL);
	if (status != pdPASS)
	{
		PRINTF("rcTask creation failed!.\r\n");
		while (1);
	}

    /*************** Position Task ***************/
	//Create Angle Queue
	QueueHandle_t angle_queue = xQueueCreate(1, sizeof(TsAngleData));
	if (angle_queue == NULL) {
		PRINTF("Angle queue creation failed!.\r\n");
		while(1);
	}
	
	//Create Position Task
}


// Configure PWM pins for DC and Servo motors
void setupMotorPins()
{
	BOARD_InitMotorPins();
}

//Initialize PWM for DC motor
void setupDCMotor() {
	ftm_config_t ftmInfo;
	ftm_chnl_pwm_signal_param_t ftmParam_DC_Motor;
	ftm_pwm_level_select_t pwmLevel = kFTM_HighTrue;

	ftmParam_DC_Motor.chnlNumber = FTM_CHANNEL_DC_MOTOR;
	ftmParam_DC_Motor.level = pwmLevel;
	ftmParam_DC_Motor.dutyCyclePercent = 7;
	ftmParam_DC_Motor.firstEdgeDelayPercent = 0U;
	ftmParam_DC_Motor.enableComplementary   = false;
	ftmParam_DC_Motor.enableDeadtime        = false;
	FTM_SetupPwm(FTM_MOTORS, &ftmParam_DC_Motor, 1U, kFTM_EdgeAlignedPwm, 50U, CLOCK_GetFreq(kCLOCK_BusClk));
}

//Initialize PWM for Servo motor
void setupServo()
{
	ftm_config_t ftmInfo;
	ftm_chnl_pwm_signal_param_t ftmParam_Servo_Motor;
	ftm_pwm_level_select_t pwmLevel = kFTM_HighTrue;
	ftmParam_Servo_Motor.chnlNumber = FTM_CHANNEL_SERVO_MOTOR;
	ftmParam_Servo_Motor.level = pwmLevel;
	ftmParam_Servo_Motor.dutyCyclePercent = 7;
	ftmParam_Servo_Motor.firstEdgeDelayPercent = 0U;
	ftmParam_Servo_Motor.enableComplementary   = false;
	ftmParam_Servo_Motor.enableDeadtime        = false;
	FTM_SetupPwm(FTM_MOTORS, &ftmParam_Servo_Motor, 1U, kFTM_EdgeAlignedPwm, 50U, CLOCK_GetFreq(kCLOCK_BusClk));
}

void updatePWM_dutyCycle(ftm_chnl_t channel, float dutyCycle)
{
	uint32_t cnv, cnvFirstEdge = 0, mod;

	/* The CHANNEL_COUNT macro returns -1 if it cannot match the FTM instance */
	assert(-1 != FSL_FEATURE_FTM_CHANNEL_COUNTn(FTM_MOTORS));

	mod = FTM_MOTORS->MOD;
	if(dutyCycle == 0U)
	{
		/* Signal stays low */
		cnv = 0;
	}
	else
	{
		cnv = mod * dutyCycle;
		/* For 100% duty cycle */
		if (cnv >= mod)
		{
			cnv = mod + 1U;
		}
	}

	FTM_MOTORS->CONTROLS[channel].CnV = cnv;
}

void motorTask(void* pvParameters)
{
	TsMotorData motor_data;
	uint8_t speed = 0;
	QueueHandle_t queue = (QueueHandle_t)pvParameters;

	BaseType_t status = xQueueReceive(queue, (void *)&motor_data, portMAX_DELAY);
	if (status != pdPASS) {
		PRINTF("Motor queue receive failed!.\r\n");
		while(1);
	}

	switch (motor_data.source) {
		case (DATA_SOURCE_RC):
			speed = motor_data.speed;
			int DCMotorDutyCycle = speed * 0.025f/100.0f + 0.0615;
			updatePWM_dutyCycle(FTM_CHANNEL_DC_MOTOR, DCMotorDutyCycle);
			FTM_SetSoftwareTrigger(FTM_MOTORS, true);
			break;
		// case (DATA_SOURCE_TERM) {

		// 	break;
		// }
		// case (DATA_SOURCE_ACCEL) {

		// 	break;
		// }
		default:
			break;
	}

	vTaskDelay(10 / portTICK_PERIOD_MS);
}

void positionTask(void* pvParameters)
{
	//Position task implementation
}
