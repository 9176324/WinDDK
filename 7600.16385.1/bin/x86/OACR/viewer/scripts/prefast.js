/////////////////////////////////////////////////////////////////////////////
// Copyright © Microsoft Corporation. All rights reserved.
// Microsoft ® PREfast desktop correctness checker.
//


/////////////////////////////////////////////////////////////////////////////
// Global Variables

// Script error context
var g_arrContext;            // A stack of what the script is currently doing

// Global helper objects
var g_objShell;              // The Windows Script Host WshShell object
var g_objProcEnv;            // The process environment variables collection
var g_objFSO;                // The File System object
var g_objPREfast;

// Registry key paths
var g_strRegPathSAVE;

// Input strings
var g_strSavedSwitches;      // The user's saved switches from the registry
var g_strCmdLine;            // The full command line

// Script verbosity
var g_nVerbosity = 1;        // Indicates which messages to output

// Script environment
var g_strPREfastCL;          // Full pathname of prefastcl shim
var g_strPREfastUtil;        // Full pathname of configuration cmdline interface
var g_strPREfastHELPFILE;    // Full pathname of simple user help file
var g_strPREfastCHMFILE;     // ... of private drop help user interface
var g_strWdkCHMFILE;         // ... official help user interface
var g_strPREfastXSLFlatten;  // Full pathnames of the stylesheets for FLATTEN
var g_strPREfastXSLFlattenSeq;
var g_strRunTmpDir;          // The temporary directory created for the run 
                             // under which plugins can persist information
							 // across the run.  Set into environment variable
							 // "PREfastRunTemporaryDirectory".  Cleaned up
							 // upon exit.
var g_strPREfastArch;


// Switches
var g_dictValidSwitches;     // The values in effect of all switches, no matter where the value originated
var g_dictSpecdSwitches;     // The switches specified on the command line
var g_dictSavedSwitches;     // The switches saved to the registry
var g_dictValueSwitches;     // The switches that take a value
var g_dictMutexSwitches;     // The mutually exclusive switches

// Filters
var g_objPresets = null;             // The set of all presets
var g_objFilter = null;             // The selected filter preset;

/////////////////////////////////////////////////////////////////////////////
//
var nExitCode;
try
{
    // Initialize the stack of script context
    g_arrContext = new Array();

    // Call the script's main function
    nExitCode = main(WScript.Arguments.Count(), WScript.Arguments);
}
catch (e)
{
    // Report the exception
    var strError;
    if ("string" == typeof(e))
    {
        strError = e;
    }
    else
    {
        var strErrNum = "0000000" + (e.number + 0x100000000).toString(16);
        strErrNum = "0x" + strErrNum.substring(strErrNum.length - 8);
        strError = "Error " + strErrNum + " occurred while " + ContextTop() +
                   ":\n\t" + e.description;
    }
    var Interactive = WScript.FullName.toLowerCase().indexOf("wscript") != -1;
    // If we happen (wrongly) to be interactive, the bars are not helpful.
    Echo(0, strError, !Interactive, !Interactive);

    // Set the exit code of the script
    nExitCode = e.number;
}

// Flush the console output
Echo(0, "", false, false);

// Exit the script
WScript.Quit(nExitCode);


/////////////////////////////////////////////////////////////////////////////
//
function main(argc, args)
{
    // Check Scripting Version
    CheckJScriptVersion( 5, 5 );  // Must be at least version 5.5

    // Create global helper objects
    ContextSet("creating script helper objects");
    g_objFSO     = GuardedCreateObject("Scripting.FileSystemObject");
    g_objShell   = GuardedCreateObject("WScript.Shell");
    g_objProcEnv = g_objShell.Environment("Process");

    g_objPREfast = GuardedCreateObject("PREfast.ExecEnvironment");

    // Some ancient paths included prefast.js where it could be executed.
    // PREFAST_ARCH should only be set by prefast.exe, and if it isn't set,
    // this is prefast.js running directly.
    g_strPREfastArch = g_objProcEnv("PREFAST_ARCH");
    if (g_strPREfastArch == undefined
        || g_strPREfastArch == "")
    {
	throw("prefast.exe not found in PATH. PATH must include <prefast>/bin/bin/<arch>");
    }
    // Initialize the environment variables
    InitEnvironment();

    var arrArgs = new Array;

    // Copy the arguments from the slightly odd collection that they're in into an
    // array that we can manipulate more easily
    for ( var i = 0; i < argc; i++ )
        arrArgs[i] = args(i);

    // Parse the command line
    var retval = ParseCommandLine(arrArgs);

    // Remove the contents of the run-temporary directory
    ContextSet("Deleting the temporary directory created for the run");
    // This occasionally fails thanks to anti-virus holding the file
	// too long, so be more forgiving.  (Only for huge runs.)
	var tries = 0;
    while (tries < 10)
    {
		try
		{
			g_objFSO.DeleteFolder(g_strRunTmpDir);
		    break;
		}
		catch (e)
        {
			tries++;
			if (tries >= 10)
			{
				throw e;
			}
			WScript.Sleep(1000);
			continue;
        }
    }

	return retval;
}

/////////////////////////////////////////////////////////////////////////////
//
function InitEnvironment()
{
    ContextSet("initializing environment variables");

    // Other registry paths
    g_strRegPathSAVE = g_objPREfast.RegistryKey + "SavedSwitches\\";

    // The locations of specific PREfast scripts
    g_strPREfastCL            = g_objPREfast.BinDirectory + "\\interceptcl\\" 
									+ g_objPREfast.ArchDirName + "\\cl.exe";
    g_strPREfastUtil          = '"' + g_objPREfast.BinArchDirectory + '\\pftutil.exe"';
    g_strPREfastXSLFlatten    = g_objPREfast.ScriptsDirectory + "\\flatten.xsl";
    g_strPREfastXSLFlattenSeq = g_objPREfast.ScriptsDirectory + "\\flattenSeq.xsl";

    // The locations of specific PREfast documents
    g_strPREfastHELPFILE = g_objPREfast.DocumentationDirectory + "\\CmdLine.html";
    g_strPREfastCHMFILE = g_objPREfast.DocumentationDirectory + "\\PFD.chm";

    var WdkBase = g_objProcEnv("BASEDIR");
	g_strWdkCHMFILE = "";
    if (WdkBase != "")
	{
	    var strWdkCHMFILE = WdkBase + "\\help" + "\\PFD.chm";
		if (g_objFSO.FileExists(strWdkCHMFILE))
		{
			g_strWdkCHMFILE = strWdkCHMFILE;
		}
	}
	if (g_strWdkCHMFILE == "")
	{
		strPREfastCHMFILE = g_objPREfast.DocumentationDirectory + "\\PFD.chm";
		if (g_objFSO.FileExists(strPREfastCHMFILE))
		{
			g_strWdkCHMFILE = strPREfastCHMFILE;
		}
	}

    // Set the default MAXPATHS, if not specified by an environment variable
    if (!g_objProcEnv("_DFA_MAX_PATHS_").length)
		g_objProcEnv("_DFA_MAX_PATHS_") = -1; // 256 set later

    // Set the default MAXTIME, if not specified by an environment variable
    if (!g_objProcEnv("_PREFAST_MAX_TIME_").length)
		g_objProcEnv("_PREFAST_MAX_TIME_") = -1; // 3000 set later

    // Set the default INCREASEHEAP, if not specified by an environment variable
    if (!g_objProcEnv("PREFAST_INCREASE_HEAP").length)
        g_objProcEnv("PREFAST_INCREASE_HEAP") = 0;

    // Set the default STACKHOGTHRESHOLD, if not specified by an environment variable
    if (!g_objProcEnv("_PRECEDENCE_STACK_THRESHOLD_").length)
		g_objProcEnv("_PRECEDENCE_STACK_THRESHOLD_") = -1; //16384 set later

    // Set the default EnableCritSState, if not specified by an environment variable
    if (!g_objProcEnv("_PRECEDENCE_FLAG_INITCRITS_").length)
        g_objProcEnv("_PRECEDENCE_FLAG_INITCRITS_") = 0;

    // Set the default NewFailure, if not specified by an environment variable
    if (!g_objProcEnv("_DFA_NEW_FAILURE_").length)
        g_objProcEnv("_DFA_NEW_FAILURE_") = "throw";

    // Set the default KernelNewFailure, if not specified by an environment variable
    if (!g_objProcEnv("_DFA_KERNEL_NEW_FAILURE_").length)
        g_objProcEnv("_DFA_KERNEL_NEW_FAILURE_") = "null";

    // Set the default regular cl, if not specified by an environment variable
    if (!g_objProcEnv("PREFAST_USER_CL_EXE").length)
        g_objProcEnv("PREFAST_USER_CL_EXE") = "default";

    // Set the default exclude list, if not specified by an environment variable
    if (!g_objProcEnv("PREFAST_EXCLUDE_LIST").length)
        g_objProcEnv("PREFAST_EXCLUDE_LIST") = "none;";

    // Set the default macro setting, if not specified by an environment variable
    // We need "src" so the XML files can access the macros.
    if (!g_objProcEnv("PREFAST_MACRO").length)
        g_objProcEnv("PREFAST_MACRO") = "src";

    // Set the default persist setting, if not specified by an environment variable
    if (!g_objProcEnv("PREFAST_PERSIST").length)
        g_objProcEnv("PREFAST_PERSIST") = "0";

    // Set the cyclomatic complexity reporting level to "infinity"
    if (!g_objProcEnv("_PREFAST_CYCLOMATIC").length)
        g_objProcEnv("_PREFAST_CYCLOMATIC") = "2147483647";

    // Set the default amd64 setting, if not specified by an environment variable
    if (!g_objProcEnv("PREFAST_AMD64").length)
        g_objProcEnv("PREFAST_AMD64") = "0";

    var strTempPath = g_objFSO.GetSpecialFolder(2).Path;
    if (strTempPath.charAt(strTempPath.length - 1) != "\\")
        strTempPath += "\\";
    var gotDir = false;
	var loopMax = 100;
	while (!gotDir)
	{
		g_strRunTmpDir = strTempPath + g_objFSO.GetTempName();
		try {
			g_objFSO.CreateFolder(g_strRunTmpDir);
			gotDir = 1;
		}
		catch (e)
		{
			loopMax--;
			if (loopMax < 0) throw e + " after 100 tries";
		}
	}
    g_objProcEnv("PREfastRunTempDirectory") = g_strRunTmpDir;
}

    
/////////////////////////////////////////////////////////////////////////////
//
function ParseCommandLine(arrArgs)
{
    var result;

    // Parse the command line
    ContextSet("parsing the command line");

    InitSwitches();

    // Check for the UNSAVE command before parsing any switches.
    // Otherwise, it is impossible to recover from bad saved switches
    if ( arrArgs[0] && "UNSAVE" == arrArgs[0].toUpperCase() )
    {
        DisplayLogo();
        return DoUnSave();
    }

    // Parse any Switches that may have been passed
    result = ParseSwitches( arrArgs );
    if (result)
        return result;

    // Display the logo; it will be suppressed if VERBOSE has been set to 0
    DisplayLogo();

    // The switches have been shifted off, so the command is next.
    var strCommand = arrArgs.shift();
    if ( undefined == strCommand )
        return Syntax();

    // Process every other prefast command
    switch ( strCommand.toUpperCase() )
    {
    case "HELP":
        return DoHelp();
        break;

    case "CONFIG":
        return DoConfig();
        break;

    case "UNSAVE":
        return DoUnSave();
        break;

    case "VERSION":
        return DoVersion();
        break;

    case "CREATENEWDEFECT":
        return DoCreateNew();
        break;

    case "RESET":
        var strLog = arrArgs.length > 0 ? arrArgs[0] : g_dictValidSwitches("LOG");
        return DoReset( strLog );
        break;

    case "LIST":
        var strLog = arrArgs.length > 0 ? arrArgs[0] : g_dictValidSwitches("LOG");
        return DoList( strLog );
        break;

    case "VIEW":
        var strLog = arrArgs.length > 0 ? arrArgs[0] : g_dictValidSwitches("LOG");
        return DoView( strLog );
        break;


    case "COUNT":
        var strLog = arrArgs.length > 0 ? arrArgs[0] : g_dictValidSwitches("LOG");
        return DoCount( strLog, false );
        break;

    case "COUNTF":
        var strLog = arrArgs.length > 0 ? arrArgs[0] : g_dictValidSwitches("LOG");
        return DoCount( strLog, true );
        break;

    case "FLATTEN":
        var strFile1 = (arrArgs.length > 0) ? arrArgs[0] : "";
        var strFile2 = (arrArgs.length > 1) ? arrArgs[1] : "";
        return DoFlatten(strFile1, strFile2);
        break;

    case "UNFLATTEN":
        var strFile1 = (arrArgs.length > 0) ? arrArgs[0] : "";
        var strFile2 = (arrArgs.length > 1) ? arrArgs[1] : "";
        return DoUnflatten(strFile1, strFile2);
        break;

    case "REMOVEDUPS":
        return DoRemoveDups(arrArgs);
        break;

    case "FILTER":
        var strFile1 = (arrArgs.length > 0) ? arrArgs[0] : "";
        var strFile2 = (arrArgs.length > 1) ? arrArgs[1] : "";
        return DoFilter( strFile1, strFile2 );
        break;

    case "MODMAN":
        return DoModMan( ConcatenateArgs( arrArgs ) );
        break;

    case "WRITEEXITFUNCTIONS":
        var strFile1 = (arrArgs.length > 0) ? arrArgs[0] : "";
        var strFile2 = (arrArgs.length > 1) ? arrArgs[1] : "";
        return DoWriteExitFunctions( strFile1, strFile2 );
        break;

    case "READEXITFUNCTIONS":
        var strFile1 = (arrArgs.length > 0) ? arrArgs[0] : "";
        var strFile2 = (arrArgs.length > 1) ? arrArgs[1] : "";
        return DoReadExitFunctions( strFile1, strFile2 );
        break;

    case "INSTALL":
        return DoInstall( arrArgs );

    case "SAVE":
        // Do not save the /N, /LOG, or switches
        g_dictValidSwitches.Remove("N");
        g_dictValidSwitches.Remove("V");
        g_dictValidSwitches.Remove("LOG");
        return DoSave();
        break;

    case "EXEC":
        // We don't want the word 'EXEC', we want the build command
        strCommand = arrArgs.shift();
        // FALL THROUGH INTENTIONALLY
    default:
        return DoExec( strCommand, ConcatenateArgs( arrArgs, "\n" ), g_dictValidSwitches);
        break;
    }

    // We shouldn't ever get to this point, but if we do...
    return Syntax();
}


