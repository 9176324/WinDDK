@if "%overbose%" == "" echo off
goto LBegin

===============================================================================
= This file is NOT Office specific and no changes are necessary to use OACR   =
= in non-Office build environments.                                           =
===============================================================================

-------------------------------------------------------------------------------
OACR.BAT

Part of the Office Auto Code Review (OACR) toolset.

Frontend to the OACR command line tools.
(Needed to set environment variables in the calling process)

Keep in sync with _oacr.exe and oacr.pl

-------------------------------------------------------------------------------

:LBegin
if "%TEMP%" == "" (echo OACR - Error: TEMP doesn't seem to be valid) & goto LEnd

@rem _oacr.exe must be in same location as oacr.bat or on the path
if exist "%~dp0\_oacr.exe" (set _OACR_EXE="%~dp0\_oacr.exe") else (set _OACR_EXE=_oacr.exe)

@rem generate unique temp filename to support concurrent calls
for /f %%i in ('%_OACR_EXE% uid') do set _OACR_SET=%%i
set _OACR_SET=oacr_set_%_OACR_SET%.bat

@rem _oacr.exe writes to %OACR_SETENV_BAT%, fallback is current directory
set OACR_SETENV_BAT=%TEMP%\%_OACR_SET%
if exist "%OACR_SETENV_BAT%" del "%OACR_SETENV_BAT%"
if exist %_OACR_SET% del %_OACR_SET%

@rem let OACR generate the commands to setup the environment
setlocal
@rem oenvtest call is conditional to support non-office clients
if exist "%~dp0\oenvtest.bat" call "%~dp0\oenvtest.bat" nodirs
%_OACR_EXE% %*
endlocal
set _OACR_ERROR_LEVEL=%ERRORLEVEL%

@rem setup the environment
if exist "%OACR_SETENV_BAT%" call "%OACR_SETENV_BAT%" & del "%OACR_SETENV_BAT%"
if exist %_OACR_SET% call %_OACR_SET% & del %_OACR_SET%
set _OACR_EXE=
set _OACR_SET=
set OACR_SETENV_BAT=
if not "%_OACR_ERROR_LEVEL%" == "0" exit /b %_OACR_ERROR_LEVEL%

:LEnd
