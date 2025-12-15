################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../lib/TinyUSB/src/device/usbd.c \
../lib/TinyUSB/src/device/usbd_control.c 

C_DEPS += \
./lib/TinyUSB/src/device/usbd.d \
./lib/TinyUSB/src/device/usbd_control.d 

OBJS += \
./lib/TinyUSB/src/device/usbd.o \
./lib/TinyUSB/src/device/usbd_control.o 


# Each subdirectory must supply rules for building sources it contributes
lib/TinyUSB/src/device/%.o lib/TinyUSB/src/device/%.su lib/TinyUSB/src/device/%.cyclo: ../lib/TinyUSB/src/device/%.c lib/TinyUSB/src/device/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../lib/TinyUSB/hw -I../lib/TinyUSB/src -I../lib/Eigen -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-lib-2f-TinyUSB-2f-src-2f-device

clean-lib-2f-TinyUSB-2f-src-2f-device:
	-$(RM) ./lib/TinyUSB/src/device/usbd.cyclo ./lib/TinyUSB/src/device/usbd.d ./lib/TinyUSB/src/device/usbd.o ./lib/TinyUSB/src/device/usbd.su ./lib/TinyUSB/src/device/usbd_control.cyclo ./lib/TinyUSB/src/device/usbd_control.d ./lib/TinyUSB/src/device/usbd_control.o ./lib/TinyUSB/src/device/usbd_control.su

.PHONY: clean-lib-2f-TinyUSB-2f-src-2f-device