/////////////////////////////////////////////////////////////////////////////
//
function ConcatenateArgs( arrArgs, delim )
{
    ContextPush("generating the full command-line string");
    var strCmdLine = "";
    if ( !delim )
        delim = " ";
    for (var i = 0; i < arrArgs.length; ++i)
        strCmdLine += ((i > 0) ? delim : "") + arrArgs[i];
    ContextPop();
    return strCmdLine;
}

/////////////////////////////////////////////////////////////////////////////
//
function InitSwitches()
{
    // Define the valid switches and their default values
    g_dictValidSwitches = GuardedCreateObject("Scripting.Dictionary");
    g_dictValidSwitches.CompareMode = 1;

	// No action:
    g_dictValidSwitches("N"         ) = false;

	// Pure VS parser only (for testing):
    g_dictValidSwitches("V"         ) = false;

    g_dictValidSwitches("LOG"       ) = g_objProcEnv("PREFASTLOG");
    g_dictValidSwitches("CONFIGFILE") = g_objProcEnv("PREFASTCONFIG");
    g_dictValidSwitches("MAXPATHS"  ) = g_objProcEnv("_DFA_MAX_PATHS_");
    g_dictValidSwitches("MAXTIME"   ) = g_objProcEnv("_PREFAST_MAX_TIME_");
    g_dictValidSwitches("INCREASEHEAP") = g_objProcEnv("PREFAST_INCREASE_HEAP");
    g_dictValidSwitches("STACKHOGTHRESHOLD") = g_objProcEnv("_PRECEDENCE_STACK_THRESHOLD_");
    g_dictValidSwitches("ENABLECRITSWARNING") = g_objProcEnv("_PRECEDENCE_FLAG_INITCRITS_");
    g_dictValidSwitches("NEW_FAILURE") = g_objProcEnv("_DFA_NEW_FAILURE_");
    g_dictValidSwitches("KERNEL_NEW_FAILURE") = g_objProcEnv("_DFA_KERNEL_NEW_FAILURE_");
    g_dictValidSwitches("CL_EXE"   ) = g_objProcEnv("PREFAST_USER_CL_EXE");
    g_dictValidSwitches("EXCLUDE"  ) = g_objProcEnv("PREFAST_EXCLUDE_LIST");
    g_dictValidSwitches("FILTERSFILE") = g_objProcEnv("PREFAST_FILTERS");
    g_dictValidSwitches("FILTERPRESET") = g_objProcEnv("PREFAST_FILTER_PRESET");
    g_dictValidSwitches("MODELFILES") = g_objProcEnv("PREFASTMODEL");
    g_dictValidSwitches("COVERAGEFILE") = g_objProcEnv("PREFASTCOVERAGEFILE");
    g_dictValidSwitches("MACRO"     ) = g_objProcEnv("PREFAST_MACRO");
    g_dictValidSwitches("PERSIST"   ) = false;
    g_dictValidSwitches("AMD64"     ) = false;
    g_dictValidSwitches("VERBOSE"   ) = 1;
    g_dictValidSwitches("RESET"     ) = true;
    g_dictValidSwitches("NORESET"   ) = false;
    g_dictValidSwitches("LIST"      ) = false;
    g_dictValidSwitches("NOLIST"    ) = true;
    g_dictValidSwitches("VIEW"      ) = false;
    g_dictValidSwitches("NOVIEW"    ) = true;
    g_dictValidSwitches("ONEPASS"   ) = false;
    g_dictValidSwitches("TWOPASS"   ) = true;
    g_dictValidSwitches("NOREMOVEDUPS") = false;
    g_dictValidSwitches("REMOVEDUPS") = true;
    g_dictValidSwitches("HELP"      ) = false;
    g_dictValidSwitches("?"         ) = false;
    g_dictValidSwitches("NOFILTER"  ) = true;
    g_dictValidSwitches("FILTER"    ) = false;
    g_dictValidSwitches("WSPMIN"    ) = false;
    g_dictValidSwitches("IGNOREMANAGEDOPTIONS") = false;
    g_dictValidSwitches("SKIPMANAGEDFILES") = true;
    g_dictValidSwitches("DRIVERS"   ) = true;
    g_dictValidSwitches("USER") = false;

    // Define the switches that must take values
    g_dictValueSwitches = GuardedCreateObject("Scripting.Dictionary");
    g_dictValueSwitches.CompareMode = 1;
    g_dictValueSwitches("LOG"     ) = ValidateNotEmpty;
    g_dictValueSwitches("CONFIGFILE"  ) = ValidateConfigFile;
    g_dictValueSwitches("MAXPATHS") = ValidateNotEmpty;
    g_dictValueSwitches("MAXTIME" ) = ValidateNotEmpty;
    g_dictValueSwitches("INCREASEHEAP" ) = ValidateNotEmpty;
    g_dictValueSwitches("MACRO" ) = ValidateMacro;
    g_dictValueSwitches("VERBOSE" ) = ValidateVerbose;
    g_dictValueSwitches("STACKHOGTHRESHOLD") = ValidateNotEmpty;
    g_dictValueSwitches("ENABLECRITSWARNING") = ValidateNotEmpty;
    g_dictValueSwitches("NEW_FAILURE") = ValidateNewFailure;
    g_dictValueSwitches("KERNEL_NEW_FAILURE") = ValidateNewFailure;
    g_dictValueSwitches("CL_EXE") = ValidateNotEmpty;
    g_dictValueSwitches("EXCLUDE") = ValidateNotEmpty;
    g_dictValueSwitches("FILTERPRESET") = ValidateFilterPreset;
    g_dictValueSwitches("FILTERSFILE") = ValidateFiltersFile;
    g_dictValueSwitches("MODELFILES") = ValidateModelFiles;
    g_dictValueSwitches("COVERAGEFILE") = ValidateCoverageFile;
    // Doesn't actually take a value, but needs a validation function
    g_dictValueSwitches("WSPMIN") = ValidateWspmin;

    // Define the switches that are mutually exclusive
    g_dictMutexSwitches = GuardedCreateObject("Scripting.Dictionary");
    g_dictMutexSwitches.CompareMode = 1;
    g_dictMutexSwitches("RESET"     ) = "NORESET";
    g_dictMutexSwitches("NORESET"   ) = "RESET";
    g_dictMutexSwitches("LIST"      ) = "NOLIST";
    g_dictMutexSwitches("NOLIST"    ) = "LIST";
    g_dictMutexSwitches("VIEW"      ) = "NOVIEW";
    g_dictMutexSwitches("NOVIEW"    ) = "VIEW";
    g_dictMutexSwitches("ONEPASS"   ) = "TWOPASS";
    g_dictMutexSwitches("TWOPASS"   ) = "ONEPASS";
    g_dictMutexSwitches("REMOVEDUPS" ) = "NOREMOVEDUPS";
    g_dictMutexSwitches("NOREMOVEDUPS") = "REMOVEDUPS";
    g_dictMutexSwitches("FILTER"    ) = "NOFILTER";
    g_dictMutexSwitches("NOFILTER"  ) = "FILTER";
    g_dictMutexSwitches("IGNOREMANAGEDOPTIONS") = "SKIPMANAGEDFILES";
    g_dictMutexSwitches("SKIPMANAGEDFILES") = "IGNOREMANAGEDOPTIONS";
    g_dictMutexSwitches("DRIVERS") = "USER";
    g_dictMutexSwitches("USER") = "DRIVERS";
}

