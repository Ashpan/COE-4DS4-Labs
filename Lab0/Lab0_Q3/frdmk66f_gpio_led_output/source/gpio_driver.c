#include <stdint.h>
#include <stdio.h>
#include "gpio_driver.h"

TeGPIO_RetVal GPIO_Lab0_InitPin(uint32_t pinNum, TsGPIO_Struct *gpioConfig, TePinDirection pinDir) {
	TeGPIO_RetVal retVal = SUCCESS;

	if  ((pinNum < 0) || (gpioConfig == NULL)){
		return INVALID_INPUT;
	}

    if (pinDir == INPUT_PIN) {
    	// Reset pin (to configure as INPUT) based on register and pin number
        gpioConfig->PDDR &= ~(1UL << pinNum);
    } else {
        if ((retVal = (GPIO_Lab0_PinWrite(gpioConfig, pinNum, PIN_LOW))) != SUCCESS) {
        	printf("Failed PinWrite during initPin\n");
        	return retVal;
        }
    	gpioConfig->PDDR |= (1UL << pinNum);
    }

    return SUCCESS;
}

TeGPIO_RetVal GPIO_Lab0_PinWrite(TsGPIO_Struct *gpioConfig, uint32_t pinNum, TePinLevel level) {
	if ((gpioConfig == NULL) || (pinNum < 0)) {
		return INVALID_INPUT;
	}

    if (level == PIN_LOW) {
        gpioConfig->PCOR = (1UL << pinNum);
    } else {
        gpioConfig->PSOR = (1UL << pinNum);
    }

    return SUCCESS;
}

TeGPIO_RetVal GPIO_Lab0_PinToggle(TsGPIO_Struct *gpioConfig, uint32_t pinNum) {
	TeGPIO_RetVal retVal = SUCCESS;

    TePinLevel currPinLevel = (((uint32_t)(gpioConfig->PDIR) >> pinNum) & 0x01UL);

    if (currPinLevel == PIN_LOW) {
        if ((retVal = (GPIO_Lab0_PinWrite(gpioConfig, pinNum, PIN_HIGH))) != SUCCESS) {
        	return retVal;
        }
    } else {
        if ((retVal = (GPIO_Lab0_PinWrite(gpioConfig, pinNum, PIN_LOW))) != SUCCESS) {
        	return retVal;
        }
    }

    return SUCCESS;
}


