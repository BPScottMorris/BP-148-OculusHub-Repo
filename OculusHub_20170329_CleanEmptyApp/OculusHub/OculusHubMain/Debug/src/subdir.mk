################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/main.cpp \
../src/oculushub_main.cpp 

OBJS += \
./src/main.o \
./src/oculushub_main.o 

CPP_DEPS += \
./src/main.d \
./src/oculushub_main.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C++ Compiler'
	arm-none-eabi-g++ -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -DDEBUG -I"C:\Company\BLUEPRINT\BP_148_Oculus\Top_Box\Firmware\BP-148-OculusHub-Repo\OculusHub_20170329_CleanEmptyApp\OculusHub\BpClasses\headers" -I"C:\Company\BLUEPRINT\BP_148_Oculus\Top_Box\Firmware\BP-148-OculusHub-Repo\OculusHub_20170329_CleanEmptyApp\OculusHub\BpApplication\headers" -I"C:\Company\BLUEPRINT\BP_148_Oculus\Top_Box\Firmware\BP-148-OculusHub-Repo\OculusHub_20170329_CleanEmptyApp\OculusHub\OculusHubMain\headers" -std=gnu++11 -fabi-version=0 -fno-exceptions -fno-rtti -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