/////////////////////////////////////////////////////////////////////////////
//
function ParseSwitches(arrArgs)
{
    ContextPush("parsing switches");

    // Create the collection of saved switches
    g_dictSavedSwitches = GuardedCreateObject("Scripting.Dictionary");
    g_dictSavedSwitches.CompareMode = 1; // Text

    // Create the collection of specified switches
    g_dictSpecdSwitches = GuardedCreateObject("Scripting.Dictionary");
    g_dictSpecdSwitches.CompareMode = 1; // Text

    ContextPush("parsing the saved switches");

    var strSwitch;
    var arrSwitches = new VBArray(g_dictValidSwitches.Keys()).toArray();
    for ( var i in arrSwitches )
    {
        strSwitch = arrSwitches[i];
        try
        {
            strValue = g_objShell.RegRead(g_strRegPathSAVE + strSwitch + "\\");
        }
        catch (e)
        {
            // Ignore the switch if it can't be read
            continue;
        }
        nResult = ParseSwitch( strSwitch, strValue, g_dictSavedSwitches, false );
        if ( nResult )
		{
            throw "One or more saved switches are invalid.\n\n" +
            "Enter PREFAST UNSAVE to resolve the problem.";
		}
    }
    ContextPop();

    ContextPush("parsing the specified switches");
    var strArg;
    while ( (strSwitch = arrArgs.shift()) != undefined )
    {
        var strFlag = strSwitch.charAt(0);
        if ('-' == strFlag || '/' == strFlag)
        {
            var arrArg = strSwitch.substring(1).split('=', 2);
            nResult = ParseSwitch( arrArg[0], arrArg[1], g_dictSpecdSwitches, true );
            if ( nResult )
                return nResult;
        }
        else
        {
            arrArgs.unshift( strSwitch );
            break;
        }
    }
    ContextPop();

    if (g_dictValidSwitches("USER"))
	{
		// The default is DRIVERS, but if /USER is present it overrides that.
	    g_dictValidSwitches("DRIVERS") = false;
	}

	// /DRIVERS sets some special defaults, so set those, unless
	// it was explicitly overridden.
    // Set the _DFA_MAX_PATHS_ environment variable

	if (g_dictValidSwitches("MAXPATHS") == -1)
	{
		if (g_dictValidSwitches("DRIVERS"))
		{
			g_dictValidSwitches("MAXPATHS") = 256; // subject to change
		}
		else
		{
			g_dictValidSwitches("MAXPATHS") = 256;
		}
	}

    // Set the _PREFAST_MAX_TIME_ environment variable
	if (g_dictValidSwitches("MAXTIME") == -1)
	{
		if (g_dictValidSwitches("DRIVERS"))
		{
			g_dictValidSwitches("MAXTIME") = 5000;
		}
		else
		{
			g_dictValidSwitches("MAXTIME") = 3000;
		}
	}

    // Set the _PRECEDENCE_STACK_THRESHOLD_ environment variable
	if (g_dictValidSwitches("STACKHOGTHRESHOLD") == -1) 
	{
		if (g_dictValidSwitches("DRIVERS"))
		{
			g_dictValidSwitches("STACKHOGTHRESHOLD") = 1024;
		}
		else
		{
			g_dictValidSwitches("STACKHOGTHRESHOLD") = 16384;
		}
	}

	// Insert DriversModel, if needed.
	ReValidateModelFiles();

    ContextPop();
    return 0;
}


/////////////////////////////////////////////////////////////////////////////
// Description: Parses an individual switch.
//
// Parameters:
//     strSwitch - The name of a switch, without the - or /
//     strValue  - The value of the switch
//     fReportViolations - When false, the function will not report syntax
// violations, but will not ignore them either. Instead, it will silently
// return a non-zero error code.
//      dictSpecdSwitches - The function saves all valid switch names
// into the provided dictionary
//
//     g_dictValidSwitches - A Dictionary object containing the valid switch
// string mapped to its current value. In this dictionary, the switch Strings
// must not be prefixed with a / or -. The CompareMode of the dictionary must
// have been previously set to 1 (Text). The switch and value, if valid,
// will be added to this dictionary.
//
// Return Value: If the parsing succeeds as specified, zero will be returned
// Otherwise, non-zero wil be returned.
//
function ParseSwitch(strSwitch, strValue, dictSpecdSwitches, fReportViolations)
{
    // Validate the specified Dictionary objects
    if (1 != g_dictValidSwitches.CompareMode)
        throw "The CompareMode of the dictValidSwitches argument to function\n" +
        "ParseSwitchArray must have been set to 1 (Text).";
    if (1 != g_dictSpecdSwitches.CompareMode)
        throw "The CompareMode of the dictSpecdSwitches argument to function\n" +
        "ParseSwitchArray must have been set to 1 (Text).";

    // Validate that this is a valid switch
    if (!g_dictValidSwitches.Exists(strSwitch))
        return SyntaxViolated("Invalid switch specified: " + strSwitch, !fReportViolations);

    // Process differently based on if the switch takes a value or not
    if (g_dictValueSwitches.Exists(strSwitch))
    {
        var fnValidate = g_dictValueSwitches(strSwitch);
        if (fnValidate)
        {
            var result = fnValidate(strSwitch, strValue, fReportViolations);
            if (result) {
                return result;
		    }
        }

        dictSpecdSwitches(strSwitch) = strSwitch;
        g_dictValidSwitches(strSwitch) = strValue;
    }
    else
    {
        // Validate that a value was not specified with this switch
        if (strValue && "" != strValue)
            return SyntaxViolated("Switch does not take a value: " + strSwitch, !fReportViolations);

        // Validate that a conflicting switch has not already been specified
        if (g_dictMutexSwitches.Exists(strSwitch))
        {
            var strConflict = g_dictMutexSwitches(strSwitch);
            if (dictSpecdSwitches.Exists(strConflict))
                return SyntaxViolated("Conflicting switches specified: /" + strSwitch + " and /" + dictSpecdSwitches(strConflict), !fReportViolations);
            g_dictValidSwitches(strConflict) = false;
        }

        dictSpecdSwitches(strSwitch) = strSwitch;
        g_dictValidSwitches(strSwitch) = true;
    }

    // Indicate success
    return 0;
}


/////////////////////////////////////////////////////////////////////////////
// Validates switches that must specify a value.
//
function ValidateNotEmpty(strSwitch, strValue, fReportViolations)
{
    // A value must be specified with this switch
    if (!strValue || "" == strValue)
        return SyntaxViolated("Switch was specified without a value: " + strSwitch, !fReportViolations);
    return 0;
}


/////////////////////////////////////////////////////////////////////////////
// Validates the /VERBOSE= switch
//
function ValidateVerbose(strSwitch, strValue, fReportViolations)
{
    // A value must be specified with this switch
    var result = ValidateNotEmpty(strSwitch, strValue, fReportViolations)
                 if (result)
                     return result;

                 // Only certain values are allowed
                 var nValue = new Number(strValue);
    switch (nValue.toString())
    {
    case '0':
    case '1':
    case '2':
    case '3':
        g_nVerbosity = nValue;
        return 0;

    default:
        return SyntaxViolated("Invalid value '" + strValue + "' specified for switch '" + strSwitch + "'.", !fReportViolations);
    }
}

/////////////////////////////////////////////////////////////////////////////
// Validates the /FILTERPRESET= switch
//
// Ensures that the value is an existing filter preset name in the registry
// or one of the default set.
function ValidateFilterPreset( strSwitch, strValue, fReportViolations )
{
    LoadPresets();
	try
	{
		// if strValue is null or really trashed, this might throw
        g_objFilter = g_objPresets.item(strValue);
    }
	catch (e) {
		g_objFilter = null;
	};

    if (g_objFilter == null )
	{
        return SyntaxViolated("Switch " + strSwitch + " must specify an existing filter preset name.\n"
                              + "Filter preset names are case sensitive.", !fReportViolations );
    }

    g_objProcEnv("PREFAST_FILTER_PRESET") = strValue;

    return 0;
}

/////////////////////////////////////////////////////////////////////////////
// Validates the /FILTERSFILE= switch
//
// Currently, sets the PREFAST_FILTERS env var without checking if the file
// exists or not.
function ValidateFiltersFile( strSwitch, strValue, fReportViolations )
{
    g_objProcEnv("PREFAST_FILTERS") = strValue;

    // If g_objPresets already exists, we need to delete and recreate it so the
    // new setting of PREFAST_FILTERS will take effect
    if ( g_objPresets )
    {
        g_objPresets = null;
        LoadPresets();
        g_objFilter = g_objPresets.selectedPreset;
    }

    return 0;
}

/////////////////////////////////////////////////////////////////////////////
// Validates the /WSPMIN switch
//
// Sets the value of FilterPreset to WSPMIN
//
function ValidateWspmin( strSwitch, strValue, fReportViolations )
{
    return ValidateFilterPreset( strSwitch, "wspmin", fReportViolations );
}

/////////////////////////////////////////////////////////////////////////////
// Validates the /COVERAGEFILE= switch
//
function ValidateCoverageFile( strSwitch, strValue, fReportViolations )
{
    if ( undefined == strValue || 0 == strValue.length )
        return SyntaxViolated("Switch was specified without a value: " + strSwitch, !fReportViolations);

    try
    {
        g_objProcEnv("PREFASTCOVERAGEFILE") = ValidateXMLPath(strValue, "coverage file", 0);
    }
    catch (e)
    {
        var err = "Invalid value '" + strValue + "' specified for " + strSwitch + ":\n";
        if ( "string" == typeof( e ) )
            err += e;
        return SyntaxViolated(err, !fReportViolations);
    }
    return 0;
}


//////////////////////////////////////////////////////////////////////////////
//
function ValidateModelFiles( strSwitch, strModelFiles, fReportViolations )
{
    if ( undefined == strModelFiles )
        strModelFiles = "";

    ContextPush("validating the model files: \"" + strModelFiles + "\"");

    var arrModelFiles = strModelFiles.split(";");
    var strNewModelFiles = "";
    var strModelsDirectory = g_objPREfast.ModelsDirectory;

    for ( var i in arrModelFiles )
    {
        strModelFile = arrModelFiles[i];
        if ( !strModelFile || strModelFile.length == 0 )
            continue;

        strNewModelFiles += g_objFSO.GetAbsolutePathName( strModelFile );
        strNewModelFiles += ";";
    }

    g_objProcEnv("PREFASTMODEL") = strNewModelFiles;

    Echo( 2, "PREFASTMODEL is " + strNewModelFiles );

    ContextPop();
    return 0;
}

//////////////////////////////////////////////////////////////////////////////
//
// Insert DriversModel.xml and the built-in one, as needed.
function ReValidateModelFiles()
{
	var strModelFiles = g_objProcEnv("PREFASTMODEL");

    ContextPush("re-validating the model files: \"" + strModelFiles + "\"");

    var arrModelFiles = strModelFiles.split(";");
    var strNewModelFiles = "";
    var strModelsDirectory = g_objPREfast.ModelsDirectory;
	var priorEntry = "";
	var modelDefault 
		= g_objFSO.BuildPath( g_objPREfast.ModelsDirectory, "DriversModel.xml" );
	var builtInModel = "res://mspft80.dll/300";

    for ( var i in arrModelFiles )
    {
        strModelFile = arrModelFiles[i];
        if ( !strModelFile || strModelFile.length == 0 )
            continue;

		strNewModelFiles += strModelFile;
        strNewModelFiles += ";";
    }
	if (strNewModelFiles.indexOf(builtInModel) == -1)
	{
		strNewModelFiles = builtInModel + ";" + strNewModelFiles;
	}
	if (strNewModelFiles.indexOf(modelDefault) == -1)
	{
		strNewModelFiles += modelDefault;
		modelDefault 
			= g_objFSO.BuildPath(g_objPREfast.ModelsDirectory, "SuppModel.xml");
		strNewModelFiles += ";" + modelDefault;
	}

    g_objProcEnv("PREFASTMODEL") = strNewModelFiles;

    ContextPop();
    return 0;
}

