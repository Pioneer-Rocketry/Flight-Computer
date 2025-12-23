################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../lib/TinyUSB/src/portable/synopsys/dwc2/dcd_dwc2.c \
../lib/TinyUSB/src/portable/synopsys/dwc2/dwc2_common.c \
../lib/TinyUSB/src/portable/synopsys/dwc2/hcd_dwc2.c 

C_DEPS += \
./lib/TinyUSB/src/portable/synopsys/dwc2/dcd_dwc2.d \
./lib/TinyUSB/src/portable/synopsys/dwc2/dwc2_common.d \
./lib/TinyUSB/src/portable/synopsys/dwc2/hcd_dwc2.d 

OBJS += \
./lib/TinyUSB/src/portable/synopsys/dwc2/dcd_dwc2.o \
./lib/TinyUSB/src/portable/synopsys/dwc2/dwc2_common.o \
./lib/TinyUSB/src/portable/synopsys/dwc2/hcd_dwc2.o 


# Each subdirectory must supply rules for building sources it contributes
lib/TinyUSB/src/portable/synopsys/dwc2/%.o lib/TinyUSB/src/portable/synopsys/dwc2/%.su lib/TinyUSB/src/portable/synopsys/dwc2/%.cyclo: ../lib/TinyUSB/src/portable/synopsys/dwc2/%.c lib/TinyUSB/src/portable/synopsys/dwc2/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../lib/TinyUSB/hw -I../lib/TinyUSB/src -I../lib/Eigen -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-lib-2f-TinyUSB-2f-src-2f-portable-2f-synopsys-2f-dwc2

clean-lib-2f-TinyUSB-2f-src-2f-portable-2f-synopsys-2f-dwc2:
	-$(RM) ./lib/TinyUSB/src/portable/synopsys/dwc2/dcd_dwc2.cyclo ./lib/TinyUSB/src/portable/synopsys/dwc2/dcd_dwc2.d ./lib/TinyUSB/src/portable/synopsys/dwc2/dcd_dwc2.o ./lib/TinyUSB/src/portable/synopsys/dwc2/dcd_dwc2.su ./lib/TinyUSB/src/portable/synopsys/dwc2/dwc2_common.cyclo ./lib/TinyUSB/src/portable/synopsys/dwc2/dwc2_common.d ./lib/TinyUSB/src/portable/synopsys/dwc2/dwc2_common.o ./lib/TinyUSB/src/portable/synopsys/dwc2/dwc2_common.su ./lib/TinyUSB/src/portable/synopsys/dwc2/hcd_dwc2.cyclo ./lib/TinyUSB/src/portable/synopsys/dwc2/hcd_dwc2.d ./lib/TinyUSB/src/portable/synopsys/dwc2/hcd_dwc2.o ./lib/TinyUSB/src/portable/synopsys/dwc2/hcd_dwc2.su

.PHONY: clean-lib-2f-TinyUSB-2f-src-2f-portable-2f-synopsys-2f-dwc2

