#include "FreeRTOS.h"

/* Freescale includes. */
#include "fsl_device_registers.h"
#include "fsl_uart.h"
#include "clock_config.h"
#include "board.h"

#include "Motor_Control_Component.h"
#include "RC_Receiver_Component.h"
#include "Terminal_Component.h"
#include "LED_Component.h"
#include "Accelerometer_Component.h"

int main(void)
{
    /* Init board hardware. */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    setupMotorComponent();
    setupLEDComponent();
    // Data input components
    setupRCReceiverComponent();
    setupTerminalComponent();
    setupAccelerometerComponent();
    vTaskStartScheduler();

    while (1)
    {
    }
}