//////////////////////////////////////////////////////////////////////////////
//
function ValidateConfigFile( strSwitch, strConfigFile, fReportViolations )
{
    ContextPush("validating the config file: \"" + strConfigFile + "\"");

    // Just validate that the directory exists.  It is legal to pass in
    // a non-existent file name for PREFAST CONFIG to create, but if the
    // directory doesn't exist we need to signal an error here.

    var strDirectory;

    strDirectory = g_objFSO.GetParentFolderName( strConfigFile );
    if ( !g_objFSO.FolderExists( strDirectory ) )
        return SyntaxViolated( strSwitch + " must specify a file in an existing directory.\n"
                              + "Directory '" + strDirectory + "' does not exist.\n" );


    ContextPop();
    return 0;
}

/////////////////////////////////////////////////////////////////////////////
// Validates the /NEW_FAILURE= or /KERNEL_NEW_FAILURE switch
//
function ValidateNewFailure(strSwitch, strValue, fReportViolations)
{
    // A value must be specified with this switch
    var result = ValidateNotEmpty(strSwitch, strValue, fReportViolations);
    if (result)
        return result;

    // Only certain values are allowed
    switch (strValue.toUpperCase())
    {
    case 'NEVER':
    case 'NULL':
    case 'THROW':
        return 0;

    default:
        return SyntaxViolated("Invalid value '" + strValue + "' specified with switch '" + strSwitch + "'.", !fReportViolations);
    }
}

/////////////////////////////////////////////////////////////////////////////
// Validates the /MACRO= switch
//
function ValidateMacro(strSwitch, strValue, fReportViolations)
{
    // A value must be specified with this switch
    var result = ValidateNotEmpty(strSwitch, strValue, fReportViolations);
    if (result)
        return result;

    // Only certain values are allowed
    switch (strValue.toUpperCase())
    {
    case 'ALL':
    case 'SRC':
        return 0;

    default:
        return SyntaxViolated("Invalid value '" + strValue + "' specified with switch '" + strSwitch + "'.", !fReportViolations);
    }
}

/////////////////////////////////////////////////////////////////////////////
//
function DoHelp()
{
    // Launch the help file
    ContextSet("launching the PREfast help file");
	var helpFile = "";

    if (g_objFSO.FileExists(g_strWdkCHMFILE))
	{
		helpFile = g_strWdkCHMFILE;
	}
    else if (g_objFSO.FileExists(g_strPREfastCHMFILE))
	{
		helpFile = g_strPREfastCHMFILE;
	}
	if (helpFile != "")
	{
		g_objShell.Run("hh.exe "
		     + "-mapid " + g_objPREfast.HelpTopicNumber + " "
		     + helpFile, 1, false);
	}
	else
	{
		Echo(0, "The PFD help files are not installed. " + 
                "Running the WDK installer to install " +
				"the WDK documentation should fix this.")
	}

    // Indicate success
    return 0;
}


/////////////////////////////////////////////////////////////////////////////
//
function DoConfig()
{
    // Set the PREFASTCONFIG environment variable
    ValidateConfigFilePath(g_dictValidSwitches("CONFIGFILE"));

    // Launch the Configuration User Interface
    ContextSet("launching the PREfast Configuration user interface");
    var strFullCmdLine = g_strPREfastUtil + " config";

    g_objShell.Run( strFullCmdLine, 1 );

    // Indicate success
    return 0;
}


/////////////////////////////////////////////////////////////////////////////
//
function DoVersion()
{
    var nVerbosityPrev = g_nVerbosity;
    g_nVerbosity = 2;
    var strInstalled = g_objPREfast.InstalledBuildNumber;
    g_nVerbosity = nVerbosityPrev;
}

/////////////////////////////////////////////////////////////////////////////
//
function DoCreateNew()
{
    g_objShell.Run('"' + g_objPREfast.BinArchDirectory + "\\PREdecl\\PREdecl.exe" + '"');
}

/////////////////////////////////////////////////////////////////////////////
//
function DoReset(strLog, fQuiet)
{
    // Set the PREFASTLOG environment variable
    strLog = ValidateDefectLogPath(strLog);

    // Reset the defect log file
    ContextSet("reseting the PREfast Defect Log file: \"" + strLog + "\"");
    g_objFSO.DeleteFile(strLog);

    // Report what we just did
    Echo(fQuiet ? 2 : 1,
         "The PREfast Defect Log file was reset:\n\t\"" + strLog + "\"",
         true, true);

    // Indicate success
    return 0;
}


/////////////////////////////////////////////////////////////////////////////
//
function DoView(strLog)
{
    // Set the PREFASTLOG environment variable
    strLog = ValidateDefectLogPath(strLog, true);

    // Launch the Defect Log User Interface
    ContextSet("launching the PREfast Defect Log user interface");
    g_objPREfast.LaunchHTA("DefectUI\\DefectUI.hta", false);

    // Indicate success
    return 0;
}


/////////////////////////////////////////////////////////////////////////////
//
function DoList(strLog)
{
    // Launch the Defect Log text formatter
    ContextSet("transforming the PREfast Defect Log to text");
    Echo(1, "Contents of defect log: " + strLog, true, true);

    // Flush the console echo
    Echo(1, "", false, false);

    var objDefectSet = LoadDefects( strLog, true );

    // Loop through all of the defects
    ContextSet("looping through all defects in function DoList");
    var defects = objDefectSet.defectNodesFiltered;
    var cDefects = defects.length;
    for (var i = 0; i < cDefects; ++i)
    {
        // Get the next defect
        var defect = defects(i);

        // Extract the XML data and format the strings
        var strDesc     = defect.selectSingleNode("DESCRIPTION").text;
        var strWarning  = defect.selectSingleNode("DEFECTCODE").text;
        var strFile     = defect.selectSingleNode("SFA/FILEPATH").text +
                          defect.selectSingleNode("SFA/FILENAME").text + " (" +
                          defect.selectSingleNode("SFA/LINE").text + ")";
        var strFunction = defect.selectSingleNode("FUNCTION").text;
        var strFuncLine = defect.selectSingleNode("FUNCLINE").text;

        // Output the information about the defect
        WScript.Echo(strFile + ": warning " + strWarning + ": " + strDesc);
        WScript.Echo("\tFUNCTION: " + strFunction + " (" + strFuncLine + ")");
        var strPath = "";
        var sfas = defect.selectNodes("PATH/SFA");
        var cSfas = sfas.length;
        for (var iPath = 0; iPath < cSfas; ++iPath)
            strPath += sfas(iPath).selectSingleNode("LINE").text + " ";
	if (strPath != "")
            WScript.Echo("\tPATH: " + strPath);
        WScript.Echo("");
    }

    // Display a summary
    var cDefectsListed = objDefectSet.defectNodesFiltered.length;
    var strDefectsListed = ((1 == cDefectsListed) ? " Defect " : " Defects ") + "Listed\n";
    if ("(all defects)" == g_objFilter.name)
    {
        var str = cDefectsListed + strDefectsListed
                  str += "No filter in effect";
        Echo(1, str, true, true);
    }
    else
    {
        var str = objDefectSet.defectNodesFiltered.length + " of ";
        str += cDefectsListed + strDefectsListed;
        str += "Filter in effect: " + g_objFilter.name;
        Echo(1, str, true, true);
    }

    var str = "You can change the filter and sort options from the user interface.\n";
    str += "Enter PREFAST VIEW to display the defect log user interface.";
    Echo(1, str, true);

    // Indicate success
    return 0;
}


/////////////////////////////////////////////////////////////////////////////
//
function DoCount(strLog, fFiltered)
{
    ContextSet("counting the defects in the PREfast Defect Log");
    if (fFiltered)
        Echo(1, "Number of filtered defects in defect log: " + strLog, true, true);
    else
        Echo(1, "Number of unfiltered defects in defect log: " + strLog, true, true);

    var objDefectSet = LoadDefects( strLog, fFiltered );

    // Loop through all of the defects
    ContextSet("counting all defects in function DoCount");
    var defects = fFiltered ? objDefectSet.defectNodesFiltered : objDefectSet.defectNodesMaster;
    var cDefects = defects.length;

    // Display the number of defects
    Echo(0, cDefects.toString(), false, false);
    Echo(0, "", false, false);

    // Return the number of defects
    return cDefects;
}


/////////////////////////////////////////////////////////////////////////////
//
function DoFlatten(strFileIn, strFileOut)
{
    // Verify that the filenames were specified
    if (!strFileIn || !strFileOut || !strFileIn.length || !strFileOut.length)
        return SyntaxViolated("Not enough parameters specified for FLATTEN command.");

    //
    // Transform the specified defect log to a flat file
    //

    // Open the output file
    ContextSet("opening the output file: " + strFileOut);
    var fileOut = g_objFSO.CreateTextFile(strFileOut, true);

    FlattenToFile( false, strFileIn, fileOut );
    fileOut.Close();
    delete fileOut;

    // Flush the console echo
    Echo(0, "", false, false);

    // Indicate success
    return 0;
}


var g_nCoverageSeq;

/////////////////////////////////////////////////////////////////////////////
//
// Helper function for DoFlatten, and DoMerge
// Flattens file strFileIn and appends the results to fileOut,
// which must already be open.
// The fRemoveDups flag governs which XSL stylesheet is used.
// Also appends coverage messages to objCoverage if it is not null
//
function FlattenToFile( fRemoveDups, strFileIn, fileOut, objCoverage )
{
    // Open the specified defect log
    ContextSet("opening the specified defect log file: " + strFileIn);
    var doc = GuardedCreateObject("Microsoft.XMLDOM");
    if (!doc.load(strFileIn))
    {
        var parseError = doc.parseError;
        var strErr = "Error loading " + strFileIn + "\n";
        strErr += parseError.reason   + "\n"
                  strErr += parseError.srcText;
        Echo(0, strErr);
        return 2;
    }

    if ( fRemoveDups )
        strFileStyleSheet = g_strPREfastXSLFlattenSeq;
    else
        strFileStyleSheet = g_strPREfastXSLFlatten;

    // Write coverage info, if any
    if ( objCoverage != null )
    {
        var objCoverageNodes = doc.selectNodes("/DEFECTS/DEFECT[(DEFECTCODE=98101) or (DEFECTCODE=98102)]");
        var objNode;
        for ( objCoverageNodes.reset(), objNode = objCoverageNodes.nextNode();
              objNode != null;
              objNode = objCoverageNodes.nextNode()
            )
        {
            objNode.setAttribute("_seq", ++g_nCoverageSeq );
            objCoverage.documentElement.appendChild( objNode );
        }
    }

    // Open the flatten transformation
    ContextSet("opening the flatten transformation style sheet: " + strFileStyleSheet);
    var docSheet = GuardedCreateObject("Microsoft.XMLDOM");
    if (!docSheet.load(strFileStyleSheet))
    {
        var parseError = doc.parseError;
        var strErr = "Error loading " + strFileStyleSheet + "\n";
        strErr += parseError.reason   + "\n"
                  strErr += parseError.srcText;
        Echo(0, strErr);
        return 2;
    }

    // Transform the defect log
    ContextSet("transforming the defect log to a flat file");
    var strOut = doc.transformNode(docSheet);

    // Write the transformed text to the output file
    ContextSet("writing the transformed text to the output file");
    fileOut.Write(strOut);

    return 0;
}

