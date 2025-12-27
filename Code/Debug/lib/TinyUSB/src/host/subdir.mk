################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../lib/TinyUSB/src/host/hub.c \
../lib/TinyUSB/src/host/usbh.c 

C_DEPS += \
./lib/TinyUSB/src/host/hub.d \
./lib/TinyUSB/src/host/usbh.d 

OBJS += \
./lib/TinyUSB/src/host/hub.o \
./lib/TinyUSB/src/host/usbh.o 


# Each subdirectory must supply rules for building sources it contributes
lib/TinyUSB/src/host/%.o lib/TinyUSB/src/host/%.su lib/TinyUSB/src/host/%.cyclo: ../lib/TinyUSB/src/host/%.c lib/TinyUSB/src/host/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../lib/TinyUSB/hw -I../lib/TinyUSB/src -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-lib-2f-TinyUSB-2f-src-2f-host

clean-lib-2f-TinyUSB-2f-src-2f-host:
	-$(RM) ./lib/TinyUSB/src/host/hub.cyclo ./lib/TinyUSB/src/host/hub.d ./lib/TinyUSB/src/host/hub.o ./lib/TinyUSB/src/host/hub.su ./lib/TinyUSB/src/host/usbh.cyclo ./lib/TinyUSB/src/host/usbh.d ./lib/TinyUSB/src/host/usbh.o ./lib/TinyUSB/src/host/usbh.su

.PHONY: clean-lib-2f-TinyUSB-2f-src-2f-host

