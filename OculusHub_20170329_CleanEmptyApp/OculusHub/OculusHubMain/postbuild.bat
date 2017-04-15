@echo off

echo.
echo Performing post-build commands at %TIME%, %DATE%...
echo Current Directory="%CD%"
echo.

rem Store the name of the active configuration
set BUILD_MODE=%1

rem Store the base path of the project (make all "/" characters be "\"
set PROJ_PATH=%2
set PROJ_PATH=%PROJ_PATH:/=\%

rem Store the name of the project
set PROJ_NAME=%3

rem Determine what action to take
if "%BUILD_MODE%"=="DEBUG" (	
	goto PostBuild
)
if "%BUILD_MODE%"=="RELEASE" (	
	goto PostBuild
)

echo Post-Build: Invalid build action parameter '%BUILD_MODE%'.
set ERRORLEVEL=100
goto End

rem ---------------------------------------------------------------------------
rem PERFORM POST-BUILD ACTIONS (Build Firmware File)
rem ---------------------------------------------------------------------------
:PostBuild
rem (
	set FIRMWARE_NAME=%PROJ_NAME%
		
	rem Setup variables based on application mode
	if "%BUILD_MODE%"=="DEBUG" (		
		set FIRMWARE_PATH=%PROJ_PATH%\debug
		
	)
	if "%BUILD_MODE%"=="RELEASE" (	
		set FIRMWARE_PATH=%PROJ_PATH%\release
	)
	
	echo Post-build Mode "%BUILD_MODE%"
	echo Post-build Project Directory "%PROJ_PATH%"	
	echo Firmware Path="%FIRMWARE_PATH%"
	echo Firmware Name="%FIRMWARE_NAME%"
	echo.
	
	rem Delete existing files
	del /Q "%FIRMWARE_PATH%\%FIRMWARE_NAME%.bin"
	del /Q "%FIRMWARE_PATH%\%FIRMWARE_NAME%.hex"
	del /Q "%FIRMWARE_PATH%\%FIRMWARE_NAME%.fwx"
	
	rem Manually create the output binary from the ELF file BEFORE we run the FWX firmware-formatter
	rem (This is done as the GCC toolchain want to automatically run post-build commands before
	rem converting the ELF to a binary - so disable this function and do it manually here)
	echo Post-Build: Cross ARM GNU Create Flash Image (Convert ELF to BIN and Intel-Hex)	
	arm-none-eabi-objcopy -O binary "%FIRMWARE_PATH%\%FIRMWARE_NAME%.elf"  "%FIRMWARE_PATH%\%FIRMWARE_NAME%.bin"
	arm-none-eabi-objcopy -O ihex "%FIRMWARE_PATH%\%FIRMWARE_NAME%.elf"  "%FIRMWARE_PATH%\%FIRMWARE_NAME%.hex"
	echo Finished building
	echo.
	
	rem Build the firmware FWX file
	echo Post-Build: Firmware FWX Formatter	
	%PROJ_PATH%\tools\Bpff.exe -src:"%FIRMWARE_PATH%\%FIRMWARE_NAME%.fwdef" -dest:"%FIRMWARE_PATH%\%FIRMWARE_NAME%.fwx"
	echo Finished building
	echo.
	
rem ) > PostBuild.log
goto End

rem ---------------------------------------------------------------------------
:End
	echo Post-build: Complete	
	echo.
	