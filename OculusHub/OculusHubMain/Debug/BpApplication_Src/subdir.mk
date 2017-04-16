################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
C:/Users/s.morris/OculusHub/BpApplication/src/app.cpp \
C:/Users/s.morris/OculusHub/BpApplication/src/flash_data.cpp \
C:/Users/s.morris/OculusHub/BpApplication/src/flash_prog.cpp \
C:/Users/s.morris/OculusHub/BpApplication/src/service.cpp \
C:/Users/s.morris/OculusHub/BpApplication/src/ticktimer.cpp 

OBJS += \
./BpApplication_Src/app.o \
./BpApplication_Src/flash_data.o \
./BpApplication_Src/flash_prog.o \
./BpApplication_Src/service.o \
./BpApplication_Src/ticktimer.o 

CPP_DEPS += \
./BpApplication_Src/app.d \
./BpApplication_Src/flash_data.d \
./BpApplication_Src/flash_prog.d \
./BpApplication_Src/service.d \
./BpApplication_Src/ticktimer.d 


# Each subdirectory must supply rules for building sources it contributes
BpApplication_Src/app.o: C:/Users/s.morris/OculusHub/BpApplication/src/app.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C++ Compiler'
	arm-none-eabi-g++ -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -DDEBUG -I"C:\Users\s.morris\OculusHub\BpClasses\headers" -I"C:\Users\s.morris\OculusHub\BpApplication\headers" -I"C:\Users\s.morris\OculusHub\BpDevices_K60\headers" -I"C:\Users\s.morris\OculusHub\OculusHub\headers" -I"C:\Users\s.morris\OculusHub\OculusHubMain\headers" -std=gnu++11 -fabi-version=0 -fno-exceptions -fno-rtti -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

BpApplication_Src/flash_data.o: C:/Users/s.morris/OculusHub/BpApplication/src/flash_data.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C++ Compiler'
	arm-none-eabi-g++ -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -DDEBUG -I"C:\Users\s.morris\OculusHub\BpClasses\headers" -I"C:\Users\s.morris\OculusHub\BpApplication\headers" -I"C:\Users\s.morris\OculusHub\BpDevices_K60\headers" -I"C:\Users\s.morris\OculusHub\OculusHub\headers" -I"C:\Users\s.morris\OculusHub\OculusHubMain\headers" -std=gnu++11 -fabi-version=0 -fno-exceptions -fno-rtti -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

BpApplication_Src/flash_prog.o: C:/Users/s.morris/OculusHub/BpApplication/src/flash_prog.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C++ Compiler'
	arm-none-eabi-g++ -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -DDEBUG -I"C:\Users\s.morris\OculusHub\BpClasses\headers" -I"C:\Users\s.morris\OculusHub\BpApplication\headers" -I"C:\Users\s.morris\OculusHub\BpDevices_K60\headers" -I"C:\Users\s.morris\OculusHub\OculusHub\headers" -I"C:\Users\s.morris\OculusHub\OculusHubMain\headers" -std=gnu++11 -fabi-version=0 -fno-exceptions -fno-rtti -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

BpApplication_Src/service.o: C:/Users/s.morris/OculusHub/BpApplication/src/service.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C++ Compiler'
	arm-none-eabi-g++ -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -DDEBUG -I"C:\Users\s.morris\OculusHub\BpClasses\headers" -I"C:\Users\s.morris\OculusHub\BpApplication\headers" -I"C:\Users\s.morris\OculusHub\BpDevices_K60\headers" -I"C:\Users\s.morris\OculusHub\OculusHub\headers" -I"C:\Users\s.morris\OculusHub\OculusHubMain\headers" -std=gnu++11 -fabi-version=0 -fno-exceptions -fno-rtti -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

BpApplication_Src/ticktimer.o: C:/Users/s.morris/OculusHub/BpApplication/src/ticktimer.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C++ Compiler'
	arm-none-eabi-g++ -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -DDEBUG -I"C:\Users\s.morris\OculusHub\BpClasses\headers" -I"C:\Users\s.morris\OculusHub\BpApplication\headers" -I"C:\Users\s.morris\OculusHub\BpDevices_K60\headers" -I"C:\Users\s.morris\OculusHub\OculusHub\headers" -I"C:\Users\s.morris\OculusHub\OculusHubMain\headers" -std=gnu++11 -fabi-version=0 -fno-exceptions -fno-rtti -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


