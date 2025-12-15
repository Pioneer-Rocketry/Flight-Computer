################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../lib/TinyUSB/src/portable/microchip/pic/dcd_pic.c 

C_DEPS += \
./lib/TinyUSB/src/portable/microchip/pic/dcd_pic.d 

OBJS += \
./lib/TinyUSB/src/portable/microchip/pic/dcd_pic.o 


# Each subdirectory must supply rules for building sources it contributes
lib/TinyUSB/src/portable/microchip/pic/%.o lib/TinyUSB/src/portable/microchip/pic/%.su lib/TinyUSB/src/portable/microchip/pic/%.cyclo: ../lib/TinyUSB/src/portable/microchip/pic/%.c lib/TinyUSB/src/portable/microchip/pic/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../lib/TinyUSB/hw -I../lib/TinyUSB/src -I../lib/Eigen -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-lib-2f-TinyUSB-2f-src-2f-portable-2f-microchip-2f-pic

clean-lib-2f-TinyUSB-2f-src-2f-portable-2f-microchip-2f-pic:
	-$(RM) ./lib/TinyUSB/src/portable/microchip/pic/dcd_pic.cyclo ./lib/TinyUSB/src/portable/microchip/pic/dcd_pic.d ./lib/TinyUSB/src/portable/microchip/pic/dcd_pic.o ./lib/TinyUSB/src/portable/microchip/pic/dcd_pic.su

.PHONY: clean-lib-2f-TinyUSB-2f-src-2f-portable-2f-microchip-2f-pic

