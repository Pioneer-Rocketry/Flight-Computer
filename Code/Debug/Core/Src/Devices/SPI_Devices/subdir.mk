################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Core/Src/Devices/SPI_Devices/LSM6DSV320.cpp \
../Core/Src/Devices/SPI_Devices/MS560702BA03.cpp 

OBJS += \
./Core/Src/Devices/SPI_Devices/LSM6DSV320.o \
./Core/Src/Devices/SPI_Devices/MS560702BA03.o 

CPP_DEPS += \
./Core/Src/Devices/SPI_Devices/LSM6DSV320.d \
./Core/Src/Devices/SPI_Devices/MS560702BA03.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/Devices/SPI_Devices/%.o Core/Src/Devices/SPI_Devices/%.su Core/Src/Devices/SPI_Devices/%.cyclo: ../Core/Src/Devices/SPI_Devices/%.cpp Core/Src/Devices/SPI_Devices/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++14 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../lib/TinyUSB/src -I../lib/TinyUSB/hw -I"/home/colin/rocketry/Flight-Computer/Code/lib/Eigen" -I../lib/Eigen -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-Devices-2f-SPI_Devices

clean-Core-2f-Src-2f-Devices-2f-SPI_Devices:
	-$(RM) ./Core/Src/Devices/SPI_Devices/LSM6DSV320.cyclo ./Core/Src/Devices/SPI_Devices/LSM6DSV320.d ./Core/Src/Devices/SPI_Devices/LSM6DSV320.o ./Core/Src/Devices/SPI_Devices/LSM6DSV320.su ./Core/Src/Devices/SPI_Devices/MS560702BA03.cyclo ./Core/Src/Devices/SPI_Devices/MS560702BA03.d ./Core/Src/Devices/SPI_Devices/MS560702BA03.o ./Core/Src/Devices/SPI_Devices/MS560702BA03.su

.PHONY: clean-Core-2f-Src-2f-Devices-2f-SPI_Devices

