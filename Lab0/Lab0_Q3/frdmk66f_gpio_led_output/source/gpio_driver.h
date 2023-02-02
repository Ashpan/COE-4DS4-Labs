#include <stdint.h>

#define INPUT_PIN   (0u)
#define OUTPUT_PIN  (1u)
#define PIN_LOW     (0u)
#define PIN_HIGH    (1u)

#define GPIO_A ((TsGPIO_Struct*)0x400FF000)
#define GPIO_B ((TsGPIO_Struct*)0x400FF040)
#define GPIO_C ((TsGPIO_Struct*)0x400FF080)
#define GPIO_D ((TsGPIO_Struct*)0x400FF0C0)
#define GPIO_E ((TsGPIO_Struct*)0x400FF100)

typedef enum {
	INPUT = 0,
	OUTPUT
} TePinDirection;

typedef enum {
	LOW = 0,
	HIGH
} TePinLevel;

typedef enum{
	UNKNOWN = 0,
	SUCCESS,
	FAIL,
	INVALID_INPUT
} TeGPIO_RetVal;

typedef struct {
	uint32_t PDOR; // PDOR -> Port data output register (offset 0x0)
	uint32_t PSOR; // PSOR -> Port set output register (offset 0x4)
	uint32_t PCOR; // PCOR -> Port clear output register (offset 0x8)
	uint32_t PTOR; // PTOR -> Port toggle output register (offset 0xC)
	uint32_t PDIR; // PDIR -> Port data input register (offset 0x10)
	uint32_t PDDR; // PDDR -> Port data direction register (offset 0x14)
} TsGPIO_Struct;

TeGPIO_RetVal GPIO_Lab0_InitPin(uint32_t pinNum, TsGPIO_Struct *gpioConfig, TePinDirection pinDir);
TeGPIO_RetVal GPIO_Lab0_PinWrite(TsGPIO_Struct *gpioConfig, uint32_t pinNum, TePinLevel level);
TeGPIO_RetVal GPIO_Lab0_PinToggle(TsGPIO_Struct *gpioConfig, uint32_t pinNum);




