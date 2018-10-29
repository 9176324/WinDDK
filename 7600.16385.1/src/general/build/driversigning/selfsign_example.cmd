REM Copyright (c) Microsoft Corporation. All rights reserved.
Echo off
Rem Clean up environment before starting demo
Rem Delete any old certificates
certmgr -del -all -s PrivateCertStore >junk.txt
set CERTDIR=%CD%
del junk.txt
set myproc=x86
Echo on
cls
@Echo *******************************************************************************
@Echo This script is intended to show driver signing for Windows Vista.
@Echo It should run correctly on Windows XP and Windows Server 2003,
@Echo however some of the instructions may not be relevant prior to Windows Vista.
@Echo *******************************************************************************
@Echo For Windows Vista, this script needs to "Run as Administrator" in a 
@Echo WDK Build Environment Window.
@Echo .
@Echo Some of the tools will not run correctly if you are not "running elevated".
@Echo .
@Echo If you did not open the Window as Administrator, please do the following:
@Echo From the Start Menu, navigate to the WDK Build Environments.
@Echo Instead of left clicking on the build environment shortcut, right click.
@Echo Now select "Run as Administrator"
@Echo .
@Echo Hit Ctrl Break to stop this script if you are not "Running as Administrator."
@Echo *******************************************************************************
pause
cls
echo off
REM CHECK for x64
if defined %PROCESSOR_ARCHITEW6432%  goto AMD64
if not %PROCESSOR_ARCHITECTURE% == AMD64 goto Continue
:AMD64
set myproc=AMD64
rem CHECK if testsigning is on
rem bcdedit >junk.txt
rem findstr /c:"testsigning             Yes" junk.txt
rem if %errorlevel% EQU 0 goto Continue 
rem bcdedit -set testsigning on
cls
@Echo *******************************************************************************
@ECHO On x64 Vista, Testsigning must be set on for this script to work correctly.
@Echo If Testsigning is on, you will see "Test Mode" in the 4 corners of your desktop.
@ECHO .
@Echo If Testsigning is not on, please open a command window, running
@Echo as Administrator, and issue the command:
@Echo bcdedit -set testsigning on
@Echo Then please reboot and rerun this script.
@Echo *******************************************************************************
Pause
:Continue
cls
Echo on
@Echo *******************************************************************************
@Echo This is a demonstration of how to use the tools in the WDK to:
@Echo 1. Create a certificate
@Echo 2. Sign your driver package with the certificate 
@Echo 3. Install the certificate into your certificate store
@Echo 4. Install the driver
@Echo .
@Echo *******************************************************************************
@Echo First, We will create a certificate with the makecert.exe tool.
@Echo Syntax: Makecert -r -pe -ss YourCertStore -n "CN=TestCertName" TestCert.cer
@Echo *******************************************************************************

Makecert -r -pe -ss PrivateCertStore -n "CN=TestCertforWDK" TestCert.cer
 
Pause
cls
@Echo *******************************************************************************
@Echo After you have created your cert, you can use it to sign your driver package.
@Echo We will use the toaster driver in our demo.
@Echo .
@Echo .
@Echo Go to the directory with the toaster driver package.
pushd %PROJECT_ROOT%\general\toaster\toastpkg\toastcd
pause
REM Copy the catalog files that ship with the kit
If not exist *.original copy *.cat *.original
cls

@Echo *******************************************************************************
@Echo Next, you need to create a CAT file for your driver package.
@Echo You can use inf2cat.exe to create the CAT file from your INF file.
@Echo Syntax: inf2cat /driver:path to driver location  /os:OS name
@Echo Use inf2cat /? or the WDK documentation for more information and valid OS names.
@Echo *******************************************************************************

inf2cat.exe /driver:%PROJECT_ROOT%\general\toaster\toastpkg\toastcd /os:Vista_x86,Vista_X64

pause
cls

@Echo *******************************************************************************
@Echo Now that we have a CAT file, we need to sign it.
@Echo We will use signtool.exe to sign the CAT file.
@Echo .
@Echo Syntax: SignTool sign /s YourPrivateCertStore yourCat.cat
@Echo .
@Echo *******************************************************************************


