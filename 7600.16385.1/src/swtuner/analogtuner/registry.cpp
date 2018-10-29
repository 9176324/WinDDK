//==========================================================================;
//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
//  PURPOSE.
//
//  Copyright (c) Microsoft Corporation.  All Rights Reserved.
//
//==========================================================================;

#include "common.h"

NTSTATUS
PptRegGetDeviceParameterDword(
    IN     PDEVICE_OBJECT  Pdo,
    IN     __nullterminated PWSTR           ParameterName,
    IN OUT PULONG          ParameterValue
    )
/*++

Routine Description:

    retrieve a devnode registry parameter of type dword

Arguments:

    Pdo - ParPort PDO

    ParameterName - parameter name to look up

    ParameterValue - default parameter value

Return Value:

    Status - if RegKeyValue does not exist or other failure occurs,
               then default is returned via ParameterValue

--*/
{
    NTSTATUS                 status;
    HANDLE                   hKey;
    RTL_QUERY_REGISTRY_TABLE queryTable[2];
    ULONG                    defaultValue;
    PVOID                    context = NULL;

    PAGED_CODE();

    status = IoOpenDeviceRegistryKey(Pdo, PLUGPLAY_REGKEY_DEVICE, KEY_READ, &hKey);

    if(!NT_SUCCESS(status)) {
        return status;
    }

    defaultValue = *ParameterValue;

    RtlZeroMemory(queryTable, sizeof(queryTable));

    queryTable[0].Flags         = RTL_QUERY_REGISTRY_DIRECT;
    queryTable[0].Name          = ParameterName;
    queryTable[0].EntryContext  = ParameterValue;
    queryTable[0].DefaultType   = REG_DWORD;
    queryTable[0].DefaultData   = &defaultValue;
    queryTable[0].DefaultLength = sizeof(ULONG);
    
    status = RtlQueryRegistryValues(RTL_REGISTRY_HANDLE, (PCWSTR)hKey, queryTable, &context, NULL);

    if ( !NT_SUCCESS(status) ) {
        *ParameterValue = defaultValue;
    }

    ZwClose(hKey);

    return status;
}

NTSTATUS
PptRegSetDeviceParameterDword(
    IN PDEVICE_OBJECT  Pdo,
    IN __nullterminated PWSTR           ParameterName,
    IN PULONG          ParameterValue
    )
/*++

Routine Description:

    Create/set a devnode registry parameter of type dword

Arguments:

    Pdo - ParPort PDO

    ParameterName - parameter name

    ParameterValue - parameter value

Return Value:

    Status - status from attempt

--*/
{
    NTSTATUS        status;
    HANDLE          hKey;
    UNICODE_STRING  valueName;
//    PPDO_EXTENSION  pdx = Pdo->DeviceExtension;

    PAGED_CODE();

    status = IoOpenDeviceRegistryKey(Pdo, PLUGPLAY_REGKEY_DEVICE, KEY_WRITE, &hKey);

    if( !NT_SUCCESS( status ) ) {
        DbgPrint("PptRegSetDeviceParameterDword - openKey FAILED w/status=%x",status);
        return status;
    }

    RtlInitUnicodeString( &valueName, ParameterName );

    status = ZwSetValueKey( hKey, &valueName, 0, REG_DWORD, ParameterValue, sizeof(ULONG) );
    if( !NT_SUCCESS( status ) ) {
       DbgPrint("PptRegSetDeviceParameterDword - setValue FAILED w/status=%x",status);
    }

    ZwClose(hKey);

    return status;
}

NTSTATUS
PptRegGetDeviceParameterSZ(
    IN     PDEVICE_OBJECT  Pdo,
    IN     __nullterminated PWSTR           ParameterName,
    IN OUT PUNICODE_STRING          ParameterValue
    )
