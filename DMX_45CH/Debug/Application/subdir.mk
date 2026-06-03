################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Application/app_CMD.c \
../Application/app_DMXCore.c \
../Application/app_Sequence.c \
../Application/app_WS2811.c 

OBJS += \
./Application/app_CMD.o \
./Application/app_DMXCore.o \
./Application/app_Sequence.o \
./Application/app_WS2811.o 

C_DEPS += \
./Application/app_CMD.d \
./Application/app_DMXCore.d \
./Application/app_Sequence.d \
./Application/app_WS2811.d 


# Each subdirectory must supply rules for building sources it contributes
Application/%.o Application/%.su Application/%.cyclo: ../Application/%.c Application/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F302xC -c -I../Core/Inc -I../Drivers/STM32F3xx_HAL_Driver/Inc -I../Drivers/STM32F3xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F3xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/OmarSevilla/OneDrive/Documentos/Gitkraken/DMX_45CH/DMX_45CH/Application" -I"C:/Users/OmarSevilla/OneDrive/Documentos/Gitkraken/DMX_45CH/DMX_45CH/Libraries" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Application

clean-Application:
	-$(RM) ./Application/app_CMD.cyclo ./Application/app_CMD.d ./Application/app_CMD.o ./Application/app_CMD.su ./Application/app_DMXCore.cyclo ./Application/app_DMXCore.d ./Application/app_DMXCore.o ./Application/app_DMXCore.su ./Application/app_Sequence.cyclo ./Application/app_Sequence.d ./Application/app_Sequence.o ./Application/app_Sequence.su ./Application/app_WS2811.cyclo ./Application/app_WS2811.d ./Application/app_WS2811.o ./Application/app_WS2811.su

.PHONY: clean-Application

