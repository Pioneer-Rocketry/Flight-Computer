################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Core/Src/Devices/GPS.cpp \
../Core/Src/Devices/I2CDevice.cpp \
../Core/Src/Devices/SPIDevice.cpp 

OBJS += \
./Core/Src/Devices/GPS.o \
./Core/Src/Devices/I2CDevice.o \
./Core/Src/Devices/SPIDevice.o 

CPP_DEPS += \
./Core/Src/Devices/GPS.d \
./Core/Src/Devices/I2CDevice.d \
./Core/Src/Devices/SPIDevice.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/Devices/%.o Core/Src/Devices/%.su Core/Src/Devices/%.cyclo: ../Core/Src/Devices/%.cpp Core/Src/Devices/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++14 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I"/home/colin/rocketry/Flight-Computer/Code/lib/Eigen" -I../lib/Eigen -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-Devices

clean-Core-2f-Src-2f-Devices:
	-$(RM) ./Core/Src/Devices/GPS.cyclo ./Core/Src/Devices/GPS.d ./Core/Src/Devices/GPS.o ./Core/Src/Devices/GPS.su ./Core/Src/Devices/I2CDevice.cyclo ./Core/Src/Devices/I2CDevice.d ./Core/Src/Devices/I2CDevice.o ./Core/Src/Devices/I2CDevice.su ./Core/Src/Devices/SPIDevice.cyclo ./Core/Src/Devices/SPIDevice.d ./Core/Src/Devices/SPIDevice.o ./Core/Src/Devices/SPIDevice.su

.PHONY: clean-Core-2f-Src-2f-Devices

