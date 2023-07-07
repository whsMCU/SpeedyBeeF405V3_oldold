################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../SpeedyBeeF405V3_CUBE/Core/Src/adc.c \
../SpeedyBeeF405V3_CUBE/Core/Src/dma.c \
../SpeedyBeeF405V3_CUBE/Core/Src/gpio.c \
../SpeedyBeeF405V3_CUBE/Core/Src/i2c.c \
../SpeedyBeeF405V3_CUBE/Core/Src/main.c \
../SpeedyBeeF405V3_CUBE/Core/Src/rtc.c \
../SpeedyBeeF405V3_CUBE/Core/Src/spi.c \
../SpeedyBeeF405V3_CUBE/Core/Src/stm32f4xx_hal_msp.c \
../SpeedyBeeF405V3_CUBE/Core/Src/stm32f4xx_it.c \
../SpeedyBeeF405V3_CUBE/Core/Src/syscalls.c \
../SpeedyBeeF405V3_CUBE/Core/Src/sysmem.c \
../SpeedyBeeF405V3_CUBE/Core/Src/system_stm32f4xx.c \
../SpeedyBeeF405V3_CUBE/Core/Src/tim.c \
../SpeedyBeeF405V3_CUBE/Core/Src/usart.c 

OBJS += \
./SpeedyBeeF405V3_CUBE/Core/Src/adc.o \
./SpeedyBeeF405V3_CUBE/Core/Src/dma.o \
./SpeedyBeeF405V3_CUBE/Core/Src/gpio.o \
./SpeedyBeeF405V3_CUBE/Core/Src/i2c.o \
./SpeedyBeeF405V3_CUBE/Core/Src/main.o \
./SpeedyBeeF405V3_CUBE/Core/Src/rtc.o \
./SpeedyBeeF405V3_CUBE/Core/Src/spi.o \
./SpeedyBeeF405V3_CUBE/Core/Src/stm32f4xx_hal_msp.o \
./SpeedyBeeF405V3_CUBE/Core/Src/stm32f4xx_it.o \
./SpeedyBeeF405V3_CUBE/Core/Src/syscalls.o \
./SpeedyBeeF405V3_CUBE/Core/Src/sysmem.o \
./SpeedyBeeF405V3_CUBE/Core/Src/system_stm32f4xx.o \
./SpeedyBeeF405V3_CUBE/Core/Src/tim.o \
./SpeedyBeeF405V3_CUBE/Core/Src/usart.o 

C_DEPS += \
./SpeedyBeeF405V3_CUBE/Core/Src/adc.d \
./SpeedyBeeF405V3_CUBE/Core/Src/dma.d \
./SpeedyBeeF405V3_CUBE/Core/Src/gpio.d \
./SpeedyBeeF405V3_CUBE/Core/Src/i2c.d \
./SpeedyBeeF405V3_CUBE/Core/Src/main.d \
./SpeedyBeeF405V3_CUBE/Core/Src/rtc.d \
./SpeedyBeeF405V3_CUBE/Core/Src/spi.d \
./SpeedyBeeF405V3_CUBE/Core/Src/stm32f4xx_hal_msp.d \
./SpeedyBeeF405V3_CUBE/Core/Src/stm32f4xx_it.d \
./SpeedyBeeF405V3_CUBE/Core/Src/syscalls.d \
./SpeedyBeeF405V3_CUBE/Core/Src/sysmem.d \
./SpeedyBeeF405V3_CUBE/Core/Src/system_stm32f4xx.d \
./SpeedyBeeF405V3_CUBE/Core/Src/tim.d \
./SpeedyBeeF405V3_CUBE/Core/Src/usart.d 