SignTool sign /s PrivateCertStore %PROJECT_ROOT%\general\toaster\toastpkg\toastcd\tostx86.cat
pause
SignTool sign /s PrivateCertStore %PROJECT_ROOT%\general\toaster\toastpkg\toastcd\tstamd64.cat

pause
cls

@Echo *******************************************************************************
@Echo We will also embed sign the driver binaries.  For boot critical drivers you
@Echo should embed sign the binaries to optimize boot performance.
@Echo We will use signtool.exe to sign the binaries file.
@Echo .
@Echo Syntax: SignTool sign /s YourPrivateCertStore yourdriver.sys
@Echo .
@Echo *******************************************************************************


SignTool sign /s PrivateCertStore %PROJECT_ROOT%\general\toaster\toastpkg\toastcd\i386\toaster.sys
SignTool sign /s PrivateCertStore %PROJECT_ROOT%\general\toaster\toastpkg\toastcd\i386\tostrco2.dll
SignTool sign /s PrivateCertStore %PROJECT_ROOT%\general\toaster\toastpkg\toastcd\amd64\toaster.sys
SignTool sign /s PrivateCertStore %PROJECT_ROOT%\general\toaster\toastpkg\toastcd\amd64\tostrco2.dll

pause
cls
@Echo *******************************************************************************
@Echo IMPORTANT: If you are signing a driver for public release you should be using
@Echo a Software Publisher Certificate with a Cross Certificate to the Microsoft
@Echo root.  And you should TIMESTAMP your signature.  The syntax for production
@Echo signing is:
@Echo Signtool sign /v /ac CrossCertificateFile /s SPCCertificateStore 
@Echo     /n SPCSubjectName /t http://timestamp.verisign.com /scripts/timestamp.dll
@Echo     yourCat.cat
@Echo .
@Echo For more information on signing for publication, go to:
@Echo http://www.microsoft.com/whdc/winlogo/drvsign/drvsign.mspx
@Echo *******************************************************************************
pause
cls
@Echo *******************************************************************************
@Echo We recommend that you use a different machine for testing a driver than the
@Echo one you used to build your driver.  Signing will normally occur on the build
@Echo machine.  The steps that follow will normally occur on the test machine.
@Echo *******************************************************************************
pause
cls
@Echo *******************************************************************************
@Echo Before you can install your Driver Package you must add your test certificates
@Echo into the certificate store on the target machine.
@Echo .
@Echo You can automate certificate installation using the certmgr.exe tool.
@Echo Syntax: certmgr.exe -add TestCert.cer -s -r localMachine root 
@Echo *******************************************************************************

certmgr.exe -add %CERTDIR%\testcert.cer -s -r localMachine root 
certmgr.exe -add %CERTDIR%\testcert.cer -s -r localMachine trustedpublisher
Pause
cls
@Echo *******************************************************************************
@Echo Finally, you can use devcon.exe to install the signed driver
@Echo Syntax: devcon install INFfile HardWareID

@Echo *******************************************************************************
Echo on
if %myproc% == x86 %basedir%\tools\devcon\i386\devcon.exe install %PROJECT_ROOT%\general\toaster\toastpkg\toastcd\toastpkg.inf {b85b7c50-6a01-11d2-b841-00c04fad5171}\mstoaster
if %myproc% == AMD64 %basedir%\tools\devcon\amd64\devcon.exe install %PROJECT_ROOT%\general\toaster\toastpkg\toastcd\toastpkg.inf {b85b7c50-6a01-11d2-b841-00c04fad5171}\mstoaster

Pause
popd
cls
@Echo *******************************************************************************
@Echo *******************************************************************************
@Echo You can see the Toaster driver successfully installed using Device Manager.
@Echo You can also see the signature (unless you are running x86 Windows XP).  
@Echo It appears on the driver, but not the individual files of the driver.  
@Echo .
@Echo If you are running on Windows Vista x64 and you do not have testsigning turned
@Echo on, you will see an error for the toaster driver in Device Manager.
@Echo .
@Echo Thank you for viewing this demo.
@Echo *******************************************************************************
@Echo *******************************************************************************

