################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/subdir.mk
-include OculusHub_Src/subdir.mk
-include BpDevices_Src/subdir.mk
-include BpClasses_Src/subdir.mk
-include BpApplication_Src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_LIST += \
OculusHubMainDebug.lst \

SECONDARY_SIZE += \
OculusHubMainDebug.siz \


# All Target
all: pre-build main-build

# Main-build Target
main-build: OculusHubMainDebug.elf secondary-outputs

# Tool invocations
OculusHubMainDebug.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: Cross ARM C++ Linker'
	arm-none-eabi-g++ -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -T "C:\Users\s.morris\OculusHub\OculusHubMain\linker_main.ld" -Xlinker --gc-sections -Wl,-Map,"OculusHubMainDebug.map" -specs=nano.specs -specs=nosys.specs -o "OculusHubMainDebug.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

OculusHubMainDebug.lst: OculusHubMainDebug.elf
	@echo 'Invoking: Cross ARM GNU Create Listing'
	arm-none-eabi-objdump --source --all-headers --demangle --line-numbers --wide "OculusHubMainDebug.elf" > "OculusHubMainDebug.lst"
	@echo 'Finished building: $@'
	@echo ' '

OculusHubMainDebug.siz: OculusHubMainDebug.elf
	@echo 'Invoking: Cross ARM GNU Print Size'
	arm-none-eabi-size --format=sysv --totals "OculusHubMainDebug.elf"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(SECONDARY_SIZE)$(C++_DEPS)$(OBJS)$(C_DEPS)$(ASM_DEPS)$(CC_DEPS)$(CPP_DEPS)$(CXX_DEPS)$(C_UPPER_DEPS)$(SECONDARY_LIST)$(S_UPPER_DEPS) OculusHubMainDebug.elf
	-@echo ' '

pre-build:
	-@echo 'Perform Pre-Build Actions (Incriment version numbers)'
	-C:/Users/s.morris/OculusHub/OculusHubMain/prebuild.bat DEBUG "C:/Users/s.morris/OculusHub/OculusHubMain"
	-@echo ' '

post-build:
	-@echo 'Perform Post-Build Actions (Compile FWX firmware)'
	-C:/Users/s.morris/OculusHub/OculusHubMain/postbuild.bat DEBUG "C:/Users/s.morris/OculusHub/OculusHubMain" "OculusHubMainDebug"
	-@echo ' '

secondary-outputs: $(SECONDARY_LIST) $(SECONDARY_SIZE)

.PHONY: all clean dependents
.SECONDARY: main-build pre-build post-build

-include ../makefile.targets
