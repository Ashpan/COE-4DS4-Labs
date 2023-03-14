/* This task receives events from the event group and act if any of
‘w’, ‘a’, ‘s’, or ‘d’ is pressed. The task responds to the events
by sending a fixed value to Motor, Angle, and/or LED queues.
Additionally, the task signals Hold semaphore to pause RC task
as long as it receives events, and after the events stop for a certain
period, it allows RC task to resume
*/

#include "Terminal_Component.h"

#define MODULE_NAME "Terminal"

EventGroupHandle_t event_group;
QueueHandle_t uart_queue;

void setupTerminalComponent()
{
	printf("\n");
	BaseType_t status;

	setupTerminalPins();

//	setupUART();

    /*************** UART Task ***************/
	uart_queue = xQueueCreate(10, sizeof(char*));
	if (uart_queue == NULL)
	{
		printf("Queue creation failed!.\r\n");
		while (1);
	}

// 	Currently breaks the the terminal module

//    status = xTaskCreate(uartTask, "UART task", 200, NULL, 2, NULL);
//    if (status != pdPASS)
//    {
//        printf("Task creation failed!.\r\n");
//        while (1);
//    }

    /*************** Terminal Control Task ***************/
    //Create Event groups
    event_group = xEventGroupCreate();

	//Create Terminal Control Task
    status = xTaskCreate(terminalControlTask, "terminalControlTask", 200, (void*)event_group, 3, NULL);

    if (status != pdPASS)
    {
    	PRINTF("Task creation failed!.\r\n");
    	while (1);
    }


}

void setupTerminalPins()
{
    //Configure UART pins

	/* Port C Clock Gate Control: Clock enabled */
	CLOCK_EnableClock(kCLOCK_PortC);

	PORT_SetPinMux(PORTC, 12U, kPORT_MuxAlt3); // UART4_RTS_b
	PORT_SetPinMux(PORTC, 13U, kPORT_MuxAlt3); // UART4_CTS_b
	PORT_SetPinMux(PORTC, 14U, kPORT_MuxAlt3); // UART4_RTX
	PORT_SetPinMux(PORTC, 15U, kPORT_MuxAlt3); // UART4_TX
}

void sendMessage(const char *format, ...)
{
	va_list args;

	char* str = (char*)pvPortMalloc(250 * sizeof(char));

	va_start(args, format);
	vsprintf(str, format, args);

	if(xQueueSendToBack(uart_queue, (void *) &str, portMAX_DELAY) != pdPASS )
	{
		PRINTF("Send message to uart_queue failed!.\r\n");
		while (1);
	}

	va_end(args);
}

void setupUART()
{

	//Setup UART for sending and receiving
}

void uartTask(void* pvParameters)
{
	char* welcome_message = "UART task started\n\r";
	char* received_str;
	BaseType_t status;

	UART_WriteBlocking(TARGET_UART, welcome_message, strlen(welcome_message));

	while(1)
	{
		status = xQueueReceive(uart_queue, (void *) &received_str, portMAX_DELAY);
		if (status != pdPASS)
		{
			PRINTF("Queue Send failed!.\r\n");
			while (1);
		}
		UART_WriteBlocking(TARGET_UART, received_str, strlen(received_str));
		vPortFree(received_str);
	}
}

void UART4_RX_TX_IRQHandler()
{
	//UART ISR
}

void terminalControlTask(void* pvParameters)
{
	printf("%s: terminalControlTask created\n", MODULE_NAME);

	//Terminal Control Task implementation

	EventGroupHandle_t event_group = (EventGroupHandle_t) pvParameters;
	BaseType_t status;

	char c;
	while (1) {
	  scanf("%c", & c);
	  switch (c) {
	  case 'a':
	    xEventGroupSetBits(event_group, LEFT_BIT);
	    break;
	  case 's':
	    xEventGroupSetBits(event_group, DOWN_BIT);
	    break;
	  case 'd':
	    xEventGroupSetBits(event_group, RIGHT_BIT);
	    break;
	  case 'w':
	    xEventGroupSetBits(event_group, UP_BIT);
	    break;
	  }



	}



}

