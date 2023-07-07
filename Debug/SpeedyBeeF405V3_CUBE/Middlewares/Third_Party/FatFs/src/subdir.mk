################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../SpeedyBeeF405V3_CUBE/Middlewares/Third_Party/FatFs/src/diskio.c \
../SpeedyBeeF405V3_CUBE/Middlewares/Third_Party/FatFs/src/ff.c \
../SpeedyBeeF405V3_CUBE/Middlewares/Third_Party/FatFs/src/ff_gen_drv.c 

OBJS += \
./SpeedyBeeF405V3_CUBE/Middlewares/Third_Party/FatFs/src/diskio.o \
./SpeedyBeeF405V3_CUBE/Middlewares/Third_Party/FatFs/src/ff.o \
./SpeedyBeeF405V3_CUBE/Middlewares/Third_Party/FatFs/src/ff_gen_drv.o 

C_DEPS += \
./SpeedyBeeF405V3_CUBE/Middlewares/Third_Party/FatFs/src/diskio.d \
./SpeedyBeeF405V3_CUBE/Middlewares/Third_Party/FatFs/src/ff.d \
./SpeedyBeeF405V3_CUBE/Middlewares/Third_Party/FatFs/src/ff_gen_drv.d 


# Each subdirectory must supply rules for building sources it contributes
SpeedyBeeF405V3_CUBE/Middlewares/Third_Party/FatFs/src/%.o SpeedyBeeF405V3_CUBE/Middlewares/Third_Party/FatFs/src/%.su SpeedyBeeF405V3_CUBE/Middlewares/Third_Party/FatFs/src/%.cyclo: ../SpeedyBeeF405V3_CUBE/Middlewares/Third_Party/FatFs/src/%.c SpeedyBeeF405V3_CUBE/Middlewares/Third_Party/FatFs/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DSTM32F405xx -c -I"C:/Users/jjins/Documents/Project/SpeedyBeeF405V3/src" -I"C:/Users/jjins/Documents/Project/SpeedyBeeF405V3/src/ap" -I"C:/Users/jjins/Documents/Project/SpeedyBeeF405V3/src/bsp" -I"C:/Users/jjins/Documents/Project/SpeedyBeeF405V3/src/common" -I"C:/Users/jjins/Documents/Project/SpeedyBeeF405V3/src/common/core" -I"C:/Users/jjins/Documents/Project/SpeedyBeeF405V3/src/common/core/BMI270-Sensor-API" -I"C:/Users/jjins/Documents/Project/SpeedyBeeF405V3/src/common/core/config" -I"C:/Users/jjins/Documents/Project/SpeedyBeeF405V3/src/common/core/rx" -I"C:/Users/jjins/Documents/Project/SpeedyBeeF405V3/src/common/core/scheduler" -I"C:/Users/jjins/Documents/Project/SpeedyBeeF405V3/src/common/core/sensors" -I"C:/Users/jjins/Documents/Project/SpeedyBeeF405V3/src/common/hw/include" -I"C:/Users/jjins/Documents/Project/SpeedyBeeF405V3/src/hw" -I"C:/Users/jjins/Documents/Project/SpeedyBeeF405V3/src/hw/driver" -I"C:/Users/jjins/Documents/Project/SpeedyBeeF405V3/src/hw/driver/accgyro" -I"C:/Users/jjins/Documents/Project/SpeedyBeeF405V3/src/hw/driver/barometer" -I"C:/Users/jjins/Documents/Project/SpeedyBeeF405V3/src/hw/driver/compass" -I"C:/Users/jjins/Documents/Project/SpeedyBeeF405V3/src/hw/driver/gps" -I"C:/Users/jjins/Documents/Project/SpeedyBeeF405V3/src/hw/driver/sdcard" -I"C:/Users/jjins/Documents/Project/SpeedyBeeF405V3/src/lib/Drivers/CMSIS/Include" -I"C:/Users/jjins/Documents/Project/SpeedyBeeF405V3/src/lib/Drivers/CMSIS/Device/ST/STM32F4xx/Include" -I"C:/Users/jjins/Documents/Project/SpeedyBeeF405V3/src/lib/Drivers/STM32F4xx_HAL_Driver/Inc" -I"C:/Users/jjins/Documents/Project/SpeedyBeeF405V3/src/lib/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc" -I"C:/Users/jjins/Documents/Project/SpeedyBeeF405V3/src/lib/Middlewares/ST/STM32_USB_Device_Library/Core/Inc" -I"C:/Users/jjins/Documents/Project/SpeedyBeeF405V3/src/lib/Middlewares/Third_Party/FatFs/src" -I"C:/Users/jjins/Documents/Project/SpeedyBeeF405V3/src/lib/USB_DEVICE/App" -I"C:/Users/jjins/Documents/Project/SpeedyBeeF405V3/src/lib/USB_DEVICE/Target" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-SpeedyBeeF405V3_CUBE-2f-Middlewares-2f-Third_Party-2f-FatFs-2f-src

clean-SpeedyBeeF405V3_CUBE-2f-Middlewares-2f-Third_Party-2f-FatFs-2f-src:
	-$(RM) ./SpeedyBeeF405V3_CUBE/Middlewares/Third_Party/FatFs/src/diskio.cyclo ./SpeedyBeeF405V3_CUBE/Middlewares/Third_Party/FatFs/src/diskio.d ./SpeedyBeeF405V3_CUBE/Middlewares/Third_Party/FatFs/src/diskio.o ./SpeedyBeeF405V3_CUBE/Middlewares/Third_Party/FatFs/src/diskio.su ./SpeedyBeeF405V3_CUBE/Middlewares/Third_Party/FatFs/src/ff.cyclo ./SpeedyBeeF405V3_CUBE/Middlewares/Third_Party/FatFs/src/ff.d ./SpeedyBeeF405V3_CUBE/Middlewares/Third_Party/FatFs/src/ff.o ./SpeedyBeeF405V3_CUBE/Middlewares/Third_Party/FatFs/src/ff.su ./SpeedyBeeF405V3_CUBE/Middlewares/Third_Party/FatFs/src/ff_gen_drv.cyclo ./SpeedyBeeF405V3_CUBE/Middlewares/Third_Party/FatFs/src/ff_gen_drv.d ./SpeedyBeeF405V3_CUBE/Middlewares/Third_Party/FatFs/src/ff_gen_drv.o ./SpeedyBeeF405V3_CUBE/Middlewares/Third_Party/FatFs/src/ff_gen_drv.su

.PHONY: clean-SpeedyBeeF405V3_CUBE-2f-Middlewares-2f-Third_Party-2f-FatFs-2f-src

