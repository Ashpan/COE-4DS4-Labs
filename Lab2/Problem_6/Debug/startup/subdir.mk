################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../startup/startup_mk66f18.c 

C_DEPS += \
./startup/startup_mk66f18.d 

OBJS += \
./startup/startup_mk66f18.o 


# Each subdirectory must supply rules for building sources it contributes
startup/%.o: ../startup/%.c startup/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -std=gnu99 -D__REDLIB__ -DCPU_MK66FN2M0VMD18 -DCPU_MK66FN2M0VMD18_cm4 -DFRDM_K66F -DFREEDOM -DSERIAL_PORT_TYPE_UART=1 -DSDK_OS_FREE_RTOS -DMCUXPRESSO_SDK -DSDK_DEBUGCONSOLE=0 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -I"C:\Users\raskara\Documents\COE-4DS4-Labs\Lab2\Problem_6\source" -I"C:\Users\raskara\Documents\COE-4DS4-Labs\Lab2\Problem_6\drivers" -I"C:\Users\raskara\Documents\COE-4DS4-Labs\Lab2\Problem_6\device" -I"C:\Users\raskara\Documents\COE-4DS4-Labs\Lab2\Problem_6\utilities" -I"C:\Users\raskara\Documents\COE-4DS4-Labs\Lab2\Problem_6\component\uart" -I"C:\Users\raskara\Documents\COE-4DS4-Labs\Lab2\Problem_6\component\serial_manager" -I"C:\Users\raskara\Documents\COE-4DS4-Labs\Lab2\Problem_6\component\lists" -I"C:\Users\raskara\Documents\COE-4DS4-Labs\Lab2\Problem_6\CMSIS" -I"C:\Users\raskara\Documents\COE-4DS4-Labs\Lab2\Problem_6\freertos\freertos_kernel\include" -I"C:\Users\raskara\Documents\COE-4DS4-Labs\Lab2\Problem_6\freertos\freertos_kernel\portable\GCC\ARM_CM4F" -I"C:\Users\raskara\Documents\COE-4DS4-Labs\Lab2\Problem_6\board" -O0 -fno-common -g3 -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-startup

clean-startup:
	-$(RM) ./startup/startup_mk66f18.d ./startup/startup_mk66f18.o

.PHONY: clean-startup

