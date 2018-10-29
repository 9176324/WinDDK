/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name:

    Driver.h

Environment:

    Kernel mode

--*/

//
// Driverspecs.h includes the definitions for PREFast for Drivers (PFD) annotations.  
//
#include <driverspecs.h>
#include <wdm.h>

//
// Driver Function Declarations:
// 
// This example driver declares six functions, each designed to do different jobs in the driver.
// PFD will suggest a preferred method of declaring these functions, using the __drv_dispatchType annotation,
// combined with the Role Type declarations discussed here: http://msdn.microsoft.com/en-us/library/cc264108.aspx
//

//
// Function: DriverEntry
// No Role Type declaration, this will result in a PFD informational message stating it believes this to be the DRIVER_INITIALIZE function.
// Delcaring with the Role Type would be as follows: DRIVER_INITIALIZE DriverEntry;
//
NTSTATUS
DriverEntry(__in PDRIVER_OBJECT  DriverObject,
    __in PUNICODE_STRING RegistryPath
    );

//
// Function: DriverAddDevice;
// This function is declared using the Role Type.  The Role Type declaration for DriverAddDevice declares that it should clear the DO_DEVICE_INITIALIZING bit.
// Failure to clear that bit in the DRIVER_ADD_DEVICE defined function will result in PFD Warning 28152
//
DRIVER_ADD_DEVICE DriverAddDevice;

//
// Function: DriverUnload
// This is declared using the Role Type.
//
DRIVER_UNLOAD DriverUnload;

//
// Function: DispatchPnp
// This declaration does not use either the __drv_dispatchType annotation or the Role Type Declaration.
// Because neither of those annotations are used, PFD will issue Warnings 28155 and 28168
// A fully annotated declaration would be as follows: __drv_dispatchType(IRP_MJ_PNP) DRIVER_DISPATCH DispatchPnp;
//
NTSTATUS
DispatchPnp (
    __in PDEVICE_OBJECT DeviceObject,
    __in PIRP           Irp
    );

//
// Function: DispatchPower
// This declaration utilizes the Role Type declaration, but does not include the __drv_dispatchType annotation.
// This will cause PFD to issue Warning 28168
// A fully annotated declaration would be as follows: __drv_dispatchType(IRP_MJ_POWER) DRIVER_DISPATCH DispatchPower;
//
DRIVER_DISPATCH DispatchPower;

//
// Function: DispatchSystemControl
// This is an example of a fully annotated declaration.  
// IRP_MJ_SYSTEM_CONTROL is the type of IRP handled by this function.  
// Multiple __drv_dispatchType lines are acceptable if the function handles more than 1 IRP type.
//
__drv_dispatchType(IRP_MJ_SYSTEM_CONTROL) 
DRIVER_DISPATCH DispatchSystemControl;
    