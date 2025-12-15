################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Core/Src/Subsystems/Navigation.cpp 

OBJS += \
./Core/Src/Subsystems/Navigation.o 

CPP_DEPS += \
./Core/Src/Subsystems/Navigation.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/Subsystems/%.o Core/Src/Subsystems/%.su Core/Src/Subsystems/%.cyclo: ../Core/Src/Subsystems/%.cpp Core/Src/Subsystems/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++14 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../lib/TinyUSB/src -I../lib/TinyUSB/hw -I"/home/colin/Downloads/Flight-Computer/Code/lib/Eigen" -I../lib/Eigen -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-Subsystems

clean-Core-2f-Src-2f-Subsystems:
	-$(RM) ./Core/Src/Subsystems/Navigation.cyclo ./Core/Src/Subsystems/Navigation.d ./Core/Src/Subsystems/Navigation.o ./Core/Src/Subsystems/Navigation.su

.PHONY: clean-Core-2f-Src-2f-Subsystems