/*++

Routine Description:

    retrieve a devnode registry parameter of type REG_SZ

Arguments:

    Pdo - ParPort PDO

    ParameterName - parameter name to look up

    ParameterValue - default parameter value

Return Value:

    Status - if RegKeyValue does not exist or other failure occurs,
               then default is returned via ParameterValue

--*/
{
    NTSTATUS                 status;
    HANDLE                   hKey;
    RTL_QUERY_REGISTRY_TABLE queryTable[2];
    PVOID                    context = NULL;

    DbgPrint("PptRegGetDeviceParameterSZ -- ParameterName is %S\n",ParameterName);

    PAGED_CODE();

    status = IoOpenDeviceRegistryKey(Pdo, PLUGPLAY_REGKEY_DRIVER, KEY_READ|KEY_WRITE, &hKey);

    if(!NT_SUCCESS(status)) {
	 DbgPrint("PptRegGetDeviceParameterSZ - IoOpenDeviceRegistryKey : query value FAILED w/status=%x",status);
        return status;
    }

    RtlZeroMemory(queryTable, sizeof(queryTable));

    queryTable[0].Flags         = RTL_QUERY_REGISTRY_DIRECT;
    queryTable[0].Name          = ParameterName;
    queryTable[0].EntryContext  = ParameterValue;
    queryTable[0].DefaultType   = REG_SZ;
    queryTable[0].DefaultData   = L"";
    queryTable[0].DefaultLength = 0;

    status = RtlQueryRegistryValues(RTL_REGISTRY_HANDLE, (PCWSTR)hKey, queryTable, &context, NULL);

    if ( !NT_SUCCESS(status) ) {
		ParameterValue->Length        = 0;
        ParameterValue->MaximumLength = 0;
        ParameterValue->Buffer        = 0;
	DbgPrint("PptRegGetDeviceParameterSZ - RtlQueryRegistryValues : query value FAILED w/status=%x",status);
    }

    if(0 == ParameterValue->Buffer || 0 == ParameterValue->Buffer[0])
    {
	DbgPrint("PptRegGetDeviceParameterSZ - RtlQueryRegistryValues : query result -- a null device id!\n");    
    }
    ZwClose(hKey);

    return status;
}


/************************************************************************/
/* PptRegGetDword                                                       */
/************************************************************************/
//
// Routine Description:
//
//     Read a REG_DWORD from the registry. This is a wrapper for
//       function RtlQueryRegistryValues.
//
// Arguments: 
//
//     RelativeTo     - starting point for the Path
//     Path           - path to the registry key
//     ParameterName  - name of the value to be read
//     ParameterValue - used to return the DWORD value read from the registry
//
// Return Value:                                          
//                                                            
//     NTSTATUS
//                                                        
// Notes:
//
//     - On an ERROR or if the requested registry value does not exist, 
//         *ParameterValue retains its original value.
//
// Log:
//
/************************************************************************/
NTSTATUS
PptRegGetDword(
    IN     ULONG  RelativeTo,               
    IN     __nullterminated PWSTR  Path,
    IN     __nullterminated PWSTR  ParameterName,
    IN OUT PULONG ParameterValue
    )
{
    NTSTATUS                  status;
    RTL_QUERY_REGISTRY_TABLE  paramTable[2];
    ULONG                     defaultValue;
    PVOID                     context = NULL;

    if( ( NULL == Path ) || ( NULL == ParameterName ) || ( NULL == ParameterValue ) ) {
        return STATUS_INVALID_PARAMETER;
    }

    //DbgPrint("PptRegGetDword - RelativeTo= %x, Path=<%S>, ParameterName=<%S>\n", RelativeTo, Path, ParameterName);

    //
    // set up table entries for call to RtlQueryRegistryValues
    //
    // leave paramTable[1] as all zeros to terminate the table
    //
    // use original value as default value
    //
    // use RtlQueryRegistryValues to do the grunge work
    //
    RtlZeroMemory( paramTable, sizeof(paramTable) );

    defaultValue = *ParameterValue;

    paramTable[0].Flags         = RTL_QUERY_REGISTRY_DIRECT;
    paramTable[0].Name          = ParameterName;
    paramTable[0].EntryContext  = ParameterValue;
    paramTable[0].DefaultType   = REG_DWORD;
    paramTable[0].DefaultData   = &defaultValue;
    paramTable[0].DefaultLength = sizeof(ULONG);

    status = RtlQueryRegistryValues( RelativeTo | RTL_REGISTRY_OPTIONAL,
                                     Path,
                                     &paramTable[0],
                                     &context,
                                     NULL);
       
    if( status != STATUS_SUCCESS ) {
        DbgPrint("PptRegGetDword - RtlQueryRegistryValues FAILED w/status=%x\n",status);
    }

    //DbgPrint("PptRegGetDword - post-query <%S> *ParameterValue = %x\n", ParameterName, *ParameterValue);

    return status;
}


