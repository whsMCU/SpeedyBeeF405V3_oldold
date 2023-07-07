################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/lib/USB_DEVICE/Target/subdir.mk
-include src/lib/USB_DEVICE/App/subdir.mk
-include src/lib/Middlewares/Third_Party/FatFs/src/option/subdir.mk
-include src/lib/Middlewares/Third_Party/FatFs/src/subdir.mk
-include src/lib/Middlewares/ST/STM32_USB_Device_Library/Core/Src/subdir.mk
-include src/lib/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Src/subdir.mk
-include src/lib/Drivers/STM32F4xx_HAL_Driver/Src/subdir.mk
-include src/hw/driver/sdcard/subdir.mk
-include src/hw/driver/gps/subdir.mk
-include src/hw/driver/compass/subdir.mk
-include src/hw/driver/barometer/subdir.mk
-include src/hw/driver/accgyro/subdir.mk
-include src/hw/driver/subdir.mk
-include src/hw/subdir.mk
-include src/common/core/sensors/subdir.mk
-include src/common/core/scheduler/subdir.mk
-include src/common/core/rx/subdir.mk
-include src/common/core/pg/subdir.mk
-include src/common/core/config/subdir.mk
-include src/common/core/BMI270-Sensor-API/subdir.mk
-include src/common/core/subdir.mk
-include src/common/subdir.mk
-include src/bsp/startup/subdir.mk
-include src/bsp/subdir.mk
-include src/ap/subdir.mk
-include src/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := SpeedyBeeF405V3
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
SpeedyBeeF405V3.elf \

MAP_FILES += \
SpeedyBeeF405V3.map \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
SpeedyBeeF405V3.list \


# All Target
all: main-build

# Main-build Target
main-build: SpeedyBeeF405V3.elf secondary-outputs

# Tool invocations
SpeedyBeeF405V3.elf SpeedyBeeF405V3.map: $(OBJS) $(USER_OBJS) D:\STM32CubeIDE\SpeedyBeeF405V3\src\bsp\ldscript\STM32F405RGTX_FLASH.ld makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-gcc -o "SpeedyBeeF405V3.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m4 -T"D:\STM32CubeIDE\SpeedyBeeF405V3\src\bsp\ldscript\STM32F405RGTX_FLASH.ld" --specs=nosys.specs -Wl,-Map="SpeedyBeeF405V3.map" -Wl,--gc-sections -static --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

SpeedyBeeF405V3.list: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "SpeedyBeeF405V3.list"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) SpeedyBeeF405V3.elf SpeedyBeeF405V3.list SpeedyBeeF405V3.map default.size.stdout
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST)

fail-specified-linker-script-missing:
	@echo 'Error: Cannot find the specified linker script. Check the linker settings in the build configuration.'
	@exit 2

warn-no-linker-script-specified:
	@echo 'Warning: No linker script specified. Check the linker settings in the build configuration.'

.PHONY: all clean dependents main-build fail-specified-linker-script-missing warn-no-linker-script-specified

-include ../makefile.targets