/////////////////////////////////////////////////////////////////////////////
//
function DoUnflatten(strFileIn, strFileOut)
{
    // Verify that the filenames were specified
    if (!strFileIn || !strFileOut || !strFileIn.length || !strFileOut.length)
        return SyntaxViolated("Not enough parameters specified for the UNFLATTEN command.");

    // Open the specified input file
    ContextSet("opening the specified flat file: " + strFileIn);
    var ts = g_objFSO.OpenTextFile(strFileIn, 1); // ForReading = 1

    // Create the output XML DOM
    ContextSet("creating an XMLDOM instance");
    var doc = GuardedCreateObject("microsoft.xmldom");
    doc.appendChild(doc.createProcessingInstruction("xml", 'version="1.0" encoding="UTF-8"'));
    var defects = doc.appendChild(doc.createElement("DEFECTS"));

    // Loop through each line of the input file
    ContextSet("looping through the input file");
    while (!ts.AtEndOfStream)
    {
        var strLine = ts.ReadLine();
        var arr = strLine.split("\t");

        var defect = defects.appendChild(doc.createElement("DEFECT"));
        defect.setAttribute("_seq", arr[0]);
        var sfa = defect.appendChild(doc.createElement("SFA"));
        sfa.appendChild(doc.createElement("LINE"       )).text = arr[ 1];
        sfa.appendChild(doc.createElement("COLUMN"     )).text = arr[ 2];
        sfa.appendChild(doc.createElement("FILENAME"   )).text = arr[ 3];
        sfa.appendChild(doc.createElement("FILEPATH"   )).text = arr[ 4];
        defect.appendChild(doc.createElement("DEFECTCODE" )).text = arr[ 5];
        defect.appendChild(doc.createElement("DESCRIPTION")).text = arr[ 6];
        defect.appendChild(doc.createElement("RANK"       )).text = arr[ 7];
        defect.appendChild(doc.createElement("MODULE"     )).text = arr[ 8];
        defect.appendChild(doc.createElement("RUNID"      )).text = arr[ 9];
        defect.appendChild(doc.createElement("FUNCTION"   )).text = arr[10];
        defect.appendChild(doc.createElement("FUNCLINE"   )).text = arr[11];
        var elemPath = defect.appendChild(doc.createElement("PATH"       ));
        if (arr[12] && arr[12].length)
        {
            var arrPath = arr[12].split(";");
            for (var iPath = 0; iPath < arrPath.length; ++iPath)
            {
                var strSFA = arrPath[iPath];
                if (strSFA && strSFA.length)
                {
                    var arrSFA = strSFA.split(",");
                    var elemSFA = elemPath.appendChild(doc.createElement("SFA"));
                    elemSFA.appendChild(doc.createElement("LINE"    )).text = arrSFA[0];
                    elemSFA.appendChild(doc.createElement("COLUMN"  )).text = arrSFA[1];
                    elemSFA.appendChild(doc.createElement("FILENAME")).text = arrSFA[2];
                    elemSFA.appendChild(doc.createElement("FILEPATH")).text = arrSFA[3];
                }
            }
        }
    }

    // Close the input text stream
    ts.Close();
    delete ts;

    // Delete the specified output file
    ContextSet("deleting the specified output file: " + strFileOut);
    if (g_objFSO.FileExists(strFileOut))
        g_objFSO.DeleteFile(strFileOut);

    // Save the new XML document to the specified output file name
    ContextSet("saving the XMLDOM instance to the specified output file: " + strFileOut);
    doc.save(strFileOut);

    // Indicate success
    return 0;
}


/////////////////////////////////////////////////////////////////////////////
//
function DoRemoveDups( arrFiles )
{
    ContextSet("Removing duplicate defects");

    // If there are no arguments, remove dups from the default log file
    if ( undefined == arrFiles || 0 == arrFiles.length ) {
        arrFiles = new Array;
        arrFiles[0] = g_dictValidSwitches("LOG");
    }

    // Add the temp file into which we write the grand unified results.
    arrFiles[arrFiles.length] = g_objFSO.GetTempName();

    // Build the path of the executable of interest.
    var strRemoveDupsExe = g_objPREfast.BinArchDirectory + "\\RemoveDups.exe";

    // Verify that removedups.exe exists in the scripts directory
    if (!g_objFSO.FileExists(strRemoveDupsExe)) {
        throw "removedups.exe does not exist in " + g_objPREfast.BinArchDirectory;
    }

    // Exec string starts with the exe name.
    var strRemoveDupsExec = '"' + strRemoveDupsExe + '"';

    // Add the flag to emit sequence numbers.
    strRemoveDupsExec += " -s";

    // Add in the coverage file if there is one.
    var strCoverageFile = g_objProcEnv("PREFASTCOVERAGEFILE");
    if ((strCoverageFile != null) &&
        (strCoverageFile.length != 0)) {

        strRemoveDupsExec += " -c:" + strCoverageFile;
    }

    // Add each log file name to the exec string.  The last file-name is the
    // name of the output file.
    for (var i = 0; i < arrFiles.length; i++) {
        strRemoveDupsExec += " " + arrFiles[i];
    }

    // Fire !
    nRet = RunProcess(strRemoveDupsExec);

    if (nRet != 0) {
        Echo(0, "Could not perform REMOVEDUPS, " + strRemoveDupsExec +
             "returned '" + nRet + "'.\n");

        ContextPop();
        return nRet;
    }

    // Delete the last original file and rename the temp file to replace it.
    // Get the name to rename the temp file to
    var objFileOld = g_objFSO.GetFile(arrFiles[arrFiles.length - 2]);
    var strName = objFileOld.Path;

    var objFileTemp = g_objFSO.GetFile(arrFiles[arrFiles.length - 1]);

    // rename the old source file to something random
    objFileOld.Name = g_objFSO.GetTempName();

    // move and rename the temp file to the old file's name
    g_objFSO.MoveFile(objFileTemp.Name, strName);

    // Delete the old data
    objFileOld.Delete();

    // Now rename the statistical summary file to match.
    var oldSumName = arrFiles[arrFiles.length - 1];
    oldSumName = oldSumName.substring(0, oldSumName.lastIndexOf("."));
    oldSumName += "Sum.txt";

    var newSumName = arrFiles[arrFiles.length - 2];
    newSumName = newSumName.substring(0, newSumName.lastIndexOf("."));
    newSumName += "Sum.txt";

    // Delete the prior instance, if any
    try {
        objFileOld = g_objFSO.GetFile(newSumName);
	objFileOld.Delete();
    }
    catch(e) {}

    g_objFSO.MoveFile(oldSumName, newSumName);

    // All done.
    ContextPop();
    return 0;
}


/////////////////////////////////////////////////////////////////////////////
//
function DoModMan(strCmdLine)
{
    // Set the PREFASTCONFIG environment variable
    ValidateConfigFilePath(g_dictValidSwitches("CONFIGFILE"));

    // Launch the Configuration command line interface
    ContextSet("launching the PREfast Configuration command line interface");

    var strFullCmdLine = g_strPREfastUtil + " modman " + strCmdLine;

    var nRet = RunProcess( strFullCmdLine );
    if ( nRet != 0 )
    {
        Echo( 0, "ERROR: " + strFullCmdLine + " returned bad status '" + nRet + "'.\n" );
    }
    return nRet;
}

/////////////////////////////////////////////////////////////////////////////
//
// DoWriteExitFunctions
//
// Creates an xml model file at strXMLFile containing exit models
// for each function (one per line) in strTextFile.  This file
// can be added to the /MODELFILES argument.
//
function DoWriteExitFunctions( strTextFile, strXMLFile )
{
    // Verify that the filenames were specified
    if (!strTextFile || !strXMLFile || !strTextFile.length || !strXMLFile.length)
        return SyntaxViolated("Not enough parameters specified for the ExitFuncsFromText command.");

	// Create a helper object
	var objFSO = new ActiveXObject("Scripting.FileSystemObject");

	try
	{
		// Attempt to open the text file
		var streamInput = objFSO.OpenTextFile(strTextFile, 1, 0); // 1=ForReading, 0=ASCII
    }
    catch (e)
    {
        Echo( 0, "ERROR: could not read file '" + strTextFile + "'." );
        return 4;
    }

    try
    {
        // Attempt to open the XML file for writing
        var streamOutput = objFSO.CreateTextFile( strXMLFile, true, false ); // Allow overwriting, use ASCII (not unicode)
        streamOutput.WriteLine( "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" );
        streamOutput.WriteLine( "<Models>" );
    }
    catch (e)
    {
        Echo( 0, "ERROR: could not open file '" + strXMLFile + "' for writing." );
        return 4;
    }

	while (!streamInput.AtEndOfStream)
    {
    	var strLine = streamInput.ReadLine();
        var arrFuncs = strLine.split(/\s/);
        for ( var i in arrFuncs )
        {
            if ( arrFuncs[i].length == 0 )
                break;
            streamOutput.WriteLine( "  <Function name=\"" + arrFuncs[i] + "\">" );
            streamOutput.WriteLine( "    <FunctionProperties>"                  );
            streamOutput.WriteLine( "      <Terminates value=\"1\"/>"           );
            streamOutput.WriteLine( "    </FunctionProperties>"                 );
            streamOutput.WriteLine( "  </Function>"                             );
        }
    }

    streamOutput.WriteLine("</Models>");
    streamOutput.Close();
    streamInput.Close();

    return 0;
}

/////////////////////////////////////////////////////////////////////////////
//
// DoReadExitFunctions
//
// Creates a text file at strTextFile containing the names of
// exit functions in strXMLFile, one function name per line
// Only exit functions are written; any other models in strXMLFile
// are ignored.
//
function DoReadExitFunctions( strXMLFile, strTextFile )
{
    // Verify that the filenames were specified
    if (!strTextFile || !strXMLFile || !strTextFile.length || !strXMLFile.length)
        return SyntaxViolated("Not enough parameters specified for the ExitFuncsFromText command.");

	// Create a helper object
	var objFSO = new ActiveXObject("Scripting.FileSystemObject");

    try
    {
        // Attempt to open the text file for writing
        var streamOutput = objFSO.CreateTextFile( strTextFile, true, false ); // Allow overwriting, use ASCII (not unicode)
    }
    catch (e)
    {
        Echo( 0, "ERROR: could not open file '" + strTextFile + "' for writing." );
        return 4;
    }

    var docModels = GuardedCreateObject("Microsoft.XMLDOM");
    if (!docModels.load(strXMLFile))
    {
        var parseError = docModels.parseError;
        var strErr = "Error loading " + strXMLFile + "\n";
        strErr += parseError.reason   + "\n"
                  strErr += parseError.srcText;
        Echo(0, strErr);
        return 2;
    }

    objModels = docModels.selectNodes("Models/Function");
    for ( var i = 0; i < objModels.length; i++ )
    {
        objModel = objModels(i);
        objTerminates = objModel.selectSingleNode( "FunctionProperties/Terminates[@value=1]" );
        if ( objTerminates )
            streamOutput.writeLine( objModel.getAttribute("name") );
    }

    streamOutput.Close();
}


