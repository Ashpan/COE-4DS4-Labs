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

typedef struct
{
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

int main(void)
{
    uart_config_t config;
    RC_Values rc_values;
    uint8_t *ptr = (uint8_t *)&rc_values;
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
        if (*ptr != 0x20)
            continue;
        UART_ReadBlocking(UART1, &ptr[1], sizeof(rc_values) - 1);
        if (rc_values.header == 0x4020)
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

    //    /* Init board hardware. */
    //    BOARD_InitBootClocks();
    //    setupMotorComponent();
    //    printf("BOOT CLOCKS INIT");
    ////    setupRCReceiverComponent();
    ////    setupTerminalComponent();
    ////    setupLEDComponent();
    ////    setupAccelerometerComponent();
    //
    //    vTaskStartScheduler();
    //
    //    while(1)
    //    {}
}
