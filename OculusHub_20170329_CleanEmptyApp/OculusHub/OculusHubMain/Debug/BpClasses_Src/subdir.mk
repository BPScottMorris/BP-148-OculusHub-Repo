################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
C:/Users/s.morris/OculusHub/BpClasses/src/bitutils.cpp \
C:/Users/s.morris/OculusHub/BpClasses/src/conversion.cpp \
C:/Users/s.morris/OculusHub/BpClasses/src/crc16.cpp \
C:/Users/s.morris/OculusHub/BpClasses/src/crc32.cpp \
C:/Users/s.morris/OculusHub/BpClasses/src/datetime.cpp \
C:/Users/s.morris/OculusHub/BpClasses/src/maths.cpp \
C:/Users/s.morris/OculusHub/BpClasses/src/pid.cpp \
C:/Users/s.morris/OculusHub/BpClasses/src/serialize.cpp \
C:/Users/s.morris/OculusHub/BpClasses/src/serializebuffer.cpp \
C:/Users/s.morris/OculusHub/BpClasses/src/sha1.cpp \
C:/Users/s.morris/OculusHub/BpClasses/src/tea.cpp 

C_SRCS += \
C:/Users/s.morris/OculusHub/BpClasses/src/macros.c 

OBJS += \
./BpClasses_Src/bitutils.o \
./BpClasses_Src/conversion.o \
./BpClasses_Src/crc16.o \
./BpClasses_Src/crc32.o \
./BpClasses_Src/datetime.o \
./BpClasses_Src/macros.o \
./BpClasses_Src/maths.o \
./BpClasses_Src/pid.o \
./BpClasses_Src/serialize.o \
./BpClasses_Src/serializebuffer.o \
./BpClasses_Src/sha1.o \
./BpClasses_Src/tea.o 

C_DEPS += \
./BpClasses_Src/macros.d 

CPP_DEPS += \
./BpClasses_Src/bitutils.d \
./BpClasses_Src/conversion.d \
./BpClasses_Src/crc16.d \
./BpClasses_Src/crc32.d \
./BpClasses_Src/datetime.d \
./BpClasses_Src/maths.d \
./BpClasses_Src/pid.d \
./BpClasses_Src/serialize.d \
./BpClasses_Src/serializebuffer.d \
./BpClasses_Src/sha1.d \
./BpClasses_Src/tea.d 


# Each subdirectory must supply rules for building sources it contributes
BpClasses_Src/bitutils.o: C:/Users/s.morris/OculusHub/BpClasses/src/bitutils.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C++ Compiler'
	arm-none-eabi-g++ -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -DDEBUG -I"C:\Users\s.morris\OculusHub\BpClasses\headers" -I"C:\Users\s.morris\OculusHub\BpApplication\headers" -I"C:\Users\s.morris\OculusHub\BpDevices_K60\headers" -I"C:\Users\s.morris\OculusHub\OculusHub\headers" -I"C:\Users\s.morris\OculusHub\OculusHubMain\headers" -std=gnu++11 -fabi-version=0 -fno-exceptions -fno-rtti -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

BpClasses_Src/conversion.o: C:/Users/s.morris/OculusHub/BpClasses/src/conversion.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C++ Compiler'
	arm-none-eabi-g++ -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -DDEBUG -I"C:\Users\s.morris\OculusHub\BpClasses\headers" -I"C:\Users\s.morris\OculusHub\BpApplication\headers" -I"C:\Users\s.morris\OculusHub\BpDevices_K60\headers" -I"C:\Users\s.morris\OculusHub\OculusHub\headers" -I"C:\Users\s.morris\OculusHub\OculusHubMain\headers" -std=gnu++11 -fabi-version=0 -fno-exceptions -fno-rtti -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

BpClasses_Src/crc16.o: C:/Users/s.morris/OculusHub/BpClasses/src/crc16.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C++ Compiler'
	arm-none-eabi-g++ -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -DDEBUG -I"C:\Users\s.morris\OculusHub\BpClasses\headers" -I"C:\Users\s.morris\OculusHub\BpApplication\headers" -I"C:\Users\s.morris\OculusHub\BpDevices_K60\headers" -I"C:\Users\s.morris\OculusHub\OculusHub\headers" -I"C:\Users\s.morris\OculusHub\OculusHubMain\headers" -std=gnu++11 -fabi-version=0 -fno-exceptions -fno-rtti -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

BpClasses_Src/crc32.o: C:/Users/s.morris/OculusHub/BpClasses/src/crc32.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C++ Compiler'
	arm-none-eabi-g++ -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -DDEBUG -I"C:\Users\s.morris\OculusHub\BpClasses\headers" -I"C:\Users\s.morris\OculusHub\BpApplication\headers" -I"C:\Users\s.morris\OculusHub\BpDevices_K60\headers" -I"C:\Users\s.morris\OculusHub\OculusHub\headers" -I"C:\Users\s.morris\OculusHub\OculusHubMain\headers" -std=gnu++11 -fabi-version=0 -fno-exceptions -fno-rtti -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

