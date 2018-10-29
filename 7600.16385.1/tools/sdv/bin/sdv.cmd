@echo off

REM
REM sdv.cmd
REM
REM Invokes the Static Driver Verifier (SDV) frontend staticdv.exe.
REM
REM As a precondition for executing staticdv.exe the variable SDV must
REM be set and point to the root of the SDV installation and the
REM staticdv.exe executable must reside in %SDV%\bin.
REM

setlocal
if defined SDV "%SDV%\bin\staticdv.exe" /?
endlocal