# Each subdirectory must supply rules for building sources it contributes
SpeedyBeeF405V3_CUBE/Core/Src/%.o SpeedyBeeF405V3_CUBE/Core/Src/%.su SpeedyBeeF405V3_CUBE/Core/Src/%.cyclo: ../SpeedyBeeF405V3_CUBE/Core/Src/%.c SpeedyBeeF405V3_CUBE/Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DSTM32F405xx -c -I"C:/Users/jjins/Documents/Project/SpeedyBeeF405V3/src" -I"C:/Users/jjins/Documents/Project/SpeedyBeeF405V3/src/ap" -I"C:/Users/jjins/Documents/Project/SpeedyBeeF405V3/src/bsp" -I"C:/Users/jjins/Documents/Project/SpeedyBeeF405V3/src/common" -I"C:/Users/jjins/Documents/Project/SpeedyBeeF405V3/src/common/core" -I"C:/Users/jjins/Documents/Project/SpeedyBeeF405V3/src/common/core/BMI270-Sensor-API" -I"C:/Users/jjins/Documents/Project/SpeedyBeeF405V3/src/common/core/config" -I"C:/Users/jjins/Documents/Project/SpeedyBeeF405V3/src/common/core/rx" -I"C:/Users/jjins/Documents/Project/SpeedyBeeF405V3/src/common/core/scheduler" -I"C:/Users/jjins/Documents/Project/SpeedyBeeF405V3/src/common/core/sensors" -I"C:/Users/jjins/Documents/Project/SpeedyBeeF405V3/src/common/hw/include" -I"C:/Users/jjins/Documents/Project/SpeedyBeeF405V3/src/hw" -I"C:/Users/jjins/Documents/Project/SpeedyBeeF405V3/src/hw/driver" -I"C:/Users/jjins/Documents/Project/SpeedyBeeF405V3/src/hw/driver/accgyro" -I"C:/Users/jjins/Documents/Project/SpeedyBeeF405V3/src/hw/driver/barometer" -I"C:/Users/jjins/Documents/Project/SpeedyBeeF405V3/src/hw/driver/compass" -I"C:/Users/jjins/Documents/Project/SpeedyBeeF405V3/src/hw/driver/gps" -I"C:/Users/jjins/Documents/Project/SpeedyBeeF405V3/src/hw/driver/sdcard" -I"C:/Users/jjins/Documents/Project/SpeedyBeeF405V3/src/lib/Drivers/CMSIS/Include" -I"C:/Users/jjins/Documents/Project/SpeedyBeeF405V3/src/lib/Drivers/CMSIS/Device/ST/STM32F4xx/Include" -I"C:/Users/jjins/Documents/Project/SpeedyBeeF405V3/src/lib/Drivers/STM32F4xx_HAL_Driver/Inc" -I"C:/Users/jjins/Documents/Project/SpeedyBeeF405V3/src/lib/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc" -I"C:/Users/jjins/Documents/Project/SpeedyBeeF405V3/src/lib/Middlewares/ST/STM32_USB_Device_Library/Core/Inc" -I"C:/Users/jjins/Documents/Project/SpeedyBeeF405V3/src/lib/Middlewares/Third_Party/FatFs/src" -I"C:/Users/jjins/Documents/Project/SpeedyBeeF405V3/src/lib/USB_DEVICE/App" -I"C:/Users/jjins/Documents/Project/SpeedyBeeF405V3/src/lib/USB_DEVICE/Target" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-SpeedyBeeF405V3_CUBE-2f-Core-2f-Src