BpClasses_Src/datetime.o: C:/Users/s.morris/OculusHub/BpClasses/src/datetime.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C++ Compiler'
	arm-none-eabi-g++ -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -DDEBUG -I"C:\Users\s.morris\OculusHub\BpClasses\headers" -I"C:\Users\s.morris\OculusHub\BpApplication\headers" -I"C:\Users\s.morris\OculusHub\BpDevices_K60\headers" -I"C:\Users\s.morris\OculusHub\OculusHub\headers" -I"C:\Users\s.morris\OculusHub\OculusHubMain\headers" -std=gnu++11 -fabi-version=0 -fno-exceptions -fno-rtti -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

BpClasses_Src/macros.o: C:/Users/s.morris/OculusHub/BpClasses/src/macros.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -DDEBUG -I"C:\Users\s.morris\OculusHub\BpClasses\headers" -I"C:\Users\s.morris\OculusHub\BpApplication\headers" -I"C:\Users\s.morris\OculusHub\BpDevices_K60\headers" -I"C:\Users\s.morris\OculusHub\OculusHub\headers" -I"C:\Users\s.morris\OculusHub\OculusHubMain\headers" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

BpClasses_Src/maths.o: C:/Users/s.morris/OculusHub/BpClasses/src/maths.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C++ Compiler'
	arm-none-eabi-g++ -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -DDEBUG -I"C:\Users\s.morris\OculusHub\BpClasses\headers" -I"C:\Users\s.morris\OculusHub\BpApplication\headers" -I"C:\Users\s.morris\OculusHub\BpDevices_K60\headers" -I"C:\Users\s.morris\OculusHub\OculusHub\headers" -I"C:\Users\s.morris\OculusHub\OculusHubMain\headers" -std=gnu++11 -fabi-version=0 -fno-exceptions -fno-rtti -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

BpClasses_Src/pid.o: C:/Users/s.morris/OculusHub/BpClasses/src/pid.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C++ Compiler'
	arm-none-eabi-g++ -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -DDEBUG -I"C:\Users\s.morris\OculusHub\BpClasses\headers" -I"C:\Users\s.morris\OculusHub\BpApplication\headers" -I"C:\Users\s.morris\OculusHub\BpDevices_K60\headers" -I"C:\Users\s.morris\OculusHub\OculusHub\headers" -I"C:\Users\s.morris\OculusHub\OculusHubMain\headers" -std=gnu++11 -fabi-version=0 -fno-exceptions -fno-rtti -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

BpClasses_Src/serialize.o: C:/Users/s.morris/OculusHub/BpClasses/src/serialize.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C++ Compiler'
	arm-none-eabi-g++ -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -DDEBUG -I"C:\Users\s.morris\OculusHub\BpClasses\headers" -I"C:\Users\s.morris\OculusHub\BpApplication\headers" -I"C:\Users\s.morris\OculusHub\BpDevices_K60\headers" -I"C:\Users\s.morris\OculusHub\OculusHub\headers" -I"C:\Users\s.morris\OculusHub\OculusHubMain\headers" -std=gnu++11 -fabi-version=0 -fno-exceptions -fno-rtti -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

BpClasses_Src/serializebuffer.o: C:/Users/s.morris/OculusHub/BpClasses/src/serializebuffer.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C++ Compiler'
	arm-none-eabi-g++ -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -DDEBUG -I"C:\Users\s.morris\OculusHub\BpClasses\headers" -I"C:\Users\s.morris\OculusHub\BpApplication\headers" -I"C:\Users\s.morris\OculusHub\BpDevices_K60\headers" -I"C:\Users\s.morris\OculusHub\OculusHub\headers" -I"C:\Users\s.morris\OculusHub\OculusHubMain\headers" -std=gnu++11 -fabi-version=0 -fno-exceptions -fno-rtti -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

BpClasses_Src/sha1.o: C:/Users/s.morris/OculusHub/BpClasses/src/sha1.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C++ Compiler'
	arm-none-eabi-g++ -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -DDEBUG -I"C:\Users\s.morris\OculusHub\BpClasses\headers" -I"C:\Users\s.morris\OculusHub\BpApplication\headers" -I"C:\Users\s.morris\OculusHub\BpDevices_K60\headers" -I"C:\Users\s.morris\OculusHub\OculusHub\headers" -I"C:\Users\s.morris\OculusHub\OculusHubMain\headers" -std=gnu++11 -fabi-version=0 -fno-exceptions -fno-rtti -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

BpClasses_Src/tea.o: C:/Users/s.morris/OculusHub/BpClasses/src/tea.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C++ Compiler'
	arm-none-eabi-g++ -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -DDEBUG -I"C:\Users\s.morris\OculusHub\BpClasses\headers" -I"C:\Users\s.morris\OculusHub\BpApplication\headers" -I"C:\Users\s.morris\OculusHub\BpDevices_K60\headers" -I"C:\Users\s.morris\OculusHub\OculusHub\headers" -I"C:\Users\s.morris\OculusHub\OculusHubMain\headers" -std=gnu++11 -fabi-version=0 -fno-exceptions -fno-rtti -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


