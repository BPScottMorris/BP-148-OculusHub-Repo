@echo off

rem Store the name of the active configuration
set BUILD_MODE=%1

rem Store the base path of the project (make all "/" characters be "\"
set PROJ_PATH=%2
set PROJ_PATH=%PROJ_PATH:/=\%

rem Determine what action to take
if "%BUILD_MODE%"=="DEBUG" (	
	goto PreBuild
)
if "%BUILD_MODE%"=="RELEASE" (	
	goto PreBuild
)

echo Invalid build action parameter.
set ERRORLEVEL=100
goto End

rem ---------------------------------------------------------------------------
rem PERFORM PRE-BUILD ACTIONS (Incriment Version numbers etc)
rem ---------------------------------------------------------------------------
:PreBuild
rem (
	echo Performing pre-build commands at %TIME%, %DATE%...
	echo Pre-build Mode "%BUILD_MODE%"
	echo Pre-build Project Directory "%PROJ_PATH%"

	set INC_FILE=headers\version.h
	set INC_TOKEN=FIRMWARE_VERSION_BUILD

	rem Incriment the version number for release builds only
	if "%BUILD_MODE%"=="RELEASE" (			
		echo Auto-Incriment File "%INC_FILE%"
		echo Auto-Incriment Token "%INC_TOKEN%"
	
		echo Incrimenting version number...
		%PROJ_PATH%\tools\AutoInc.exe /f:"%PROJ_PATH%\%INC_FILE%" /s:"%INC_TOKEN%"
	)

	if "%BUILD_MODE%"=="DEBUG" (
		echo Version number not incriment for DEBUG builds.
		
		%PROJ_PATH%\tools\AutoInc.exe /f:"%PROJ_PATH%\%INC_FILE%" /s:"%INC_TOKEN%"
	)

	echo Pre-build complete
rem ) > PreBuild.log

goto End

rem ---------------------------------------------------------------------------
:End
