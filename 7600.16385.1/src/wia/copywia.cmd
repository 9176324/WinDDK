@echo off
rem  Copy all of the WIA DDK binaries to a directory named wiabins. An
rem  optional parameter is a destination directory to prepend to wiabins.

if /I "%BUILD_DEFAULT_TARGETS%" EQU "-386" set cpu_samples=i386
if /I "%BUILD_DEFAULT_TARGETS%" EQU "-amd64" set cpu_samples=amd64
if /I "%BUILD_DEFAULT_TARGETS%" EQU "-ia64" set cpu_samples=ia64

if /I "%BUILD_DEFAULT_TARGETS%" EQU "-386" set cpu_tools=x86
if /I "%BUILD_DEFAULT_TARGETS%" EQU "-amd64" set cpu_tools=amd64
if /I "%BUILD_DEFAULT_TARGETS%" EQU "-ia64" set cpu_tools=ia64

md %1wiabins
md %1wiabins\drivers


copy microdrv\obj%build_alt_dir%\%cpu_samples%\testmcro.dll   %1wiabins\drivers
copy microdrv\testmcro.inf                             %1wiabins\drivers

copy wiacam\wiacam.inf                                 %1wiabins\drivers
copy wiacam\obj%build_alt_dir%\%cpu_samples%\wiacam.dll       %1wiabins\drivers
copy microcam\obj%build_alt_dir%\%cpu_samples%\fakecam.dll    %1wiabins\drivers
copy extend\obj%build_alt_dir%\%cpu_samples%\extend.dll       %1wiabins\drivers

copy sti\sampcpl\obj%build_alt_dir%\%cpu_samples%\sampcpl.cpl  %1wiabins\drivers
copy sti\sampusd\obj%build_alt_dir%\%cpu_samples%\sampusd.dll  %1wiabins\drivers
copy sti\sampusd\sampusd.inf                            %1wiabins\drivers

if /I "%DDK_TARGET_OS%" EQU "WinLH" (
copy wiadriverex\usd\obj%build_alt_dir%\%cpu_samples%\wiadriverex.dll       %1wiabins\drivers
copy wiadriverex\segfilter\obj%build_alt_dir%\%cpu_samples%\segfilter.dll   %1wiabins\drivers
copy wiadriverex\imgfilter\obj%build_alt_dir%\%cpu_samples%\imgfilter.dll   %1wiabins\drivers
copy wiadriverex\errhandler\obj%build_alt_dir%\%cpu_samples%\errhandler.dll %1wiabins\drivers
copy wiadriverex\uiext2\obj%build_alt_dir%\%cpu_samples%\uiext2.dll         %1wiabins\drivers
copy wiadriverex\wiadriver.inf                                       %1wiabins\drivers
copy wiadriverex\sample.bmp                                          %1wiabins\drivers )

copy %basedir%\tools\wia\%cpu_tools%\wiatest.exe            %1wiabins
copy %basedir%\tools\wia\%cpu_tools%\wiainfo2.exe           %1wiabins
copy %basedir%\tools\wia\%cpu_tools%\wiatrcvw.exe           %1wiabins
copy %basedir%\tools\wia\%cpu_tools%\wiadbgcfg.exe          %1wiabins
copy %basedir%\tools\wia\%cpu_tools%\wialogcfg.exe          %1wiabins
copy %basedir%\tools\wia\%cpu_tools%\wiapreview.exe         %1wiabins

goto end

:Syntax
Echo %0 Drive\path\

:end