/////////////////////////////////////////////////////////////////////////////
//
function DoInstall( arrModules )
{
    var nRet = 0;
    ContextPush("Installing the specified modules");

    var strResult;
    var dictOldModules = GuardedCreateObject("Scripting.Dictionary");
    var strModule;
    var bFailure = false;

    // Read the existing modules.txt file
    var strModulesDirectory = g_objFSO.GetAbsolutePathName( g_objPREfast.ModulesDirectory );
    var strModulesFile = g_objFSO.BuildPath( strModulesDirectory, "modules.txt" );
    var streamModules = g_objFSO.OpenTextFile( strModulesFile, 1, true );  // 1 is for reading

    while ( !streamModules.AtEndOfStream )
    {
        strModule = streamModules.ReadLine();

        dictOldModules( strModule ) = 1;
    }

    // Close the file and reopen it for appending the names of new modules
    streamModules.Close();
    streamModules = g_objFSO.OpenTextFile( strModulesFile, 8, true ); // 2 is for appending

    for ( var i in arrModules )
    {
        strModule = arrModules[i];
        strResult = "";

        var strBaseName = g_objFSO.GetBaseName( strModule );
        if ( dictOldModules( strBaseName ) != 1 )
            streamModules.WriteLine( strBaseName );
        var strFolder = g_objFSO.GetParentFolderName( g_objFSO.GetAbsolutePathName( strModule ) );
        if ( strFolder != strModulesDirectory )
        {
            var strDestination = g_objFSO.BuildPath( strModulesDirectory, strBaseName );
            g_objFSO.CopyFile( strModule, strDestination, true );
        }
    }

    if ( bFailure )
        return 5;
    else
    {
        var strDefectDefs = g_objFSO.BuildPath( g_objPREfast.DataDirectory, "\\DefectDefs.xml" );
        if ( g_objFSO.FileExists( strDefectDefs ) )
        {
            // ensure that it is deleted, even if it is read-only
            g_objFSO.DeleteFile( strDefectDefs, true );
        }
        var strFullCmdLine = g_strPREfastUtil + " concatenate " + strDefectDefs;

        nRet = RunProcess( strFullCmdLine );
        if ( nRet != 0 )
        {
            Echo( 0, "ERROR: " + strFullCmdLine + " returned bad status '" + nRet + "'.\n" );
        }
    }
    ContextPop();

    return nRet;
}

/////////////////////////////////////////////////////////////////////////////
//
function DoSave()
{
    ContextSet("saving the specified command-line switches");

    var str = "The following switch defaults are in effect:\n";
    if (g_dictSpecdSwitches.Count)
        str += "(switches just saved are indicated with '*')\n";
    if (g_dictSavedSwitches.Count)
        str += "(switches saved earlier are indicated with '+')\n";
    Echo(1, str, true, false);

    for (var it = new Enumerator(g_dictValidSwitches); !it.atEnd(); it.moveNext())
    {
        var strSwitch = it.item();
        var strValue = g_dictValidSwitches(strSwitch);
        var strFormatted = "";
        var strOpposite = g_dictMutexSwitches( strSwitch );
        var boolNew = ( g_dictSpecdSwitches( strSwitch ) != undefined );
        var boolSaved = ( g_dictSavedSwitches( strSwitch ) != undefined );

        if ( strOpposite != undefined )
        {
            strFormatted = "/" + strSwitch;
            if ( boolNew )
            {
                g_objShell.RegWrite( g_strRegPathSAVE + strSwitch + "\\", "" );
                try
                {
                    g_objShell.RegDelete( g_strRegPathSAVE + strOpposite + "\\");
                }
                catch (e)
                {
                }
            }
        }
        else
        {
            strFormatted = "/" + strSwitch + "=" + strValue;
            if ( boolNew )
                g_objShell.RegWrite( g_strRegPathSAVE + "\\" + strSwitch + "\\", strValue );
        }

        // Print all the switch values except the false half of mutex switches
        if ( strOpposite == undefined || strValue == true )
        {
            var strPrepend = "  ";
            if ( boolNew )
                strPrepend = "* ";
            else if ( boolSaved )
                strPrepend = "+ ";
            strFormatted = strPrepend + strFormatted;
            Echo(1, strFormatted, false, false);
        }
    }

    Echo(1, "\n", true, false);

    return 0;
}


/////////////////////////////////////////////////////////////////////////////
//
function DoUnSave()
{
    // Clear the default switches from the registry
    ContextSet("resetting default switches");

    var strSwitch;
    var arrSwitches = new VBArray(g_dictValidSwitches.Keys()).toArray();
    for ( var i in arrSwitches )
    {
        strSwitch = arrSwitches[i];
        try
        {
            strValue = g_objShell.RegDelete(g_strRegPathSAVE + strSwitch + "\\");
        }
        catch (e)
        {
        }
    }

    // Display a confirmation message
    Echo(1, "Switch settings have been restored to product defaults.", true, true);

    // Indicate success
    return 0;
}

//////////////////////////////////////////////////////////////////////////////
//
// Filter strFromFile according to the selected filter preset and
// output to strToFileName.
//
function DoFilter( strFromFile, strToFile )
{
    ContextSet("Filtering defect log file");
    // Verify that the filenames were specified
    if (!strFromFile || !strToFile.length)
        return SyntaxViolated("Not enough parameters specified for FILTER command.");

    var objDefectSet = LoadDefects( strFromFile, true );
    return objDefectSet.save( strToFile, true );
}

/////////////////////////////////////////////////////////////////////////////
//
function DoExec(strCommandName, strCommandArgs, dictSwitches)
{
    var buildresult = 0;

    // Validate that a command was specified
    if (!strCommandName || !strCommandName.length)
        return SyntaxViolated("No build command specified.");

    // Set the PREFASTCONFIG environment variable
    ValidateConfigFilePath(dictSwitches("CONFIGFILE"));

    // Set the PREFASTLOG environment variable
    var strLogReal = ValidateDefectLogPath(dictSwitches("LOG"));

	// Use a name in the temporary directory, and if it's not null after
	// the run, promote it to the real log file.
    var strLog = ValidateDefectLogPath(g_strRunTmpDir + "\\defectDefs.xml");

    // Set the _DFA_MAX_PATHS_ environment variable
    g_objProcEnv("_DFA_MAX_PATHS_") = dictSwitches("MAXPATHS");

    // Set the _PREFAST_MAX_TIME_ environment variable
    g_objProcEnv("_PREFAST_MAX_TIME_") = dictSwitches("MAXTIME");

    // Set the PREFAST_INCREASE_HEAP environment variable
    g_objProcEnv("PREFAST_INCREASE_HEAP") = dictSwitches("INCREASEHEAP");

    // Set the _PRECEDENCE_STACK_THRESHOLD_ environment variable
    g_objProcEnv("_PRECEDENCE_STACK_THRESHOLD_") = dictSwitches("STACKHOGTHRESHOLD");

    // Set the _PRECEDENCE_flag_initcrits_ environment variable
    g_objProcEnv("_PRECEDENCE_FLAG_INITCRITS_") = dictSwitches("ENABLECRITSWARNING");

    // Set the _DFA_NEW_FAILURE_ environment variable
    g_objProcEnv("_DFA_NEW_FAILURE_") = dictSwitches("NEW_FAILURE");
    g_objProcEnv("_DFA_KERNEL_NEW_FAILURE_") = dictSwitches("KERNEL_NEW_FAILURE");

    // Set the PREFAST_USER_CL_EXE environment variable
    g_objProcEnv("PREFAST_USER_CL_EXE") = dictSwitches("CL_EXE");

    // Set the PREFAST_EXCLUDE_LIST environment variable
    g_objProcEnv("PREFAST_EXCLUDE_LIST") = dictSwitches("EXCLUDE");

    // Set the PREFAST_MACRO environment variable
    g_objProcEnv("PREFAST_MACRO") = dictSwitches("MACRO");

    // Set the PREFAST_PERSIST environment variable
    if (dictSwitches("PERSIST"))
        g_objProcEnv("PREFAST_PERSIST") = "1";

    // Set the PREFAST_AMD64 environment variable
    if (dictSwitches("AMD64"))
        g_objProcEnv("PREFAST_AMD64") = "1";

    // Set the PREFAST_IGNORE_MANAGED_OPTIONS environment variable
    if (dictSwitches("IGNOREMANAGEDOPTIONS"))
        g_objProcEnv("PREFAST_IGNORE_MANAGED_OPTIONS") = "1";

    // Set the PREFAST_ONEPASS environment variable, if specified
    if (dictSwitches("ONEPASS"))
        g_objProcEnv("PREFAST_ONEPASS") = "1";

	// We always set this, and almost always set it to 3 below.
	g_objProcEnv("PREFAST_DRIVERS") = "0";

    // Set the PREFAST_DRIVERS environment variable, if specified
    if (dictSwitches("DRIVERS"))
        g_objProcEnv("PREFAST_DRIVERS") = "3";

	// "VS only": for testing
	if (dictSwitches("V"))
        g_objProcEnv("PREFAST_V_FLAG") = "1";

	var oldSize = 0;

    // Reset the defect log, if specified (and it exists).
	if (g_objFSO.FileExists(strLogReal))
	{
		if (dictSwitches("RESET"))
		{
			if (!dictSwitches("N"))
			{
				// Do nothing... the working copy stuff will get it reset
				// incidentally.  (Used to call DoReset.)

				// Report what we just did
				Echo(2,
				     "The PREfast Defect Log file was reset:\n\t\"" 
					 + strLogReal + "\"",
				     true, true);
			}
			else
			{
				Echo(1, "<Reset the defect log>");
			}
		}
		else
		{
			if (!dictSwitches("N"))
			{
				// We're not resetting (the rare case)

				// If there's a log file copy it to the
				// temp, so that if something goes wrong the real log won't be
				// damaged.
				g_objFSO.CopyFile(strLogReal, strLog, true );
				oldSize = g_objFSO.GetFile(strLog).size;
			}
			else
			{
				Echo(1, "<Reset the defect log>");
			}
		}
	}

    // Make our parser the default by changing the PATH and BUILD_PATH environment variables
    g_objProcEnv("PREFAST_ADD_PATH")  = g_objPREfast.BinDirectory + "\\interceptcl\\" + g_objPREfast.ArchDirName + ";";
    g_objProcEnv("PATH")              = g_objProcEnv("PREFAST_ADD_PATH") + g_objProcEnv("PATH");

    if (g_objProcEnv("BUILD_PATH") != "")
    {
        g_objProcEnv("BUILD_PATH")     = g_objProcEnv("PREFAST_ADD_PATH") + g_objProcEnv("BUILD_PATH");
    }

    // Get the filesize of the defect log prior to the launch of the specified command
    var cDefectsBefore = GetDefectCount(strLog);
    var strCommand = strCommandName + "\n" + strCommandArgs;

    // Flush the console echo
    Echo(0, "", false, false);

    // Launch the specified command
    ContextSet("launching the specified command");
    if (!dictSwitches("N"))
    {
        buildresult = RunProcess( strCommand, "\n" );
        if (buildresult != 0)
        {
            Echo( 1, "Error: The build command '" + strCommandName + "' returned status '" + buildresult + "'.\n");
        }
    }
    else
    {
        Echo(1, strCommand);
    }

	if (g_objFSO.FileExists(strLog))
	{
		var newSize = g_objFSO.GetFile(strLog).size;
		// If we got here, and all went reasonably well, copy the log
		// back to the designated location, and that becomes the file
		// we work on for the rest of the run.
		if (oldSize != newSize)
		{
			// It did something, so assume it did the right thing.
			g_objFSO.CopyFile( strLog , strLogReal, true );
			strLog = ValidateDefectLogPath(strLogReal);
		}
		else
		{
			Echo(0, "PREfast did not appear to do anything.")
			Echo(0, "If there was no error on your command line, it may be")
			Echo(0, "that your build command is overriding PREfast's settings.")
			Echo(0, "The prior result log was retained.")
			return buildresult;
		}

		// If the result file doesn't exist or didn't change size, do
		// nothing, which if there was a results file, will leave it unchanged
		// after a bogus call to PREfast.
	}
	else
	{
		Echo(0, "PREfast did not appear to do anything.")
		Echo(0, "If there was no error on your command line, it may be")
		Echo(0, "that your build command is overriding PREfast's settings.")
		Echo(0, "The prior result log was retained.")
		return buildresult;
	}

    // Filter the log file, if specified
    ContextSet("Filtering the log file");
    if (dictSwitches("FILTER"))
    {
        if (!dictSwitches("N"))
        {
            if (g_objFSO.FileExists(strLog))
            {
                Echo(1, "Filtering the defect log...\nFilter in effect: " + g_objFilter.name, true, false);
                DoFilter(strLog, strLog);
            }
        }
        else
        {
            Echo(1, "<filter the log file>");
        }
    }

    // Remove duplicate defects, if specified
    ContextSet("launching the REMOVEDUPS command");
    if (dictSwitches("REMOVEDUPS"))
    {
        if (!dictSwitches("N"))
        {
            if (g_objFSO.FileExists(strLog))
            {
                Echo(1, "Removing duplicate defects from the log...", true, false);
                DoRemoveDups();
            }
        }
        else
        {
            Echo(1, "<Remove duplicate defects>");
        }
    }

    // Get the filesize of the defect log following the launch of the specified command
    var cDefectsAfter = GetDefectCount(strLog);

    // List the defects, if specified
    ContextSet("listing the defect log as specified");
    if (dictSwitches("LIST"))
    {
        if (!dictSwitches("N"))
        {
            var result = DoList(strLog);
            if (result)
                return result;
        }
        else
        {
            Echo(1, "<List the defect log as text>");
        }
    }

    // View the defects, if specified
    ContextSet("viewing the defect log as specified");
    if (dictSwitches("VIEW"))
    {
        if (!dictSwitches("N"))
        {
            var result = DoView(strLog);
            if (result)
                return result;
        }
        else
        {
            Echo(1, "<View the defect log using the UI>");
        }
    }

    // Report if new defects were detected
    ContextSet("reporting the number of defects detected");
    if (!dictSwitches("N"))
    {
        if (cDefectsAfter > cDefectsBefore)
        {
            var str = Decorate(
                              "PREfast reported " + (cDefectsAfter - cDefectsBefore) +
                              " defects during execution of the command.", false, true) + "\n";
            if (!dictSwitches("LIST"))
                str += "Enter PREFAST LIST to list the defect log as text within the console.\n";
            str += "Enter PREFAST VIEW to display the defect log user interface.";
            Echo(1, str, true, false);
        }
        else if (cDefectsAfter == cDefectsBefore)
        {
            Echo(1, "No defects were detected during execution of the command.", true, true);
        }
    }

    // Indicate success
    return buildresult;
}



