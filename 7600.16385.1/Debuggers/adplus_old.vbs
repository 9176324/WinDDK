Option Explicit

' ADPlus version and Date
Const VERSION = "6.03.006"
Const VERSIONDATE = "12/16/2008"
'
'
' Documentation for ADPlus can be found in debugger.chm


'*********************************************************************************************
'*
'* File:   	ADPlus.vbs
'* Authors:	Robert Hensing, Solution Integration Engineering - Versions 1 to 5
'*              Israel Burman, Solution Integration Engineering - Versions 6 ->
'*
'* Purpose:	ADPlus is used to automatically generate memory dumps or log files containing
'*		scripted debug output usefull for troubleshooting N-tier WinDNA / .NET applications.  
'*              Since WinDNA / .NET applications often encompass multiple, interdependent 
'*              processes on one or more machines, when troubleshooting an application hang 
'*              or crash it is often necessary to 'look' at all of the processes at the same time.
'*              ADPlus allows you to profile an application that may be hanging by taking 
'*              'snapshots' (memory dumps, or debug log files) of that application and all of
'*              its processes, all at the same time.  Multiple 'snapshots' can then be compared
'*		or analyzed to build a history or profile of an application over time. 
'*               
'*              In addition to taking 'snapshots' of hung/non-responsive WinDNA / .NET applications
'*              ADPlus can be used to troubleshoot applications that are crashing or throwing 
'*              unhandled exceptions which can lead to a crash.
'* Usage:
'*		ADPlus has 3 modes of operation:  'Hang', 'Quick' and 'Crash' mode.
'* 
'* 		In 'Hang' mode, ADPlus assumes that a process is hung and it will attach a
'* 		debugger to the process(s) specified on the command line with either the '-p'
'* 		or '-pn' or '-iis' switches.  After the debugger is attached to the process(s)
'* 		ADPlus will dump the memory of each process to a .dmp file for later analysis
'* 		with a debugger (such as WinDBG).  In this mode, processes are paused briefly 
'* 		while their memory is being dumped to a file and then resumed.
'* 
'* 		In '-quick' mode ADPlus assumes that a process is hung and it will attach a
'* 		debugger to the process(s) specified on the command line with either the '-p'
'* 		or '-pn' or '-iis' switches.  After the debugger is attached to the process(s)
'* 		ADPlus will create mini dumps for each process, containing commonly requested 
'* 		debug information, rather than dumping the entire memory for each process.
'* 		'Quick' mode is generally faster than 'Hang' mode, but requires symbols to be
'* 		installed on the server where ADPlus is running.
'* 
'* 		In 'Crash' mode, ADPlus assumes that a process will crash and it will attach
'* 		a debugger to the process(s) specified on the command line with either the '-p'
'* 		or '-pn' or '-iis' switches.  After the debugger is attached to the process(s)
'* 		ADPlus will configure the debugger to log 'first chance' access violations
'* 		(AV's) to a text file.  When a 'second chance' access violation occurs, the
'* 		processes memory is dumped to a .dmp file for analysis with a debugger such as
'* 		WinDBG.  In this mode, a debugger remains attached to the process(s) until the
'* 		process exits or the debugger is exited by pressing CTRL-C in the minimized
'* 		debugger window.  When the process crashes, or CTRL-C is pressed, it will
'* 		need to be re-started.
'*         
'*              Behavior of ADPlus can be configured through an external configuration file.
'*
'***********************************************************************************************
'*--------------------------------------------------------------------------
'* Run ADPlus without any switches to get usage information
'*
'* Switches: '-hang', '-quick', '-crash', '-iis', '-p <PID>', '-pn <Process Name>
'*           '-sc <Spawn Command>
'*           '-quiet', '-o <output directory>, '-notify <target>
'*           '-c <Config file name>
'*           '-FullOnFirst', 'MiniOnSecond', 'NoDumpOnFirst', 'NoDumpOnSecond'
'*           '-do'
'* 
'* 
'* Required: ('-hang', or '-quick', or '-crash') AND ('-iis' or '-p' or '-pn' or '-sc')
'*           or '-c'  (the config file can include all required parameters)
'* 
'*           The -sc switch, if used, must be the last one.
'* 
'* 
'* Examples: 'ADPlus -hang -iis',     Produces memory dumps of IIS and all
'*                                    MTS/COM+ packages currently running.
'* 
'*           'ADPlus -crash -p 1896', Attaches the debugger to process with PID
'*                                    1896, and monitors it for 1st and 2nd
'*                                    chance access violations (crashes).
'* 
'*           'ADPlus -quick -pn mmc.exe', Attaches the debugger to all instances
'*                                        of MMC.EXE and dumps debug information
'*                                        about these processes to a text file.
'* 
'*           'ADPlus -?' or 'ADPlus -help':  Displays usage information.
'*-------------------------------------------------------------------------------
'****************************************************************************


' These are ADPlus's constants.  By default, they should all be set to FALSE 
' for performance reasons.
' These constants should only be set to TRUE for troubleshooting script problems 
' or producing verbose debug information.  Setting some of these constant to 
' TRUE can have a performance impact on production servers.

' Set DEBUGGING = TRUE to turn on verbose debug output, usefull for 
' troubleshooting problems with AD+.

Const DEBUGGING = FALSE


' Set g_Debug_Log = FALSE if you do NOT want AD+ to create a log file 
' in the InitializeGlobals function. 
' Preferable use the external configuration file to configure which exceptions should be logged 
' and which ones not, and keep the creation of a log file to log important info.



'
'   ADPlus defined structures
'
'   After the declaration of all globals you can find documentation about
'   some specific fields, such as acceptable values for ExceptionAction.Commands1, etc.
'
' Used to store the list of running processes
'
Class RunningProcess
	public Name ' Process Name  (Kept with UCase)
	public ID ' ProcessID
	public Package ' Package name if COM+
	public Selected ' True/False - if the process was selected for ADPlus
End Class
'
' This structure is used to define the available commands that can be 
' executed when a given exception happens
'
Class ExceptionAction
	public ExceptionName ' external name without blanks (Access_Violation)
	public ExceptionCode ' such as av, ii, or hexa values such as "0x80010105"
	public Commands1 ' Commands for 1st chance exception
	public Commands2 ' Commands for 2nd chance exception
	public CustomCommands1 ' CustomCommands for 1st chance exception
	public CustomCommands2 ' CustomCommands for 2nd chance exception
	public ReturnAction1 ' GN, GH, Q;  GN: Go not handled, GH: Go handled, Q: Quit
	public ReturnAction2 ' same for second chance (never use GH to avoid recursive dumps)
	public Comment ' comments to add to the log (lines should be separated with Char(10)
End Class
'
' This structure is used to define the available commands that can be 
' executed when a given break point is reached
'
Class BPAction
	public AdrType ' BP, BU or BM - for BM we can use wild cards in the address
	public Address ' same sintax as defined in windbg help for BP
	public Passes ' same sintax as defined in windbg help for BP
	public Commands ' Commands to be executed (same as for ExceptionAction.Commands1)
	public CustomCommands ' CustomCommands to be executed (same as for ExceptionAction.Commands1)
	public ReturnAction ' G or Q;  (G= Go, Q= Quit)
End Class
'
' This structure is used to define the set of actions to be taken
' when running in ADPlus
'
Class HangAction
	public Name ' internal friendly name (MiniDump, Stacks, etc.)
	public Command ' CDB Command - to be defined only if an internal friendly name not available
                       ' used to add additional custom commands
	public Description ' just a short description to be added to the logs
	public Selected
End Class

' Global variables
'Dim g_MaxApplications not checked anymore
Dim g_LastScriptCommand
Dim g_AdpLogFile ' file where we log data about this ADPlus execution
Dim g_AdpLogBuffer ' string buffer to acumulate data before the log is created
Dim g_ExternalConfigurationFileName ' Name of Configuration File
Dim g_NoTlist ' if true ADPlus will not use TList (no -pn switch can be used)
Dim g_CheckTS ' if checking if it's running from TS when OS prior to XP
Dim g_CheckFreeSpace
Dim g_RunMode
dim g_HelpRequested
Dim g_IISMode
Dim g_QuietMode
Dim g_OSVer
Dim g_OSBuildNumber
Dim g_ComputerName
Dim g_InstallDir ' the directory where ADPlus is being run from
Dim g_OutPutDir ' the output directory selected by the user
dim g_DumpDir ' the directory where we create the dumps (subdirectory of g_OutPutDir)
Dim g_IISVer
Dim g_DateTimeStamp
Dim g_LocalUserName
Dim g_GeneratedScriptName ' if -gs switch was used
Dim g_MinFreeSpace ' minimum free space needed on output drive (will be checked)
Dim g_Debug_Log ' if CDB should create a log file
Dim g_Sympath
Dim g_SympathPlus
Dim g_Debugger
dim g_DebuggerEvent ' used for the -e switch which simply passes its value to the debugger
dim g_DumpOnly ' changes default behavior to create only dumps (no call stacks, etc.)
dim g_AttachInterval ' seconds for multiple attaches in hang mode
dim g_AttachRepeats ' number of repeated attachments

Dim g_objFSO ' FileSystemObject
dim g_objShell ' Wscript.Shell

Redim g_CurrentProcesses(0)
Redim g_SelectedProcessNames(0)
Redim g_SelectedProcessIDs(0)
Redim g_SelectedSpawningCommands(0) ' processes to be spawned by cdb 
ReDim g_ExceptionActions(0)
ReDim g_BPActions(0)  ' break point actions
ReDim g_HangActions(0) ' actions for hang mode

Redim g_PreCommands(0) ' Initialization commands (only for crash mode)
Redim g_PostCommands(0) ' Termination commands (only for crash mode)

' Initialization Shell commands
' Shell commands are stored as ["A" or "S"] plus command
'   The A or S means Asyncronous or Syncronous
Redim g_PreShellCommands(0)

' Termination Shell commands
Redim g_PostShellCommands(0)

' Set g_CheckFreeSpace = FALSE if you do NOT want AD+ to check for 
' free space before running.  Certain RAID drivers may cause problems for 
' the free space checking routines and it may be necessary to set this 
' to FALSE to allow AD+ to run.

Redim g_NotifyList(0)




' The following constants are used by AD+ for launching command shells and for the FileSystemObject.
' These constants should NOT be changed.
Const MINIMIZE_NOACTIVATE = 7
Const ACTIVATE_AND_DISPLAY = 1
Const ForReading = 1, ForWriting = 2, ForAppending = 8
Const TristateUseDefault = -2, TristateTrue = -1, TristateFalse = 0



'=========================================================
'
'   Documentation related to the defined structures
'
'=========================================================
'
'
'  Available Commands to be used in ExceptionAction.Commands
'     Commands should be separated by ";"
'        Ex.: ExceptionAction.Commands1 = "Log;Stack;MiniDump"
'
'   NOTE!!! - Commands are case sensitive
'
'    Log - adds a log entry related to the exception
'    EventLog - adds an avent log related to the exception
'    MiniDump - creates a mini dump
'    FullDump - creates a full dump
'    MiniDumpOver - creates a mini dump and overwrites multiple instances for the same exception
'    FullDumpOver - creates a full dump and overwrites multiple instances for the same exception
'    Stack - lists the faulting call stack into the log
'    Stacks - lists all thread call stacks into the log
'    LoadedModules - Lists all loaded modules
'    MatchingSymbols - Lists all modules with matching symbols
'    Custom_commands - Allows you to add additional commands separated by Chr(10)
'     you can add as many commands you need
'     Example:  "Custom_!lml" & Chr(10) & "!runaway" & Chr(10) & "!locks"
'
'        ExceptionName - should not include blanks
'                        should not include any special character that cannot be part
'                           of a file name (we use this for the file name)
'        Comments - should not include ";" and should not be too long, there is a limit
'                   for the size of the final command. The size of the final string is 2000
'                   characters including all other commands (log, eventlog, dump, etc.)
'


' Hang mode actions
'
'   Standard commands available:
'
'    MiniDump - creates a mini dump
'    FullDump - creates a full dump
'    Stacks - lists all thread call stacks into the log
'    LoadedModules - Lists all loaded modules
'    MatchingSymbols - Lists all modules with matching symbols
'###IB??? SHould we include some of these for exceptions? (heap, handle, locks, threadusage)
'    Heap - Heap information
'    Handle - Handle Information
'    Dlls - Dll information
'    Locks - Critical section information
'    ThreadUsage - Thread cpu information (!runaway)
'



'   Executing the main function
	Main



'---------------------------------------------------------------------------------------------
' Function:  Main
'            The following subroutines and functions provide the core functionality for  
'            ADPlus.  All core functionality should be encapsulated in these sub's or functions.
'---------------------------------------------------------------------------------------------
Sub Main
	dim strAux


	InitializeGlobals
	strAux = GetArguments()
	if strAux <> "" Then
		QuitWithError "Errors found reading external arguments: " & Chr(13) & Chr(10) & strAux & Chr(13) & Chr(10) & "Please see ADPlus.doc for usage information"		
	End If
	ValidateSettings
	PrepareToRun


	' we are ready to roll
	If g_CheckFreeSpace = True Then Call CheckAvailableSpace()

	GetCurrentProcesses
	DumpSelectedProcesses

	ShowStatus
End Sub
' ------------------------------------------------------------------------------------------

'-------------------------------------------------------------------------------------------
' Function: InitializeGlobals
'
'  Created by iburman on 053002
'  This function has all globals initialization to defaultvalues
'  Mostly of what is here was before in the common code
'
'---------------------------------------------------------------------------------------------
Sub InitializeGlobals
	Dim objFolder
	Dim strAux ' temporary string
	Dim i,n
	Dim WShNetwork

	AdpTrace "In the InitializeGlobals() function "

	g_Debug_Log = True
	g_Debugger = "cdb.exe"
	g_DebuggerEvent = ""

	' g_MaxApplications = 20
	g_LastScriptCommand = "Q"
	g_NoTlist = False
	g_CheckTS = True
	set g_AdpLogFile = Nothing
	g_AdpLogBuffer = ""
	AdpLog "ADPlus Version: " & VERSION
	AdpLog "ADPlus Version Date: " & VERSIONDATE

	g_MinFreeSpace = 10000000
	g_CheckFreeSpace = TRUE

	g_ExternalConfigurationFileName = ""
	g_RunMode = ""
	g_DumpDir = ""
	g_IISMode = false
	g_QuietMode = false
	g_HelpRequested = false
	g_GeneratedScriptName = ""
	g_Sympath = ""
	g_SympathPlus = ""

	g_DumpOnly = False

	g_AttachInterval = 0
	g_AttachRepeats = 1



	Set g_objShell = createobject("Wscript.shell")
	Set g_objFSO = CreateObject("Scripting.FileSystemObject")

	DetectOSVersion
	GetDateTimeStamp

	Set WShNetwork = CreateObject("Wscript.Network")
	g_ComputerName = WshNetwork.ComputerName
	AdpLog "ADPlus was run on: " & g_ComputerName


	set g_CurrentProcesses(0) = new RunningProcess
	g_CurrentProcesses(0).Name = ""
	g_CurrentProcesses(0).ID = 0
	g_CurrentProcesses(0).Package = ""
	g_CurrentProcesses(0).Selected = False

	g_SelectedProcessNames(0) = ""
	g_SelectedProcessIDs(0) = 0
	g_SelectedSpawningCommands(0) = ""

	g_PreCommands(0) = ""
	g_PostCommands(0) = ""
	g_PreShellCommands(0) = ""
	g_PostShellCommands(0) = ""

	g_NotifyList(0) = ""
	g_IISMode = false
	g_QuietMode = false
	g_IISVer = ""

	' Get the installation directory name, used for verifying required files are present.
	' and setting it as the default directory
	On Error Resume Next
	g_InstallDir = Left(wscript.scriptfullname,Len(wscript.scriptfullname)-Len(wscript.scriptname)-1)
	Set objFolder = g_objFSO.GetFolder(g_InstallDir)
	if err.Number <> 0 Then
		QuitWithError "Unable to instantiate folder object = Install Directory: " & g_InstallDir
	End If
'
'       The CurrentDirectory property is new to WScript version 5.6
'       If not installed we could start ADPlus from the debuggers folder to avoid the upgrade
'
	if UCase(g_objFSO.GetAbsolutePathName(".")) <> UCase(g_InstallDir) Then
		g_objShell.CurrentDirectory = g_InstallDir
		if err.Number <> 0 Then
			strAux = "Unable to set CurrentDirectory = Install Directory: " & g_InstallDir & Chr(13) & Chr(10)
			strAux = strAux & "Your scripting engine's version (wshom.ocx) is probably prior to 5.6.0.6626"  & Chr(13) & Chr(10)
			strAux = strAux & "To avoid this problem simply start ADPlus from the debuggers folder or upgrade the scripting engine" & Chr(13) & Chr(10)
			strAux = strAux & "You can download Windows Script 5.6 from http://msdn.microsoft.com/downloads"
			QuitWithError strAux
		End If
	End If



	On Error GoTo 0

	AdpLog "Debuggers Installation Directory: " & g_InstallDir

	g_OutPutDir = g_InstallDir





	' defining the default behavior for exceptions
	' if you need to add new exceptions change the dimension below
ReDim g_ExceptionActions(23)
	'
	' First element of the array is for special purpose, it's used to store
	' the default behavior for a given exception.
	' If the user adds a custom exception using the -ce switch, this will be
	' the settings for the new exception.
	'
	set g_ExceptionActions(0) = new ExceptionAction
	g_ExceptionActions(0).ExceptionName = ""
	g_ExceptionActions(0).ExceptionCode = ""
	g_ExceptionActions(0).Commands1 = "Log;Time;Stack;MiniDump"
	g_ExceptionActions(0).Commands2 = "Log;Time;Stack;FullDump;EventLog"
	g_ExceptionActions(0).CustomCommands1 = ""
	g_ExceptionActions(0).CustomCommands2 = ""
	g_ExceptionActions(0).ReturnAction1 = "GN"
	g_ExceptionActions(0).ReturnAction2 = "GN"
	g_ExceptionActions(0).Comment = ""

	' defining the standard exceptions used in ADPlus
	n = UBound(g_ExceptionActions)
	for i=1 to n
		set g_ExceptionActions(i) = new ExceptionAction
		g_ExceptionActions(i).ExceptionName = ""
		g_ExceptionActions(i).ExceptionCode = ""
		g_ExceptionActions(i).Commands1 = g_ExceptionActions(0).Commands1
		g_ExceptionActions(i).Commands2 = g_ExceptionActions(0).Commands2
		g_ExceptionActions(i).CustomCommands1 = g_ExceptionActions(0).CustomCommands1
		g_ExceptionActions(i).CustomCommands2 = g_ExceptionActions(0).CustomCommands2
		g_ExceptionActions(i).ReturnAction1 = g_ExceptionActions(0).ReturnAction1
		g_ExceptionActions(i).ReturnAction2 = g_ExceptionActions(0).ReturnAction2
		g_ExceptionActions(i).Comment = ""
	Next
	' defining the specifics for each exception
	g_ExceptionActions(1).ExceptionName = "AccessViolation"
	g_ExceptionActions(1).ExceptionCode = "av"

	g_ExceptionActions(2).ExceptionName = "InvalidHandle"
	g_ExceptionActions(2).ExceptionCode = "ch"

	g_ExceptionActions(3).ExceptionName = "IllegalInstruction"
	g_ExceptionActions(3).ExceptionCode = "ii"

	g_ExceptionActions(4).ExceptionName = "IntegerDivide"
	g_ExceptionActions(4).ExceptionCode = "dz"

	g_ExceptionActions(5).ExceptionName = "FloatingDivide"
	g_ExceptionActions(5).ExceptionCode = "c000008e"

	g_ExceptionActions(6).ExceptionName = "IntegerOverflow"
	g_ExceptionActions(6).ExceptionCode = "iov"

	g_ExceptionActions(7).ExceptionName = "InvalidLockSequence"
	g_ExceptionActions(7).ExceptionCode = "lsq"

	g_ExceptionActions(8).ExceptionName = "StackOverflow"
	g_ExceptionActions(8).ExceptionCode = "sov"

	g_ExceptionActions(9).ExceptionName = "CPlusPlusEH"
	g_ExceptionActions(9).ExceptionCode = "eh"
	g_ExceptionActions(9).Commands1 = "Log;Time;Stack"

	g_ExceptionActions(10).ExceptionName = "UnknownException"
	g_ExceptionActions(10).ExceptionCode = "*"
	g_ExceptionActions(10).Commands1 = "Log;Time;Stack"

	g_ExceptionActions(11).ExceptionName = "NET_CLR"
	g_ExceptionActions(11).ExceptionCode = "clr" ' was "e0434f4d"
	g_ExceptionActions(11).Commands1 = "VOID"  ' .Net throws a lot of exceptions

'###IB??? think better of how we set this considering that this can be thrown for other reasons than ctl-c
' Do we really want to quit?
	g_ExceptionActions(12).ExceptionName = "CONTRL_C_OR_Debug_Break"
	g_ExceptionActions(12).ExceptionCode = "bpe"
	g_ExceptionActions(12).Commands1 = "Log;Time;EventLog;Stacks;LoadedModules;MatchingSymbols;MiniDump"
	g_ExceptionActions(12).CustomCommands1 = "!locks;!runaway"
	g_ExceptionActions(12).Commands2 = "" ' This one never has second chance.
	g_ExceptionActions(12).ReturnAction1 = "Q"
	g_ExceptionActions(12).ReturnAction2 = "GN"
	strAux = "CTRL-C was pressed to stop debugging this process!"
	strAux = strAux & Chr(10) & "When PageHeap is in use this exception can be an internal exception"
	g_ExceptionActions(12).Comment = strAux

	g_ExceptionActions(13).ExceptionName = "Wake_Debugger"
	g_ExceptionActions(13).ExceptionCode = "wkd"
	g_ExceptionActions(13).Commands1 = "Log;Time;EventLog;Stacks;LoadedModules;MatchingSymbols;MiniDump"
	g_ExceptionActions(13).CustomCommands1 = "!locks;!runaway"
	g_ExceptionActions(13).Commands2 = "" ' we quit at the first chance anyways.
	g_ExceptionActions(13).ReturnAction1 = "Q"
	g_ExceptionActions(13).ReturnAction2 = "GN"
	strAux = "This was added for cases whereby the CTRL-C / debug break exception does not work and the debugger reverts to a non-invasive attach" & Chr(10) & "CTRL-C was pressed to stop debugging this process!"
	strAux = strAux & Chr(10) & "When PageHeap is in use this exception can be an internal exception"
	strAux = strAux & Chr(10) & "Exiting the debugger!!!"
	g_ExceptionActions(13).Comment = strAux

	g_ExceptionActions(14).ExceptionName = "DLL_Load"
	g_ExceptionActions(14).ExceptionCode = "ld"
	g_ExceptionActions(14).Commands1 = "Log"  '  "Log;Time;EventLog;Stack"
	g_ExceptionActions(14).Commands2 = "Log"  '  "Log;Time;EventLog;Stack"

	g_ExceptionActions(15).ExceptionName = "DLL_UnLoad"
	g_ExceptionActions(15).ExceptionCode = "ud"
	g_ExceptionActions(15).Commands1 = "Log"  '  "Log;Time;EventLog;Stack"
	g_ExceptionActions(15).Commands2 = "Log"  '  "Log;Time;EventLog;Stack"

	g_ExceptionActions(16).ExceptionName = "Process_Shut_Down"
	g_ExceptionActions(16).ExceptionCode = "epr"
	g_ExceptionActions(16).Commands1 = "Log;Time;EventLog;Stacks;FullDump"
	g_ExceptionActions(16).CustomCommands1 = "!runaway"
	g_ExceptionActions(16).Commands2 = "" ' we don't get to the second chance anyways, and we need to reduce the size of the final string
	g_ExceptionActions(16).ReturnAction1 = "VOID"  ' epr doesn't allow GN
	g_ExceptionActions(16).ReturnAction2 = "VOID"
	strAux = "This process is shutting down!"
	strAux = strAux & Chr(10) & "This can happen for the following reasons"
	strAux = strAux & Chr(10) & "1) Someone killed the process with Task Manager or the kill command"
	strAux = strAux & Chr(10) & "2.) If this process is an MTS or COM+ server package, it could be" 
	strAux = strAux & Chr(10) & "*    exiting because an MTS/COM+ server package idle limit was reached." 
	strAux = strAux & Chr(10) & "3.) If this process is an MTS or COM+ server package," 
	strAux = strAux & Chr(10) & "*    someone may have shutdown the package via the MTS Explorer or" 
	strAux = strAux & Chr(10) & "*    Component Services MMC snap-in." 
	strAux = strAux & Chr(10) & "4.) If this process is an MTS or COM+ server package," 
	strAux = strAux & Chr(10) & "*    MTS or COM+ could be shutting down the process because an internal" 
	strAux = strAux & Chr(10) & "*    error was detected in the process (MTS/COM+ fail fast condition)." 
	g_ExceptionActions(16).Comment = strAux

	' new exceptions added in ADPlus V6
	g_ExceptionActions(17).ExceptionName = "Application_hang"
	g_ExceptionActions(17).ExceptionCode = "aph"

	g_ExceptionActions(18).ExceptionName = "Ctl_C_Console_app" ' this is the ctl+C from dos
	g_ExceptionActions(18).ExceptionCode = "cce"

	g_ExceptionActions(19).ExceptionName = "Data_misaligned"
	g_ExceptionActions(19).ExceptionCode = "dm"

	g_ExceptionActions(20).ExceptionName = "Guard_page_violation"
	g_ExceptionActions(20).ExceptionCode = "gp"

	g_ExceptionActions(21).ExceptionName = "In_page_IO_error"
	g_ExceptionActions(21).ExceptionCode = "ip"

	g_ExceptionActions(22).ExceptionName = "Invalid_system_call"
	g_ExceptionActions(22).ExceptionCode = "isc"

	g_ExceptionActions(23).ExceptionName = "Stack_buffer_overflow"
	g_ExceptionActions(23).ExceptionCode = "sbo"


	
	' Initializing Breakpoints
	'
	' First element of the array is for special purpose, it's used as a
	' template for all defined breakpoints
	' Breakpoints are defined using the -bp parameter:
	'    -bp address;optional_additional_parameters
	'         optional parameters must be separated by ";" and no blanks and can be any of the following:
	'            MiniDump, FullDump or NoDump - to indicate if you want a dump (default is NoDump)
	'            an integer number - indicating the number of passes to ignore
	'            Q or G - if you want to quit or go after the action; Default is G
	'         if not including any optional parameters the default behavior is to 
	'            create a log,list the call stack, and return with GO.
	'         Example:
	'           -bp "MyModule!MyClass::MyMethod"
	'           -bp "MyModule!MyClass::MyMethod;MiniDump;Q"
	'          
	'
	'
	set g_BPActions(0) = new BPAction
	g_BPActions(0).AdrType = "bp"
	g_BPActions(0).Address = ""
	g_BPActions(0).Passes = ""
	g_BPActions(0).Commands = "Log;Stack"
	g_BPActions(0).CustomCommands = ""
	g_BPActions(0).ReturnAction = "G"

' Defining default behavior for hang mode

ReDim g_HangActions(8)
	n = UBound(g_HangActions)
	for i=0 to n
		set g_HangActions(i) = new HangAction
		g_HangActions(i).Name = ""
		g_HangActions(i).Command = ""
		g_HangActions(i).Description = ""
		g_HangActions(i).Selected = True
	Next

	' defining the specifics
	g_HangActions(0).Name = "FullDump"
	g_HangActions(0).Description = ""

	g_HangActions(1).Name = "Stacks"
	g_HangActions(1).Description = "Thread stack backtrace information"

	g_HangActions(2).Name = "LoadedModules"
	g_HangActions(2).Description = "Loaded modules"

	g_HangActions(3).Name = "MatchingSymbols"
	g_HangActions(3).Description = "Loaded modules with matching symbols"

	g_HangActions(4).Name = "Heap"
	g_HangActions(4).Description = "Heap information"

	g_HangActions(5).Name = "Handle"
	g_HangActions(5).Description = "Handle information"

	g_HangActions(6).Name = "Dlls"
	g_HangActions(6).Description = "DLL Information"

	g_HangActions(7).Name = "Locks"
	g_HangActions(7).Description = "Critical section information"

	g_HangActions(8).Name = "ThreadUsage"
	g_HangActions(8).Description = "Thread CPU usage information"


End Sub




'---------------------------------------------------------------------------------------------
' Function:  GetDateTimeStamp
'            This function is responsible for getting the unique Date / Time stamp used for 
'            creating unique directory / file names.
'            To Change the names of the output directories, edit them below.
'---------------------------------------------------------------------------------------------
Function GetDateTimeStamp()
	
	Dim AMorPM
	Dim Seconds
	Dim Minutes
	Dim Hours
	Dim theDay
	Dim theMonth

	AdpTrace "In the GetDateTimeStamp() function . . ."

	Hours = Hour(Now)
	Minutes = Minute(Now)
	Seconds = Second(Now)
	theDay = Day(Now)
	theMonth = Month(Now)
	AMorPM = Right(Now(),2)
	
	If Len(Hours) = 1 Then Hours = "0" & Hours
	If Len(Minutes) = 1 Then Minutes = "0" & Minutes
	If Len(Seconds) = 1 Then Seconds = "0" & Seconds
	If Len(theDay) = 1 Then theDay = "0" & theDay
	If Len(theMonth) = 1 Then theMonth = "0" & theMonth
	
	g_DateTimeStamp = "Date_" & theMonth & "-" & theDay & "-" & Year(Now) & "__Time_" & Hours & "-" & Minutes & "-" & Seconds & AMorPM

	AdpLog "DateTimeStamp: " & g_DateTimeStamp
	
	
