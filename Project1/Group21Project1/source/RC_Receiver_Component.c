#include "RC_Receiver_Component.h"
#include "fsl_uart.h"

#define TARGET_UART UART4

SemaphoreHandle_t rc_hold_semaphore;
TaskHandle_t rc_task_handle;

typedef struct {
	uint16_t header;
	uint16_t ch1;
	uint16_t ch3;
	uint16_t ch6;
} RC_Values;

uart_config_t config;
RC_Values rc_values;
uint8_t* ptr = (uint8_t*) &rc_values;

void setupRCReceiverComponent()
{
	setupRCPins();

	setupUART_RC();

    /*************** RC Task ***************/
	//Create RC Semaphore
	//Create RC Task
	BaseType_t status;
	status = xTaskCreate(rcTask, "rcTask", 200, NULL, 2, NULL);
	if (status != pdPASS)
	{
	    PRINTF("Task creation failed!.\r\n");
	    while (1);
	}
}

void setupRCPins()
{
	//Configure RC pins

	BOARD_InitBootPins();
	UART_GetDefaultConfig(&config);
	config.baudRate_Bps = 115200;
	config.enableTx = false;
	config.enableRx = true;

}

void setupUART_RC()
{
	//setup UART for RC receiver
    UART_Init(UART1, &config, CLOCK_GetFreq(kCLOCK_CoreSysClk));
}


void rcTask(void* pvParameters)
{

	while (1)
		{
            // Check if the task should be paused
			UART_ReadBlocking(UART1, ptr, 1);
			if(*ptr != 0x20)
				continue;
			UART_ReadBlocking(UART1, &ptr[1], sizeof(rc_values) - 1);

			if(rc_values.header == 0x4020)
			{
				printf("Channel 1 = %d\t", rc_values.ch1);
				printf("Channel 3 = %d\t", rc_values.ch3);
				printf("Channel 6 = %d\t", rc_values.ch6);
			}
			printf("\n");
            // Send the decoded values to the corresponding queues
            //xQueueSendToBack(angleQueue, &rc_values.ch1, portMAX_DELAY);
            //xQueueSendToBack(motorQueue, &rc_values.ch3, portMAX_DELAY);
            //xQueueSendToBack(motorQueue, &rc_values.ch6, portMAX_DELAY);
            xQueueSendToBack(led_queue, (void *)&rc_values.ch3, portMAX_DELAY);
//            printf("Waiting %d", uxQueueMessagesWaiting(led_queue));

            // Wait for next data from radio receiver
            vTaskDelay(10 / portTICK_PERIOD_MS); // 10 ms delay

		}



}

