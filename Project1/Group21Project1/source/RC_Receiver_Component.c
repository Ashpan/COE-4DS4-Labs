#include "RC_Receiver_Component.h"
#include "fsl_uart.h"

#define TARGET_UART UART4

SemaphoreHandle_t rc_hold_semaphore;
TaskHandle_t rc_task_handle;

typedef struct {
	uint16_t header;
	uint16_t ch1;
	uint16_t ch2;
	uint16_t ch3;
	uint16_t ch4;
	uint16_t ch5;
	uint16_t ch6;
	uint16_t ch7;
	uint16_t ch8;
} RC_Values;

void setupRCReceiverComponent()
{
	setupRCPins();

	setupUART_RC();

    /*************** RC Task ***************/
	//Create RC Semaphore
	//Create RC Task
}

void setupRCPins()
{
	//Configure RC pins
	uart_config_t config;
	RC_Values rc_values;
	uint8_t* ptr = (uint8_t*) &rc_values;
	BOARD_InitBootPins();
	BOARD_InitBootClocks();
	UART_GetDefaultConfig(&config);
	config.baudRate_Bps = 115200;
	config.enableTx = false;
	config.enableRx = true;
	UART_Init(UART1, &config, CLOCK_GetFreq(kCLOCK_CoreSysClk));
	while (1)
		{
			UART_ReadBlocking(UART1, ptr, 1);
			if(*ptr != 0x20)
				continue;
			UART_ReadBlocking(UART1, &ptr[1], sizeof(rc_values) - 1);
			if(rc_values.header == 0x4020)
			{
				printf("Channel 1 = %d\t", rc_values.ch1);
				printf("Channel 2 = %d\t", rc_values.ch2);
				printf("Channel 3 = %d\t", rc_values.ch3);
				printf("Channel 4 = %d\t", rc_values.ch4);
				printf("Channel 5 = %d\t", rc_values.ch5);
				printf("Channel 6 = %d\t", rc_values.ch6);
				printf("Channel 7 = %d\t", rc_values.ch7);
				printf("Channel 8 = %d\r\n", rc_values.ch8);
			}
		}
}

void setupUART_RC()
{
	//setup UART for RC receiver

}

//void rcTask(void* pvParameters)
//{
//	//RC task implementation
//	 int data;
//
//	  while (1) {
//	    // Wait for data from radio receiver
//	    data = radio_receive(); //radio_receive defined by user
//
//	    // Send data to motor queue
//	    xQueueSendToBack(xMotorQueue, (void *) &data, portMAX_DELAY);
//
//	    // Send data to angle queue
//	    xQueueSendToBack(xAngleQueue, (void *) &data, portMAX_DELAY);
//
//	    // Send data to LED queue
//	    xQueueSendToBack(xLEDQueue, (void *) &data, portMAX_DELAY);
//
//	    // Check if hold semaphore is signaled
//	    if (xSemaphoreTake(rc_hold_semaphore, 0) == pdTRUE) {
//	      // Semaphore was signaled, pause task
//	      vTaskSuspend(NULL);
//	    }
//
//	  }
//
//}
//
//
