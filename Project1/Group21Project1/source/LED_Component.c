#include "LED_Component.h"

QueueHandle_t led_queue;

// defines the RGB LEDs to use
#define BOARD_RED_LED_GPIO_PIN   (1u)
#define BOARD_BLUE_LED_GPIO_PIN  (8u)
#define BOARD_GREEN_LED_GPIO_PIN (9u)

#define TRUE (1u)
#define FALSE (0u)
#define MODULE_NAME "LED Component"

uint8_t inputFlag = FALSE;	

#define RED (0xff0000)
#define YELLOW (0x0000FF)
#define GREEN (0x00ff00)

void setupLEDComponent()
{
	printf("\n"); // Remove after fixing printing format
	// setupLEDBootClocks();
	setupLEDPins();
	setupLEDs();


    /*************** LED Task ***************/
	// Create LED Queue
	QueueHandle_t led_queue = xQueueCreate(1, sizeof(char)*50);
	
	// Error checking for LED Queue
	if (led_queue == NULL) {
		PRINTF("Queue creation failed!.\r\n");
		while(1);
	}


	//Create LED Task
	BaseType_t status;

	// Initial Testing 
	unsigned long ulVar = 10UL;

	// status = xQueueSendToBack(led_queue, (void *)&ulVar, portMAX_DELAY);
	status = xTaskCreate(receiveLedInput, "receiveLedInput", 200, (void *)led_queue, 2, NULL);

	// Error checking for creating LED Task
	if (status != pdPASS) {
		PRINTF("%s: Task creation failed!.\r\n", MODULE_NAME);
		while(1);
	}

	status = xTaskCreate(ledTask, "ledTask", 200, (void *)led_queue, 3, NULL);

	// Error checking for creating LED Task
	if (status != pdPASS) {
		PRINTF("%s: Task creation failed!.\r\n", MODULE_NAME);
		while(1);
	}
	
}

// void setupLEDBootClocks()
// {
// 	CLOCK_SetSimSafeDivs();
// 	CLOCK_SetInternalRefClkConfig(kMCG_IrclkEnable, kMCG_IrcFast, 2);
// 	CLOCK_CONFIG_SetFllExtRefDiv(0);
// 	CLOCK_SetExternalRefClkConfig(kMCG_OscselIrc);
// 	CLOCK_SetSimConfig(&simConfig_BOARD_BootClockRUN);
// 	SystemCoreClock = BOARD_BOOTCLOCKRUN_CORE_CLOCK;
// }

void setupLEDPins()
{
	//Configure LED pins
    // BOARD_InitPins();

    /* Port A Clock Gate Control: Clock enabled */
       CLOCK_EnableClock(kCLOCK_PortA);
       /* Port B Clock Gate Control: Clock enabled */
       CLOCK_EnableClock(kCLOCK_PortB);
       /* Port C Clock Gate Control: Clock enabled */
       CLOCK_EnableClock(kCLOCK_PortC);
       /* Port D Clock Gate Control: Clock enabled */
       CLOCK_EnableClock(kCLOCK_PortD);

    //    /* PORTA2 (pin K6) is configured as TRACE_SWO */
    //    PORT_SetPinMux(BOARD_TRACE_SWO_PORT, BOARD_TRACE_SWO_PIN, kPORT_MuxAlt7);

    //    PORTA->PCR[2] = ((PORTA->PCR[2] &
    //                      /* Mask bits to zero which are setting */
    //                      (~(PORT_PCR_PS_MASK | PORT_PCR_PE_MASK | PORT_PCR_ISF_MASK)))

    //                     /* Pull Select: Internal pulldown resistor is enabled on the corresponding pin, if the
    //                      * corresponding PE field is set. */
    //                     | PORT_PCR_PS(kPORT_PullDown)

    //                     /* Pull Enable: Internal pullup or pulldown resistor is not enabled on the corresponding pin. */
    //                     | PORT_PCR_PE(kPORT_PullDisable));

    //    /* PORTB16 (pin E10) is configured as UART0_RX */
    //    PORT_SetPinMux(BOARD_DEBUG_UART_RX_PORT, BOARD_DEBUG_UART_RX_PIN, kPORT_MuxAlt3);

    //    /* PORTB17 (pin E9) is configured as UART0_TX */
    //    PORT_SetPinMux(BOARD_DEBUG_UART_TX_PORT, BOARD_DEBUG_UART_TX_PIN, kPORT_MuxAlt3);

    //    SIM->SOPT5 = ((SIM->SOPT5 &
    //                   /* Mask bits to zero which are setting */
    //                   (~(SIM_SOPT5_UART0TXSRC_MASK)))

    //                  /* UART 0 transmit data source select: UART0_TX pin. */
    //                  | SIM_SOPT5_UART0TXSRC(SOPT5_UART0TXSRC_UART_TX));


	// PWM RGB Setup
    PORT_SetPinMux(PORTD, 1u, kPORT_MuxAlt4);
    PORT_SetPinMux(PORTC, 8u, kPORT_MuxAlt3);
    PORT_SetPinMux(PORTC, 9u, kPORT_MuxAlt3);

    printf("%s: Successfully set init boot pins!\r\n", MODULE_NAME);
}