End Function


'---------------------------------------------------------------------------------------------
' Function:  CheckFiles
'            This function is responsible for ensuring that all the required files are  
'            installed in the 'InstallDir'.  If files are missing, display an error or 
'            log an error to the event log if running in quiet mode.
'---------------------------------------------------------------------------------------------
Sub CheckFiles()
	Dim strAux

	AdpTrace "In the CheckFiles() function "

	' all error messages start with the same sentence:
	strAux = "ADPlus is not configured properly. "
	
	'Make sure CDB.EXE & AD+.vbs are in the right place.  If its not it could mean AD+.vbs isn't running from the debuggers directory.
	If not g_objFSO.FileExists(g_InstallDir & "\cdb.exe") Then
		strAux = strAux & "Either the file CDB.EXE is missing from " & g_InstallDir & " or ADPlus.vbs is not running from the debuggers installation directory. "
		strAux = strAux & "Please place ADPlus.vbs in the debuggers installation directory or try re-installing the Debugging Tools for Windows."
		QuitWithError strAux
	End If

		
	'Make sure the NT 4.0 debug extensions are installed
	If g_OSVer = "4.0" Then
		If not g_objFSO.FileExists(g_InstallDir & "\nt4fre\userexts.dll") Then
			strAux = strAux & "The file userexts.dll is missing from " & g_InstallDir & "\nt4fre" &  ". "
			strAux = strAux & "Please re-install the Debugging Tools for Windows."
			QuitWithError strAux
		End If
	End If
		
	'Make sure the Windows 2000 debug extensions are installed
	If CInt(g_OSBuildNumber) = 2195 Then
		If not g_objFSO.FileExists(g_InstallDir & "\w2kfre\userexts.dll") Then
			strAux = strAux & "The file userexts.dll is missing from " & g_InstallDir & "\w2kfre" &  ".  "
			strAux = strAux & "Please re-install the Debugging Tools for Windows."
			QuitWithError strAux
		End If	
	End If
		
'###IB??? Why is this commented out???
	'Make sure the Windows XP debug extensions are installed
	'If CInt(g_OSBuildNumber) > 2195 Then
	'If not g_objFSO.FileExists(g_InstallDir & "\w2001\userexts.dll") Then
	'	strAux = strAux & "The file userexts.dll is missing from " & g_InstallDir & "\winxp" &  ".  "
	'	strAux = strAux & "Please re-install the Debugging Tools for Windows."
	'	QuitWithError strAux
	'End If
	
	If not g_objFSO.FileExists(g_InstallDir & "\dbgeng.dll") Then
		strAux = strAux & "The file dbgeng.dll is missing from " & g_InstallDir & ". "
		strAux = strAux & "Please place ADPlus.vbs in the debuggers installation directory or try re-installing the Debugging Tools for Windows."
		QuitWithError strAux
	end If
	
	If not g_objFSO.FileExists(g_InstallDir & "\dbghelp.dll") then
		strAux = strAux & "The file dbghelp.dll is missing from " & g_InstallDir & ". "
		strAux = strAux & "Please place ADPlus.vbs in the debuggers installation directory or try re-installing the Debugging Tools for Windows."
		QuitWithError strAux
	end If
	
	if g_NoTlist = FALSE Then
		If not g_objFSO.FileExists(g_InstallDir & "\tlist.exe") then
			strAux = strAux & "The file tlist.exe is missing from " & g_InstallDir & ". "
			strAux = strAux & "Please place ADPlus.vbs in the debuggers installation directory or try re-installing the Debugging Tools for Windows."
			QuitWithError strAux
		end If
	end if

End Sub





