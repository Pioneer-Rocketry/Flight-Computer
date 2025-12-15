################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../lib/TinyUSB/src/portable/mentor/musb/dcd_musb.c \
../lib/TinyUSB/src/portable/mentor/musb/hcd_musb.c 

C_DEPS += \
./lib/TinyUSB/src/portable/mentor/musb/dcd_musb.d \
./lib/TinyUSB/src/portable/mentor/musb/hcd_musb.d 

OBJS += \
./lib/TinyUSB/src/portable/mentor/musb/dcd_musb.o \
./lib/TinyUSB/src/portable/mentor/musb/hcd_musb.o 


# Each subdirectory must supply rules for building sources it contributes
lib/TinyUSB/src/portable/mentor/musb/%.o lib/TinyUSB/src/portable/mentor/musb/%.su lib/TinyUSB/src/portable/mentor/musb/%.cyclo: ../lib/TinyUSB/src/portable/mentor/musb/%.c lib/TinyUSB/src/portable/mentor/musb/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../lib/TinyUSB/hw -I../lib/TinyUSB/src -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-lib-2f-TinyUSB-2f-src-2f-portable-2f-mentor-2f-musb

clean-lib-2f-TinyUSB-2f-src-2f-portable-2f-mentor-2f-musb:
	-$(RM) ./lib/TinyUSB/src/portable/mentor/musb/dcd_musb.cyclo ./lib/TinyUSB/src/portable/mentor/musb/dcd_musb.d ./lib/TinyUSB/src/portable/mentor/musb/dcd_musb.o ./lib/TinyUSB/src/portable/mentor/musb/dcd_musb.su ./lib/TinyUSB/src/portable/mentor/musb/hcd_musb.cyclo ./lib/TinyUSB/src/portable/mentor/musb/hcd_musb.d ./lib/TinyUSB/src/portable/mentor/musb/hcd_musb.o ./lib/TinyUSB/src/portable/mentor/musb/hcd_musb.su

.PHONY: clean-lib-2f-TinyUSB-2f-src-2f-portable-2f-mentor-2f-musb

