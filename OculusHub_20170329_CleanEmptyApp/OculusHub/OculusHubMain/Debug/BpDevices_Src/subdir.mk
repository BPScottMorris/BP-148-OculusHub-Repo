################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
C:/Users/s.morris/OculusHub/BpDevices_K60/src/com.cpp \
C:/Users/s.morris/OculusHub/BpDevices_K60/src/com_uart.cpp \
C:/Users/s.morris/OculusHub/BpDevices_K60/src/flash.cpp \
C:/Users/s.morris/OculusHub/BpDevices_K60/src/mcg.cpp \
C:/Users/s.morris/OculusHub/BpDevices_K60/src/systick.cpp 

OBJS += \
./BpDevices_Src/com.o \
./BpDevices_Src/com_uart.o \
./BpDevices_Src/flash.o \
./BpDevices_Src/mcg.o \
./BpDevices_Src/systick.o 

CPP_DEPS += \
./BpDevices_Src/com.d \
./BpDevices_Src/com_uart.d \
./BpDevices_Src/flash.d \
./BpDevices_Src/mcg.d \
./BpDevices_Src/systick.d 


# Each subdirectory must supply rules for building sources it contributes
BpDevices_Src/com.o: C:/Users/s.morris/OculusHub/BpDevices_K60/src/com.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C++ Compiler'
	arm-none-eabi-g++ -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -DDEBUG -I"C:\Users\s.morris\OculusHub\BpClasses\headers" -I"C:\Users\s.morris\OculusHub\BpApplication\headers" -I"C:\Users\s.morris\OculusHub\BpDevices_K60\headers" -I"C:\Users\s.morris\OculusHub\OculusHub\headers" -I"C:\Users\s.morris\OculusHub\OculusHubMain\headers" -std=gnu++11 -fabi-version=0 -fno-exceptions -fno-rtti -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

BpDevices_Src/com_uart.o: C:/Users/s.morris/OculusHub/BpDevices_K60/src/com_uart.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C++ Compiler'
	arm-none-eabi-g++ -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -DDEBUG -I"C:\Users\s.morris\OculusHub\BpClasses\headers" -I"C:\Users\s.morris\OculusHub\BpApplication\headers" -I"C:\Users\s.morris\OculusHub\BpDevices_K60\headers" -I"C:\Users\s.morris\OculusHub\OculusHub\headers" -I"C:\Users\s.morris\OculusHub\OculusHubMain\headers" -std=gnu++11 -fabi-version=0 -fno-exceptions -fno-rtti -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

BpDevices_Src/flash.o: C:/Users/s.morris/OculusHub/BpDevices_K60/src/flash.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C++ Compiler'
	arm-none-eabi-g++ -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -DDEBUG -I"C:\Users\s.morris\OculusHub\BpClasses\headers" -I"C:\Users\s.morris\OculusHub\BpApplication\headers" -I"C:\Users\s.morris\OculusHub\BpDevices_K60\headers" -I"C:\Users\s.morris\OculusHub\OculusHub\headers" -I"C:\Users\s.morris\OculusHub\OculusHubMain\headers" -std=gnu++11 -fabi-version=0 -fno-exceptions -fno-rtti -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

BpDevices_Src/mcg.o: C:/Users/s.morris/OculusHub/BpDevices_K60/src/mcg.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C++ Compiler'
	arm-none-eabi-g++ -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -DDEBUG -I"C:\Users\s.morris\OculusHub\BpClasses\headers" -I"C:\Users\s.morris\OculusHub\BpApplication\headers" -I"C:\Users\s.morris\OculusHub\BpDevices_K60\headers" -I"C:\Users\s.morris\OculusHub\OculusHub\headers" -I"C:\Users\s.morris\OculusHub\OculusHubMain\headers" -std=gnu++11 -fabi-version=0 -fno-exceptions -fno-rtti -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

BpDevices_Src/systick.o: C:/Users/s.morris/OculusHub/BpDevices_K60/src/systick.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C++ Compiler'
	arm-none-eabi-g++ -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -DDEBUG -I"C:\Users\s.morris\OculusHub\BpClasses\headers" -I"C:\Users\s.morris\OculusHub\BpApplication\headers" -I"C:\Users\s.morris\OculusHub\BpDevices_K60\headers" -I"C:\Users\s.morris\OculusHub\OculusHub\headers" -I"C:\Users\s.morris\OculusHub\OculusHubMain\headers" -std=gnu++11 -fabi-version=0 -fno-exceptions -fno-rtti -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


