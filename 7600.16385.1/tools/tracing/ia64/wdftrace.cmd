REM -------------------------
REM USAGE:
REM   wdftrace start
REM   wdftrace stop
REM   wdftrace view
REM   wdftrace view_realtime
REM -------------------------
REM Make sure WMI trace tools (tracelog, tracepdb, tracefmt) are in your execution PATH

@if "%_echo%"=="" echo off

setlocal


set _TRACE_LOG_PATH=%SystemRoot%\Tracing\%_TRACE_NAME%

set _PROVIDER_GUID=#544d4c9d-942c-46d5-bf50-df5cd9524a50

if not defined _TRACE_NAME (
    set _TRACE_NAME=WDF
)

if not defined TRACE_FORMAT_PREFIX (
  set TRACE_FORMAT_PREFIX=%%2!-20.20s!%%!FUNC!- 
)

if not defined _TRACE_FLAGS (
   set _TRACE_FLAGS=0xffff
)

if not defined _TRACE_LEVEL (
   set _TRACE_LEVEL=5
)

if /I "%1" == "start" goto STARTTRACE
if /I "%1" == "stop" goto STOPTRACE
if /I "%1" == "view" goto VIEWTRACE
if /I "%1" == "view_realtime" goto VIEWTRACE_REALTIME

    
echo "Usage: %0 start|stop|view"
echo You must have the WMI trace tools - tracelog, tracepdb, tracefmt - installed in your path.
goto :EOF

:STARTTRACE
md "%_TRACE_LOG_PATH%" >NUL 2>NUL
tracelog -start "%_TRACE_NAME%" -seq 10 -rt -guid %_PROVIDER_GUID% -flags %_TRACE_FLAGS% -level %_TRACE_LEVEL% -f "%_TRACE_LOG_PATH%\%_TRACE_NAME%.etl"
goto :EOF

:STOPTRACE
tracelog -stop "%_TRACE_NAME%"
goto :EOF

:VIEWTRACE
if not defined _TMF_FILES (
   echo Error: set _TMF_FILES environment variable to point to the directory that contains all the WDF TMF files
   goto :EOF
)

tracelog -flush "%_TRACE_NAME%"
tracefmt "%_TRACE_LOG_PATH%\%_TRACE_NAME%.etl" -p "%_TMF_FILES%" -nosummary -o "%_TRACE_LOG_PATH%\%COMPUTERNAME%-%_TRACE_NAME%.txt"
@echo Tracelog dumped to %_TRACE_LOG_PATH%\%COMPUTERNAME%-%_TRACE_NAME%.txt
start notepad "%_TRACE_LOG_PATH%\%COMPUTERNAME%-%_TRACE_NAME%.txt"
goto :EOF

:VIEWTRACE_REALTIME

if not defined _TMF_FILES (
   echo Error: set _TMF_FILES environment variable to point to the directory that contains all the WDF TMF files
   goto :EOF
)

tracefmt -rt %_TRACE_NAME% -p "%_TMF_FILES%" -display
goto :EOF