'---------------------------------------------------------------------------------------------
' Function:  CheckAvailableSpace
'
'      This was the previous RunTlist function.
'      TList is now running in the GetCurrentProcesses function
'
'            This function only checks the available space
'---------------------------------------------------------------------------------------------
Sub CheckAvailableSpace()

	Dim DriveObject
	Dim DriveName
	Dim strAux ' a temporary string
	Dim arrAux ' an array of strings


	

	AdpTrace "In the CheckAvailableSpace() function "


	'Check the output directories drive to ensure there is enough free space for the files.
	If Left(g_DumpDir,2) <> "\\" Then 'We are not logging to a UNC path.
		DriveName = Left(g_DumpDir,1)
	Else
		' we are writing to a UNC path - extracting the server name and share name
		strAux = Right(g_DumpDir, Len(g_DumpDir) - 2) ' removing the first \\
		arrAux = Split(strAux, "\", -1) 
		DriveName = "\\" & arrAux(0) & "\" & arrAux(1)
	End If

	AdpTrace "Checking for space on drive: " & DriveName

	On Error Resume Next
	Set DriveObject = g_objFSO.GetDrive(DriveName)
	if err.number<>0 Then
		QuitWithError "Error seting drive object to: " & DriveName & " Error: " & err.number & " - " & err.description
	End If
	On Error GoTo 0


	If DriveObject.FreeSpace < g_MinFreeSpace Then
		strAux = "ADPlus has detected that there is not enough free space on the " & DriveName & " drive. "
		strAux = strAux & "ADPlus requires at least " & g_MinFreeSpace & " of free space. "
		strAux = strAux & "Please free up some space on that drive and try running ADPlus again."
		QuitWithError strAux
	End If
		


	
End Sub


'---------------------------------------------------------------------------------------------
' Function:  CreateCDBScript
'            This function is used to create the CDB scripts used by the debugger.
'
'---------------------------------------------------------------------------------------------
Function CreateCDBScript(pid, packagename,SpawningCommand)

	Dim objTextFile
	Dim strFile
	Dim strAux
	Dim strAux1
	Dim strAux2
	Dim arrAux
	Dim i,n,j

	AdpTrace "In the CreateCDBScript() function "

	' creating the folder to store the script
	strFile = g_DumpDir & "\CDBScripts"
	CreateDirectory(strFile)
	strFile = g_DumpDir & "\CDBScripts\" & "PID-" & pid & "__" & packagename & ".cfg"
	

	' opening the script file
	On Error Resume Next
		
	Set objTextFile = g_objFSO.CreateTextFile(strFile,True)
	if err.number<>0 Then
		QuitWithError "Error creating file: " & strFile & " Error: " & err.number & " - " & err.description
	End If
	On Error GoTo 0

	' writing initial information and commands
	' common to hang and crash modes

	If g_Debug_Log = TRUE Then
		if g_GeneratedScriptName = "" Then
			objTextFile.Writeline ".logopen " & Chr(34) & g_DumpDir & "\" & "PID-" & pid & "__" & packagename & "__" & g_DateTimeStamp & ".log" & Chr(34)
		else
			objTextFile.Writeline ".logopen /t " & Chr(34) & g_DumpDir & "\"  & packagename  & ".log" & Chr(34)
		End If
	End If
	objTextFile.Writeline "* Created with ADPlus Version " & VERSION & "    Version Date " & VERSIONDATE

	if g_ExternalConfigurationFileName <> "" Then
		objTextFile.Writeline "*"
		objTextFile.Writeline "* ------ !!! External Configuration File was used: ----"

		arrAux = GetStringArray(g_ExternalConfigurationFileName,Chr(10))
		n = UBound(arrAux)
		for i = 1 to n
			objTextFile.Writeline "*    " & arrAux(i)
		Next

		objTextFile.Writeline "*"
	End If

	objTextFile.Writeline "*"
	objTextFile.Writeline "* --------- ADPlus  was started at: -----------" 
	objTextFile.Writeline ".time"
	objTextFile.Writeline "* ------------------------------------------------------"
	objTextFile.Writeline "*"
	objTextFile.Writeline "*"
	objTextFile.Writeline "*"
	objTextFile.Writeline "* --------- ADPlus " & VERSION & " was run on server: --------" 
	objTextFile.Writeline "* Server name: " & g_ComputerName
	objTextFile.Writeline "* ------------------------------------------------------"
	objTextFile.Writeline "*"
	objTextFile.Writeline "*"
	objTextFile.Writeline "*"
	objTextFile.Writeline "* ------ OS Version Information displayed below. -------"
	objTextFile.Writeline "!version"
	objTextFile.Writeline "* ------------------------------------------------------"
	objTextFile.Writeline "*"
	objTextFile.Writeline "*"
	objTextFile.Writeline "*"

	' Part 1 - printing the configuration settings to the debugger log file

	n = UBound(g_PreCommands)
	if n>0 or g_PreCommands(0) <> "" Then
		objTextFile.Writeline "* ------ Initialization Commands ----"
		For i=0 to n
			strAux1 = Trim(g_PreCommands(i))
			if strAux1 <> "" Then
				objTextFile.writeline "*  " & strAux1

			End If
		Next
	End If

	n = UBound(g_PostCommands)
	if n>0 OR g_PostCommands(0) <> "" Then
		objTextFile.Writeline "* ------ Termination Commands ----"
		For i=0 to n
			strAux1 = Trim(g_PostCommands(i))
			if strAux1 <> "" Then
				objTextFile.writeline "*  " & strAux1
			End If
		Next
	End If


	' Spawning a new process (only printing info)
	if Trim(SpawningCommand) <> "" Then
		objTextFile.Writeline "* ------ Spawning a new debugee ----"		
		objTextFile.Writeline "* " & SpawningCommand		
		objTextFile.Writeline "* ----------------------------------"		
	End If

	' printing specific configurations for each run mode
	If g_RunMode = "CRASH" Then

		objTextFile.Writeline "* "
		objTextFile.Writeline "* ------ Current Actions for Exceptions ----"
		n = UBound(g_ExceptionActions)
		For i=0 to n
			if g_ExceptionActions (i).ExceptionCode <> "" Then
				objTextFile.writeline "*  " & g_ExceptionActions (i).ExceptionName & " [" & g_ExceptionActions (i).ExceptionCode & "]    " & "   return: " & g_ExceptionActions (i).ReturnAction1 & " " & g_ExceptionActions (i).ReturnAction2

				strAux1 = g_ExceptionActions (i).Commands1 & " " & g_ExceptionActions (i).CustomCommands1
				objTextFile.writeline "*      1st chance: " & strAux1

				strAux2 = g_ExceptionActions (i).Commands2 & " " & g_ExceptionActions (i).CustomCommands2
				objTextFile.writeline "*      2nd chance: " & strAux2

			End If
		Next

		n = UBound(g_BPActions)
		if n>0 or g_BPActions(0).Address <> "" Then
			objTextFile.Writeline "* "
			objTextFile.Writeline "* ------ Current Actions for Breakpoints ----"		
			For i=0 to n
				if g_BPActions(i).Address <> "" Then
					objTextFile.writeline "*" & "  Type: " & g_BPActions(i).AdrType & "  Address: " & g_BPActions(i).Address & "   return: " & g_BPActions(i).ReturnAction 

					strAux1 = g_BPActions(i).Commands & " " & g_BPActions(i).CustomCommands
					objTextFile.writeline "*      Actions: " & strAux1
				End If
			Next
		End If

		' printing the last script command
		objTextFile.Writeline "* "
		objTextFile.Writeline "* ------ Last Script Command: " & g_LastScriptCommand	



	Else 'We are in hang mode . . . 

		objTextFile.Writeline "* "
		objTextFile.Writeline "* ------ Current Actions for Hang Mode ----"
		
		n = UBound(g_HangActions)
		For i=0 to n
			if g_HangActions (i).Selected = True And (g_HangActions (i).Name <> "" or g_HangActions (i).Command <> "") Then

				strAux1 = ""
				if g_HangActions (i).Name <> "" Then
					strAux1 = strAux1 & "Name: " & g_HangActions (i).Name
				End If
				if g_HangActions (i).Command <> "" Then
					strAux1 = strAux1 & "Command: " & g_HangActions (i).Command
				End If

				objTextFile.writeline "*  " & strAux1
			End If
		Next

	End If


	' Part 2 - Executing commands

	' Defining an alias for the output directories
	strAux1 = "as AdpOutputDir " & g_OutPutDir
	objTextFile.Writeline strAux1
	strAux1 = "as AdpDumpDir " & g_DumpDir
	objTextFile.Writeline strAux1


	' defining the symbol path
	if g_Sympath <> "" or g_SympathPlus <> "" Then

		objTextFile.Writeline "* ------ Defining the symbol path ----"
		
		if g_Sympath <> "" Then
			strAux1 = ".sympath " & g_Sympath
			objTextFile.Writeline strAux1
		End If
		if g_SympathPlus <> "" Then
			strAux1 = ".sympath+ " & g_SympathPlus
			objTextFile.Writeline strAux1
		End If
		strAux1 = ".reload"
		objTextFile.Writeline strAux1
	End If
	objTextFile.Writeline "* ------ Current symbol path ----"
	objTextFile.Writeline ".sympath"

	' Creating pre-commands
	n = UBound(g_PreCommands)
	For i=0 to n
		strAux1 = Trim(g_PreCommands(i))
		if strAux1 <> "" Then
			objTextFile.Writeline strAux1
		End If
	Next

	' commands specific to each run mode
	If g_RunMode = "CRASH" Then

		objTextFile.Writeline ".echotimestamps"

                ' creating commands for all exceptions and breakpoints

		n = UBound(g_ExceptionActions)
		For i=0 to n
			if g_ExceptionActions (i).ExceptionCode <> "" Then
				strAux2 = CreateExceptionAction(g_ExceptionActions (i), pid, packagename)
				if Len(strAux2)>=2000 Then
					AdpLog "Comments truncated for the [" & g_ExceptionActions (i).ExceptionCode & "]"
					AdpLog "The final cdb command exceeded 2000 characters"
					g_ExceptionActions (i).Comment = ""
					strAux2 = CreateExceptionAction(g_ExceptionActions (i), pid, packagename)
				End If

				objTextFile.Writeline 	strAux2
				' checking for size limit for the string
				if Len(strAux2)>=2000 Then
					strAux = "The command size for the exception [" & g_ExceptionActions(i).ExceptionName & "] is [" & Len(strAux2) & "] and exceds the allowed limit. You can reduce it's size by using an output directory with a shorter name"
					QuitWithError strAux
				End If
			End If
		Next

		' creating commands for breakpoints
		n = UBound(g_BPActions)
		For i=0 to n
			strAux2 = CreateBPAction(g_BPActions (i), pid, packagename)
			if Trim(strAux2) <> "" Then
				objTextFile.Writeline 	strAux2
				' checking for size limit for the string
				if Len(strAux2)>=1024 Then
					strAux = "The command size for the breakpoint [" & g_BPActions(i).Address & "] is [" & Len(strAux) & "] and exceds the allowed limit. You can reduce it's size by using an output directory with a shorter name"
					QuitWithError strAux
				End If
			End If
		Next




	
	Else 'We are in hang mode . . . 


		n = UBound(g_HangActions)
		for i=0 to n
			' converting HangAction.Name into cdb commands
			strAux = g_HangActions(i).Name
			if strAux <> "" Then
				if strAux = "MiniDump" Then
					g_HangActions(i).Command = ".dump -u /mdi "
					' comment
					g_HangActions(i).Command = g_HangActions(i).Command & "/c " & Chr(34) & "Mini dump in Hang Mode for " & packagename & "_running_on_" & g_ComputerName & Chr(34)
					g_HangActions(i).Command = g_HangActions(i).Command & " "
					' output filename
					g_HangActions(i).Command = g_HangActions(i).Command & g_DumpDir & "\PID-" & pid & "__" & packagename & "__mini.dmp"

				elseif strAux = "FullDump" Then
					g_HangActions(i).Command = ".dump -u /ma "
					' comment
					g_HangActions(i).Command = g_HangActions(i).Command & "/c " & Chr(34) & "Full dump in Hang Mode for " & packagename & "_running_on_" & g_ComputerName & Chr(34)
					g_HangActions(i).Command = g_HangActions(i).Command & " "
					' output filename
					g_HangActions(i).Command = g_HangActions(i).Command & g_DumpDir & "\PID-" & pid & "__" & packagename & "__full.dmp"

				elseif strAux = "Stacks" Then
					g_HangActions(i).Command = "~*kb250"

				elseif strAux = "LoadedModules" Then
					g_HangActions(i).Command = "lmv"

				elseif strAux = "MatchingSymbols" Then
					g_HangActions(i).Command = "lml"

				elseif strAux = "Heap" Then
					g_HangActions(i).Command = "!heap 0 -k"

				elseif strAux = "Handle" Then
					g_HangActions(i).Command = "!handle 0 0"

				elseif strAux = "Dlls" Then
					g_HangActions(i).Command = "!dlls"

				elseif strAux = "Locks" Then
					g_HangActions(i).Command = "!locks"

				elseif strAux = "ThreadUsage" Then
					g_HangActions(i).Command = "!runaway"
				else
					strAux = "Invalid command name in Hang Mode Actions: [" & g_HangActions(i).Name & "]"
					QuitWithError strAux				
				End If
				
			End if
			
			' executing the command
			if g_HangActions(i).Command <> "" And g_HangActions(i).Selected=True Then
				' creating the comment line
				strAux = ""
				if g_HangActions(i).Description <> "" Then
					strAux = "------ " & g_HangActions(i).Description & " ------"
				Else
					if g_HangActions(i).Name <> "" Then
						strAux = "------ Executing " & g_HangActions(i).Name & " ------"
					Else
						strAux = "------ Executing " & g_HangActions(i).Command & " ------"
					End if
				End If

				objTextFile.Writeline "*"
				objTextFile.Writeline "* " &  strAux

				' adding the command itself
				objTextFile.Writeline g_HangActions(i).Command

			End If 
		Next


	End If
	
	' part 3 - common closing commands

	' creating commands for post commands
	n = UBound(g_PostCommands)
	For i=0 to n
		strAux1 = Trim(g_PostCommands(i))
		if strAux1 <> "" Then
			objTextFile.Writeline strAux1
		End If
	Next

	' part 4 - specific closing commands

	If g_RunMode = "CRASH" Then

		objTextFile.Writeline "*"
		objTextFile.Writeline "* ADPlus is monitoring: " & packagename 
		objTextFile.Writeline "* for  1st chance and 2nd chance exceptions as configured above."
		objTextFile.Writeline "* To change ADPlus configuration please refer to ADPlus.Doc. This file can be found"
		objTextFile.Writeline "* in the same folder as ADPlus.vbs"
		objTextFile.Writeline "*"
		objTextFile.Writeline "g"
		objTextFile.Writeline "*"

		' checking special condition with $t19=99
		' we do this because bpe doesn't accept GH, so we are unable to do induced dumps
		' instead of a GH I do a $t19=99 and I check it out here
		' DECIDED NOT TO DO THIS as it will work only for the first CTL+C
		'objTextFile.Writeline "j (@$t19=99) 'r $t19=0; gh'"

		' continuing normal finish
		objTextFile.Writeline "*"
		objTextFile.Writeline "*"
		objTextFile.Writeline "* -------- ADPlus " & VERSION & " finished running at: --------"
		objTextFile.Writeline ".time"
		objTextFile.Writeline "* -------------------------------------------------------"

		'If the -notify switch was used then send a message to the computer or user specified on the command line
		'Note on NT 4.0 LocalUserName will be blank because the Volatile Environment variable can only be quired on Windows 2000 to get the 
		'local username.  So we must send the message from the NotifyTarget on NT 4.0, instead of the locally logged on user.
		n = UBound(g_NotifyList)
		for i=0 to n
			if g_NotifyList(i)<>"" Then
				If g_OSVer = "4.0" Then
					objTextFile.Writeline "!net_send " & g_ComputerName & " " & g_NotifyList(i) & " " & g_NotifyList(i) & " ADPlus has finished running in crash mode on " & g_ComputerName & " for the following process: " & packagename & ".  This could indicate that a crash has occurred, or that the debugging session was ended manually (CTRL-C was pressed to stop debugging).  Please check the application event log on " & g_ComputerName & " for more information."
				Else
					objTextFile.Writeline "!net_send " & g_ComputerName & " " & g_NotifyList(i) & " " & g_LocalUserName & " ADPlus has finished running in crash mode on " & g_ComputerName & " for the following process: " & packagename & ".  This could indicate that a crash has occurred, or that the debugging session was ended manually (CTRL-C was pressed to stop debugging).  Please check the application event log on " & g_ComputerName & " for more information."
				End If				
			End If
		Next
		if UCase(g_Debugger) <> "WINDBG" AND UCase(g_Debugger) <> "WINDBG.EXE" Then
			objTextFile.Writeline "* -- Ending the script with a QUIT  --"
			objTextFile.Writeline g_LastScriptCommand  ' was "q"
	
		End If

	Else 'We are in hang mode . . . 

		' closing
		objTextFile.Writeline ".logclose"
		objTextFile.Writeline "q"

	End If

	' closing the script file

	objTextFile.Close
	Set objTextFile = nothing
	CreateCDBScript = strFile

End Function




'---------------------------------------------------------------------------------------------
' Function:  ShowStatus
'            This is the code used to display the pop-up summary at the end of ADPlus.
'---------------------------------------------------------------------------------------------
Sub ShowStatus()

	AdpTrace "In the ShowStatus() function "
	
	Dim versionFile
	Dim Arg, objArgs, argString
	Dim i,j,n
	Dim strAux1, strAux2 
	Dim strAux

	If g_QuietMode = true Then
		strAux = "ADPlus is now running in " & g_RunMode & " mode with the '-quiet' switch, and was started at: " & g_DateTimeStamp & "."
		g_objShell.LogEvent 0, strAux
	Else
		strAux = "ADPlus is now running in " & g_RunMode & " mode and is logging information for all of the threads in the processes you have chosen to examine. "
		strAux = strAux & "You will see one minimized command shell window for each of these processes. "
		strAux = strAux & "ADPlus is finished running when these windows disappear. "
		strAux = strAux & "After ADPlus completes, please check the " & g_DumpDir & " directory to verify the log files for each process were created!"
		g_objShell.popup strAux
	End If

	
		
		
		




			

End Sub





'---------------------------------------------------------------------------------------------
' Function:  PrintUsage
'            This is the code used to print the usage information.
'---------------------------------------------------------------------------------------------
Sub PrintUsage()
	AdpTrace "In the PrintUsage() function "

	
		Wscript.Echo ""
		Wscript.Echo "ADPlus " & VERSION & " Usage Information"
		Wscript.Echo ""
		Wscript.Echo " Command line switches"
		Wscript.Echo ""
		Wscript.Echo "-Crash    Runs ADPlus in Crash mode"
		Wscript.Echo "-Hang     Runs ADPlus in Hang mode"
		Wscript.Echo "-p <PID>  Defines a Process ID to be monitored"
		Wscript.Echo "-pn <ProcessName> Defines a process name to be monitored"
		Wscript.Echo "-sc <spawning command> Defines the application and parameters to be started"
		Wscript.Echo "                       in the debugger"
		Wscript.Echo "-iis      All iis related processes will be monitored (inetinfo, dllhost,"
		Wscript.Echo "              mtx, etc.)"
		Wscript.Echo "-o <output directory>   Defines the directory where logs and dumps are"
		Wscript.Echo "                         to be placed."
		Wscript.Echo "-quiet    No dialog boxes will be displayed"
		Wscript.Echo "-notify <destination>  Will send a message to the destination"
		Wscript.Echo ""
		Wscript.Echo "-c <config file name>   Defines a configuration file to be used"
		Wscript.Echo ""
		Wscript.Echo "-ce <custom exception code>   Defines a custom exception to be monitored"
		Wscript.Echo "                 -ce 0x80501001"
		Wscript.Echo ""
		Wscript.Echo "-bp <breakpoint parameters>   Sets a breakpoint"
		Wscript.Echo "      Syntax: -bp address;optional_additional_parameters"
		Wscript.Echo "                   -bp MyModule!MyClass::MyMethod"
		Wscript.Echo "                   -bp MyModule!MyClass::MyMethod;MiniDump"
		Wscript.Echo ""
		Wscript.Echo "-y <symbol path>   Defines the symbol path to be used"
		Wscript.Echo "-yp <symbol path to add>   Defines an additional symbol path"
		Wscript.Echo ""
		Wscript.Echo "-FullOnFirst   Sets ADPlus to create full dumps on first chance exceptions"
		Wscript.Echo "-FullOnFirstOver   Sets ADPlus to create full dumps on first chance exceptions, "
		Wscript.Echo "                   overwriting the previous dump"
		Wscript.Echo "-MiniOnSecond  Sets ADPlus to create mini dumps on second chance exceptions"
		Wscript.Echo "-NoDumpOnFirst Sets ADPlus to not create any dumps on first chance exceptions"
		Wscript.Echo "-NoDumpOnSecond  Sets ADPlus to not create any dumps on second chance exceptions"
		Wscript.Echo "-do Dump Only - changes default behavior to not include additional info, just a dump"
		Wscript.Echo "-CTCF  Creates a full dump on CTL+C, and quits"
		Wscript.Echo "-CTCFG  Creates a full dump on CTL+C, and resumes execution"
		Wscript.Echo "-CTCL  Creates only a Log on CTL+C, use together with -lc?? commands"
		Wscript.Echo "-lcq  sets the last script command to Q (quit)"
		Wscript.Echo "-lcg  sets the last script command to G (go)"
		Wscript.Echo "-lcgn  sets the last script command to GN (go not handled)"
		Wscript.Echo "-lcqd  sets the last script command to QD (quit and detach)"
		Wscript.Echo "-lcv  sets the last script command to void (no command; waits for user input)"
		Wscript.Echo "-NoTlist  Will not use TList; only -p can be used (-pn and -iis will not work)"
		Wscript.Echo "-dbg <debugger>  Allows you to select the debugger to be used"
		Wscript.Echo "                    cdb, windbg or ntsd  (default is cdb)"
		Wscript.Echo ""
		Wscript.Echo ""
		Wscript.Echo "-r <quantity> <interval in seconds> for multiple attachments in hang mode"
		Wscript.Echo "-gs <filename>   only generates the script file"
		Wscript.Echo ""
		Wscript.Echo ""
		Wscript.Echo "Required: ('-hang', or '-crash') AND ('-iis' or '-p' or '-pn')"
		Wscript.Echo "   If using a config file (-c switch) the required switches above can be    "
		Wscript.Echo "   provided in the config file or in the command line"
		Wscript.Echo ""
		Wscript.Echo " The -sc switch, if used, must be the last one"
		Wscript.Echo ""
		Wscript.Echo ""
		Wscript.Echo "Examples: 'ADPlus -hang -iis',     Produces memory dumps of IIS and all "
		Wscript.Echo "                                   MTS/COM+ packages currently running."
		Wscript.ECho ""
		Wscript.Echo "          'ADPlus -crash -p 1896', Attaches the debugger to process with PID"
		Wscript.Echo "                                   1896, and monitors it for 1st and 2nd"
		Wscript.Echo "                                   chance access violations (crashes)."
		Wscript.Echo ""
		Wscript.Echo ""
		Wscript.Echo "          'ADPlus -?' or 'ADPlus -help':  Displays this information."
		Wscript.Echo ""
		Wscript.Echo "-------------------------------------------------------------------------------"
		Wscript.Echo ""
		Wscript.Echo "  HELP and Documentation"
		Wscript.Echo ""
		Wscript.Echo "  For more detailed information on how to use and config ADPlus please see"
		Wscript.Echo "  the debugger's help file (debugger.chm) under"
		Wscript.Echo ""
		Wscript.Echo "    Using Debugging Tools for Windows"
		Wscript.Echo "        Crash dumps"
		Wscript.Echo "           User mode dump files"
		Wscript.Echo "               Creating a user mode dump file"
		Wscript.Echo "                  ADPlus"
		Wscript.Echo ""
		Wscript.Echo "-------------------------------------------------------------------------------"
		Wscript.Echo "For more information on using ADPlus, please refer to the following KB:"
		Wscript.Echo "http://support.microsoft.com/support/kb/articles/q286/3/50.asp"
		Wscript.quit 1
End Sub





'---------------------------------------------------------------------------------------------
' Function:  CreateDirectory()
'            This is the code used to create a directory on the file system or the network
'	     It checks to make sure you have proper permissions and displays/logs an error
'            if you don't.
'---------------------------------------------------------------------------------------------
Sub CreateDirectory(strDirectory)


	AdpTrace "In the CreateDirectory() function "

	On Error Resume Next

	Dim oFile
'###IB??? Should we check if it already exists???
	Set oFile = g_objFSO.CreateFolder(strDirectory)

	If err.number = -2147024784 Then 'The disk is full!
		
		QuitWithError "Could not create the " & strDirectory & " directory because the drive is full.  Please free up some disk space and try running ADPlus again!"

	ElseIf err.number = 76 Then 'Specified a subdirectory below a share that does not exist or path with more than one subdirectory that does not exist!
		QuitWithError "Could not create the " & strDirectory & " directory.  If you are using the '-o' switch and specifying a UNC path, please ensure that the share and any sub-directories below the share you specified actually exist!"

	ElseIf err.number = 52 Then 'The share does not exist or can not connect to the UNC path.
		QuitWithError "Could not create the " & strDirectory & " directory.  If you are using the '-o' switch and specifying a UNC path, please ensure that the UNC path you specified exists and that you are able to connect to it!"

	ElseIf err.number <> 0 and err.number <> 58 Then 'Permissions problem
		QuitWithError "Could not create the " & strDirectory & " directory.  Please verify that you have permissions to create this directory and try running ADPlus again!"
		
	End If	
	
	Set oFile = nothing
	On Error GoTo 0

End Sub

'---------------------------------------------------------------------------------------------
' Function:  DetectScriptEngine()
'            This is the used to determine what script engine is currently running the script.
'	     It will prompt the user to switch the default script engine to cscript.exe if
'            the script is running under wscript.exe.
'---------------------------------------------------------------------------------------------
Sub DetectScriptEngine ()

	Dim ScriptHost
	Dim CurrentPathExt
	Dim EnvObject
	Dim RegCScript
	Dim RegPopupType ' This is used to set the pop-up box flags.  
						' I couldn't find the pre-defined names
	Dim ProcString
	Dim ArgIndex
	Dim ArgObj
	Dim Result
	Dim StrShell

	AdpTrace "In the DetectScriptEngine() function "

	On Error Resume Next

	RegPopupType = 32 + 4


	AdpLog "Script engine Version: " & WScript.Version & "." & WScript.BuildVersion

	ScriptHost = WScript.FullName
	ScriptHost = Right (ScriptHost, Len (ScriptHost) - InStrRev(ScriptHost,"\"))


	If (UCase (ScriptHost) = "WSCRIPT.EXE") Then
		
		' Create a pop-up box and ask if they want to register cscript as the default host.
		' -1 is the time to wait.  0 means wait forever.

		If g_QuietMode = False Then
			RegCScript = g_objShell.PopUp ("Wscript.exe is currently your default script interpreter.  This script requires the Cscript.exe script interpreter to work properly.  Would you like to register Cscript.exe as your default script interpreter for VBscript?", 0, "Register Cscript.exe as default script interpreter?", RegPopupType)
		Else 'Running in Quiet mode, assume the user would WANT to change their script default script interpreter if they were prompted.
			RegCscript = 6
		End If 
                                                                                
		If (Err.Number <> 0) Then
			AdpTrace "Error in the DetectScriptEngine (1)"
			AdpLog "Error in the DetectScriptEngine (1)"
			WScript.Echo "To run this script using the CScript.exe script interpreter, type: ""CScript.exe " & WScript.ScriptName & """"
			QuitWithError "Error in the DetectScriptEngine (1)"			
		End If

		' Check to see if the user pressed yes or no.  Yes is 6, no is 7
		If (RegCScript = 6) Then
			g_objShell.RegWrite "HKEY_CLASSES_ROOT\VBSFile\Shell\Open\Command\", "%WINDIR%\System32\CScript.exe //nologo ""%1"" %*", "REG_EXPAND_SZ"
			if err.number<>0 Then
				QuitWithError "Error writing to registry(1):  Error: " & err.number & " - " & err.description
			End If

			g_objShell.RegWrite "HKEY_LOCAL_MACHINE\SOFTWARE\Classes\VBSFile\Shell\Open\Command\", "%WINDIR%\System32\CScript.exe //nologo ""%1"" %*", "REG_EXPAND_SZ"
			if err.number<>0 Then
				QuitWithError "Error writing to registry(2):  Error: " & err.number & " - " & err.description
			End If

			' Check if PathExt already existed
			CurrentPathExt = g_objShell.RegRead ("HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\Session Manager\Environment\PATHEXT")
			if Err.Number = &h80070002 Then
				Err.Clear
				Set EnvObject = g_objShell.Environment ("PROCESS")
				CurrentPathExt = EnvObject.Item ("PATHEXT")
			End If

			g_objShell.RegWrite "HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\Session Manager\Environment\PATHEXT", CurrentPathExt & ";.VBS", "REG_SZ"

			If (Err.Number <> 0) Then
				QuitWithError "Error trying to write the registry settings!  The error number was: " & Err.Number & " and the error description was: " & err.description
			End If

			' Default scripting host changed succesfully
			AdpTrace "Default scripting host changed succesfully"
			AdpLog "Default scripting host changed succesfully"

			If g_QuietMode = True Then
				g_objShell.Logevent 0, "Since ADPlus is running in 'Quiet' mode, the default script interpreter was automatically changed to CScript to ensure compatibility with this script.  ADPlus will now open a new command shell window and run ADPlus.vbs again with the argument passsed in on the command line.  To change the default script engine back to Wscript, type 'wscript.exe //h:wscript' in a command shell."
			Else
				WScript.Echo "Successfully registered Cscript.exe as the default script interpreter!  To change the default script engine back to Wscript, type 'wscript.exe //h:wscript' in a command shell."
			End If
			
		Else 'The user does not want the script engine changed and didn't use the '-quiet' switch.
			WScript.Echo "The default script interpreter was NOT changed to CScript.  Press 'OK' to continue running ADPlus with the Cscript.exe script interpreter.  NOTE:  ADPlus will now open a new command shell to run ADPlus.vbs with the CScript script engine.  A new command shell will be opened each time ADPlus is run until the default script interpreter is changed to Cscript.exe."
		End If

		On Error GoTo 0

		' Launching a new instance of adplus.vbs


		ProcString = "Cscript //nologo " & Chr(34) & WScript.ScriptFullName & Chr(34)
	
		AdpTrace "In DetectScriptEngine(), the ProcString = " & ProcString

		Set ArgObj = WScript.Arguments
	


		For ArgIndex = 0 To ArgObj.Count - 1
			ProcString = ProcString & " " & ArgObj.Item (ArgIndex)
		Next

		strShell = "cmd /k " & ProcString

		' closing all open files before running a new script
		AdpTrace "Launching a new instance of adplus.vbs"
		AdpLog "Launching a new instance of adplus.vbs"

		CloseAllFiles
		g_objShell.Run strShell,ACTIVATE_AND_DISPLAY,TRUE
		
		WScript.Quit 1
	End If

End Sub





'---------------------------------------------------------------------------------------------
' Function:  DetectOSVersion
'            This function is responsible for determining which operating system we are   
'            running on and to ensure its NT 4.0 SP4 or higher.
'---------------------------------------------------------------------------------------------
Sub DetectOSVersion()

	AdpTrace "In the DetectOSVersion() function "

'	Dim SPLevel
	Dim Temp, cnt
	
	On Error Resume Next
	
	g_OSVer = g_objShell.RegRead("HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\CurrentVersion")
	if err.number<>0 Then
		QuitWithError "There was an error trying to read the OS version information from the registry!  The error number was: " & Err.Number & " and the error description was: " & err.description & "  Please check the permissions on the key shown and try running ADPlus again."
	End If

	g_OSBuildNumber = g_objShell.RegRead("HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\CurrentBuildNumber")
	If (Err.Number <> 0) Then
		QuitWithError "There was an error trying to read the OS version information from the registry!  The error number was: " & Err.Number & " and the error description was: " & err.description & "  Please check the permissions on the key shown and try running ADPlus again."
	End If

'		Commented out for version 5.0 . . . too many problems trying to detect the SPLevel with various hotfixes for NT4
'		If g_OSVer = "4.0" then
'			SPLevel = g_objShell.RegRead("HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\CSDVersion")
'			AdpTrace "In DetectOSVersin(), SPLevel = " & SPLevel
'			Temp = Split(SPLevel,",")
'			If UBound(Temp) <> 0 Then 'The customer must not have a released service pack, make a best effor to get the version.
'				'Wscript.echo " Temp: " & Temp(0) & Temp(1)
'				
'				For cnt = 0 to UBound(Temp)
'				   If instr(1,Temp(cnt),"sp",1) <> 0 then
'				       SPLevel = Right(Temp(cnt), 1)
'				       'Wscript.echo "SPlevel = " & SPlevel
'				       exit for
'				   else
'				       'continue to the next string in the list.....
'				   End if
'				
'				Next
'			
'			Else
'			'Customer has a released version of the service pack, get the SP number.
'			SPLevel = Right(SPLevel, 1)
'
'			End If
'			
'			If SPLevel < 4 then
'				Wscript.echo "ADPlus.vbs requires NT 4.0 Service Pack 4 or higher, or Windows 2000 in order to run."
'				Wscript.quit 1
'			end if
'		End if


	On Error GoTo 0

	AdpLog "OS Version: " & g_OSVer
	AdpLog "OS Build: " & g_OSBuildNumber

End sub


'---------------------------------------------------------------------------------------------
' Function:  DetectIISVersion
'            This function is responsible for determining which version of IIS we are   
'            running on.  Since the '-iis' switch does not work on IIS 3.0, ADPlus
'	     checks for IIS 3.0 or lower in the registry and does not allow the switch to be
'	     used.  If the version of IIS is greater than 5.x, ADPlus also does not 
'            allow the switch to be used as IIS 6.0 was not tested in time for this release.
'---------------------------------------------------------------------------------------------
Sub DetectIISVersion()

	AdpTrace "In the DetectIISVersion() function "


	Dim IIS4Key
	Dim IIS3Key
	
	IIS4Key = "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\InetStp\VersionString"
	IIS3Key = "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Inetsrv\CurrentVersion\ServiceName"


	On Error Resume Next

	'Check the IIS 4.0 and 5.x registry key to see if the user is running IIS 4.0 or 5.0
	g_IISVer = g_objShell.RegRead(IIS4Key)
	If err.number = -2147024894 Then 'The key was not found, check for IIS 3.0

		err.clear
		g_IISVer = g_objShell.RegRead(IIS3Key)
		If err.number = -2147024891 Then
			'ADPlus doesn't have permissions to read IIS 3.0's registry version key.
			QuitWithError "You appear to be running IIS 3.0 or lower, however ADPlus had trouble verifying the IIS version in the registry at the following location: '" & IIS3Key & "', because of a permissions problem.  The '-IIS' switch only works on IIS 4.0 or higher.  If you are trying to monitor IIS 2.0 or IIS 3.0 please using the '-pn' switch to monitor inetinfo.exe."
		ElseIf err.number = -2147024894 Then
			'IIS 3.0 or higher is not installed.
			QuitWithError "You have specified the '-iis' switch, however you do not appear to be running a supported version of IIS.  The '-iis' switch only works with IIS 4.0 and higher.",,"ADPlus",0
		ElseIf err.number = 0 Then
			'IIS 3.0 is installed and the version key can be read.
			QuitWithError "You have specified the '-iis' switch, however you appear to be running " & g_IISVer & ".  The '-iis' switch only works with IIS 4.0 and higher.",,"ADPlus",0
		Else 
			'Some other error has occurred accessing the version key's.  Display an error and exit gracefully.
			QuitWithError "There was an error reading the registry. Unable to detect IIS version. The error number was: " & err.number & " and the error description was: " & err.description
		End If
	ElseIf err.number = -2147024891 Then 'They are running IIS 4.0 or 5.x (the key was found) but they don't have permissions
		QuitWithError "You appear to be running IIS 4.0 or higher, however ADPlus had trouble verifying the IIS version in the registry at the following location: '" & IIS4Key & "', because of a permissions problem.  The '-IIS' switch only works on IIS 4.0 or higher.  If you are trying to monitor IIS 2.0 or IIS 3.0 please using the '-pn' switch to monitor inetinfo.exe."
	ElseIf err.number <> 0 Then
		QuitWithError "There was an error reading the registry. Unable to detect IIS version. " & err.number & " " & err.Description
	End If

	On Error GoTo 0


	' checking if version is higher than 6
	If InStr(g_IISVer,"4") <= 0 And InStr(g_IISVer,"5") <=0 And InStr(g_IISVer,"6") <= 0 Then
		'ADPlus has detected a newer version of IIS that has not been tested.  Display an error and disable the '-iis' switch to be safe.

		QuitWithError "You appear to have IIS " & g_IISVer & " installed.  This has not been tested with the '-iis' switch.  Please run ADPlus again, without the '-iis' switch."			
	End If
		
		


	AdpLog "IIS Version: " & g_IISVer

End sub

'---------------------------------------------------------------------------------------------
' Function:  GetArguments
'
'            Created by Israel Burman on 5/30/2002
'
'            Reading the parameters provided by the user in the command line
'
'            Returns: A string with error messages found during parsing the parameters
'            
'---------------------------------------------------------------------------------------------
Function GetArguments()

	AdpTrace "In the GetArguments() function . . ."

	dim objArgs
	dim ArgsCount
	dim strMsgErr
	dim Lf ' Cr & Line Feed
	dim strAux ' temporary string
	dim x ' arguments counter
	dim i,n


	strMsgErr = ""
	Lf = Chr(13) & Chr(10)

	Set objArgs = Wscript.Arguments
	ArgsCount = objArgs.count

	AdpLog " "
	AdpLog "Command line arguments used were: "
	strAux = ""
	if ArgsCount>0 Then
		For i=0 to ArgsCount-1
			strAux = strAux & CStr(objArgs(i)) & " "
		Next
	End If
	AdpLog strAux

	If ArgsCount > 0 Then
		x = 0
		while x<ArgsCount
				If UCase(objArgs(x)) = "-QUIET" Then
					g_QuietMode = True
				ElseIf UCase(objArgs(x)) = "-DO" Then
					g_DumpOnly = True
					DumpOnlySelected
				ElseIf UCase(objArgs(x)) = "-HANG" Then
					if g_RunMode <> "" Then
						strMsgErr = strMsgErr & Lf & "Run mode redefined - only one of [-CRASH, -HANG, -QUICK] allowed"
					End If
					g_RunMode = "HANG"
				ElseIf UCase(objArgs(x)) = "-QUICK" Then
					if g_RunMode <> "" Then
						strMsgErr = strMsgErr & Lf & "Run mode redefined - only one of [-CRASH, -HANG, -QUICK] allowed"
					End If
					g_RunMode = "QUICK"
				ElseIf	UCase(objArgs(x)) = "-CRASH" Then
					if g_RunMode <> "" Then
						strMsgErr = strMsgErr & Lf & "Run mode redefined - only one of [-CRASH, -HANG, -QUICK] allowed"
					End If
					g_RunMode = "CRASH"
				ElseIf	UCase(objArgs(x)) = "-NOTIFY" Then
					if x >= ArgsCount-1 Then
						strMsgErr = strMsgErr & Lf & "Expecting additional parameters in command line"
					Else
						x = x + 1					
						strAux = objArgs(x)
						If Left(strAux,1) = "-"  Then
							strMsgErr = strMsgErr & Lf & "No computer name or username was specified after the '-notify' switch"
						Else
							n = UBound(g_NotifyList) + 1
							Redim Preserve g_NotifyList(n)
							g_NotifyList(n) = objArgs(x)
						End If
					End If

				ElseIf	UCase(objArgs(x)) = "-LCQ" Then
					g_LastScriptCommand = "Q"
				ElseIf	UCase(objArgs(x)) = "-LCG" Then
					g_LastScriptCommand = "G"
				ElseIf	UCase(objArgs(x)) = "-LCGN" Then
					g_LastScriptCommand = "GN"
				ElseIf	UCase(objArgs(x)) = "-LCQD" Then
					g_LastScriptCommand = "QD"
				ElseIf	UCase(objArgs(x)) = "-LCV" Then
					g_LastScriptCommand = ""

				ElseIf	UCase(objArgs(x)) = "-?" Then
					g_HelpRequested = True
				ElseIf	UCase(objArgs(x)) = "-HELP" Then
					g_HelpRequested = True
				' Defining a custom exception
				ElseIf	UCase(objArgs(x)) = "-CE" Then
					if x >= ArgsCount-1 Then
						strMsgErr = strMsgErr & Lf & "Expecting additional parameters in command line"
					Else
						x = x + 1					
						strAux = objArgs(x)
						If Left(strAux,1) = "-"  Then
							strMsgErr = strMsgErr & Lf & "No exception code was specified after the '-CE' switch"
						Else
							AddException objArgs(x), objArgs(x)
						End If
					End If
				' Defining a breakpoint
				ElseIf	UCase(objArgs(x)) = "-BP" Then
					if x >= ArgsCount-1 Then
						strMsgErr = strMsgErr & Lf & "Expecting additional parameters in command line"
					Else
						x = x + 1					
						strAux = objArgs(x)
						If Left(strAux,1) = "-"  Then
							strMsgErr = strMsgErr & Lf & "No breakpoint address was specified after the '-BP' switch"
						Else
							AddBreakPoint objArgs(x)
						End If
					End If
				' Defining a file with custom commands
				ElseIf	UCase(objArgs(x)) = "-C" Then
					if x >= ArgsCount-1 Then
						strMsgErr = strMsgErr & Lf & "Expecting additional parameters in command line"
					Else
						x = x + 1					
						strAux = objArgs(x)
						If Left(strAux,1) = "-"  Then
							strMsgErr = strMsgErr & Lf & "No file name or username was specified after the '-C' switch"
						Else
							strAux = objArgs(x)
							ReadCustomCommands strAux
						End If
					End If

				ElseIf	UCase(objArgs(x)) = "-CDH" Then
					strAux = g_InstallDir
					if Right(strAux,1) <> "\" Then 
						strAux = strAux & "\"
					End If
					strAux = strAux & "ADP_Default_Hang.cfg"
					ReadCustomCommands strAux

				ElseIf	UCase(objArgs(x)) = "-CDC" Then
					strAux = g_InstallDir
					if Right(strAux,1) <> "\" Then 
						strAux = strAux & "\"
					End If
					strAux = strAux & "ADP_Default_Crash.cfg"
					ReadCustomCommands strAux

				ElseIf	UCase(objArgs(x)) = "-CDJ" Then
					strAux = g_InstallDir
					if Right(strAux,1) <> "\" Then 
						strAux = strAux & "\"
					End If
					strAux = strAux & "ADP_Default_JIT.cfg"
					ReadCustomCommands strAux
				ElseIf	UCase(objArgs(x)) = "-IIS" Then
					g_IISMode = True
				ElseIf UCase(objArgs(x)) = "-O" Then
					if x >= ArgsCount-1 Then
						strMsgErr = strMsgErr & Lf & "Expecting additional parameters in command line"
					Else
						x = x + 1					
						strAux = objArgs(x)
						If Left(strAux,1) = "-"  Then
							strMsgErr = strMsgErr & Lf & "No valid directory or UNC path was specified after the '-o' switch."
						Else
							g_OutPutDir = objArgs(x)
						End If
					End If
	
				ElseIf UCase(objArgs(x)) = "-R" Then 'repetition parameters
					if x >= ArgsCount-1 Then
						strMsgErr = strMsgErr & Lf & "Expecting additional parameters in command line"
					Else
						x = x + 1					
						strAux = objArgs(x)
						If Left(strAux,1) = "-"  Then
							strMsgErr = strMsgErr & Lf & "No quantity of repeats was specified after the '-r' switch."
						Else
							g_AttachRepeats = objArgs(x)
						End If
					End If
					' reading the interval in seconds
					if x >= ArgsCount-1 Then
						strMsgErr = strMsgErr & Lf & "Expecting additional parameters in command line"
					Else
						x = x + 1					
						strAux = objArgs(x)
						If Left(strAux,1) = "-"  Then
							strMsgErr = strMsgErr & Lf & "No interval was specified after the '-r' switch."
						Else
							g_AttachInterval = objArgs(x)
						End If
					End If


				ElseIf UCase(objArgs(x)) = "-P" Then
					if x >= ArgsCount-1 Then
						strMsgErr = strMsgErr & Lf & "Expecting additional parameters in command line"
					Else
						x = x + 1					
						strAux = objArgs(x)
						If Left(strAux,1) = "-"  Then
							strMsgErr = strMsgErr & Lf & "No process ID was specified after the '-p' switch."
						Else
							AddProcessID objArgs(x)
						End If
					End If
	
				ElseIf	UCase(objArgs(x)) = "-PN" Then
					if x >= ArgsCount-1 Then
						strMsgErr = strMsgErr & Lf & "Expecting additional parameters in command line"
					Else	
						x = x + 1				
						strAux = objArgs(x)
						If Left(strAux,1) = "-"  Then
							strMsgErr = strMsgErr & Lf & "No process name was specified after the '-pn' switch."
						Else
							AddProcessName objArgs(x)
						End If
					End If
				ElseIf	UCase(objArgs(x)) = "-E" Then
					if x >= ArgsCount-1 Then
						strMsgErr = strMsgErr & Lf & "Expecting additional parameters in command line"
					Else
						x = x + 1					
						strAux = objArgs(x)
						If Left(strAux,1) = "-"  Then
							strMsgErr = strMsgErr & Lf & "No process name was specified after the '-e' switch."
						Else
							x = x + 1
							' this is simply passed to the debugger
							g_DebuggerEvent = objArgs(x)
						End If
					End If
				ElseIf	UCase(objArgs(x)) = "-SC" Then  ' spawn command
					' the rest of the line is the command
					if x >= ArgsCount-1 Then
						strMsgErr = strMsgErr & Lf & "Expecting additional parameters in command line"
					Else					
						strAux = ""
						for i = x+1 to ArgsCount-1
							strAux = strAux & objArgs(i) & " "
						Next
					End If
					AddSpawningCommand strAux

					AdpLog " "
					AdpLog " *** Warning *** "
					AdpLog " The -sc switch was used. This must be the last switch in the command line"
					AdpLog " If you used other switches following -sc, they were all ignored."
					AdpLog " *** End of warning ***"
					AdpLog " "

					x = ArgsCount ' to end the loop on x


				ElseIf	UCase(objArgs(x)) = "-FULLONFIRST" Then  
					n = UBound(g_ExceptionActions)
					For i=0 to n
						if IsExceptionSpecial(g_ExceptionActions(i).ExceptionCode) = False Then
							g_ExceptionActions(i).Commands1 = ReplaceDumpType(g_ExceptionActions(i).Commands1,"FullDump")
						End If
					Next

				ElseIf	UCase(objArgs(x)) = "-FULLONFIRSTOVER" Then  
					n = UBound(g_ExceptionActions)
					For i=0 to n
						if IsExceptionSpecial(g_ExceptionActions(i).ExceptionCode) = False Then
							g_ExceptionActions(i).Commands1 = ReplaceDumpType(g_ExceptionActions(i).Commands1,"FullDumpOver")
						End If
					Next


				ElseIf	UCase(objArgs(x)) = "-MINIONSECOND" Then  
					n = UBound(g_ExceptionActions)
					For i=0 to n
						if IsExceptionSpecial(g_ExceptionActions(i).ExceptionCode) = False Then
							g_ExceptionActions(i).Commands2 = ReplaceDumpType(g_ExceptionActions(i).Commands2,"MiniDump")
						End If
					Next

				ElseIf	UCase(objArgs(x)) = "-NODUMPONFIRST" Then  
					n = UBound(g_ExceptionActions)
					For i=0 to n
						if IsExceptionSpecial(g_ExceptionActions(i).ExceptionCode) = False Then
							g_ExceptionActions(i).Commands1 = ReplaceDumpType(g_ExceptionActions(i).Commands1,"NoDump")
						End If
					Next

				ElseIf	UCase(objArgs(x)) = "-NODUMPONSECOND" Then  
					n = UBound(g_ExceptionActions)
					For i=0 to n
						if IsExceptionSpecial(g_ExceptionActions(i).ExceptionCode) = False Then
							g_ExceptionActions(i).Commands2 = ReplaceDumpType(g_ExceptionActions(i).Commands2,"NoDump")
						End If
					Next


				ElseIf	UCase(objArgs(x)) = "-CTCF" Then  
					n = UBound(g_ExceptionActions)
					For i=0 to n
						if g_ExceptionActions(i).ExceptionCode = "bpe" Then
							g_ExceptionActions(i).Commands1 = ReplaceDumpType(g_ExceptionActions(i).Commands1,"FullDump")
						End If
					Next


				ElseIf	UCase(objArgs(x)) = "-CTCFG" Then  
					n = UBound(g_ExceptionActions)
					For i=0 to n
						if g_ExceptionActions(i).ExceptionCode = "bpe" Then
							g_ExceptionActions(i).Commands1 = ReplaceDumpType(g_ExceptionActions(i).Commands1,"FullDump")
							g_ExceptionActions(i).ReturnAction1 = "GH"
							' the above gets ignored; I need to set $t19=99
							'g_ExceptionActions(i).CustomCommands1 = "r $t19=99"
							' I make the last script command empty; users will have to do a GN manually
							g_LastScriptCommand = ""

						End If
					Next


				ElseIf	UCase(objArgs(x)) = "-CTCL" Then  
					n = UBound(g_ExceptionActions)
					For i=0 to n
						if g_ExceptionActions(i).ExceptionCode = "bpe" Then
							g_ExceptionActions(i).Commands1 = "Log"
							g_ExceptionActions(i).ReturnAction1 = "GH"
							' users should set g_LastScriptCommand accordingly
						End If
					Next


				ElseIf	UCase(objArgs(x)) = "-NOTLIST" Then
  					g_NoTlist = True

				ElseIf	UCase(objArgs(x)) = "-NOTSCHECK" Then
  					g_CheckTS = False

				ElseIf	UCase(objArgs(x)) = "-GS" Then
					if x >= ArgsCount-1 Then
						strMsgErr = strMsgErr & Lf & "Expecting additional parameters in command line"
					Else
						x = x + 1					
						strAux = objArgs(x)
						If Left(strAux,1) = "-"  Then
							strMsgErr = strMsgErr & Lf & "No script name was specified after the '-gs' switch."
						Else
							g_GeneratedScriptName = objArgs(x)
						End If
					End If
				ElseIf	UCase(objArgs(x)) = "-Y" Then
					if x >= ArgsCount-1 Then
						strMsgErr = strMsgErr & Lf & "Expecting additional parameters in command line"
					Else
						x = x + 1					
						strAux = objArgs(x)
						If Left(strAux,1) = "-"  Then
							strMsgErr = strMsgErr & Lf & "No symbol path was specified after the '-y' switch."
						Else
							g_Sympath = objArgs(x)
						End If
					End If
				ElseIf	UCase(objArgs(x)) = "-YP" Then
					if x >= ArgsCount-1 Then
						strMsgErr = strMsgErr & Lf & "Expecting additional parameters in command line"
					Else
						x = x + 1					
						strAux = objArgs(x)
						If Left(strAux,1) = "-"  Then
							strMsgErr = strMsgErr & Lf & "No symbol path was specified after the '-yp' switch."
						Else
							if g_SympathPlus <> "" Then 
								g_SympathPlus = g_SympathPlus & ";"
							End If
							g_SympathPlus = g_SympathPlus & objArgs(x)
						End If
					End If
				ElseIf	UCase(objArgs(x)) = "-DBG" Then
					if x >= ArgsCount-1 Then
						strMsgErr = strMsgErr & Lf & "Expecting additional parameters in command line"
					Else
						x = x + 1					
						strAux = objArgs(x)
						If Left(strAux,1) = "-"  Then
							strMsgErr = strMsgErr & Lf & "No debugger was specified after the '-dbg' switch."
						Else
							g_Debugger = objArgs(x)
						End If
					End If
				Else
					strMsgErr = strMsgErr & Lf & "Invalid command line parameter: [" & objArgs(x) & "]"
				End If

			x = x + 1
		Wend
	
	Else 'No arguments were typed, display the usage info . . .

		g_HelpRequested = TRUE
	End If

	GetArguments = strMsgErr

	AdpTrace "Exiting GetArguments() function . . ."

End FUnction

'---------------------------------------------------------------------------------------------
' Function:  GetCurrentProcesses
'
'    This function is responsible for running tlist.exe -v to get a list of all running processes

'    Reads the Process_List.txt file created by tlist and populates the
'      g_CurrentProcesses array

'            Created by Israel Burman on 5/30/2002
'
'---------------------------------------------------------------------------------------------
Sub GetCurrentProcesses()


	Dim objTextFile
	Dim FileName
	Dim PCount ' process counter
	Dim ArraySize ' UBound for g_CurrentProcesses
	Dim StrLength
	Dim strPackageName
	Dim i,k
	Dim strAux ' a temporary string
	Dim arrAux ' an array of strings
	Dim Lf ' Cr & Line Feed

	AdpTrace "In the GetCurrentProcesses() function "
	if g_NoTlist Then
		AdpTrace "g_NoTlist = True; TLIst not executed "
		AdpLog "g_NoTlist = True; TLIst not executed "
		Exit Sub
	End If

	Lf = Chr(13) & Chr(10)



	FileName = g_DumpDir & "\Process_List.txt"

	' running tlist
	strAux = "cmd /c " & "tlist.exe" & " -v >" & Chr(34) & g_DumpDir & "\Process_List.txt" & Chr(34)

	' we run tlist syncronously
	g_objShell.Run strAux,MINIMIZE_NOACTIVATE,TRUE
	



	'Per the scripting team's advice, sleep for .5 second prior to calling this function.
	Wscript.sleep 500

	' Checking if we can find Process_List.txt
	i = 0
	While not g_objFSO.FileExists(FileName) 
		Wscript.sleep 1000
		i = i +1
		If i = 10 Then

			strAux = "The file Process_List.txt could not be found in the " & g_DumpDir & " directory after waiting for 10 seconds."
			strAux = strAux & Lf & "This could indicate that there was a problem running tlist.exe or that the file could not be read using the Scripting.FileSystemObject."  
			strAux = strAux & Lf & "Please see the -NoTlist switch  or re-register scrrun.dll and try running ADPlus again."
			QuitWithError strAux
		End If
	Wend

	' The file was found. Let's give tlist some time to finish
	' we do this as an added safety check
	' It's really not necessary because tlist was run syncronous
	Set objTextFile = g_objFSO.OpenTextFile(FileName,ForReading,TristateUseDefault)
	i = 0
	' if the file is still empty it will be positioned at the end and we wait for 10 seconds
	While objTextFile.AtEndOfStream
		'Close the text file to let it finish writing and to get a new copy . . .
		objTextFile.Close
		wscript.sleep 1000
		i = i +1
		If i = 10 Then

			strAux = "The file Process_List.txt was never closed (finished writing) by tlist.exe in the " & g_DumpDir & " directory after waiting for 10 seconds."
			strAux = strAux & Lf & "This could indicate that there was a problem running tlist.exe or that the file could not be read using the Scripting.FileSystemObject."  
			strAux = strAux & Lf & "Please try running tlist.exe manually from the " & g_InstallDir & " directory or re-register scrrun.dll and try running ADPlus again."
			QuitWithError strAux
		End If
		'Open the text file again before looping . . .
		Set objTextFile = g_objFSO.OpenTextFile(FileName,ForReading,TristateUseDefault)
	Wend

	' if we got here then tlist is done
	' The File objTextFile remains open from the previous loop

	ArraySize = 50
	Redim g_CurrentProcesses(ArraySize)
	PCount = 0

	While not objTextFile.AtEndofStream
		strAux = objTextFile.ReadLine
		If instr(strAux, "Command Line:") = 0 and (instr(strAux, " 32 ") > 0 or instr(strAux, " 64 ") > 0) then
			if PCount > ArraySize Then
				ArraySize = ArraySize + 50
				Redim Preserve g_CurrentProcesses(ArraySize)
			End If
			' parsing PID, PNAME and PackageName if existing
			
			' removing the initial part we dont need [group] 0 32
			' [group] was introduced in win7
			
			strAux = LTrim(strAux)
			StrLength = Len(strAux)

			If instr(strAux, "[") = 1 then
				k = instr(strAux, "]")
				strAux = Right(strAux, StrLength - k)
				strAux = LTrim(strAux)
				StrLength = Len(strAux)
			End If

			' removing the first four chars; they are always "0 32" (it used to be only 2 chars - it didn't have the 32)
			strAux = Right(strAux, StrLength - 4)
			'strAux = Right(strAux, StrLength - 2)
			strAux = LTrim(strAux)

			arrAux= split(strAux," ",-1,1)

			set g_CurrentProcesses(PCount) = new RunningProcess
			' Process Names are kept with UCase
			g_CurrentProcesses(PCount).Name = UCase(trim(arrAux(1)))


			g_CurrentProcesses(PCount).ID = trim(arrAux(0))
			g_CurrentProcesses(PCount).Package = ""
			g_CurrentProcesses(PCount).Selected = False

			' checking if there is a package name
			strPackageName = ""
			i = InStr(strAux, "Mts:")
			If i>0 Then
				'Get the package name for the MTS / COM+ package
				i = i + 4 ' to point to the first position of the package name
				strPackageName = Trim(Mid(strAux,i,Len(strAux)-i+1))				
				
			End If
			' if we didn't get a package name, check if it is IIS 6
			if g_CurrentProcesses(PCount).Name = "W3WP.EXE" Then
				' read the next line
				if not objTextFile.AtEndofStream Then
					strAux = objTextFile.ReadLine
					' check the command line for the -ap parameter
					If InStr(strAux, "-ap") Then 
						arrAux = split(strAux, "-ap", -1, 1)
						strPackageName = trim(arrAux(1))
					End If
				End If
			End If
			strPackageName = trim(strPackageName)

			'Sanitize the package name because we use it in a file name
			if strPackageName <> "" Then
				strPackageName=CleanStringForFileName(strPackageName)
			End If
			
			g_CurrentProcesses(PCount).Package = Trim(strPackageName)

			PCount = PCount + 1
		End If
	Wend
	' releasing the excess of memory allocated
	Redim Preserve g_CurrentProcesses(PCount-1)

	objTextFile.Close

	AdpTrace "Exit GetCurrentProcesses() function "


End Sub
'---------------------------------------------------------------------------------------------
' Function:  AddSpawningCommand
'
'    Adds a Spawning Command to the g_SelectedSpawningCommands array
'
'      SpawningCommand should include the full path to the image and the 
'      related command line arguments
'      
'            Created by Israel Burman on 8/23/2002
'
'---------------------------------------------------------------------------------------------
Sub AddSpawningCommand(SpawningCommand)
	Dim i,n

	AdpTrace "In the AddSpawningCommand() function "

	if Trim(SpawningCommand) = "" Then Exit Sub

	n = UBound(g_SelectedSpawningCommands)
	' the last item is always empty; we use it and redim the array
	g_SelectedSpawningCommands(n) = SpawningCommand
	n = n + 1
	Redim Preserve g_SelectedSpawningCommands(n)
	g_SelectedSpawningCommands(n) = ""

End Sub
'---------------------------------------------------------------------------------------------
' Function:  AddProcessName
'
'    Adds a process name to the g_SelectedProcessNames array
'      
'            Created by Israel Burman on 5/30/2002
'
'---------------------------------------------------------------------------------------------
Sub AddProcessName(ProcessName)
	Dim i,n
	dim strAux

	AdpTrace "In the AddProcessName() function "

	strAux = UCase(Trim(ProcessName))
	if strAux = "" Then Exit Sub
	n = UBound(g_SelectedProcessNames)
	' checking for redefinition
	for i=0 to n
		if g_SelectedProcessNames(i) = strAux Then Exit Sub
	Next
	' the last item is always empty; we use it and redim the array
	g_SelectedProcessNames(n) = strAux
	n = n + 1
	Redim Preserve g_SelectedProcessNames(n)
	g_SelectedProcessNames(n) = ""

End Sub
'---------------------------------------------------------------------------------------------
' Function:  AddProcessID
'
'    Adds a process name to the g_SelectedProcessIDs array
'      
'            Created by Israel Burman on 5/30/2002
'
'---------------------------------------------------------------------------------------------
Sub AddProcessID(ProcessID)
	Dim i,n

	AdpTrace "In the AddProcessID() function "

	if ProcessID <= 0 Then Exit Sub
	n = UBound(g_SelectedProcessIDs)
	' checking for redefinition
	for i=0 to n
		if g_SelectedProcessIDs(i) = ProcessID Then Exit Sub
	Next
	' the last item is always empty; we use it and redim the array
	g_SelectedProcessIDs(n) = ProcessID
	n = n + 1
	Redim Preserve g_SelectedProcessIDs(n)
	g_SelectedProcessIDs(n) = 0

End Sub
'---------------------------------------------------------------------------------------------
' Function:  SelectProcesses
'
'    Sets RunningProcess.Selected for all elements in the g_CurrentProcesses array
'      
'            Created by Israel Burman on 5/30/2002
'
'---------------------------------------------------------------------------------------------
Sub SelectProcesses()
	Dim i,n,j,m
	Dim lFound
	Dim strMissingPIDs
	Dim strMissingPNames
	Dim PID
	Dim PName
	Dim strAux

	AdpTrace "In the SelectProcesses() function "

	strMissingPIDs = ""
	strMissingPNames = ""

	if g_NoTlist Then
		' No TLIst was used; I will create a list of running processes based on
		' the selected process ids.
		n = UBound(g_SelectedProcessIDs)
		if n>0 Then
			Redim g_CurrentProcesses(n)
			for i=0 to n
				set g_CurrentProcesses(i) = new RunningProcess
				g_CurrentProcesses(i).ID = g_SelectedProcessIDs(i)
				g_CurrentProcesses(i).Name = "PID" & g_SelectedProcessIDs(i)
				g_CurrentProcesses(i).Package = ""
				g_CurrentProcesses(i).Selected = False
			Next
		End If

	End If

	m = UBound(g_CurrentProcesses)

	' Searching by process ID
	n = UBound(g_SelectedProcessIDs)
	for i=0 to n
		PID = g_SelectedProcessIDs(i)
		if PID>0 Then
			lFound = 0
			for j = 0 to m
				if g_CurrentProcesses(j).ID = PID Then
					g_CurrentProcesses(j).Selected = True
					lFound = lFound + 1
				End If
			Next
			if lFound<=0 Then
				strMissingPIDs = strMissingPIDs & PID & ";"
			End If
		End If
	Next

	' Searching by process name
	n = UBound(g_SelectedProcessNames)
	for i=0 to n
		PName = g_SelectedProcessNames(i)
		if PName<>"" Then
			lFound = 0
			if InStr(1,PName,"*") <= 0 Then
				for j = 0 to m
					if g_CurrentProcesses(j).Name = PName Then
						g_CurrentProcesses(j).Selected = True
						lFound = lFound + 1
					End If
				Next
			Else
				for j = 0 to m
					if CompareWildProcessName(PName,g_CurrentProcesses(j).Name) = 0 Then
						g_CurrentProcesses(j).Selected = True
						lFound = lFound + 1
					End If
				Next
			End If
			if lFound<=0 Then
				strMissingPNames = strMissingPNames & PName & ";"
			End If
		End If
	Next

	' if -gs switch used, counts as a process as well
	if g_GeneratedScriptName <> "" Then
		lFound = lFound + 1
	End If

	' decide what to do if there are missing processes
	If strMissingPNames<>"" Or strMissingPIDs<>"" Then
		if g_IISMode = True Then
			' checking if inetinfo was found
			lFound = 0
			m = UBound(g_CurrentProcesses)
			for i=0 to m
				if g_CurrentProcesses(i).Name = "INETINFO.EXE" Then lFound = lFound + 1
			Next
			if lFound <= 0 Then
				strAux = "You selected the -IIS switch and IIS is currently not running"
 				QuitWithError strAux
			End If
		Else
			' we didn't find processes requested by the user
			strAux = "The following requested processes are not executing:" 
			if strMissingPNames <> "" Then strAux = strAux & Chr(13) & Chr(10) & strMissingPNames
			if strMissingPIDs <> "" Then strAux = strAux & Chr(13) & Chr(10) & strMissingPIDs
			QuitWithError strAux			
		End If
	End If

End Sub
'---------------------------------------------------------------------------------------------
' Function:  DumpSelectedProcesses
'
'    Prepares CDB to dump all selected processes
'    This function replaces the old DumpIIS and DumpAnyProc
'      
'            Created by Israel Burman on 5/30/2002
'
'---------------------------------------------------------------------------------------------
Sub DumpSelectedProcesses()
	dim i,n, lAttachment
	'dim ProcessCount  we don't check g_MaxApplications anymore
	dim strAux
	dim strShell
	dim strPackageName
	dim strAttachMode

	AdpTrace "In the DumpSelectedProcesses() function "


	' Adding processes for -IIS switch
	if g_IISMode = True Then
		AddProcessName "Inetinfo.exe"
		AddProcessName "dllhost.exe" ' iis5
		AddProcessName "w3wp.exe" ' iis6
		AddProcessName "aspnet_wp.exe"
		AddProcessName "mtx.exe"  ' nt4
		AddProcessName "dllhst3g.exe" ' dllhost for 3gb
	End If

	' Decide which processes to dump
	SelectProcesses

	n = UBound(g_CurrentProcesses)

	' checking g_MaxApplications
	'if g_MaxApplications > 0 Then
		'ProcessCount = 0
		'for i=0 to n
			'if g_CurrentProcesses(i).Selected = True Then ProcessCount = ProcessCount + 1
		'Next

		' adding the process to spawn
		'ProcessCount = ProcessCount + UBound(g_SelectedSpawningCommands)

		'if ProcessCount > g_MaxApplications Then
			'strAux = "There are " & ProcessCount & " processes selected to be monitored. The current limit is " & g_MaxApplications
			'strAux = strAux & Chr(13) & Chr(10) & "Please see ADPlus help on how to change this limitation"
			'QuitWithError strAux			
		'End If

	'End If

	AdpTrace "In the DumpSelectedProcesses() function - before PreShellCommands "
	' Executing the PreShellCommands
	n = UBound(g_PreShellCommands) 
	for i=0 to n
		if g_PreShellCommands(i) <> "" Then
			' First char is A or S (Asyncronous or Syncronous)
			strAux = Right(g_PreShellCommands(i),Len(g_PreShellCommands(i))-1)
			if Left(g_PreShellCommands(i),1) = "S" Then
				AdpLog "Runnig Shell Syncronous: " & strAux
				g_objShell.Run strAux,MINIMIZE_NOACTIVATE,TRUE
			else
				AdpLog "Runnig Shell Asyncronous: " & strAux
				g_objShell.Run strAux,MINIMIZE_NOACTIVATE,FALSE
			End If
		End If
	Next
	

	' Dumping all processes
	AdpTrace "Starting to attach the debugger to each process"
	AdpLog "Starting to attach the debugger to each process"
	' preparing for repeat attachments
	If g_RunMode = "HANG" Then
		if g_AttachInterval <= 0 Then
			g_AttachRepeats = 1
			g_AttachInterval = 0
		End If
		if g_AttachRepeats <= 0 Then g_AttachRepeats = 1
	else
		g_AttachInterval = 0
		g_AttachRepeats = 1
	end if

	for lAttachment = 1 to g_AttachRepeats	
		n = UBound(g_CurrentProcesses)
		for i=0 to n
			if g_CurrentProcesses(i).Selected = True Then
				strPackageName = g_CurrentProcesses(i).Name
				if g_CurrentProcesses(i).Package <> "" Then
					strPackageName = strPackageName & "_" & g_CurrentProcesses(i).Package
				End If

				Wscript.Echo "Attaching the debugger to: " & strPackageName
				Wscript.Echo "                               (Process ID: " & g_CurrentProcesses(i).ID & ")"
				AdpTrace "Attaching to " & g_CurrentProcesses(i).ID & " - " & strPackageName
				AdpLog "Attaching to " & g_CurrentProcesses(i).ID & " - " & strPackageName

				' command line to launch cdb
				strShell = g_Debugger ' "cdb.exe"
				If g_RunMode = "HANG" Then
					strShell = strShell & " -pv" ' to attach non invasive
				End If

				if g_DebuggerEvent <> "" Then
					strShell =  strShell & " -e " & g_DebuggerEvent
				End If

				'strShell =  strShell & " -p " & g_CurrentProcesses(i).ID & " -c $<" & Chr(34) & CreateCDBScript(g_CurrentProcesses(i).ID, strPackageName,"") & Chr(34)
				strShell =  strShell & " -p " & g_CurrentProcesses(i).ID & " -c " & Chr(34) & "$<"  & CreateCDBScript(g_CurrentProcesses(i).ID, strPackageName,"") & Chr(34)

				g_objShell.Run strShell,MINIMIZE_NOACTIVATE,FALSE

			End If
		Next

		if ((g_AttachInterval > 0) and (g_AttachRepeats-lAttachment > 0)) Then
			Wscript.Echo "sleeping " & lAttachment & "  " & g_AttachRepeats
			wscript.sleep g_AttachInterval*1000
		End If
	Next

	' spawning new processes (only crash mode)
	If g_RunMode = "CRASH" Then
		n = UBound(g_SelectedSpawningCommands)
		if n > 0 Then
			for i=0 to n
				if Trim(g_SelectedSpawningCommands(i))<>"" Then
					strPackageName = "Spawned" & i
					Wscript.Echo "Spawning: " & strPackageName
					AdpTrace "Spawning: " & g_SelectedSpawningCommands(i)
					AdpLog "Spawning: " & g_SelectedSpawningCommands(i)

					' command line to launch cdb
					strShell = g_Debugger ' "cdb.exe"

					if g_DebuggerEvent <> "" Then
						strShell =  strShell & " -e " & g_DebuggerEvent
					End If

					'strShell =  strShell & " -c $<" & Chr(34) & CreateCDBScript(0, strPackageName,g_SelectedSpawningCommands(i)) & Chr(34)
					strShell =  strShell & " -c " & Chr(34) & "$<"  & CreateCDBScript(0, strPackageName,g_SelectedSpawningCommands(i)) & Chr(34)
					strShell = strShell & " " & g_SelectedSpawningCommands(i)

					g_objShell.Run strShell,MINIMIZE_NOACTIVATE,FALSE
				End If

			Next
		End If
	End If

	' Creating the script for the -gs switch
	if g_GeneratedScriptName <> "" Then
		strPackageName = g_GeneratedScriptName

		Wscript.Echo "Creating a CDB script: " & strPackageName
		AdpTrace "Creating a CDB script: " & strPackageName
		AdpLog "Creating a CDB script: " & strPackageName
		i = 0
		strAux = CreateCDBScript(i, strPackageName,"")

	End If

	' Executing the PostShellCommands
	AdpTrace "In the DumpSelectedProcesses() function - before PostShellCommands "
	n = UBound(g_PostShellCommands) 
	for i=0 to n
		if g_PostShellCommands(i) <> "" Then
			' First char is A or S (Asyncronous or Syncronous)
			strAux = Right(g_PostShellCommands(i),Len(g_PostShellCommands(i))-1)
			if Left(g_PostShellCommands(i),1) = "S" Then
				AdpLog "Runnig Shell Syncronous: " & strAux
				g_objShell.Run strAux,MINIMIZE_NOACTIVATE,TRUE
			else
				AdpLog "Runnig Shell Asyncronous: " & strAux
				g_objShell.Run strAux,MINIMIZE_NOACTIVATE,FALSE
			End If
		End If
	Next

End Sub
'---------------------------------------------------------------------------------------------
' Function:  AdpTrace
'
'    Prints out debugging (tracing) information
'
'            Created by Israel Burman on 5/30/2002
'
'---------------------------------------------------------------------------------------------
Sub AdpTrace(strTrace)
	If DEBUGGING = TRUE Then
		Wscript.echo "*trace:  " & strTrace
	End If
End Sub
'---------------------------------------------------------------------------------------------
' Function:  AdpLog
'
'    Prints out logging information
'
'            Created by Israel Burman on 5/30/2002
'
'---------------------------------------------------------------------------------------------
Sub AdpLog(strLog)
	If g_AdpLogFile is Nothing Then
		g_AdpLogBuffer = g_AdpLogBuffer & strLog & Chr(13) & Chr(10)
	Else
		' if there is content in the buffer, flush it
		if g_AdpLogBuffer<> "" Then
			g_AdpLogFile.Write g_AdpLogBuffer
			g_AdpLogBuffer = ""
		End If
		g_AdpLogFile.Writeline strLog
	End If
End Sub
'---------------------------------------------------------------------------------------------
' Function:  CreateDebugCommand
'
'            This function is used to create the CDB commands for a given exception
'
'            Created by Israel Burman on 5/30/2002
'
'            Input:
'                Action - an instance of the ExceptionAction class
'                pid - the process id
'                packagename
'                FirstSecond - should be 1st or 2nd - indicates if the command is for 1st chance or 2nd.
'
'            Output: A string defining the debugger comands to be executed
'                Ex.: ".dump -u /mdi /c MyComments OutputFileName;"
'
'---------------------------------------------------------------------------------------------
Function CreateDebugCommand(Action, pid, packagename, FirstSecond )
	Dim strCmd ' commands for the debugger
	Dim strCommands ' string of keywords (ExceptionAction.Commands1/2)
	Dim arrCommands ' array of commands (resulting from parsing ExceptionAction.Commands1/2)
	Dim strAux ' just an intermediate auxiliary string
	Dim strTmp ' a temporary string variable
	Dim arrAux ' an intermediate array of strings
	Dim i,n,j
	Dim bLogs ' boolean indicating if Log keyword was included
	Dim UCaseCmd
	Dim CompName ' g_ComputerName replacing blanks with underscore

	CompName = Replace(g_ComputerName, " ", "_")
	strCmd = ""


	' converting the string with the key words into an array of key words
	if FirstSecond = "1st" Then
		strCommands = Action.Commands1
	Else
		strCommands = Action.Commands2
	End If
	arrCommands = GetStringArray(strCommands,";")
	
	n = UBound(arrCommands)
	If n>0 then

		' Trim all commands
		For i = 1 to n
			arrCommands(i) = Trim(arrCommands(i)) 
		Next

		bLogs = FALSE
		For i=1 to n
			if UCase(arrCommands(i)) = "LOG" Then
				bLogs = TRUE
				Exit For
			End If
		Next

		' Incluing each command into the final string



		' Adding a Log and Comments to the log
		if bLogs Then
			strCmd = strCmd & ".echo ---;"
			strCmd = strCmd & ".echo --- " & FirstSecond & " chance " & Action.ExceptionName & " exception ----;"
			strCmd = strCmd & ".echo ---------------------------------------------------------------;"

			if Action.Comment <> "" Then
				arrAux = GetStringArray(Action.Comment,Chr(10))
				for j = 1 to UBound(arrAux)
					strCmd = strCmd & ".echo " & arrAux(j) & ";"
				Next
				strCmd = strCmd & ".echo ---------------------------------------------------------------;"		
			End If
		End If

		For i=1 to n

			UCaseCmd = UCase(arrCommands(i))

			if UCaseCmd = "LOG" Then			
				' already considered
			Elseif UCaseCmd = "VOID" Then			
				' nothing to do
			Elseif UCaseCmd = "EVENTLOG" Then
				strAux = "ADPlus detected a " & FirstSecond & " chance " & Action.ExceptionName & " exception in process " & packagename
				strAux = strAux & " and has taken the following actions at the time of the crash: " &  strCommands & ". The output directory is " & g_DumpDir
				' strAux can include ";" and Chr(10) separators from the Custom_ command
				' and we need to take these out and replace them with blanks
				strAux = Replace(strAux , ";", " ")
				strAux = Replace(strAux , Chr(10), " ")
				strCmd = strCmd & "!elog_str"  & " " & strAux & ";"

			Elseif UCaseCmd = "STACK" Then			
				strCmd = strCmd & ".echo;.echo Faulting stack below ---;~#kvn250;.echo;"

			Elseif UCaseCmd = "STACKS" Then			
				strCmd = strCmd & ".echo;.echo All thread stacks below ---;~*kvn250;.echo;"

			Elseif UCaseCmd = "LOADEDMODULES" Then
				strCmd = strCmd & ".echo;.echo Listing loaded modules ---;lmv;"

			Elseif UCaseCmd = "MATCHINGSYMBOLS" Then
				strCmd = strCmd & ".echo;.echo Modules with matching symbols ---;lml;"

			Elseif UCaseCmd = "TIME" Then			
				strCmd = strCmd & ".echo;.echo Occurrence happened at: ;.time;"

			' eliminated Elseif Left(UCaseCmd,7) = "CUSTOM_" Then
				'strAux = Right(arrCommands(i),Len(arrCommands(i))-7)
				'arrAux = GetStringArray(strAux,Chr(10))
				'for j = 1 to UBound(arrAux)
				'	strCmd = strCmd & ".echo;.echo Executing command " & arrAux(j) & ";"
				'	strCmd = strCmd & arrAux(j) & ";"
				'Next

			' note: in .dump the /c can go between double quotes but here it's part of a sxe so the .dump by itself is already
			' between quotes, therefore we can not use it for the /c. We use CompName to replace blanks with underscore in g_ComputerName

			Elseif UCaseCmd = "MINIDUMP" Then			
				strCmd = strCmd & ".dump -u /mdi "
				strCmd = strCmd & "/c " & FirstSecond & "_chance_" & Action.ExceptionName & "_exception_in_" & packagename & "_running_on_" & CompName
				' output filename
				strAux = g_DumpDir & "\" & "PID-" & pid & "__" & packagename
				strAux = strAux & "__" & FirstSecond & "_chance_" & Action.ExceptionName
				strAux = strAux & "__mini.dmp"
				strCmd = strCmd & " " & strAux
				strCmd = strCmd & ";"

			Elseif UCaseCmd = "FULLDUMP" Then			
				strCmd = strCmd & ".dump -u /ma "
				strCmd = strCmd & "/c " & FirstSecond & "_chance_" & Action.ExceptionName & "_exception_in_" & packagename & "_running_on_" & CompName
				' output filename
				strAux = g_DumpDir & "\" & "PID-" & pid & "__" & packagename
				strAux = strAux & "__" & FirstSecond & "_chance_" & Action.ExceptionName
				strAux = strAux & "__full.dmp"
				strCmd = strCmd & " " & strAux
				strCmd = strCmd & ";"

			Elseif UCaseCmd = "MINIDUMPOVER" Then			
				strCmd = strCmd & ".dump -o /mdi "
				strCmd = strCmd & "/c " & FirstSecond & "_chance_" & Action.ExceptionName & "_exception_in_" & packagename & "_running_on_" & CompName
				' output filename
				strAux = g_DumpDir & "\" & "PID-" & pid & "__" & packagename
				strAux = strAux & "__" & g_DateTimeStamp
				strAux = strAux & "__" & FirstSecond & "_chance_" & Action.ExceptionName
				strAux = strAux & "__mini.dmp"
				strCmd = strCmd & " " & strAux
				strCmd = strCmd & ";"

			Elseif UCaseCmd = "FULLDUMPOVER" Then			
				strCmd = strCmd & ".dump -o /ma "
				strCmd = strCmd & "/c " & FirstSecond & "_chance_" & Action.ExceptionName & "_exception_in_" & packagename & "_running_on_" & CompName
				' output filename
				strAux = g_DumpDir & "\" & "PID-" & pid & "__" & packagename
				strAux = strAux & "__" & g_DateTimeStamp
				strAux = strAux & "__" & FirstSecond & "_chance_" & Action.ExceptionName
				strAux = strAux & "__full.dmp"
				strCmd = strCmd & " " & strAux
				strCmd = strCmd & ";"


			Else
				if arrCommands(i) <> "" Then
					strAux = "Invalid keyword in Commands for exception [" & Action.ExceptionCode & "]: " & arrCommands(i)
					QuitWithError strAux
				End If


			End If
		Next
	End If

	' Adding the custom actions
	if FirstSecond = "1st" Then
		strAux = Action.CustomCommands1
	Else
		strAux = Action.CustomCommands2
	End If
	if UCase(strAux) = "VOID" Then strAux = ""

	if strAux <> "" Then
		strCmd = strCmd & ".echo;.echo Executing custom commands; "
		' using .printf will only work if I don't use @ in front of the string
		' for breakpoints I use printf
		'strTmp = " Executing custom commands: " & strAux
		'strCmd = strCmd & ".echo;" & InternalPrintf(strTmp) & ";.echo;"
		strCmd = strCmd & strAux & ";"
	End If


	CreateDebugCommand = strCmd
	
End Function
'---------------------------------------------------------------------------------------------
' Function:  CreateExceptionAction
'            This function is used to create a CDB command of type SXE to instruct the debugger
'            what to do for a given exception.
'
'            Created by Israel Burman on 5/30/2002
'
'            Input:
'                Action - an instance of the ExceptionAction class
'                pid - the process id
'                packagename
'
'            Output: A string defining the sxe command for the debugger
'
'---------------------------------------------------------------------------------------------
Function CreateExceptionAction(Action, pid, packagename)
	Dim rt1,rt2
	Dim ac1,ac2
	Dim sx
	Dim strAux
	
	rt1 = Action.ReturnAction1
	rt2 = Action.ReturnAction2
	if UCase(Trim(rt1)) = "VOID" Then
		rt1 = ""
	End If
	if UCase(Trim(rt2)) = "VOID" Then
		rt2 = ""
	End If

'#IB021203

	ac1 = CreateDebugCommand(Action, pid, packagename, "1st")
	ac2 = CreateDebugCommand(Action, pid, packagename, "2nd")

	sx = "sxe"
	' for dll unload I use sxn to get the name of the module being unloaded
	if Action.ExceptionCode = "ud" Then sx = "sxn"

	if ac1= "" or ac2="" Then sx = "sxi"

	strAux = sx
	if Trim(ac1) <> "" Then
		strAux = strAux & " -c @" & Chr(34) & ac1 & " " & rt1  & Chr(34)
	End If
	if Trim(ac2) <> "" Then
		strAux = strAux & " -c2 @" & Chr(34) & ac2 & " " & rt2 & Chr(34)
	End If
	CreateExceptionAction = strAux &  " " & Action.ExceptionCode


	'CreateExceptionAction =	sx & " -c @" & Chr(34) & ac1 & " " & rt1  & Chr(34) & " -c2 @" & Chr(34) & ac2 & " " & rt2 & Chr(34) &  " " & Action.ExceptionCode	
	' replaced on 10/18/2003 (v6.02.015) Now we need to put a @ before all strings sxe = -c @"..."
	' CreateExceptionAction =	sx & " -c " & Chr(34) & ac1 & " " & rt1  & Chr(34) & " -c2 " & Chr(34) & ac2 & " " & rt2 & Chr(34) &  " " & Action.ExceptionCode	
	' CreateExceptionAction =	"sxe -c " & Chr(34) & CreateDebugCommand(Action, pid, packagename, "1st") & " " & rt1  & Chr(34) & " -c2 " & Chr(34) & CreateDebugCommand(Action, pid, packagename, "2nd") & " " & rt2 & Chr(34) &  " " & Action.ExceptionCode	
	
	
End Function
'---------------------------------------------------------------------------------------------
' Function:  QuitWithError
'            Terminates the script due to an error
'
'            Created by Israel Burman on 5/30/2002
'
'
'---------------------------------------------------------------------------------------------
Sub QuitWithError(ErrMsg)

	' if a log file already exists we log the error
	' if it doesn't exist we append the current log buffer to the error message

	' logging the error
	if g_AdpLogBuffer = "" Then
		AdpLog "*** ERROR *** "
		AdpLog "*** ERROR *** "
		AdpLog "*** ERROR *** "
		AdpLog ErrMsg
	else
		ErrMsg = "*** ERROR *** " & Chr(13) & Chr(10) & ErrMsg & Chr(13) & Chr(10) & "Log data below:" & Chr(13) & Chr(10) & g_AdpLogBuffer
	End If

	If g_QuietMode = True Then
		g_objShell.Logevent 1, ErrMsg
		Wscript.echo ErrMsg ' this goes into the console, not a dialog box, so it's ok with quite mode
		Wscript.Quit (512)
	Else
		Wscript.echo ErrMsg
		WScript.Quit (512)
	End If
End Sub
'---------------------------------------------------------------------------------------------
' Function:  GetStringArray
' 	converts a string containing a sequence of commands
' 	separated by a separator into an array of strings
'
' NOTE!!! - I'm creating my own function instead of using the
'  already existing Split function from the script engine, so I have
'  more control on what to do with empty substrings, or if the last char is a separator
'
' ATTENTION!!!
'   The UBound will be equal to number of elements
'   substrings will be placed in indexes 1 to n
'   if UBound = 0 there are no sub-strings in the string
'
'            Created by Israel Burman on 5/30/2002
'
'
'---------------------------------------------------------------------------------------------
Function GetStringArray(strSource, strSeparator)

	' strSource a string to be parsed
	' strSeparator the separator to be used when parsing strSource 

	Dim i1,i2 ' initial and end position of a sub-string
	Dim nc ' number of commands (sub strings) detected
	Dim ic ' current command
	dim strTemp ' internal copy of strSource
	Dim Length ' strTemp length

	' converts a string containing a sequence of commands
	' separated by a separator into an array of strings
	
	nc = 0 

	strTemp = Trim(strSource)
	Length = Len(strTemp)
	' first and last character should not be a separator
	while Left(strTemp,1) = strSeparator
		strTemp = Right(strTemp,Length-1)
		strTemp = Trim(strTemp)
		Length = Len(strTemp)
	wend
	while Right(strTemp,1) = strSeparator
		strTemp = Left(strTemp,Length-1)
		strTemp = Trim(strTemp)
		Length = Len(strTemp)
	wend

	if Length>0 Then nc = 1 ' We have at least one sub-string

	' counting the internal separators 
	i1 = 1 ' used here as initial position for search
	i2 = 1 ' used here as the position where the next ";" was found
	While i2>0 
		i2 = InStr(i1, strTemp, strSeparator)
		if i2 > 0 Then
			nc = nc + 1
			i1 = i2 + 1 ' next starting search position
		End If
	wend

	' preparing the output array
	' The UBound will be equal to nc
	' substrings will be placed in indexes 1 to nc
	' if nc = 0 there are no sub-strings in the string
	ReDim arrStrings(nc)
	if nc >0 then
		i1 = 1 ' used here as initial position for search
		' 02/11/2003
		'  was i2 = 1 but it doesn't get if the string has only one char
		i2 = 0 ' used here as the position where the next ";" was found
		ic = 1 ' current sub-string
		While i2<Length
			i2 = InStr(i1, strTemp, strSeparator)
			if i2 <= 0 Then i2 = Length + 1 ' virtual position
			arrStrings(ic) = Mid(strTemp , i1, i2 - i1)
			ic = ic + 1
			i1 = i2 + 1 ' next starting search position
		wend
	End If

	GetStringArray = arrStrings
End Function

'---------------------------------------------------------------------------------------------
' Function:  ValidateSettings
' 	
' 	Validates all Settings
'       Initializes globals that depend on settings
'
'            Created by Israel Burman on 5/30/2002
'
'
'---------------------------------------------------------------------------------------------
Sub ValidateSettings()
	Dim i,n, j
	Dim strErr
	Dim strAux
	Dim objArgs
	Dim Sympath
	Dim SysEnv, UserEnv
	dim Lf ' CR Line Feed

	Lf = Chr(13) & Chr(10)

	AdpTrace "In the ValidateSettings() function "

	'After getting the arguments, the first thing we need to do is determine the script engine that is running ADPlus.
	' the reason we do this is that if it's in wscript we need to re-launch in cscript
	Call DetectScriptEngine()


	if g_HelpRequested Then
		PrintUsage()
		' if no run mode we simply quit, else we continue validating
		if g_RunMode = "" Then
			wscript.quit 0
		End if
	End If

		


	if g_RunMode = "" Then
		strAux = "Run Mode not defined. A Run Mode is required."
		strAux = strAux & Lf & "Choose between -CRASH, -HANG or -QUICK"
		strAux = strAux & Lf & "For more information run ADPlus -Help"
		QuitWithError strAux
	End If

	' special checkings if -gs (generateScript) used
	if g_GeneratedScriptName <> "" Then
		if UBound(g_SelectedProcessNames)>0 or UBound(g_SelectedProcessIDs)>0 or UBound(g_SelectedSpawningCommands)>0 or g_IISMode = TRUE Then
			strErr = "Using the -gs switch; No process selection allowed." 
			QuitWithError strErr
		End If

		g_NoTlist = TRUE
		g_CheckTS = FALSE
		g_CheckFreeSpace = FALSE

	End If

	if UBound(g_SelectedSpawningCommands) > 0 Then
		if g_RunMode <> "CRASH" Then
			strAux = "Spawning processes can only be used in CRASH mode"
			QuitWithError strAux
		End If
	End If

	if g_IISMode = False Then
		if UBound(g_SelectedProcessNames) <=0 and UBound(g_SelectedProcessIDs) <= 0 and UBound(g_SelectedSpawningCommands) <= 0 and g_GeneratedScriptName = "" Then
			strAux = "No process was selected"
			strAux = strAux & Lf & "Provide a Process ID using the -p switch or"
			strAux = strAux & Lf & "provide a Process Name using the -pn switch or"
			strAux = strAux & Lf & "use the -IIS switch."
			strAux = strAux & Lf & "For more information run ADPlus -Help"
			QuitWithError strAux
		End If
	End If

	if g_NoTlist Then
		if g_IISMode = True OR UBound(g_SelectedProcessNames) > 0 Then
			strAux = "NoTList was selected; No IIS and no -pn are allowed"
			strAux = strAux & Lf & "Please use only Process IDs or spawning processes"
			QuitWithError strAux
		End If
	End If


	If InStr(1,g_OutPutDir,"\\",1) Then
		g_OutputDir = g_OutPutDir & "\" & g_ComputerName
	End If



	' Checking _NT_SYMBOL_PATH
	Set SysEnv = g_objShell.Environment("SYSTEM")
	Set UserEnv = g_objShell.Environment("USER")

	If SysEnv("_NT_SYMBOL_PATH") <> "" Then 
		Sympath = SysEnv("_NT_SYMBOL_PATH")
	ElseIf UserEnv("_NT_SYMBOL_PATH") <> "" Then
		Sympath = UserEnv("_NT_SYMBOL_PATH")
	Else
		if g_Sympath = "" And g_SympathPlus = "" Then
			AdpLog "   WARNING!  An '_NT_SYMBOL_PATH' environment variable is not set, as a result ADPlus will be forced to use 'export' symbols (if present) to resolve function names in the stack trace information for each thread listed in the log file for the processes being debugged.  To resolve this warning, please copy the appropriate symbols to a directory on the server and then create an environment variable with a name of '_NT_SYMBOL_PATH' and a value containing the path to the proper symbols (i.e. c:\winnt\symbols) before running ADPlus in quick or crash modes again.  NOTE:  After creating the '_NT_SYMBOL_PATH' system environment variable you will need to close the current command shell and open a new one before running ADPlus again."

			If g_QuietMode = False Then
				g_objShell.Popup "WARNING!  An '_NT_SYMBOL_PATH' environment variable is not set, as a result ADPlus will be forced to use 'export' symbols (if present) to resolve function names in the stack trace information for each thread listed in the log file for the processes being debugged.  To resolve this warning, please copy the appropriate symbols to a directory on the server and then create an environment variable with a name of '_NT_SYMBOL_PATH' and a value containing the path to the proper symbols (i.e. c:\winnt\symbols) before running ADPlus in quick or crash modes again.  NOTE:  After creating the '_NT_SYMBOL_PATH' system environment variable you will need to close the current command shell and open a new one before running ADPlus again.",,"ADPlus",0
			Else
				Wscript.echo "WARNING!  An '_NT_SYMBOL_PATH' environment variable is not set."
				Wscript.echo "Please check the application event log or the ADPlus-report.txt"
				Wscript.echo "for more details."
				Wscript.echo ""
				g_objShell.LogEvent 1, "WARNING!  An '_NT_SYMBOL_PATH' environment variable is not set, as a result ADPlus will be forced to use 'export' symbols (if present) to resolve function names in the stack trace information for each thread listed in the log file for the processes being debugged.  To resolve this warning, please copy the appropriate symbols to a directory on the server and then create an environment variable with a name of '_NT_SYMBOL_PATH' and a value containing the path to the proper symbols (i.e. c:\winnt\symbols) before running ADPlus in quick or crash modes again.  NOTE:  After creating the '_NT_SYMBOL_PATH' system environment variable you will need to close the current command shell and open a new one before running ADPlus again."
			End If
		End If
	End If

	n = UBound(g_ExceptionActions)
	For i=0 to n
		if g_ExceptionActions(i).ExceptionCode<> "" And Trim(g_ExceptionActions(i).ExceptionName)="" Then
			strErr = "ExceptionCode [" & g_ExceptionActions(i).ExceptionCode & "] doesn't have an ExceptionName"
			QuitWithError strErr
		End If
		
		' replacing blanks in ExceptionName
		g_ExceptionActions(i).ExceptionName = Replace( g_ExceptionActions(i).ExceptionName, " ", "_")
		' Replacing ";" in Comments
		g_ExceptionActions(i).Comment = Replace( g_ExceptionActions(i).Comment, ";", ".")
	Next

	' for backward compatibility we check for QuickMode
	If g_RunMode = "QUICK" Then
		'this is the same as HangMode just that the dump is a mini dump
		g_RunMode = "HANG"
		n = UBound(g_HangActions)
		for i = 0 to n
			if g_HangActions(i).Name = "FullDump" Then g_HangActions(i).Name = "MiniDump"
		Next
	End if
	
	' we do this to avoid the final sxe command to excede the 2000 limit
	' custom configuration could have defined Commands2 for the process shut down
	' exception which will create a very long command due to the long comments.
	' We don't need commands2 because we quit at first chance
	n = UBound(g_ExceptionActions)
	For i=0 to n
		if g_ExceptionActions(i).ExceptionCode = "epr"  Then
			g_ExceptionActions(i).Commands2 = ""
		End If
	Next

	' Cleaning up script name
	if g_GeneratedScriptName <> "" Then
		g_GeneratedScriptName = CleanStringForFileName(g_GeneratedScriptName)
	End If

	AdpLog "Selected debugger: " & g_Debugger



	AdpTrace "Exiting ValidateSettings() function . . ."


End Sub
'---------------------------------------------------------------------------------------------
' Function:  PrepareToRun
' 	
'       Initializes globals that depend on settings
'       Checks some conditions (iis version, etc.)
'       Creates all required directories and files
'
'            Created by Israel Burman on 5/30/2002
'
'
'---------------------------------------------------------------------------------------------
Sub PrepareToRun()

	Dim i,n, j
	Dim strErr
	Dim strAux
	Dim objArgs
	Dim arrAux
	Dim SysEnv, UserEnv, VolatileEnv
	Dim TermServDeviceDesc
	Dim TermServKey

	AdpTrace "In the PrepareToRun() function "


	CheckFiles

	'Initializing Globals that depend on settings

	If g_IISMode = True Then
		DetectIISVersion()
	End If


	' check if the output directory exists
	If not g_objFSO.FolderExists(g_OutPutDir) Then
		CreateDirectory(g_OutPutDir)
	End If



	if g_RunMode = "CRASH" Then
		g_DumpDir = g_OutPutDir & "\Crash_Mode__" & g_DateTimeStamp
	Else
		g_DumpDir = g_OutPutDir & "\Hang_Mode__" & g_DateTimeStamp
	End If
	if g_GeneratedScriptName <> "" Then
		g_DumpDir = g_OutPutDir & "\" & g_GeneratedScriptName
	End If

	On Error Resume Next	

	Call CreateDirectory(g_DumpDir) 
	if err.Number <> 0 Then
		QuitWithError "Unable to create Dump Directory: " & g_DumpDir & " Error: " & err.number & " - " & err.Description
	End If


	AdpLog "Output Directory: " & g_DumpDir

	Set g_AdpLogFile = g_objFSO.CreateTextFile(g_DumpDir & "\ADPlus_report.txt",True)
	if err.Number <> 0 Then
		QuitWithError "Unable to create History File: " & g_DumpDir & "\ADPlus_report.txt" & " Error: " & err.number & " - " & err.Description
	End If



'IB###??? Check if there is some restriction with VolatileEnv in nt4
' I saw some comment like this where we make the -notify
' the Set VolatileEnv was inside the if, therefore it was not defined for the
' user name when xp. This never failed before because we had on error resume next	
	Set VolatileEnv = g_objShell.Environment("PROCESS")

'###IB??? g_LOcalUserName should go to INitializeGlobals after we have the question above cleared
	'Get the locally logged on users name for use with the !Net_send command
	g_LocalUserName = VolatileEnv("USERNAME")


	'If not running on Windows XP, check to see if the user account is running inside of a terminal server session. 
	'If so, and the -crash switch is being used, display an error since 'crash' mode doesn't work inside of a terminal 
	'server session on NT 4.0 or Windows 2000.
	'If the server doesn't have terminal services installed, we can assume they aren't running AD+ in a TS session
	'so check for that first.
	'I have not tested this on NT 4.0 TSE so I'm not sure if this code will work properly on NT 4.0 TSE boxes.
	If CInt(g_OSBuildNumber) <= 2195 Then
		TermServKey = "HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Enum\Root\RDPDR\0000\DeviceDesc"
		err.clear
		TermServDeviceDesc = g_objShell.RegRead(TermServKey)
		If err.number <> -2147024894 Then 'The TS RDPDR key was found, Terminal services is installed, check to make sure the user isn't in a TS session.

			If (UCase(VolatileEnv("SESSIONNAME")) <> "CONSOLE" and VolatileEnv("SESSIONNAME") <> "") and (g_RunMode = "CRASH") Then
				If g_IISMode = True Then				
					QuitWithError "ADPlus has detected that you are attempting to run in 'Crash' mode, but this account is currently logged into terminal server session ID: " & UCase(VolatileEnv("SESSIONNAME")) & ".  'Crash' mode (invoked via the '-crash' switch) will not work inside a terminal server session.  To run ADPlus in 'Crash' mode, please log in locally at the console."
				Else
					' if not iis it's possible that the user is trying to debug a process that was started in the TS session which is fine
					' so we just warn if user selected -NoTsCheck
					if g_CheckTS Then
						QuitWithError "ADPlus has detected that you are attempting to run in 'Crash' mode from a terminal server session.  This will not work unless you are selecting a process that is running in the same session.  If this is the case, please use the NoTsCheck switch, otherwise please log in locally at the console."
					else
						AdpLog "ADPlus is running in a Terminal Server session. This will work only if you are trying to attach to a process started in the same session."
					End If
				End If
			End If		
		End If
	End If

	On Error GoTo 0


	' printing out all settings to the history file

	AdpLog " "

	If g_NoTlist = true Then
		AdpLog "ADPlus was run with the '-NoTlist' switch."
	End If
	If g_QuietMode = true Then
		AdpLog "ADPlus was run with the '-quiet' switch."
	End If
	If g_IISMode = True Then
		AdpLog "ADPlus was run with the '-IIS' switch."
	End If
	AdpLog " "
	AdpLog "g_Debug_Log = " & g_Debug_Log
	AdpLog "g_CheckFreeSpace = " & g_CheckFreeSpace

	if g_GeneratedScriptName <> "" Then
		AdpLog "-gs switch used; Script Name: " & g_GeneratedScriptName
	End If

	' loging settings
	if g_ExternalConfigurationFileName <> "" Then
		AdpLog " "
		AdpLog " ------ !!! External Configuration File was used: ----"
		arrAux = GetStringArray(g_ExternalConfigurationFileName,Chr(10))
		n = UBound(arrAux)
		for i = 1 to n
			AdpLog "    " & arrAux(i)
		Next
		AdpLog " "
	End If

	n = UBound(g_PreShellCommands)
	if n>0 or g_PreShellCommands(0) <> "" Then
		AdpLog " ------ Pre Shell Commands ----"
		For i=0 to n
			strAux = Trim(g_PreShellCommands(i))
			if strAux <> "" Then
				if Left(strAux,1) = "A" Then
					AdpLog "Async: " & Right(strAux,Len(strAux)-1)
				Else
					AdpLog "Sync: " & Right(strAux,Len(strAux)-1)
				End If
			End If
		Next
	End If

	n = UBound(g_PostShellCommands)
	if n>0 or g_PostShellCommands(0) <> "" Then
		AdpLog " ------ Post Shell Commands ----"
		For i=0 to n
			strAux = Trim(g_PostShellCommands(i))
			if strAux <> "" Then
				if Left(strAux,1) = "A" Then
					AdpLog "Async: " & Right(strAux,Len(strAux)-1)
				Else
					AdpLog "Sync: " & Right(strAux,Len(strAux)-1)
				End If
			End If
		Next
	End If

	n = UBound(g_PreCommands)
	if n>0 or g_PreCommands(0) <> "" Then
		AdpLog " ------ Initialization Commands ----"
		For i=0 to n
			strAux = Trim(g_PreCommands(i))
			if strAux <> "" Then
				AdpLog strAux
			End If
		Next
	End If

	n = UBound(g_PostCommands)
	if n>0 or g_PostCommands(0) <> "" Then
		AdpLog " ------ Termination Commands ----"
		For i=0 to n
			strAux = Trim(g_PostCommands(i))
			if strAux <> "" Then
				AdpLog strAux
			End If
		Next
	End If

	if g_RunMode = "CRASH" Then
		AdpLog " ------ Current Actions for Exceptions ----"
		
		n = UBound(g_ExceptionActions)
		For i=0 to n
			if g_ExceptionActions (i).ExceptionCode <> "" Then
				AdpLog "  " & g_ExceptionActions (i).ExceptionName & " [" & g_ExceptionActions (i).ExceptionCode & "]    " & "   return: " & g_ExceptionActions (i).ReturnAction1 & " " & g_ExceptionActions (i).ReturnAction2

				strAux = g_ExceptionActions (i).Commands1
				' eliminating Chr(10) separators
				strAux = Replace(strAux,Chr(10)," ")
				AdpLog "      1st chance: " & strAux

				strAux = g_ExceptionActions (i).Commands2
				' eliminating Chr(10) separators
				strAux = Replace(strAux,Chr(10)," ")
				AdpLog "      2nd chance: " & strAux

			End If
		Next


		n = UBound(g_BPActions)
		if n>0 or g_BPActions(0).Address <> "" Then
			AdpLog " ------ Current Actions for Breakpoints ----"
		
			For i=0 to n
				if g_BPActions(i).Address <> "" Then
					AdpLog "  Type: " & g_BPActions(i).AdrType & "  Address: " & g_BPActions(i).Address & "   return: " & g_BPActions(i).ReturnAction 

					strAux = g_BPActions(i).Commands
					' eliminating Chr(10) separators
					strAux = Replace(strAux,Chr(10)," ")
					AdpLog "      Actions: " & strAux
				End If
			Next
		End If
	Else
		AdpLog " ------ Current Actions for Hang Mode ----"
		
		n = UBound(g_HangActions)
		For i=0 to n
			if g_HangActions (i).Selected = True And (g_HangActions (i).Name <> "" or g_HangActions (i).Command <> "") Then

				strAux = ""
				if g_HangActions (i).Name <> "" Then
					strAux = strAux & "Name: " & g_HangActions (i).Name
				End If
				if g_HangActions (i).Command <> "" Then
					strAux = strAux & "Command: " & g_HangActions (i).Command
				End If
			End If
			AdpLog "  " & strAux
		Next

	End If

End Sub

'---------------------------------------------------------------------------------------------
' Function:  CreateBPAction
'            This function is used to create a CDB command of type BP to instruct the debugger
'            what to do for a given breakpoint.
'
'            Created by Israel Burman on 5/30/2002
'
'            Input:
'                Action - an instance of the BPAction class
'                pid - the process id
'                packagename
'
'            Output: A string defining the BP command for the debugger
'
'---------------------------------------------------------------------------------------------
Function CreateBPAction(Action, pid, packagename)

	Dim strCmd ' commands for the debugger
	Dim strCommands ' string of keywords (BPAction.Commands)
	Dim arrCommands ' array of commands (resulting from parsing strCommands)
	Dim strAux ' just an intermediate auxiliary string
	Dim strTmp ' a temporary string variable
	Dim arrAux ' an intermediate array of strings
	Dim i,n,j
	Dim bLogs ' boolean indicating if Log keyword was included
	Dim EscapedCrashDir ' CrashDir replacing \ with \\
	Dim BPAddrForFlnm ' BP Address used for file name (taking special characters off)
	Dim UCaseCmd
	Dim CompName ' g_ComputerName replacing blanks with underscore

	CompName = Replace(g_ComputerName, " ", "_")

	strCmd = ""
	

	' converting the string with the key words into an array of key words
	strCommands = Action.Commands
	arrCommands = GetStringArray(strCommands,";")
	
	n = UBound(arrCommands)

	If n>0 Then

			' no commands selected
			'CreateBPAction = ""
			'Exit Function	
			'End If

		' Trim all commands
		For i = 1 to n
			arrCommands(i) = Trim(arrCommands(i)) 
		Next

		bLogs = FALSE
		For i=1 to n
			if UCase(arrCommands(i)) = "LOG" Then
				bLogs = TRUE
				Exit For
			End If
		Next

		' Incluing each command into the final string


		' in bp we need to replace \ with \\ in filenames
		EscapedCrashDir = Replace( g_DumpDir, "\", "\\")
		if Trim(Action.Address)="" Then
			CreateBPAction = ""
			Exit Function	
		End If

		' Adding a Log and Comments to the log
		if bLogs Then
			strCmd = strCmd & ".echo ---;"
			strCmd = strCmd & ".echo --- Breakpoint " & Action.Address & " encountered ----;"
		End If

		' Also, to add the BP Address to a file name I need to eliminate special characters
		' When using BM we can have * and ?
		BPAddrForFlnm = Action.Address
		BPAddrForFlnm = Replace(BPAddrForFlnm,"!","_")
		BPAddrForFlnm = Replace(BPAddrForFlnm,":","_")
		BPAddrForFlnm = Replace(BPAddrForFlnm,"*","_")
		BPAddrForFlnm = Replace(BPAddrForFlnm,"?","_")

		For i=1 to n

			UCaseCmd = UCase(arrCommands(i))

			if UCaseCmd = "LOG" Then			
				' already considered
			Elseif UCaseCmd = "VOID" Then
				' nothing to do
			Elseif UCaseCmd = "EVENTLOG" Then
				strAux = "ADPlus detected a BreakPoint at " & Action.Address & " in process " & packagename
				strAux = strAux & " and has taken the following actions at that time: " &  strCommands & ". The output directory is " & g_DumpDir
				' strAux can include ";" and Chr(10) separators from the Custom_ command
				' and we need to take these out and replace them with blanks
				strAux = Replace(strAux , ";", " ")
				strAux = Replace(strAux , Chr(10), " ")
				strCmd = strCmd & "!elog_str"  & " " & strAux & ";"

			Elseif UCaseCmd = "STACK" Then			
				strCmd = strCmd & ".echo;.echo Current stack below ---;~.kvn250;"

			Elseif UCaseCmd = "STACKS" Then			
				strCmd = strCmd & ".echo;.echo All thread stacks below ---;~*kvn250;"

			Elseif UCaseCmd = "LOADEDMODULES" Then
				strCmd = strCmd & ".echo;.echo Listing loaded modules ---;lmv;"

			Elseif UCaseCmd = "MATCHINGSYMBOLS" Then
				strCmd = strCmd & ".echo;.echo Modules with matching symbols ---;lml;"

			Elseif UCaseCmd = "TIME" Then			
				strCmd = strCmd & ".echo;.echo Occurrence happened at: ;.time;"

			' eliminated  Elseif Left(UCaseCmd,7) = "CUSTOM_" Then
				'strAux = Right(arrCommands(i),Len(arrCommands(i))-7)
				'arrAux = GetStringArray(strAux,Chr(10))
				'for j = 1 to UBound(arrAux)
				'	' eliminated the .echo below to allow complex conditional breakpoints
				'	'strCmd = strCmd & ".echo;.echo Executing command " & arrAux(j) & ";"
				'	strCmd = strCmd & arrAux(j) & ";"
				'Next

			' Dump commands

			' NOTE!!!
			' In BP commands we need to use \\ for every \ as the debuger considers here
			' C controls characters like \n, \", etc.
			' Therefore we use here the EscapedCrashDir variable, defined earlier in this function
	'

			Elseif UCaseCmd = "MINIDUMP" Then			
				strCmd = strCmd & ".dump -u /mdi "
				strCmd = strCmd & "/c " & "Breakpoint_" & BPAddrForFlnm & "_in_" & packagename & "_running_on_" & CompName
				' output filename
				strAux = EscapedCrashDir & "\\" & "PID-" & pid & "__" & packagename
				strAux = strAux & "_Breakpoint_" & BPAddrForFlnm
				strAux = strAux & "__mini.dmp"
				strCmd = strCmd & " " & strAux
				strCmd = strCmd & ";"

			Elseif UCaseCmd = "FULLDUMP" Then			
				strCmd = strCmd & ".dump -u /ma "
				strCmd = strCmd & "/c " & "Breakpoint_" & BPAddrForFlnm & "_in_" & packagename & "_running_on_" & CompName
				' output filename
				strAux = EscapedCrashDir & "\\" & "PID-" & pid & "__" & packagename
				strAux = strAux & "_Breakpoint_" & BPAddrForFlnm
				strAux = strAux & "__full.dmp"
				strCmd = strCmd & " " & strAux
				strCmd = strCmd & ";"

			Elseif UCaseCmd = "MINIDUMPOVER" Then			
				strCmd = strCmd & ".dump -o /mdi "
				strCmd = strCmd & "/c " & "Breakpoint_" & BPAddrForFlnm & "_in_" & packagename & "_running_on_" & CompName
				' output filename
				strAux = EscapedCrashDir & "\\" & "PID-" & pid & "__" & packagename
				strAux = strAux & "__" & g_DateTimeStamp
				strAux = strAux & "_Breakpoint_" & BPAddrForFlnm
				strAux = strAux & "__mini.dmp"
				strCmd = strCmd & " " & strAux
				strCmd = strCmd & ";"

			Elseif UCaseCmd = "FULLDUMPOVER" Then			
				strCmd = strCmd & ".dump -o /ma "
				strCmd = strCmd & "/c " & "Breakpoint_" & BPAddrForFlnm & "_in_" & packagename & "_running_on_" & CompName
				' output filename
				strAux = EscapedCrashDir & "\\" & "PID-" & pid & "__" & packagename
				strAux = strAux & "__" & g_DateTimeStamp
				strAux = strAux & "_Breakpoint_" & BPAddrForFlnm
				strAux = strAux & "__full.dmp"
				strCmd = strCmd & " " & strAux
				strCmd = strCmd & ";"


			Else
				if UCaseCmd<> "" Then
					strAux = "Invalid keyword in Commands for breakpoint [" & Action.Address & "]: " & arrCommands(i)
					QuitWithError strAux
				End If


			End If
		Next
	End If

	strAux = Action.CustomCommands
	if UCase(strAux) = "VOID" Then strAux = ""
	if strAux <> "" Then
		'strCmd = strCmd & ".echo;.echo Executing custom commands; "
		' using .printf will only work if I don't use @ in front of the string
		' i use it here because I dont use @ but cannot use it with exceptions
		strTmp = " Executing custom commands: " & strAux
		strCmd = strCmd & ".echo;" & InternalPrintf(strTmp) & ";.echo;"
		strCmd = strCmd & strAux & ";"
	End If
	
	if UCase(Action.ReturnAction) <> "VOID" Then
		strCmd = strCmd & Action.ReturnAction
	End If



	strAux = Action.AdrType & " " & Action.Address & " "
	if Trim(Action.Passes) <> "" Then strAux = strAux & Action.Passes & " "
	strAux = strAux & Chr(34) & strCmd & Chr(34)


	CreateBPAction = strAux
	
	
End Function
'---------------------------------------------------------------------------------------------
' Function:  AddBreakPoint
' 	
' 	Adds a breakpoint to the g_BPActions array
'
'       Input: a string with the following syntax
'
'        "address;optional_additional_parameters"
'         optional parameters must be separated by ";" and no blanks and can be any of the following:
'            MiniDump, FullDump or NoDump - to indicate if you want a dump (default is NoDump)
'            an integer number - indicating the number of passes to ignore
'            Q or G - if you want to quit or go after the action; Defualt is G
'         if not including any optional parameters the default behavior is to 
'            create a log,list the call stack, and return with GO.
'         Example:
'           -bp MyModule!MyClass::MyMethod;MiniDump;Q
'
'            Created by Israel Burman on 5/30/2002
'
'
'---------------------------------------------------------------------------------------------
Sub AddBreakPoint(strVal)
	dim i,j,n
	dim BPIndex ' index of the new bp in the g_BPActions array
	Dim strCommands ' string of keywords (BPAction.Commands)
	Dim arrCommands ' array of commands (resulting from parsing strCommands)
	Dim DumpType ' optional dump type selected
	Dim strAux ' temporary string
	Dim i1,i2

	if Trim(strVal) = "" then Exit Sub
	strCommands = strVal
	arrCommands = GetStringArray(strCommands,";")

	BPIndex = UBound(g_BPActions) + 1
	Redim Preserve g_BPActions(BPIndex)

	' creating the new element of the array
	set g_BPActions(BPIndex) = new BPAction
	g_BPActions(BPIndex).Address = Trim(arrCommands(1))
	g_BPActions(BPIndex).Passes = ""
	' using the default template	
	g_BPActions(BPIndex).AdrType = g_BPActions(0).AdrType
	g_BPActions(BPIndex).Commands = g_BPActions(0).Commands
	g_BPActions(BPIndex).CustomCommands = g_BPActions(0).CustomCommands
	g_BPActions(BPIndex).ReturnAction = g_BPActions(0).ReturnAction

	' checking for optional parameters
	n = UBound(arrCommands)
	if n<=1 Then Exit Sub

	DumpType = ""
	for i=2 to n
		strAux = UCase(Trim(arrCommands(i)))
		if strAux = "G" Then
			g_BPActions(BPIndex).ReturnAction = "G"
		Elseif strAux = "Q" Then
			g_BPActions(BPIndex).ReturnAction = "Q"
		Elseif strAux = "QD" Then
			g_BPActions(BPIndex).ReturnAction = "QD"
		Elseif strAux = "MINIDUMP" Then
			DumpType = "MiniDump"
		Elseif strAux = "FULLDUMP" Then
			DumpType = "FullDump"
		Elseif strAux = "NODUMP" Then
			DumpType = "NoDump"
		Elseif strAux = "BU" Then
			g_BPActions(BPIndex).AdrType = "bu"
		Elseif strAux = "BM" Then
			g_BPActions(BPIndex).AdrType = "bm"
		Elseif IsNumeric(strAux) Then
			g_BPActions(BPIndex).Passes = strAux
		Else
			if strAux <> "" Then
				strAux = "Invalid option in -bp parameter [" & strVal & "]: " & arrCommands(i)
				QuitWithError strAux
			End If
		End If
	Next
	' setting the dump type if requested
	if DumpType <> "" Then		
		g_BPActions(BPIndex).Commands = ReplaceDumpType(g_BPActions(BPIndex).Commands, DumpType)
	End If


End Sub
'---------------------------------------------------------------------------------------------
' Function:  AddException
' 	
' 	Adds an exception to the g_ExceptionActions array
'         The new exception will be configured with default parameters taken from 
'           g_ExceptionActions(0)
'
'       Input: Exception Code and Exception Name
'              if exception name = "" will use the exception code for the name
'
'
'            Created by Israel Burman on 5/30/2002
'
'
'---------------------------------------------------------------------------------------------
Sub AddException(strCode, strName)
	dim n
	if Trim(strCode) = "" then Exit Sub

	n = UBound(g_ExceptionActions) + 1
	Redim Preserve g_ExceptionActions(n)

	' creating the new element of the array
	set g_ExceptionActions(n) = new ExceptionAction
	g_ExceptionActions(n).Comment = ""
	g_ExceptionActions(n).ExceptionCode = Trim(strCode)
	if Trim(strName)<> "" Then 
		g_ExceptionActions(n).ExceptionName = Trim(strName)
	else
		g_ExceptionActions(n).ExceptionName = g_ExceptionActions(n).ExceptionCode
	End If

	' using the default template	
	g_ExceptionActions(n).Commands1 = g_ExceptionActions(0).Commands1
	g_ExceptionActions(n).Commands2 = g_ExceptionActions(0).Commands2
	g_ExceptionActions(n).CustomCommands1 = g_ExceptionActions(0).CustomCommands1
	g_ExceptionActions(n).CustomCommands2 = g_ExceptionActions(0).CustomCommands2
	g_ExceptionActions(n).ReturnAction1 = g_ExceptionActions(0).ReturnAction1
	g_ExceptionActions(n).ReturnAction2 = g_ExceptionActions(0).ReturnAction2


End Sub
'---------------------------------------------------------------------------------------------
' Function:  ReplaceDumpType
' 	
' 	Replaces the selected dump type in a string of commands
'
'       Input: 
'              strCmd - the original commands string
'              strDumpType - the desired dump type
'                 MiniDump, FullDump, MiniDumpOver, FullDumpOver, or NoDump (if no dump desired)
'       Return: 
'              The new commands string
'
'            Created by Israel Burman on 5/30/2002
'
'
'---------------------------------------------------------------------------------------------
Function ReplaceDumpType(strCmd, strDumpType)

	dim strOut
	dim strAux
	dim i1,i2
	if strDumpType="" Then
		ReplaceDumpType = strCmd
		Exit Function
	End If

	strAux = strCmd
	' Checking if there is already a dump type selected
	i1 = InStr(1,strAux,"Dump")
	if i1>0 Then
		i1 = i1 -4 ' because the full word is MiniDump or FullDump not just Dump
		' finding the end of the word
		i2 = InStr(i1,strAux,";")
		if i2 <=0 then i2 = Len(strAux)
		' taking out the current selection
		strAux = Left(strAux,i1-1) & Right(strAux, Len(strAux) - i2)
	End If
	' adding the new selection
	if strDumpType <> "NoDump" Then
		strAux = strDumpType & ";" & strAux
	End If
	
	ReplaceDumpType = strAux
End Function
'---------------------------------------------------------------------------------------------
' Function:  ReadCustomCommands
'
'    Reads a file with a customization script
'    Extracts each of the main sections from the config file and
'      calls the related function that is going to parse it
' 	
'
'            Created by Israel Burman on 5/30/2002
'
'
'   This are the main sections of a config file

'  <ADPlus>
'    <!-- Comments -->
'    <Settings>
'        <!-- defining basic settings (run mode, quiet mode, etc.) -->
'    </Settings>
'    <PreCommands>
'        <!-- defines a set of commands to execute before the sxe and bp commands -->
'    </PreCommands>
'    <PostCommands>
'        <!-- defines a set of commands to execute after the sxe and bp commands -->
'    </PostCommands>
'    <Exceptions>
'        <!-- commands acting on the exception actions -->
'    </Exceptions>
'    <Breakpoints>
'        <!-- defining breakpoints -->
'    </Breakpoints>
'    <HangActions>
'        <!-- defining actions for hang mode -->
'    </HangActions>
'    <LinkConfig>
'        <!-- linking another config file -->
'    </LinkConfig>
'
'  </ADPlus>
'
'
'---------------------------------------------------------------------------------------------
Sub ReadCustomCommands(strFlnmOrg)
	dim strFlnm
	dim strTotal
	dim strAdPlus
	dim strAux
	dim p1,p2 ' positions in a string
	Dim CfgFile ' the file to read

	AdpTrace "In the ReadCustomCommands() function "

	
	strFlnm = strFlnmOrg

	while strFlnm <> ""


		if g_ExternalConfigurationFileName <> "" Then
			g_ExternalConfigurationFileName = g_ExternalConfigurationFileName & Chr(10)
		End If
		g_ExternalConfigurationFileName = g_ExternalConfigurationFileName & strFlnm

		If not g_objFSO.FileExists(strFlnm) Then
			strAux = "Configuration file not found: " & strFlnm
			QuitWithError strAux

		End If

		On Error Resume Next

		set CfgFile = g_objFSO.OpenTextFile( strFlnm,ForReading )
		if err.Number<>0 Then
			strAux = "Error opening config file: " & strFlnm
			strAux = strAux & "  Error: " & err.Number & " - " & err.Description
			QuitWithError strAux
		
		End If
		strTotal = CfgFile.ReadAll()
		CfgFile.Close()
		strFlnm = ""

		On Error GoTo 0


		strAdPlus = XMLGetTagText(1, strTotal, "ADPlus", p1,p2, TRUE)
		if strAdPlus = "" Then Exit Sub

		strAux = XMLGetTagText(1, strAdPlus, "Settings", p1,p2, TRUE)
		ReadCustomSettings strAux

		strAux = XMLGetTagText(1, strAdPlus, "PreCommands", p1,p2, TRUE)
		ReadCustomPreCommands strAux

		strAux = XMLGetTagText(1, strAdPlus, "PostCommands", p1,p2, TRUE)
		ReadCustomPostCommands strAux

		strAux = XMLGetTagText(1, strAdPlus, "BreakPoints", p1,p2, TRUE)
		ReadCustomBreakPoints strAux

		strAux = XMLGetTagText(1, strAdPlus, "Exceptions", p1,p2, TRUE)
		ReadCustomExceptions strAux

		strAux = XMLGetTagText(1, strAdPlus, "HangActions", p1,p2, TRUE)
		ReadCustomHangActions strAux

		strFlnm = Trim(XMLGetTagText(1, strAdPlus, "LinkConfig", p1,p2, TRUE))


	Wend

End Sub
'---------------------------------------------------------------------------------------------
' Function:  XMLFindTagDelimiter
'
'    Locates a given tag in a given string
'    Can locate open or closing delimiter
'    Locates the first instance of the tag we are looking for
'
'    Input:
'      StartPosition - position to start the search (1 to Len)
'      strOrigin - the string to be searched
'      strTag - the tag to be looked for (without the <>)
'      OPenClose = 1 for OPen tag and 2 for Closing tag
'
'    Return
'      If searching for the open tag, returns the position of the closing ">"
'      If searching for the closing tag, returns the position of the opening "<"
' 	
'
'            Created by Israel Burman on 5/30/2002
'
'---------------------------------------------------------------------------------------------
Function XMLFindTagDelimiter(StartPosition, strOrigin, strTag, OpenClose)
	dim OpenTag1 ' Position of the "<" for the tag we are looking for
	dim OpenTag2 ' Position of the related ">"
	dim Position1 ' Position of the "<" for a generic tag
	dim Position2 ' Position of the ">" for a generic tag
	dim UCaseTag ' the tag we are looking for with UCase
	dim strAux ' temporary string
'
'        <MyTag>
'           <MyTag>
'               this is an internal tag
'           </MyTag>
'        </MyTag>
'
	AdpTrace "In the XMLFindTagDelimiter() function "

	if StartPosition<=0 Then StartPosition = 1
	if StartPosition > Len(strOrigin) Then
		XMLFindTagDelimiter = 0
		Exit Function
	End If
	UCaseTag = UCase(Trim(strTag))
	if OpenClose = 2 then UCaseTag = "/" & UCaseTag

	' searching for the open tag
	OpenTag1 = 0
	OpenTag2 = 0
	Position1 = InStr(StartPosition,strOrigin,"<")
	while Position1>0
		Position2 = InStr(Position1+1,strOrigin,">")
		if Position2 <= 0 Then
			strAux = "Unbalanced < > in XML string (config file)"
			QuitWithError strAux
		End If
		strAux = Mid(strOrigin,Position1 + 1,Position2-Position1-1 )
		strAux = Replace(strAux, Chr(10), " ")
		strAux = Replace(strAux, Chr(13), " ")
		strAux = UCase(Trim(strAux))
		if strAux = UCaseTag Then
			' we found the tag
			OpenTag1 = Position1
			OpenTag2 = Position2
			' changing Position1 so we exit the while
			Position1 = 0
		Else
			' continue searching the rest of the string
			Position1 = InStr(Position2,strOrigin,"<")
		End If
	Wend
	
	if OpenClose = 1 Then
		XMLFindTagDelimiter = OpenTag2
	else
		XMLFindTagDelimiter = OpenTag1
	End If
End Function
'---------------------------------------------------------------------------------------------
' Function:  XMLGetTagText
'
'    Extracts the text for an XML Tag from a given string
'    Only the first instance of this tag is extracted
'
'    Input:
'      StartPosition - where to start the search
'      strOrigin - the string to be searched
'      strTag - the tag to be looked for (without the <>)
'      Linearize - TRUE/FALSE - eliminates Chr(10) and Chr(13) and Chr(9) - Tab and trims.
'
'    Output:
'      returns the text inside the tags
'      FirstColumn, LastColumn - position of first and last char of the text
'      
' 	
'
'            Created by Israel Burman on 5/30/2002
'
'---------------------------------------------------------------------------------------------
Function XMLGetTagText(StartPosition, strOrigin, strTag, ByRef FirstColumn, ByRef LastColumn, Linearize)

	dim OpenTag ' Position of the ">" for the tag we are looking for
	dim CloseTag ' position of the "<" for the closing tag (</MyTag>)
	' we need some logic to take into account internal equivalent tags
	dim OpensFound ' counter for equivalent open tags found
	dim ClosesFound ' same for equivalent closes found
	dim SearchClosePos ' Position to start searching for the closing tag
	dim SearchOpenPos ' Position to start searching for new opening tags	
	dim Position ' a generic position
	dim TotalLen' length of strOrigin	
	dim strAux ' temporary string
'
'        <MyTag>
'           <MyTag>
'               this is an internal tag
'           </MyTag>
'        </MyTag>

	AdpTrace "In the XMLGetTagText() function "

	FirstColumn = 0
	LastColumn = 0

	TotalLen = Len(strOrigin)
	' searching for the open tag
	OpenTag = XMLFindTagDelimiter(StartPosition, strOrigin, strTag, 1)
	if OpenTag <= 0 Then
		XMLGetTagText = ""
		Exit Function
	End If
	OpensFound = 1
	ClosesFound = 0
	SearchClosePos = OpenTag + 1
	SearchOpenPos = OpenTag + 1
	While OpensFound <> ClosesFound
		CloseTag = XMLFindTagDelimiter(SearchClosePos, strOrigin, strTag, 2)
		if CloseTag <=0 Then
			strAux = "No matching closing tag in XML string [" & strTag & "]. Check missing closing tag or use of invalid characters like '<' or '>' inside the tag."
			QuitWithError strAux
		End If
		ClosesFound = ClosesFound + 1
		SearchClosePos = CloseTag + 1
		' checking if there is an internal equivalent open before this position
		Position = XMLFindTagDelimiter(SearchOpenPos, strOrigin, strTag, 1)
		if Position >0 and Position < CloseTag Then
			' we found an internal open
			OpensFound = OpensFound + 1
			SearchOpenPos = Position + 1
		End If		
	wend

	' We have an open and matching close
	FirstColumn = OpenTag + 1
	LastColumn = CloseTag - 1
	strAux = Mid(strOrigin , OpenTag + 1, CloseTag - OpenTag - 1)
	if Linearize Then
		strAux = Replace(strAux, Chr(13), " ")
		strAux = Replace(strAux, Chr(10), " ")
		strAux = Replace(strAux, Chr(9), " ")
		strAux = Trim(strAux)

	End If
	XMLGetTagText = strAux

End Function
'---------------------------------------------------------------------------------------------
' Function:  XMLFindFirstTag
'
'    Locates the first open tag in a string
'
'    Input:
'      StartPosition - position to start the search (1 to Len)
'      strOrigin - the string to be searched
'
'    Return
'      The found tag (without <>)
'
'            Created by Israel Burman on 5/30/2002
'
'---------------------------------------------------------------------------------------------
Function XMLFindFirstTag(StartPosition, strOrigin)
	dim Position1 ' Position of the "<" for a generic tag
	dim Position2 ' Position of the ">" for a generic tag
	dim strAux ' temporary string
	

	AdpTrace "In the XMLFindFirstTag() function "

	Position1 = InStr(StartPosition, strOrigin, "<")
	while Position1 > 0
		Position2 = InStr(Position1+1,strOrigin,">")
		if Position2 <= 0 Then
			strAux = "Unbalanced < > in XML string (config file)"
			QuitWithError strAux
		End If
		strAux = Mid(strOrigin,Position1 + 1,Position2-Position1-1 )
		strAux = Replace(strAux, Chr(10), " ")
		strAux = Replace(strAux, Chr(13), " ")
		strAux = Trim(strAux)
		if Left(strAux,1) <> "/" And Left(strAux,1)<> "!" Then
			' we found the tag
			XMLFindFirstTag = strAux
			Exit function
		End If
		' continue searching the rest of the string
		Position1 = InStr(Position2,strOrigin,"<")		
	wend
	' nothing found
	XMLFindFirstTag = ""

End Function
'---------------------------------------------------------------------------------------------
' Function:  ReadCustomPreCommands
'
'    Parses PreCommands from a string
'
'    Input:
'      strOrigin - the string to be searched

'    <!-- defines a set of commands to execute before the sxe and bp commands -->
'    <PreCommands>
'       <Cmd> sometext  </Cmd>
'       <ShellSync> Something( todo ) </ShellSync>
'       <ShellAsync> Something( todo ) </ShellAsync>
'    </PreCommands>
'
'
'            Created by Israel Burman on 5/30/2002
'
'---------------------------------------------------------------------------------------------
Sub ReadCustomPreCommands(strOrigin)
	dim Position ' current working position
	dim Position1 ' begining position of some tag text
	dim Position2 ' related ending position
	dim TotalLen ' size of strOrigin
	dim strAux, strTag, strTagText
	dim UCaseTag
	dim n 

	AdpTrace "In the ReadCustomPreCommands() function "

	TotalLen = Len(strOrigin)
	Position = 1
	while Position < TotalLen
		strTag = XMLFindFirstTag(Position, strOrigin)
		if strTag = "" Then Exit Sub
		strTagText = XMLGetTagText(Position, strOrigin, strTag, Position1, Position2, TRUE)
		strTagText = ReplaceReservedChars(strTagText)
		' updating working position
		Position = Position2 + 1
		' parsing the current command
		UCaseTag = Trim(UCase(strTag))
		if UCaseTag = "CMD" Then
			n = UBound(g_PreCommands) + 1
			Redim Preserve g_PreCommands(n)
			g_PreCommands(n) = strTagText
		elseif UCaseTag = "SHELLSYNC" Then
			n = UBound(g_PreShellCommands) + 1
			Redim Preserve g_PreShellCommands(n)
			g_PreShellCommands(n) = "S" & strTagText
		elseif UCaseTag = "SHELLASYNC" Then
			n = UBound(g_PreShellCommands) + 1
			Redim Preserve g_PreShellCommands(n)
			g_PreShellCommands(n) = "A" & strTagText
		else
			strAux = "Invalid tag in PreCommands: " & strTag
			QuitWithError strAux
		End If 
	wend

End Sub
'---------------------------------------------------------------------------------------------
' Function:  ReadCustomPostCommands
'
'    Parses PostCommands from a string
'
'    Input:
'      strOrigin - the string to be parsed
'
'
'    <!-- defines a set of commands to execute after the sxe and bp commands -->
'    <PostCommands>
'       same as pre commands
'    </PostCommands>
'
'            Created by Israel Burman on 5/30/2002
'
'---------------------------------------------------------------------------------------------
Sub ReadCustomPostCommands(strOrigin)
	dim Position ' current working position
	dim Position1 ' begining position of some tag text
	dim Position2 ' related ending position
	dim TotalLen ' size of strOrigin
	dim strAux, strTag, strTagText
	dim UCaseTag
	dim n 

	AdpTrace "In the ReadCustomPostCommands() function "

	TotalLen = Len(strOrigin)
	Position = 1
	while Position < TotalLen
		strTag = XMLFindFirstTag(Position, strOrigin)
		if strTag = "" Then Exit Sub
		strTagText = XMLGetTagText(Position, strOrigin, strTag, Position1, Position2, TRUE)
		strTagText = ReplaceReservedChars(strTagText)
		' updating working position
		Position = Position2 + 1
		' parsing the current command
		UCaseTag = Trim(UCase(strTag))
		if UCaseTag = "CMD" Then
			n = UBound(g_PostCommands) + 1
			Redim Preserve g_PostCommands(n)
			g_PostCommands(n) = strTagText
		elseif UCaseTag = "SHELLSYNC" Then
			n = UBound(g_PostShellCommands) + 1
			Redim Preserve g_PostShellCommands(n)
			g_PostShellCommands(n) = "S" & strTagText
		elseif UCaseTag = "SHELLASYNC" Then
			n = UBound(g_PostShellCommands) + 1
			Redim Preserve g_PostShellCommands(n)
			g_PostShellCommands(n) = "A" & strTagText
		else
			strAux = "Invalid tag in PostCommands: " & strTag
			QuitWithError strAux
		End If 
	wend
End Sub
'---------------------------------------------------------------------------------------------
' Function:  ReadCustomBreakPoints
'
'    Parses BreakPoints from a string
'
'    Input:
'      strOrigin - the string to be parsed
'
'    <!-- defining breakpoints -->
'    <Breakpoints>
'       <NewBP>
'          <Address> Address </Address>
'          <Type> Address </Type>
'          <Passes> Passes  </Passes>  (optional)
'          <Actions> actions  </Actions>  (optional)
'          <CustomActions> customactions  </CustomActions>  (optional)
'          <ReturnAction> G or Q  </ReturnAction>  (optional)
'       </NewBP>  
'       <Config>
'          <Address> Address or AllBreakpoints </Address>
'          <Passes> Passes  </Passes>  (optional)
'          <Actions> actions  </Actions>  (optional)
'          <CustomActions> customactions  </CustomActions>  (optional)
'          <ReturnAction> G or Q  </ReturnAction>  (optional)
'       </Config>  
'    </Breakpoints>
'
'            Created by Israel Burman on 5/30/2002
'
'---------------------------------------------------------------------------------------------
Sub ReadCustomBreakPoints(strOrigin)
	dim Position ' current working position
	dim Position1 ' begining position of some tag text
	dim Position2 ' related ending position
	dim p1,p2 ' temporary positions
	dim TotalLen ' size of strOrigin
	dim arrAux ' an array of strings
	dim strAux, strAux2, strTag, strTagText
	dim strAddress, strPasses, strCommands, strCustomCommands, strReturnAction
	dim strAdrType
	dim ArrAddress
	dim UCaseTag
	dim i,n,j,nArrAddress

	AdpTrace "In the ReadCustomBreakPoints() function "

	TotalLen = Len(strOrigin)
	Position = 1
	while Position < TotalLen
		strTag = XMLFindFirstTag(Position, strOrigin)
		if strTag = "" Then Exit Sub
		UCaseTag = Trim(UCase(strTag))
		strTagText = XMLGetTagText(Position, strOrigin, strTag, Position1, Position2, TRUE)
		' updating working position
		Position = Position2 + 1
		' as both NewBP and Config have the same sub tags, we interprete the sub-tags first
		' parsing the current command
		strAddress = XMLGetTagText(1, strTagText, "Address", p1, p2, TRUE)
		if Trim(strAddress)="" Then
			strAux = "Invalid address in breakpoint command: " & strTagText
			QuitWithError strAux
		End If
		' defining the array of addresses (addr1;addr2;addr3)
		ArrAddress = GetStringArray(strAddress,";")
		nArrAddress = UBound(ArrAddress)
		if nArrAddress<=0 Then
			strAux = "Invalid address in breakpoint command: " & strTagText
			QuitWithError strAux
		End If

		strAdrType = XMLGetTagText(1, strTagText, "Type", p1, p2, TRUE)
		if Trim(strAdrType)="" Then
			strAdrType = "bp"
		End If
		strAdrType = LCase(Trim(strAdrType))

		if strAdrType<>"bp" and strAdrType<>"bu" and strAdrType<>"bm" Then
			strAux = "Invalid type in breakpoint command: " & strTagText
			QuitWithError strAux
		End If



		strPasses = XMLGetTagText(1, strTagText, "Passes", p1, p2, TRUE)
		strReturnAction = XMLGetTagText(1, strTagText, "ReturnAction", p1, p2, TRUE)
		if strReturnAction <> "" And strReturnAction <> "G" And strReturnAction <> "Q" And strReturnAction <> "QD" And strReturnAction <> "VOID" Then
			strAux = "Invalid return action in breakpoint command: " & strReturnAction
			QuitWithError strAux
		End If
		strAux = Trim(XMLGetTagText(1, strTagText, "Actions", p1, p2, TRUE))
		strAux2 = XMLGetTagText(1, strTagText, "CustomActions", p1, p2, TRUE)
		' replacing special XML characters
		strCustomCommands = Trim(ReplaceReservedChars(strAux2))
		strCommands = strAux
		'strCommands = ParseActionsFromConfig(strAux, strAux2)

		if UCaseTag = "NEWBP" Then
			for j = 1 to nArrAddress
				AddBreakPoint ArrAddress(j)
				' defining custom settings
				n = UBound(g_BPActions)
				g_BPActions(n).AdrType = strAdrType
				if strPasses <> "" Then g_BPActions(n).Passes = strPasses
				if strCommands <> "" Then g_BPActions(n).Commands = strCommands
				if strCustomCommands <> "" Then g_BPActions(n).CustomCommands = strCustomCommands
				if strReturnAction <> "" Then g_BPActions(n).ReturnAction = strReturnAction
			Next
			

		elseif UCaseTag = "CONFIG" Then
			n = UBound(g_BPActions)

			for i=0 to n
				for j = 1 to nArrAddress
					if g_BPActions(i).Address = ArrAddress(j) or UCase(ArrAddress(j)) = "ALLBREAKPOINTS" Then
						if strPasses <> "" Then g_BPActions(i).Passes = strPasses
						if strCommands <> "" Then g_BPActions(i).Commands = strCommands
						if strCustomCommands <> "" Then g_BPActions(i).CustomCommands = strCustomCommands
						if strReturnAction <> "" Then g_BPActions(i).ReturnAction = strReturnAction
						Exit For
					End If
				Next
			Next

		else
			strAux = "Invalid tag in Breakpoints Commands: " & strTag
			QuitWithError strAux
		End If
	wend
End Sub
'---------------------------------------------------------------------------------------------
' Function:  ReadCustomExceptions
'
'    Parses Exceptions from a string
'
'    Input:
'      strOrigin - the string to be parsed
'
'    <!-- commands acting on the exception actions -->
'    <Exceptions>
'          <!-- options act on currently defined exceptions -->
'       <Option>  FullDumpOnFirstChance  </Option>
'       <Option>  MiniDumpOnSecondChance  </Option>
'       <Option>  NoDumpOnFirstChance  </Option>
'       <Option>  NoDumpOnSecondChance  </Option>
'       <NewException>
'           <Code> ExceptionCode </Code>
'           <Name> ExceptionName </Name>
'       </NewException>
'       <Config>
'          <Code>
'             an exception code or AllExceptions (where to apply this configuration)
'          </Code>
'          <Actions1>   actions     </Actions1>
'          <CustomActions1>   customactions     </CustomActions1>
'          <Actions2>   actions     </Actions2>
'          <CustomActions2>   customactions     </CustomActions2>
'                   actions:  Log;Stack;
'                   customactions: !locks;!runaway
'          <ReturnAction1> GN or GH or Q    </ReturnAction1>
'          <ReturnAction2> GN or Q    </ReturnAction2>
'       </Config>'
'    </Exceptions>
'
'            Created by Israel Burman on 5/30/2002
'
'---------------------------------------------------------------------------------------------
Sub ReadCustomExceptions(strOrigin)
	dim Position ' current working position
	dim Position1 ' begining position of some tag text
	dim Position2 ' related ending position
	dim p1,p2 ' temporary positions
	dim TotalLen ' size of strOrigin
	dim arrAux ' an array of strings
	dim strAux, strAux2, strTag, strTagText
	dim strExceptionCode, strExceptionName, strCommands1, strCommands2, strReturnAction1, strReturnAction2
	dim strCustomCommands1, strCustomCommands2
	dim UCaseTag, UCaseTagText
	dim ArrCodes
	dim i,n,j,nArrCodes

	AdpTrace "In the ReadCustomExceptions() function "

	TotalLen = Len(strOrigin)
	Position = 1
	while Position < TotalLen
		strTag = XMLFindFirstTag(Position, strOrigin)
		if strTag = "" Then Exit Sub
		strTagText = XMLGetTagText(Position, strOrigin, strTag, Position1, Position2, TRUE)
		' updating working position
		Position = Position2 + 1
		' parsing the current command
		UCaseTag = Trim(UCase(strTag))
		UCaseTagText = Trim(UCase(strTagText))
		if UCaseTag = "OPTION" Then
			if UCaseTagText = "FULLDUMPONFIRSTCHANCE" Then
				n = UBound(g_ExceptionActions)
				For i=0 to n
					if IsExceptionSpecial(g_ExceptionActions(i).ExceptionCode) = False Then
						g_ExceptionActions(i).Commands1 = ReplaceDumpType(g_ExceptionActions(i).Commands1,"FullDumpOver")
					End If
				Next
			elseif UCaseTagText = "MINIDUMPONSECONDCHANCE" Then
				n = UBound(g_ExceptionActions)
				For i=0 to n
					if IsExceptionSpecial(g_ExceptionActions(i).ExceptionCode) = False Then
						g_ExceptionActions(i).Commands2 = ReplaceDumpType(g_ExceptionActions(i).Commands2,"MiniDump")
					End If
				Next
			elseif UCaseTagText = "NODUMPONFIRSTCHANCE" Then
				n = UBound(g_ExceptionActions)
				For i=0 to n
					if IsExceptionSpecial(g_ExceptionActions(i).ExceptionCode) = False Then
						g_ExceptionActions(i).Commands1 = ReplaceDumpType(g_ExceptionActions(i).Commands1,"NoDump")
					End If
				Next
			elseif UCaseTagText = "NODUMPONSECONDCHANCE" Then
				n = UBound(g_ExceptionActions)
				For i=0 to n
					if IsExceptionSpecial(g_ExceptionActions(i).ExceptionCode) = False Then
						g_ExceptionActions(i).Commands2 = ReplaceDumpType(g_ExceptionActions(i).Commands2,"NoDump")
					End If
				Next

			End If

		elseif UCaseTag = "NEWEXCEPTION" Then
			strExceptionCode = XMLGetTagText(1, strTagText, "Code", p1, p2, TRUE)
			strExceptionName = XMLGetTagText(1, strTagText, "Name", p1, p2, TRUE)
			if strExceptionCode = "" or strExceptionName = "" Then
				strAux = "Missing Exception name or code: " & strTagText
				QuitWithError strAux
			End If
			AddException strExceptionCode, strExceptionName		


		elseif UCaseTag = "CONFIG" Then
			strExceptionCode = XMLGetTagText(1, strTagText, "Code", p1, p2, TRUE)
			if Trim(strExceptionCode)="" Then
				strAux = "Missing Exception Code in Exception Config command: " & strTagText
				QuitWithError strAux
			End If
			' defining the array of exception codes
			ArrCodes = GetStringArray(UCase(strExceptionCode), ";")
			nArrCodes = UBound(ArrCodes)
			if nArrCodes<=0 Then
				strAux = "Missing Exception Code in Exception Config command: " & strTagText
				QuitWithError strAux
			End If

			strReturnAction1 = XMLGetTagText(1, strTagText, "ReturnAction1", p1, p2, TRUE)
			strReturnAction1 = UCase(strReturnAction1)
			if strReturnAction1 <> "" And strReturnAction1 <> "GN" And strReturnAction1 <> "GH" And strReturnAction1 <> "Q" And strReturnAction1 <> "QD" And strReturnAction1 <> "VOID" Then
				strAux = "Invalid return action1 in exception command: " & strReturnAction1
				QuitWithError strAux
			End If
			strReturnAction2 = XMLGetTagText(1, strTagText, "ReturnAction2", p1, p2, TRUE)
			strReturnAction2 = UCase(strReturnAction2)
			if strReturnAction2 <> "" And strReturnAction2 <> "GN" And strReturnAction2 <> "Q" And strReturnAction2 <> "QD" And strReturnAction2 <> "VOID" Then
				strAux = "Invalid return action2 in exception command: " & strReturnAction2
				QuitWithError strAux
			End If
			strAux = Trim(XMLGetTagText(1, strTagText, "Actions1", p1, p2, TRUE))
			strAux2 = XMLGetTagText(1, strTagText, "CustomActions1", p1, p2, TRUE)
			' replacing special XML characters
			strCustomCommands1 = Trim(ReplaceReservedChars(strAux2))
			strCommands1 = strAux
			'strCommands1 = ParseActionsFromConfig(strAux,strAux2)
			strAux = Trim(XMLGetTagText(1, strTagText, "Actions2", p1, p2, TRUE))
			strAux2 = XMLGetTagText(1, strTagText, "CustomActions2", p1, p2, TRUE)
			' replacing special XML characters
			strCustomCommands2 = Trim(ReplaceReservedChars(strAux2))
			strCommands2 = strAux
			'strCommands2 = ParseActionsFromConfig(strAux,strAux2)

			n = UBound(g_ExceptionActions)
			for i=0 to n
				for j = 1 to nArrCodes
					if UCase(g_ExceptionActions(i).ExceptionCode) = ArrCodes(j) or ArrCodes(j) = "ALLEXCEPTIONS" Then
						if strCommands1 <> "" Then g_ExceptionActions(i).Commands1 = strCommands1
						if strCommands2 <> "" Then g_ExceptionActions(i).Commands2 = strCommands2
						if strCustomCommands1 <> "" Then g_ExceptionActions(i).CustomCommands1 = strCustomCommands1
						if strCustomCommands2 <> "" Then g_ExceptionActions(i).CustomCommands2 = strCustomCommands2
						if strReturnAction1 <> "" Then g_ExceptionActions(i).ReturnAction1 = strReturnAction1
						if strReturnAction2 <> "" Then g_ExceptionActions(i).ReturnAction2 = strReturnAction2
						exit for
					End If
				Next
			Next
		else
			strAux = "Invalid tag in Exceptions Command: " & strTag
			QuitWithError strAux
		End If 
	wend

End Sub
'---------------------------------------------------------------------------------------------
' Function:  ParseActionsFromConfig
'
'    Transforms a actions and custom actions in the format
'      actions= Command1;Command2;
'      Customactions= !locks;!runaway
'    In a string of format
'      Command1;Command2;Custom_cmd1 & Chr(10) & cmd2 & Chr(10) & cmd3
'
'    Input:
'      actions and customactions (2 strings)
'    Return:
'      The internal format for Commands
'
'            Created by Israel Burman on 5/30/2002
'
'---------------------------------------------------------------------------------------------
Function ParseActionsFromConfig(strActions, strCustomActions)

	dim strOut ' parsed string
	dim strAux
	dim arrAux ' string arrays
	dim i,n

'
'  this function is deprecated because now I store custom actions in a separate field
'
	strAux = "Calling deprecated function ParseActionsFromConfig"
	QuitWithError strAux
	
	AdpTrace "In the ParseActionsFromConfig() function "

	if Trim(strActions)="" And Trim(strCustomActions)="" Then
		ParseActionsFromConfig = ""
		Exit Function
	End If
	
	strOut = Trim(strActions)
	strAux = Trim(strCustomActions)
	if strAux <> "" Then
		arrAux = GetStringArray(strAux,";")
		if strOut<> "" Then strOut = strOut & ";"
		strOut = strOut & "Custom_" & arrAux(1)
		n = UBound(arrAux)
		if n>1 Then
			for i=2 to n
				strOut = strOut & Chr(10) & arrAux(i)
			Next
		End If
	End If
	ParseActionsFromConfig = strOut
End Function
'---------------------------------------------------------------------------------------------
' Function:  ReadCustomSettings
'
'    Parses Settings from a string
'
'    Input:
'      strOrigin - the string to be parsed
'
'            Created by Israel Burman on 5/30/2002
'
'  <Settings>
'     <RunMode> runmode </RunMode>   (runmode = CRASH, HANG, QUICK)
'     <Option> IIS </Option>
'     <Option> QUIET </Option>
'     <Option> DumpOnly </Option>
'     <Option> NoTlist </Option>
'     <Option> NoTsCheck </Option>
'     <OutputDir> OutputDirectoryName </OutputDir>
'     <ProcessID> pid </ProcessID>    *1
'     <ProcessName> processname </ProcessName>   *1
'     <GenerateScript> scriptname </GenerateScript>   
'     <Spawn> Spawn Command </Spawn>   *1
'     <Sympath> symbol path </Sympath>
'     <SympathPlus> symbol path to add </SympathPlus>   *1
'     <Notify> ComputerName or UserName </Notify>   *1
'     <AttachInterval> inseconds </AttachInterval> 
'     <AttachRepeats> numberofrepeatedattachments </AttachRepeats> 
'     <LastScriptCommand> Q or QD or G or GN or VOID  </LastScriptCommand>
'     <Debugger> cdb.exe or windbg.exe or ntsd.exe </Debugger>
'          *1 may include several instances of this tag
'  </Settings>
'
'---------------------------------------------------------------------------------------------
Sub ReadCustomSettings(strOrigin)
	dim Position ' current working position
	dim Position1 ' begining position of some tag text
	dim Position2 ' related ending position
	dim TotalLen ' size of strOrigin
	dim strAux, strTag, strTagText
	dim UCaseTag, UTagText
	dim n 

	AdpTrace "In the ReadCustomSettings() function . . ."

	TotalLen = Len(strOrigin)
	Position = 1
	while Position < TotalLen
		strTag = XMLFindFirstTag(Position, strOrigin)
		if strTag = "" Then Exit Sub
		strTagText = XMLGetTagText(Position, strOrigin, strTag, Position1, Position2, TRUE)
		' updating working position
		Position = Position2 + 1
		' parsing the current command
		UCaseTag = Trim(UCase(strTag))
		if UCaseTag = "RUNMODE" Then
			if g_RunMode <> "" Then
				strAux = "RunMode redefined in config file "
				QuitWithError strAux
			End If
			UTagText = UCase(Trim(strTagText))
			if UTagText = "CRASH" or UTagText = "HANG" or UTagText = "QUICK" Then
				g_RunMode = UTagText
			else
				strAux = "Invalid RunMode defined in config file "
				QuitWithError strAux
			End If
		elseif UCaseTag = "OPTION" Then
			UTagText = UCase(Trim(strTagText))
			if UTagText = "IIS" Then
				g_IISMode = True
			elseif UTagText = "QUIET" Then
				g_QuietMode = True
			elseif UTagText = "DUMPONLY" Then
				g_DumpOnly = True
				DumpOnlySelected
			elseif UTagText = "NOTLIST" Then
				g_NoTlist = True
			elseif UTagText = "NOTSCHECK" Then
				g_CheckTS = False
			elseif UTagText = "NOFREESPACECHECKING" Then
				g_CheckFreeSpace = False
			else
				strAux = "Invalid Option defined in config file [" & strTagText & "]"
				QuitWithError strAux
			End If

		elseif UCaseTag = "LASTSCRIPTCOMMAND" Then
			UTagText = UCase(Trim(strTagText))
			if UTagText = "Q" Then
				g_LastScriptCommand = "Q"
			elseif UTagText = "QD" Then
				g_LastScriptCommand = "QD"
			elseif UTagText = "G" Then
				g_LastScriptCommand = "G"
			elseif UTagText = "GN" Then
				g_LastScriptCommand = "GN"
			elseif UTagText = "VOID" Then
				g_LastScriptCommand = ""
			else
				strAux = "Invalid Option defined in config file [" & strTagText & "]"
				QuitWithError strAux
			End If

		elseif UCaseTag = "OUTPUTDIR" Then
			g_OutPutDir = strTagText
		elseif UCaseTag = "DEBUGGER" Then
			g_Debugger = strTagText
		elseif UCaseTag = "NOTIFY" Then
			n = UBound(g_NotifyList) + 1
			Redim Preserve g_NotifyList(n)
			g_NotifyList(n) = strTagText
		elseif UCaseTag = "ATTACHINTERVAL" Then
			g_AttachInterval = strTagText
		elseif UCaseTag = "ATTACHREPEATS" Then
			g_AttachRepeats = strTagText
		elseif UCaseTag = "PROCESSID" Then
			AddProcessID strTagText
		elseif UCaseTag = "PROCESSNAME" Then
			AddProcessName strTagText
		elseif UCaseTag = "GENERATESCRIPT" Then
			g_GeneratedScriptName = strTagText
		elseif UCaseTag = "SPAWN" Then
			AddSpawningCommand strTagText
		elseif UCaseTag = "SYMPATH" Then
			g_Sympath = strTagText
		elseif UCaseTag = "SYMPATHPLUS" Then
			if g_SympathPlus <> "" Then
				g_SympathPlus = g_SympathPlus & ";"
			End If
			g_SympathPlus = g_SympathPlus & strTagText
		else
			strAux = "Invalid tag in Custom Settings: " & strTag
			QuitWithError strAux
		End If 
	wend
End Sub

'---------------------------------------------------------------------------------------------
' Function:  ReadCustomHangActions
'
'    Parses Settings from a string
'
'    Input:
'      strOrigin - the string to be parsed
'
'            Created by Israel Burman on 5/30/2002
'
'  <HangActions>
'     <Option> MiniDump </Option>
'     <Option> FullDump </Option>
'     <Option> NoDump </Option>
'     <Option> Clear </Option>  (deselects all current options)
'     <Actions> actions  </Actions>  (optional - activates the selected actions)
'     <CustomActions> customactions  </CustomActions>  (optional - adds custom actions to what is already defined)
'  </HangActions>
'
'---------------------------------------------------------------------------------------------
Sub ReadCustomHangActions(strOrigin)
	dim Position ' current working position
	dim Position1 ' begining position of some tag text
	dim Position2 ' related ending position
	dim TotalLen ' size of strOrigin
	dim strAux, strTag, strTagText
	dim arrAux ' an array of strings
	dim UCaseTag, UTagText
	dim UCaseCmd
	dim i,n,j,m
	dim bFound

	AdpTrace "In the ReadCustomHangActions() function . . ."

	TotalLen = Len(strOrigin)
	Position = 1
	while Position < TotalLen
		strTag = XMLFindFirstTag(Position, strOrigin)
		if strTag = "" Then Exit Sub
		strTagText = XMLGetTagText(Position, strOrigin, strTag, Position1, Position2, TRUE)
		' updating working position
		Position = Position2 + 1
		' parsing the current command
		UCaseTag = Trim(UCase(strTag))
		if UCaseTag = "OPTION" Then
			UTagText = UCase(Trim(strTagText))
			if UTagText = "MINIDUMP" Then
				g_HangActions(0).Name = "MiniDump"
			elseif UTagText = "FULLDUMP" Then
				g_HangActions(0).Name = "FullDump"
			elseif UTagText = "NODUMP" Then
				g_HangActions(0).Selected = False
			elseif UTagText = "CLEAR" Then
				n = UBound(g_HangActions)
				for i=0 to n
					g_HangActions(i).Selected = False
				Next
			else
				strAux = "Invalid Hang Mode option defined in config file [" & strTag & "]"
				QuitWithError strAux
			End If
		elseif UCaseTag = "ACTIONS" Then
			arrAux = GetStringArray(Trim(strTagText),";")
			n = UBound(arrAux)
			If n<=0 Then
				strAux = "Empty actions tag for Hang Mode in config file "
				QuitWithError strAux
			End If

			m = UBound(g_HangActions)
			For i = 1 to n
				arrAux(i) = Trim(arrAux(i))
				UCaseCmd = UCase(arrAux(i))

				if UCaseCmd = "MINIDUMP" Then
					g_HangActions(0).Name = "MiniDump"
					g_HangActions(0).Selected = True
				elseif UCaseCmd = "FULLDUMP" Then
					g_HangActions(0).Name = "FullDump"
					g_HangActions(0).Selected = True
				elseif UCaseCmd = "NODUMP" Then
					g_HangActions(0).Selected = False
 				else
					bFound = False
					for j=0 to m
						if UCase(g_HangActions(j).Name) = UCaseCmd Then
							g_HangActions(j).Selected = True
							bFound = True
						End If
					Next
					if bFound = False Then
						if UCaseCmd <> "" Then
							strAux = "Invalid action for Hang Mode in config file [" & arrAux(i) & "]"
							QuitWithError strAux
						End If
					End If
				End If
			Next

		elseif UCaseTag = "CUSTOMACTIONS" Then
			arrAux = GetStringArray(Trim(strTagText),";")
			n = UBound(arrAux)
			If n<=0 Then
				strAux = "Empty CustomActions tag for Hang Mode in config file "
				QuitWithError strAux
			End If

			For i = 1 to n
				arrAux(i) = Trim(arrAux(i)) 
				m = UBound(g_HangActions) + 1
				Redim Preserve g_HangActions(m)
				set g_HangActions(m) = new HangAction
				g_HangActions(m).Name = ""
				g_HangActions(m).Description = ""
				g_HangActions(m).Command = arrAux(i)
				g_HangActions(m).Selected = True
			Next
		else
			strAux = "Invalid tag in HangMode settings: " & strTag
			QuitWithError strAux
		End If 
	wend
End Sub

'---------------------------------------------------------------------------------------------
' Function:  CloseAllFiles
'
'            Created by Israel Burman on 5/30/2002
'
'---------------------------------------------------------------------------------------------
Sub CloseAllFiles()

	if Not g_AdpLogFile is Nothing Then g_AdpLogFile.Close


End Sub
'---------------------------------------------------------------------------------------------
' Function:  IsExceptionSpecial
'
'     We use this function to give special treatment to some exceptions
'     when we make changes to default behaviors that should affect all exceptions
'     such as MiniOnSecond, etc. 
'     We don't want these changes to affect some special exceptions like ctl-c, etc.
'
'---------------------------------------------------------------------------------------------
Function IsExceptionSpecial(ExceptionCode)

	IsExceptionSpecial = False

	' CONTRL_C_OR_Debug_Break
	if ExceptionCode = "bpe" Then IsExceptionSpecial = True

	' STATUS_WAKE_SYSTEM_DEBUGGER
	if ExceptionCode = "0x80000007" Then IsExceptionSpecial = True

	' DLL_Load
	if ExceptionCode = "ld" Then IsExceptionSpecial = True

	' DLL_UnLoad
	if ExceptionCode = "ud" Then IsExceptionSpecial = True

	' Process_Shut_Down
	if ExceptionCode = "epr" Then IsExceptionSpecial = True



End Function
'---------------------------------------------------------------------------------------------
' Function:  CompareWildProcessName
'
'    Checks if a actual process matches a selected name with wild cards
'       Ex.: notes*.exe
'      
'            Created by Israel Burman on 9/25/2002
'
'---------------------------------------------------------------------------------------------
Function CompareWildProcessName(SelectedName, ProcessName )
	Dim SelectedExtension
	Dim SelectedBase
	Dim ProcessExtension
	Dim ProcessBase
	Dim SelectedNucleus
	Dim ip,n

	AdpTrace "In the CompareWildProcessName() function "

	CompareWildProcessName = 1

	n = Len(SelectedName)
	if n<= 0 Then Exit Function
	ip = InStr(1,SelectedName,".")
	if ip <=0 Then Exit Function
	SelectedExtension = Right(SelectedName, n - ip)
	SelectedBase = Left(SelectedName, ip - 1)
	if SelectedBase = "" Then Exit Function

	n = Len(ProcessName)
	if n<= 0 Then Exit Function
	ip = InStr(1,ProcessName,".")
	if ip <=0 Then Exit Function
	ProcessExtension = Right(ProcessName, n - ip)
	ProcessBase = Left(ProcessName, ip - 1)
	if ProcessBase = "" Then Exit Function

	if SelectedExtension <> ProcessExtension Then Exit Function

	' checking with * on both sides *something*
	if Left(SelectedBase,1) = "*" And Right(SelectedBase,1) = "*" Then
		n = Len(SelectedBase)
		SelectedNucleus = Mid(SelectedBase,2,n-2)
		if InStr(1, ProcessBase, SelectedNucleus) > 0 Then
			CompareWildProcessName = 0
			Exit Function
		End If

		Exit Function
	End If

	' checking with * on the left *something
	if Left(SelectedBase,1) = "*" Then
		n = Len(SelectedBase)
		n = n - 1
		SelectedNucleus = Right(SelectedBase,n)
		if Right(ProcessBase, n) = SelectedNucleus Then
			CompareWildProcessName = 0
			Exit Function
		End If

		Exit Function
	End If

	' checking with * on the right something*
	if Right(SelectedBase,1) = "*" Then
		n = Len(SelectedBase)
		n = n - 1
		SelectedNucleus = Left(SelectedBase,n)
		if Left(ProcessBase, n) = SelectedNucleus Then
			CompareWildProcessName = 0
			Exit Function
		End If

		Exit Function
	End If



End Function

'---------------------------------------------------------------------------------------------
' Function:  CleanStringForFileName
'
'    Replaces invalid characters in file names for "_"
'      
'            Created by Israel Burman
'
'---------------------------------------------------------------------------------------------
Function CleanStringForFileName(strText )

	strText =Replace(strText,"/",",")
	strText =Replace(strText,"\",",")
	strText =Replace(strText,"<","(")
	strText =Replace(strText,">",")")
	strText =Replace(strText,":","")
	strText =Replace(strText,"*","")
	strText =Replace(strText,"?","")
	strText =Replace(strText,"|","")

	strText =Replace(strText," ","_")

	strText =Replace(strText,",,","-")

	strText =Replace(strText,",","-")
	'if strPackage comes from IIS 6 (-ap) it may have " delimiters. Cleaning this up as well
	strText =Replace(strText,"""","-")

	CleanStringForFileName = strText
End Function
'---------------------------------------------------------------------------------------------
' Function:  ReplaceReservedChars
'
'    Replaces special reserved xml chars
'
'       &amp(&), &lt(<), &gt(>), &apos('), &quot(")
'      
'            Created by Israel Burman
'
'---------------------------------------------------------------------------------------------
Function ReplaceReservedChars(strText )

	strText =Replace(strText,"&amp;","&")
	strText =Replace(strText,"&AMP;","&")
	strText =Replace(strText,"&Amp;","&")

	strText =Replace(strText,"&lt;","<")
	strText =Replace(strText,"&LT;","<")
	strText =Replace(strText,"&Lt;","<")

	strText =Replace(strText,"&gt;",">")
	strText =Replace(strText,"&GT;",">")
	strText =Replace(strText,"&Gt;",">")

	strText =Replace(strText,"&apos;","'")
	strText =Replace(strText,"&APOS;","'")
	strText =Replace(strText,"&Apos;","'")

	strText =Replace(strText,"&quot;","""")
	strText =Replace(strText,"&QUOT;","""")
	strText =Replace(strText,"&Quot;","""")


	ReplaceReservedChars = strText
End Function

'---------------------------------------------------------------------------------------------
' Function:  DumpOnlySelected
'
'    Changes default behavior to only create dumps (no additional info like call stacks)
'
'      
'            Created by Israel Burman
'
'---------------------------------------------------------------------------------------------
Sub DumpOnlySelected( )

	Dim i,n

	' in Hang mode I keep only the first action (Dump)
	n = UBound(g_HangActions)
	for i=1 to n
		g_HangActions(i).Selected = False
	Next

	' in Crash mode I keep only Dump, Time and Log and EventLog
	' I change only the exceptions that are still equal to the initial default definition
	' which is stored in Exception(0)

	n = UBound(g_ExceptionActions)
	for i=1 to n
		if g_ExceptionActions(i).Commands1 = g_ExceptionActions(0).Commands1 Then
			g_ExceptionActions(i).Commands1 = "Log;Time;MiniDump"
		End If
		if g_ExceptionActions(i).Commands2 = g_ExceptionActions(0).Commands2 Then
			g_ExceptionActions(i).Commands2 = "Log;Time;FullDump;EventLog"
		End If
	Next



End Sub
'---------------------------------------------------------------------------------------------
' Function:  InternalPrintf
'
'    Prepares a .printf command that can be used inside an SXE or BP command
'
'       this implies in:
'         replace all internals % with %% - this is required by the printf itself
'         replace the double-quote from the printf formatter string with a \"
'           this is required by the debugger because the sxe already has a double quote
'                  sxe -c "command" = sxe -c " .printf \"string\" "
'         replace the internal double quotes of the string with \\\"
'           on the conversion -c it will be converted to \"
'           in the conversion of the .printf it will be converted to "
'      
'            Created by Israel Burman
'
'---------------------------------------------------------------------------------------------
Function InternalPrintf(strText )
	Dim strOut

	strOut = strText

	strOut =Replace(strOut,"%","%%")
	strOut =Replace(strOut,"""","\\\" & """")

	InternalPrintf = ".printf \" & """" & strOut & "\" & """"
End Function
'' SIG '' Begin signature block
'' SIG '' MIIlcwYJKoZIhvcNAQcCoIIlZDCCJWACAQExCzAJBgUr
'' SIG '' DgMCGgUAMGcGCisGAQQBgjcCAQSgWTBXMDIGCisGAQQB
'' SIG '' gjcCAR4wJAIBAQQQTvApFpkntU2P5azhDxfrqwIBAAIB
'' SIG '' AAIBAAIBAAIBADAhMAkGBSsOAwIaBQAEFK3YUlXcyOEI
'' SIG '' taimwhM9f9A5cxBxoIIgWDCCBIUwggNtoAMCAQICCmEF
'' SIG '' 9x4AAAAAADIwDQYJKoZIhvcNAQEFBQAweTELMAkGA1UE
'' SIG '' BhMCVVMxEzARBgNVBAgTCldhc2hpbmd0b24xEDAOBgNV
'' SIG '' BAcTB1JlZG1vbmQxHjAcBgNVBAoTFU1pY3Jvc29mdCBD
'' SIG '' b3Jwb3JhdGlvbjEjMCEGA1UEAxMaTWljcm9zb2Z0IENv
'' SIG '' ZGUgU2lnbmluZyBQQ0EwHhcNMDkwNzEzMjMwMDE4WhcN
'' SIG '' MTAxMDEzMjMxMDE4WjCBgzELMAkGA1UEBhMCVVMxEzAR
'' SIG '' BgNVBAgTCldhc2hpbmd0b24xEDAOBgNVBAcTB1JlZG1v
'' SIG '' bmQxHjAcBgNVBAoTFU1pY3Jvc29mdCBDb3Jwb3JhdGlv
'' SIG '' bjENMAsGA1UECxMETU9QUjEeMBwGA1UEAxMVTWljcm9z
'' SIG '' b2Z0IENvcnBvcmF0aW9uMIIBIjANBgkqhkiG9w0BAQEF
'' SIG '' AAOCAQ8AMIIBCgKCAQEAtYypnJSEYRrMLiLEoJNkCd6F
'' SIG '' obCQXfzJbGi18e/8jmv0+KBu11HeYKMnY9T237ZM0kUL
'' SIG '' pz3Yb/7tpxhj0x/GgdS/BzeXQCt/519aNdmTT6vzKpLw
'' SIG '' po2B7AUk3Nu4YWNTYQI1ONa49e57VKbJWDXp9BXv2gds
'' SIG '' BS1NpLa6mzu4MTB+RbypoSU5DEOkRzuXnCBEcjfPUKBN
'' SIG '' TfTDZdxE0Qg7ON0+xRdsRsetcyTAj4nB6uq6zWipEtZY
'' SIG '' DFMWDP45A9aB8j5cV/N622bhBxjZGQMU9uZFmSpRTJDX
'' SIG '' Wu+vtzsMKU1tIK4Ht5Iu6GnKnzBC58MyigsiSjLonAzG
'' SIG '' UQMmAW2ScwIDAQABo4IBAjCB/zATBgNVHSUEDDAKBggr
'' SIG '' BgEFBQcDAzAdBgNVHQ4EFgQUh4G33+76d/pZBWM3BArX
'' SIG '' 6dqg4OAwDgYDVR0PAQH/BAQDAgeAMB8GA1UdIwQYMBaA
'' SIG '' FFdFdBxdsPbIQwXgjFQtjzKn/kiWMEkGA1UdHwRCMEAw
'' SIG '' PqA8oDqGOGh0dHA6Ly9jcmwubWljcm9zb2Z0LmNvbS9w
'' SIG '' a2kvY3JsL3Byb2R1Y3RzL0NvZGVTaWdQQ0EuY3JsME0G
'' SIG '' CCsGAQUFBwEBBEEwPzA9BggrBgEFBQcwAoYxaHR0cDov
'' SIG '' L3d3dy5taWNyb3NvZnQuY29tL3BraS9jZXJ0cy9Db2Rl
'' SIG '' U2lnUENBLmNydDANBgkqhkiG9w0BAQUFAAOCAQEANiGE
'' SIG '' 9Y+DIU2HqD+L58WkD0wwbll0mwuXdBapsfNwpK/CDDMk
'' SIG '' 86BK+sHkIca22epcW5Fh9yJt/zH8F07XzUDjndLltNL6
'' SIG '' jWwH2neayivuDVlHzwTCEGf66ulDO7fm0ZzZjiNgbhKb
'' SIG '' w9l+XFcy3ZQN333FSJJNSHa3osBTlT78OqOTfPKFBMfG
'' SIG '' hTiXNvgSkyIAmU55r5brWiCDrNBcWjCwAOJrOY8kido9
'' SIG '' DKRGeWI2U2rXyc2SCPuH5VT1fFnFxHd9sMEsmMeXbEOn
'' SIG '' +CD4g1RknmD44kqlZ9RJv1SIfVJE81HAJtHFfPCJ1hEJ
'' SIG '' KMJFRYAdkCK9jqIFPjyzT73XOeCIkzCCBM0wggO1oAMC
'' SIG '' AQICCmEWtSkAAAAAABAwDQYJKoZIhvcNAQEFBQAwdzEL
'' SIG '' MAkGA1UEBhMCVVMxEzARBgNVBAgTCldhc2hpbmd0b24x
'' SIG '' EDAOBgNVBAcTB1JlZG1vbmQxHjAcBgNVBAoTFU1pY3Jv
'' SIG '' c29mdCBDb3Jwb3JhdGlvbjEhMB8GA1UEAxMYTWljcm9z
'' SIG '' b2Z0IFRpbWUtU3RhbXAgUENBMB4XDTEwMDEwNDIxMTIw
'' SIG '' M1oXDTEzMDEwNDIxMjIwM1owgbYxCzAJBgNVBAYTAlVT
'' SIG '' MRMwEQYDVQQIEwpXYXNoaW5ndG9uMRAwDgYDVQQHEwdS
'' SIG '' ZWRtb25kMR4wHAYDVQQKExVNaWNyb3NvZnQgQ29ycG9y
'' SIG '' YXRpb24xEDAOBgNVBAsTB25DaXBoZXIxJzAlBgNVBAsT
'' SIG '' Hm5DaXBoZXIgRFNFIEVTTjpBQ0QzLUFFNjYtRTBCNTEl
'' SIG '' MCMGA1UEAxMcTWljcm9zb2Z0IFRpbWUtU3RhbXAgU2Vy
'' SIG '' dmljZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoC
'' SIG '' ggEBAOMoqtk8Q0L4BhO64sf5wS9ZyzyV51SQbMNYlWKz
'' SIG '' D2y9g9stWIgIkgvbzXbHVsbg7irTiDtcyqlwowAjIixT
'' SIG '' OOhQx/EUCY0dy3yL9aYza6/IRl9Le17556bHL0KFZzaN
'' SIG '' YEryE2bcsDltzhDK3XIFWHKd8gA7ADntJlM+Hs3uUJLN
'' SIG '' hfQ6/YGhBV8L6UQO79VZQCYoy4JOTivIFCK7cB3kPdH9
'' SIG '' YHZUTDns73c4i4z+dK/nFvRwMbjLrdTdi7b6AK3tdBSw
'' SIG '' Rb78ZYjhrfxJ21/R1M50edSNhm+z9ZcHIS2aYcn8uhtM
'' SIG '' 0uiruVDk0yv0wdlN1kCsFKeIhXNeaD5jt0HUZr0CAwEA
'' SIG '' AaOCARkwggEVMB0GA1UdDgQWBBTh8K4PDQX8MvMcfRl/
'' SIG '' z8TFv5FVTTAfBgNVHSMEGDAWgBQjNPjZUkZwCu1A+3b7
'' SIG '' syuwwzWzDzBUBgNVHR8ETTBLMEmgR6BFhkNodHRwOi8v
'' SIG '' Y3JsLm1pY3Jvc29mdC5jb20vcGtpL2NybC9wcm9kdWN0
'' SIG '' cy9NaWNyb3NvZnRUaW1lU3RhbXBQQ0EuY3JsMFgGCCsG
'' SIG '' AQUFBwEBBEwwSjBIBggrBgEFBQcwAoY8aHR0cDovL3d3
'' SIG '' dy5taWNyb3NvZnQuY29tL3BraS9jZXJ0cy9NaWNyb3Nv
'' SIG '' ZnRUaW1lU3RhbXBQQ0EuY3J0MBMGA1UdJQQMMAoGCCsG
'' SIG '' AQUFBwMIMA4GA1UdDwEB/wQEAwIGwDANBgkqhkiG9w0B
'' SIG '' AQUFAAOCAQEAjVtJKFSHyYqpk/6b+I1UcSCdwDiTJym5
'' SIG '' y5ZTios1tZQ3Fli9nj4XJ19ZL+Rdfh3iWdwH+2IY5WO/
'' SIG '' 9AvPTdpbMPbImv8mWb8ZwxCy8OMewQyZ2lWSxswS38BY
'' SIG '' TN2a1B0j+YYSROcmAuiAQWgWVDYdp2WzxDOh46SY2b5i
'' SIG '' kE+SN7apiBdUSB+dwiLYciq5wzCTPjfsKam5zgvVwskW
'' SIG '' pudeVQ3MgYRh6SwD25umG1MCOIsKWJEXyHFwFs4uxmP8
'' SIG '' eZ+DBIK0gecvV2mtuKUfIzZ29hWUhgoe6t0RO2ZE0hVa
'' SIG '' z6YrKB9OxQpzV0uVA2Ci6+kyfr09j6a9SSopwVcxwBDf
'' SIG '' UTCCBM0wggO1oAMCAQICCmEWtSkAAAAAABAwDQYJKoZI
'' SIG '' hvcNAQEFBQAwdzELMAkGA1UEBhMCVVMxEzARBgNVBAgT
'' SIG '' Cldhc2hpbmd0b24xEDAOBgNVBAcTB1JlZG1vbmQxHjAc
'' SIG '' BgNVBAoTFU1pY3Jvc29mdCBDb3Jwb3JhdGlvbjEhMB8G
'' SIG '' A1UEAxMYTWljcm9zb2Z0IFRpbWUtU3RhbXAgUENBMB4X
'' SIG '' DTEwMDEwNDIxMTIwM1oXDTEzMDEwNDIxMjIwM1owgbYx
'' SIG '' CzAJBgNVBAYTAlVTMRMwEQYDVQQIEwpXYXNoaW5ndG9u
'' SIG '' MRAwDgYDVQQHEwdSZWRtb25kMR4wHAYDVQQKExVNaWNy
'' SIG '' b3NvZnQgQ29ycG9yYXRpb24xEDAOBgNVBAsTB25DaXBo
'' SIG '' ZXIxJzAlBgNVBAsTHm5DaXBoZXIgRFNFIEVTTjpBQ0Qz
'' SIG '' LUFFNjYtRTBCNTElMCMGA1UEAxMcTWljcm9zb2Z0IFRp
'' SIG '' bWUtU3RhbXAgU2VydmljZTCCASIwDQYJKoZIhvcNAQEB
'' SIG '' BQADggEPADCCAQoCggEBAOMoqtk8Q0L4BhO64sf5wS9Z
'' SIG '' yzyV51SQbMNYlWKzD2y9g9stWIgIkgvbzXbHVsbg7irT
'' SIG '' iDtcyqlwowAjIixTOOhQx/EUCY0dy3yL9aYza6/IRl9L
'' SIG '' e17556bHL0KFZzaNYEryE2bcsDltzhDK3XIFWHKd8gA7
'' SIG '' ADntJlM+Hs3uUJLNhfQ6/YGhBV8L6UQO79VZQCYoy4JO
'' SIG '' TivIFCK7cB3kPdH9YHZUTDns73c4i4z+dK/nFvRwMbjL
'' SIG '' rdTdi7b6AK3tdBSwRb78ZYjhrfxJ21/R1M50edSNhm+z
'' SIG '' 9ZcHIS2aYcn8uhtM0uiruVDk0yv0wdlN1kCsFKeIhXNe
'' SIG '' aD5jt0HUZr0CAwEAAaOCARkwggEVMB0GA1UdDgQWBBTh
'' SIG '' 8K4PDQX8MvMcfRl/z8TFv5FVTTAfBgNVHSMEGDAWgBQj
'' SIG '' NPjZUkZwCu1A+3b7syuwwzWzDzBUBgNVHR8ETTBLMEmg
'' SIG '' R6BFhkNodHRwOi8vY3JsLm1pY3Jvc29mdC5jb20vcGtp
'' SIG '' L2NybC9wcm9kdWN0cy9NaWNyb3NvZnRUaW1lU3RhbXBQ
'' SIG '' Q0EuY3JsMFgGCCsGAQUFBwEBBEwwSjBIBggrBgEFBQcw
'' SIG '' AoY8aHR0cDovL3d3dy5taWNyb3NvZnQuY29tL3BraS9j
'' SIG '' ZXJ0cy9NaWNyb3NvZnRUaW1lU3RhbXBQQ0EuY3J0MBMG
'' SIG '' A1UdJQQMMAoGCCsGAQUFBwMIMA4GA1UdDwEB/wQEAwIG
'' SIG '' wDANBgkqhkiG9w0BAQUFAAOCAQEAjVtJKFSHyYqpk/6b
'' SIG '' +I1UcSCdwDiTJym5y5ZTios1tZQ3Fli9nj4XJ19ZL+Rd
'' SIG '' fh3iWdwH+2IY5WO/9AvPTdpbMPbImv8mWb8ZwxCy8OMe
'' SIG '' wQyZ2lWSxswS38BYTN2a1B0j+YYSROcmAuiAQWgWVDYd
'' SIG '' p2WzxDOh46SY2b5ikE+SN7apiBdUSB+dwiLYciq5wzCT
'' SIG '' PjfsKam5zgvVwskWpudeVQ3MgYRh6SwD25umG1MCOIsK
'' SIG '' WJEXyHFwFs4uxmP8eZ+DBIK0gecvV2mtuKUfIzZ29hWU
'' SIG '' hgoe6t0RO2ZE0hVaz6YrKB9OxQpzV0uVA2Ci6+kyfr09
'' SIG '' j6a9SSopwVcxwBDfUTCCBZkwggOBoAMCAQICEHmtFqFK
'' SIG '' oKWtTHNY9AcTLmUwDQYJKoZIhvcNAQEFBQAwXzETMBEG
'' SIG '' CgmSJomT8ixkARkWA2NvbTEZMBcGCgmSJomT8ixkARkW
'' SIG '' CW1pY3Jvc29mdDEtMCsGA1UEAxMkTWljcm9zb2Z0IFJv
'' SIG '' b3QgQ2VydGlmaWNhdGUgQXV0aG9yaXR5MB4XDTAxMDUw
'' SIG '' OTIzMTkyMloXDTIxMDUwOTIzMjgxM1owXzETMBEGCgmS
'' SIG '' JomT8ixkARkWA2NvbTEZMBcGCgmSJomT8ixkARkWCW1p
'' SIG '' Y3Jvc29mdDEtMCsGA1UEAxMkTWljcm9zb2Z0IFJvb3Qg
'' SIG '' Q2VydGlmaWNhdGUgQXV0aG9yaXR5MIICIjANBgkqhkiG
'' SIG '' 9w0BAQEFAAOCAg8AMIICCgKCAgEA8136gGfUWqepDCyQ
'' SIG '' INA1CDx1hM23B4mcidrezsNg+pFoWp6UcSkYdnzC4Mgl
'' SIG '' dpQOWPoENDbm36/3gLrpWAsrk+WdBeN3IpH3NGQ8IpEd
'' SIG '' XuEJkLwU/vx1WBnhebcHkqOuiFkI2J8HygNY/GgpbTLX
'' SIG '' 0qjLS/zhC0gyT+bruK1P5FxvE5SZ25XVdduoGreUkbR3
'' SIG '' W/VIDI9qeX0UcAR9ba+Q9dpw2Ee3v5svbOcFt+ERYKx5
'' SIG '' kRR8xdam5OF+1cN+5ZLSPAC1NoLeeeFt87Vu+J8zyctS
'' SIG '' fXOYNtuLoWuilZebo97CTSb/BpZnJQbI56zk7hIzlTGZ
'' SIG '' yDUITjTKeVPVtb5jMllANsClTgRNPdtbBzPkWL/vP1Nk
'' SIG '' 2EJZNVf9D0V8JARNntY4dBGXIpDOaER0km/VS2+whuPH
'' SIG '' NkKg0PzBwFr5o2G5MEdxlgoWsJHAQpXvEH8oauMqH7Hk
'' SIG '' zQM/d3EExyD8SQ8dRYik18t+iK2OLexF28RRBMkq/OyG
'' SIG '' npoRl1vezlOI5uK3/ayVwihA2+8EkN+BMznZskWlI4cG
'' SIG '' pVWJMbsGLWAOQRh9Hy61l8sR6xXVJKWU7xUUif1Lc/oy
'' SIG '' W/zRMwD5WWJwBzLqLqtALXvK3SFnGzCZjxaqI6hB0bBu
'' SIG '' EZs2xN5AdJzhWGXBYB56WzjIj7sEJnzUFkDltmtsqob9
'' SIG '' AL/OwTUCAwEAAaNRME8wCwYDVR0PBAQDAgHGMA8GA1Ud
'' SIG '' EwEB/wQFMAMBAf8wHQYDVR0OBBYEFA6sgmBAVieX5SUT
'' SIG '' /CrhClOVWeSkMBAGCSsGAQQBgjcVAQQDAgEAMA0GCSqG
'' SIG '' SIb3DQEBBQUAA4ICAQDFEU0DOmDdXVIRd4+yuzbIsgW/
'' SIG '' tLeo2CCdXBMDthwi+gYTNbbIY9SaR28mV9JV8QSxJl/W
'' SIG '' qVBooLzSuG7Mw+ms3xnNeKxZdKxmNDbEGz5sOEwzDjAS
'' SIG '' DaMm/lFTAP+vWk6EDQ8f5G0FLk6FS41sM29U0mSrv1Cv
'' SIG '' fXo5oDftYwMP/BMGzhY21FQ7lRtRYjrlTRfUBTmSmieo
'' SIG '' W6q97Lu+4yCJYHFsVrOlE9BtDiN+lQPtaD3y2GO4a022
'' SIG '' 6DC14cqUS/eiql2ZMLI9p8JRbCggASQnK0sAt50Ra3C+
'' SIG '' shCCvAybaNCNOySHqpkocp0zX1mQvfXek546Ylo0OeKI
'' SIG '' VR25BrDBiWst12nDGRI2hNDJoNr/L2l4suV62uvXDMD3
'' SIG '' vWMXuDkTOKI2W3vyhVZqHWRiwTjiqr9RZqKU9RKcZiIQ
'' SIG '' a/K3MJIt8inwPTsUQ2ii8ZwpN8vOOCAlbXxn834kEiQD
'' SIG '' CIFH7KWel/UY18+71e92lu/9zttWnZWgQvmXWOHXMSLT
'' SIG '' X1nmPm4iAOpDhLYl29nzCFZowGRrHXzstpOiYlduLtjn
'' SIG '' WI/EMUkm3d4pNYf1MHFwWxQ8ab2JEn3rLqP+2H+eglpS
'' SIG '' CivBQyvZMIifyBD7iY3moYV1M35snttzE2RiaaUvfcqW
'' SIG '' bZ/4BE0wkj1uIRQhyT3gw/2Ka51K/dGhnZlDdz+w2jCC
'' SIG '' BgcwggPvoAMCAQICCmEWaDQAAAAAABwwDQYJKoZIhvcN
'' SIG '' AQEFBQAwXzETMBEGCgmSJomT8ixkARkWA2NvbTEZMBcG
'' SIG '' CgmSJomT8ixkARkWCW1pY3Jvc29mdDEtMCsGA1UEAxMk
'' SIG '' TWljcm9zb2Z0IFJvb3QgQ2VydGlmaWNhdGUgQXV0aG9y
'' SIG '' aXR5MB4XDTA3MDQwMzEyNTMwOVoXDTIxMDQwMzEzMDMw
'' SIG '' OVowdzELMAkGA1UEBhMCVVMxEzARBgNVBAgTCldhc2hp
'' SIG '' bmd0b24xEDAOBgNVBAcTB1JlZG1vbmQxHjAcBgNVBAoT
'' SIG '' FU1pY3Jvc29mdCBDb3Jwb3JhdGlvbjEhMB8GA1UEAxMY
'' SIG '' TWljcm9zb2Z0IFRpbWUtU3RhbXAgUENBMIIBIjANBgkq
'' SIG '' hkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAn6Fssd/bSJIq
'' SIG '' fGsuGeG94uPFmVEjUK3O3RhOJA/u0afRTK10MCAR6wfV
'' SIG '' VJUVSZQbQpKumFwwJtoAa+h7veyJBw/3DgSY8InMH8sz
'' SIG '' JIed8vRnHCz8e+eIHernTqOhwSNTyo36Rc8J0F6v0LBC
'' SIG '' BKL5pmyTZ9co3EZTsIbQ5ShGLieshk9VUgzkAyz7apCQ
'' SIG '' MG6H81kwnfp+1pez6CGXfvjSE/MIt1NtUrRFkJ9IAEpH
'' SIG '' ZhEnKWaol+TTBoFKovmEpxFHFAmCn4TtVXj+AZodUAiF
'' SIG '' ABAwRu233iNGu8QtVJ+vHnhBMXfMm987g5OhYQK1HQ2x
'' SIG '' /PebsgHOIktU//kFw8IgCwIDAQABo4IBqzCCAacwDwYD
'' SIG '' VR0TAQH/BAUwAwEB/zAdBgNVHQ4EFgQUIzT42VJGcArt
'' SIG '' QPt2+7MrsMM1sw8wCwYDVR0PBAQDAgGGMBAGCSsGAQQB
'' SIG '' gjcVAQQDAgEAMIGYBgNVHSMEgZAwgY2AFA6sgmBAVieX
'' SIG '' 5SUT/CrhClOVWeSkoWOkYTBfMRMwEQYKCZImiZPyLGQB
'' SIG '' GRYDY29tMRkwFwYKCZImiZPyLGQBGRYJbWljcm9zb2Z0
'' SIG '' MS0wKwYDVQQDEyRNaWNyb3NvZnQgUm9vdCBDZXJ0aWZp
'' SIG '' Y2F0ZSBBdXRob3JpdHmCEHmtFqFKoKWtTHNY9AcTLmUw
'' SIG '' UAYDVR0fBEkwRzBFoEOgQYY/aHR0cDovL2NybC5taWNy
'' SIG '' b3NvZnQuY29tL3BraS9jcmwvcHJvZHVjdHMvbWljcm9z
'' SIG '' b2Z0cm9vdGNlcnQuY3JsMFQGCCsGAQUFBwEBBEgwRjBE
'' SIG '' BggrBgEFBQcwAoY4aHR0cDovL3d3dy5taWNyb3NvZnQu
'' SIG '' Y29tL3BraS9jZXJ0cy9NaWNyb3NvZnRSb290Q2VydC5j
'' SIG '' cnQwEwYDVR0lBAwwCgYIKwYBBQUHAwgwDQYJKoZIhvcN
'' SIG '' AQEFBQADggIBABCXisNcA0Q23em0rXfbznlRTQGxLnRx
'' SIG '' W20ME6vOvnuPuC7UEqKMbWK4VwLLTiATUJndekDiV7uv
'' SIG '' WJoc4R0Bhqy7ePKL0Ow7Ae7ivo8KBciNSOLwUxXdT6uS
'' SIG '' 5OeNatWAweaU8gYvhQPpkSokInD79vzkeJkuDfcH4nC8
'' SIG '' GE6djmsKcpW4oTmcZy3FUQ7qYlw/FpiLID/iBxoy+cwx
'' SIG '' SnYxPStyC8jqcD3/hQoT38IKYY7w17gX606Lf8U1K16j
'' SIG '' v+u8fQtCe9RTciHuMMq7eGVcWwEXChQO0toUmPU8uWZY
'' SIG '' sy0v5/mFhsxRVuidcJRsrDlM1PZ5v6oYemIp76KbKTQG
'' SIG '' dxpiyT0ebR+C8AvHLLvPQ7Pl+ex9teOkqHQ1uE7FcSMS
'' SIG '' JnYLPFKMcVpGQxS8s7OwTWfIn0L/gHkhgJ4VMGboQhJe
'' SIG '' GsieIiHQQ+kr6bv0SMws1NgygEwmKkgkX1rqVu+m3pmd
'' SIG '' yjpvvYEndAYR7nYhv5uCwSdUtrFqPYmhdmG0bqETpr+q
'' SIG '' R/ASb/2KMmyy/t9RyIwjyWa9nR2HEmQCPS2vWY+45CHl
'' SIG '' tbDKY7R4VAXUQS5QrJSwpXirs6CWdRrZkocTdSIvMqgI
'' SIG '' bqBbjCW/oO+EyiHW6x5PyZruSeD3AWVviQt9yGnI5m7q
'' SIG '' p5fOMSn/DsVbXNhNG6HY+i+ePy5VFmvJE6P9MIIGgTCC
'' SIG '' BGmgAwIBAgIKYRUIJwAAAAAADDANBgkqhkiG9w0BAQUF
'' SIG '' ADBfMRMwEQYKCZImiZPyLGQBGRYDY29tMRkwFwYKCZIm
'' SIG '' iZPyLGQBGRYJbWljcm9zb2Z0MS0wKwYDVQQDEyRNaWNy
'' SIG '' b3NvZnQgUm9vdCBDZXJ0aWZpY2F0ZSBBdXRob3JpdHkw
'' SIG '' HhcNMDYwMTI1MjMyMjMyWhcNMTcwMTI1MjMzMjMyWjB5
'' SIG '' MQswCQYDVQQGEwJVUzETMBEGA1UECBMKV2FzaGluZ3Rv
'' SIG '' bjEQMA4GA1UEBxMHUmVkbW9uZDEeMBwGA1UEChMVTWlj
'' SIG '' cm9zb2Z0IENvcnBvcmF0aW9uMSMwIQYDVQQDExpNaWNy
'' SIG '' b3NvZnQgQ29kZSBTaWduaW5nIFBDQTCCASIwDQYJKoZI
'' SIG '' hvcNAQEBBQADggEPADCCAQoCggEBAJ+N34U3jLEGVmY2
'' SIG '' TtQK6zYjkR6vq41Y6cTPwZ86CiTExMResO4VFMX6Ppkz
'' SIG '' axlpcvR8BV/lmSVE1zUZipDcKI6QDO/VkGGt3twYcC2D
'' SIG '' yW+vhSz43bm5MP1xeByD/cipuYJnDXq4wQnCvBaXVz8c
'' SIG '' mNNp1zCQkJCNMB/YJtSee4jZ4gntl0lF6wRSgXV7uLUi
'' SIG '' uC5Wu9fPqU48CoOiqDrFrbYBDmrPIrQjH2zVJ+e3fsG0
'' SIG '' 1TKteSUcQobinYX0V9obWS8tJpIpxbPXhaOeYFqU5nuc
'' SIG '' e16CNEtt/9TuJ9Ci3NGQDM59DhV8iKw0x8BsmIRKyUR5
'' SIG '' aCjfIoMNuIosSGHi/hECAwEAAaOCAiMwggIfMBAGCSsG
'' SIG '' AQQBgjcVAQQDAgEAMB0GA1UdDgQWBBRXRXQcXbD2yEMF
'' SIG '' 4IxULY8yp/5IljALBgNVHQ8EBAMCAcYwDwYDVR0TAQH/
'' SIG '' BAUwAwEB/zCBmAYDVR0jBIGQMIGNgBQOrIJgQFYnl+Ul
'' SIG '' E/wq4QpTlVnkpKFjpGEwXzETMBEGCgmSJomT8ixkARkW
'' SIG '' A2NvbTEZMBcGCgmSJomT8ixkARkWCW1pY3Jvc29mdDEt
'' SIG '' MCsGA1UEAxMkTWljcm9zb2Z0IFJvb3QgQ2VydGlmaWNh
'' SIG '' dGUgQXV0aG9yaXR5ghB5rRahSqClrUxzWPQHEy5lMFAG
'' SIG '' A1UdHwRJMEcwRaBDoEGGP2h0dHA6Ly9jcmwubWljcm9z
'' SIG '' b2Z0LmNvbS9wa2kvY3JsL3Byb2R1Y3RzL21pY3Jvc29m
'' SIG '' dHJvb3RjZXJ0LmNybDBUBggrBgEFBQcBAQRIMEYwRAYI
'' SIG '' KwYBBQUHMAKGOGh0dHA6Ly93d3cubWljcm9zb2Z0LmNv
'' SIG '' bS9wa2kvY2VydHMvTWljcm9zb2Z0Um9vdENlcnQuY3J0
'' SIG '' MHYGA1UdIARvMG0wawYJKwYBBAGCNxUvMF4wXAYIKwYB
'' SIG '' BQUHAgIwUB5OAEMAbwBwAHkAcgBpAGcAaAB0ACAAqQAg
'' SIG '' ADIAMAAwADYAIABNAGkAYwByAG8AcwBvAGYAdAAgAEMA
'' SIG '' bwByAHAAbwByAGEAdABpAG8AbgAuMBMGA1UdJQQMMAoG
'' SIG '' CCsGAQUFBwMDMA0GCSqGSIb3DQEBBQUAA4ICAQAwvLAg
'' SIG '' pGKgp+85JmE93KzGmdCGxC71gzJlXiI+m9aG+Oi2n8qL
'' SIG '' 1jt1C6GRPkzdZHSMSfKIjBhnbP4VZka4OkZCl8iRN9Qk
'' SIG '' sees0+pBFIN308lPBV+jFFK/lqQvPlZbEHXOU8POBVRp
'' SIG '' tGXJJKUP4SW4GrlN5QK5UB5Ps5gMHZUC7iJZrSLLBXQL
'' SIG '' BEV7BFng2A+z60z4YN3CeJ7Rup9r9/PufkQRQNK9uptL
'' SIG '' FghupL5V5KY4EqNI9BxVeoog0X3+kduUjy/Ce2umZIVP
'' SIG '' o+UsNCldC7/1xzgvxCDEVjH2ac6F+AqR7NDWrro4BQzr
'' SIG '' bk9MnAMpqqL8GKApDA1cXFYjV9oclg3IJjbBRMvl4eZv
'' SIG '' ieeP6Zi1c9N44+2jATx05V68bPYhiWcF7JedtbH9r6bp
'' SIG '' cqXDNOEvn/n0ajniLQSCW/zQnK58nRH55rVTGXS6OUo5
'' SIG '' 631Cs0o7Nz3CSnsnmOfiTpsbSlQ4aiM3vmq3SO7qQg1J
'' SIG '' JJGOtwQul2/k50W7j039YNnXWcLYgNZgNHu3oZMg/oG4
'' SIG '' qqVcCemKDb4oTX7X6A/tZXjRMV+5ZtvfQucLzAIHjd//
'' SIG '' IAajRWW0szKNLpHiTbSpyfq8awQOsp/qn96kyQqW9I33
'' SIG '' 2Jio8IUCCFmkIKYsCxryUgbtaeVkGBvgo6veynwUYUO4
'' SIG '' ZfU2o1UTK2csTRswTDGCBIcwggSDAgEBMIGHMHkxCzAJ
'' SIG '' BgNVBAYTAlVTMRMwEQYDVQQIEwpXYXNoaW5ndG9uMRAw
'' SIG '' DgYDVQQHEwdSZWRtb25kMR4wHAYDVQQKExVNaWNyb3Nv
'' SIG '' ZnQgQ29ycG9yYXRpb24xIzAhBgNVBAMTGk1pY3Jvc29m
'' SIG '' dCBDb2RlIFNpZ25pbmcgUENBAgphBfceAAAAAAAyMAkG
'' SIG '' BSsOAwIaBQCggbQwGQYJKoZIhvcNAQkDMQwGCisGAQQB
'' SIG '' gjcCAQQwHAYKKwYBBAGCNwIBCzEOMAwGCisGAQQBgjcC
'' SIG '' ARUwIwYJKoZIhvcNAQkEMRYEFP20UDOTcJWUh2NMtU8m
'' SIG '' 4yVb+yCeMFQGCisGAQQBgjcCAQwxRjBEoCaAJABNAGkA
'' SIG '' YwByAG8AcwBvAGYAdAAgAEQAZQBiAHUAZwBnAGUAcqEa
'' SIG '' gBhodHRwOi8vd3d3Lm1pY3Jvc29mdC5jb20wDQYJKoZI
'' SIG '' hvcNAQEBBQAEggEAmk8DyQWF8faKEQVx2MkJ3D48K+r/
'' SIG '' c78C9b/gRf9N/KGjx1X4kpx0VU2DyIgUo3lznwA0FBLJ
'' SIG '' b6Ut5TvHEZOaKjE7O1BySvS5NNffkjYSkFqo/VmAMzb3
'' SIG '' a7PKbASVatIoWCndzw3VGw2Ks5ZZUYSgDpXQXfavhQO+
'' SIG '' fQ5NX/71/MwHoswCInvMLlmQgWMSmqQtyXhVcziptwNL
'' SIG '' gRNtj2HqXQM/AKt+aPY5oHVPOtZ/teTo4uyy68V5MXLL
'' SIG '' 03dZpel5yDMx23AsmN1960FJfTZrgyPiR8jdWcluQ++M
'' SIG '' n3iWgAnnRQVZNTWFjxuKL2aqep1vwYX9bLyk7dqWVKGi
'' SIG '' Eeor1qGCAh0wggIZBgkqhkiG9w0BCQYxggIKMIICBgIB
'' SIG '' ATCBhTB3MQswCQYDVQQGEwJVUzETMBEGA1UECBMKV2Fz
'' SIG '' aGluZ3RvbjEQMA4GA1UEBxMHUmVkbW9uZDEeMBwGA1UE
'' SIG '' ChMVTWljcm9zb2Z0IENvcnBvcmF0aW9uMSEwHwYDVQQD
'' SIG '' ExhNaWNyb3NvZnQgVGltZS1TdGFtcCBQQ0ECCmEWtSkA
'' SIG '' AAAAABAwBwYFKw4DAhqgXTAYBgkqhkiG9w0BCQMxCwYJ
'' SIG '' KoZIhvcNAQcBMBwGCSqGSIb3DQEJBTEPFw0xMDAyMDEy
'' SIG '' MDI3MjlaMCMGCSqGSIb3DQEJBDEWBBTbkYVorb3ksDZn
'' SIG '' wnMbxCQYvZOsUzANBgkqhkiG9w0BAQUFAASCAQCQgORp
'' SIG '' +7FcauQUYFcdD56Klxs5Q8Ul6ZXl4r00ToIzolCHlKiy
'' SIG '' khKOGHaY8zPkGYUJckBAZOAvSGPAeNx9YtiikuUXDQWl
'' SIG '' HHEHZ1k0f+4FWBLMn/QC/sLbpUQNgOSBJx44yjy9xklp
'' SIG '' GlJsevgzqze2UlMaYep4CphUXOAV+8Uf1aaj4J0fWyS5
'' SIG '' 4Csq9dS3m/DuT/2tQLfxt1m6DUt3ElhEeV9qcgH7H+ZT
'' SIG '' LMtlpnz/krTAhnmPW9i7iCecj9Ym+lhE8ISePHxRPjki
'' SIG '' Na/mOnyn+vHjDk/m0t662OTiqx1yvvlBlBnGkSbwdjKR
'' SIG '' CEevtqC+RnRz32Ur3ta8sctxYVPd
'' SIG '' End signature block
