@if "%overbose%" == "" echo off
goto LBegin

===============================================================================
= This file is NOT Office specific and no changes are necessary to use OACR   =
= in non-Office build environments.                                           =
===============================================================================

-------------------------------------------------------------------------------
OACRLIST.BAT

Part of the Microsoft Auto Code Review (OACR) toolset.

Helper batch file to list warning reports from MSVC (or other editors)

-------------------------------------------------------------------------------

:LHelp
echo Microsoft (R) Auto Code Review
echo.
echo Call without arguments to list most recent warning log to stdout.
echo Call with project name to list warning log for that project to stdout.
echo.
echo See 'oacr help' for information on OACR.
goto LEnd

:LBegin
if "%1" == "-?" goto LHelp
if "%1" == "/?" goto LHelp

if "%1" == "" call oacr list last & goto LEnd
call oacr.bat list %*

:LEnd