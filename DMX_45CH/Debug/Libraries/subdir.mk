################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Libraries/lib_Flash.c 

OBJS += \
./Libraries/lib_Flash.o 

C_DEPS += \
./Libraries/lib_Flash.d 


# Each subdirectory must supply rules for building sources it contributes
Libraries/%.o Libraries/%.su Libraries/%.cyclo: ../Libraries/%.c Libraries/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F302xC -c -I../Core/Inc -I../Drivers/STM32F3xx_HAL_Driver/Inc -I../Drivers/STM32F3xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F3xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/OmarSevilla/OneDrive/Documentos/Gitkraken/DMX_45CH/DMX_45CH/Application" -I"C:/Users/OmarSevilla/OneDrive/Documentos/Gitkraken/DMX_45CH/DMX_45CH/Libraries" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Libraries

clean-Libraries:
	-$(RM) ./Libraries/lib_Flash.cyclo ./Libraries/lib_Flash.d ./Libraries/lib_Flash.o ./Libraries/lib_Flash.su

.PHONY: clean-Libraries