/////////////////////////////////////////////////////////////////////////////
//
function LoadDefects( strLog, fFiltered )
{
    // Set the PREFASTLOG environment variable
    strLog = ValidateDefectLogPath(strLog, true);

    // Open the defect log
    ContextSet("opening the specified defect log file: " + strLog);
    var objDefectSet = GuardedCreateObject("PREfast.DefectSet");
    try
    {
        objDefectSet.load(strLog);
    }
    catch (e)
    {
        var strErr = "Error loading " + strLog + "\n";
        strErr += e.description;

        throw strErr;
    }

    if ( fFiltered )
    {
        if ( g_objFilter == null )
        {
            LoadPresets();
            g_objFilter = g_objPresets.selectedPreset;
        }
        objDefectSet.filter = g_objFilter;
    }

    return objDefectSet;
}

/////////////////////////////////////////////////////////////////////////////
//
function LoadPresets()
{
    if ( g_objPresets == null )
	{
        g_objPresets = GuardedCreateObject("PREfast.DefectFilterPresets");
    }

    return g_objPresets;
}

/////////////////////////////////////////////////////////////////////////////
//
function ValidateXMLPath(strFilename, strFileType, fReadOnly)
{
    ContextPush("validating the specified file: \"" + strFilename + "\"");

    // Append an ".xml" extension if the filename has no extension
    var iPeriod = strFilename.lastIndexOf('.');
    var iBSlash = strFilename.lastIndexOf('\\');
    if (-1 == iPeriod || iBSlash > iPeriod)
        strFilename += ".xml";

    // Get the absolute path name of the specified file
    strFilename = g_objFSO.GetAbsolutePathName(strFilename);
    ContextPop();
    ContextPush("validating the specified file: \"" + strFilename + "\"");

    // Currently, it doesn't matter if the file exists if we only read from it
    if (!fReadOnly)
    {
        try
        {
            // Ensure that we can write to the file by attempting to rename it
            if (g_objFSO.FileExists(strFilename))
            {
                // Attempt to rename the file
                var objFile = g_objFSO.GetFile(strFilename);
                var strName = objFile.Name;
                var strTemp = g_objFSO.GetTempName();
                objFile.Name = strTemp;
                objFile.Name = strName;
            }
            else
            {
                // Attempt to create the file
                g_objFSO.CreateTextFile(strFilename).Close();
            }
        }
        catch (e)
        {
            var str =
            "The specified " + strFileType + " could not be written to:\n" +
            "\t" + strFilename + "\n\n" +
            "Please check that it is spelled correctly, that the directory exists,\n" +
            "and that you have write permissions to the directory/file.";
            throw str;
        }
    }
    ContextPop();

    return strFilename;
}

/////////////////////////////////////////////////////////////////////////////
//
function ValidateDefectLogPath(strDefectLog, fReadOnly)
{
    ContextPush("validating the specified defect logfile: \"" + strDefectLog + "\"");

    strDefectLog = ValidateXMLPath( strDefectLog, "defect log file", fReadOnly );

    // Set the %PREFASTLOG% environment variable
    g_objProcEnv("PREFASTLOG") = strDefectLog;
    g_dictValidSwitches("LOG") = strDefectLog;

    ContextPop();

    // Return the absolute string
    return strDefectLog;
}

//////////////////////////////////////////////////////////////////////////////
//
function ValidateConfigFilePath( strConfigFile )
{
    ContextPush("validating the specified config file: \"" + strConfigFile + "\"");

    strConfigFile = ValidateXMLPath( strConfigFile, "configuration file", true );

    // Set the PREFASTCONFIG environment variable
    g_objProcEnv("PREFASTCONFIG") = strConfigFile;

    ContextPop();

    //return the absolute string
    return strConfigFile;
}

/////////////////////////////////////////////////////////////////////////////
//
function GetDefectCount(strFile)
{
    ContextPush("creating 'microsoft.xmldom' component");
    var objXMLDoc = GuardedCreateObject("microsoft.xmldom");
    ContextPop();
    try
    {
        objXMLDoc.async = false;
        objXMLDoc.validateOnParse = false;
        if (!objXMLDoc.load(strFile))
            return 0;
        return objXMLDoc.selectNodes("DEFECTS/DEFECT").length;
    }
    catch (e)
    {
        return 0;
    }
}


/////////////////////////////////////////////////////////////////////////////
//
function GetTempFileName()
{
    // TemporaryFolder is special folder #2
    var strTempFolder = g_objFSO.GetSpecialFolder(2).Path + "\\";
    return strTempFolder + g_objFSO.GetTempName();
}


var g_dictPREfastObjects = null;

/////////////////////////////////////////////////////////////////////////////
//
function GuardedCreatePREfastObject(strProgID)
{
    try
    {
        if ( null == g_dictPREfastObjects )
        {
            g_dictPREfastObjects = GuardedCreateObject( "Scripting.Dictionary" );
            g_dictPREfastObjects("PREfast.ExecEnvironment") = "pftExecEnv.wsc";
            g_dictPREfastObjects("PREfast.DefectSet") = "pftDefectSet.wsc";
            g_dictPREfastObjects("PREfast.DefectFilterPresets") = "pftDefectFilterPresets.wsc";
            g_dictPREfastObjects("PREfast.SortKeys") = "pftSortKeys.wsc";
            g_dictPREfastObjects("PREfast.DefectFilter") = "pftDefectFilter.wsc";
            g_dictPREfastObjects("PREfast.DefectRegExpDefs") = "pftDefectRegExpDefs.wsc";
            g_dictPREfastObjects("PREfast.DefectDefs") = "pftDefectDefs.wsc";
        }

        var strFileName = g_dictPREfastObjects(strProgID);
        if ( null == strFileName )
            throw "Unknown PREfast component " + strProgID + "\n";

        var strPath = g_objProcEnv("PREFAST_ROOT");

    	var fDevEnv = false;
	    if (g_objProcEnv("PREFAST_DEVENV"))
		    if (0 != (new Number(g_objProcEnv("PREFAST_DEVENV"))))
			    fDevEnv = true;

        if ( fDevEnv )
            //strPath = g_objFSO.BuildPath( strPath, "bin\\scripts" );
			throw new Error(0,"Development environment not currently supported\n");
        else
            strPath = g_objFSO.BuildPath( strPath, "scripts" );
        strPath = g_objFSO.BuildPath( strPath, strFileName );
        if ( !g_objFSO.FileExists( strPath ) )
            throw "Required PREfast file '" + strPath + "' does not exist\n";

        var strArg = "script:" + strPath;
        var objNew = GetObject( strArg );

        if ( "undefined" != typeof( objNew.FinalConstruct ) )
            objNew.FinalConstruct( GuardedCreatePREfastObject, g_objPREfast );

        return objNew;
    }
    catch ( e )
    {
        Echo( 0, "while creating " + strProgID );
        throw e;
    }
}

