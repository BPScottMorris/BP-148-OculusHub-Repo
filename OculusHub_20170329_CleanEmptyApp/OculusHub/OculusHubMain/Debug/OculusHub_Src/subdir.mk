################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
C:/Users/s.morris/OculusHub/OculusHub/src/oculushub.cpp 

C_SRCS += \
C:/Users/s.morris/OculusHub/OculusHub/src/cpu.c \
C:/Users/s.morris/OculusHub/OculusHub/src/startup.c 

OBJS += \
./OculusHub_Src/cpu.o \
./OculusHub_Src/oculushub.o \
./OculusHub_Src/startup.o 

C_DEPS += \
./OculusHub_Src/cpu.d \
./OculusHub_Src/startup.d 

CPP_DEPS += \
./OculusHub_Src/oculushub.d 


# Each subdirectory must supply rules for building sources it contributes
OculusHub_Src/cpu.o: C:/Users/s.morris/OculusHub/OculusHub/src/cpu.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -DDEBUG -I"C:\Users\s.morris\OculusHub\BpClasses\headers" -I"C:\Users\s.morris\OculusHub\BpApplication\headers" -I"C:\Users\s.morris\OculusHub\BpDevices_K60\headers" -I"C:\Users\s.morris\OculusHub\OculusHub\headers" -I"C:\Users\s.morris\OculusHub\OculusHubMain\headers" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

OculusHub_Src/oculushub.o: C:/Users/s.morris/OculusHub/OculusHub/src/oculushub.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C++ Compiler'
	arm-none-eabi-g++ -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -DDEBUG -I"C:\Users\s.morris\OculusHub\BpClasses\headers" -I"C:\Users\s.morris\OculusHub\BpApplication\headers" -I"C:\Users\s.morris\OculusHub\BpDevices_K60\headers" -I"C:\Users\s.morris\OculusHub\OculusHub\headers" -I"C:\Users\s.morris\OculusHub\OculusHubMain\headers" -std=gnu++11 -fabi-version=0 -fno-exceptions -fno-rtti -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

OculusHub_Src/startup.o: C:/Users/s.morris/OculusHub/OculusHub/src/startup.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -DDEBUG -I"C:\Users\s.morris\OculusHub\BpClasses\headers" -I"C:\Users\s.morris\OculusHub\BpApplication\headers" -I"C:\Users\s.morris\OculusHub\BpDevices_K60\headers" -I"C:\Users\s.morris\OculusHub\OculusHub\headers" -I"C:\Users\s.morris\OculusHub\OculusHubMain\headers" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