/************************************************************************/
/* PptRegGetSz                                                          */
/************************************************************************/
//
// Routine Description:
//
//     Read a REG_SZ from the registry. This is a wrapper for
//       function RtlQueryRegistryValues.
//
// Arguments: 
//
//     RelativeTo     - starting point for the Path
//     Path           - path to the registry key
//     ParameterName  - name of the value to be read
//     ParameterValue - points to a UNICODE_STRING structure used to return 
//                        the REG_SZ read from the registry
//
// Return Value:                                          
//                                                            
//     NTSTATUS
//                                                        
//
// Notes:
//
//     - All fields of *ParameterValue UNICODE_STRING structure must be 
//         initialized to zero by the caller.
//     - On SUCCESS ParameterValue->Buffer points to an allocated buffer. The
//         caller is responsible for freeing this buffer when done.
//     - On SUCCESS ParameterValue->Buffer is UNICODE_NULL terminated and is
//         safe to use as a PWSTR.
//
// Log:
//
/************************************************************************/
NTSTATUS
PptRegGetSz(
    IN      ULONG  RelativeTo,               
    IN      __nullterminated PWSTR  Path,
    IN      __nullterminated PWSTR  ParameterName,
    IN OUT  PUNICODE_STRING ParameterValue
    )
{
    NTSTATUS                  status;
    RTL_QUERY_REGISTRY_TABLE  paramTable[2];
    PVOID                     context = NULL;

    //
    // sanity check parameters - reject NULL pointers and invalid
    //   UNICODE_STRING field initializations
    //
    if( ( NULL == Path ) || ( NULL == ParameterName ) || ( NULL == ParameterValue ) ) {
        return STATUS_INVALID_PARAMETER;
    }
    if( (ParameterValue->Length != 0) || (ParameterValue->MaximumLength !=0) || (ParameterValue->Buffer != NULL) ) {
        return STATUS_INVALID_PARAMETER;
    }

    //DbgPrint("PptRegGetSz - RelativeTo=%x, Path=<%S>, ParameterName=<%S>\n", RelativeTo, Path, ParameterName);

    //
    // set up table entries for call to RtlQueryRegistryValues
    //
    // leave paramTable[1] as all zeros to terminate the table
    //
    // use RtlQueryRegistryValues to do the grunge work
    //
    RtlZeroMemory( paramTable, sizeof(paramTable) );

    paramTable[0].Flags         = RTL_QUERY_REGISTRY_DIRECT;
    paramTable[0].Name          = ParameterName;
    paramTable[0].EntryContext  = ParameterValue;
    paramTable[0].DefaultType   = REG_SZ;
    paramTable[0].DefaultData   = L"";
    paramTable[0].DefaultLength = 0;

    status = RtlQueryRegistryValues( RelativeTo | RTL_REGISTRY_OPTIONAL,
                                     Path,
                                     &paramTable[0],
                                     &context,
                                     NULL);
       
    if( status != STATUS_SUCCESS ) {
        DbgPrint("PptRegGetSz - RtlQueryRegistryValues FAILED status=%x\n",status);
        DbgPrint("PptRegGetSz - RtlQueryRegistryValues  path %S,name is %S\n",Path,ParameterName);
    }

    //
    // Try to make ParameterValue->Buffer safe to use as a PWSTR parameter. 
    //   Clean up the allocation and fail this request if we are unable to do so.
    //
    if( (STATUS_SUCCESS == status) && (ParameterValue->Buffer != NULL) ) {

        if( ParameterValue->MaximumLength >= (ParameterValue->Length + sizeof(WCHAR)) ) {

            (ParameterValue->Buffer)[ ParameterValue->Length / sizeof(WCHAR) ] = UNICODE_NULL;
            //DbgPrint("PptRegGetSz - post-query *ParameterValue=<%S>\n", ParameterValue->Buffer);

        } else {

            ExFreePool( ParameterValue->Buffer );
            ParameterValue->Length        = 0;
            ParameterValue->MaximumLength = 0;
            ParameterValue->Buffer        = 0;
            status = STATUS_UNSUCCESSFUL;

        }
    }

    return status;
}