/////////////////////////////////////////////////////////////////////////////
//
function GuardedCreateObject(strProgID)
{
    ContextPush("creating " + strProgID + " component");
    var obj;

    try
    {
        // Create the object and return it
        if ("prefast." == strProgID.substr(0, 8).toLowerCase())
            obj = GuardedCreatePREfastObject(strProgID);
        else
            obj = new ActiveXObject(strProgID);
    }
    catch (e)
    {
        // Attempt to display a more informative message about the failure
        var strProduct;
        switch (strProgID.toLowerCase())
        {
        case "microsoft.xmldom":
            strProduct = "Microsoft XML 2.0 or greater";
            break;

        case "scripting.dictionary":
        case "scripting.filesystemobject":
            strProduct = "Microsoft Script Runtime 5.0 or greater";
            break;

        case "wscript.shell":
            strProduct = "Microsoft Windows Script Host 2.0 or greater";
            break;

        default:
            break;
        }

        // Echo the required product name, if known
        if (strProduct && strProduct.length)
            Echo(0, strProduct + " must be properly installed to use PREfast.", true, true);

        // Rethrow the exception
        throw e;
    }

    ContextPop();
    return obj;
}

/////////////////////////////////////////////////////////////////////////////
//
function QuoteArgs( strArgs, strSeparator )
{
    var bSlash;
    var bSpace;
    var strArg;

    if ( !strSeparator || 0 == strSeparator.length )
        return strArgs;

    var arrArgs = strArgs.split( strSeparator );
    for ( var i in arrArgs )
    {
        bSpace = false;
        bSlash = false;
        strArg = arrArgs[i];
        for ( var j = 0; j < strArg.length; j++ )
        {
            var c = strArg.charCodeAt( j );
            if ( c >= 0x09 && c <= 0x0D )
                bSpace = true;
            if ( c == 0x20 )
                bSpace = true;

            if ( bSlash )
                bSlash = false;
            else
            {
                if ( "\\" == strArg.charAt(j) )
                    bSlash = true;
            }
        }
        if ( bSpace )
        {
            if ( bSlash )
                arrArgs[i] = "\"" + strArg + "\\\"";
            else
                arrArgs[i] = "\"" + strArg + "\"";
        }
    }

    return arrArgs.join( " " );
}

var g_fReportedBroken = false;

/////////////////////////////////////////////////////////////////////////////
//
function RunProcess( strArgs, strSeparator )
{

    var strQuotedArgs = QuoteArgs( strArgs, strSeparator );

    if ( "undefined" == typeof( g_objShell.Exec ) )
    {
	if ( !g_fReportedBroken )
        {
            WScript.Echo("Upgrade to IE 6.0 or greater to run your build in the current window");
            g_fReportedBroken = true;
        }
        var strRunCommand = g_objProcEnv("COMSPEC") + " /c " + strQuotedArgs;
        return g_objShell.Run( strRunCommand, 1, true );
    }

	// The obvious "right" way to do this is to use the StdIn and StdErr
	// properties.  However, those don't come ready until the whole stream
	// has been written, causing the results to be delayed.  But we want
	// to see the results in real time, so use real files.

    var strTempFolder = g_objFSO.GetSpecialFolder(2).Path;

	// Get the output temp files -- be sure they're available names.
    var strOutFile = g_objFSO.BuildPath( strTempFolder, g_objFSO.GetTempName() );
	while (g_objFSO.FileExists(strOutFile) || g_objFSO.FolderExists(strOutFile))
	{
        strOutFile = g_objFSO.BuildPath( strTempFolder, g_objFSO.GetTempName() );
	}

    var strErrFile = g_objFSO.BuildPath( strTempFolder, g_objFSO.GetTempName() );
	while (g_objFSO.FileExists(strErrFile) || g_objFSO.FolderExists(strErrFile))
	{
        strErrFile = g_objFSO.BuildPath( strTempFolder, g_objFSO.GetTempName() );
	}

    var strCmd = g_objProcEnv("COMSPEC") + " /c " + strQuotedArgs + " > " + strOutFile + " 2> " + strErrFile;
    var status;
    var streamOut;
    var streamErr;
    var bReady;
	var oExec = g_objShell.Exec( strCmd );
	do		
	{
		status = oExec.status;		
		
		if ( undefined == streamOut )
		{
			try
			{
				streamOut = g_objFSO.OpenTextFile( strOutFile, 1, false );
			}
			catch (e)
			{
			}
		}
		if ( undefined == streamErr )
		{
			try
			{
				streamErr = g_objFSO.OpenTextFile( strErrFile, 1, false );
			}
			catch (e) {}
		}
		do
		{
			bReady = false;
			if ( undefined != streamOut && !streamOut.AtEndOfStream )
			{
                try
                {
				    WScript.StdOut.WriteLine( streamOut.ReadLine() );
                }
                catch (e) {} // We can't write to NUL, apparently
				bReady = true;
			}
			if ( undefined != streamErr && !streamErr.AtEndOfStream )
			{
                try
                {
				    WScript.StdErr.WriteLine( streamErr.ReadLine() );
                }
                catch (e) {} // We can't write to NUL
				bReady = true;
			}
		}
		while ( bReady );
		WScript.Sleep(50);
	} while ( 0 == status );

	if ( undefined != streamOut )	
		streamOut.Close();
	if ( undefined != streamErr )
		streamErr.Close();
	
	try
	{
		g_objFSO.DeleteFile( strOutFile );	
	}
	catch (e) {}
	try
	{
		g_objFSO.DeleteFile( strErrFile );
	}
	catch (e) {}

    return oExec.ExitCode;
}
/////////////////////////////////////////////////////////////////////////////
//
function ContextSet(strContext)
{
    g_arrContext[0] = strContext;
    g_arrContext.length = 1;
}


/////////////////////////////////////////////////////////////////////////////
//
function ContextPush(strContext)
{
    g_arrContext[g_arrContext.length] = strContext;
}


/////////////////////////////////////////////////////////////////////////////
//
function ContextPop()
{
    g_arrContext.length = g_arrContext.length - 1;
}


/////////////////////////////////////////////////////////////////////////////
//
function ContextTop()
{
    return g_arrContext.length ? g_arrContext[g_arrContext.length - 1] : null;
}


/////////////////////////////////////////////////////////////////////////////
//
var g_strHorizontalBar;
function GetHorizontalBar(fAppendLF)
{
    if (!g_strHorizontalBar)
        g_strHorizontalBar =
        "---------------------------------------" +
        "--------------------------------------";
    return fAppendLF ? g_strHorizontalBar + "\n" : g_strHorizontalBar;
}


/////////////////////////////////////////////////////////////////////////////
//
var g_fLastEchoNeedsBottomBar;
function Decorate(str, fTopBar, fBottomBar)
{
    // Modify string with horizontal bars, if specified
    if (fTopBar)
        str = GetHorizontalBar(str.length) + str;
    if (fBottomBar)
        str += "\n" + GetHorizontalBar(false);
    return str;
}


/////////////////////////////////////////////////////////////////////////////
//
function Echo(nDetailLevel, str, fTopBar, fBottomBar)
{
    // Do not display if detail level is higher than current verbosity level
    if (nDetailLevel > g_nVerbosity)
        return;

    // If the last echo needed a bottom bar, set fTopBar
    if (g_fLastEchoNeedsBottomBar)
        fTopBar = true;

    // Save the fBottomBar for next echo
    g_fLastEchoNeedsBottomBar = fBottomBar;

    // Decorate the text as specified
    str = Decorate(str, fTopBar, false);

    // Do not display if string is empty
    if (!str.length)
        return;

    // Normal processing for verbosity levels 0-2
    if (g_nVerbosity < 3)
    {
        WScript.Echo(str);
        return;
    }

    // Special processing for highest verbosity level
    var arr = str.split("\n");
    for (var i = 0; i < arr.length; ++i)
        arr[i] = nDetailLevel + ": " + arr[i];
    WScript.Echo(arr.join("\n"));
}


/////////////////////////////////////////////////////////////////////////////
//
function DisplayLogo()
{
    Echo(1,
         "Microsoft (R) PREfast Version " + g_objPREfast.ComposeVersionString() + ".\n" +
         "Copyright (C) Microsoft Corporation. All rights reserved.",
         true, true);
}


/////////////////////////////////////////////////////////////////////////////
//
function SyntaxViolated(strDetail, fSuppress)
{
    if (!fSuppress)
    {
        strDetail =
        Decorate(strDetail, false, true) +
        "\n\n" +
        "Enter PREFAST /?   for quick syntax help.\n" +
        "Enter PREFAST HELP for product documentation.\n";
        Echo(0, strDetail, true, false);
    }
    return 2;
}

/////////////////////////////////////////////////////////////////////////////
//
function CheckJScriptVersion( nMajorNeeded, nMinorNeeded )
{
    var nMajor;
    var nMinor;
    var strHave;
    try
    {
        nMajor = ScriptEngineMajorVersion();
        nMinor = ScriptEngineMinorVersion();

        if ( nMajor > nMajorNeeded || ( nMajor == nMajorNeeded && nMinor >= nMinorNeeded ) )
        {
            // Version is ok
            return 0;
        }
        strHave = "JScript version " + nMajor + "." + nMinor;
    }
    catch ( e )
    {
        // ScriptEngineMajorVersion isn't even implemented until JScript version 5,
        // so we may come here on very old platforms.
        strHave = "A JScript version older than 5.0";

    }
    strError = "ERROR: PREfast requires at least JScript version " + nMajorNeeded + "." + nMinorNeeded + ".\n";
    strError += strHave + " is installed.\n";
    strError += "Upgrading to a current version of Internet Explorer will fix the problem.\n";
    Echo( 0, strError );
    WScript.Quit( 10 );
}


/////////////////////////////////////////////////////////////////////////////
//
function Syntax()
{
    //---+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8
    var str =                                                                          //
        "The syntax of this command is:\n\n" +                                         //
                                                                                       //
        "    PREFAST /?\n" +                                                           //
        "    PREFAST /HELP\n\n" +                                                      //
                                                                                       //
        "    PREFAST HELP\n\n" +                                                       //
                                                                                       //
        "    PREFAST VERSION\n\n" +                                                    //
                                                                                       //
        "    PREFAST [/LOG=logfile] RESET\n\n" +                                       //
                                                                                       //
        "    PREFAST [/LOG=logfile]\n" +                                               //
        "            [/FILTERPRESET=filter preset name]\n" +                           //
        "            LIST\n\n" +                                                       //
                                                                                       //
        "    PREFAST [/LOG=logfile]\n" +                                               //
        "            [/FILTERPRESET=filter preset name]\n" +                           //
        "            VIEW\n\n" +                                                       //
                                                                                       //
        "    PREFAST [/LOG=logfile]\n" +                                               //
        "            [/FILTER [/FILTERPRESET=filter]]\n" +                             //
        "            [/NEW_FAILURE={never | null | THROW}]\n" +                        //
        "            [/KERNEL_NEW_FAILURE={never | NULL | throw}]\n" +                 //
        "            [/LIST]\n" +                                                      //
        "            [/VIEW]\n" +                                                      //
        "            [/[NO]RESET]\n" +                                                 //
        "            build_command [arguments]\n";                                     //
                                                                                       //
        //---+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8

    Echo(0, str);
    return 1;
}
