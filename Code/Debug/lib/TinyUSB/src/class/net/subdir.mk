################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../lib/TinyUSB/src/class/net/ecm_rndis_device.c \
../lib/TinyUSB/src/class/net/ncm_device.c 

C_DEPS += \
./lib/TinyUSB/src/class/net/ecm_rndis_device.d \
./lib/TinyUSB/src/class/net/ncm_device.d 

OBJS += \
./lib/TinyUSB/src/class/net/ecm_rndis_device.o \
./lib/TinyUSB/src/class/net/ncm_device.o 


# Each subdirectory must supply rules for building sources it contributes
lib/TinyUSB/src/class/net/%.o lib/TinyUSB/src/class/net/%.su lib/TinyUSB/src/class/net/%.cyclo: ../lib/TinyUSB/src/class/net/%.c lib/TinyUSB/src/class/net/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../lib/TinyUSB/hw -I../lib/TinyUSB/src -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-lib-2f-TinyUSB-2f-src-2f-class-2f-net

clean-lib-2f-TinyUSB-2f-src-2f-class-2f-net:
	-$(RM) ./lib/TinyUSB/src/class/net/ecm_rndis_device.cyclo ./lib/TinyUSB/src/class/net/ecm_rndis_device.d ./lib/TinyUSB/src/class/net/ecm_rndis_device.o ./lib/TinyUSB/src/class/net/ecm_rndis_device.su ./lib/TinyUSB/src/class/net/ncm_device.cyclo ./lib/TinyUSB/src/class/net/ncm_device.d ./lib/TinyUSB/src/class/net/ncm_device.o ./lib/TinyUSB/src/class/net/ncm_device.su

.PHONY: clean-lib-2f-TinyUSB-2f-src-2f-class-2f-net

