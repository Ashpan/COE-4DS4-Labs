#include "RC_Receiver_Component.h"

SemaphoreHandle_t rc_hold_semaphore;
TaskHandle_t rc_task_handle;
RC_Values rc_vals;

void setupRCReceiverComponent()
{

	setupRCPins();

	setupUART_RC();

    /*************** RC Task ***************/
	//Create RC Semaphore
	// Don't think we need to create RC semaphore until after we have terminal task working - Dharak

	// Create RC Task
	BaseType_t status = xTaskCreate(rcTask, "rcTask", 200, (void *)&rc_vals, 2, NULL);
	if (status != pdPASS)
	{
		PRINTF("rcTask creation failed!.\r\n");
		while (1);
	}
}

void setupRCPins()
{
	BOARD_InitRCPins();
}

void setupUART_RC()
{
	uart_config_t config;
    UART_GetDefaultConfig(&config);
    config.baudRate_Bps = 115200;
    config.enableTx = false;
    config.enableRx = true;
    UART_Init(UART1, &config, CLOCK_GetFreq(kCLOCK_CoreSysClk));
}

void rcTask()
{
	TsMotorData send_to_motor;

	while (1) {
		uint8_t *ptr = (uint8_t *)&rc_vals;

		UART_ReadBlocking(UART1, ptr, 1);

		if (*ptr != 0x20) {
			continue;
		}

		UART_ReadBlocking(UART1, &ptr[1], sizeof(RC_Values) - 1);

		if (rc_vals.header == 0x4020)
		{
			printf("Channel 1 = %d\t", rc_vals.ch1);
			printf("Channel 2 = %d\t", rc_vals.ch2);
			printf("Channel 3 = %d\t", rc_vals.ch3);
			printf("Channel 4 = %d\t", rc_vals.ch4);
			printf("Channel 5 = %d\t", rc_vals.ch5);
			printf("Channel 6 = %d\t", rc_vals.ch6);
			printf("Channel 7 = %d\t", rc_vals.ch7);
			printf("Channel 8 = %d\n", rc_vals.ch8);
		}
		// channel 3 goes 1000 to 2000
		// if reverse toggle, add a negative sign
		// if speed on low, scale from 0 to 33%
		// if speed on med, scale from 0 to 66%
		// if speed on high, dont scale

		double speed = ((rc_vals.ch3) / 10) - 100;

		switch (rc_vals.ch8) {
			case (1500):	// med speed
				speed *= (2/3);
				break;
			case (1000):
				speed *= (1/3);
				break;
			case (2000):
				break;
			default:
				printf("Unknown speed mode detected!\n");
				break;
		}

		// Check for reverse switch enabled/disabled
		if (rc_vals.ch6 > 1500) {
			speed *= -1;
		}

		send_to_motor.source = DATA_SOURCE_RC;
		send_to_motor.speed = speed;
		
		BaseType_t status = xQueueSendToBack(motor_queue, (void *)&send_to_motor, portMAX_DELAY);
		if (status != pdPASS) {
			PRINTF("Motor queue Send failed!.\r\n");
			while(1);
		}

		vTaskDelay(10 / portTICK_PERIOD_MS);
	}

}