clean-SpeedyBeeF405V3_CUBE-2f-Core-2f-Src:
	-$(RM) ./SpeedyBeeF405V3_CUBE/Core/Src/adc.cyclo ./SpeedyBeeF405V3_CUBE/Core/Src/adc.d ./SpeedyBeeF405V3_CUBE/Core/Src/adc.o ./SpeedyBeeF405V3_CUBE/Core/Src/adc.su ./SpeedyBeeF405V3_CUBE/Core/Src/dma.cyclo ./SpeedyBeeF405V3_CUBE/Core/Src/dma.d ./SpeedyBeeF405V3_CUBE/Core/Src/dma.o ./SpeedyBeeF405V3_CUBE/Core/Src/dma.su ./SpeedyBeeF405V3_CUBE/Core/Src/gpio.cyclo ./SpeedyBeeF405V3_CUBE/Core/Src/gpio.d ./SpeedyBeeF405V3_CUBE/Core/Src/gpio.o ./SpeedyBeeF405V3_CUBE/Core/Src/gpio.su ./SpeedyBeeF405V3_CUBE/Core/Src/i2c.cyclo ./SpeedyBeeF405V3_CUBE/Core/Src/i2c.d ./SpeedyBeeF405V3_CUBE/Core/Src/i2c.o ./SpeedyBeeF405V3_CUBE/Core/Src/i2c.su ./SpeedyBeeF405V3_CUBE/Core/Src/main.cyclo ./SpeedyBeeF405V3_CUBE/Core/Src/main.d ./SpeedyBeeF405V3_CUBE/Core/Src/main.o ./SpeedyBeeF405V3_CUBE/Core/Src/main.su ./SpeedyBeeF405V3_CUBE/Core/Src/rtc.cyclo ./SpeedyBeeF405V3_CUBE/Core/Src/rtc.d ./SpeedyBeeF405V3_CUBE/Core/Src/rtc.o ./SpeedyBeeF405V3_CUBE/Core/Src/rtc.su ./SpeedyBeeF405V3_CUBE/Core/Src/spi.cyclo ./SpeedyBeeF405V3_CUBE/Core/Src/spi.d ./SpeedyBeeF405V3_CUBE/Core/Src/spi.o ./SpeedyBeeF405V3_CUBE/Core/Src/spi.su ./SpeedyBeeF405V3_CUBE/Core/Src/stm32f4xx_hal_msp.cyclo ./SpeedyBeeF405V3_CUBE/Core/Src/stm32f4xx_hal_msp.d ./SpeedyBeeF405V3_CUBE/Core/Src/stm32f4xx_hal_msp.o ./SpeedyBeeF405V3_CUBE/Core/Src/stm32f4xx_hal_msp.su ./SpeedyBeeF405V3_CUBE/Core/Src/stm32f4xx_it.cyclo ./SpeedyBeeF405V3_CUBE/Core/Src/stm32f4xx_it.d ./SpeedyBeeF405V3_CUBE/Core/Src/stm32f4xx_it.o ./SpeedyBeeF405V3_CUBE/Core/Src/stm32f4xx_it.su ./SpeedyBeeF405V3_CUBE/Core/Src/syscalls.cyclo ./SpeedyBeeF405V3_CUBE/Core/Src/syscalls.d ./SpeedyBeeF405V3_CUBE/Core/Src/syscalls.o ./SpeedyBeeF405V3_CUBE/Core/Src/syscalls.su ./SpeedyBeeF405V3_CUBE/Core/Src/sysmem.cyclo ./SpeedyBeeF405V3_CUBE/Core/Src/sysmem.d ./SpeedyBeeF405V3_CUBE/Core/Src/sysmem.o ./SpeedyBeeF405V3_CUBE/Core/Src/sysmem.su ./SpeedyBeeF405V3_CUBE/Core/Src/system_stm32f4xx.cyclo ./SpeedyBeeF405V3_CUBE/Core/Src/system_stm32f4xx.d ./SpeedyBeeF405V3_CUBE/Core/Src/system_stm32f4xx.o ./SpeedyBeeF405V3_CUBE/Core/Src/system_stm32f4xx.su ./SpeedyBeeF405V3_CUBE/Core/Src/tim.cyclo ./SpeedyBeeF405V3_CUBE/Core/Src/tim.d ./SpeedyBeeF405V3_CUBE/Core/Src/tim.o ./SpeedyBeeF405V3_CUBE/Core/Src/tim.su ./SpeedyBeeF405V3_CUBE/Core/Src/usart.cyclo ./SpeedyBeeF405V3_CUBE/Core/Src/usart.d ./SpeedyBeeF405V3_CUBE/Core/Src/usart.o ./SpeedyBeeF405V3_CUBE/Core/Src/usart.su

.PHONY: clean-SpeedyBeeF405V3_CUBE-2f-Core-2f-Src

