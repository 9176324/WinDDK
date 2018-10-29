
//=============================================================================
//  userenv.h   -   Header file for user environment API.
//                  User Profiles, environment variables, and Group Policy
//
//  Copyright (c) Microsoft Corporation 1995-2002
//  All rights reserved
//
//=============================================================================


#ifndef _INC_USERENV
#define _INC_USERENV

#include <wbemcli.h>

//
// Define API decoration for direct importing of DLL references.
//

#if !defined(_USERENV_)
#define USERENVAPI DECLSPEC_IMPORT
#else
#define USERENVAPI
#endif


#ifdef __cplusplus
extern "C" {
#endif

//=============================================================================
//
// LoadUserProfile
//
// Loads the specified user's profile.
//
// Most applications should not need to use this function.  It's used
// when a user has logged onto the system or a service starts in a named
// user account.
//
// hToken        - Token for the user, returned from LogonUser()
// lpProfileInfo - Address of a PROFILEINFO structure
//
// Returns:  TRUE if successful
//           FALSE if not.  Call GetLastError() for more details
//
// Note:  The caller of this function must have admin privileges on the machine.
//
//        Upon successful return, the hProfile member of the PROFILEINFO
//        structure is a registry key handle opened to the root
//        of the user's hive.  It has been opened with full access. If
//        you need to read or write to the user's registry file, use
//        this key instead of HKEY_CURRENT_USER.  Do not close this
//        handle.  Instead pass it to UnloadUserProfile to close
//        the handle.
//
//=============================================================================

#include <profinfo.h>

#define PI_NOUI         0x00000001      // Prevents displaying of messages
#define PI_APPLYPOLICY  0x00000002      // Apply NT4 style policy

USERENVAPI
BOOL
WINAPI
LoadUserProfileA(
    __in    HANDLE          hToken,
    __inout LPPROFILEINFOA  lpProfileInfo);
USERENVAPI
BOOL
WINAPI
LoadUserProfileW(
    __in    HANDLE          hToken,
    __inout LPPROFILEINFOW  lpProfileInfo);
#ifdef UNICODE
#define LoadUserProfile  LoadUserProfileW
#else
#define LoadUserProfile  LoadUserProfileA
#endif // !UNICODE

//=============================================================================
//
// UnloadUserProfile
//
// Unloads a user's profile that was loaded by LoadUserProfile()
//
// hToken        -  Token for the user, returned from LogonUser()
// hProfile      -  hProfile member of the PROFILEINFO structure
//
// Returns:  TRUE if successful
//           FALSE if not.  Call GetLastError() for more details
//
// Note:     The caller of this function must have admin privileges on the machine.
//
//=============================================================================

USERENVAPI
BOOL
WINAPI
UnloadUserProfile(
    __in    HANDLE  hToken,
    __in    HANDLE  hProfile);

//=============================================================================
//
// GetProfilesDirectory
//
// Returns the path to the root of where all user profiles are stored.
//
// lpProfilesDir  -  Receives the path
// lpcchSize      -  Size of lpProfilesDir
//
// Returns:  TRUE if successful
//           FALSE if not.  Call GetLastError() for more details
//
// Note:     If lpProfilesDir is not large enough or NULL, the function will fail,
//           and lpcchSize will contain the necessary buffer size.
//
// Example return value: C:\Users
//
//=============================================================================

USERENVAPI
BOOL
WINAPI
GetProfilesDirectoryA(
    __out_ecount_opt(*lpcchSize)    LPSTR lpProfileDir,
    __inout                         LPDWORD lpcchSize);
USERENVAPI
BOOL
WINAPI
GetProfilesDirectoryW(
    __out_ecount_opt(*lpcchSize)    LPWSTR lpProfileDir,
    __inout                         LPDWORD lpcchSize);
#ifdef UNICODE
#define GetProfilesDirectory  GetProfilesDirectoryW
#else
#define GetProfilesDirectory  GetProfilesDirectoryA
#endif // !UNICODE

//=============================================================================
//
//  GetProfileType()
//
//  Returns the type of the profile that is loaded for a user.
//
//  dwFlags   - Returns the profile flags
//
//  Return:     TRUE if successful
//              FALSE if an error occurs. Call GetLastError for more details
//
//  Comments:   if profile is not already loaded the function will return an error.
//              The caller needs to have access to HKLM part of the registry.
//              (exists by default)
//
//=============================================================================

#if(WINVER >= 0x0500)

//
// Flags that can be set in the dwFlags field
//

#define PT_TEMPORARY         0x00000001      // A profile has been allocated that will be deleted at logoff.
#define PT_ROAMING           0x00000002      // The loaded profile is a roaming profile.
#define PT_MANDATORY         0x00000004      // The loaded profile is mandatory.

USERENVAPI
BOOL
WINAPI
GetProfileType(
    __out DWORD *dwFlags);

#endif /* WINVER >= 0x0500 */

//=============================================================================
//
//  DeleteProfile()
//
//  Deletes the profile and all other user related settings from the machine
//
//  lpSidString    - String form of the user sid.
//  lpProfilePath  - ProfilePath (if Null, lookup in the registry)
//  lpComputerName - Computer Name from which profile has to be deleted
//
//  Return:     TRUE if successful
//              FALSE if an error occurs. Call GetLastError for more details
//
//  Comments:   Deletes the profile directory, registry and appmgmt stuff
//=============================================================================

#if(WINVER >= 0x0500)

USERENVAPI
BOOL
WINAPI
DeleteProfileA (
    __in        LPCSTR    lpSidString,
    __in_opt    LPCSTR    lpProfilePath,
    __in_opt    LPCSTR    lpComputerName);
USERENVAPI
BOOL
WINAPI
DeleteProfileW (
    __in        LPCWSTR    lpSidString,
    __in_opt    LPCWSTR    lpProfilePath,
    __in_opt    LPCWSTR    lpComputerName);
#ifdef UNICODE
#define DeleteProfile  DeleteProfileW
#else
#define DeleteProfile  DeleteProfileA
#endif // !UNICODE

#endif /* WINVER >= 0x0500 */

//=============================================================================
//
//  CreateProfile()
//
//  Creating a user profile without loading it. 
//
//  pszUserSid     - String form of the user sid.
//  pszUserName    - Name of the user, used as the base name to create the 
//                   profile directory
//  pszProfilePath - Returned full profile path 
//  cchProfilePath - Size of pszProfilePath buffer
//
//  Return:     S_OK : Successfully created the profile
//              E_ACCESSDENIED : The caller does not have enough permission to
//                     create the profile. The caller has to be administrators
//                     in order for this API to succeed.
//              HRESULT_FROM_WIN32(ERROR_ALREADY_EXISTS) : a profile is 
//                     already exists for the given user.
//              Others : Standard HRESULT error codes.
//                          
//  Comments:   
//=============================================================================

#if(WINVER >= 0x0600)

USERENVAPI
HRESULT
WINAPI
CreateProfile(
    __in                            LPCWSTR pszUserSid,
    __in                            LPCWSTR pszUserName,
    __out_ecount(cchProfilePath)    LPWSTR  pszProfilePath,
    __in                            DWORD   cchProfilePath);

#endif /* WINVER >= 0x0600 */

//=============================================================================
//
// GetDefaultUserProfilesDirectory
//
// Returns the path to the root of the default user profile
//
// lpProfileDir   -  Receives the path
// lpcchSize      -  Size of lpProfileDir
//
// Returns:  TRUE if successful
//           FALSE if not.  Call GetLastError() for more details
//
// Note:     If lpProfileDir is not large enough or NULL, the function will fail,
//           and lpcchSize will contain the necessary buffer size.
//
// Example return value: C:\Users\Default
//
//=============================================================================

#if(WINVER >= 0x0500)

USERENVAPI
BOOL
WINAPI
GetDefaultUserProfileDirectoryA(
    __out_ecount_opt(*lpcchSize)    LPSTR lpProfileDir,
    __inout                         LPDWORD lpcchSize);
USERENVAPI
BOOL
WINAPI
GetDefaultUserProfileDirectoryW(
    __out_ecount_opt(*lpcchSize)    LPWSTR lpProfileDir,
    __inout                         LPDWORD lpcchSize);
#ifdef UNICODE
#define GetDefaultUserProfileDirectory  GetDefaultUserProfileDirectoryW
#else
#define GetDefaultUserProfileDirectory  GetDefaultUserProfileDirectoryA
#endif // !UNICODE

#endif /* WINVER >= 0x0500 */

//=============================================================================
//
// GetAllUsersProfilesDirectory
//
// Returns the path to the root of the All Users profile
//
// lpProfileDir   -  Receives the path
// lpcchSize      -  Size of lpProfileDir
//
// Returns:  TRUE if successful
//           FALSE if not.  Call GetLastError() for more details
//
// Note:     If lpProfileDir is not large enough or NULL, the function will fail,
//           and lpcchSize will contain the necessary buffer size.
//
// Example return value: C:\Program Data
//
//=============================================================================

#if(WINVER >= 0x0500)

USERENVAPI
BOOL
WINAPI
GetAllUsersProfileDirectoryA(
    __out_ecount_opt(*lpcchSize)    LPSTR lpProfileDir,
    __inout                         LPDWORD lpcchSize);
USERENVAPI
BOOL
WINAPI
GetAllUsersProfileDirectoryW(
    __out_ecount_opt(*lpcchSize)    LPWSTR lpProfileDir,
    __inout                         LPDWORD lpcchSize);
#ifdef UNICODE
#define GetAllUsersProfileDirectory  GetAllUsersProfileDirectoryW
#else
#define GetAllUsersProfileDirectory  GetAllUsersProfileDirectoryA
#endif // !UNICODE

#endif /* WINVER >= 0x0500 */

//=============================================================================
//
// GetUserProfileDirectory
//
// Returns the path to the root of the requested user's profile
//
// hToken         -  User's token returned from LogonUser()
// lpProfileDir   -  Receives the path
// lpcchSize      -  Size of lpProfileDir
//
// Returns:  TRUE if successful
//           FALSE if not.  Call GetLastError() for more details
//
// Note:     If lpProfileDir is not large enough or NULL, the function will fail,
//           and lpcchSize will contain the necessary buffer size.
//
// Example return value: C:\Users\Joe
//
//=============================================================================

USERENVAPI
BOOL
WINAPI
GetUserProfileDirectoryA(
    __in                            HANDLE  hToken,
    __out_ecount_opt(*lpcchSize)    LPSTR lpProfileDir,
    __inout                         LPDWORD lpcchSize);
USERENVAPI
BOOL
WINAPI
GetUserProfileDirectoryW(
    __in                            HANDLE  hToken,
    __out_ecount_opt(*lpcchSize)    LPWSTR lpProfileDir,
    __inout                         LPDWORD lpcchSize);
#ifdef UNICODE
#define GetUserProfileDirectory  GetUserProfileDirectoryW
#else
#define GetUserProfileDirectory  GetUserProfileDirectoryA
#endif // !UNICODE

//=============================================================================
//
// CreateEnvironmentBlock
//
// Returns the environment variables for the specified user.  This block
// can then be passed to CreateProcessAsUser().
//
// lpEnvironment  -  Receives a pointer to the new environment block
// hToken         -  User's token returned from LogonUser() (optional, can be NULL)
// bInherit       -  Inherit from the current process's environment block
//                   or start from a clean state.
//
// Returns:  TRUE if successful
//           FALSE if not.  Call GetLastError() for more details
//
// Note:     If hToken is NULL, the returned environment block will contain
//           system variables only.
//
//           Call DestroyEnvironmentBlock to free the buffer when finished.
//
//           If this block is passed to CreateProcessAsUser, the
//           CREATE_UNICODE_ENVIRONMENT flag must also be set.
//
//=============================================================================

BOOL
WINAPI
CreateEnvironmentBlock(
    __out       LPVOID  *lpEnvironment,
    __in_opt    HANDLE  hToken,
    __in        BOOL    bInherit);

//=============================================================================
//
// DestroyEnvironmentBlock
//
// Frees environment variables created by CreateEnvironmentBlock
//
// lpEnvironment  -  A pointer to the environment block
//
// Returns:  TRUE if successful
//           FALSE if not.  Call GetLastError() for more details
//
//=============================================================================

BOOL
WINAPI
DestroyEnvironmentBlock(
    __in    LPVOID  lpEnvironment);

//=============================================================================
//
// ExpandEnvironmentStringsForUser
//
// Expands the source string using the environment block for the
// specified user.  If hToken is null, the system environment block
// will be used (no user environment variables).
//
// hToken         -  User's token returned from LogonUser() (optional, can be NULL)
// lpSrc          -  Pointer to the string with environment variables
// lpDest         -  Buffer that receives the expanded string
// dwSize         -  Size of lpDest in characters (max chars)
//
// Returns:  TRUE if successful
//           FALSE if not.  Call GetLastError() for more details
//
// Note:     If the user profile for hToken is not loaded, this api will fail.
//
//=============================================================================

#if(WINVER >= 0x0500)

USERENVAPI
BOOL
WINAPI
ExpandEnvironmentStringsForUserA(
    __in_opt                HANDLE      hToken,
    __in                    LPCSTR    lpSrc,
    __out_ecount(dwSize)    LPSTR     lpDest,
    __in                    DWORD       dwSize);
USERENVAPI
BOOL
WINAPI
ExpandEnvironmentStringsForUserW(
    __in_opt                HANDLE      hToken,
    __in                    LPCWSTR    lpSrc,
    __out_ecount(dwSize)    LPWSTR     lpDest,
    __in                    DWORD       dwSize);
#ifdef UNICODE
#define ExpandEnvironmentStringsForUser  ExpandEnvironmentStringsForUserW
#else
#define ExpandEnvironmentStringsForUser  ExpandEnvironmentStringsForUserA
#endif // !UNICODE

#endif /* WINVER >= 0x0500 */

//=============================================================================
//
// RefreshPolicy()
//
// Causes group policy to be applied immediately on the client machine
//
// bMachine  -  Refresh machine or user policy
//
// Returns:  TRUE if successful
//           FALSE if not.  Call GetLastError() for more details
//
//=============================================================================

#if(WINVER >= 0x0500)

USERENVAPI
BOOL
WINAPI
RefreshPolicy(
    __in        BOOL bMachine);

#endif /* WINVER >= 0x0500 */

//=============================================================================
//
// RefreshPolicyEx()
//
// Causes group policy to be applied immediately on the client machine. 
//
// bMachine  -  Refresh machine or user policy
// dwOptions -  Option specifying the kind of refresh that needs to be done.
//
// Returns:  TRUE if successful
//           FALSE if not.  Call GetLastError() for more details
//
//=============================================================================

#if(WINVER >= 0x0500)

#define RP_FORCE            1      // Refresh policies without any optimisations.
#define RP_SYNC             2      // Refresh Policy sync; the call does not return till the time policy processing is completed

USERENVAPI
BOOL
WINAPI
RefreshPolicyEx(
    __in        BOOL    bMachine, 
    __in        DWORD   dwOptions);

#endif /* WINVER >= 0x0500 */

//=============================================================================
//
// EnterCriticalPolicySection
//
// Pauses the background application of group policy to allow safe
// reading of the registry.  Applications that need to read multiple
// policy entries and ensure that the values are not changed while reading
// them should use this function.
//
// The maximum amount of time an application can hold a critical section
// is 10 minutes.  After 10 minutes, policy can be applied again.
//
// bMachine -  Pause machine or user policy
//
// Returns:  Handle if successful
//           NULL if not.  Call GetLastError() for more details
//
// Note 1:  The handle returned should be passed to LeaveCriticalPolicySection
// when finished.  Do not close this handle, LeaveCriticalPolicySection
// will do that.
//
// Note 2:  If both user and machine critical sections need to be acquired then
// they should be done in this order: first acquire user critical section and
// then acquire machine critical section.
//
//=============================================================================

#if(WINVER >= 0x0500)

USERENVAPI
HANDLE
WINAPI
EnterCriticalPolicySection(
    __in        BOOL    bMachine);

#endif /* WINVER >= 0x0500 */

//=============================================================================
//
// LeaveCriticalPolicySection
//
// Resumes the background application of group policy.  See
// EnterCriticalPolicySection for more details.
//
// hSection - Handle returned from EnterCriticalPolicySection
//
// Returns:  TRUE if successful
//           FALSE if not.  Call GetLastError() for more details
//
// Note:  This function will close the handle.
//
//=============================================================================

#if(WINVER >= 0x0500)

USERENVAPI
BOOL
WINAPI
LeaveCriticalPolicySection(
    __in        HANDLE  hSection);

#endif /* WINVER >= 0x0500 */

//=============================================================================
//
// RegisterGPNotification
//
// Entry point for registering for Group Policy change notification.
//
// Parameters: hEvent     -   Event to be notified, by calling SetEvent(hEvent)
//             bMachine   -   If true, then register machine policy notification
//                                     else register user policy notification
//
// Returns:    True if successful
//             False if error occurs
//
// Notes:      Group Policy Notifications.  There are 2 ways an application can
//             be notify when Group Policy is finished being applied.
//
//             1) Using the RegisterGPNotifcation function and waiting for the
//                event to be signalled.
//
//             2) A WM_SETTINGCHANGE message is broadcast to all desktops.
//                wParam - 1 if machine policy was applied, 0 if user policy was applied.
//                lParam - Points to the string "Policy"
//
//=============================================================================

#if(WINVER >= 0x0500)

USERENVAPI
BOOL
WINAPI
RegisterGPNotification(
    __in        HANDLE  hEvent,
    __in        BOOL    bMachine);

#endif /* WINVER >= 0x0500 */

//=============================================================================
//
// UnregisterGPNotification
//
// Removes registration for a Group Policy change notification.
//
// Parameters: hEvent    -   Event to be removed
//
// Returns:    True if successful
//             False if error occurs
//
//=============================================================================

#if(WINVER >= 0x0500)

USERENVAPI
BOOL
WINAPI
UnregisterGPNotification(
    __in        HANDLE  hEvent);

#endif /* WINVER >= 0x0500 */

//=============================================================================
//
// GPOptions flags
//
// These are the flags found in the GPOptions property of a DS object
//
// For a given DS object (Site, Domain, OU), the GPOptions property
// contains options that effect all the GPOs link to this SDOU.
//
// This is a DWORD type
//
//=============================================================================

#if(WINVER >= 0x0500)

#define GPC_BLOCK_POLICY        0x00000001  // Block all non-forced policy from above

#endif /* WINVER >= 0x0500 */

//=============================================================================
//
// GPLink flags
//
// These are the flags found on the GPLink property of a DS object after
// the GPO path.
//
// For a given DS object (Site, Domain, OU), the GPLink property will
// be in this text format
//
// [LDAP://CN={E615A0E3-C4F1-11D1-A3A7-00AA00615092},CN=Policies,CN=System,DC=mydomain,DC=Microsoft,DC=Com;1]
//
// The GUID is the GPO name, and the number following the LDAP path are the options
// for that link from this DS object.  Note, there can be multiple GPOs
// each in their own square brackets in a prioritized list.
//
//=============================================================================

#if(WINVER >= 0x0500)

//
// Options for a GPO link
//

#define GPO_FLAG_DISABLE        0x00000001  // This GPO is disabled
#define GPO_FLAG_FORCE          0x00000002  // Don't override the settings in
                                            // this GPO with settings from
                                            // a GPO below it.
#endif /* WINVER >= 0x0500 */

//=============================================================================
//
// GetGPOList
//
//
// Queries for the list of Group Policy Objects for the specified
// user or machine.  This function will return a link list
// of Group Policy Objects.  Call FreeGPOList to free the list.
//
// Note, most applications will not need to call this function.
// This will primarily be used by services acting on behalf of
// another user or machine.  The caller of this function will
// need to look in each GPO for their specific policy
//
// This function can be called in two different ways.  Either the hToken for
// a user or machine can be supplied and the correct name and domain
// controller name will be generated, or hToken is NULL and the caller
// must supply the name and the domain controller name.
//
// Calling this function with an hToken ensures the list of Group Policy
// Objects is correct for the user or machine since security access checking
// can be perfomed.  If hToken is not supplied, the security of the caller
// is used instead which means that list may or may not be 100% correct
// for the intended user / machine.  However, this is the fastest way
// to call this function.
//
// hToken           - User or machine token, if NULL, lpName and lpHostName must be supplied
// lpName           - User or machine name in DN format, if hToken is supplied, this must be NULL
// lpHostName       - Domain DN name or domain controller name. If hToken is supplied, this must be NULL
// lpComputerName   - Computer name to use to determine site location.  If NULL,
//                    the local computer is used as the reference. Format:  \\machinename
// dwFlags          - Flags field.  See flags definition below
// pGPOList         - Address of a pointer which receives the link list of GPOs
//
//
// Returns:  TRUE if successful
//           FALSE if not.  Use GetLastError() for more details.
//
// Examples:
//
// Here's how this function will typically be called for
// looking up the list of GPOs for a user:
//
//      LPGROUP_POLICY_OBJECT  pGPOList;
//
//      if (GetGPOList (hToken, NULL, NULL, NULL, 0, &pGPOList))
//      {
//          // do processing here...
//          FreeGPOList (pGPOList);
//      }
//
//
// Here's how this function will typically be called for
// looking up the list of GPOs for a machine:
//
//      LPGROUP_POLICY_OBJECT  pGPOList;
//
//      if (GetGPOList (NULL, lpMachineName, lpHostName, lpMachineName,
//                      GPO_LIST_FLAG_MACHINE, &pGPOList))
//      {
//          // do processing here...
//          FreeGPOList (pGPOList);
//      }
//
//=============================================================================

#if(WINVER >= 0x0500)

//
// Each Group Policy Object is associated (linked) with a site, domain,
// organizational unit, or machine.
//

typedef enum _GPO_LINK {
    GPLinkUnknown = 0,                     // No link information available
    GPLinkMachine,                         // GPO linked to a machine (local or remote)
    GPLinkSite,                            // GPO linked to a site
    GPLinkDomain,                          // GPO linked to a domain
    GPLinkOrganizationalUnit               // GPO linked to a organizational unit
} GPO_LINK, *PGPO_LINK;

typedef struct _GROUP_POLICY_OBJECTA {
    DWORD       dwOptions;                  // See GPLink option flags above
    DWORD       dwVersion;                  // Revision number of the GPO
    LPSTR       lpDSPath;                   // Path to the Active Directory portion of the GPO
    LPSTR       lpFileSysPath;              // Path to the file system portion of the GPO
    LPSTR       lpDisplayName;              // Friendly display name
    CHAR        szGPOName[50];              // Unique name
    GPO_LINK    GPOLink;                    // Link information
    LPARAM      lParam;                     // Free space for the caller to store GPO specific information
    struct _GROUP_POLICY_OBJECTA * pNext;   // Next GPO in the list
    struct _GROUP_POLICY_OBJECTA * pPrev;   // Previous GPO in the list
    LPSTR       lpExtensions;               // Extensions that are relevant for this GPO
    LPARAM      lParam2;                    // Free space for the caller to store GPO specific information
    LPSTR       lpLink;                     // Path to the Active Directory site, domain, or organizational unit this GPO is linked to
                                            // If this is the local GPO, this points to the word "Local"
} GROUP_POLICY_OBJECTA, *PGROUP_POLICY_OBJECTA;
typedef struct _GROUP_POLICY_OBJECTW {
    DWORD       dwOptions;                  // See GPLink option flags above
    DWORD       dwVersion;                  // Revision number of the GPO
    LPWSTR      lpDSPath;                   // Path to the Active Directory portion of the GPO
    LPWSTR      lpFileSysPath;              // Path to the file system portion of the GPO
    LPWSTR      lpDisplayName;              // Friendly display name
    WCHAR       szGPOName[50];              // Unique name
    GPO_LINK    GPOLink;                    // Link information
    LPARAM      lParam;                     // Free space for the caller to store GPO specific information
    struct _GROUP_POLICY_OBJECTW * pNext;   // Next GPO in the list
    struct _GROUP_POLICY_OBJECTW * pPrev;   // Previous GPO in the list
    LPWSTR      lpExtensions;               // Extensions that are relevant for this GPO
    LPARAM      lParam2;                    // Free space for the caller to store GPO specific information
    LPWSTR      lpLink;                     // Path to the Active Directory site, domain, or organizational unit this GPO is linked to
                                            // If this is the local GPO, this points to the word "Local"
} GROUP_POLICY_OBJECTW, *PGROUP_POLICY_OBJECTW;
#ifdef UNICODE
typedef GROUP_POLICY_OBJECTW GROUP_POLICY_OBJECT;
typedef PGROUP_POLICY_OBJECTW PGROUP_POLICY_OBJECT;
#else
typedef GROUP_POLICY_OBJECTA GROUP_POLICY_OBJECT;
typedef PGROUP_POLICY_OBJECTA PGROUP_POLICY_OBJECT;
#endif // UNICODE


//
// dwFlags for GetGPOList()
//

#define GPO_LIST_FLAG_MACHINE        0x00000001  // Return machine policy information
#define GPO_LIST_FLAG_SITEONLY       0x00000002  // Return site policy information only
#define GPO_LIST_FLAG_NO_WMIFILTERS  0x00000004  // Ignore WMI filters when filtering GPO's
#define GPO_LIST_FLAG_NO_SECURITYFILTERS 0x00000008 // Ignore security filters

USERENVAPI
BOOL
WINAPI
GetGPOListA (
    __in_opt    HANDLE      hToken,
    __in_opt    LPCSTR    lpName,
    __in_opt    LPCSTR    lpHostName,
    __in_opt    LPCSTR    lpComputerName,
    __in        DWORD       dwFlags,
    __deref_out PGROUP_POLICY_OBJECTA * pGPOList);
USERENVAPI
BOOL
WINAPI
GetGPOListW (
    __in_opt    HANDLE      hToken,
    __in_opt    LPCWSTR    lpName,
    __in_opt    LPCWSTR    lpHostName,
    __in_opt    LPCWSTR    lpComputerName,
    __in        DWORD       dwFlags,
    __deref_out PGROUP_POLICY_OBJECTW * pGPOList);
#ifdef UNICODE
#define GetGPOList  GetGPOListW
#else
#define GetGPOList  GetGPOListA
#endif // !UNICODE

#endif /* WINVER >= 0x0500 */

//=============================================================================
//
// FreeGPOList
//
//
// Frees the linked list returned from GetGPOList
//
// pGPOList - Pointer to the linked list of GPOs
//
//
// Returns:  TRUE if successful
//           FALSE if not
//
//=============================================================================

#if(WINVER >= 0x0500)

USERENVAPI
BOOL
WINAPI
FreeGPOListA (
    __in        PGROUP_POLICY_OBJECTA   pGPOList);
USERENVAPI
BOOL
WINAPI
FreeGPOListW (
    __in        PGROUP_POLICY_OBJECTW   pGPOList);
#ifdef UNICODE
#define FreeGPOList  FreeGPOListW
#else
#define FreeGPOList  FreeGPOListA
#endif // !UNICODE

#endif /* WINVER >= 0x0500 */

//=============================================================================
//
// GetAppliedGPOList
//
// Queries for the list of applied Group Policy Objects for the specified
// user or machine and specified client side extension. This function will return
// a linked list of Group Policy Objects.  Call FreeGPOList to free the list.
//
// dwFlags          - User or machine policy, if it is GPO_LIST_FLAG_MACHINE then
//                    return machine policy information
// pMachineName     - Name of remote computer in the form \\computername. If null
//                    then local computer is used.
// pSidUser         - Security id of user (relevant for user policy). If pMachineName is
//                    null and pSidUser is null then it means current logged on user.
//                    If pMachine is null and pSidUser is non-null then it means user
//                    represented by pSidUser on local machine. If pMachineName is non-null
//                    then and if dwFlags specifies user policy, then pSidUser must be
//                    non-null.
// pGuidExtension   - Guid of the specified extension
// ppGPOList        - Address of a pointer which receives the link list of GPOs
//
// The return value is a Win32 error code. ERROR_SUCCESS means the GetAppliedGPOList
// function completed successfully. Otherwise it indicates that the function failed.
//
//=============================================================================

#if(WINVER >= 0x0500)

USERENVAPI
DWORD
WINAPI
GetAppliedGPOListA (
    __in        DWORD       dwFlags,
    __in_opt    LPCSTR    pMachineName,
    __in_opt    PSID        pSidUser,
    __in        GUID        *pGuidExtension,
    __deref_out PGROUP_POLICY_OBJECTA   *ppGPOList);
USERENVAPI
DWORD
WINAPI
GetAppliedGPOListW (
    __in        DWORD       dwFlags,
    __in_opt    LPCWSTR    pMachineName,
    __in_opt    PSID        pSidUser,
    __in        GUID        *pGuidExtension,
    __deref_out PGROUP_POLICY_OBJECTW   *ppGPOList);
#ifdef UNICODE
#define GetAppliedGPOList  GetAppliedGPOListW
#else
#define GetAppliedGPOList  GetAppliedGPOListA
#endif // !UNICODE

#endif /* WINVER >= 0x0500 */

//=============================================================================
//
// Group Policy Object client side extension support
//
// Flags, data structures and function prototype
//
// To register your extension, create a subkey under this key
//
// Software\Microsoft\Windows NT\CurrentVersion\Winlogon\GPExtensions
//
// The subkey needs to be a guid so that it is unique. The noname value of the subkey
// can be the friendly name of the extension. Then add these values:
//
//     DllName                      REG_EXPAND_SZ  Path to your DLL
//     ProcessGroupPolicy           REG_SZ       Function name (see PFNPROCESSGROUPPOLICY prototype). This
//                                                 is obsolete, it has been superseded by ProcessGroupPolicyEx.
//                                                 It's here for backward compatibility reasons only.
//     ProcessGroupPolicyEx         REG_SZ       Function name (see PFNPROCESSGROUPPOLICYEX prototype)
//     GenerateGroupPolicy          REG_SZ       Function name for Rsop (see PFNGENERATEGROUPPOLICY prototype)
//     NoMachinePolicy              REG_DWORD    True, if extension does not have to be called when
//                                                 machine policies are being processed.
//     NoUserPolicy                 REG_DWORD    True, if extension does not have to be called when
//                                                 user policies are being processed.
//     NoSlowLink                   REG_DWORD    True, if extension does not have to be called on a slow link
//     NoBackgroundPolicy           REG_DWORD    True, if extension does not have to be called 
//                                                 for background policy processing.
//     NoGPOListChanges             REG_DWORD    True, if extension does not have to be called when
//                                                 there are no changes between cached and current GPO lists.
//     PerUserLocalSettings         REG_DWORD    True, if user policies have to be cached on a per user and
//                                                 per machine basis.
//     RequiresSuccessfulRegistry   REG_DWORD    True, if extension should be called only if registry extension
//                                                 was successfully processed.
//     EnableAsynchronousProcessing REG_DWORD    True, if registry extension will complete its processing
//                                                 asynchronously.
//     NotifyLinkTransition         REG_DWORD    True, if extension should be called when a change in link
//                                                 speed is detected between previous policy application and
//                                                 current policy application.
//
// The return value is a Win32 error code. ERROR_SUCCESS means the ProcessGroupPolicy
// function completed successfully. If return value is ERROR_OVERRIDE_NOCHANGES then it
// means that the extension will be called the next time even if NoGPOListChanges is set
// and there are no changes to the GPO list. Any other return value indicates that the
// ProcessGroupPolicy or ProcessGroupPolicyEx function failed.
//
//=============================================================================

#if(WINVER >= 0x0500)

#define GP_DLLNAME                         TEXT("DllName")
#define GP_ENABLEASYNCHRONOUSPROCESSING    TEXT("EnableAsynchronousProcessing")
#define GP_MAXNOGPOLISTCHANGESINTERVAL     TEXT("MaxNoGPOListChangesInterval")
#define GP_NOBACKGROUNDPOLICY              TEXT("NoBackgroundPolicy")
#define GP_NOGPOLISTCHANGES                TEXT("NoGPOListChanges")
#define GP_NOMACHINEPOLICY                 TEXT("NoMachinePolicy")
#define GP_NOSLOWLINK                      TEXT("NoSlowLink")
#define GP_NOTIFYLINKTRANSITION            TEXT("NotifyLinkTransition")
#define GP_NOUSERPOLICY                    TEXT("NoUserPolicy")
#define GP_PERUSERLOCALSETTINGS            TEXT("PerUserLocalSettings")
#define GP_PROCESSGROUPPOLICY              TEXT("ProcessGroupPolicy")
#define GP_REQUIRESSUCCESSFULREGISTRY      TEXT("RequiresSuccessfulRegistry")

#define GPO_INFO_FLAG_MACHINE              0x00000001  // Apply machine policy rather than user policy
#define GPO_INFO_FLAG_BACKGROUND           0x00000010  // Background refresh of policy (ok to do slow stuff)
#define GPO_INFO_FLAG_SLOWLINK             0x00000020  // Policy is being applied across a slow link
#define GPO_INFO_FLAG_VERBOSE              0x00000040  // Verbose output to the eventlog
#define GPO_INFO_FLAG_NOCHANGES            0x00000080  // No changes were detected to the Group Policy Objects
#define GPO_INFO_FLAG_LINKTRANSITION       0x00000100  // A change in link speed was detected between previous policy
                                                       // application and current policy application
#define GPO_INFO_FLAG_LOGRSOP_TRANSITION   0x00000200  // A Change in Rsop Logging was detected between previous policy
                                                       // application and current policy application, (new intf only)
#define GPO_INFO_FLAG_FORCED_REFRESH       0x00000400  // Forced Refresh is being applied. redo policies.
#define GPO_INFO_FLAG_SAFEMODE_BOOT        0x00000800  // windows safe mode boot flag
#define GPO_INFO_FLAG_ASYNC_FOREGROUND     0x00001000  // Asynchronous foreground refresh of policy

typedef UINT_PTR ASYNCCOMPLETIONHANDLE;
typedef DWORD (*PFNSTATUSMESSAGECALLBACK)(__in BOOL bVerbose, __in LPWSTR lpMessage);

typedef DWORD(*PFNPROCESSGROUPPOLICY)(
    __in DWORD dwFlags,                              // GPO_INFO_FLAGS
    __in HANDLE hToken,                              // User or machine token
    __in HKEY hKeyRoot,                              // Root of registry
    __in PGROUP_POLICY_OBJECT  pDeletedGPOList,      // Linked list of deleted GPOs
    __in PGROUP_POLICY_OBJECT  pChangedGPOList,      // Linked list of changed GPOs
    __in ASYNCCOMPLETIONHANDLE pHandle,              // For asynchronous completion
    __inout BOOL *pbAbort,                           // If true, then abort GPO processing
    __in_opt PFNSTATUSMESSAGECALLBACK pStatusCallback);  // Callback function for displaying status messages
                                                     // Note, this can be NULL

typedef DWORD(*PFNPROCESSGROUPPOLICYEX)(
    __in DWORD dwFlags,                              // GPO_INFO_FLAGS
    __in HANDLE hToken,                              // User or machine token
    __in HKEY hKeyRoot,                              // Root of registry
    __in PGROUP_POLICY_OBJECT  pDeletedGPOList,      // Linked list of deleted GPOs
    __in PGROUP_POLICY_OBJECT  pChangedGPOList,      // Linked list of changed GPOs
    __in ASYNCCOMPLETIONHANDLE pHandle,              // For asynchronous completion
    __inout BOOL *pbAbort,                           // If true, then abort GPO processing
    __in_opt PFNSTATUSMESSAGECALLBACK pStatusCallback,   // Callback function for displaying status messages
                                                     // Note, this can be NULL
    __in_opt IWbemServices *pWbemServices,           // Pointer to namespace to log diagnostic mode data
                                                     // Note, this will be NULL when Rsop logging is disabled
    __out HRESULT *pRsopStatus);                     // RSOP Logging succeeded or not.

typedef PVOID PRSOPTOKEN;

typedef struct _RSOP_TARGET {
    WCHAR *     pwszAccountName;                   // Account name
    WCHAR *     pwszNewSOM;                        // New domain or OU location for account
    SAFEARRAY * psaSecurityGroups;                 // New security groups
    PRSOPTOKEN  pRsopToken;                        // Rsop token for use with Rsop security Api's
    PGROUP_POLICY_OBJECT pGPOList;                 // Linked list of GPOs
    IWbemServices *      pWbemServices;            // Pointer to namespace to log planning mode data
} RSOP_TARGET, *PRSOP_TARGET;

typedef DWORD(*PFNGENERATEGROUPPOLICY)(
    __in DWORD dwFlags,                            // GPO_INFO_FLAGS
    __inout BOOL  *pbAbort,                        // If true, then abort GPO processing
    __in_opt WCHAR *pwszSite,                      // Site the target computer is in
    __in_opt PRSOP_TARGET pComputerTarget,         // Computer target info, can be null
    __in_opt PRSOP_TARGET pUserTarget );           // User target info, can be null

//
// GUID that identifies the registry extension
//

#define REGISTRY_EXTENSION_GUID  { 0x35378EAC, 0x683F, 0x11D2, 0xA8, 0x9A, 0x00, 0xC0, 0x4F, 0xBB, 0xCF, 0xA2 }

//
// UBPM trigger provider and event GUIDs
//
#define GROUP_POLICY_TRIGGER_EVENT_PROVIDER_GUID  { 0xBD2F4252, 0x5E1E, 0x49FC, 0x9A, 0x30, 0xF3, 0x97, 0x8A, 0xD8, 0x9E, 0xE2 }
#define MACHINE_POLICY_PRESENT_TRIGGER_GUID  { 0x659FCAE6, 0x5BDB, 0x4DA9, 0xB1, 0xFF, 0xCA, 0x2A, 0x17, 0x8D, 0x46, 0xE0 }
#define USER_POLICY_PRESENT_TRIGGER_GUID  { 0x54FB46C8, 0xF089, 0x464C, 0xB1, 0xFD, 0x59, 0xD1, 0xB6, 0x2C, 0x3B, 0x50 }

#endif /* WINVER >= 0x0500 */

//=============================================================================
//
// Group Policy Object client side asynchronous extension processing
//
// extensionId    - Unique guid identifying the extension
// pAsyncHandle   - Asynchronous completion handle that was passed to extension in
//                  ProcessGroupPolicy call
// dwStatus       - Completion status of asynchronous processing
//
// The return value is a Win32 error code. ERROR_SUCCESS means the ProcessGroupPolicyCompleted
// function completed successfully. Otherwise it indicates that the function failed.
//
//=============================================================================

#if(WINVER >= 0x0500)

typedef GUID *REFGPEXTENSIONID;

USERENVAPI
DWORD
WINAPI
ProcessGroupPolicyCompleted(
    __in REFGPEXTENSIONID extensionId,
    __in ASYNCCOMPLETIONHANDLE pAsyncHandle,
    __in DWORD dwStatus);

#endif /* WINVER >= 0x0500 */

//=============================================================================
//
// Group Policy Object client side asynchronous extension processing
//
// extensionId    - Unique guid identifying the extension
// pAsyncHandle   - Asynchronous completion handle that was passed to extension in
//                  ProcessGroupPolicy call
// dwStatus       - Completion status of asynchronous processing
// RsopStatus     - RSoP Logging status
//
// The return value is a Win32 error code. ERROR_SUCCESS means the ProcessGroupPolicyCompleted
// function completed successfully. Otherwise it indicates that the function failed.
//
//=============================================================================

#if(WINVER >= 0x0500)

USERENVAPI
DWORD
WINAPI
ProcessGroupPolicyCompletedEx(
    __in REFGPEXTENSIONID extensionId,
    __in ASYNCCOMPLETIONHANDLE pAsyncHandle,
    __in DWORD dwStatus,
    __in HRESULT RsopStatus);

#endif /* WINVER >= 0x0500 */

//=============================================================================
//
// Function:    RsopAccessCheckByType
//
// Description: Determines whether the security descriptor pointed to by pSecurityDescriptor
//                              grants the set of access rights specified in dwDesiredAccessMask
//                              to the client identified by the RSOPTOKEN pointed to by pRsopToken.
//
// pSecurityDescriptor  - Security Descriptor on the object
// pPrincipalSelfSid    - Principal Sid
// pRsopToken           - Pointer to a valid RSOPTOKEN against which access needs to be checked
// dwDesiredAccessMask  - Mask of requested generic and/or standard and or specific access rights
// pObjectTypeList      - Object Type List
// ObjectTypeListLength - Object Type List Length
// pGenericMapping      - Generic Mapping
// pPrivilegeSet        - privilege set
// pdwPrivilegeSetLength- privilege set length
// pdwGrantedAccessMask - On success, if pbAccessStatus is true, it contains
//                                         the mask of standard and specific rights granted.
//                                         If pbAccessStatus is false, it is set to 0.
//                                         On failure, it is not modified.
// pbAccessStatus       - On success, indicates wether the requested set
//                                    of access rights was granted.
//                                    On failure, it is not modified
//
// Returns S_OK on success or appropriate error code.
// For additional details, look at the documentation of AccessCheckByType
//=============================================================================

#if(WINVER >= 0x0500)

USERENVAPI
HRESULT 
WINAPI
RsopAccessCheckByType(  
    __in       PSECURITY_DESCRIPTOR pSecurityDescriptor,
    __in_opt   PSID pPrincipalSelfSid,
    __in       PRSOPTOKEN pRsopToken,
    __in       DWORD dwDesiredAccessMask,
    __in_ecount_opt(ObjectTypeListLength)   POBJECT_TYPE_LIST pObjectTypeList,
    __in       DWORD ObjectTypeListLength,
    __in       PGENERIC_MAPPING pGenericMapping,
    __in_bcount_opt(*pdwPrivilegeSetLength) PPRIVILEGE_SET pPrivilegeSet,
    __in_opt   LPDWORD pdwPrivilegeSetLength,
    __out LPDWORD pdwGrantedAccessMask,
    __out LPBOOL pbAccessStatus);

#endif /* WINVER >= 0x0500 */

//=============================================================================
//
// Function:    RsopFileAccessCheck
//
// Description: Determines whether the security descriptor on the file grants the set of file access 
//                              rights specified in dwDesiredAccessMask
//                              to the client identified by the RSOPTOKEN pointed to by pRsopToken.
//
// pszFileName          - Name of an existing filename
// pRsopToken           - Pointer to a valid RSOPTOKEN against which access needs to be checked
// dwDesiredAccessMask  - Mask of requested generic and/or standard and or specific access rights
// pdwGrantedAccessMask - On success, if pbAccessStatus is true, it contains
//                                         the mask of standard and specific rights granted.
//                                         If pbAccessStatus is false, it is set to 0.
//                                         On failure, it is not modified.
// pbAccessStatus       - On success, indicates wether the requested set
//                                    of access rights was granted.
//                                    On failure, it is not modified
//
// Returns S_OK on success or appropriate error code
//=============================================================================

#if(WINVER >= 0x0500)

USERENVAPI
HRESULT 
WINAPI
RsopFileAccessCheck(
    __in  LPWSTR pszFileName,
    __in  PRSOPTOKEN pRsopToken,
    __in  DWORD dwDesiredAccessMask,
    __out LPDWORD pdwGrantedAccessMask,
    __out LPBOOL pbAccessStatus);

#endif /* WINVER >= 0x0500 */

typedef enum _SETTINGSTATUS
{
        RSOPUnspecified = 0,
        RSOPApplied,
        RSOPIgnored,
        RSOPFailed,
        RSOPSubsettingFailed
} SETTINGSTATUS;

//=============================================================================
//
//  POLICYSETTINGSTATUSINFO
//
//  Describes the instance of RSOP_PolicySettingStatus
//
//  szKey               - OPTIONAL, if NULL, the key is generated on the fly
//  szEventSource       - name of the source generation event log messages
//  szEventLogName      - name of the event log database where the messages are logged
//  dwEventID           - event log message ID
//  status              - status of the policy setting
//  timeLogged          - time at which the event log message was logged
//
//=============================================================================

typedef struct _POLICYSETTINGSTATUSINFO
{
        LPWSTR                  szKey;
        LPWSTR                  szEventSource;
        LPWSTR                  szEventLogName;
        DWORD                   dwEventID;
        DWORD                   dwErrorCode;
        SETTINGSTATUS   status;
        SYSTEMTIME              timeLogged;
} POLICYSETTINGSTATUSINFO, *LPPOLICYSETTINGSTATUSINFO;

//=============================================================================
//
//  RsopSetPolicySettingStatus
//
//  Creates an instance of RSOP_PolicySettingStatus and RSOP_PolicySettingLink
//  and links RSOP_PolicySettingStatus to RSOP_PolicySetting
//
//  dwFlags             - flags
//  pServices           - RSOP namespace
//  pSettingInstance    - instance of RSOP_PolicySetting or its children
//  nInfo               - number of PolicySettingStatusInfo
//  pStatus             - array of PolicySettingStatusInfo
//
//  Return:     S_OK if successful, HRESULT otherwise
//
//=============================================================================

USERENVAPI
HRESULT
WINAPI
RsopSetPolicySettingStatus( __in DWORD                       dwFlags,
                            __in IWbemServices*              pServices,
                            __in IWbemClassObject*           pSettingInstance,
                            __in DWORD                       nInfo,
                            __in_ecount(nInfo) POLICYSETTINGSTATUSINFO*    pStatus );

//=============================================================================
//
//  RsopResetPolicySettingStatus
//
//  Unlinks RSOP_PolicySettingStatus from RSOP_PolicySetting,
//  deletes the instance of RSOP_PolicySettingStatus and RSOP_PolicySettingLink
//  and optionally deletes the instance of RSOP_PolicySetting
//
//  dwFlags             - flags
//  pServices           - RSOP namespace
//  pSettingInstance    - instance of RSOP_PolicySetting or its children
//
//  Return:     S_OK if successful, HRESULT otherwise
//
//=============================================================================

USERENVAPI
HRESULT
WINAPI
RsopResetPolicySettingStatus( __in DWORD               dwFlags,
                              __in IWbemServices*      pServices,
                              __in IWbemClassObject*   pSettingInstance );

//=============================================================================
//
// Flags for RSoP WMI providers
//
//=============================================================================

// planning mode provider flags
#define FLAG_NO_GPO_FILTER      0x80000000  // GPOs are not filtered, implies FLAG_NO_CSE_INVOKE
#define FLAG_NO_CSE_INVOKE      0x40000000  // only GP processing done for planning mode
#define FLAG_ASSUME_SLOW_LINK   0x20000000  // planning mode RSoP assumes slow link
#define FLAG_LOOPBACK_MERGE     0x10000000  // planning mode RSoP assumes merge loop back
#define FLAG_LOOPBACK_REPLACE   0x08000000  // planning mode RSoP assumes replace loop back

#define FLAG_ASSUME_USER_WQLFILTER_TRUE   0x04000000  // planning mode RSoP assumes all comp filters to be true
#define FLAG_ASSUME_COMP_WQLFILTER_TRUE   0x02000000  // planning mode RSoP assumes all user filters to be true

#define FLAG_PLANNING_MODE                0x01000000  // flag that indicates that a given namespace was created
                                                      // for planning mode. This flag cannot be passed in but the
                                                      // relevant subnamespace will be marked with this flag

// diagnostic mode provider flags
#define FLAG_NO_USER                      0x00000001  // Don't get any user data
#define FLAG_NO_COMPUTER                  0x00000002  // Don't get any machine data
#define FLAG_FORCE_CREATENAMESPACE        0x00000004  
                   // Delete and recreate the namespace for this snapshot.

//=============================================================================
//
// Extended Errors returned by RSoP WMI Providers
//
//=============================================================================

// User accessing the rsop provider doesn't have access to user data.
#define RSOP_USER_ACCESS_DENIED         0x00000001  

// User accessing the rsop provider doesn't have access to computer data.
#define RSOP_COMPUTER_ACCESS_DENIED     0x00000002  

// This user is an interactive non admin user, the temp snapshot namespace already exists
// and the FLAG_FORCE_CREATENAMESPACE was not passed in
#define RSOP_TEMPNAMESPACE_EXISTS        0x00000004



#ifdef __cplusplus
}
#endif


#endif // _INC_USERENV

