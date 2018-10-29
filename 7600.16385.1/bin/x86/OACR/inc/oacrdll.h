#pragma once
/*****************************************************************************

   Module  : OACRDll
   Owner   : MarcK

******************************************************************************

  APIs exported by oacr.dll.
  
  oacr.dll is part of the Office Auto Code Review (OACR) system. It provides
  the same functionality as oacr.exe. It is intended for build integration
  from build environments using their own executable.

*****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

// Typedefs for dynamic loading via LoadLibrary and GetProcAddress
typedef int (__stdcall* OACRCMDWPROC)( const wchar_t* );
typedef int (__stdcall* OACRCMDAPROC)( const char* );


// Run an OACR command. Same command line syntax as the oacr.exe command line tool.
int __stdcall OACRCmdW( const wchar_t* wzCmd );
int __stdcall OACRCmdA( const char* szCmd );

#ifdef __cplusplus
}
#endif
