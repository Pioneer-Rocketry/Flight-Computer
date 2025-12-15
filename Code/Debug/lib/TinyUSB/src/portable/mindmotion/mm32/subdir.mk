################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../lib/TinyUSB/src/portable/mindmotion/mm32/dcd_mm32f327x_otg.c 

C_DEPS += \
./lib/TinyUSB/src/portable/mindmotion/mm32/dcd_mm32f327x_otg.d 

OBJS += \
./lib/TinyUSB/src/portable/mindmotion/mm32/dcd_mm32f327x_otg.o 


# Each subdirectory must supply rules for building sources it contributes
lib/TinyUSB/src/portable/mindmotion/mm32/%.o lib/TinyUSB/src/portable/mindmotion/mm32/%.su lib/TinyUSB/src/portable/mindmotion/mm32/%.cyclo: ../lib/TinyUSB/src/portable/mindmotion/mm32/%.c lib/TinyUSB/src/portable/mindmotion/mm32/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../lib/TinyUSB/hw -I../lib/TinyUSB/src -I../lib/Eigen -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-lib-2f-TinyUSB-2f-src-2f-portable-2f-mindmotion-2f-mm32

clean-lib-2f-TinyUSB-2f-src-2f-portable-2f-mindmotion-2f-mm32:
	-$(RM) ./lib/TinyUSB/src/portable/mindmotion/mm32/dcd_mm32f327x_otg.cyclo ./lib/TinyUSB/src/portable/mindmotion/mm32/dcd_mm32f327x_otg.d ./lib/TinyUSB/src/portable/mindmotion/mm32/dcd_mm32f327x_otg.o ./lib/TinyUSB/src/portable/mindmotion/mm32/dcd_mm32f327x_otg.su

.PHONY: clean-lib-2f-TinyUSB-2f-src-2f-portable-2f-mindmotion-2f-mm32