void setupLEDs()
{
	//Initialize PWM for the LEDs
	// Copied from https://github.com/Ashpan/COE-4DS4-Labs/blob/main/Lab0/Lab0_Q5/lab0_q5_hello_world/source/hello_world.c

	ftm_config_t ftmInfo;
	ftm_chnl_pwm_signal_param_t ftmParamRed;
	ftm_chnl_pwm_signal_param_t ftmParamGreen;
	ftm_chnl_pwm_signal_param_t ftmParamBlue;

	// RED LED FTM
	ftmParamRed.chnlNumber = kFTM_Chnl_1;
	ftmParamRed.level = kFTM_HighTrue;
	ftmParamRed.dutyCyclePercent = 0;
	ftmParamRed.firstEdgeDelayPercent = 0U;
	ftmParamRed.enableComplementary = false;
	ftmParamRed.enableDeadtime = false;

	// BLUE LED FTM
	ftmParamGreen.chnlNumber = kFTM_Chnl_5;
	ftmParamGreen.level = kFTM_HighTrue;
	ftmParamGreen.dutyCyclePercent = 0;
	ftmParamGreen.firstEdgeDelayPercent = 0U;
	ftmParamGreen.enableComplementary = false;
	ftmParamGreen.enableDeadtime = false;

	// GREEN LED
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

void receiveLedInput(void *pvParameters) {
	char userInput[50] = "";
	QueueHandle_t queue1 = (QueueHandle_t)pvParameters;
	BaseType_t status;

	printf("Entered user input: ");
	scanf("%s", &userInput);

	status = xQueueSendToBack(queue1, (void *)userInput, portMAX_DELAY);
	if (status != pdPASS)
	{
		PRINTF("Queue Send failed!.\r\n");
		while(1);
	}

	inputFlag = TRUE;

	vTaskDelete(NULL);
}

void ledTask(void *pvParameters) {
	unsigned long color; 
	char receivedInput[50] = "";
	QueueHandle_t queue1 = (QueueHandle_t)pvParameters;
	BaseType_t status;

	status = xQueueReceive(queue1, (void *)&receivedInput, portMAX_DELAY);
	if (status != pdPASS) {
		PRINTF("Queue Receive failed!.\r\n");
		while(1);
	}

	while (1) {
		if (inputFlag) {
				if (strcmp("1", receivedInput) == 0){ color = GREEN; }
				if (strcmp("2", receivedInput) == 0){ color = YELLOW; }
				if (strcmp("3", receivedInput) == 0){ color = RED; }

				// unsigned long hex_code = 0x00ff00;
				FTM_UpdatePwmDutycycle(FTM_LED, FTM_RED_CHANNEL, kFTM_EdgeAlignedPwm, (((color >> 16) & (0xFF))/255)*100);
				FTM_UpdatePwmDutycycle(FTM_LED, FTM_GREEN_CHANNEL, kFTM_EdgeAlignedPwm, (((color >> 8) & (0xFF))/255)*100);
				FTM_UpdatePwmDutycycle(FTM_LED, FTM_BLUE_CHANNEL, kFTM_EdgeAlignedPwm, (((color) & (0xFF))/255)*100);
				FTM_SetSoftwareTrigger(FTM_LED, true);

			// PRINTF("%s\r\n", receivedInput);
			// vTaskDelay(1000 / portTICK_PERIOD_MS);
		}

		// Force scheduler to move to other tasks if inputFlag != TRUE
		vTaskDelay(10 / portTICK_PERIOD_MS);
	}

	vTaskDelete(NULL);
}




//void receiveLedInput(void* pvParameters){
//	char userInput[50] = "";
//	// int input = 3;
//	QueueHandle_t led_queue = (QueueHandle_t)pvParameters;
//	BaseType_t status;
//
//	printf("Entered user input: ");
//	scanf("%s", userInput);
//
//	status = xQueueSendToBack(led_queue, (void *)userInput, portMAX_DELAY);
//	if (status != pdPASS)
//	{
//		PRINTF("Queue Send failed!.\r\n");
//		while(1);
//	}
//
//	inputFlag = 1;
//
//	vTaskDelete(NULL);
//}
//
//void ledTask(void* pvParameters)
//{
//	printf("%s: Initialize LED Task\r\n", MODULE_NAME);
//
//	char receivedInput[50] = "";
//	int led_input;
//
//	//LED task implementation
//	QueueHandle_t led_queue = (QueueHandle_t)pvParameters;
//	BaseType_t status;
//
//	// Receive input from RC Controller
//	status = xQueueReceive(led_queue, (void * )&receivedInput, portMAX_DELAY);
//
//	// Error catching for queue reeiving failed
//	if (status != pdPASS) {
//		PRINTF("%s: Queue Receive failed!.\r\n", MODULE_NAME);
//		while(1);
//	}
//
//	while (TRUE){
//		if (inputFlag) {
//			PRINTF("%d\r\n", receivedInput);
//			vTaskDelay(1000 / portTICK_PERIOD_MS);
//		}
//
//		// Force scheduler to move to other tasks if inputFlag != TRUE
//		vTaskDelay(10 / portTICK_PERIOD_MS);
//	}
//
//	vTaskDelete(NULL);
//	// printf("%d\n", *led_input);
//}
