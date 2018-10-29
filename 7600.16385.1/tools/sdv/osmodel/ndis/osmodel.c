/*
	Copyright (c) Microsoft Corporation.  All rights reserved.
*/

/* ..\wdm\harness-copyright.c begin */
/*
	Copyright (c) Microsoft Corporation.  All rights reserved.
*/

/*

Module Name:
    
    SDV harness


Abstract:
    
    Functions as representation of Windows OS for SDV.  It boots the 
    device driver and puts it into various states via its entry 
    DriverEntry routine and dispatch routines, etc.


Environment:

    Symbolic execution in Static Driver Verifier.

*/
/* ..\wdm\harness-copyright.c end */

/* ..\wdm\malloc.c begin */
/*
	Copyright (c) Microsoft Corporation.  All rights reserved.
*/

char * malloc(int);
/* ..\wdm\malloc.c end */

/* ..\wdm\harness-parts.c begin */
/*
	Copyright (c) Microsoft Corporation.  All rights reserved.
*/

#include <ntifs.h>

#include <ntdddisk.h>

#include "sdv-map.h"
#include "dispatch_routines.h"
#include "sdv_stubs.h"


#define SDV_IS_FLAT_HARNESS() \
    ( \
        SDV_HARNESS==SDV_FLAT_DISPATCH_HARNESS || \
        SDV_HARNESS==SDV_FLAT_DISPATCH_STARTIO_HARNESS || \
        SDV_HARNESS==SDV_FLAT_SIMPLE_HARNESS || \
        SDV_HARNESS==SDV_FLAT_HARNESS \
    )

#define SDV_MACRO_COPYCURRENTIRPSTACKLOCATIONTONEXT(arg1)\
    if (arg1 == &sdv_harnessIrp) {\
        sdv_harnessStackLocation_next.MinorFunction =\
            sdv_harnessStackLocation.MinorFunction;\
        sdv_harnessStackLocation_next.MajorFunction =\
            sdv_harnessStackLocation.MajorFunction;\
    }\
    if (arg1 == &sdv_other_harnessIrp) {\
        sdv_other_harnessStackLocation_next.MinorFunction =\
            sdv_other_harnessStackLocation.MinorFunction;\
        sdv_other_harnessStackLocation_next.MajorFunction =\
            sdv_other_harnessStackLocation.MajorFunction;\
    }\

#define SDV_MACRO_IOGETCURRENTIRPSTACKLOCATION(arg1)\
(arg1->Tail.Overlay.CurrentStackLocation)\

#define SDV_MACRO_STUB_CANCEL_BEGIN(arg1)\
IoAcquireCancelSpinLock(&(arg1->CancelIrql))\

#define SDV_MACRO_STUB_CANCEL_END(arg1)\
arg1->CancelRoutine=NULL\


VOID 
sdv_SetIrpMinorFunctionNonBusDriver(
    PIRP pirp
    );

VOID
sdv_SetMajorFunction(
    PIRP pirp,
    UCHAR fun
    );

VOID
sdv_SetPowerIrpMinorFunction(
    PIRP pirp
    );

VOID
sdv_SetStatus(
    PIRP pirp
    );


NTSTATUS
sdv_DoNothing(
    );

NTSTATUS         
sdv_RunDispatchFunction(
    PDEVICE_OBJECT po, 
    PIRP pirp
    );

VOID         
sdv_RunCancelFunction(
    PDEVICE_OBJECT po, 
    PIRP pirp
    );
    
NTSTATUS         
sdv_RunQueryDeviceRelations(
    PDEVICE_OBJECT po, 
    PIRP pirp
    );

PIRP             
sdv_MakeAbstractIrp(PIRP pirp
    );


PIRP             
sdv_MakeStartDeviceIrp(PIRP pirp
    );


PIRP             
sdv_MakeRemoveDeviceIrp(PIRP pirp
    );

LONG
SdvMakeChoice();

LONG
SdvKeepChoice();

/*POWER_STATE
sdv_Make_POWER_STATE();*/

VOID
sdv_RunStartIo(
    PDEVICE_OBJECT po, 
    PIRP pirp
    );

VOID
sdv_RunIoCompletionRoutines(
    __in PDEVICE_OBJECT DeviceObject, 
    __in PIRP Irp, 
    __in_opt PVOID Context
    );

VOID
sdv_RunISRRoutines(
    struct _KINTERRUPT *ki, 
    PVOID pv1
    );

VOID
sdv_RunKeDpcRoutines(
    struct _KDPC *kdpc, 
    PVOID pDpcContext, 
    PVOID pv2, 
    PVOID pv3
    );
    
VOID
sdv_RunIoDpcRoutines(
    IN PKDPC  Dpc,    
    IN struct _DEVICE_OBJECT  *DeviceObject,    
    IN struct _IRP  *Irp,    
    IN PVOID  Context
    );

VOID
sdv_RunUnload(
    PDRIVER_OBJECT pdrivo
    );

NTSTATUS sdv_RunAddDevice(
    PDRIVER_OBJECT p1,
    PDEVICE_OBJECT p2
    );


BOOLEAN 
sdv_CheckDispatchRoutines(
    );

BOOLEAN 
sdv_CheckStartIoRoutines(
    );

BOOLEAN 
sdv_CheckDpcRoutines(
    );

BOOLEAN 
sdv_CheckIsrRoutines(
    );

BOOLEAN 
sdv_CheckCancelRoutines(
    );

BOOLEAN 
sdv_CheckCancelRoutines1(
    );


BOOLEAN 
sdv_CheckIoDpcRoutines(
    );

BOOLEAN 
sdv_IoCompletionRoutines(
    );

BOOLEAN 
sdv_CheckWorkerRoutines(
    );

BOOLEAN 
sdv_CheckAddDevice(
    );

BOOLEAN 
sdv_CheckIrpMjPnp(
    );

BOOLEAN 
sdv_CheckDriverUnload(
    );



VOID SdvExit();

VOID SdvAssume(int e);

VOID SdvAssumeSoft(int e);





int sdv_start_irp_already_issued = 0;
int sdv_remove_irp_already_issued = 0;


int sdv_compFset = 0;

BOOLEAN
sdv_isr_dummy (
    IN struct _KINTERRUPT * Interrupt,
    IN PVOID ServiceContext
    )
{
    return FALSE;
}

PKSERVICE_ROUTINE  sdv_isr_routine = sdv_isr_dummy;




VOID
sdv_dpc_dummy (
    IN struct _KDPC * Dpc,
    IN PVOID DeferredContext,
    IN PVOID SystemArgument1,
    IN PVOID SystemArgument2
    )
{
}

PKDEFERRED_ROUTINE  sdv_ke_dpc=sdv_dpc_dummy;

BOOLEAN sdv_dpc_ke_registered=FALSE;



VOID
sdv_io_dpc_dummy (
    IN PKDPC  Dpc,    
    IN struct _DEVICE_OBJECT  *DeviceObject,    
    IN struct _IRP  *Irp,    
    IN PVOID  Context
    )
{
}

PIO_DPC_ROUTINE  sdv_io_dpc=sdv_io_dpc_dummy;
BOOLEAN sdv_dpc_io_registered = FALSE;


IRP sdv_harnessIrp;
PIRP sdv_irp = &sdv_harnessIrp; 
IO_STACK_LOCATION sdv_harnessStackLocation;
IO_STACK_LOCATION sdv_harnessStackLocation_next;

IRP sdv_other_harnessIrp;
PIRP sdv_other_irp = &sdv_other_harnessIrp;
IO_STACK_LOCATION sdv_other_harnessStackLocation;
IO_STACK_LOCATION sdv_other_harnessStackLocation_next;

int sdv_harnessDeviceExtension_val;
int sdv_harnessDeviceExtension_two_val;

PVOID sdv_harnessDeviceExtension = (PVOID) &sdv_harnessDeviceExtension_val;
PVOID sdv_harnessDeviceExtension_two = (PVOID) &sdv_harnessDeviceExtension_two_val;



int sdv_io_create_device_called = 0;

int sdv_context_val ;
PVOID sdv_context = (PVOID) &sdv_context_val;
int sdv_start_info_val ;
ULONG_PTR sdv_start_info = (ULONG_PTR) &sdv_start_info_val;
int sdv_end_info_val ;
ULONG_PTR sdv_end_info = (ULONG_PTR) &sdv_end_info_val;


/* SDV_DIRQL:

   The OS model raises IRQL to DIRQL by the use of the SDV_DIRQL
   macro.  DIRQL is not a specific IRQL, but rather a range of
   possible Device IRQLs (On X86 from 3-26, on AMD64 from 3-11 and on
   IA64 from 4-11).  The OS Model nevertheless declares SDV_DIRQL as
   the concrete value 4 (which is a valid DIRQL representative for all
   architectures).  See ntddk_slic.h for details why this
   concretization is proper and will not result in loss of any traces.
*/

#define SDV_DIRQL 4


/* SDV OS Model IRQL Stack:

The OS Model contains a Bounded IRQL Stack.

The Bounded IRQL Stack should only be changed in OS Model DDIs and
only using 1) the operation SDV_IRQL_PUSH for pushing onto the stack
and 2) SDV_IRQL_POP and SDV_IRQL_POPTO for popping from the stack.

The stack is used in a number of rules that specify proper stack
behaviour for pairs of DDIs such as for example KeAcquireSpinLock and
KeReleaseSpinLock.

Pushing is used in the OS Model when a DDI sets/increases the IRQL to
a new and weakly higher IRQL.  An example of such a DDI is
KeAcquireSpinLock.

Popping is used in the OS Model when a DDI restores/lowers the IRQL to
a new and weakly lower IRQL.  An example of such a DDI is
KeReleaseSpinLock.

The stack is represented by the global variables sdv_irql_current,
sdv_irql_previous, sdv_irql_previous_2 and sdv_irql_previous_3.
sdv_irql_current is considered the top of the stack, sdv_irql_previous
is the second element of the stack and so on.

The stack is bounded.  Currently to height three.  This means that the
OS Model will not correctly capture the meaning of more than three
pushes to the stack.  Certain rules that check proper stack behaviour
takes this into account by counting current depth of the stack.

*/

/* Global IRQL stack, 4 levels high: */
KIRQL sdv_irql_current = PASSIVE_LEVEL;
KIRQL sdv_irql_previous = PASSIVE_LEVEL;
KIRQL sdv_irql_previous_2 = PASSIVE_LEVEL;
KIRQL sdv_irql_previous_3 = PASSIVE_LEVEL;

/* Operations on the global IRQL stack: */

/*
   SDV_IRQL_PUSH(new_irql):
     Change to new_irql IRQL, pushing old levels on the stack
*/

#define SDV_IRQL_PUSH(new_irql) \
    sdv_irql_previous_3 = sdv_irql_previous_2; \
    sdv_irql_previous_2 = sdv_irql_previous; \
    sdv_irql_previous = sdv_irql_current; \
    sdv_irql_current = new_irql

/*
   SDV_IRQL_POP():
     Change to previous IRQL, popping it from the stack
*/

#define SDV_IRQL_POP() \
    sdv_irql_current = sdv_irql_previous; \
    sdv_irql_previous = sdv_irql_previous_2; \
    sdv_irql_previous_2 = sdv_irql_previous_3

/*
   SDV_IRQL_POPTO(new_irql):
     Change to new_irql IRQL,
     popping (and ignoring) an IRQL from the stack
*/

#define SDV_IRQL_POPTO(new_irql) \
    sdv_irql_current = new_irql; \
    sdv_irql_previous = sdv_irql_previous_2; \
    sdv_irql_previous_2 = sdv_irql_previous_3



struct _KDPC sdv_kdpc_val;
struct _KDPC * sdv_kdpc = &sdv_kdpc_val;

PKDPC sdv_pkdpc=(PKDPC) &sdv_kdpc;

struct _KDPC sdv_kdpc_val3;
struct _KDPC * sdv_kdpc3 = &sdv_kdpc_val3;


int sdv_DpcContext;
PVOID sdv_pDpcContext = &sdv_DpcContext;

int sdv_IoDpcContext;
PVOID sdv_pIoDpcContext = &sdv_IoDpcContext;

int sdv_IoCompletionContext;
PVOID sdv_pIoCompletionContext = &sdv_IoCompletionContext;


PVOID sdv_pv1;
PVOID sdv_pv2;
PVOID sdv_pv3;

DRIVER_OBJECT sdv_driver_object;


DEVICE_OBJECT sdv_devobj_two;
PDEVICE_OBJECT sdv_p_devobj_two = &sdv_devobj_two;



DEVICE_OBJECT sdv_devobj_one;
PDEVICE_OBJECT sdv_p_devobj_one = &sdv_devobj_one;

int sdv_kinterrupt_val;
struct _KINTERRUPT *sdv_kinterrupt = (struct _KINTERRUPT *) &sdv_kinterrupt_val;



#define SDV_MAIN_INIT()\
    sdv_devobj_one.DeviceExtension = &sdv_harnessDeviceExtension;\
    sdv_devobj_two.DeviceExtension = &sdv_harnessDeviceExtension_two;\
    sdv_irp->Tail.Overlay.CurrentStackLocation = &sdv_harnessStackLocation;\
    sdv_other_irp->Tail.Overlay.CurrentStackLocation = &sdv_other_harnessStackLocation


VOID 
sdv_SetPowerIrpMinorFunction(
    PIRP pirp
    )  
/*

Routine Description:

    Sets the MN IRP fields to the possible values.  

Arguments:

    pirp - The IRP to set.

Notes:
    Note how we're using non-determinism here with "x" and "y".  "x", for
    example, could take on any value.

*/
{
    PIO_STACK_LOCATION r = SDV_MACRO_IOGETCURRENTIRPSTACKLOCATION(pirp);
    LONG x = SdvMakeChoice();
    LONG y = SdvMakeChoice();

    switch (x) {
        case 0:
        r->MinorFunction = IRP_MN_SET_POWER;
        switch (y) {
            case 0:
            r->Parameters.Power.Type = SystemPowerState;
            break;

            case 1:
            default:
            r->Parameters.Power.Type = DevicePowerState;
            break;
        }
              break;

        case 1:
        r->MinorFunction = IRP_MN_QUERY_POWER;
        switch (y) {

            case 0:
            r->Parameters.Power.Type = SystemPowerState;
            break;

            case 1:
            default:
            r->Parameters.Power.Type = DevicePowerState;
            break;
        }
        break;


        case 2:
        r->MinorFunction = IRP_MN_POWER_SEQUENCE;
        break;      


        case 3:
        default:
        r->MinorFunction = IRP_MN_WAIT_WAKE;
        break;
        }
}




PIRP 
sdv_MakeRemoveDeviceIrp(PIRP pirp
    )
{
    PIO_STACK_LOCATION ps = SDV_MACRO_IOGETCURRENTIRPSTACKLOCATION(pirp);

    ps->MajorFunction = IRP_MJ_PNP;
    ps->MinorFunction = IRP_MN_REMOVE_DEVICE;
    pirp->CancelRoutine = NULL;
    
    sdv_SetStatus(pirp);

    return pirp;
}




PIRP 
sdv_MakeStartDeviceIrp(PIRP pirp
    )
{
    PIO_STACK_LOCATION ps = SDV_MACRO_IOGETCURRENTIRPSTACKLOCATION(pirp);

    ps->MajorFunction = IRP_MJ_PNP;
    ps->MinorFunction = IRP_MN_START_DEVICE;
    pirp->CancelRoutine = NULL;
    
    sdv_SetStatus(pirp);

    return pirp;
}



VOID
sdv_SetMajorFunction(
    PIRP pirp,
    UCHAR fun
    )
{

    PIO_STACK_LOCATION ps = SDV_MACRO_IOGETCURRENTIRPSTACKLOCATION(pirp);
    ps->MajorFunction = fun;   

}




VOID 
sdv_SetStatus(
    PIRP pirp
    )
{
    LONG choice = SdvMakeChoice();

    switch(choice) {
        case 0:
            pirp->IoStatus.Status = STATUS_NOT_SUPPORTED;
            break;
        case 1:
        default:
            pirp->IoStatus.Status = STATUS_SUCCESS;
            break;
    }
}


        

NTSTATUS 
sdv_DoNothing(
    )
{
    return STATUS_UNSUCCESSFUL;
}





LONG 
SdvMakeChoice(
    )
/*

Routine Description:

    Non-deterministically chooses a value and returns it:
    to full cl we return 0, but then assignment x = SdvKeepChoice
    is eliminated - so that x remains uninitialized

Arguments:

Notes:
    Note how we're using non-determinism here:  "x" can be any value.

    If you wanted to take this harness and actually execute it, you need
    to implement the non-determinism.  Changing this function would be a
    start, but you would also need to change the places where IRPs and other
    types are non-deterministically choosen.

*/
{
    return 0;
}

LONG 
SdvKeepChoice(
    )
/*

Routine Description:

    Non-deterministically chooses a value and returns it:
    to full cl we return 0, but then assignment x = SdvMakeChoice
    is eliminated - so that x remains uninitialized

Arguments:

Notes:
    Note how we're using non-determinism here:  "x" can be any value.

    If you wanted to take this harness and actually execute it, you need
    to implement the non-determinism.  Changing this function would be a
    start, but you would also need to change the places where IRPs and other
    types are non-deterministically choosen.

*/
{
    return 0;
}

/*POWER_STATE
sdv_Make_POWER_STATE(
    )
{
  POWER_STATE x;
  int y = SdvMakeChoice();
  switch(y) {
    case 1 : x.SystemState = SdvMakeChoice(); break;
    default: x.DeviceState = SdvMakeChoice(); break;
  }
  return x;
}*/


NTSTATUS 
sdv_RunDispatchFunction(
    PDEVICE_OBJECT po, 
    PIRP pirp
    )
/*

Routine Description:

    Finds the appropriate dispatch function, and then applies it to the
    IRP argument.

Arguments:

    po - pointer to the device object.    
    pirp - pointer to the irp that we're using.

Return value:

    we're passing the dispatch's return value back.

Notes:
    
    We're interacting with SDV meta-data here.  If "fun_IRP_MJ_CREATE" is
    defined in the meta-data, then we're calling it.  Otherwise: we call
    the sdv_DoNothing() function.

*/
{
    NTSTATUS status;
    int x = SdvMakeChoice();
    UCHAR minor_function = (UCHAR) SdvKeepChoice();

    PIO_STACK_LOCATION ps = SDV_MACRO_IOGETCURRENTIRPSTACKLOCATION(pirp);


    sdv_end_info = sdv_start_info = pirp->IoStatus.Information;
 
    sdv_SetStatus(pirp);

    pirp->CancelRoutine = NULL;
    


    ps->MinorFunction = minor_function;
    ps->CompletionRoutine = NULL;
    
    sdv_dpc_io_registered = FALSE;
    
    sdv_stub_dispatch_begin();

    switch (x) { 

#ifndef SDV_FLAT_HARNESS_MODIFIER_NO_IRP_MJ_CLEANUP
        case IRP_MJ_CLEANUP:
        ps->MajorFunction = IRP_MJ_CLEANUP;
#ifdef fun_IRP_MJ_CLEANUP
        status = fun_IRP_MJ_CLEANUP(po,pirp);
#ifdef SDV_FLAT_HARNESS_CANCEL_ROUTINE
        sdv_RunCancelFunction(po,pirp);
#endif
        #if (SDV_HARNESS==SDV_PNP_HARNESS)
            sdv_RunISRRoutines(sdv_kinterrupt,sdv_pDpcContext);
            sdv_RunKeDpcRoutines(sdv_kdpc3,sdv_pDpcContext,sdv_pv1,sdv_pv2);
            sdv_RunIoDpcRoutines(sdv_kdpc,po,pirp,sdv_pIoDpcContext);
        #endif
#else
        status = sdv_DoNothing();
#endif
        break;
#endif

#ifndef SDV_FLAT_HARNESS_MODIFIER_NO_IRP_MJ_CLOSE
        case IRP_MJ_CLOSE:
        ps->MajorFunction = IRP_MJ_CLOSE;
#ifdef fun_IRP_MJ_CLOSE
        status = fun_IRP_MJ_CLOSE(po,pirp);
#ifdef SDV_FLAT_HARNESS_CANCEL_ROUTINE
        sdv_RunCancelFunction(po,pirp);
#endif
        #if (SDV_HARNESS==SDV_PNP_HARNESS)
            sdv_RunISRRoutines(sdv_kinterrupt,sdv_pDpcContext);
            sdv_RunKeDpcRoutines(sdv_kdpc3,sdv_pDpcContext,sdv_pv1,sdv_pv2);
            sdv_RunIoDpcRoutines(sdv_kdpc,po,pirp,sdv_pIoDpcContext);
        #endif
#else
        status = sdv_DoNothing();
#endif
        break;
#endif

#ifndef SDV_FLAT_HARNESS_MODIFIER_NO_IRP_MJ_CREATE
        case IRP_MJ_CREATE:
        ps->MajorFunction = IRP_MJ_CREATE;
#ifdef fun_IRP_MJ_CREATE
        status = fun_IRP_MJ_CREATE(po,pirp);
#ifdef SDV_FLAT_HARNESS_CANCEL_ROUTINE
        sdv_RunCancelFunction(po,pirp);
#endif
        #if (SDV_HARNESS==SDV_PNP_HARNESS)
            sdv_RunISRRoutines(sdv_kinterrupt,sdv_pDpcContext);
            sdv_RunKeDpcRoutines(sdv_kdpc3,sdv_pDpcContext,sdv_pv1,sdv_pv2);
            sdv_RunIoDpcRoutines(sdv_kdpc,po,pirp,sdv_pIoDpcContext);
        #endif
#else
        status = sdv_DoNothing();
#endif
        break;
#endif

#ifndef SDV_FLAT_HARNESS_MODIFIER_NO_IRP_MJ_DEVICE_CONTROL
        case IRP_MJ_DEVICE_CONTROL:
        ps->MajorFunction = IRP_MJ_DEVICE_CONTROL;
#ifdef fun_IRP_MJ_DEVICE_CONTROL
        status = fun_IRP_MJ_DEVICE_CONTROL(po,pirp);
#ifdef SDV_FLAT_HARNESS_CANCEL_ROUTINE
        sdv_RunCancelFunction(po,pirp);
#endif
        #if (SDV_HARNESS==SDV_PNP_HARNESS)
            sdv_RunISRRoutines(sdv_kinterrupt,sdv_pDpcContext);
            sdv_RunKeDpcRoutines(sdv_kdpc3,sdv_pDpcContext,sdv_pv1,sdv_pv2);
            sdv_RunIoDpcRoutines(sdv_kdpc,po,pirp,sdv_pIoDpcContext);
        #endif
#else
        status = sdv_DoNothing();
#endif
        break;
#endif

#ifndef SDV_FLAT_HARNESS_MODIFIER_NO_IRP_MJ_FILE_SYSTEM_CONTROL
        case IRP_MJ_FILE_SYSTEM_CONTROL:
#ifdef fun_IRP_MJ_FILE_SYSTEM_CONTROL
        status = fun_IRP_MJ_FILE_SYSTEM_CONTROL(po,pirp);
#ifdef SDV_FLAT_HARNESS_CANCEL_ROUTINE
        sdv_RunCancelFunction(po,pirp);
#endif
        #if (SDV_HARNESS==SDV_PNP_HARNESS)
            sdv_RunISRRoutines(sdv_kinterrupt,sdv_pDpcContext);
            sdv_RunKeDpcRoutines(sdv_kdpc3,sdv_pDpcContext,sdv_pv1,sdv_pv2);
            sdv_RunIoDpcRoutines(sdv_kdpc,po,pirp,sdv_pIoDpcContext);
        #endif
#else
        status = sdv_DoNothing();
#endif
        break;
#endif

#ifndef SDV_FLAT_HARNESS_MODIFIER_NO_IRP_MJ_FLUSH_BUFFERS
        case IRP_MJ_FLUSH_BUFFERS:
        ps->MajorFunction = IRP_MJ_FLUSH_BUFFERS;
#ifdef fun_IRP_MJ_FLUSH_BUFFERS
        status = fun_IRP_MJ_FLUSH_BUFFERS(po,pirp);
#ifdef SDV_FLAT_HARNESS_CANCEL_ROUTINE
        sdv_RunCancelFunction(po,pirp);
#endif
        #if (SDV_HARNESS==SDV_PNP_HARNESS)
            sdv_RunISRRoutines(sdv_kinterrupt,sdv_pDpcContext);
            sdv_RunKeDpcRoutines(sdv_kdpc3,sdv_pDpcContext,sdv_pv1,sdv_pv2);
            sdv_RunIoDpcRoutines(sdv_kdpc,po,pirp,sdv_pIoDpcContext);
        #endif
#else
        status = sdv_DoNothing();
#endif
        break;
#endif

#ifndef SDV_FLAT_HARNESS_MODIFIER_NO_IRP_MJ_INTERNAL_DEVICE_CONTROL
        case IRP_MJ_INTERNAL_DEVICE_CONTROL:
        ps->MajorFunction = IRP_MJ_INTERNAL_DEVICE_CONTROL;
#ifdef fun_IRP_MJ_INTERNAL_DEVICE_CONTROL
        status = fun_IRP_MJ_INTERNAL_DEVICE_CONTROL(po,pirp);
#ifdef SDV_FLAT_HARNESS_CANCEL_ROUTINE
        sdv_RunCancelFunction(po,pirp);
#endif
        #if (SDV_HARNESS==SDV_PNP_HARNESS)
           sdv_RunISRRoutines(sdv_kinterrupt,sdv_pDpcContext);
            sdv_RunKeDpcRoutines(sdv_kdpc3,sdv_pDpcContext,sdv_pv1,sdv_pv2);
            sdv_RunIoDpcRoutines(sdv_kdpc,po,pirp,sdv_pIoDpcContext);
        #endif
#else
        status = sdv_DoNothing();
#endif
        break;
#endif


#ifndef SDV_FLAT_HARNESS_MODIFIER_NO_IRP_MJ_LOCK_CONTROL
        case IRP_MJ_LOCK_CONTROL:
#ifdef fun_IRP_MJ_LOCK_CONTROL
        status = fun_IRP_MJ_LOCK_CONTROL(po,pirp);
#ifdef SDV_FLAT_HARNESS_CANCEL_ROUTINE
        sdv_RunCancelFunction(po,pirp);
#endif
        #if (SDV_HARNESS==SDV_PNP_HARNESS)
            sdv_RunISRRoutines(sdv_kinterrupt,sdv_pDpcContext);
            sdv_RunKeDpcRoutines(sdv_kdpc3,sdv_pDpcContext,sdv_pv1,sdv_pv2);
            sdv_RunIoDpcRoutines(sdv_kdpc,po,pirp,sdv_pIoDpcContext);
        #endif
#else
        status = sdv_DoNothing();
#endif
        break;
#endif

#ifndef SDV_FLAT_HARNESS_MODIFIER_NO_IRP_MJ_QUERY_INFORMATION
        case IRP_MJ_QUERY_INFORMATION:
        ps->MajorFunction = IRP_MJ_QUERY_INFORMATION;
#ifdef fun_IRP_MJ_QUERY_INFORMATION
        status = fun_IRP_MJ_QUERY_INFORMATION(po,pirp);
#ifdef SDV_FLAT_HARNESS_CANCEL_ROUTINE
        sdv_RunCancelFunction(po,pirp);
#endif
        #if (SDV_HARNESS==SDV_PNP_HARNESS)
            sdv_RunISRRoutines(sdv_kinterrupt,sdv_pDpcContext);
            sdv_RunKeDpcRoutines(sdv_kdpc3,sdv_pDpcContext,sdv_pv1,sdv_pv2);
            sdv_RunIoDpcRoutines(sdv_kdpc,po,pirp,sdv_pIoDpcContext);
        #endif
#else
        status = sdv_DoNothing();
#endif
        break;
#endif

#ifndef SDV_FLAT_HARNESS_MODIFIER_NO_IRP_MJ_READ
        case IRP_MJ_READ:
        ps->MajorFunction = IRP_MJ_READ;
#ifdef fun_IRP_MJ_READ
        status = fun_IRP_MJ_READ(po,pirp);
#ifdef SDV_FLAT_HARNESS_CANCEL_ROUTINE
        sdv_RunCancelFunction(po,pirp);
#endif
        #if (SDV_HARNESS==SDV_PNP_HARNESS)
            sdv_RunISRRoutines(sdv_kinterrupt,sdv_pDpcContext);
            sdv_RunKeDpcRoutines(sdv_kdpc3,sdv_pDpcContext,sdv_pv1,sdv_pv2);
            sdv_RunIoDpcRoutines(sdv_kdpc,po,pirp,sdv_pIoDpcContext);
        #endif
#else
        status = sdv_DoNothing();
#endif
        break;
#endif

#ifndef SDV_FLAT_HARNESS_MODIFIER_NO_IRP_MJ_SET_INFORMATION
        case IRP_MJ_SET_INFORMATION:
        ps->MajorFunction = IRP_MJ_SET_INFORMATION;
#ifdef fun_IRP_MJ_SET_INFORMATION
        status = fun_IRP_MJ_SET_INFORMATION(po,pirp);
#ifdef SDV_FLAT_HARNESS_CANCEL_ROUTINE
        sdv_RunCancelFunction(po,pirp);
#endif
        #if (SDV_HARNESS==SDV_PNP_HARNESS)
            sdv_RunISRRoutines(sdv_kinterrupt,sdv_pDpcContext);
            sdv_RunKeDpcRoutines(sdv_kdpc3,sdv_pDpcContext,sdv_pv1,sdv_pv2);
            sdv_RunIoDpcRoutines(sdv_kdpc,po,pirp,sdv_pIoDpcContext);
        #endif
#else
        status = sdv_DoNothing();
#endif
        break;
#endif

#ifndef SDV_FLAT_HARNESS_MODIFIER_NO_IRP_MJ_SYSTEM_CONTROL
        case IRP_MJ_SYSTEM_CONTROL:
        ps->MajorFunction = IRP_MJ_SYSTEM_CONTROL;
#ifdef fun_IRP_MJ_SYSTEM_CONTROL
        status = fun_IRP_MJ_SYSTEM_CONTROL(po,pirp);
#ifdef SDV_FLAT_HARNESS_CANCEL_ROUTINE
        sdv_RunCancelFunction(po,pirp);
#endif
        #if (SDV_HARNESS==SDV_PNP_HARNESS)
            sdv_RunISRRoutines(sdv_kinterrupt,sdv_pDpcContext);
            sdv_RunKeDpcRoutines(sdv_kdpc3,sdv_pDpcContext,sdv_pv1,sdv_pv2);
            sdv_RunIoDpcRoutines(sdv_kdpc,po,pirp,sdv_pIoDpcContext);
        #endif
#else
        status = sdv_DoNothing();
#endif
        break;
#endif

#ifndef SDV_FLAT_HARNESS_MODIFIER_NO_IRP_MJ_WRITE
        case IRP_MJ_WRITE:
        ps->MajorFunction = IRP_MJ_WRITE;
#ifdef fun_IRP_MJ_WRITE
        status = fun_IRP_MJ_WRITE(po,pirp);
#ifdef SDV_FLAT_HARNESS_CANCEL_ROUTINE
        sdv_RunCancelFunction(po,pirp);
#endif
        #if (SDV_HARNESS==SDV_PNP_HARNESS)
            sdv_RunISRRoutines(sdv_kinterrupt,sdv_pDpcContext);
            sdv_RunKeDpcRoutines(sdv_kdpc3,sdv_pDpcContext,sdv_pv1,sdv_pv2);
            sdv_RunIoDpcRoutines(sdv_kdpc,po,pirp,sdv_pIoDpcContext);
        #endif
#else
        status = sdv_DoNothing();
#endif
        break;
#endif

#ifndef SDV_FLAT_HARNESS_MODIFIER_NO_IRP_MJ_PNP
        case IRP_MJ_PNP:
        ps->MajorFunction = IRP_MJ_PNP;


 
        if (ps->MinorFunction == IRP_MN_START_DEVICE) {
            SdvAssume(!sdv_start_irp_already_issued);
        }

	if (ps->MinorFunction == IRP_MN_CANCEL_REMOVE_DEVICE) {
	    SdvAssume(!sdv_remove_irp_already_issued);
	}

        if (ps->MinorFunction == IRP_MN_REMOVE_DEVICE) {
            sdv_remove_irp_already_issued = 1;
#ifdef SDV_FLAT_HARNESS_MODIFIER_NO_IRP_MJ_PNP_MN_REMOVE_DEVICE
            SdvExit();
#endif
        }

#ifdef fun_IRP_MJ_PNP
#ifdef SDV_NON_BUS_MN_FUNCTIONS
        sdv_SetIrpMinorFunctionNonBusDriver(pirp);
#endif
        status = fun_IRP_MJ_PNP(po,pirp);
#ifdef SDV_FLAT_HARNESS_CANCEL_ROUTINE
        sdv_RunCancelFunction(po,pirp);
#endif
        #if (SDV_HARNESS==SDV_PNP_HARNESS)
            sdv_RunISRRoutines(sdv_kinterrupt,sdv_pDpcContext);
            sdv_RunKeDpcRoutines(sdv_kdpc3,sdv_pDpcContext,sdv_pv1,sdv_pv2);
            sdv_RunIoDpcRoutines(sdv_kdpc,po,pirp,sdv_pIoDpcContext);
        #endif
#else
        status = sdv_DoNothing();
#endif
        break;
#endif

#ifndef SDV_FLAT_HARNESS_MODIFIER_NO_IRP_MJ_POWER
        case IRP_MJ_POWER:
        ps->MajorFunction = IRP_MJ_POWER;
        
#ifdef fun_IRP_MJ_POWER
        sdv_SetPowerIrpMinorFunction(pirp);
        status = fun_IRP_MJ_POWER(po,pirp);
#ifdef SDV_FLAT_HARNESS_CANCEL_ROUTINE
        sdv_RunCancelFunction(po,pirp);
#endif
        #if (SDV_HARNESS==SDV_PNP_HARNESS)
            sdv_RunISRRoutines(sdv_kinterrupt,sdv_pDpcContext);
            sdv_RunKeDpcRoutines(sdv_kdpc3,sdv_pDpcContext,sdv_pv1,sdv_pv2);
            sdv_RunIoDpcRoutines(sdv_kdpc,po,pirp,sdv_pIoDpcContext);
        #endif
#else
        status = sdv_DoNothing();
#endif
        break;
#endif

        default:
        status = sdv_DoNothing();
        break;
    }
    
    sdv_stub_dispatch_end(status,pirp);

    sdv_end_info = pirp->IoStatus.Information;
    return status;
}




VOID 
sdv_RunCancelFunction(
    PDEVICE_OBJECT po, 
    PIRP pirp
    )
/*

Routine Description:

    Finds the appropriate Cancel function for the IRP, and then call that 
    cancelation routine in the driver.

Arguments:

    po - pointer to the device object.    
    pirp - pointer to the irp that we're using.

Return value:

    we have no return value for this function.

Notes:
    
    We will execute the correct Cancel routine for the driver.
    The Cancel routine is set by calling IoSetCancelationRoutine on the Irp.
    After the Cancelation routine has pirp->CancelRoutine should be set to NULL
    This is done in SDV_MACRO_STUB_CANCEL_END;.

*/
{
   
#ifdef fun_DRIVER_CANCEL_1    
    if(pirp->CancelRoutine!=0&&pirp->CancelRoutine==fun_DRIVER_CANCEL_1)
	{
	  SDV_MACRO_STUB_CANCEL_BEGIN(pirp);
	  fun_DRIVER_CANCEL_1(po, pirp);
	  SDV_MACRO_STUB_CANCEL_END(pirp);
	}
#endif
#ifdef fun_DRIVER_CANCEL_2    
    if(pirp->CancelRoutine!=0&&pirp->CancelRoutine==fun_DRIVER_CANCEL_2)
	{
	  SDV_MACRO_STUB_CANCEL_BEGIN(pirp);
	  fun_DRIVER_CANCEL_2(po, pirp);
	  SDV_MACRO_STUB_CANCEL_END(pirp);
	}
#endif
#ifdef fun_DRIVER_CANCEL_3    
    if(pirp->CancelRoutine!=0&&pirp->CancelRoutine==fun_DRIVER_CANCEL_3)
	{
	  SDV_MACRO_STUB_CANCEL_BEGIN(pirp);
	  fun_DRIVER_CANCEL_3(po, pirp);
	  SDV_MACRO_STUB_CANCEL_END(pirp);
	}
#endif
#ifdef fun_DRIVER_CANCEL_4    
    if(pirp->CancelRoutine!=0&&pirp->CancelRoutine==fun_DRIVER_CANCEL_4)
	{
	  SDV_MACRO_STUB_CANCEL_BEGIN(pirp);
	  fun_DRIVER_CANCEL_4(po, pirp);
	  SDV_MACRO_STUB_CANCEL_END(pirp);
	}
#endif
#ifdef fun_DRIVER_CANCEL_5    
    if(pirp->CancelRoutine!=0&&pirp->CancelRoutine==fun_DRIVER_CANCEL_5)
	{
	  SDV_MACRO_STUB_CANCEL_BEGIN(pirp);
	  fun_DRIVER_CANCEL_5(po, pirp);
	  SDV_MACRO_STUB_CANCEL_END(pirp);
	}
#endif
#ifdef fun_DRIVER_CANCEL_6    
    if(pirp->CancelRoutine!=0&&pirp->CancelRoutine==fun_DRIVER_CANCEL_6)
	{
	  SDV_MACRO_STUB_CANCEL_BEGIN(pirp);
	  fun_DRIVER_CANCEL_6(po, pirp);
	  SDV_MACRO_STUB_CANCEL_END(pirp);
	}
#endif
#ifdef fun_DRIVER_CANCEL_7    
    if(pirp->CancelRoutine!=0&&pirp->CancelRoutine==fun_DRIVER_CANCEL_7)
	{
	  SDV_MACRO_STUB_CANCEL_BEGIN(pirp);
	  fun_DRIVER_CANCEL_7(po, pirp);
	  SDV_MACRO_STUB_CANCEL_END(pirp);
	}
#endif
}



VOID
sdv_RunIoCompletionRoutines(
    __in PDEVICE_OBJECT DeviceObject, 
    __in PIRP Irp, 
    __in_opt PVOID Context
    )
/*

Routine Description:

    Finds the appropriate completion function for the IRP, and then calls that 
    completion routine in the driver.

Arguments:

    DeviceObject - pointer to a device object.
	po - pointer to the device object.    
    Context - context pointer.

Return value:

    we have no return value for this function.

Notes:
    
    We will execute the correct completion routine for the driver.
    The completion routine is set by calling IoSetConpletionRoutine on the Irp.
    

*/
{

PIO_STACK_LOCATION irpsp;    
irpsp = sdv_IoGetNextIrpStackLocation(Irp);
#ifdef fun_IO_COMPLETION_ROUTINE_1
if(irpsp->CompletionRoutine == fun_IO_COMPLETION_ROUTINE_1)
{
    SDV_IRQL_PUSH(DISPATCH_LEVEL);
    fun_IO_COMPLETION_ROUTINE_1(DeviceObject,Irp,Context);  
    SDV_IRQL_POP();
}
#endif
#ifdef fun_IO_COMPLETION_ROUTINE_2
if(irpsp->CompletionRoutine == fun_IO_COMPLETION_ROUTINE_2)
{
    SDV_IRQL_PUSH(DISPATCH_LEVEL);
    fun_IO_COMPLETION_ROUTINE_2(DeviceObject,Irp,Context);  
    SDV_IRQL_POP();
}
#endif
#ifdef fun_IO_COMPLETION_ROUTINE_3
if(irpsp->CompletionRoutine == fun_IO_COMPLETION_ROUTINE_3)
{
    SDV_IRQL_PUSH(DISPATCH_LEVEL);
    fun_IO_COMPLETION_ROUTINE_3(DeviceObject,Irp,Context);  
    SDV_IRQL_POP();
}
#endif
#ifdef fun_IO_COMPLETION_ROUTINE_4
if(irpsp->CompletionRoutine == fun_IO_COMPLETION_ROUTINE_4)
{
    SDV_IRQL_PUSH(DISPATCH_LEVEL);
    fun_IO_COMPLETION_ROUTINE_4(DeviceObject,Irp,Context);  
    SDV_IRQL_POP();
}
#endif
#ifdef fun_IO_COMPLETION_ROUTINE_5
if(irpsp->CompletionRoutine == fun_IO_COMPLETION_ROUTINE_5)
{
    SDV_IRQL_PUSH(DISPATCH_LEVEL);
    fun_IO_COMPLETION_ROUTINE_5(DeviceObject,Irp,Context);  
    SDV_IRQL_POP();
}
#endif
#ifdef fun_IO_COMPLETION_ROUTINE_6
if(irpsp->CompletionRoutine == fun_IO_COMPLETION_ROUTINE_6)
{
    SDV_IRQL_PUSH(DISPATCH_LEVEL);
    fun_IO_COMPLETION_ROUTINE_6(DeviceObject,Irp,Context);  
    SDV_IRQL_POP();
}
#endif
}




VOID
sdv_RunISRRoutines(
    struct _KINTERRUPT *ki, 
    PVOID pv1
    )
/*

Routine Description:

    Finds the appropriate ISR function and then calls that 
    ISR routine in the driver.

Arguments:

    ki - Points to the Interrupt object for this device.
	  pv1 - Context.    
    
Return value:

    we have no return value for this function.

Notes:
    
    We will execute the correct ISR routine for the driver.
    The ISR routine is set by calling IoConnectInterrupt function.
    

*/
{
#ifdef fun_KSERVICE_ROUTINE_1
       if(sdv_isr_routine==fun_KSERVICE_ROUTINE_1)
       {
           SDV_IRQL_PUSH(SDV_DIRQL);
           fun_KSERVICE_ROUTINE_1(ki, pv1);
           SDV_IRQL_POPTO(PASSIVE_LEVEL);
       }
#endif
#ifdef fun_KSERVICE_ROUTINE_2
       if(sdv_isr_routine==fun_KSERVICE_ROUTINE_2)
       {
           SDV_IRQL_PUSH(SDV_DIRQL);
           fun_KSERVICE_ROUTINE_2(ki, pv1);
           SDV_IRQL_POPTO(PASSIVE_LEVEL);
       }
#endif
#ifdef fun_KSERVICE_ROUTINE_3
       if(sdv_isr_routine==fun_KSERVICE_ROUTINE_3)
       {
           SDV_IRQL_PUSH(SDV_DIRQL);
           fun_KSERVICE_ROUTINE_3(ki, pv1);
           SDV_IRQL_POPTO(PASSIVE_LEVEL);
       }
#endif
#ifdef fun_KSERVICE_ROUTINE_4
       if(sdv_isr_routine==fun_KSERVICE_ROUTINE_4)
       {
           SDV_IRQL_PUSH(SDV_DIRQL);
           fun_KSERVICE_ROUTINE_4(ki, pv1);
           SDV_IRQL_POPTO(PASSIVE_LEVEL);
       }
#endif
#ifdef fun_KSERVICE_ROUTINE_5
       if(sdv_isr_routine==fun_KSERVICE_ROUTINE_5)
       {
           SDV_IRQL_PUSH(SDV_DIRQL);
           fun_KSERVICE_ROUTINE_5(ki, pv1);
           SDV_IRQL_POPTO(PASSIVE_LEVEL);
       }
#endif
#ifdef fun_KSERVICE_ROUTINE_6
       if(sdv_isr_routine==fun_KSERVICE_ROUTINE_6)
       {
           SDV_IRQL_PUSH(SDV_DIRQL);
           fun_KSERVICE_ROUTINE_6(ki, pv1);
           SDV_IRQL_POPTO(PASSIVE_LEVEL);
       }
#endif

}


VOID
sdv_RunKeDpcRoutines(
    IN struct _KDPC *kdpc, 
    IN PVOID pDpcContext, 
    IN PVOID pv2, 
    IN PVOID pv3
    )
/*

Routine Description:

    Finds the appropriate Dpc function and then calls that 
    Dpc routine in the driver.

Arguments:

    kdpc - Pointer to the KDPC structure for the DPC object.
	  pDpcContext -  Caller-supplied pointer to driver-defined context information that was specified in a previous call to KeInitializeDpc.
	  pv2 -  Caller-supplied pointer to driver-supplied information that was specified in a previous call to KeInsertQueueDpc.  
	  pv3 -  Caller-supplied pointer to driver-supplied information that was specified in a previous call to KeInsertQueueDpc.
    
Return value:

    we have no return value for this function.

Notes:
    
    We will execute the correct DPC routine for the driver.

*/
{
#ifdef fun_KDEFERRED_ROUTINE_1
       if(kdpc->DeferredRoutine==fun_KDEFERRED_ROUTINE_1&&sdv_dpc_ke_registered)
       {
           SDV_IRQL_PUSH(DISPATCH_LEVEL);
           fun_KDEFERRED_ROUTINE_1(kdpc,pDpcContext,sdv_pv2,sdv_pv3);
           SDV_IRQL_POPTO(PASSIVE_LEVEL);
       }
#endif
#ifdef fun_KDEFERRED_ROUTINE_2
       if(kdpc->DeferredRoutine==fun_KDEFERRED_ROUTINE_2&&sdv_dpc_ke_registered)
       {
           SDV_IRQL_PUSH(DISPATCH_LEVEL);
           fun_KDEFERRED_ROUTINE_2(kdpc,pDpcContext,sdv_pv2,sdv_pv3);
           SDV_IRQL_POPTO(PASSIVE_LEVEL);
       }
#endif
#ifdef fun_KDEFERRED_ROUTINE_3
       if(kdpc->DeferredRoutine==fun_KDEFERRED_ROUTINE_3&&sdv_dpc_ke_registered)
       {
           SDV_IRQL_PUSH(DISPATCH_LEVEL);
           fun_KDEFERRED_ROUTINE_3(kdpc,pDpcContext,sdv_pv2,sdv_pv3);
           SDV_IRQL_POPTO(PASSIVE_LEVEL);
       }
#endif
#ifdef fun_KDEFERRED_ROUTINE_4
       if(kdpc->DeferredRoutine==fun_KDEFERRED_ROUTINE_4&&sdv_dpc_ke_registered)
       {
           SDV_IRQL_PUSH(DISPATCH_LEVEL);
           fun_KDEFERRED_ROUTINE_4(kdpc,pDpcContext,sdv_pv2,sdv_pv3);
           SDV_IRQL_POPTO(PASSIVE_LEVEL);
       }
#endif
#ifdef fun_KDEFERRED_ROUTINE_5
       if(kdpc->DeferredRoutine==fun_KDEFERRED_ROUTINE_5&&sdv_dpc_ke_registered)
       {
           SDV_IRQL_PUSH(DISPATCH_LEVEL);
           fun_KDEFERRED_ROUTINE_5(kdpc,pDpcContext,sdv_pv2,sdv_pv3);
           SDV_IRQL_POPTO(PASSIVE_LEVEL);
       }
#endif
#ifdef fun_KDEFERRED_ROUTINE_6
       if(kdpc->DeferredRoutine==fun_KDEFERRED_ROUTINE_6&&sdv_dpc_ke_registered)
       {
           SDV_IRQL_PUSH(DISPATCH_LEVEL);
           fun_KDEFERRED_ROUTINE_6(kdpc,pDpcContext,sdv_pv2,sdv_pv3);
           SDV_IRQL_POPTO(PASSIVE_LEVEL);
       }
#endif
#ifdef fun_KDEFERRED_ROUTINE_7
       if(kdpc->DeferredRoutine==fun_KDEFERRED_ROUTINE_7&&sdv_dpc_ke_registered)
       {
           SDV_IRQL_PUSH(DISPATCH_LEVEL);
           fun_KDEFERRED_ROUTINE_7(kdpc,pDpcContext,sdv_pv2,sdv_pv3);
           SDV_IRQL_POPTO(PASSIVE_LEVEL);
       }
#endif
#ifdef fun_KDEFERRED_ROUTINE_8
       if(kdpc->DeferredRoutine==fun_KDEFERRED_ROUTINE_8&&sdv_dpc_ke_registered)
       {
           SDV_IRQL_PUSH(DISPATCH_LEVEL);
           fun_KDEFERRED_ROUTINE_8(kdpc,pDpcContext,sdv_pv2,sdv_pv3);
           SDV_IRQL_POPTO(PASSIVE_LEVEL);
       }
#endif
#ifdef fun_KDEFERRED_ROUTINE_9
       if(kdpc->DeferredRoutine==fun_KDEFERRED_ROUTINE_9&&sdv_dpc_ke_registered)
       {
           SDV_IRQL_PUSH(DISPATCH_LEVEL);
           fun_KDEFERRED_ROUTINE_9(kdpc,pDpcContext,sdv_pv2,sdv_pv3);
           SDV_IRQL_POPTO(PASSIVE_LEVEL);
       }
#endif
}




VOID
sdv_RunIoDpcRoutines(
    IN PKDPC  Dpc,    
    IN struct _DEVICE_OBJECT  *DeviceObject,    
    IN struct _IRP  *Irp,    
    IN PVOID  Context
    )
/*

Routine Description:

    Finds the appropriate Dpc function and then calls that 
    Dpc routine in the driver.

Arguments:

    dpc          -  Pointer to the KDPC structure for the DPC object.
	  DeviceObject -  Caller-supplied pointer to a DEVICE_OBJECT structure. This is the device object for the target device, previously created by the driver's AddDevice routine
	  Irp          -  Caller-supplied pointer to an IRP structure that describes the I/O operation. 
	  Context      -  Caller-supplied pointer to driver-defined context information, specified in a previous call to IoRequestDpc.
    
Return value:

    we have no return value for this function.

Notes:
    
    We will execute the correct IO DPC routine for the driver.

*/
{
#ifdef fun_IO_DPC_ROUTINE_1
if(sdv_io_dpc==fun_IO_DPC_ROUTINE_1&&sdv_dpc_io_registered)
{
    SDV_IRQL_PUSH(DISPATCH_LEVEL);
    fun_IO_DPC_ROUTINE_1(Dpc,DeviceObject,Irp,Context); 
    SDV_IRQL_POPTO(PASSIVE_LEVEL);
}
#endif 
#ifdef fun_IO_DPC_ROUTINE_2
if(sdv_io_dpc==fun_IO_DPC_ROUTINE_2&&sdv_dpc_io_registered)
{
    SDV_IRQL_PUSH(DISPATCH_LEVEL);
    fun_IO_DPC_ROUTINE_2(Dpc,DeviceObject,Irp,Context); 
    SDV_IRQL_POPTO(PASSIVE_LEVEL);
}
#endif 
#ifdef fun_IO_DPC_ROUTINE_3
if(sdv_io_dpc==fun_IO_DPC_ROUTINE_3&&sdv_dpc_io_registered)
{
    SDV_IRQL_PUSH(DISPATCH_LEVEL);
    fun_IO_DPC_ROUTINE_3(Dpc,DeviceObject,Irp,Context); 
    SDV_IRQL_POPTO(PASSIVE_LEVEL);
}
#endif 
#ifdef fun_IO_DPC_ROUTINE_4
if(sdv_io_dpc==fun_IO_DPC_ROUTINE_4&&sdv_dpc_io_registered)
{
    SDV_IRQL_PUSH(DISPATCH_LEVEL);
    fun_IO_DPC_ROUTINE_4(Dpc,DeviceObject,Irp,Context); 
    SDV_IRQL_POPTO(PASSIVE_LEVEL);
}
#endif 
#ifdef fun_IO_DPC_ROUTINE_5
if(sdv_io_dpc==fun_IO_DPC_ROUTINE_5&&sdv_dpc_io_registered)
{
    SDV_IRQL_PUSH(DISPATCH_LEVEL);
    fun_IO_DPC_ROUTINE_5(Dpc,DeviceObject,Irp,Context); 
    SDV_IRQL_POPTO(PASSIVE_LEVEL);
}
#endif 
#ifdef fun_IO_DPC_ROUTINE_6
if(sdv_io_dpc==fun_IO_DPC_ROUTINE_6&&sdv_dpc_io_registered)
{
    SDV_IRQL_PUSH(DISPATCH_LEVEL);
    fun_IO_DPC_ROUTINE_6(Dpc,DeviceObject,Irp,Context); 
    SDV_IRQL_POPTO(PASSIVE_LEVEL);
}
#endif 
}



NTSTATUS 
sdv_RunQueryDeviceRelations(
    PDEVICE_OBJECT po, 
    PIRP pirp
    )
/*

Routine Description:

   Call PNP dispatch with IRP_MN_QUERY_DEVICE_RELATIONS

Arguments:

    po - pointer to the device object.    
    pirp - pointer to the irp that we're using.

Return value:

    we're passing the dispatch's return value back.

Notes:
    

*/
{
    NTSTATUS status;
    PIO_STACK_LOCATION ps = SDV_MACRO_IOGETCURRENTIRPSTACKLOCATION(pirp);

    sdv_end_info = sdv_start_info = pirp->IoStatus.Information;
 
    sdv_SetStatus(pirp);

    pirp->CancelRoutine = NULL;


    ps->MinorFunction = IRP_MN_QUERY_DEVICE_RELATIONS;
    ps->MajorFunction = IRP_MJ_PNP;

#ifdef fun_IRP_MJ_PNP
        status = fun_IRP_MJ_PNP(po,pirp);
#else
        status = sdv_DoNothing();
#endif

	return status;
}

NTSTATUS 
sdv_RunStartDevice(
    PDEVICE_OBJECT po, 
    PIRP pirp
    )
/*

Routine Description:

   Run the start device function

Arguments:

    po - pointer to the device object.    
    pirp - pointer to the irp that we're using.

Return value:

    we're passing the dispatch's return value back.

Notes:
    
    We're interacting with SDV meta-data here.  If "fun_IRP_MJ_CREATE" is
    defined in the meta-data, then we're calling it.  Otherwise: we call
    the sdv_DoNothing() function.

*/
{
    NTSTATUS status;

    PIO_STACK_LOCATION ps = SDV_MACRO_IOGETCURRENTIRPSTACKLOCATION(pirp);
    ps->MajorFunction = IRP_MJ_PNP;
    ps->MinorFunction = IRP_MN_START_DEVICE;
    
    sdv_SetStatus(pirp);
    sdv_start_irp_already_issued = 1;
    ps->CompletionRoutine = NULL;

    sdv_stub_dispatch_begin();
   
    #ifdef fun_IRP_MJ_PNP
        status = fun_IRP_MJ_PNP(po,pirp);
    #else
        status = sdv_DoNothing();
    #endif
    sdv_stub_dispatch_end(status,pirp);

    return status;
}

NTSTATUS 
sdv_RunRemoveDevice(
    PDEVICE_OBJECT po, 
    PIRP pirp
    )
/*

Routine Description:

   Run the remove device function

Arguments:

    po - pointer to the device object.    
    pirp - pointer to the irp that we're using.

Return value:

    we're passing the dispatch's return value back.

*/
{

    NTSTATUS status;

    PIO_STACK_LOCATION ps = SDV_MACRO_IOGETCURRENTIRPSTACKLOCATION(pirp);
    ps->MajorFunction = IRP_MJ_PNP;
    ps->MinorFunction = IRP_MN_REMOVE_DEVICE;
    pirp->CancelRoutine = NULL;
    
 
    SdvAssume(!sdv_remove_irp_already_issued);
   
    sdv_stub_dispatch_begin();


    sdv_SetStatus(pirp);

    
   
    #ifdef fun_IRP_MJ_PNP
        status = fun_IRP_MJ_PNP(po,pirp);
    #else
        status = sdv_DoNothing();
    #endif
    sdv_stub_dispatch_end(status,pirp);

    return status;

}



VOID
sdv_RunStartIo(
    PDEVICE_OBJECT po, 
    PIRP pirp
    )
{
    sdv_stub_startio_begin();
#ifdef fun_DriverStartIo
    pirp->CancelRoutine = NULL;
    fun_DriverStartIo(po, pirp);
#else
    sdv_DoNothing();
#endif
    sdv_stub_startio_end();
}


VOID
sdv_RunUnload(
    PDRIVER_OBJECT pdrivo
    )
{
#ifdef fun_DriverUnload
    fun_DriverUnload(pdrivo);
#else
    sdv_DoNothing();
#endif
}



BOOLEAN
sdv_CheckDispatchRoutines()
{
#if defined(fun_IRP_MJ_CLEANUP) || \
	defined(fun_IRP_MJ_CLOSE) || \
	defined(fun_IRP_MJ_CREATE) ||\
	defined(fun_IRP_MJ_DEVICE_CONTROL) || \
	defined(fun_IRP_MJ_FILE_SYSTEM_CONTROL) || \
	defined(fun_IRP_MJ_FLUSH_BUFFERS) || \
	defined(fun_IRP_MJ_INTERNAL_DEVICE_CONTROL) || \
	defined(fun_IRP_MJ_LOCK_CONTROL) || \
	defined(fun_IRP_MJ_QUERY_INFORMATION) || \
	defined(fun_IRP_MJ_READ) || \
	defined(fun_IRP_MJ_SET_INFORMATION) || \
	defined(fun_IRP_MJ_SYSTEM_CONTROL) || \
	defined(fun_IRP_MJ_WRITE) || \
	defined(fun_IRP_MJ_PNP) || \
	defined(fun_IRP_MJ_POWER) 

	return TRUE;
#else
    return FALSE;
#endif
}

BOOLEAN
sdv_CheckStartIoRoutines()
{
#if defined(fun_DriverStartIo)
	return TRUE;
#else
    return FALSE;
#endif
}



BOOLEAN
sdv_CheckDpcRoutines()
{
#if defined(fun_KDEFERRED_ROUTINE_1) || \
	defined(fun_KDEFERRED_ROUTINE_2) || \
	defined(fun_KDEFERRED_ROUTINE_3) || \
	defined(fun_KDEFERRED_ROUTINE_4) || \
	defined(fun_KDEFERRED_ROUTINE_5) || \
	defined(fun_KDEFERRED_ROUTINE_6) || \
	defined(fun_KDEFERRED_ROUTINE_7) || \
	defined(fun_KDEFERRED_ROUTINE_8) || \
	defined(fun_KDEFERRED_ROUTINE_9) 
	return TRUE;
#else
    return FALSE;
#endif
}



BOOLEAN
sdv_CheckIsrRoutines()
{
#if defined(fun_KSERVICE_ROUTINE_1) || \
	defined(fun_KSERVICE_ROUTINE_2) || \
	defined(fun_KSERVICE_ROUTINE_3) ||\
	defined(fun_KSERVICE_ROUTINE_4) || \
	defined(fun_KSERVICE_ROUTINE_5) || \
	defined(fun_KSERVICE_ROUTINE_6) 
	return TRUE;
#else
    return FALSE;
#endif
}



BOOLEAN
sdv_CheckCancelRoutines()
{
#if defined(fun_DRIVER_CANCEL_1) || \
	defined(fun_DRIVER_CANCEL_2) || \
	defined(fun_DRIVER_CANCEL_3) || \
	defined(fun_DRIVER_CANCEL_4) || \
	defined(fun_DRIVER_CANCEL_5) || \
	defined(fun_DRIVER_CANCEL_6) || \
	defined(fun_DRIVER_CANCEL_7)
	return TRUE;
#else
    return FALSE;
#endif
}






BOOLEAN
sdv_CheckCancelRoutines1()
{
#if defined(fun_DRIVER_CANCEL_1) && \
	!defined(fun_DRIVER_CANCEL_2) && \
	!defined(fun_DRIVER_CANCEL_3) && \
	!defined(fun_DRIVER_CANCEL_4) && \
	!defined(fun_DRIVER_CANCEL_5) && \
	!defined(fun_DRIVER_CANCEL_6) && \
	!defined(fun_DRIVER_CANCEL_7) 
	return TRUE;
#else
    return FALSE;
#endif
}


BOOLEAN
sdv_CheckIoDpcRoutines()
{
#if defined(fun_IO_DPC_ROUTINE_1) || \
	defined(fun_IO_DPC_ROUTINE_2) || \
	defined(fun_IO_DPC_ROUTINE_3) ||\
	defined(fun_IO_DPC_ROUTINE_4) || \
	defined(fun_IO_DPC_ROUTINE_5) || \
	defined(fun_IO_DPC_ROUTINE_6) 
	return TRUE;
#else
    return FALSE;
#endif
}


BOOLEAN
sdv_IoCompletionRoutines()
{
#if defined(fun_IO_COMPLETION_ROUTINE_1) || \
	defined(fun_IO_COMPLETION_ROUTINE_2) || \
	defined(fun_IO_COMPLETION_ROUTINE_3) ||\
	defined(fun_IO_COMPLETION_ROUTINE_4) || \
	defined(fun_IO_COMPLETION_ROUTINE_5) || \
	defined(fun_IO_COMPLETION_ROUTINE_6) 
	return TRUE;
#else
    return FALSE;
#endif
}


BOOLEAN
sdv_CheckWorkerRoutines()
{
#if defined(fun_WORKER_THREAD_ROUTINE_1) || \
	defined(fun_WORKER_THREAD_ROUTINE_2) || \
	defined(fun_WORKER_THREAD_ROUTINE_3) ||\
	defined(fun_WORKER_THREAD_ROUTINE_4) || \
	defined(fun_WORKER_THREAD_ROUTINE_5) || \
	defined(fun_WORKER_THREAD_ROUTINE_6) 
	return TRUE;
#else
    return FALSE;
#endif
}


BOOLEAN
sdv_CheckAddDevice()
{
#if defined(fun_AddDevice)  
	return TRUE;
#else
    return FALSE;
#endif
}


BOOLEAN
sdv_CheckIrpMjPnp()
{
#if defined(fun_IRP_MJ_PNP) 
	return TRUE;
#else
    return FALSE;
#endif
}


BOOLEAN
sdv_CheckDriverUnload()
{
#if defined(fun_DriverUnload)
	return TRUE;
#else
    return FALSE;
#endif
}



NTSTATUS sdv_RunAddDevice(
    PDRIVER_OBJECT p1,
    PDEVICE_OBJECT p2
    )
{

    NTSTATUS status;
    sdv_stub_add_begin();
#ifdef fun_AddDevice
    status = fun_AddDevice(p1,p2);
#else
    status = sdv_DoNothing();
#endif
    sdv_stub_add_end();
    return status;
}


VOID 
sdv_SetIrpMinorFunctionNonBusDriver(
    PIRP pirp
    )  
{
    PIO_STACK_LOCATION r = SDV_MACRO_IOGETCURRENTIRPSTACKLOCATION(pirp);
    LONG x = SdvMakeChoice();
    
    switch (x) 
    {
        case 0:
        r->MinorFunction = IRP_MN_START_DEVICE;
        break;
        case 1:
        r->MinorFunction = IRP_MN_STOP_DEVICE;
        break;
        case 2:
        r->MinorFunction = IRP_MN_CANCEL_STOP_DEVICE;
        break; 
        case 3:
        r->MinorFunction = IRP_MN_REMOVE_DEVICE;
        break;      
        case 4:
        r->MinorFunction = IRP_MN_CANCEL_REMOVE_DEVICE;
        break;      
        case 5:
        r->MinorFunction = IRP_MN_SURPRISE_REMOVAL;
        break;           
        default:
        r->MinorFunction = IRP_MN_DEVICE_USAGE_NOTIFICATION;
        break;
    }

}




VOID SdvAssume(int e)
/*
Routine Description:
    Acts like an ASSERT, but halts analysis if the assert fails.
*/
{
  if (!e) SdvExit();
}


VOID SdvAssumeSoft(int e)
/*
Routine Description:
   Acts like an SdvAssume.   
*/
{
  if (!e) SdvExit();
}




/* 
    Disable for SdvExit: C4717: recursive on all control paths,
    function will cause runtime stack overflow.

    This is correctly flagged by the compiler, and would be a serious
    issue if the harness was to be executed rather than simulated.

    However in this case, this is per design in order to simulate
    non-progress:
*/
#pragma warning(disable:4717)

VOID SdvExit() 
/*

Routine Description:

    Acts like "exit()" within the context of model checking.

Notes:
    Since SdvExit contributes no extra reachable states, this serves as a
    dead-end of sorts to tools based on reachabilitity analysis.

*/
{ 
    SdvExit();

    /* Enable after SdvExit: C4717: */
    #pragma warning(default:4717)
}

/* ..\wdm\harness-parts.c end */

/* ..\wdm\ntddk-io.c begin */
/*
	Copyright (c) Microsoft Corporation.  All rights reserved.
*/



typedef struct _IO_WORKITEM *PIO_WORKITEM;

typedef
VOID
(*PIO_WORKITEM_ROUTINE) (
    IN PDEVICE_OBJECT DeviceObject,
    IN PVOID Context
    );

typedef
VOID
(*PIO_WORKITEM_ROUTINE_EX) (
    IN PVOID IoObject,
    IN PVOID Context,
    IN PIO_WORKITEM IoWorkItem
    );

typedef struct _IO_WORKITEM {
    WORK_QUEUE_ITEM WorkItem;
    PIO_WORKITEM_ROUTINE_EX Routine;
    PVOID IoObject;
    PVOID Context;
    ULONG Type;
#if DBG
    ULONG Size;
#endif
} IO_WORKITEM;


DEVICE_OBJECT sdv_devobj_top;
PDEVICE_OBJECT sdv_p_devobj_top = &sdv_devobj_top;

CONTROLLER_OBJECT sdv_IoCreateController_CONTROLLER_OBJECT;
KEVENT sdv_IoCreateNotificationEvent_KEVENT;
KEVENT sdv_IoCreateSynchronizationEvent_KEVENT;
DEVICE_OBJECT sdv_IoGetDeviceObjectPointer_DEVICE_OBJECT;
DEVICE_OBJECT sdv_IoGetDeviceToVerify_DEVICE_OBJECT;
DMA_ADAPTER sdv_IoGetDmaAdapter_DMA_ADAPTER;
GENERIC_MAPPING sdv_IoGetFileObjectGenericMapping_GENERIC_MAPPING;
DEVICE_OBJECT sdv_IoGetRelatedDeviceObject_DEVICE_OBJECT;
struct _DRIVE_LAYOUT_INFORMATION_EX sdv_IoReadPartitionTableEx_DRIVE_LAYOUT_INFORMATION_EX;




NTKERNELAPI
VOID
IoAcquireCancelSpinLock(
    OUT PKIRQL p_old_irql
    )
{
    SDV_IRQL_PUSH(DISPATCH_LEVEL);
    *p_old_irql = sdv_irql_previous;
}

NTSTATUS
sdv_IoAcquireRemoveLock(
    IN PIO_REMOVE_LOCK RemoveLock,
    IN PVOID Tag
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return STATUS_UNSUCCESSFUL;break;
        default: 
            if (sdv_remove_irp_already_issued) {
                return STATUS_DELETE_PENDING;
            }
            else {
	        return STATUS_SUCCESS;
            }
			break;
    }
}

NTSYSAPI
NTSTATUS
NTAPI
IoAcquireRemoveLockEx(
    IN PIO_REMOVE_LOCK RemoveLock,
    IN OPTIONAL PVOID Tag,
    IN PCSTR File,
    IN ULONG Line,
    IN ULONG RemlockSize
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return STATUS_UNSUCCESSFUL;break;
        default: 
            if (sdv_remove_irp_already_issued) {
                return STATUS_DELETE_PENDING;
            }
            else {
	        return STATUS_SUCCESS;
            }
			break;
    }
}

NTKERNELAPI
VOID
sdv_IoAdjustPagingPathCount(
    IN PLONG Count,
    IN BOOLEAN Increment
    )
{
}

NTKERNELAPI
NTSTATUS
sdv_IoAllocateAdapterChannel(
    IN PADAPTER_OBJECT AdapterObject,
    IN PDEVICE_OBJECT DeviceObject,
    IN ULONG NumberOfMapRegisters,
    IN PDRIVER_CONTROL ExecutionRoutine,
    IN PVOID Context
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return STATUS_SUCCESS;break;
        default: return STATUS_INSUFFICIENT_RESOURCES;break;
    }
}

NTKERNELAPI
VOID
IoAllocateController(
    IN PCONTROLLER_OBJECT ControllerObject,
    IN PDEVICE_OBJECT DeviceObject,
    IN PDRIVER_CONTROL ExecutionRoutine,
    IN PVOID Context
    )
{
}

NTKERNELAPI
NTSTATUS
IoAllocateDriverObjectExtension(
    IN PDRIVER_OBJECT DriverObject,
    IN PVOID ClientIdentificationAddress,
    IN ULONG DriverObjectExtensionSize,
    OUT PVOID *DriverObjectExtension
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return STATUS_SUCCESS;break;
        case 1: return STATUS_INSUFFICIENT_RESOURCES;break;
        default: return STATUS_OBJECT_NAME_COLLISION;break;
    }
}

NTKERNELAPI
PVOID
IoAllocateErrorLogEntry(
    IN PVOID IoObject,
    IN UCHAR EntrySize
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return malloc(1);break;
        default: return NULL;break;
    }
}

NTKERNELAPI
PIRP
IoAllocateIrp(
    IN CCHAR StackSize,
    IN BOOLEAN ChargeQuota
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return sdv_other_irp;break;
        default: return NULL;break;
    }
}

NTKERNELAPI
PMDL
IoAllocateMdl(
    IN PVOID VirtualAddress,
    IN ULONG Length,
    IN BOOLEAN SecondaryBuffer,
    IN BOOLEAN ChargeQuota,
    IN OUT PIRP pirp OPTIONAL
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return (PMDL) malloc(1);break;
        default: return NULL;break;
    }
}

PIO_WORKITEM
IoAllocateWorkItem(
    IN PDEVICE_OBJECT DeviceObject
    )
{
    int choice = SdvMakeChoice();
    PIO_WORKITEM ioWorkItem = (PIO_WORKITEM) malloc(1);
    switch (choice) {

    case 0:

        SdvAssume(ioWorkItem != NULL);
        ioWorkItem->WorkItem.List.Flink = NULL;
        return ioWorkItem;
		break;
    default:

        return NULL;
		break;

    }
}

VOID
sdv_IoAssignArcName(
    IN PUNICODE_STRING ArcName,
    IN PUNICODE_STRING DeviceName
    )
{
}

NTKERNELAPI
NTSTATUS
IoAssignResources(
    IN PUNICODE_STRING RegistryPath,
    IN PUNICODE_STRING DriverClassName OPTIONAL,
    IN PDRIVER_OBJECT DriverObject,
    IN PDEVICE_OBJECT DeviceObject OPTIONAL,
    IN PIO_RESOURCE_REQUIREMENTS_LIST RequestedResources,
    IN OUT PCM_RESOURCE_LIST *AllocatedResources
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return STATUS_SUCCESS;break;
        default: return STATUS_UNSUCCESSFUL;break;
    }
}

NTKERNELAPI
NTSTATUS
IoAttachDevice(
    IN PDEVICE_OBJECT SourceDevice,
    IN PUNICODE_STRING TargetDevice,
    OUT PDEVICE_OBJECT *AttachedDevice
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return STATUS_SUCCESS;break;
        case 1: return STATUS_INVALID_PARAMETER;break;
        case 2: return STATUS_OBJECT_TYPE_MISMATCH;break;
        case 3: return STATUS_OBJECT_NAME_INVALID;break;
        default: return STATUS_INSUFFICIENT_RESOURCES;break;
    }
}

NTKERNELAPI
NTSTATUS
IoAttachDeviceByPointer(
    IN PDEVICE_OBJECT SourceDevice,
    IN PDEVICE_OBJECT TargetDevice
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return STATUS_SUCCESS;break;
        default: return STATUS_UNSUCCESSFUL;break;
    }
}

NTKERNELAPI
PDEVICE_OBJECT
IoAttachDeviceToDeviceStack(
    IN PDEVICE_OBJECT SourceDevice,
    IN PDEVICE_OBJECT TargetDevice
    )
{
    if (TargetDevice == sdv_p_devobj_one) {
        return TargetDevice;
    } else {
        return NULL;
    }
}

NTKERNELAPI
PIRP
IoBuildAsynchronousFsdRequest(
    IN ULONG MajorFunction,
    IN PDEVICE_OBJECT DeviceObject,
    IN OUT PVOID Buffer OPTIONAL,
    IN ULONG Length OPTIONAL,
    IN PLARGE_INTEGER StartingOffset OPTIONAL,
    IN PIO_STATUS_BLOCK IoStatusBlock OPTIONAL
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {

    case 0:

        sdv_other_irp->Tail.Overlay.CurrentStackLocation->MajorFunction=
            (UCHAR) MajorFunction;
        return sdv_other_irp;
		break;
    default:

        return NULL;
		break;

    }
}

NTKERNELAPI
PIRP
IoBuildDeviceIoControlRequest(
    IN ULONG IoControlCode,
    IN PDEVICE_OBJECT DeviceObject,
    IN PVOID InputBuffer OPTIONAL,
    IN ULONG InputBufferLength,
    OUT PVOID OutputBuffer OPTIONAL,
    IN ULONG OutputBufferLength,
    IN BOOLEAN InternalDeviceIoControl,
    IN PKEVENT Event,
    OUT PIO_STATUS_BLOCK IoStatusBlock
    )
{
    int choice = SdvMakeChoice();

    IoStatusBlock->Status = STATUS_SUCCESS;

    switch (choice) {

    case 0:
        if(InternalDeviceIoControl)
        {
            sdv_other_irp->Tail.Overlay.CurrentStackLocation->MajorFunction=IRP_MJ_INTERNAL_DEVICE_CONTROL;
            sdv_other_harnessStackLocation_next.MajorFunction=IRP_MJ_INTERNAL_DEVICE_CONTROL;
        }
        else
        {
            sdv_other_irp->Tail.Overlay.CurrentStackLocation->MajorFunction=IRP_MJ_DEVICE_CONTROL;
            sdv_other_harnessStackLocation_next.MajorFunction=IRP_MJ_DEVICE_CONTROL;
        }
        return sdv_other_irp;
        break;
    default:

        return NULL;
		break;

    }
}

NTKERNELAPI
VOID
IoBuildPartialMdl(
    IN PMDL SourceMdl,
    IN OUT PMDL TargetMdl,
    IN PVOID VirtualAddress,
    IN ULONG Length
    )
{
}

NTKERNELAPI
PIRP
IoBuildSynchronousFsdRequest(
    IN ULONG MajorFunction,
    IN PDEVICE_OBJECT DeviceObject,
    IN OUT PVOID Buffer OPTIONAL,
    IN ULONG Length OPTIONAL,
    IN PLARGE_INTEGER StartingOffset OPTIONAL,
    IN PKEVENT Event,
    OUT PIO_STATUS_BLOCK IoStatusBlock
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {

    case 0:

        sdv_other_irp->Tail.Overlay.CurrentStackLocation->MajorFunction=
            (UCHAR) MajorFunction;
            return sdv_other_irp;
			break;

    default:

            return NULL;
			break;

    }
}

NTKERNELAPI
NTSTATUS
sdv_IoCallDriver(
    IN PDEVICE_OBJECT DeviceObject,
    IN OUT PIRP Irp
    )
{

    int choice = SdvMakeChoice();
    #ifdef SDV_FLAT_HARNESS_COMPLETION_ROUTINE
    PVOID lcontext = sdv_context;
    #endif

    NTSTATUS status;

    switch (choice) {

    case 0:

        Irp->IoStatus.Status = STATUS_SUCCESS;
        status=STATUS_SUCCESS;
        Irp->PendingReturned = 0;
        break;

    case 1:

        Irp->IoStatus.Status = STATUS_UNSUCCESSFUL;
        status=STATUS_UNSUCCESSFUL;
        Irp->PendingReturned = 0;
        break;

    default:
        status=STATUS_PENDING;
        Irp->PendingReturned = 1;
        break;

    }

    SdvAssume(Irp->IoStatus.Status != STATUS_PENDING);

    if (status != STATUS_PENDING) {
        SdvAssume( status == Irp->IoStatus.Status) ;
    }
    #ifdef SDV_FLAT_HARNESS_COMPLETION_ROUTINE
    if (sdv_compFset) 
    {
      sdv_RunIoCompletionRoutines(DeviceObject, Irp, lcontext);
    }
    #endif

        
        


    return status;
}

NTKERNELAPI
BOOLEAN
IoCancelIrp(
    IN PIRP Irp
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return FALSE;break;
        default: return TRUE;break;
    }
}

NTKERNELAPI
NTSTATUS
IoCheckShareAccess(
    IN ACCESS_MASK DesiredAccess,
    IN ULONG DesiredShareAccess,
    IN OUT PFILE_OBJECT FileObject,
    IN OUT PSHARE_ACCESS ShareAccess,
    IN BOOLEAN Update
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return STATUS_SUCCESS;break;
        default: return STATUS_SHARING_VIOLATION;break;
    }
}

VOID
sdv_IoCompleteRequest(
    IN PIRP pirp,
    IN CCHAR PriorityBoost
    )
{
}

NTKERNELAPI
NTSTATUS
IoConnectInterrupt(
    OUT PKINTERRUPT *InterruptObject,
    IN PKSERVICE_ROUTINE ServiceRoutine,
    IN PVOID ServiceContext,
    IN PKSPIN_LOCK SpinLock OPTIONAL,
    IN ULONG Vector,
    IN KIRQL Irql,
    IN KIRQL SynchronizeIrql,
    IN KINTERRUPT_MODE InterruptMode,
    IN BOOLEAN ShareVector,
    IN KAFFINITY ProcessorEnableMask,
    IN BOOLEAN FloatingSave
    )
{
    int choice = SdvMakeChoice();
    sdv_isr_routine = ServiceRoutine;
    sdv_pDpcContext = ServiceContext;
    switch (choice) {
        case 0: return STATUS_SUCCESS;break;
        case 1: return STATUS_INVALID_PARAMETER;break;
        default: return STATUS_INSUFFICIENT_RESOURCES;break;
    }
}

#if WINVER >= 0x0600
NTKERNELAPI
NTSTATUS
IoConnectInterruptEx(
    IN OUT PIO_CONNECT_INTERRUPT_PARAMETERS Parameters
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return STATUS_SUCCESS;break;
        case 1: return STATUS_UNSUCCESSFUL;break;
        case 2: return STATUS_INVALID_DEVICE_REQUEST;break;
        case 3: return STATUS_INVALID_PARAMETER;break;
        case 4: return STATUS_INVALID_PARAMETER_1;break;
        case 5: return STATUS_INVALID_PARAMETER_10;break;
        default: return STATUS_NOT_FOUND;break;
    }
}
#endif

VOID
sdv_IoCopyCurrentIrpStackLocationToNext(
    IN PIRP pirp
    )
{
   SDV_MACRO_COPYCURRENTIRPSTACKLOCATIONTONEXT(pirp)
}

NTKERNELAPI
PCONTROLLER_OBJECT
IoCreateController(
    IN ULONG Size
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return &sdv_IoCreateController_CONTROLLER_OBJECT;break;
        default: return NULL;break;
    }
}

NTKERNELAPI
NTSTATUS
IoCreateDevice(
    IN PDRIVER_OBJECT DriverObject,
    IN ULONG DeviceExtensionSize,
    IN PUNICODE_STRING DeviceName OPTIONAL,
    IN DEVICE_TYPE DeviceType,
    IN ULONG DeviceCharacteristics,
    IN BOOLEAN Exclusive,
    OUT PDEVICE_OBJECT *DeviceObject
    )
{

    int choice = SdvMakeChoice();

    sdv_io_create_device_called++;

    switch (choice) {
        case 0: (*DeviceObject) = sdv_p_devobj_two; return STATUS_SUCCESS;break;
        case 1: (*DeviceObject) = NULL; return STATUS_UNSUCCESSFUL;break;
        case 2: (*DeviceObject) = NULL; return STATUS_INSUFFICIENT_RESOURCES;break;
        case 3: (*DeviceObject) = NULL; return STATUS_OBJECT_NAME_EXISTS;break;
        default: (*DeviceObject) = NULL; return STATUS_OBJECT_NAME_COLLISION;break;
    }
}

NTKERNELAPI
NTSTATUS
sdv_IoCreateDeviceSecure(
    IN PDRIVER_OBJECT DriverObject,
    IN ULONG DeviceExtensionSize,
    IN PUNICODE_STRING DeviceName OPTIONAL,
    IN DEVICE_TYPE DeviceType,
    IN ULONG DeviceCharacteristics,
    IN BOOLEAN Exclusive,
    IN PCUNICODE_STRING DefaultSDDLString,
    IN LPCGUID DeviceClassGuid OPTIONAL,
    OUT PDEVICE_OBJECT *DeviceObject
    )
{

    int choice = SdvMakeChoice();

    sdv_io_create_device_called++;

    switch (choice) {
        case 0: (*DeviceObject) = sdv_p_devobj_two; return STATUS_SUCCESS;break;
        case 1: (*DeviceObject) = NULL; return STATUS_UNSUCCESSFUL;break;
        case 2: (*DeviceObject) = NULL; return STATUS_INSUFFICIENT_RESOURCES;break;
        case 3: (*DeviceObject) = NULL; return STATUS_OBJECT_NAME_EXISTS;break;
        default: (*DeviceObject) = NULL; return STATUS_OBJECT_NAME_COLLISION;break;
    }
}

NTKERNELAPI
NTSTATUS
IoCreateFile(
    OUT PHANDLE FileHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes,
    OUT PIO_STATUS_BLOCK IoStatusBlock,
    IN PLARGE_INTEGER AllocationSize OPTIONAL,
    IN ULONG FileAttributes,
    IN ULONG ShareAccess,
    IN ULONG Disposition,
    IN ULONG CreateOptions,
    IN PVOID EaBuffer OPTIONAL,
    IN ULONG EaLength,
    IN CREATE_FILE_TYPE CreateFileType,
    IN PVOID ExtraCreateParameters OPTIONAL,
    IN ULONG Options
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return STATUS_SUCCESS;break;
        default: return STATUS_UNSUCCESSFUL;break;
    }
}

NTKERNELAPI
PKEVENT
IoCreateNotificationEvent(
    IN PUNICODE_STRING EventName,
    OUT PHANDLE EventHandle
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return &sdv_IoCreateNotificationEvent_KEVENT;break;
        default: return NULL;break;
    }
}

NTKERNELAPI
NTSTATUS
IoCreateSymbolicLink(
    IN PUNICODE_STRING SymbolicLinkName,
    IN PUNICODE_STRING DeviceName
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return STATUS_SUCCESS;break;
        default: return STATUS_UNSUCCESSFUL;break;
    }
}

NTKERNELAPI
PKEVENT
IoCreateSynchronizationEvent(
    IN PUNICODE_STRING EventName,
    OUT PHANDLE EventHandle
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return &sdv_IoCreateSynchronizationEvent_KEVENT;break;
        default: return NULL;break;
    }
}

NTKERNELAPI
NTSTATUS
IoCreateUnprotectedSymbolicLink(
    IN PUNICODE_STRING SymbolicLinkName,
    IN PUNICODE_STRING DeviceName
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return STATUS_SUCCESS;break;
        default: return STATUS_UNSUCCESSFUL;break;
    }
}

NTSTATUS
sdv_IoCsqInitialize(
    IN PIO_CSQ Csq,
    IN PIO_CSQ_INSERT_IRP CsqInsertIrp,
    IN PIO_CSQ_REMOVE_IRP CsqRemoveIrp,
    IN PIO_CSQ_PEEK_NEXT_IRP CsqPeekNextIrp,
    IN PIO_CSQ_ACQUIRE_LOCK CsqAcquireLock,
    IN PIO_CSQ_RELEASE_LOCK CsqReleaseLock,
    IN PIO_CSQ_COMPLETE_CANCELED_IRP CsqCompleteCanceledIrp
    )
{
    Csq->CsqInsertIrp = CsqInsertIrp;
    Csq->CsqRemoveIrp = CsqRemoveIrp;
    Csq->CsqPeekNextIrp = CsqPeekNextIrp;
    Csq->CsqAcquireLock = CsqAcquireLock;
    Csq->CsqReleaseLock = CsqReleaseLock;
    Csq->CsqCompleteCanceledIrp = CsqCompleteCanceledIrp;
    Csq->ReservePointer = NULL;

    Csq->Type = IO_TYPE_CSQ;

    return STATUS_SUCCESS;
}

NTKERNELAPI
NTSTATUS
sdv_IoCsqInitializeEx(
    IN PIO_CSQ Csq,
    IN PIO_CSQ_INSERT_IRP_EX CsqInsertIrp,
    IN PIO_CSQ_REMOVE_IRP CsqRemoveIrp,
    IN PIO_CSQ_PEEK_NEXT_IRP CsqPeekNextIrp,
    IN PIO_CSQ_ACQUIRE_LOCK CsqAcquireLock,
    IN PIO_CSQ_RELEASE_LOCK CsqReleaseLock,
    IN PIO_CSQ_COMPLETE_CANCELED_IRP CsqCompleteCanceledIrp 
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return STATUS_SUCCESS;break;
        default: return STATUS_UNSUCCESSFUL;break;
    }
}

VOID
sdv_IoCsqInsertIrp(
    IN PIO_CSQ Csq,
    IN PIRP pirp,
    IN PIO_CSQ_IRP_CONTEXT Context
    )
{
    Csq->CsqInsertIrp(Csq, pirp);
    IoMarkIrpPending(pirp);
}

NTSTATUS
sdv_IoCsqInsertIrpEx(
    IN PIO_CSQ Csq,
    IN PIRP pirp,
    IN PIO_CSQ_IRP_CONTEXT Context,
    IN PVOID InsertContext
    )
{
    NTSTATUS status;

    PIO_CSQ_INSERT_IRP_EX func;

    func = (PIO_CSQ_INSERT_IRP_EX)Csq->CsqInsertIrp;

    status = func(Csq, pirp, InsertContext);

    if (!NT_SUCCESS(status)) {
        return status;
    }
    IoMarkIrpPending(pirp);

    return status;
}

NTKERNELAPI
PIRP
sdv_IoCsqRemoveIrp(
    IN PIO_CSQ Csq,
    IN PIO_CSQ_IRP_CONTEXT Context
    )
{

    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return (PIRP) malloc(1);break;
        default: return NULL;break;
    }
}

PIRP
sdv_IoCsqRemoveNextIrp(
    IN PIO_CSQ Csq,
    IN PVOID PeekContext
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return (PIRP) malloc(1);break;
        default: return NULL;break;
    }
}

NTKERNELAPI
VOID
sdv_IoDeassignArcName(
    IN PUNICODE_STRING ArcName
    )
{
}

NTKERNELAPI
VOID
IoDeleteController(
    IN PCONTROLLER_OBJECT ControllerObject
    )
{
}

NTKERNELAPI
VOID
IoDeleteDevice(
    IN PDEVICE_OBJECT DeviceObject
    )
{
}

NTKERNELAPI
NTSTATUS
IoDeleteSymbolicLink(
    IN PUNICODE_STRING SymbolicLinkName
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return STATUS_SUCCESS;break;
        default: return STATUS_UNSUCCESSFUL;break;
    }
}

NTKERNELAPI
VOID
IoDetachDevice(
    IN OUT PDEVICE_OBJECT TargetDevice
    )
{
}

NTKERNELAPI
VOID
IoDisconnectInterrupt(
    IN PKINTERRUPT InterruptObject
    )
{
}

#if WINVER >= 0x0600
NTKERNELAPI
VOID
IoDisconnectInterruptEx(
    IN OUT PIO_DISCONNECT_INTERRUPT_PARAMETERS Parameters
    )
{
}
#endif

NTKERNELAPI
BOOLEAN
sdv_IoFlushAdapterBuffers(
    IN PADAPTER_OBJECT AdapterObject,
    IN PMDL Mdl,
    IN PVOID MapRegisterBase,
    IN PVOID CurrentVa,
    IN ULONG Length,
    IN BOOLEAN WriteToDevice
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return FALSE;break;
        default: return TRUE;break;
    }
}

NTKERNELAPI
BOOLEAN
IoForwardIrpSynchronously(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp 
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return FALSE;break;
        default: return TRUE;break;
    }
}

NTKERNELAPI
VOID
sdv_IoFreeAdapterChannel(
    IN PADAPTER_OBJECT AdapterObject
    )
{
}

NTKERNELAPI
VOID
IoFreeController(
    IN PCONTROLLER_OBJECT ControllerObject
    )
{
}

NTKERNELAPI
VOID
IoFreeErrorLogEntry(
    PVOID ElEntry
    )
{
}

NTKERNELAPI
VOID
IoFreeIrp(
    IN PIRP pirp
    )
{
}

VOID 
sdv_IoFreeMapRegisters(
    IN PADAPTER_OBJECT AdapterObject,
    IN PVOID MapRegisterBase,
    IN ULONG NumberOfMapRegisters
    )
{
}

NTKERNELAPI
VOID
IoFreeMdl(
    IN PMDL Mdl
    )
{
}

VOID
IoFreeWorkItem(
    IN PIO_WORKITEM IoWorkItem
    )
{
}

NTKERNELAPI
PDEVICE_OBJECT
IoGetAttachedDeviceReference(
    IN PDEVICE_OBJECT DeviceObject
    )
{
    return &sdv_devobj_top;
}

NTKERNELAPI
NTSTATUS
IoGetBootDiskInformation(
    IN OUT PBOOTDISK_INFORMATION BootDiskInformation,
    IN ULONG Size
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return STATUS_SUCCESS;break;
        case 1: return STATUS_INVALID_PARAMETER;break;
        default: return STATUS_TOO_LATE;break;
    }
}

NTKERNELAPI
PCONFIGURATION_INFORMATION
IoGetConfigurationInformation(
    VOID
    )
{
    return (PCONFIGURATION_INFORMATION) malloc(1);
}

PIO_STACK_LOCATION
sdv_IoGetCurrentIrpStackLocation(
    IN PIRP pirp
    )
{
    return (pirp->Tail.Overlay.CurrentStackLocation);
}

NTKERNELAPI
PEPROCESS
IoGetCurrentProcess(
    VOID
    )
{
    PEPROCESS p = (PEPROCESS) malloc(1);
    return p;
}

NTKERNELAPI
NTSTATUS
NTAPI
IoGetDeviceInterfaceAlias(
    IN PUNICODE_STRING SymbolicLinkName,
    IN CONST GUID *AliasInterfaceClassGuid,
    OUT PUNICODE_STRING AliasSymbolicLinkName
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return STATUS_SUCCESS;break;
        case 1: return STATUS_UNSUCCESSFUL;break;
        case 2: return STATUS_OBJECT_NAME_NOT_FOUND;break;
        case 3: return STATUS_OBJECT_PATH_NOT_FOUND;break;
        default: return STATUS_INVALID_HANDLE;break;
    }
}

NTKERNELAPI
NTSTATUS
NTAPI
IoGetDeviceInterfaces(
    IN CONST GUID *InterfaceClassGuid,
    IN PDEVICE_OBJECT PhysicalDeviceObject OPTIONAL,
    IN ULONG Flags,
    OUT PWSTR *SymbolicLinkList
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: SdvAssume(*SymbolicLinkList != NULL);  
                return STATUS_SUCCESS;break;
        case 1: return STATUS_UNSUCCESSFUL;break;
        default: return STATUS_INVALID_DEVICE_REQUEST;break;
    }
}

NTKERNELAPI
NTSTATUS
IoGetDeviceObjectPointer(
    IN PUNICODE_STRING ObjectName,
    IN ACCESS_MASK DesiredAccess,
    OUT PFILE_OBJECT *FileObject,
    OUT PDEVICE_OBJECT *DeviceObject
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: *DeviceObject = &sdv_IoGetDeviceObjectPointer_DEVICE_OBJECT;
                return STATUS_SUCCESS;break;
        case 1: return STATUS_OBJECT_TYPE_MISMATCH;break;
        case 2: return STATUS_INVALID_PARAMETER;break;
        case 3: return STATUS_PRIVILEGE_NOT_HELD;break;
        case 4: return STATUS_INSUFFICIENT_RESOURCES;break;
        default: return STATUS_OBJECT_NAME_INVALID;break;
    }
}

NTKERNELAPI
NTSTATUS
IoGetDeviceProperty(
    IN PDEVICE_OBJECT DeviceObject,
    IN DEVICE_REGISTRY_PROPERTY DeviceProperty,
    IN ULONG BufferLength,
    OUT PVOID PropertyBuffer,
    OUT PULONG ResultLength
    )
{
    ULONG L = SdvKeepChoice();
    if ( L <= 0 ) {
        switch ( L ) {
            case 0: return STATUS_UNSUCCESSFUL;break;
            case -1: return STATUS_INVALID_PARAMETER_2;break;
            default: return STATUS_INVALID_DEVICE_REQUEST;break;
        }
    } else if ( L <= BufferLength ) {
        *ResultLength = L;
        return STATUS_SUCCESS;
    } else {
        *ResultLength = L;
        return STATUS_BUFFER_TOO_SMALL;
    }
}

NTKERNELAPI
PDEVICE_OBJECT
IoGetDeviceToVerify(
    IN PETHREAD Thread
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return NULL;break;
        default: return &sdv_IoGetDeviceToVerify_DEVICE_OBJECT;break;
    }
}

NTKERNELAPI
PDMA_ADAPTER
IoGetDmaAdapter(
    IN PDEVICE_OBJECT PhysicalDeviceObject,
    IN PDEVICE_DESCRIPTION DeviceDescription,
    IN OUT PULONG NumberOfMapRegisters
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return NULL;break;
        default: return &sdv_IoGetDmaAdapter_DMA_ADAPTER;break;
    }
}

NTKERNELAPI
PVOID
IoGetDriverObjectExtension(
    IN PDRIVER_OBJECT DriverObject,
    IN PVOID ClientIdentificationAddress
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return malloc(1);break;
        default: return NULL;break;
    }
}

NTKERNELAPI
PGENERIC_MAPPING
IoGetFileObjectGenericMapping(
    VOID
    )
{
    return &sdv_IoGetFileObjectGenericMapping_GENERIC_MAPPING;
}

NTKERNELAPI
ULONG
sdv_IoGetFunctionCodeFromCtlCode(
    IN ULONG ControlCode
    )
{
    ULONG res;
    res = SdvKeepChoice();
    return res;
}

NTKERNELAPI
PVOID
IoGetInitialStack(
    VOID
    )
{
    return malloc(1);
}

PIO_STACK_LOCATION
sdv_IoGetNextIrpStackLocation(
    IN PIRP pirp
    )
{
    if (pirp == &sdv_harnessIrp) {
        return &sdv_harnessStackLocation_next;
    } else if (pirp == &sdv_other_harnessIrp) {
        return &sdv_other_harnessStackLocation_next;
    } else {
        return &sdv_harnessStackLocation;
    }
}

NTKERNELAPI
PDEVICE_OBJECT
IoGetRelatedDeviceObject(
    IN PFILE_OBJECT FileObject
    )
{
    return &sdv_IoGetRelatedDeviceObject_DEVICE_OBJECT;
}

NTKERNELAPI
ULONG_PTR
sdv_IoGetRemainingStackSize(
    VOID
    )
{
    ULONG_PTR l = SdvKeepChoice();
    return l;
}

NTKERNELAPI
VOID
IoGetStackLimits(
    OUT PULONG_PTR LowLimit,
    OUT PULONG_PTR HighLimit
    )
{
}

VOID
sdv_IoInitializeDpcRequest(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIO_DPC_ROUTINE DpcRoutine
    )
{
    sdv_io_dpc = DpcRoutine;
}

NTKERNELAPI
VOID
IoInitializeIrp(
    IN OUT PIRP Irp,
    IN USHORT PacketSize,
    IN CCHAR StackSize
    )
{
}

NTKERNELAPI
VOID
sdv_IoInitializeRemoveLock(
    IN PIO_REMOVE_LOCK Lock,
    IN ULONG AllocateTag,
    IN ULONG MaxLockedMinutes,
    IN ULONG HighWatermark
    )
{
}


NTKERNELAPI
NTSTATUS
IoInitializeTimer(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIO_TIMER_ROUTINE TimerRoutine,
    IN PVOID Context
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return STATUS_SUCCESS;break;
        default: return STATUS_UNSUCCESSFUL;break;
    }
}

VOID
IoInitializeWorkItem(
    IN PVOID IoObject,
    IN PIO_WORKITEM IoWorkItem
    )
{
}

NTKERNELAPI
VOID
IoInvalidateDeviceRelations(
    IN PDEVICE_OBJECT DeviceObject,
    IN DEVICE_RELATION_TYPE Type
    )
{
}

NTKERNELAPI
VOID
IoInvalidateDeviceState(
    IN PDEVICE_OBJECT PhysicalDeviceObject
    )
{
}

NTKERNELAPI
BOOLEAN
IoIs32bitProcess(
    IN PIRP Irp OPTIONAL
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return FALSE;break;
        default: return TRUE;break;
    }
}

NTKERNELAPI
BOOLEAN
sdv_IoIsErrorUserInduced(
    IN NTSTATUS Status
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return FALSE;break;
        default: return TRUE;break;
    }
}

NTKERNELAPI
BOOLEAN
IoIsWdmVersionAvailable(
    IN UCHAR MajorVersion,
    IN UCHAR MinorVersion
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return FALSE;break;
        default: return TRUE;break;
    }
}

NTKERNELAPI
PIRP
IoMakeAssociatedIrp(
    IN PIRP pirp,
    IN CCHAR StackSize
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return sdv_other_irp;break;
        default: return NULL;break;
    }
}

NTKERNELAPI
PHYSICAL_ADDRESS
sdv_IoMapTransfer(
    IN PADAPTER_OBJECT AdapterObject,
    IN PMDL Mdl,
    IN PVOID MapRegisterBase,
    IN PVOID CurrentVa,
    IN OUT PULONG Length,
    IN BOOLEAN WriteToDevice
    )
{
    PHYSICAL_ADDRESS l;
    l.QuadPart = (LONGLONG) SdvKeepChoice();
    return l;
}

VOID
sdv_IoMarkIrpPending(
    IN OUT PIRP pirp
    )
{
}

NTKERNELAPI
NTSTATUS
IoOpenDeviceInterfaceRegistryKey(
    IN PUNICODE_STRING SymbolicLinkName,
    IN ACCESS_MASK DesiredAccess,
    OUT PHANDLE DeviceInterfaceKey
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return STATUS_SUCCESS;break;
        case 1: return STATUS_UNSUCCESSFUL;break;
        case 2: return STATUS_OBJECT_NAME_NOT_FOUND;break;
        case 3: return STATUS_OBJECT_PATH_NOT_FOUND;break;
        default: return STATUS_INVALID_PARAMETER;break;
    }
}

NTKERNELAPI
NTSTATUS
IoOpenDeviceRegistryKey(
    IN PDEVICE_OBJECT DeviceObject,
    IN ULONG DevInstKeyType,
    IN ACCESS_MASK DesiredAccess,
    OUT PHANDLE DevInstRegKey
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return STATUS_SUCCESS;break;
        case 1: return STATUS_UNSUCCESSFUL;break;
        case 2: return STATUS_INVALID_PARAMETER;break;
        default: return STATUS_INVALID_DEVICE_REQUEST;break;
    }
}

NTKERNELAPI
NTSTATUS
IoQueryDeviceDescription(
    IN PINTERFACE_TYPE BusType OPTIONAL,
    IN PULONG BusNumber OPTIONAL,
    IN PCONFIGURATION_TYPE ControllerType OPTIONAL,
    IN PULONG ControllerNumber OPTIONAL,
    IN PCONFIGURATION_TYPE PeripheralType OPTIONAL,
    IN PULONG PeripheralNumber OPTIONAL,
    IN PIO_QUERY_DEVICE_ROUTINE CalloutRoutine,
    IN PVOID Context
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return STATUS_SUCCESS;break;
        default: return STATUS_UNSUCCESSFUL;break;
    }
}

NTKERNELAPI
VOID
IoQueueWorkItem(
    IN PIO_WORKITEM IoWorkItem,
    IN PIO_WORKITEM_ROUTINE WorkerRoutine,
    IN WORK_QUEUE_TYPE QueueType,
    IN PVOID Context
    )
{
}

NTKERNELAPI
VOID
IoQueueWorkItemEx(
    IN PIO_WORKITEM IoWorkItem,
    IN PIO_WORKITEM_ROUTINE_EX WorkerRoutine,
    IN WORK_QUEUE_TYPE QueueType,
    IN PVOID Context
    )
{
}

NTKERNELAPI
VOID
IoRaiseHardError(
    IN PIRP Irp,
    IN PVPB Vpb OPTIONAL,
    IN PDEVICE_OBJECT RealDeviceObject
    )
{
}

NTKERNELAPI
BOOLEAN
IoRaiseInformationalHardError(
    IN NTSTATUS ErrorStatus,
    IN PUNICODE_STRING String OPTIONAL,
    IN PKTHREAD Thread OPTIONAL
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return FALSE;break;
        default: return TRUE;break;
    }
}

NTKERNELAPI
VOID
IoRegisterBootDriverReinitialization(
    IN PDRIVER_OBJECT DriverObject,
    IN PDRIVER_REINITIALIZE DriverReinitializationRoutine,
    IN PVOID Context
    )
{
}

NTKERNELAPI
NTSTATUS
NTAPI
IoRegisterDeviceInterface(
    IN PDEVICE_OBJECT PhysicalDeviceObject,
    IN CONST GUID *InterfaceClassGuid,
    IN PUNICODE_STRING ReferenceString, OPTIONAL
    OUT PUNICODE_STRING SymbolicLinkName
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: SdvAssume(SymbolicLinkName->Buffer != NULL);
                return STATUS_SUCCESS;break;
        case 1: return STATUS_UNSUCCESSFUL;break;
        default: return STATUS_INVALID_DEVICE_REQUEST;break;
    }
}

NTKERNELAPI
VOID
IoRegisterDriverReinitialization(
    IN PDRIVER_OBJECT DriverObject,
    IN PDRIVER_REINITIALIZE DriverReinitializationRoutine,
    IN PVOID Context
    )
{
}

NTKERNELAPI
NTSTATUS
IoRegisterLastChanceShutdownNotification(
    IN PDEVICE_OBJECT DeviceObject
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return STATUS_SUCCESS;break;
        default: return STATUS_UNSUCCESSFUL;break;
    }
}

NTKERNELAPI
NTSTATUS
IoRegisterPlugPlayNotification(
    IN IO_NOTIFICATION_EVENT_CATEGORY EventCategory,
    IN ULONG EventCategoryFlags,
    IN PVOID EventCategoryData OPTIONAL,
    IN PDRIVER_OBJECT DriverObject,
    IN PDRIVER_NOTIFICATION_CALLBACK_ROUTINE CallbackRoutine,
    IN PVOID Context,
    OUT PVOID *NotificationEntry
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return STATUS_SUCCESS;break;
        default: return STATUS_UNSUCCESSFUL;break;
    }
}

NTKERNELAPI
NTSTATUS
IoRegisterShutdownNotification(
    IN PDEVICE_OBJECT DeviceObject
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return STATUS_SUCCESS;break;
        default: return STATUS_UNSUCCESSFUL;break;
    }
}

NTKERNELAPI
VOID
IoReleaseCancelSpinLock(
    IN KIRQL new_irql
    )
{
    SDV_IRQL_POPTO(new_irql);
}

VOID
sdv_IoReleaseRemoveLock(
    IN PIO_REMOVE_LOCK RemoveLock,
    IN PVOID Tag
    )
{
}

VOID
sdv_IoReleaseRemoveLockAndWait(
    IN PIO_REMOVE_LOCK RemoveLock,
    IN PVOID Tag
    )
{
}



NTKERNELAPI
VOID
IoRemoveShareAccess(
    IN PFILE_OBJECT FileObject,
    IN OUT PSHARE_ACCESS ShareAccess
    )
{
}

NTKERNELAPI
NTSTATUS
IoReportDetectedDevice(
    IN PDRIVER_OBJECT DriverObject,
    IN INTERFACE_TYPE LegacyBusType,
    IN ULONG BusNumber,
    IN ULONG SlotNumber,
    IN PCM_RESOURCE_LIST ResourceList,
    IN PIO_RESOURCE_REQUIREMENTS_LIST ResourceRequirements OPTIONAL,
    IN BOOLEAN ResourceAssigned,
    IN OUT PDEVICE_OBJECT *DeviceObject
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return STATUS_SUCCESS;break;
        default: return STATUS_UNSUCCESSFUL;break;
    }
}

NTKERNELAPI
NTSTATUS
IoReportResourceForDetection(
    IN PDRIVER_OBJECT DriverObject,
    IN PCM_RESOURCE_LIST DriverList OPTIONAL,
    IN ULONG DriverListSize OPTIONAL,
    IN PDEVICE_OBJECT DeviceObject OPTIONAL,
    IN PCM_RESOURCE_LIST DeviceList OPTIONAL,
    IN ULONG DeviceListSize OPTIONAL,
    OUT PBOOLEAN ConflictDetected
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return STATUS_SUCCESS;break;
        case 1: return STATUS_UNSUCCESSFUL;break;
        default: return STATUS_CONFLICTING_ADDRESSES;break;
    }
}

NTKERNELAPI
NTSTATUS
IoReportResourceUsage(
    IN PUNICODE_STRING DriverClassName OPTIONAL,
    IN PDRIVER_OBJECT DriverObject,
    IN PCM_RESOURCE_LIST DriverList OPTIONAL,
    IN ULONG DriverListSize OPTIONAL,
    IN PDEVICE_OBJECT DeviceObject,
    IN PCM_RESOURCE_LIST DeviceList OPTIONAL,
    IN ULONG DeviceListSize OPTIONAL,
    IN BOOLEAN OverrideConflict,
    OUT PBOOLEAN ConflictDetected
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return STATUS_SUCCESS;break;
        default: return STATUS_UNSUCCESSFUL;break;
    }
}

NTKERNELAPI
NTSTATUS
IoReportTargetDeviceChange(
    IN PDEVICE_OBJECT PhysicalDeviceObject,
    IN PVOID NotificationStructure
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return STATUS_SUCCESS;break;
        case 1: return STATUS_UNSUCCESSFUL;break;
        default: return STATUS_INVALID_DEVICE_REQUEST;break;
    }
}

NTKERNELAPI
NTSTATUS
IoReportTargetDeviceChangeAsynchronous(
    IN PDEVICE_OBJECT PhysicalDeviceObject,
    IN PVOID NotificationStructure,
    IN PDEVICE_CHANGE_COMPLETE_CALLBACK Callback OPTIONAL,
    IN PVOID Context OPTIONAL
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return STATUS_SUCCESS;break;
        case 1: return STATUS_UNSUCCESSFUL;break;
        default: return STATUS_INVALID_DEVICE_REQUEST;break;
    }
}

NTKERNELAPI
VOID
IoRequestDeviceEject(
    IN PDEVICE_OBJECT PhysicalDeviceObject
    )
{
}

NTKERNELAPI
VOID
sdv_IoRequestDpc(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN PVOID Context
    )
{
    sdv_dpc_io_registered = TRUE;
}

NTKERNELAPI
VOID
IoReuseIrp(
    IN OUT PIRP Irp,
    IN NTSTATUS Status
    )
{
}

PDRIVER_CANCEL
sdv_IoSetCancelRoutine(
    IN PIRP pirp,
    IN PDRIVER_CANCEL CancelRoutine
    )
{
    
    PDRIVER_CANCEL r = (PDRIVER_CANCEL) pirp->CancelRoutine;
    pirp->CancelRoutine = CancelRoutine;
    return r;
}

NTKERNELAPI
VOID
sdv_IoSetCompletionRoutine(
    IN PIRP pirp,
    IN PIO_COMPLETION_ROUTINE CompletionRoutine,
    IN PVOID Context,
    IN BOOLEAN InvokeOnSuccess,
    IN BOOLEAN InvokeOnError,
    IN BOOLEAN InvokeOnCancel
    )
{
    PIO_STACK_LOCATION irpSp;
    irpSp = IoGetNextIrpStackLocation(pirp);
    irpSp->CompletionRoutine = CompletionRoutine;
    sdv_compFset = 1;
    sdv_context = Context;
}

NTKERNELAPI
NTSTATUS
sdv_IoSetCompletionRoutineEx(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN PIO_COMPLETION_ROUTINE CompletionRoutine,
    IN PVOID Context,
    IN BOOLEAN InvokeOnSuccess,
    IN BOOLEAN InvokeOnError,
    IN BOOLEAN InvokeOnCancel
    )
{
    PIO_STACK_LOCATION irpSp;
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: 
           irpSp = IoGetNextIrpStackLocation(Irp);
           irpSp->CompletionRoutine = CompletionRoutine;
           sdv_compFset = 1;
           sdv_context = Context;
 	   return STATUS_SUCCESS;
	   break;
        default: return STATUS_INSUFFICIENT_RESOURCES;break;
    }
}

NTKERNELAPI
NTSTATUS
IoSetDeviceInterfaceState(
    IN PUNICODE_STRING SymbolicLinkName,
    IN BOOLEAN Enable
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return STATUS_SUCCESS;break;
        case 1: return STATUS_OBJECT_NAME_EXISTS;break;
        default: return STATUS_OBJECT_NAME_NOT_FOUND;break;
    }
}

NTKERNELAPI
VOID
IoSetHardErrorOrVerifyDevice(
    IN PIRP Irp,
    IN PDEVICE_OBJECT DeviceObject
    )
{
}

NTKERNELAPI
VOID
sdv_IoSetNextIrpStackLocation(
    IN OUT PIRP Irp
    )
{
}

NTKERNELAPI
VOID
IoSetShareAccess(
    IN ACCESS_MASK DesiredAccess,
    IN ULONG DesiredShareAccess,
    IN OUT PFILE_OBJECT FileObject,
    OUT PSHARE_ACCESS ShareAccess
    )
{
}

VOID
IoSetStartIoAttributes(
    IN PDEVICE_OBJECT DeviceObject,
    IN BOOLEAN DeferredStartIo,
    IN BOOLEAN NonCancelable
    )
{
}

NTKERNELAPI
NTSTATUS
IoSetSystemPartition(
    PUNICODE_STRING VolumeNameString
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return STATUS_SUCCESS;break;
        default: return STATUS_UNSUCCESSFUL;break;
    }
}

NTKERNELAPI
BOOLEAN
IoSetThreadHardErrorMode(
    IN BOOLEAN EnableHardErrors
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return FALSE;break;
        default: return TRUE;break;
    }
}

NTKERNELAPI
USHORT
sdv_IoSizeOfIrp(
    IN CCHAR StackSize
    )
{
    USHORT l = (USHORT) SdvKeepChoice();
    return l;
}

NTKERNELAPI
ULONG
sdv_IoSizeofWorkItem(
    VOID
    )
{
    ULONG l = SdvKeepChoice();
    return l;
}

VOID
sdv_IoSkipCurrentIrpStackLocation(
    IN PIRP pirp
    )
{
    SDV_MACRO_COPYCURRENTIRPSTACKLOCATIONTONEXT(pirp)
}

NTKERNELAPI
VOID
IoStartNextPacket(
    IN PDEVICE_OBJECT DeviceObject,
    IN BOOLEAN Cancelable
    )
{
}

VOID 
IoStartNextPacketByKey(
    IN PDEVICE_OBJECT DeviceObject,
    IN BOOLEAN Cancelable,
    IN ULONG Key
    )
{
}

NTKERNELAPI
VOID
IoStartPacket(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN PULONG Key OPTIONAL,
    IN PDRIVER_CANCEL CancelFunction OPTIONAL
    )
{
}

NTKERNELAPI
VOID
IoStartTimer(
    IN PDEVICE_OBJECT DeviceObject
    )
{
}

NTKERNELAPI
VOID
IoStopTimer(
    IN PDEVICE_OBJECT DeviceObject
    )
{
}

VOID
IoUninitializeWorkItem(
    IN PIO_WORKITEM IoWorkItem
    )
{
}

NTKERNELAPI
NTSTATUS
IoUnregisterPlugPlayNotification(
    IN PVOID NotificationEntry
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return STATUS_SUCCESS;break;
        default: return STATUS_UNSUCCESSFUL;break;
    }
}

NTKERNELAPI
VOID
IoUnregisterShutdownNotification(
    IN PDEVICE_OBJECT DeviceObject
    )
{
}

NTKERNELAPI
VOID
IoUpdateShareAccess(
    IN PFILE_OBJECT FileObject,
    IN OUT PSHARE_ACCESS ShareAccess
    )
{
}

NTSTATUS
IoValidateDeviceIoControlAccess(
    IN PIRP pirp,
    IN ULONG RequiredAccess
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return STATUS_SUCCESS;break;
        case 1: return STATUS_UNSUCCESSFUL;break;
        case 2: return STATUS_ACCESS_DENIED;break;
        default: return STATUS_INVALID_PARAMETER;break;
    }
}

NTKERNELAPI
NTSTATUS
IoVerifyPartitionTable(
    IN PDEVICE_OBJECT DeviceObject,
    IN BOOLEAN FixErrors
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return STATUS_SUCCESS;break;
        case 1: return STATUS_DISK_CORRUPT_ERROR;break;
        default: return STATUS_UNSUCCESSFUL;break;
    }
}

NTKERNELAPI
NTSTATUS
IoVolumeDeviceToDosName(
    IN PVOID VolumeDeviceObject,
    OUT PUNICODE_STRING DosName
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return STATUS_SUCCESS;break;
        default: return STATUS_UNSUCCESSFUL;break;
    }
}

NTKERNELAPI
NTSTATUS
IoWMIAllocateInstanceIds(
    IN GUID *Guid,
    IN ULONG InstanceCount,
    OUT ULONG *FirstInstanceId
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return STATUS_SUCCESS;break;
        case 1: return STATUS_UNSUCCESSFUL;break;
        default: return STATUS_INSUFFICIENT_RESOURCES;break;
    }
}

NTKERNELAPI
NTSTATUS
IoWMIDeviceObjectToInstanceName(
    IN PVOID DataBlockObject,
    IN PDEVICE_OBJECT DeviceObject,
    OUT PUNICODE_STRING InstanceName
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return STATUS_SUCCESS;break;
        case 1: return STATUS_UNSUCCESSFUL;break;
        default: return STATUS_WMI_INSTANCE_NOT_FOUND;break;
    }
}

NTKERNELAPI
ULONG
sdv_IoWMIDeviceObjectToProviderId(
    IN PDEVICE_OBJECT DeviceObject
    )
{
    ULONG l = SdvKeepChoice();
    return l;
}

NTKERNELAPI
NTSTATUS
IoWMIExecuteMethod(
    IN PVOID DataBlockObject,
    IN PUNICODE_STRING InstanceName,
    IN ULONG MethodId,
    IN ULONG InBufferSize,
    IN OUT PULONG OutBufferSize,
    IN OUT PUCHAR InOutBuffer
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return STATUS_SUCCESS;break;
        case 1: return STATUS_UNSUCCESSFUL;break;
        case 2: return STATUS_WMI_GUID_NOT_FOUND;break;
        case 3: return STATUS_WMI_INSTANCE_NOT_FOUND;break;
        case 4: return STATUS_WMI_ITEMID_NOT_FOUND;break;
        default: return STATUS_BUFFER_TOO_SMALL;break;
    }
}

NTKERNELAPI
NTSTATUS
IoWMIHandleToInstanceName(
    IN PVOID DataBlockObject,
    IN HANDLE FileHandle,
    OUT PUNICODE_STRING InstanceName
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return STATUS_SUCCESS;break;
        case 1: return STATUS_UNSUCCESSFUL;break;
        default: return STATUS_WMI_INSTANCE_NOT_FOUND;break;
    }
}

NTKERNELAPI
NTSTATUS
IoWMIOpenBlock(
    IN GUID *DataBlockGuid,
    IN ULONG DesiredAccess,
    OUT PVOID *DataBlockObject
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return STATUS_SUCCESS;break;
        default: return STATUS_UNSUCCESSFUL;break;
    }
}

NTKERNELAPI
NTSTATUS
IoWMIQueryAllData(
    IN PVOID DataBlockObject,
    IN OUT ULONG *InOutBufferSize,
    OUT PVOID OutBuffer
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return STATUS_SUCCESS;break;
        case 1: return STATUS_UNSUCCESSFUL;break;
        case 2: return STATUS_WMI_GUID_NOT_FOUND;break;
        default: return STATUS_BUFFER_TOO_SMALL;break;
    }
}

NTKERNELAPI
NTSTATUS
IoWMIQueryAllDataMultiple(
    IN PVOID *DataBlockObjectList,
    IN ULONG ObjectCount,
    IN OUT ULONG *InOutBufferSize,
    OUT PVOID OutBuffer
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return STATUS_SUCCESS;break;
        case 1: return STATUS_UNSUCCESSFUL;break;
        default: return STATUS_BUFFER_TOO_SMALL;break;
    }
}

NTKERNELAPI
NTSTATUS
IoWMIQuerySingleInstance(
    IN PVOID DataBlockObject,
    IN PUNICODE_STRING InstanceName,
    IN OUT ULONG *InOutBufferSize,
    OUT PVOID OutBuffer
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return STATUS_SUCCESS;break;
        case 1: return STATUS_UNSUCCESSFUL;break;
        case 2: return STATUS_WMI_GUID_NOT_FOUND;break;
        case 3: return STATUS_WMI_INSTANCE_NOT_FOUND;break;
        default: return STATUS_BUFFER_TOO_SMALL;break;
    }
}

NTKERNELAPI
NTSTATUS
IoWMIQuerySingleInstanceMultiple(
    IN PVOID *DataBlockObjectList,
    IN PUNICODE_STRING InstanceNames,
    IN ULONG ObjectCount,
    IN OUT ULONG *InOutBufferSize,
    OUT PVOID OutBuffer
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return STATUS_SUCCESS;break;
        case 1: return STATUS_UNSUCCESSFUL;break;
        default: return STATUS_BUFFER_TOO_SMALL;break;
    }
}

NTKERNELAPI
NTSTATUS
IoWMIRegistrationControl(
    IN PDEVICE_OBJECT DeviceObject,
    IN ULONG Action
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return STATUS_SUCCESS;break;
        case 1: return STATUS_UNSUCCESSFUL;break;
        default: return STATUS_INVALID_PARAMETER;break;
    }
}

#if WINVER > 0x0500
NTKERNELAPI
NTSTATUS
IoWMISetNotificationCallback(
    IN PVOID Object,
    IN WMI_NOTIFICATION_CALLBACK Callback,
    IN PVOID Context
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return STATUS_SUCCESS;break;
        default: return STATUS_UNSUCCESSFUL;break;
    }
}
#endif

#if WINVER > 0x0500
NTKERNELAPI
NTSTATUS
IoWMISetSingleInstance(
    IN PVOID DataBlockObject,
    IN PUNICODE_STRING InstanceName,
    IN ULONG Version,
    IN ULONG ValueBufferSize,
    IN PVOID ValueBuffer
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return STATUS_SUCCESS;break;
        case 1: return STATUS_UNSUCCESSFUL;break;
        case 2: return STATUS_WMI_GUID_NOT_FOUND;break;
        case 3: return STATUS_WMI_INSTANCE_NOT_FOUND;break;
        case 4: return STATUS_WMI_READ_ONLY;break;
        default: return STATUS_WMI_SET_FAILURE;break;
    }
}
#endif

#if WINVER > 0x0500
NTKERNELAPI
NTSTATUS
IoWMISetSingleItem(
    IN PVOID DataBlockObject,
    IN PUNICODE_STRING InstanceName,
    IN ULONG DataItemId,
    IN ULONG Version,
    IN ULONG ValueBufferSize,
    IN PVOID ValueBuffer
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return STATUS_SUCCESS;break;
        case 1: return STATUS_UNSUCCESSFUL;break;
        case 2: return STATUS_WMI_GUID_NOT_FOUND;break;
        case 3: return STATUS_WMI_INSTANCE_NOT_FOUND;break;
        case 4: return STATUS_WMI_ITEMID_NOT_FOUND;break;
        case 5: return STATUS_WMI_READ_ONLY;break;
        default: return STATUS_WMI_SET_FAILURE;break;
    }
}
#endif

NTKERNELAPI
NTSTATUS
IoWMISuggestInstanceName(
    IN PDEVICE_OBJECT PhysicalDeviceObject OPTIONAL,
    IN PUNICODE_STRING SymbolicLinkName OPTIONAL,
    IN BOOLEAN CombineNames,
    OUT PUNICODE_STRING SuggestedInstanceName
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return STATUS_SUCCESS;break;
        case 1: return STATUS_UNSUCCESSFUL;break;
        case 2: return STATUS_INSUFFICIENT_RESOURCES;break;
        default: return STATUS_NO_MEMORY;break;
    }
}

NTKERNELAPI
NTSTATUS
IoWMIWriteEvent(
    IN PVOID WnodeEventItem
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return STATUS_SUCCESS;break;
        case 1: return STATUS_UNSUCCESSFUL;break;
        case 2: return STATUS_BUFFER_OVERFLOW;break;
        default: return STATUS_INSUFFICIENT_RESOURCES;break;
    }
}

NTKERNELAPI
VOID
IoWriteErrorLogEntry(
    IN PVOID ElEntry
    )
{
}




NTKERNELAPI
NTSTATUS
IoAttachDeviceToDeviceStackSafe(
    IN PDEVICE_OBJECT SourceDevice,
    IN PDEVICE_OBJECT TargetDevice,
    IN OUT PDEVICE_OBJECT *AttachedToDeviceObject 
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return STATUS_SUCCESS;break;
        default: return STATUS_NO_SUCH_DEVICE;break;
    }
}



NTKERNELAPI
NTSTATUS
IoCheckEaBufferValidity(
    IN PFILE_FULL_EA_INFORMATION EaBuffer,
    IN ULONG EaLength,
    OUT PULONG ErrorOffset
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return STATUS_SUCCESS;break;
        default: return STATUS_EA_LIST_INCONSISTENT;break;
    }
}




NTKERNELAPI
NTSTATUS
IoCheckQuotaBufferValidity(
    IN PFILE_QUOTA_INFORMATION QuotaBuffer,
    IN ULONG QuotaLength,
    OUT PULONG ErrorOffset
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return STATUS_SUCCESS;break;
        case 1: return STATUS_DATATYPE_MISALIGNMENT;break;
        default: return STATUS_QUOTA_LIST_INCONSISTENT;break;
    }
}

NTKERNELAPI
NTSTATUS
IoCreateFileSpecifyDeviceObjectHint(
    OUT PHANDLE FileHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes,
    OUT PIO_STATUS_BLOCK IoStatusBlock,
    IN PLARGE_INTEGER AllocationSize OPTIONAL,
    IN ULONG FileAttributes,
    IN ULONG ShareAccess,
    IN ULONG Disposition,
    IN ULONG CreateOptions,
    IN PVOID EaBuffer OPTIONAL,
    IN ULONG EaLength,
    IN CREATE_FILE_TYPE CreateFileType,
    IN PVOID ExtraCreateParameters OPTIONAL,
    IN ULONG Options,
    IN PVOID DeviceObject
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return STATUS_SUCCESS;break;
        case 1: return STATUS_UNSUCCESSFUL;break;
        case 2: return STATUS_INVALID_DEVICE_OBJECT_PARAMETER;break;
        case 3: return STATUS_MOUNT_POINT_NOT_RESOLVED;break;
        default: return STATUS_OBJECT_PATH_SYNTAX_BAD;break;
    }
}




NTKERNELAPI
NTSTATUS
IoEnumerateDeviceObjectList(
    IN PDRIVER_OBJECT DriverObject,
    IN PDEVICE_OBJECT *DeviceObjectList,
    IN ULONG DeviceObjectListSize,
    OUT PULONG ActualNumberDeviceObjects
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return STATUS_SUCCESS;break;
        default: return STATUS_BUFFER_TOO_SMALL;break;
    }
}

NTKERNELAPI
NTSTATUS
IoEnumerateRegisteredFiltersList(
    IN PDRIVER_OBJECT *DriverObjectList,
    IN ULONG DriverObjectListSize,
    OUT PULONG ActualNumberDriverObjects
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return STATUS_SUCCESS;break;
        default: return STATUS_BUFFER_TOO_SMALL;break;
    }
}





NTKERNELAPI
NTSTATUS
IoGetDiskDeviceObject(
    IN PDEVICE_OBJECT FileSystemDeviceObject,
    OUT PDEVICE_OBJECT *DeviceObject
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return STATUS_INVALID_PARAMETER;break;
        case 1: return STATUS_VOLUME_DISMOUNTED;break;
        default: return STATUS_SUCCESS;break;
    }
}




NTKERNELAPI
NTSTATUS
IoGetRequestorSessionId(
    IN PIRP Irp,
    OUT PULONG pSessionId
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return STATUS_SUCCESS;break;
        default: return STATUS_UNSUCCESSFUL;break;
    }
}









NTKERNELAPI
NTSTATUS
IoQueryFileDosDeviceName(
    IN PFILE_OBJECT FileObject,
    OUT POBJECT_NAME_INFORMATION *ObjectNameInformation
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return STATUS_SUCCESS;break;
        case 1: return STATUS_UNSUCCESSFUL;break;
        default: return STATUS_INSUFFICIENT_RESOURCES;break;
    }
}





NTKERNELAPI
NTSTATUS
IoRegisterFsRegistrationChange(
    IN PDRIVER_OBJECT DriverObject,
    IN PDRIVER_FS_NOTIFICATION DriverNotificationRoutine
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return STATUS_SUCCESS;break;
        default: return STATUS_INSUFFICIENT_RESOURCES;break;
    }
}

NTKERNELAPI
NTSTATUS
IoRegisterFsRegistrationChangeEx(
    IN PDRIVER_OBJECT  DriverObject,
    IN PDRIVER_FS_NOTIFICATION  DriverNotificationRoutine
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return STATUS_SUCCESS;break;
        default: return STATUS_INSUFFICIENT_RESOURCES;break;
    }
}



NTKERNELAPI
NTSTATUS
IoSetFileOrigin(
    IN PFILE_OBJECT FileObject,
    IN BOOLEAN Remote
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return STATUS_SUCCESS;break;
        default: return STATUS_INVALID_PARAMETER_MIX;break;
    }
}







NTKERNELAPI
NTSTATUS
IoVerifyVolume(
    IN PDEVICE_OBJECT DeviceObject,
    IN BOOLEAN AllowRawMount
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return STATUS_SUCCESS;break;
        case 1: return STATUS_INSUFFICIENT_RESOURCES;break;
        case 2: return STATUS_UNSUCCESSFUL;break;
        default: return STATUS_WRONG_VOLUME;break;
    }
}






#if WINVER > 0x0500
NTKERNELAPI
NTSTATUS
IoCreateDisk(
    IN PDEVICE_OBJECT DeviceObject,
    IN PCREATE_DISK Disk
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return STATUS_SUCCESS;break;
        default: return STATUS_UNSUCCESSFUL;break;
    }
}
#endif

#if WINVER > 0x0500
NTKERNELAPI
NTSTATUS
IoReadDiskSignature(
    IN PDEVICE_OBJECT DeviceObject,
    IN ULONG BytesPerSector,
    OUT PDISK_SIGNATURE Signature
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return STATUS_SUCCESS;break;
        case 1: return STATUS_UNSUCCESSFUL;break;
        default: return STATUS_DISK_CORRUPT_ERROR;break;
    }
}
#endif

#if WINVER > 0x0500
DECLSPEC_DEPRECATED_DDK
#endif
NTKERNELAPI
NTSTATUS
#if WINVER > 0x0500
FASTCALL
#endif
IoReadPartitionTable(
    IN PDEVICE_OBJECT DeviceObject,
    IN ULONG SectorSize,
    IN BOOLEAN ReturnRecognizedPartitions,
    OUT struct _DRIVE_LAYOUT_INFORMATION **PartitionBuffer
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return STATUS_SUCCESS;break;
        default: return STATUS_UNSUCCESSFUL;break;
    }
}

NTKERNELAPI
NTSTATUS
IoReadPartitionTableEx(
    IN PDEVICE_OBJECT DeviceObject,
    IN struct _DRIVE_LAYOUT_INFORMATION_EX **PartitionBuffer
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: *PartitionBuffer=&sdv_IoReadPartitionTableEx_DRIVE_LAYOUT_INFORMATION_EX;
                return STATUS_SUCCESS;
				break;
        default: *PartitionBuffer=NULL;
                 return STATUS_UNSUCCESSFUL;
				 break;
    }
}

#if WINVER > 0x0500
DECLSPEC_DEPRECATED_DDK
#endif
NTKERNELAPI
NTSTATUS
#if WINVER > 0x0500
FASTCALL
#endif
IoSetPartitionInformation(
    IN PDEVICE_OBJECT DeviceObject,
    IN ULONG SectorSize,
    IN ULONG PartitionNumber,
    IN ULONG PartitionType
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return STATUS_SUCCESS;break;
        default: return STATUS_UNSUCCESSFUL;break;
    }
}

NTKERNELAPI
NTSTATUS
IoSetPartitionInformationEx(
    IN PDEVICE_OBJECT DeviceObject,
    IN ULONG PartitionNumber,
    IN struct _SET_PARTITION_INFORMATION_EX *PartitionInfo
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return STATUS_SUCCESS;break;
        default: return STATUS_UNSUCCESSFUL;break;
    }
}

#if WINVER > 0x0500
DECLSPEC_DEPRECATED_DDK
#endif
NTKERNELAPI
NTSTATUS
#if WINVER > 0x0500
FASTCALL
#endif
IoWritePartitionTable(
    IN PDEVICE_OBJECT DeviceObject,
    IN ULONG SectorSize,
    IN ULONG SectorsPerTrack,
    IN ULONG NumberOfHeads,
    IN struct _DRIVE_LAYOUT_INFORMATION *PartitionBuffer
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return STATUS_SUCCESS;break;
        case 1: return STATUS_UNSUCCESSFUL;break;
        case 2: return STATUS_DEVICE_NOT_READY;break;
        default: return STATUS_INSUFFICIENT_RESOURCES;break;
    }
}

NTKERNELAPI
NTSTATUS
IoWritePartitionTableEx(
    IN PDEVICE_OBJECT DeviceObject,
    IN struct _DRIVE_LAYOUT_INFORMATION_EX *PartitionBuffer
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return STATUS_SUCCESS;break;
        case 1: return STATUS_UNSUCCESSFUL;break;
        case 2: return STATUS_DEVICE_NOT_READY;break;
        default: return STATUS_INSUFFICIENT_RESOURCES;break;
    }
}


/* ..\wdm\ntddk-io.c end */

/* ..\wdm\ntddk-rtl.c begin */
/*
	Copyright (c) Microsoft Corporation.  All rights reserved.
*/

NTSYSAPI
NTSTATUS
NTAPI
RtlAnsiStringToUnicodeString(
    PUNICODE_STRING DestinationString,
    PANSI_STRING SourceString,
    BOOLEAN AllocateDestinationString
    )
{
    int x = SdvMakeChoice();
    switch (x) {
        case 0: return STATUS_SUCCESS;break;
        default: return STATUS_UNSUCCESSFUL;break;
    }
}

NTSYSAPI
VOID
NTAPI
RtlAssert(
    PVOID FailedAssertion,
    PVOID FileName,
    ULONG LineNumber,
    PCHAR Message
    ) 
{
}

NTSYSAPI                                            
NTSTATUS                                            
NTAPI                                               
RtlCharToInteger(                                  
    PCSZ String,                                    
    ULONG Base,                                     
    PULONG Value                                    
    )
{
    int x = SdvMakeChoice();
    switch (x) 
    {
        case 0: return STATUS_SUCCESS;break;
        default: return STATUS_INVALID_PARAMETER;break;
    }
}

NTSYSAPI
NTSTATUS
NTAPI
RtlCheckRegistryKey(
    IN ULONG RelativeTo,
    IN PWSTR Path
    )
{
    int x = SdvMakeChoice();
    switch (x) 
    {
        case 0: return STATUS_SUCCESS;break;
        default: return STATUS_INVALID_PARAMETER;break;
    }
}

NTSYSAPI
SIZE_T
NTAPI
RtlCompareMemory(
    const VOID *Source1,
    const VOID *Source2,
    SIZE_T Length
    )
{
    SIZE_T r = SdvKeepChoice();
    return r;
}

NTSYSAPI
VOID
NTAPI
RtlCopyMemory32(
   VOID UNALIGNED *Destination,
   CONST VOID UNALIGNED *Source,
   ULONG Length
   ) 
{  
}

NTSYSAPI
NTSTATUS
NTAPI
RtlCreateRegistryKey(
    IN ULONG RelativeTo,
    IN PWSTR Path
    )
{
    int x = SdvMakeChoice();
    switch (x) 
    {
        case 0: return STATUS_SUCCESS;break;
        default: return STATUS_UNSUCCESSFUL;break;
    }
}

NTSYSAPI
NTSTATUS
NTAPI
RtlCreateSecurityDescriptor(
    PSECURITY_DESCRIPTOR SecurityDescriptor,
    ULONG Revision
    )
{
    int x = SdvMakeChoice();
    switch (x) 
    {
        case 0: return STATUS_SUCCESS;break;
        default: return STATUS_UNKNOWN_REVISION;break;
    }
}

NTSYSAPI
NTSTATUS
NTAPI
RtlDeleteRegistryValue(
    IN ULONG RelativeTo,
    IN PCWSTR Path,
    IN PCWSTR ValueName
    )
{
    int x = SdvMakeChoice();
    switch (x) 
    {
        case 0: return STATUS_SUCCESS;break;
        default: return STATUS_UNSUCCESSFUL;break;
    }
}

NTSYSAPI
ULONG
NTAPI
RtlFindLastBackwardRunClear(
    IN PRTL_BITMAP BitMapHeader,
    IN ULONG FromIndex,
    IN PULONG StartingRunIndex
    )
{
    ULONG r = SdvKeepChoice();
    return r;
}

NTSYSAPI
CCHAR
NTAPI
RtlFindLeastSignificantBit(
    IN ULONGLONG Set
    )
{
    CCHAR r = (CCHAR) SdvKeepChoice();
    return r;
}

NTSYSAPI
CCHAR
NTAPI
RtlFindMostSignificantBit(
    IN ULONGLONG Set
    )
{
    CCHAR r = (CCHAR) SdvKeepChoice();
    return r;
}

NTSYSAPI
VOID
NTAPI
RtlFreeAnsiString(
    PANSI_STRING AnsiString
    ) 
{  
}

NTSYSAPI
VOID
NTAPI
RtlFreeUnicodeString(
    PUNICODE_STRING UnicodeString
    ) 
{ 
}

NTSYSAPI
NTSTATUS
RtlGetVersion(
    OUT PRTL_OSVERSIONINFOW lpVersionInformation
    )
{
    return STATUS_SUCCESS;
}

NTSYSAPI
NTSTATUS
NTAPI
RtlGUIDFromString(
    IN PUNICODE_STRING GuidString,
    OUT GUID* Guid
    )
{
    int x = SdvMakeChoice();
    switch (x) 
    {
        case 0: return STATUS_SUCCESS;break;
        default: return STATUS_UNSUCCESSFUL;break;
    }
}

NTSYSAPI
VOID
NTAPI
RtlInitAnsiString(
    PANSI_STRING DestinationString,
    PCSZ SourceString
    ) 
{  
}

NTSYSAPI
VOID
NTAPI
RtlInitString(
    PSTRING DestinationString,
    PCSZ SourceString
    ) 
{  
}

NTSYSAPI
VOID
NTAPI
RtlInitUnicodeString(
    IN OUT PUNICODE_STRING DestinationString,
    IN PCWSTR SourceString
    ) 
{  
    /* 
        Disable for RtlInitUnicodeString: C4090: '=' : different
        'const' qualifiers.
      
        This is correctly flagged by the compiler, and would be a serious
        issue if the harness was to be executed rather than simulated.

        However, in this case this is per design in order to simulate
        copy of data:
    */
    #pragma warning(disable:4090)

    if (DestinationString) 
    {
        DestinationString->Buffer = SourceString;
    }
    if ( SourceString == NULL)
    {
        DestinationString->Length = 0;
        DestinationString->MaximumLength = 0;
    }
     /* Enable after RtlInitUnicodeString: C4090: */
   #pragma warning(default:4090)
}

NTSYSAPI
NTSTATUS
NTAPI
RtlInitUnicodeStringEx(
    IN OUT PUNICODE_STRING DestinationString,
    IN PCWSTR SourceString
    ) 
{  
    /* 
        Disable for RtlInitUnicodeString: C4090: '=' : different
        'const' qualifiers.
      
        This is correctly flagged by the compiler, and would be a serious
        issue if the harness was to be executed rather than simulated.

        However, in this case this is per design in order to simulate
        copy of data:
    */
    int x = SdvMakeChoice();
    switch (x) 
    {
        case 0: return STATUS_NAME_TOO_LONG;break;
        default: 
        #pragma warning(disable:4090)
        if (DestinationString) 
        {
            DestinationString->Buffer = SourceString;
        
        }
        /* Enable after RtlInitUnicodeString: C4090: */
        #pragma warning(default:4090)
        return STATUS_SUCCESS;
        break;
    }
}

NTSYSAPI
NTSTATUS
NTAPI
RtlInt64ToUnicodeString(
    IN ULONGLONG Value,
    IN ULONG Base OPTIONAL,
    IN OUT PUNICODE_STRING String
    )
{
    int x = SdvMakeChoice();
    switch (x) 
    {

        case 0: return STATUS_SUCCESS;break;
#ifdef SLAM_PRECISE_STATUS
        case 1: return STATUS_BUFFER_OVERFLOW ;break;
        case 2: return STATUS_INVALID_PARAMETER ;break;
#else
        default: return STATUS_UNSUCCESSFUL;break;
#endif
    }
}

NTSYSAPI
NTSTATUS
NTAPI
RtlIntegerToUnicodeString(
    ULONG Value,
    ULONG Base,
    PUNICODE_STRING String
    )
{
    int x = SdvMakeChoice();
    switch (x) 
    {

        case 0: return STATUS_SUCCESS;break;
#ifdef SLAM_PRECISE_STATUS
        case 1: return STATUS_BUFFER_OVERFLOW ;break;
        case 2: return STATUS_INVALID_PARAMETER ;break;
#else
        default: return STATUS_UNSUCCESSFUL;break;
#endif
    }
}

NTSYSAPI
ULONG
NTAPI
RtlLengthSecurityDescriptor(
    PSECURITY_DESCRIPTOR SecurityDescriptor
    )
{
    ULONG r = SdvKeepChoice();
    return r;
}

NTSYSAPI
VOID
NTAPI
RtlMapGenericMask(
    PACCESS_MASK AccessMask,
    PGENERIC_MAPPING GenericMapping
    ) 
{  
}

NTSYSAPI
VOID
NTAPI
sdv_RtlMoveMemory(
    VOID UNALIGNED *Destination,
    CONST VOID UNALIGNED *Source,
    SIZE_T Length
    ) 
{
}

NTSYSAPI
NTSTATUS
NTAPI
RtlQueryRegistryValues(
    IN ULONG RelativeTo,
    IN PCWSTR Path,
    IN PRTL_QUERY_REGISTRY_TABLE QueryTable,
    IN PVOID Context,
    IN PVOID Environment OPTIONAL
    )
{
    int x = SdvMakeChoice();
    switch (x) 
    {
        case 0: return STATUS_SUCCESS;break;
#ifdef SLAM_PRECISE_STATUS
        case 1: return STATUS_INVALID_PARAMETER;break;
        case 2: return STATUS_OBJECT_NAME_NOT_FOUND;break;
#endif
        default: return STATUS_UNSUCCESSFUL;break;
    }
}

NTSYSAPI
NTSTATUS
NTAPI
RtlSetDaclSecurityDescriptor(
    PSECURITY_DESCRIPTOR SecurityDescriptor,
    BOOLEAN DaclPresent,
    PACL Dacl,
    BOOLEAN DaclDefaulted
    )
{
    int x = SdvMakeChoice();
    switch (x) 
    {
        case 0: return STATUS_SUCCESS;break;
        case 1: return STATUS_UNKNOWN_REVISION;break;
        default: return STATUS_INVALID_SECURITY_DESCR;break;
    }
}

NTSYSAPI
NTSTATUS
NTAPI
RtlStringFromGUID(
    IN REFGUID Guid,
    OUT PUNICODE_STRING GuidString
    )
{
    int x = SdvMakeChoice();
    switch (x) 
    {
        case 0: return STATUS_SUCCESS;break;
        default: return STATUS_NO_MEMORY;break;
    }
}

ULONG
FASTCALL
RtlU_longByteSwap(
    IN ULONG Source
    )
{
    ULONG r = SdvKeepChoice();
    return r;
}

ULONGLONG
FASTCALL
RtlU_longlongByteSwap(
    IN ULONGLONG Source
    )
{
    ULONGLONG r = SdvKeepChoice();
    return r;
}

USHORT
FASTCALL
RtlU_shortByteSwap(
    IN USHORT Source
    )
{
    USHORT r = (USHORT) SdvKeepChoice();
    return r;
}

NTSYSAPI
CHAR
NTAPI
RtlUpperChar(
    CHAR Character
    )
{
    CHAR r = (CHAR) SdvKeepChoice();
    return r;
}

NTSYSAPI
BOOLEAN
NTAPI
RtlValidRelativeSecurityDescriptor(
    IN PSECURITY_DESCRIPTOR SecurityDescriptorInput,
    IN ULONG SecurityDescriptorLength,
    IN SECURITY_INFORMATION RequiredInformation
    )
{
    LONG choice = SdvMakeChoice();
    switch(choice) 
    {
        case 0:
            return FALSE;
            break;
        default:
            return TRUE;
            break;
    }
}

NTSYSAPI
BOOLEAN
NTAPI
RtlValidSecurityDescriptor(
    PSECURITY_DESCRIPTOR SecurityDescriptor
    )
{
    LONG choice = SdvMakeChoice();
    switch(choice) 
    {
        case 0:
            return FALSE;
            break;
        default:
            return TRUE;
            break;
    }
}

NTSYSAPI
NTSTATUS
RtlVerifyVersionInfo(
    IN PRTL_OSVERSIONINFOEXW VersionInfo,
    IN ULONG TypeMask,
    IN ULONGLONG  ConditionMask
    )
{
    int x = SdvMakeChoice();
    switch (x) 
    {
        case 0: return STATUS_SUCCESS;break;
        case 1: return STATUS_INVALID_PARAMETER ;break;
        default: return STATUS_REVISION_MISMATCH ;break;
    }
}

NTSYSAPI
NTSTATUS
NTAPI
RtlVolumeDeviceToDosName(
    IN  PVOID           VolumeDeviceObject,
    OUT PUNICODE_STRING DosName
    )
{
    return STATUS_SUCCESS;
}

NTSYSAPI
ULONG
NTAPI
RtlWalkFrameChain(
    OUT PVOID *Callers,
    IN ULONG Count,
    IN ULONG Flags
    ) 
{ 
    ULONG r = SdvKeepChoice(); 
    return r; 
}

NTSYSAPI
NTSTATUS
NTAPI
RtlWriteRegistryValue(
    IN ULONG RelativeTo,
    IN PCWSTR Path,
    IN PCWSTR ValueName,
    IN ULONG ValueType,
    IN PVOID ValueData,
    IN ULONG ValueLength
    )
{
    NTSTATUS r = SdvMakeChoice();
    switch (r) {
        case 0: return STATUS_SUCCESS;break;
#ifdef SLAM_PRECISE_STATUS
        case 1: return STATUS_ACCESS_DENIED;break;
        case 2: return STATUS_INVALID_HANDLE;break;
        case 3: return STATUS_INVALID_PARAMETER;break;
        case 4: return STATUS_NO_MEMORY;break;
        case 5: return STATUS_BUFFER_TOO_SMALL;break;
        case 6: return STATUS_INVALID_SID;break;
        case 7: return STATUS_BUFFER_OVERFLOW;break;
#endif
        default: return STATUS_UNSUCCESSFUL;break;
  }
}

NTSYSAPI
VOID
NTAPI
sdv_RtlZeroMemory(
    VOID UNALIGNED *Destination,
    SIZE_T Length
    ) 
{
}
/* ..\wdm\ntddk-rtl.c end */

/* ..\wdm\ntddk-zw.c begin */
/*
	Copyright (c) Microsoft Corporation.  All rights reserved.
*/



int sdv_ZwCreateKey_1;
int sdv_ZwCreateKey_2;
static int sdv_ZwCreateKey_count = 0;

int sdv_ZwOpenKey_1;
int sdv_ZwOpenKey_2;
static int sdv_ZwOpenKey_count = 0;




NTSTATUS
ZwCancelTimer(
    IN HANDLE TimerHandle,
    OUT PBOOLEAN CurrentState OPTIONAL
    )
{
    int x = SdvMakeChoice();
    switch (x) 
    { 
        case 0: return STATUS_SUCCESS;break;
#ifdef SLAM_PRECISE_STATUS
        case 1: return STATUS_OBJECT_TYPE_MISMATCH;break;
        case 2: return STATUS_ACCESS_DENIED;break;
        case 3: return STATUS_INVALID_HANDLE;break;
#endif
        default: return STATUS_UNSUCCESSFUL;break;
    }
}

NTSYSAPI
NTSTATUS
NTAPI
ZwClose(
    IN HANDLE Handle
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return STATUS_SUCCESS;break;
        case 1: return STATUS_UNSUCCESSFUL;break;
        case 2: return STATUS_INVALID_HANDLE;break;
        default: return STATUS_HANDLE_NOT_CLOSABLE;break;
    }
}

NTSYSAPI
NTSTATUS
NTAPI
ZwCreateDirectoryObject(
    OUT PHANDLE DirectoryHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return STATUS_SUCCESS;break;
        case 1: return STATUS_UNSUCCESSFUL;break;
        case 2: return STATUS_ACCESS_DENIED;break;
        case 3: return STATUS_ACCESS_VIOLATION;break;
        default: return STATUS_DATATYPE_MISALIGNMENT;break;
    }
}

NTSYSAPI
NTSTATUS
NTAPI
ZwCreateFile(
    OUT PHANDLE FileHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes,
    OUT PIO_STATUS_BLOCK IoStatusBlock,
    IN PLARGE_INTEGER AllocationSize OPTIONAL,
    IN ULONG FileAttributes,
    IN ULONG ShareAccess,
    IN ULONG CreateDisposition,
    IN ULONG CreateOptions,
    IN PVOID EaBuffer OPTIONAL,
    IN ULONG EaLength
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return STATUS_SUCCESS;break;
        default: return STATUS_UNSUCCESSFUL;break;
    }
}

NTSYSAPI
NTSTATUS
NTAPI
ZwCreateKey(
    OUT PHANDLE KeyHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes,
    IN ULONG TitleIndex,
    IN PUNICODE_STRING Class OPTIONAL,
    IN ULONG CreateOptions,
    OUT PULONG Disposition OPTIONAL
    )
{
    
    int choice = SdvMakeChoice();
    switch (choice) 
    {
        case 0: 
        switch (sdv_ZwCreateKey_count) 
        {
          case 0:
              SdvAssume(sdv_ZwCreateKey_count == 0);
              sdv_ZwCreateKey_count++;
              *KeyHandle = (HANDLE) &sdv_ZwCreateKey_1;
              break;
          default:
              SdvAssume(sdv_ZwCreateKey_count != 0);
              *KeyHandle = (HANDLE) &sdv_ZwCreateKey_2;
              break;
        }
        return STATUS_SUCCESS;
        break;
        #ifdef SLAM_PRECISE_STATUS
        case 1: return STATUS_OBJECT_NAME_NOT_FOUND;break;
        case 2: return STATUS_INVALID_PARAMETER;break;
        case 3: return STATUS_BUFFER_TOO_SMALL;break;
        case 4: return STATUS_TOO_LATE;break;
        case 5: return STATUS_INVALID_PARAMETER_4;break;
        case 6: return STATUS_OBJECT_TYPE_MISMATCH;break;
        case 7: return STATUS_ACCESS_DENIED;break;
        case 8: return STATUS_INVALID_HANDLE;break;
        case 9: return STATUS_TOO_LATE;break;
        #endif
        default: return STATUS_UNSUCCESSFUL;break;
    }
}

NTKERNELAPI
NTSTATUS
ZwCreateSection(
    OUT PHANDLE SectionHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL,
    IN PLARGE_INTEGER MaximumSize OPTIONAL,
    IN ULONG SectionPageProtection,
    IN ULONG AllocationAttributes,
    IN HANDLE FileHandle OPTIONAL
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return STATUS_SUCCESS;break;
        case 1: return STATUS_UNSUCCESSFUL;break;
        case 2: return STATUS_FILE_LOCK_CONFLICT;break;
        case 3: return STATUS_INVALID_FILE_FOR_SECTION;break;
        case 4: return STATUS_INVALID_PAGE_PROTECTION;break;
        case 5: return STATUS_MAPPED_FILE_SIZE_ZERO;break;
        default: return STATUS_SECTION_TOO_BIG;break;
    }
}

NTSTATUS
ZwCreateTimer(
    OUT PHANDLE TimerHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL,
    IN TIMER_TYPE TimerType
    )
{
    int choice = SdvMakeChoice();
    switch (choice) 
    {
        #ifdef SLAM_PRECISE_STATUS
        case 0: return STATUS_SUCCESS;break;
        case 1: return STATUS_INVALID_PARAMETER_4;break;
        case 2: return STATUS_INSUFFICIENT_RESOURCES;break;
        case 3: return STATUS_PRIVILEGE_NOT_HELD;break;
        case 4: return STATUS_OBJECT_NAME_INVALID;break;
        case 5: return STATUS_INSUFFICIENT_RESOURCES;break;
        case 6: return STATUS_INVALID_SID;break;
        case 7: return STATUS_INVALID_ACL;break;
        case 8: return STATUS_UNKNOWN_REVISION;break;
        #endif
        default: return STATUS_INVALID_PARAMETER;break;
    }
}

NTSYSAPI
NTSTATUS
NTAPI
ZwDeleteKey(
    IN HANDLE KeyHandle
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return STATUS_SUCCESS;break;
        case 1: return STATUS_UNSUCCESSFUL;break;
        case 2: return STATUS_ACCESS_DENIED;break;
        default: return STATUS_INVALID_HANDLE;break;
    }
}

NTKERNELAPI
NTSTATUS
ZwDeleteValueKey(
    IN HANDLE KeyHandle,
    IN PUNICODE_STRING ValueName
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return STATUS_SUCCESS;break;
        default: return STATUS_UNSUCCESSFUL;break;
    }
}

NTSYSAPI
NTSTATUS
NTAPI
ZwEnumerateKey(
    IN HANDLE KeyHandle,
    IN ULONG Index,
    IN KEY_INFORMATION_CLASS KeyInformationClass,
    OUT PVOID KeyInformation,
    IN ULONG Length,
    OUT PULONG ResultLength
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return STATUS_SUCCESS;break;
        case 1: return STATUS_UNSUCCESSFUL;break;
        case 2: return STATUS_BUFFER_OVERFLOW;break;
        case 3: return STATUS_BUFFER_TOO_SMALL;break;
        case 4: return STATUS_INVALID_PARAMETER;break;
        default: return STATUS_NO_MORE_ENTRIES;break;
    }
}

NTSYSAPI
NTSTATUS
NTAPI
ZwEnumerateValueKey(
    IN HANDLE KeyHandle,
    IN ULONG Index,
    IN KEY_VALUE_INFORMATION_CLASS KeyValueInformationClass,
    OUT PVOID KeyValueInformation,
    IN ULONG Length,
    OUT PULONG ResultLength
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return STATUS_SUCCESS;break;
        case 1: return STATUS_UNSUCCESSFUL;break;
        case 2: return STATUS_BUFFER_OVERFLOW;break;
        case 3: return STATUS_BUFFER_TOO_SMALL;break;
        case 4: return STATUS_INVALID_PARAMETER;break;
        default: return STATUS_NO_MORE_ENTRIES;break;
    }
}

NTSYSAPI
NTSTATUS
NTAPI
ZwFlushKey(
    IN HANDLE KeyHandle
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return STATUS_SUCCESS;break;
        default: return STATUS_UNSUCCESSFUL;break;
    }
}

NTSYSAPI
NTSTATUS
NTAPI
ZwMakeTemporaryObject(
    IN HANDLE Handle
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return STATUS_SUCCESS;break;
        default: return STATUS_UNSUCCESSFUL;break;
    }
}

NTSYSAPI
NTSTATUS
NTAPI
ZwMapViewOfSection(
    IN HANDLE SectionHandle,
    IN HANDLE ProcessHandle,
    IN OUT PVOID *BaseAddress,
    IN ULONG_PTR ZeroBits,
    IN SIZE_T CommitSize,
    IN OUT PLARGE_INTEGER SectionOffset OPTIONAL,
    IN OUT PSIZE_T ViewSize,
    IN SECTION_INHERIT InheritDisposition,
    IN ULONG AllocationType,
    IN ULONG Win32Protect
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return STATUS_SUCCESS;break;
        case 1: return STATUS_UNSUCCESSFUL;break;
        case 2: return STATUS_CONFLICTING_ADDRESSES;break;
        case 3: return STATUS_INVALID_PAGE_PROTECTION;break;
        default: return STATUS_SECTION_PROTECTION;break;
    }
}

NTSYSAPI
NTSTATUS
NTAPI
ZwOpenFile(
    OUT PHANDLE FileHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes,
    OUT PIO_STATUS_BLOCK IoStatusBlock,
    IN ULONG ShareAccess,
    IN ULONG OpenOptions
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return STATUS_SUCCESS;break;
        default: return STATUS_UNSUCCESSFUL;break;
    }
}

NTSYSAPI
NTSTATUS
NTAPI
ZwOpenKey(
    OUT PHANDLE KeyHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes
    )
{
    int choice = SdvMakeChoice();
    switch (choice) 
    {
        case 0: 
        switch (sdv_ZwOpenKey_count) 
        {
          case 0:
              SdvAssume(sdv_ZwOpenKey_count == 0);
              sdv_ZwOpenKey_count++;
              *KeyHandle = (HANDLE) &sdv_ZwOpenKey_1;
              break;
          default: 
              SdvAssume(sdv_ZwOpenKey_count != 0);
              *KeyHandle = (HANDLE) &sdv_ZwOpenKey_2;
              break;
        }
        return STATUS_SUCCESS;
        break;
        #ifdef SLAM_PRECISE_STATUS
        case 1: return STATUS_TOO_LATE;break;
        case 2: return STATUS_INVALID_PARAMETER_4;break;
        case 3: return STATUS_INVALID_PARAMETER;break;
        case 4: return STATUS_INSUFFICIENT_RESOURCES;break;
        case 5: return STATUS_OBJECT_PATH_SYNTAX_BAD;break;
        case 6: return STATUS_NO_MEMORY;break;
        case 7: return STATUS_INVALID_INFO_CLASS;break;
        #endif
        default: return STATUS_PRIVILEGE_NOT_HELD;break;
    }
    
}

NTSYSAPI
NTSTATUS
NTAPI
ZwOpenSection(
    OUT PHANDLE SectionHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return STATUS_SUCCESS;break;
        case 1: return STATUS_UNSUCCESSFUL;break;
        case 2: return STATUS_ACCESS_DENIED;break;
        default: return STATUS_INVALID_HANDLE;break;
    }
}

NTSYSAPI
NTSTATUS
NTAPI
ZwOpenSymbolicLinkObject(
    OUT PHANDLE LinkHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return STATUS_SUCCESS;break;
        default: return STATUS_UNSUCCESSFUL;break;
    }
}

NTSTATUS
ZwOpenTimer(
    OUT PHANDLE TimerHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes
    )
{

    int choice = SdvMakeChoice();
    switch (choice) 
    {
        case 0: return STATUS_SUCCESS;break;
        #ifdef SLAM_PRECISE_STATUS
        case 1: return STATUS_INSUFFICIENT_RESOURCES;break;
        case 2: return STATUS_OBJECT_NAME_INVALID;break;
        case 3: return STATUS_OBJECT_PATH_SYNTAX_BAD;break;
        case 4: return STATUS_OBJECT_TYPE_MISMATCH;break;
        case 5: return STATUS_OBJECT_NAME_NOT_FOUND;break;
        case 6: return STATUS_OBJECT_NAME_INVALID;break;
        case 7: return OBJ_NAME_PATH_SEPARATOR;break;
        case 8: return STATUS_NO_MEMORY;break;
        case 9: return STATUS_INVALID_SID;break;
        case 10: return STATUS_INVALID_ACL;break;
        case 11: return STATUS_UNKNOWN_REVISION;break;
        case 12: return STATUS_REPARSE;break;
        case 13: return STATUS_REPARSE_OBJECT;break;
        #endif
        default: return STATUS_INVALID_PARAMETER;break;
    }
}

NTSYSAPI
NTSTATUS
NTAPI
ZwQueryInformationFile(
    IN HANDLE FileHandle,
    OUT PIO_STATUS_BLOCK IoStatusBlock,
    OUT PVOID FileInformation,
    IN ULONG Length,
    IN FILE_INFORMATION_CLASS FileInformationClass
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return STATUS_SUCCESS;break;
        default: return STATUS_UNSUCCESSFUL;break;
    }
}

NTSYSAPI
NTSTATUS
NTAPI
ZwQueryKey(
    IN HANDLE KeyHandle,
    IN KEY_INFORMATION_CLASS KeyInformationClass,
    OUT PVOID KeyInformation,
    IN ULONG Length,
    OUT PULONG ResultLength
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return STATUS_SUCCESS;break;
        case 1: return STATUS_UNSUCCESSFUL;break;
        case 2: return STATUS_BUFFER_OVERFLOW;break;
        case 3: return STATUS_BUFFER_TOO_SMALL;break;
        default: return STATUS_INVALID_PARAMETER;break;
    }
}

NTSYSAPI
NTSTATUS
NTAPI
ZwQuerySymbolicLinkObject(
    IN HANDLE LinkHandle,
    IN OUT PUNICODE_STRING LinkTarget,
    OUT PULONG ReturnedLength OPTIONAL
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return STATUS_SUCCESS;break;
        default: return STATUS_BUFFER_TOO_SMALL;break;
    }
}

NTSYSAPI
NTSTATUS
NTAPI
ZwQueryValueKey(
    IN HANDLE KeyHandle,
    IN PUNICODE_STRING ValueName,
    IN KEY_VALUE_INFORMATION_CLASS KeyValueInformationClass,
    OUT PVOID KeyValueInformation,
    IN ULONG Length,
    OUT PULONG ResultLength
    )
{
    /* 
      This model for ZwQueryValueKey has a custom body that supports a
      common pattern in drivers: To initially call ZwQueryValueKey
      with Length==0 in order to determine a ResultLenght x, then
      allocate this amount x of memory and thereafter make a
      subsequent call to ZwQueryValueKey with new Length==x.  In the
      first case the driver can assume that STATUS_SUCCESS is not
      returned.
    */
    int choice = SdvMakeChoice();

    if(Length!=0) {
        switch (choice) {
            case 0: return STATUS_SUCCESS;break;
            case 1: return STATUS_UNSUCCESSFUL;break;
            case 2: return STATUS_BUFFER_OVERFLOW;break;
            case 3: return STATUS_BUFFER_TOO_SMALL;break;
            default: return STATUS_INVALID_PARAMETER;break;
        }
    }
    else {
        switch (choice) {
            case 0: return STATUS_UNSUCCESSFUL;break;
            case 1: return STATUS_BUFFER_OVERFLOW;break;
            case 2: return STATUS_BUFFER_TOO_SMALL;break;
            default: return STATUS_INVALID_PARAMETER;break;
        }
    }
}

NTSYSAPI
NTSTATUS
NTAPI
ZwReadFile(
    IN HANDLE FileHandle,
    IN HANDLE Event OPTIONAL,
    IN PIO_APC_ROUTINE ApcRoutine OPTIONAL,
    IN PVOID ApcContext OPTIONAL,
    OUT PIO_STATUS_BLOCK IoStatusBlock,
    OUT PVOID Buffer,
    IN ULONG Length,
    IN PLARGE_INTEGER ByteOffset OPTIONAL,
    IN PULONG Key OPTIONAL
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return STATUS_SUCCESS;break;
        default: return STATUS_UNSUCCESSFUL;break;
    }
}

NTSYSAPI
NTSTATUS
NTAPI
ZwSetInformationFile(
    IN HANDLE FileHandle,
    OUT PIO_STATUS_BLOCK IoStatusBlock,
    IN PVOID FileInformation,
    IN ULONG Length,
    IN FILE_INFORMATION_CLASS FileInformationClass
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return STATUS_SUCCESS;break;
        default: return STATUS_UNSUCCESSFUL;break;
    }
}

NTSYSAPI
NTSTATUS
NTAPI
ZwSetInformationThread(
    IN HANDLE ThreadHandle,
    IN THREADINFOCLASS ThreadInformationClass,
    IN PVOID ThreadInformation,
    IN ULONG ThreadInformationLength
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return STATUS_SUCCESS;break;
        case 1: return STATUS_UNSUCCESSFUL;break;
        case 2: return STATUS_INFO_LENGTH_MISMATCH;break;
        default: return STATUS_INVALID_PARAMETER;break;
    }
}

NTSTATUS
ZwSetTimer(
    IN HANDLE TimerHandle,
    IN PLARGE_INTEGER DueTime,
    IN PTIMER_APC_ROUTINE TimerApcRoutine OPTIONAL,
    IN PVOID TimerContext OPTIONAL,
    IN BOOLEAN WakeTimer,
    IN LONG Period OPTIONAL,
    OUT PBOOLEAN PreviousState OPTIONAL
    )
{
    int choice = SdvMakeChoice();
    switch (choice) 
    {
        case 0: return STATUS_SUCCESS;break;
        #ifdef SLAM_PRECISE_STATUS
        case 1: return STATUS_INVALID_PARAMETER_6;break;
        case 2: return STATUS_TIMER_RESUME_IGNORED;break;
        case 3: return STATUS_OBJECT_TYPE_MISMATCH;break;
        case 4: return STATUS_ACCESS_DENIED;break;
        case 5: return STATUS_INVALID_HANDLE;break;
        case 6: return STATUS_UNSUCCESSFUL;break;
        case 7: return STATUS_INVALID_PARAMETER;break;
        case 8: return STATUS_INSUFFICIENT_RESOURCES;break;
        #endif
        default: return STATUS_INSUFFICIENT_RESOURCES;break;
    }
}

NTSYSAPI
NTSTATUS
NTAPI
ZwSetValueKey(
    IN HANDLE KeyHandle,
    IN PUNICODE_STRING ValueName,
    IN ULONG TitleIndex OPTIONAL,
    IN ULONG Type,
    IN PVOID Data,
    IN ULONG DataSize
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return STATUS_SUCCESS;break;
        case 1: return STATUS_UNSUCCESSFUL;break;
        case 2: return STATUS_ACCESS_DENIED;break;
        default: return STATUS_INVALID_HANDLE;break;
    }
}

NTSYSAPI
NTSTATUS
NTAPI
ZwUnmapViewOfSection(
    IN HANDLE ProcessHandle,
    IN PVOID BaseAddress
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return STATUS_SUCCESS;break;
        case 1: return STATUS_UNSUCCESSFUL;break;
        default: return STATUS_INVALID_PARAMETER;break;
    }
}

NTSYSAPI
NTSTATUS
NTAPI
ZwWriteFile(
    IN HANDLE FileHandle,
    IN HANDLE Event OPTIONAL,
    IN PIO_APC_ROUTINE ApcRoutine OPTIONAL,
    IN PVOID ApcContext OPTIONAL,
    OUT PIO_STATUS_BLOCK IoStatusBlock,
    IN PVOID Buffer,
    IN ULONG Length,
    IN PLARGE_INTEGER ByteOffset OPTIONAL,
    IN PULONG Key OPTIONAL
    )
{
    int choice = SdvMakeChoice();
    switch (choice) {
        case 0: return STATUS_SUCCESS;break;
        default: return STATUS_UNSUCCESSFUL;break;
    }
}


/* ..\wdm\ntddk-zw.c end */

/* ..\wdm\ntddk-ke.c begin */
/*
	Copyright (c) Microsoft Corporation.  All rights reserved.
*/

#if WINVER > 0x0500
_DECL_HAL_KE_IMPORT
VOID
FASTCALL
KeAcquireInStackQueuedSpinLock(
    IN PKSPIN_LOCK SpinLock,
    IN PKLOCK_QUEUE_HANDLE LockHandle
    ) 
{
    SDV_IRQL_PUSH(DISPATCH_LEVEL);
}
#endif

#if WINVER > 0x0500
NTKERNELAPI
VOID
FASTCALL
KeAcquireInStackQueuedSpinLockAtDpcLevel(
    IN PKSPIN_LOCK SpinLock,
    IN PKLOCK_QUEUE_HANDLE LockHandle
    ) 
{
}
#endif

#if WINVER > 0x0500
NTKERNELAPI
VOID
FASTCALL
KeAcquireInStackQueuedSpinLockForDpc(
    IN PKSPIN_LOCK SpinLock,
    IN PKLOCK_QUEUE_HANDLE LockHandle
    ) 
{
    SDV_IRQL_PUSH(DISPATCH_LEVEL);
}
#endif

KIRQL
KeAcquireInterruptSpinLock(
    IN PKINTERRUPT Interrupt
    )
{
    SDV_IRQL_PUSH(SDV_DIRQL);
    return sdv_irql_previous;
}

VOID
sdv_KeAcquireSpinLock(
    IN PKSPIN_LOCK SpinLock,
    OUT PKIRQL p_old_irql
    ) 
{
    SDV_IRQL_PUSH(DISPATCH_LEVEL);
    (*p_old_irql) = sdv_irql_previous;
}

VOID
sdv_KeAcquireSpinLockAtDpcLevel(
    IN PKSPIN_LOCK  SpinLock
    ) 
{
}

NTKERNELAPI
KIRQL
KeAcquireSpinLockRaiseToDpc(
    IN PKSPIN_LOCK SpinLock
    ) 
{ 
    SDV_IRQL_PUSH(DISPATCH_LEVEL);
    return sdv_irql_previous;
}

NTKERNELAPI
DECLSPEC_NORETURN
VOID
NTAPI
KeBugCheck (
    __in ULONG BugCheckCode
    )
{
    /* Simulate bug check by stopping verification: */
    SdvExit();
}

NTKERNELAPI
DECLSPEC_NORETURN
VOID
NTAPI
KeBugCheckEx(
    __in ULONG BugCheckCode,
    __in ULONG_PTR BugCheckParameter1,
    __in ULONG_PTR BugCheckParameter2,
    __in ULONG_PTR BugCheckParameter3,
    __in ULONG_PTR BugCheckParameter4
    )
{
    /* Simulate bug check by stopping verification: */
    SdvExit();
}

NTKERNELAPI
VOID
KeClearEvent(
    IN PRKEVENT Event
    ) 
{
}

NTKERNELAPI                                         
NTSTATUS                                            
KeDelayExecutionThread(                            
    IN KPROCESSOR_MODE WaitMode,                    
    IN BOOLEAN Alertable,                           
    IN PLARGE_INTEGER Interval                      
    )
{
    int x = SdvMakeChoice();
    switch (x) {
        case 0: return STATUS_SUCCESS;break;
#ifdef SLAM_PRECISE_STATUS
        case 1: return STATUS_ALERTED;break;
		case 2: return STATUS_USER_APC;break;
#endif
        default: return STATUS_UNSUCCESSFUL;break;

  }
}

NTKERNELAPI
VOID
sdv_KeEnterCriticalRegion(
    VOID
    )
{
}

NTKERNELAPI
VOID
sdv_KeFlushIoBuffers(
    IN PMDL Mdl,
    IN BOOLEAN ReadOperation,
    IN BOOLEAN DmaOperation
    )
{
}

NTHALAPI
KIRQL
NTAPI
sdv_KeGetCurrentIrql() 
{
    return sdv_irql_current;
}

NTKERNELAPI
VOID
KeInitializeDpc(
    OUT PRKDPC Dpc,
    IN PKDEFERRED_ROUTINE DeferredRoutine,
    IN PVOID DeferredContext
    ) 
{
   Dpc->DeferredRoutine = DeferredRoutine;
}

NTKERNELAPI
VOID
KeInitializeEvent(
    IN PRKEVENT Event,
    IN EVENT_TYPE Type,
    IN BOOLEAN State
    )
{ 
}

NTKERNELAPI
BOOLEAN
KeInsertByKeyDeviceQueue(
    IN PKDEVICE_QUEUE DeviceQueue,
    IN PKDEVICE_QUEUE_ENTRY DeviceQueueEntry,
    IN ULONG SortKey
    )
{
    LONG choice = SdvMakeChoice();
    switch(choice) 
    {
        case 0:
            return FALSE;
            break;
        default:
            return TRUE;
            break;
    }
}

NTKERNELAPI
BOOLEAN
KeInsertDeviceQueue(
    IN PKDEVICE_QUEUE DeviceQueue,
    IN PKDEVICE_QUEUE_ENTRY DeviceQueueEntry
    )
{
    LONG choice = SdvMakeChoice();
    switch(choice) 
    {
        case 0:
            return FALSE;
            break;
        default:
            return TRUE;
            break;
    }
}

NTKERNELAPI
BOOLEAN
KeInsertQueueDpc(
    OUT PRKDPC Dpc,
    IN PVOID SystemArgument1,
    IN PVOID SystemArgument2
    )
{
    LONG choice = SdvMakeChoice();
    switch(choice) 
    {
        case 0:
            return FALSE;
            break;
        default:
            sdv_kdpc3=Dpc;
            sdv_dpc_ke_registered=TRUE; 
            return TRUE;
            break;
    }
}
NTKERNELAPI
VOID
sdv_KeLeaveCriticalRegion(
    VOID
    )
{
}

VOID
sdv_KeLowerIrql(
    IN KIRQL new_irql
    ) 
{  
    SDV_IRQL_POPTO(new_irql);
}

NTKERNELAPI
LONG
KePulseEvent(
    IN PRKEVENT Event,
    IN KPRIORITY Increment,
    IN BOOLEAN Wait
    )
{
    LONG l = SdvKeepChoice();
    return l;
}

VOID
sdv_KeRaiseIrql(
    IN KIRQL new_irql,
    OUT PKIRQL p_old_irql
    ) 
{  
    SDV_IRQL_PUSH(new_irql);
    *p_old_irql = sdv_irql_previous;
}

KIRQL
KeRaiseIrqlToDpcLevel(
    VOID
    ) 
{ 
    SDV_IRQL_PUSH(DISPATCH_LEVEL);
    return sdv_irql_previous;
}

KIRQL
sdv_KeRaiseIrqlToSynchLevel(
    VOID
    ) 
{ 
    KIRQL r = (KIRQL) SdvMakeChoice(); 
    return r; 
}

NTKERNELAPI
LONG
KeReadStateEvent(
    IN PRKEVENT Event
    )
{
    LONG l = SdvKeepChoice();
    return l;
}

#if WINVER > 0x0500
_DECL_HAL_KE_IMPORT
VOID
FASTCALL
KeReleaseInStackQueuedSpinLock(
    IN PKLOCK_QUEUE_HANDLE LockHandle
    ) 
{
    SDV_IRQL_POP();
}
#endif

#if WINVER > 0x0500
NTKERNELAPI
VOID
FASTCALL
KeReleaseInStackQueuedSpinLockForDpc(
    IN PKLOCK_QUEUE_HANDLE LockHandle
    ) 
{
    SDV_IRQL_POP();
}
#endif

#if WINVER > 0x0500
NTKERNELAPI
VOID
FASTCALL
KeReleaseInStackQueuedSpinLockFromDpcLevel(
    IN PKLOCK_QUEUE_HANDLE LockHandle
    ) 
{
}
#endif

VOID
KeReleaseInterruptSpinLock(
    IN PKINTERRUPT Interrupt,
    IN KIRQL old_irql
    )
{
    SDV_IRQL_POPTO(old_irql);
}

NTKERNELAPI
LONG
KeReleaseSemaphore(
    IN PRKSEMAPHORE Semaphore,
    IN KPRIORITY Increment,
    IN LONG Adjustment,
    IN BOOLEAN Wait
    ) 
{
    LONG r = SdvKeepChoice(); 
    return r;
}

VOID
sdv_KeReleaseSpinLock(
    IN PKSPIN_LOCK  SpinLock,
    IN KIRQL  new_irql
    ) 
{
    SDV_IRQL_POPTO(new_irql);
}

VOID
sdv_KeReleaseSpinLockFromDpcLevel(
    IN PKSPIN_LOCK  SpinLock
    ) 
{
}

NTKERNELAPI
LONG
KeResetEvent(
    IN PRKEVENT Event
    ) 
{
    LONG l = SdvKeepChoice();
    return l;
}

NTKERNELAPI
LONG
KeSetEvent(
    IN PRKEVENT Event,
    IN KPRIORITY Increment,
    IN BOOLEAN Wait
    ) 
{
    LONG r = SdvKeepChoice(); 
    return r;
}

BOOLEAN
KeSynchronizeExecution(
    IN PKINTERRUPT Interrupt,
    IN PKSYNCHRONIZE_ROUTINE SynchronizeRoutine,
    IN PVOID SynchronizeContext
    )
{
    BOOLEAN b;

    /* SynchronizeRoutine must be non-null.  Check anyhow. */
    if ( SynchronizeRoutine == NULL )
    {
        return FALSE;
    }

    SDV_IRQL_PUSH(SDV_DIRQL);
    b = SynchronizeRoutine( SynchronizeContext );
    SDV_IRQL_POP();
    return b;
}

NTKERNELAPI
NTSTATUS
KeWaitForMultipleObjects(
    IN ULONG Count,
    IN PVOID Object[],
    IN WAIT_TYPE WaitType,
    IN KWAIT_REASON WaitReason,
    IN KPROCESSOR_MODE WaitMode,
    IN BOOLEAN Alertable,
    IN PLARGE_INTEGER Timeout OPTIONAL,
    IN PKWAIT_BLOCK WaitBlockArray OPTIONAL
    )  
{
    int x = SdvMakeChoice();
      
    switch (x) { 
        case 0: return STATUS_SUCCESS;break;
#ifdef SLAM_PRECISE_STATUS
        case 1: return STATUS_ALERTED;break;
        case 2: return STATUS_USER_APC;break;
        default: return STATUS_TIMEOUT;break;
#else
        default: return STATUS_UNSUCCESSFUL;break;
#endif
    }
}

NTKERNELAPI
NTSTATUS
KeWaitForMutexObject(
    IN PRKMUTEX Mutex,
    IN KWAIT_REASON WaitReason,
    IN KPROCESSOR_MODE WaitMode,
    IN BOOLEAN Alertable,
    IN PLARGE_INTEGER Timeout OPTIONAL
    )
{

    int x = SdvMakeChoice();

    switch (x) {
        case 0: return STATUS_SUCCESS;break;
#ifdef SLAM_PRECISE_STATUS
        case 1: return STATUS_ALERTED;break;
        case 2: return STATUS_USER_APC;break;
        default: return STATUS_TIMEOUT;break;
#else
        default: return STATUS_UNSUCCESSFUL;break;
#endif
    }
}

NTKERNELAPI
NTSTATUS
KeWaitForSingleObject(
    IN PVOID Object,
    IN KWAIT_REASON WaitReason,
    IN KPROCESSOR_MODE WaitMode,
    IN BOOLEAN Alertable,
    IN PLARGE_INTEGER Timeout OPTIONAL
    )
{

    int x = SdvMakeChoice();

    switch (x) {
        case 0: return STATUS_SUCCESS;break;
#ifdef SLAM_PRECISE_STATUS
        case 1: return STATUS_ALERTED;break;
        case 2: return STATUS_USER_APC;break;
        default: return STATUS_TIMEOUT;break;
#else
        default: return STATUS_UNSUCCESSFUL;break;
#endif
    }
}

BOOLEAN  
sdv_KeTryToAcquireSpinLockAtDpcLevel(
    IN PKSPIN_LOCK  SpinLock)
{
    LONG choice = SdvMakeChoice();
    switch(choice) 
    {
        case 0:
            return FALSE;
            break;
        default:
            return TRUE;
            break;
    }
}



/* ..\wdm\ntddk-ke.c end */

/* ..\wdm\ntddk-mm.c begin */
/*
	Copyright (c) Microsoft Corporation.  All rights reserved.
*/

NTKERNELAPI
PVOID
MmAllocateContiguousMemory(
    IN SIZE_T NumberOfBytes,
    IN PHYSICAL_ADDRESS HighestAcceptableAddress
    ) 
{
    int x = SdvMakeChoice();
    switch (x) {
        case 0: return malloc(1);break;
        default: return NULL;break;
    }
}

ULONG
sdv_MmGetMdlByteCount(
    IN PMDL Mdl
     )
{
    /* Suppress C4311: 'type cast' : pointer truncation .
       This is acceptable as the OS Model will not be executed:*/
    #pragma warning(disable:4305)
    ULONG x = ((ULONG)malloc(1));
    #pragma warning(default:4305)
    return  x;
}

ULONG
sdv_MmGetMdlByteOffset (
    IN PMDL Mdl
    )
{
    /* Suppress C4311: 'type cast' : pointer truncation .
       This is acceptable as the OS Model will not be executed:*/
    #pragma warning(disable:4305)
    ULONG x = ((ULONG)malloc(1));
    #pragma warning(default:4305)
    return x;
}


PPFN_NUMBER
sdv_MmGetMdlPfnArray(
    IN PMDL Mdl
    )
{
	PPFN_NUMBER x=(PPFN_NUMBER) malloc(1);
	return x;
}


PVOID
sdv_MmGetMdlVirtualAddress(
    IN PMDL Mdl
     )
{
	PVOID x= (PVOID) malloc(1); 
	return x;
}

PVOID
sdv_MmGetSystemAddressForMdlSafe(
     IN PMDL MDL,
     IN MM_PAGE_PRIORITY PRIORITY
     ) 
{
    int choice = SdvMakeChoice();
    PVOID p;
    switch (choice) 
    {
        case 0: p= (PVOID) malloc(1);
                return p;
                break;
        default:return NULL;
                break;
    }
}
PVOID
sdv_MmLockPagableCodeSection(
    IN PVOID  AddressWithinSection
    ) 
{
    return(AddressWithinSection);
}

NTKERNELAPI
VOID
MmLockPagableSectionByHandle(
    IN PVOID ImageSectionHandle
    )
{
}

int sdv_MmMapIoSpace_int=0;  

NTKERNELAPI
PVOID
MmMapIoSpace(
    IN PHYSICAL_ADDRESS PhysicalAddress,
    IN SIZE_T NumberOfBytes,
    IN MEMORY_CACHING_TYPE CacheType
    )
{
    return (PVOID) &sdv_MmMapIoSpace_int;
}

NTKERNELAPI
PVOID
MmMapLockedPages(
    IN PMDL MemoryDescriptorList,
    IN KPROCESSOR_MODE AccessMode
    ) 
{
	PVOID r= (PVOID) malloc(1);
	return r;
}

NTKERNELAPI
PVOID
MmMapLockedPagesSpecifyCache(
     IN PMDL MemoryDescriptorList,
     IN KPROCESSOR_MODE AccessMode,
     IN MEMORY_CACHING_TYPE CacheType,
     IN PVOID BaseAddress,
     IN ULONG BugCheckOnFailure,
     IN MM_PAGE_PRIORITY Priority
     ) 
{
	PVOID r= (PVOID) malloc(1);
	return r;
}


NTKERNELAPI
PVOID
MmPageEntireDriver(
    IN PVOID AddressWithinSection
    )
{
	PVOID p= (PVOID) malloc(1);
	return p;
}

VOID
sdv_MmPrepareMdlForReuse(
    IN PMDL Mdl
    )
{
}

NTKERNELAPI
VOID
MmResetDriverPaging(
    IN PVOID AddressWithinSection
    )
{
}

NTKERNELAPI
VOID
MmUnlockPagableImageSection(
    IN PVOID ImageSectionHandle
    )
{
}
/* ..\wdm\ntddk-mm.c end */

/* ..\wdm\ntddk-ex.c begin */
/*
	Copyright (c) Microsoft Corporation.  All rights reserved.
*/

NTKERNELAPI
VOID
FASTCALL
sdv_ExAcquireFastMutex(
    IN PFAST_MUTEX FastMutex
    ) 
{  
    SDV_IRQL_PUSH(APC_LEVEL);
}

NTKERNELAPI
VOID
FASTCALL
ExAcquireFastMutexUnsafe(
    IN PFAST_MUTEX  FastMutex
    ) 
{
}

NTKERNELAPI
BOOLEAN
ExAcquireResourceExclusiveLite(
    IN PERESOURCE Resource,
    IN BOOLEAN Wait
    ) 
{ 
    if (Wait) { return TRUE; }
    else {
        LONG choice = SdvMakeChoice();
        switch(choice) 
        {
            case 0:
                return FALSE;
                break;
            default:
                return TRUE;
                break;
        }
        
    };
}

NTKERNELAPI
BOOLEAN
ExAcquireResourceSharedLite(
    IN PERESOURCE Resource,
    IN BOOLEAN Wait
    ) 
{
    if (Wait) { return TRUE; }
    else {
        LONG choice = SdvMakeChoice();
        switch(choice) 
        {
            case 0:
                return FALSE;
                break;
            default:
                return TRUE;
                break;
        }
    };
}

NTKERNELAPI
BOOLEAN
ExAcquireSharedStarveExclusive(
    IN PERESOURCE Resource,
    IN BOOLEAN Wait
    ) 
{ 
    if (Wait) { return TRUE; }
    else {
        LONG choice = SdvMakeChoice();
        switch(choice) 
        {
            case 0:
                return FALSE;
                break;
            default:
                return TRUE;
                break;
        }
    };
}

NTKERNELAPI
BOOLEAN
ExAcquireSharedWaitForExclusive(
    IN PERESOURCE Resource,
    IN BOOLEAN Wait
    ) 
{ 
    if (Wait) { return TRUE; }
    else {
        LONG choice = SdvMakeChoice();
        switch(choice) 
        {
            case 0:
                return FALSE;
                break;
            default:
                return TRUE;
                break;
        }
    };
}

NTKERNELAPI
PVOID
sdv_ExAllocatePool(
    IN POOL_TYPE PoolType,
    IN SIZE_T NumberOfBytes
    ) 
{ 
  int x = SdvMakeChoice();
  switch (x) 
  {
      case 0: return malloc(1);break;
      default: return NULL;break;
  }
    
}

NTKERNELAPI
PVOID
sdv_ExAllocatePoolWithQuota(
    IN POOL_TYPE PoolType,
    IN SIZE_T NumberOfBytes
    )
{
  int x = SdvMakeChoice();
  switch (x) 
  {
      case 0: return malloc(1);break;
      default: return NULL;break;
  }
}

NTKERNELAPI
PVOID
sdv_ExAllocatePoolWithQuotaTag(
    IN POOL_TYPE PoolType,
    IN SIZE_T NumberOfBytes,
    IN ULONG Tag
    )
{
  int x = SdvMakeChoice();
  switch (x) 
  {
      case 0: return malloc(1);break;
      default: return NULL;break;
  }
}

NTKERNELAPI
PVOID
NTAPI
ExAllocatePoolWithTag(
    IN POOL_TYPE PoolType,
    IN SIZE_T NumberOfBytes,
    IN ULONG Tag
    ) 
{
  int x = SdvMakeChoice();
  switch (x) 
  {
      case 0: return malloc(1);break;
      default: return NULL;break;
  }
}

NTKERNELAPI
PVOID
NTAPI
ExAllocatePoolWithTagPriority(
    IN POOL_TYPE PoolType,
    IN SIZE_T NumberOfBytes,
    IN ULONG Tag,
    IN EX_POOL_PRIORITY Priority
    )
{
  int x = SdvMakeChoice();
  switch (x) 
  {
      case 0: return malloc(1);break;
      default: return NULL;break;
  }
}

NTKERNELAPI
NTSTATUS
ExCreateCallback(
    OUT PCALLBACK_OBJECT *CallbackObject,
    IN POBJECT_ATTRIBUTES ObjectAttributes,
    IN BOOLEAN Create,
    IN BOOLEAN AllowMultipleCallbacks
    ) 
{ 
    int x = SdvMakeChoice();
    switch (x) {
        case 0: return STATUS_SUCCESS;break;
        default: return STATUS_UNSUCCESSFUL;break;
    }
}

NTKERNELAPI
NTSTATUS
ExDeleteResourceLite(
    IN PERESOURCE Resource
    ) 
{ 
    int x = SdvMakeChoice();
    if (x) {
        return STATUS_SUCCESS;
    } else {
        return STATUS_UNSUCCESSFUL;
    }
}

NTKERNELAPI
VOID
NTAPI
sdv_ExFreePool(
    IN PVOID P
    ) 
{
}

VOID
sdv_ExInitializeFastMutex(
    IN PFAST_MUTEX FastMutex
    )
{
}

NTKERNELAPI
NTSTATUS
ExInitializeResourceLite(
    IN PERESOURCE Resource
    ) 
{ 
    return STATUS_SUCCESS; 
}

NTKERNELAPI
PLIST_ENTRY
FASTCALL
sdv_ExInterlockedInsertHeadList(
    IN PLIST_ENTRY ListHead,
    IN PLIST_ENTRY ListEntry,
    IN PKSPIN_LOCK Lock
    ) 
{
	PVOID p;
	return p;
}

NTKERNELAPI
PLIST_ENTRY
FASTCALL
sdv_ExInterlockedInsertTailList(
    IN PLIST_ENTRY ListTail,
    IN PLIST_ENTRY ListEntry,
    IN PKSPIN_LOCK Lock
    ) 
{
	PVOID p;
	return p;
}

NTKERNELAPI
PSINGLE_LIST_ENTRY
FASTCALL
sdv_ExInterlockedPushEntryList(
    IN PSINGLE_LIST_ENTRY ListHead,
    IN PSINGLE_LIST_ENTRY ListEntry,
    IN PKSPIN_LOCK Lock
    )
{
	PVOID p;
	return p;
}

NTKERNELAPI
DECLSPEC_NORETURN
VOID
ExRaiseAccessViolation (
    VOID
    )
{
    /* As SDV does not support SEH: Stop verification: */
    /* This is unsound but is useful for suppressing false defects */ 
    SdvExit();
}

NTKERNELAPI
DECLSPEC_NORETURN
VOID
ExRaiseDatatypeMisalignment (
    VOID
    )
{
    /* As SDV does not support SEH: Stop verification: */
    /* This is unsound but is useful for suppressing false defects */ 
    SdvExit();
}

NTKERNELAPI
DECLSPEC_NORETURN
VOID
NTAPI
ExRaiseStatus (
    __in NTSTATUS Status
    )
{
    /* As SDV does not support SEH: Stop verification: */
    /* This is unsound but is useful for suppressing false defects */ 
    SdvExit();
}

NTKERNELAPI
NTSTATUS
ExReinitializeResourceLite(
    IN PERESOURCE Resource
    ) 
{ 
    return STATUS_SUCCESS; 
}

NTKERNELAPI
VOID
FASTCALL
sdv_ExReleaseFastMutex(
    IN PFAST_MUTEX FastMutex
    ) 
{  
    SDV_IRQL_POP();
}

NTKERNELAPI
VOID
FASTCALL
ExReleaseFastMutexUnsafe(
    IN PFAST_MUTEX  FastMutex
    ) 
{
}
    
NTKERNELAPI
VOID
ExReleaseResourceForThreadLite(
    IN PERESOURCE Resource,
    IN ERESOURCE_THREAD ResourceThreadId
    ) 
{
}

NTKERNELAPI
VOID
FASTCALL
ExReleaseResourceLite(
    IN PERESOURCE Resource    
    ) 
{
}

NTHALAPI
BOOLEAN
FASTCALL
sdv_ExTryToAcquireFastMutex(
    IN PFAST_MUTEX FastMutex
    ) 
{ 
    LONG choice = SdvMakeChoice();
    switch(choice) 
    {
        case 0:
            return FALSE;
            break;
        default:
            return TRUE;
            break;
    }
}

NTKERNELAPI
NTSTATUS
ExUuidCreate(
    OUT UUID *Uuid
    ) 
{ 
    int x = SdvMakeChoice();
    switch (x) {
        case 0: return STATUS_SUCCESS;break;
        default: return STATUS_RETRY;break;
    }
}
/* ..\wdm\ntddk-ex.c end */

/* ..\wdm\ntddk-misc.c begin */
/*
	Copyright (c) Microsoft Corporation.  All rights reserved.
*/

PCHAR sdv_containing_record(
    IN PCHAR Address
    )
{
    PCHAR record = (PCHAR) malloc(1);
    SdvAssume(record != NULL && record != Address);
    return record;
}

VOID
NTAPI
DbgBreakPoint(
    VOID
    )
{
}

NTSYSAPI
VOID
NTAPI
DbgBreakPointWithStatus(
    IN ULONG Status
    )
{
}

VOID
sdv_InitializeObjectAttributes(
     OUT POBJECT_ATTRIBUTES p,
     IN PUNICODE_STRING n,
     IN ULONG a,
     IN HANDLE r,
     IN PSECURITY_DESCRIPTOR s
     )
{
}

NTKERNELAPI
LONG
FASTCALL
sdv_InterlockedDecrement(
    IN LONG *Addend
    )
{
    (*Addend)--;
    return *Addend;
}

NTKERNELAPI
LONG
FASTCALL
sdv_InterlockedIncrement(
    IN LONG *Addend
    )
{
    (*Addend)++;
    return *Addend;
}

NTKERNELAPI
VOID
NTAPI
ProbeForRead(
    IN CONST VOID *Address,
    IN SIZE_T Length,
    IN ULONG Alignment
    )
{
}

NTKERNELAPI
VOID
NTAPI
ProbeForWrite (
    IN PVOID Address,
    IN SIZE_T Length,
    IN ULONG Alignment
    )
{
}

ULONGLONG
NTAPI
VerSetConditionMask(
        IN  ULONGLONG   ConditionMask,
        IN  ULONG   TypeMask,
        IN  UCHAR   Condition
    )
{
    ULONGLONG r = SdvKeepChoice();
    return r;
}



BOOLEAN sdv_NT_ERROR (
    NTSTATUS Status
    )
{
    int choice = SdvKeepChoice();
    if( 0 <= Status && Status <= 0x1FFFFFFF ) {
        return FALSE ;
    } else {
      return choice ? TRUE : FALSE ;
    }
}
/* ..\wdm\ntddk-misc.c end */

/* harness-part.c begin */


int                     notused_4MiniportAdapterContext;
NDIS_HANDLE             MiniportAdapterContext      = (NDIS_HANDLE) &notused_4MiniportAdapterContext;

int                     notused_4NetBufferList;
PNET_BUFFER_LIST        NetBufferList               = (PNET_BUFFER_LIST) &notused_4NetBufferList;

int                     notused_4MiniportInterruptContext;
NDIS_HANDLE             MiniportInterruptContext    = (NDIS_HANDLE) &notused_4MiniportInterruptContext;

int                     notused_4Context;
PVOID                   Context                     = (PVOID) &notused_4Context;


BOOLEAN                 AddressingReset= (BOOLEAN) 0;

int                     notused_4MiniportRestartParameters;
PNDIS_MINIPORT_RESTART_PARAMETERS MiniportRestartParameters = (PNDIS_MINIPORT_RESTART_PARAMETERS) &notused_4MiniportRestartParameters;

int                     notused_4MiniportPauseParameters;
PNDIS_MINIPORT_PAUSE_PARAMETERS MiniportPauseParameters = (PNDIS_MINIPORT_PAUSE_PARAMETERS) &notused_4MiniportPauseParameters;

int                     notused_4NdisMiniportHandle;
NDIS_HANDLE             NdisMiniportHandle = (NDIS_HANDLE) &notused_4NdisMiniportHandle;

int                     notused_4MiniportDriverContext;
NDIS_HANDLE             MiniportDriverContext = (NDIS_HANDLE) &notused_4MiniportDriverContext;

int                     notused_4MiniportInitParameters;
PNDIS_MINIPORT_INIT_PARAMETERS MiniportInitParameters = (PNDIS_MINIPORT_INIT_PARAMETERS) &notused_4MiniportInitParameters;

NDIS_HALT_ACTION        HaltAction      = (NDIS_HALT_ACTION) 0;

int                     notused_4OidRequest;
PNDIS_OID_REQUEST       OidRequest      = (PNDIS_OID_REQUEST) &notused_4OidRequest;

NDIS_SHUTDOWN_ACTION    ShutdownAction  = (NDIS_SHUTDOWN_ACTION) 0;

int                     notused_4NetDevicePnPEvent;
PNET_DEVICE_PNP_EVENT   NetDevicePnPEvent = (PNET_DEVICE_PNP_EVENT) &notused_4NetDevicePnPEvent;

int                     notused_4CancelId;
PVOID                   CancelId        = (PVOID) &notused_4CancelId;

int                     notused_4RequestId;
PVOID                   RequestId       = (PVOID) &notused_4RequestId;

NDIS_PORT_NUMBER        PortNumber      = (NDIS_PORT_NUMBER) 0;
ULONG                   SendFlags       = 0;

ULONG                   ReturnFlags     = (ULONG) 0;

BOOLEAN					        notused_4QueueDefaultInterruptDpc;
PBOOLEAN				        QueueDefaultInterruptDpc = &notused_4QueueDefaultInterruptDpc;

int                     notused_4TargetProcessors;
PULONG                  TargetProcessors = (PULONG) &notused_4TargetProcessors;

int                     notused_4MiniportDpcContext;
PVOID                   MiniportDpcContext = (PVOID) &notused_4MiniportDpcContext;

int                     notused_4NdisReserved1;
PVOID                   NdisReserved1   = (PVOID) &notused_4NdisReserved1;

int                     notused_4NdisReserved2;
PVOID                   NdisReserved2   = (PVOID) &notused_4NdisReserved2;

int                     notused_4DriverObject;
PDRIVER_OBJECT          DriverObject    = (PDRIVER_OBJECT) &notused_4DriverObject;

int                     notused_4NdisDriverHandle;
NDIS_HANDLE             NdisDriverHandle = (NDIS_HANDLE) &notused_4NdisDriverHandle;

int                     notused_4DriverContext;
NDIS_HANDLE             DriverContext   = (NDIS_HANDLE) &notused_4DriverContext;

int                     notused_4DO;
PDEVICE_OBJECT          pDO             = (PDEVICE_OBJECT) &notused_4DO;

int         notused_4Reserved;
PVOID       Reserved        = (PVOID) &notused_4Reserved;

int         notused_4SGL;
PSCATTER_GATHER_LIST    pSGL            = (PSCATTER_GATHER_LIST) &notused_4SGL;

int         notused_4VirtualAddress;
PVOID       VirtualAddress  = (PVOID) &notused_4VirtualAddress;

int         notused_4PhysicalAddress;
PNDIS_PHYSICAL_ADDRESS  PhysicalAddress = (PNDIS_PHYSICAL_ADDRESS) &notused_4PhysicalAddress;

ULONG        Length                     = 0;

ULONG        MessageId                  = 0;

int          notused_4MiniportAddDeviceContext;
NDIS_HANDLE  MiniportAddDeviceContext   = (NDIS_HANDLE) &notused_4MiniportAddDeviceContext;

int          notused_4Irp;
PIRP         Irp                        = (IRP *) &notused_4Irp;

int          notused_4WorkItemContext;
PVOID        WorkItemContext            = (PVOID) &notused_4WorkItemContext;

int          notused_4NdisIoWorkItemHandle;
NDIS_HANDLE  NdisIoWorkItemHandle       = (NDIS_HANDLE) &notused_4NdisIoWorkItemHandle;

int          notused_4SynchronizeContext;
PVOID        SynchronizeContext         = (PVOID) &notused_4SynchronizeContext;

int          notused_4SystemSpecific1;
PVOID        SystemSpecific1     = (PVOID) &notused_4SystemSpecific1;

int          notused_4FunctionContext;
PVOID        FunctionContext            = (PVOID) &notused_4FunctionContext;

int          notused_4SystemSpecific2;
PVOID        SystemSpecific2			      = (PVOID) &notused_4SystemSpecific2;

int          notused_4SystemSpecific3;
PVOID        SystemSpecific3            = (PVOID) &notused_4SystemSpecific3;
/* harness-part.c end */

/* harness-main.c begin */
/*
    Copyright (c) Microsoft Corporation.  All rights reserved.
*/

int notused_4p_AdapterContext;
NDIS_HANDLE  g_AdapterContext = (NDIS_HANDLE) &notused_4p_AdapterContext;

int notuset_4p_TimerObject;
NDIS_HANDLE g_TimerObject = (NDIS_HANDLE) &notuset_4p_TimerObject;

NDIS_STATUS
sdv_NDIS_DoNothing()
{
    int dummy = SdvKeepChoice();
    return dummy;
}

NDIS_STATUS
sdv_SetReturnNDISStatus()
{
    LONG choice = SdvMakeChoice();

    switch(choice) {
        case 0:
            return (NDIS_STATUS) NDIS_STATUS_FAILURE;
            break;
        case 1:
            return (NDIS_STATUS) NDIS_STATUS_NOT_SUPPORTED;
            break;
        default:
            return (NDIS_STATUS) NDIS_STATUS_SUCCESS;
            break;
    }
}

/*PVOID
sdv_SetReturnAlloc()
{
    LONG choice = SdvMakeChoice();

    switch(choice) {
        case 0:
            return (PVOID) malloc(1);
            break;
        case 1:
        default:
            return (PVOID) NULL;
            break;
    }
}*/

#define Sdv_SetReturnAlloc \
    LONG choice = SdvMakeChoice();\
    switch(choice) {\
        case 0:\
            return (PVOID) malloc(1);\
            break;\
        default:\
            return (PVOID) NULL;\
            break;\
    }\

BOOLEAN
sdv_SetReturnBool()
{
    LONG choice = SdvMakeChoice();

    switch(choice) {
        case 0:
            return TRUE;
            break;
        default:
            return FALSE;
            break;
    }
}

#define sdv_SetReturnNDISHandle \
    LONG choice = SdvMakeChoice();\
    switch(choice) {\
        case 0:\
            return (NDIS_HANDLE) malloc(1);\
            break;\
        default:\
            return (NDIS_HANDLE) NULL;\
            break;\
    }\

/*NDIS_HANDLE
sdv_SetReturnNDISHandle()
{
    LONG choice = SdvMakeChoice();

    switch(choice) {
        case 0:
            return (NDIS_HANDLE) malloc(1);
            break;
        default:
            return (NDIS_HANDLE) NULL;
            break;
    }
}*/



NDIS_STATUS
sdv_MINIPORT_RESET(
    IN NDIS_HANDLE MiniportAdapterContext,
    OUT PBOOLEAN AddressingReset
)
{
    NDIS_STATUS status;
#ifdef fun_MINIPORT_RESET

    int irql = SdvMakeChoice();

    switch(irql) {
      case 0:
        SDV_IRQL_PUSH(PASSIVE_LEVEL);
        break;
    default:
        SDV_IRQL_PUSH(DISPATCH_LEVEL);
		break;
    };

    status = fun_MINIPORT_RESET(MiniportAdapterContext, AddressingReset);

#else
    status = sdv_NDIS_DoNothing();
#endif
    return status;
}

NDIS_STATUS
sdv_MINIPORT_PAUSE(
    IN NDIS_HANDLE  MiniportAdapterContext,
        IN PNDIS_MINIPORT_PAUSE_PARAMETERS  MiniportPauseParameters
)
{
    NDIS_STATUS status;
#ifdef fun_MINIPORT_PAUSE

    SDV_IRQL_PUSH(PASSIVE_LEVEL);

    status = fun_MINIPORT_PAUSE(MiniportAdapterContext,MiniportPauseParameters);

#else
    status = sdv_NDIS_DoNothing();
#endif
    return status;
}

NDIS_STATUS
sdv_MINIPORT_RESTART(
    IN NDIS_HANDLE  MiniportAdapterContext,
    IN PNDIS_MINIPORT_RESTART_PARAMETERS  MiniportRestartParameters
)
{
    NDIS_STATUS status;
#ifdef fun_MINIPORT_RESTART

    SDV_IRQL_PUSH(PASSIVE_LEVEL);

    status = fun_MINIPORT_RESTART(MiniportAdapterContext, MiniportRestartParameters);

#else
    status = sdv_NDIS_DoNothing();
#endif
    return status;
}

NDIS_STATUS
sdv_MINIPORT_INITIALIZE(
    IN  NDIS_HANDLE                         NdisMiniportHandle,
    IN  NDIS_HANDLE                         MiniportDriverContext,
    IN  PNDIS_MINIPORT_INIT_PARAMETERS      MiniportInitParameters
    )
{
    NDIS_STATUS status;
#ifdef fun_MINIPORT_INITIALIZE

    SDV_IRQL_PUSH(PASSIVE_LEVEL);

    status = fun_MINIPORT_INITIALIZE(
                    NdisMiniportHandle,
                    MiniportDriverContext,
                    MiniportInitParameters);

#else
    status = sdv_NDIS_DoNothing();
#endif
    return status;
}


VOID
sdv_MINIPORT_HALT(
    IN  NDIS_HANDLE             MiniportAdapterContext,
    IN  NDIS_HALT_ACTION        HaltAction
    )
{
#ifdef fun_MINIPORT_HALT
    
    SDV_IRQL_PUSH(PASSIVE_LEVEL);

    fun_MINIPORT_HALT(
        MiniportAdapterContext,
        HaltAction);

#endif
}
    

NDIS_STATUS
sdv_MINIPORT_OID_REQUEST(
    IN  NDIS_HANDLE             MiniportAdapterContext,
    IN  PNDIS_OID_REQUEST       OidRequest
    )
{
    NDIS_STATUS status;
#ifdef fun_MINIPORT_OID_REQUEST
    
    int irql = SdvMakeChoice();

    switch(irql) {
      case 0:
        SDV_IRQL_PUSH(PASSIVE_LEVEL);
        break;
    default:
        SDV_IRQL_PUSH(DISPATCH_LEVEL);
		break;
    };

    status = fun_MINIPORT_OID_REQUEST(
                    MiniportAdapterContext,
                    OidRequest);
#else
    status = sdv_NDIS_DoNothing();
#endif
    return status;
}
    
VOID
sdv_MINIPORT_SHUTDOWN(
    IN  NDIS_HANDLE             MiniportAdapterContext,
    IN  NDIS_SHUTDOWN_ACTION    ShutdownAction
    )
{
#ifdef fun_MINIPORT_SHUTDOWN
    
    int irql = SdvMakeChoice();
    NDIS_SHUTDOWN_ACTION    LocalShutdownAction = ShutdownAction;

    switch(irql) {
      case 0:
        LocalShutdownAction = NdisShutdownBugCheck;
        SDV_IRQL_PUSH(PASSIVE_LEVEL);
        break;
      case 2:
        LocalShutdownAction = NdisShutdownBugCheck;
        SDV_IRQL_PUSH(DISPATCH_LEVEL);
        break;
      case 3:
        LocalShutdownAction = NdisShutdownBugCheck;
        SDV_IRQL_PUSH(SDV_DIRQL);
        break;
      default:
        LocalShutdownAction = NdisShutdownPowerOff;
        SDV_IRQL_PUSH(PASSIVE_LEVEL);
		break;
    };

    fun_MINIPORT_SHUTDOWN(
                    MiniportAdapterContext,
                    LocalShutdownAction);
#endif
}

VOID
sdv_MINIPORT_DEVICE_PNP_EVENT_NOTIFY(
    IN  NDIS_HANDLE             MiniportAdapterContext,
    IN  PNET_DEVICE_PNP_EVENT   NetDevicePnPEvent
    )
{
#ifdef fun_MINIPORT_DEVICE_PNP_EVENT_NOTIFY
    
    SDV_IRQL_PUSH(PASSIVE_LEVEL);

    fun_MINIPORT_DEVICE_PNP_EVENT_NOTIFY(
        MiniportAdapterContext,
        NetDevicePnPEvent);
    
#endif
}


VOID
sdv_MINIPORT_CANCEL_SEND(
    IN  NDIS_HANDLE             MiniportAdapterContext,
    IN  PVOID                   CancelId
    )
{
#ifdef fun_MINIPORT_CANCEL_SEND
       
    int irql = SdvMakeChoice();

    switch(irql) {
      case 0:
        SDV_IRQL_PUSH(PASSIVE_LEVEL);
        break;
      default:
        SDV_IRQL_PUSH(DISPATCH_LEVEL);
		break;
    };

    fun_MINIPORT_CANCEL_SEND(
        MiniportAdapterContext,
        CancelId);
#endif
}


BOOLEAN
sdv_MINIPORT_CHECK_FOR_HANG(
    IN  NDIS_HANDLE             MiniportAdapterContext
    )
{
    BOOLEAN ret = FALSE;
#ifdef fun_MINIPORT_CHECK_FOR_HANG
        
    int irql = SdvMakeChoice();

    switch(irql) {
      case 0:
        SDV_IRQL_PUSH(PASSIVE_LEVEL);
        break;
      default:
        SDV_IRQL_PUSH(DISPATCH_LEVEL);
        break;
    };

    ret = fun_MINIPORT_CHECK_FOR_HANG(
                MiniportAdapterContext);

#endif
    return ret;
}



VOID
sdv_MINIPORT_CANCEL_OID_REQUEST(
    IN NDIS_HANDLE              MiniportAdapterContext,
    IN PVOID                    RequestId
    )
    {
#ifdef fun_MINIPORT_CANCEL_OID_REQUEST
            
    int irql = SdvMakeChoice();

    switch(irql) {
      case 0:
        SDV_IRQL_PUSH(PASSIVE_LEVEL);
        break;
     default:
        SDV_IRQL_PUSH(DISPATCH_LEVEL);
		break;
    };

    fun_MINIPORT_CANCEL_OID_REQUEST(
                MiniportAdapterContext,
                RequestId);
                    
#endif
}


VOID
sdv_MINIPORT_SEND_NET_BUFFER_LISTS(
    IN  NDIS_HANDLE             MiniportAdapterContext,
    IN  PNET_BUFFER_LIST        NetBufferList,
    IN  NDIS_PORT_NUMBER        PortNumber,
    IN  ULONG                   SendFlags
    )
{
#ifdef fun_MINIPORT_SEND_NET_BUFFER_LISTS
        
    int irql = SdvMakeChoice();

    switch(irql) {
      case 0:
        SDV_IRQL_PUSH(PASSIVE_LEVEL);
        break;
    default:
        SDV_IRQL_PUSH(DISPATCH_LEVEL);
		break;
    };

    fun_MINIPORT_SEND_NET_BUFFER_LISTS(
            MiniportAdapterContext,
            NetBufferList,
            PortNumber,
            SendFlags);    
#endif
}


VOID
sdv_MINIPORT_RETURN_NET_BUFFER_LISTS(
    IN  NDIS_HANDLE             MiniportAdapterContext,
    IN  PNET_BUFFER_LIST        NetBufferList,
    IN  ULONG                   ReturnFlags
    )
{
#ifdef fun_MINIPORT_RETURN_NET_BUFFER_LISTS
        
    int irql = SdvMakeChoice();

    switch(irql) {
      case 0:
        SDV_IRQL_PUSH(PASSIVE_LEVEL);
        break;
      default:
        SDV_IRQL_PUSH(DISPATCH_LEVEL);
		break;
    };

    fun_MINIPORT_RETURN_NET_BUFFER_LISTS(
            MiniportAdapterContext,
            NetBufferList,
            ReturnFlags);
#endif
}
   
VOID
sdv_MINIPORT_UNLOAD(
    IN PDRIVER_OBJECT   DriverObject
    )
{
#ifdef fun_MINIPORT_UNLOAD
    
    SDV_IRQL_PUSH(PASSIVE_LEVEL);

    fun_MINIPORT_UNLOAD (DriverObject);

#endif
}


NDIS_STATUS
sdv_MINIPORT_SET_OPTIONS(
    IN  NDIS_HANDLE             NdisDriverHandle,
    IN  NDIS_HANDLE             DriverContext
    )
{
    NDIS_STATUS status;
#ifdef fun_MINIPORT_SET_OPTIONS

    SDV_IRQL_PUSH(PASSIVE_LEVEL);

    status = fun_MINIPORT_SET_OPTIONS(
                    NdisDriverHandle,
                    DriverContext);

#else
    status = sdv_NDIS_DoNothing();
#endif
    return status;
}


VOID
sdv_MINIPORT_PROCESS_SG_LIST(
    IN  PDEVICE_OBJECT          pDO,
    IN  PVOID                   Reserved,
    IN  PSCATTER_GATHER_LIST    pSGL,
    IN  PVOID                   Context
    )
{
#ifdef fun_MINIPORT_PROCESS_SG_LIST

    SDV_IRQL_PUSH(DISPATCH_LEVEL);

    fun_MINIPORT_PROCESS_SG_LIST(
                    pDO,
                    Reserved,
                    pSGL,
                    Context);

#endif
}

VOID
sdv_MINIPORT_ALLOCATE_SHARED_MEM_COMPLETE(
    IN  NDIS_HANDLE             MiniportAdapterContext,
    IN  PVOID                   VirtualAddress,
    IN  PNDIS_PHYSICAL_ADDRESS  PhysicalAddress,
    IN  ULONG                   Length,
    IN  PVOID                   Context
    )
{
#ifdef fun_MINIPORT_ALLOCATE_SHARED_MEM_COMPLETE

    SDV_IRQL_PUSH(PASSIVE_LEVEL);

    fun_MINIPORT_ALLOCATE_SHARED_MEM_COMPLETE(
                    MiniportAdapterContext,
                    VirtualAddress,
                    PhysicalAddress,
                    Length,
                    Context);

#endif
}



VOID
sdv_MINIPORT_ADD_DEVICE(
    IN NDIS_HANDLE  MiniportAdapterHandle, 
    IN NDIS_HANDLE  MiniportDriverContext
    )
{
#ifdef fun_MINIPORT_ADD_DEVICE

    SDV_IRQL_PUSH(PASSIVE_LEVEL);

    fun_MINIPORT_ADD_DEVICE( 
                    MiniportAdapterHandle,
                    MiniportDriverContext );

#endif
}

VOID
sdv_MINIPORT_START_DEVICE(
    IN NDIS_HANDLE  MiniportAddDeviceContext,
    IN PIRP  Irp
    )
{
#ifdef fun_MINIPORT_START_DEVICE

    SDV_IRQL_PUSH(PASSIVE_LEVEL);
    
    fun_MINIPORT_START_DEVICE(
        MiniportAddDeviceContext,
        Irp);
#endif
}

VOID
sdv_MINIPORT_REMOVE_DEVICE(
    IN NDIS_HANDLE  MiniportAddDeviceContext
    )
{
#ifdef fun_MINIPORT_REMOVE_DEVICE

    SDV_IRQL_PUSH(PASSIVE_LEVEL);

    fun_MINIPORT_REMOVE_DEVICE( MiniportAddDeviceContext );

#endif
}

NDIS_STATUS 
sdv_MINIPORT_FILTER_RESOURCE_REQUIREMENTS(
    IN NDIS_HANDLE  MiniportAddDeviceContext,
    IN PIRP  Irp
    )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
#ifdef fun_MINIPORT_FILTER_RESOURCE_REQUIREMENTS

    SDV_IRQL_PUSH(PASSIVE_LEVEL);
    
    status = fun_MINIPORT_FILTER_RESOURCE_REQUIREMENTS(
        MiniportAddDeviceContext,
        Irp);
#endif
    return status;
}


VOID
sdv_NDIS_IO_WORKITEM_FUNCTION(
    IN PVOID WorkItemContext, 
    IN NDIS_HANDLE  NdisIoWorkItemHandle)
{
    int choice = SdvMakeChoice();

    SDV_IRQL_PUSH(PASSIVE_LEVEL);

    switch (choice)
    {
        case 1:
#ifdef fun_NDIS_IO_WORKITEM_FUNCTION_1
      fun_NDIS_IO_WORKITEM_FUNCTION_1(WorkItemContext, NdisIoWorkItemHandle);
#endif
        break;

        case 2: 
#ifdef fun_NDIS_IO_WORKITEM_FUNCTION_2

      fun_NDIS_IO_WORKITEM_FUNCTION_2(WorkItemContext, NdisIoWorkItemHandle);
#endif
        break;

        case 3: 
#ifdef fun_NDIS_IO_WORKITEM_FUNCTION_3
      fun_NDIS_IO_WORKITEM_FUNCTION_3(WorkItemContext, NdisIoWorkItemHandle);
#endif
        break;

        case 4: 
#ifdef fun_NDIS_IO_WORKITEM_FUNCTION_4
      fun_NDIS_IO_WORKITEM_FUNCTION_4(WorkItemContext, NdisIoWorkItemHandle);
#endif
        break;

        case 5: 
#ifdef fun_NDIS_IO_WORKITEM_FUNCTION_5
      fun_NDIS_IO_WORKITEM_FUNCTION_5(WorkItemContext, NdisIoWorkItemHandle);
#endif
        break;

        case 6: 
#ifdef fun_NDIS_IO_WORKITEM_FUNCTION_6
      fun_NDIS_IO_WORKITEM_FUNCTION_6(WorkItemContext, NdisIoWorkItemHandle);
#endif
        break;
       default: 0;break;
    }
}


VOID
sdv_NDIS_TIMER_FUNCTION(
    IN  PVOID                   SystemSpecific1,
    IN  PVOID                   FunctionContext,
    IN  PVOID                   SystemSpecific2,
    IN  PVOID                   SystemSpecific3
    )
{
    int choice = SdvMakeChoice();
    int irql = SdvMakeChoice();

    switch(irql) {
      case 0:
        SDV_IRQL_PUSH(PASSIVE_LEVEL);
        break;
      default:
        SDV_IRQL_PUSH(DISPATCH_LEVEL);
		break;
    };


    switch (choice)
    {
        case 1:
#ifdef fun_NDIS_TIMER_FUNCTION_1
      fun_NDIS_TIMER_FUNCTION_1(
            SystemSpecific1,
            FunctionContext,
            SystemSpecific2,
            SystemSpecific3);
#endif
        break;

        case 2: 
#ifdef fun_NDIS_TIMER_FUNCTION_2
      fun_NDIS_TIMER_FUNCTION_2(
            SystemSpecific1,
            FunctionContext,
            SystemSpecific2,
            SystemSpecific3);
#endif
        break;

        case 3: 
#ifdef fun_NDIS_TIMER_FUNCTION_3
      fun_NDIS_TIMER_FUNCTION_3(
            SystemSpecific1,
            FunctionContext,
            SystemSpecific2,
            SystemSpecific3);
#endif
        break;

        case 4: 
#ifdef fun_NDIS_TIMER_FUNCTION_4
      fun_NDIS_TIMER_FUNCTION_4(
            SystemSpecific1,
            FunctionContext,
            SystemSpecific2,
            SystemSpecific3);
#endif
        break;

        case 5: 
#ifdef fun_NDIS_TIMER_FUNCTION_5
      fun_NDIS_TIMER_FUNCTION_5(
            SystemSpecific1,
            FunctionContext,
            SystemSpecific2,
            SystemSpecific3);
#endif
        break;

        case 6: 
#ifdef fun_NDIS_TIMER_FUNCTION_6
      fun_NDIS_TIMER_FUNCTION_6(
            SystemSpecific1,
            FunctionContext,
            SystemSpecific2,
            SystemSpecific3);
#endif
        break;

        case 7: 
#ifdef fun_NDIS_TIMER_FUNCTION_7
      fun_NDIS_TIMER_FUNCTION_7(
            SystemSpecific1,
            FunctionContext,
            SystemSpecific2,
            SystemSpecific3);
#endif
        break;

        case 8: 
#ifdef fun_NDIS_TIMER_FUNCTION_8
      fun_NDIS_TIMER_FUNCTION_8(
            SystemSpecific1,
            FunctionContext,
            SystemSpecific2,
            SystemSpecific3);
#endif
        break;
        
        case 9: 
#ifdef fun_NDIS_TIMER_FUNCTION_9
      fun_NDIS_TIMER_FUNCTION_9(
            SystemSpecific1,
            FunctionContext,
            SystemSpecific2,
            SystemSpecific3);
#endif
        break;
        
        case 10: 
#ifdef fun_NDIS_TIMER_FUNCTION_10
      fun_NDIS_TIMER_FUNCTION_10(
            SystemSpecific1,
            FunctionContext,
            SystemSpecific2,
            SystemSpecific3);
#endif
        break;
        
        case 11: 
#ifdef fun_NDIS_TIMER_FUNCTION_11
      fun_NDIS_TIMER_FUNCTION_11(
            SystemSpecific1,
            FunctionContext,
            SystemSpecific2,
            SystemSpecific3);
#endif
        break;
        
        case 12: 
#ifdef fun_NDIS_TIMER_FUNCTION_12
      fun_NDIS_TIMER_FUNCTION_12(
            SystemSpecific1,
            FunctionContext,
            SystemSpecific2,
            SystemSpecific3);
#endif
        break;
        
        case 13: 
#ifdef fun_NDIS_TIMER_FUNCTION_13
      fun_NDIS_TIMER_FUNCTION_13(
            SystemSpecific1,
            FunctionContext,
            SystemSpecific2,
            SystemSpecific3);
#endif
        break;
        
        case 14: 
#ifdef fun_NDIS_TIMER_FUNCTION_14
      fun_NDIS_TIMER_FUNCTION_14(
            SystemSpecific1,
            FunctionContext,
            SystemSpecific2,
            SystemSpecific3);
#endif
        break;
        
        case 15: 
#ifdef fun_NDIS_TIMER_FUNCTION_15
      fun_NDIS_TIMER_FUNCTION_15(
            SystemSpecific1,
            FunctionContext,
            SystemSpecific2,
            SystemSpecific3);
#endif
        break;
        
        case 16: 
#ifdef fun_NDIS_TIMER_FUNCTION_16
      fun_NDIS_TIMER_FUNCTION_16(
            SystemSpecific1,
            FunctionContext,
            SystemSpecific2,
            SystemSpecific3);
#endif
        break;
        
        case 17: 
#ifdef fun_NDIS_TIMER_FUNCTION_17
      fun_NDIS_TIMER_FUNCTION_17(
            SystemSpecific1,
            FunctionContext,
            SystemSpecific2,
            SystemSpecific3);
#endif
        break;
        
        case 18: 
#ifdef fun_NDIS_TIMER_FUNCTION_18
      fun_NDIS_TIMER_FUNCTION_18(
            SystemSpecific1,
            FunctionContext,
            SystemSpecific2,
            SystemSpecific3);
#endif
        break;
        
        case 19: 
#ifdef fun_NDIS_TIMER_FUNCTION_19
      fun_NDIS_TIMER_FUNCTION_19(
            SystemSpecific1,
            FunctionContext,
            SystemSpecific2,
            SystemSpecific3);
#endif
        break;
        
        case 20: 
#ifdef fun_NDIS_TIMER_FUNCTION_20
      fun_NDIS_TIMER_FUNCTION_20(
            SystemSpecific1,
            FunctionContext,
            SystemSpecific2,
            SystemSpecific3);
#endif
        break;
        
        case 21: 
#ifdef fun_NDIS_TIMER_FUNCTION_21
      fun_NDIS_TIMER_FUNCTION_21(
            SystemSpecific1,
            FunctionContext,
            SystemSpecific2,
            SystemSpecific3);
#endif
        break;
        
        case 22: 
#ifdef fun_NDIS_TIMER_FUNCTION_22
      fun_NDIS_TIMER_FUNCTION_22(
            SystemSpecific1,
            FunctionContext,
            SystemSpecific2,
            SystemSpecific3);
#endif
        break;
        
        case 23: 
#ifdef fun_NDIS_TIMER_FUNCTION_23
      fun_NDIS_TIMER_FUNCTION_23(
            SystemSpecific1,
            FunctionContext,
            SystemSpecific2,
            SystemSpecific3);
#endif
        break;
        
        case 24: 
#ifdef fun_NDIS_TIMER_FUNCTION_24
      fun_NDIS_TIMER_FUNCTION_24(
            SystemSpecific1,
            FunctionContext,
            SystemSpecific2,
            SystemSpecific3);
#endif
        break;


       default: 0;break;
    }
}




BOOLEAN
sdv_MINIPORT_ISR(
    IN  NDIS_HANDLE             MiniportInterruptContext,
    OUT PBOOLEAN                QueueDefaultInterruptDpc,
    OUT PULONG                  TargetProcessors)
{
    BOOLEAN ret;
    int choice = SdvMakeChoice();

    SDV_IRQL_PUSH(SDV_DIRQL);

    switch (choice)
    {
        case 1:
#ifdef fun_MINIPORT_ISR_1
            ret = fun_MINIPORT_ISR_1(
                    MiniportInterruptContext,
                    QueueDefaultInterruptDpc,
                    TargetProcessors);  
#else
            ret = (BOOLEAN) sdv_SetReturnBool ();
#endif
        break;

        case 2: 
#ifdef fun_MINIPORT_ISR_2
            ret = fun_MINIPORT_ISR_2(
                    MiniportInterruptContext,
                    QueueDefaultInterruptDpc,
                    TargetProcessors);  
#else
            ret = (BOOLEAN) sdv_SetReturnBool ();
#endif
        break;

        case 3: 
#ifdef fun_MINIPORT_ISR_3
            ret = fun_MINIPORT_ISR_3(
                MiniportInterruptContext,
                QueueDefaultInterruptDpc,
                TargetProcessors);  
#else
            ret = (BOOLEAN) sdv_SetReturnBool ();
#endif
        break;

        case 4: 
#ifdef fun_MINIPORT_ISR_4
            ret = fun_MINIPORT_ISR_4(
                MiniportInterruptContext,
                QueueDefaultInterruptDpc,
                TargetProcessors);  
#else
            ret = (BOOLEAN) sdv_SetReturnBool ();
#endif
        break;

        case 5: 
#ifdef fun_MINIPORT_ISR_5
            ret = fun_MINIPORT_ISR_5(
                MiniportInterruptContext,
                QueueDefaultInterruptDpc,
                TargetProcessors);  
#else
            ret = (BOOLEAN) sdv_SetReturnBool ();
#endif
        break;

        case 6: 
#ifdef fun_MINIPORT_ISR_6
                  ret = fun_MINIPORT_ISR_6(
                    MiniportInterruptContext,
                    QueueDefaultInterruptDpc,
                    TargetProcessors);  
#else
            ret = (BOOLEAN) sdv_SetReturnBool ();
#endif
        break;

       default: 
	    ret = (BOOLEAN) sdv_SetReturnBool ();
	break;
    }
    
    return ret;
}

VOID
sdv_MINIPORT_INTERRUPT_DPC(
    IN  NDIS_HANDLE             MiniportInterruptContext,
    IN  PVOID                   MiniportDpcContext,
    IN  PVOID                   NdisReserved1,
    IN  PVOID                   NdisReserved2
    )
{
    int choice = SdvMakeChoice();
    SDV_IRQL_PUSH(DISPATCH_LEVEL);

    switch (choice)
    {
        case 1:
#ifdef fun_MINIPORT_INTERRUPT_DPC_1
       fun_MINIPORT_INTERRUPT_DPC_1(
            MiniportInterruptContext,
            MiniportDpcContext,
            NdisReserved1,
            NdisReserved2);
#endif
        break;

        case 2: 
#ifdef fun_MINIPORT_INTERRUPT_DPC_2
        fun_MINIPORT_INTERRUPT_DPC_2(
            MiniportInterruptContext,
            MiniportDpcContext,
            NdisReserved1,
            NdisReserved2);
#endif
        break;

        case 3: 
#ifdef fun_MINIPORT_INTERRUPT_DPC_3
       fun_MINIPORT_INTERRUPT_DPC_3(
            MiniportInterruptContext,
            MiniportDpcContext,
            NdisReserved1,
            NdisReserved2);
#endif
        break;

        case 4: 
#ifdef fun_MINIPORT_INTERRUPT_DPC_4
       fun_MINIPORT_INTERRUPT_DPC_4(
            MiniportInterruptContext,
            MiniportDpcContext,
            NdisReserved1,
            NdisReserved2);
#endif
        break;

        case 5: 
#ifdef fun_MINIPORT_INTERRUPT_DPC_5
       fun_MINIPORT_INTERRUPT_DPC_5(
            MiniportInterruptContext,
            MiniportDpcContext,
            NdisReserved1,
            NdisReserved2);
#endif
        break;

        case 6: 
#ifdef fun_MINIPORT_INTERRUPT_DPC_6
       fun_MINIPORT_INTERRUPT_DPC_6(
            MiniportInterruptContext,
            MiniportDpcContext,
            NdisReserved1,
            NdisReserved2);
#endif
        break;
       default: 0;break;
    }
}


VOID
sdv_MINIPORT_DISABLE_INTERRUPT(
    IN  NDIS_HANDLE             MiniportInterruptContext
    )
{
    int choice = SdvMakeChoice();

    SDV_IRQL_PUSH(SDV_DIRQL);

    switch (choice)
    {
        case 1:
#ifdef fun_MINIPORT_DISABLE_INTERRUPT_1
      fun_MINIPORT_DISABLE_INTERRUPT_1(MiniportInterruptContext);
#endif
        break;

        case 2: 
#ifdef fun_MINIPORT_DISABLE_INTERRUPT_2
        fun_MINIPORT_DISABLE_INTERRUPT_2(MiniportInterruptContext);
#endif
        break;

        case 3: 
#ifdef fun_MINIPORT_DISABLE_INTERRUPT_3
       fun_MINIPORT_DISABLE_INTERRUPT_3(MiniportInterruptContext);
#endif
        break;

        case 4: 
#ifdef fun_MINIPORT_DISABLE_INTERRUPT_4
       fun_MINIPORT_DISABLE_INTERRUPT_4(MiniportInterruptContext);
#endif
        break;

        case 5: 
#ifdef fun_MINIPORT_DISABLE_INTERRUPT_5
       fun_MINIPORT_DISABLE_INTERRUPT_5(MiniportInterruptContext);
#endif
        break;

        case 6: 
#ifdef fun_MINIPORT_DISABLE_INTERRUPT_6
       fun_MINIPORT_DISABLE_INTERRUPT_6(MiniportInterruptContext);
#endif
        break;
       default: 0;break;
    }
}
    

VOID
sdv_MINIPORT_ENABLE_INTERRUPT(
    IN  NDIS_HANDLE             MiniportInterruptContext
    )
{
    int choice = SdvMakeChoice();

    SDV_IRQL_PUSH(SDV_DIRQL);    

    switch (choice)
    {
        case 1:
#ifdef fun_MINIPORT_ENABLE_INTERRUPT_1
      fun_MINIPORT_ENABLE_INTERRUPT_1(MiniportInterruptContext);
#endif
        break;

        case 2: 
#ifdef fun_MINIPORT_ENABLE_INTERRUPT_2
        fun_MINIPORT_ENABLE_INTERRUPT_2(MiniportInterruptContext);
#endif
        break;

        case 3: 
#ifdef fun_MINIPORT_ENABLE_INTERRUPT_3
       fun_MINIPORT_ENABLE_INTERRUPT_3(MiniportInterruptContext);
#endif
        break;

        case 4: 
#ifdef fun_MINIPORT_ENABLE_INTERRUPT_4
       fun_MINIPORT_ENABLE_INTERRUPT_4(MiniportInterruptContext);
#endif
        break;

        case 5: 
#ifdef fun_MINIPORT_ENABLE_INTERRUPT_5
       fun_MINIPORT_ENABLE_INTERRUPT_5(MiniportInterruptContext);
#endif
        break;

        case 6: 
#ifdef fun_MINIPORT_ENABLE_INTERRUPT_6
       fun_MINIPORT_ENABLE_INTERRUPT_6(MiniportInterruptContext);
#endif
        break;
       default: 0;break;
    }
}

BOOLEAN
sdv_MINIPORT_SYNCHRONIZE_INTERRUPT(
    IN PVOID            SynchronizeContext
    )
{
    BOOLEAN ret;
    int choice = SdvMakeChoice();

    SDV_IRQL_PUSH(SDV_DIRQL);    

    switch (choice)
    {
        case 1:
#ifdef fun_MINIPORT_SYNCHRONIZE_INTERRUPT_1
          ret = fun_MINIPORT_SYNCHRONIZE_INTERRUPT_1(SynchronizeContext);
#else
          ret = (BOOLEAN) sdv_SetReturnBool ();
#endif
        break;

        case 2: 
#ifdef fun_MINIPORT_SYNCHRONIZE_INTERRUPT_2
          ret = fun_MINIPORT_SYNCHRONIZE_INTERRUPT_2(SynchronizeContext);
#else
          ret = (BOOLEAN) sdv_SetReturnBool ();
#endif
        break;

        case 3: 
#ifdef fun_MINIPORT_SYNCHRONIZE_INTERRUPT_3
          ret = fun_MINIPORT_SYNCHRONIZE_INTERRUPT_3(SynchronizeContext);
#else
          ret = (BOOLEAN) sdv_SetReturnBool ();
#endif
        break;

        case 4: 
#ifdef fun_MINIPORT_SYNCHRONIZE_INTERRUPT_4
          ret = fun_MINIPORT_SYNCHRONIZE_INTERRUPT_4(SynchronizeContext);
#else
          ret = (BOOLEAN) sdv_SetReturnBool ();
#endif
        break;

        case 5: 
#ifdef fun_MINIPORT_SYNCHRONIZE_INTERRUPT_5
          ret = fun_MINIPORT_SYNCHRONIZE_INTERRUPT_5(SynchronizeContext);
#else
          ret = (BOOLEAN) sdv_SetReturnBool ();
#endif
        break;

        case 6: 
#ifdef fun_MINIPORT_SYNCHRONIZE_INTERRUPT_6
          ret = fun_MINIPORT_SYNCHRONIZE_INTERRUPT_6(SynchronizeContext);
#else
          ret = (BOOLEAN) sdv_SetReturnBool ();
#endif
        break;
       default: 0;break;
    }
    
    return ret;
}



BOOLEAN
sdv_MINIPORT_MESSAGE_INTERRUPT(
    IN  NDIS_HANDLE             MiniportInterruptContext,
    IN  ULONG                   MessageId,
    OUT PBOOLEAN                QueueDefaultInterruptDpc,
    OUT PULONG                  TargetProcessors)
{
    BOOLEAN ret;
    int choice = SdvMakeChoice();

    SDV_IRQL_PUSH(SDV_DIRQL);

    switch (choice)
    {
        case 1:
#ifdef fun_MINIPORT_MESSAGE_INTERRUPT_1
            ret = fun_MINIPORT_MESSAGE_INTERRUPT_1(
                    MiniportInterruptContext,
                    MessageId,
                    QueueDefaultInterruptDpc,
                    TargetProcessors);  
#else
            ret = (BOOLEAN) sdv_SetReturnBool ();
#endif
        break;

        case 2: 
#ifdef fun_MINIPORT_MESSAGE_INTERRUPT_2
            ret = fun_MINIPORT_MESSAGE_INTERRUPT_2(
                    MiniportInterruptContext,
                    MessageId,
                    QueueDefaultInterruptDpc,
                    TargetProcessors);  
#else
            ret = (BOOLEAN) sdv_SetReturnBool ();
#endif
        break;

        case 3: 
#ifdef fun_MINIPORT_MESSAGE_INTERRUPT_3
            ret = fun_MINIPORT_MESSAGE_INTERRUPT_3(
                MiniportInterruptContext,
                MessageId,
                QueueDefaultInterruptDpc,
                TargetProcessors);  
#else
            ret = (BOOLEAN) sdv_SetReturnBool ();
#endif
        break;

        case 4: 
#ifdef fun_MINIPORT_MESSAGE_INTERRUPT_4
            ret = fun_MINIPORT_MESSAGE_INTERRUPT_4(
                MiniportInterruptContext,
                MessageId,
                QueueDefaultInterruptDpc,
                TargetProcessors);  
#else
            ret = (BOOLEAN) sdv_SetReturnBool ();
#endif
        break;

        case 5: 
#ifdef fun_MINIPORT_MESSAGE_INTERRUPT_5
            ret = fun_MINIPORT_MESSAGE_INTERRUPT_5(
                MiniportInterruptContext,
                MessageId,
                QueueDefaultInterruptDpc,
                TargetProcessors);  
#else
            ret = (BOOLEAN) sdv_SetReturnBool ();
#endif
        break;

        case 6: 
#ifdef fun_MINIPORT_MESSAGE_INTERRUPT_6
                  ret = fun_MINIPORT_MESSAGE_INTERRUPT_6(
                    MiniportInterruptContext,
                    MessageId,
                    QueueDefaultInterruptDpc,
                    TargetProcessors);  
#else
            ret = (BOOLEAN) sdv_SetReturnBool ();
#endif
        break;
       default: 0;break;
    }
    
    return ret;
}

VOID
sdv_MINIPORT_MESSAGE_INTERRUPT_DPC(
    IN  NDIS_HANDLE             MiniportInterruptContext,
    IN  ULONG                   MessageId,
    IN  PVOID                   MiniportDpcContext,
    IN  PVOID                   NdisReserved1,
    IN  PVOID                   NdisReserved2
    )
{
    int choice = SdvMakeChoice();
    SDV_IRQL_PUSH(DISPATCH_LEVEL);

    switch (choice)
    {
        case 1:
#ifdef fun_MINIPORT_MESSAGE_INTERRUPT_DPC_1
       fun_MINIPORT_MESSAGE_INTERRUPT_DPC_1(
            MiniportInterruptContext,
            MessageId,
            MiniportDpcContext,
            NdisReserved1,
            NdisReserved2);
#endif
        break;

        case 2: 
#ifdef fun_MINIPORT_MESSAGE_INTERRUPT_DPC_2
        fun_MINIPORT_MESSAGE_INTERRUPT_DPC_2(
            MiniportInterruptContext,
            MessageId,
            MiniportDpcContext,
            NdisReserved1,
            NdisReserved2);
#endif
        break;

        case 3: 
#ifdef fun_MINIPORT_MESSAGE_INTERRUPT_DPC_3
       fun_MINIPORT_MESSAGE_INTERRUPT_DPC_3(
            MiniportInterruptContext,
            MessageId,
            MiniportDpcContext,
            NdisReserved1,
            NdisReserved2);
#endif
        break;

        case 4: 
#ifdef fun_MINIPORT_MESSAGE_INTERRUPT_DPC_4
       fun_MINIPORT_MESSAGE_INTERRUPT_DPC_4(
            MiniportInterruptContext,
            MessageId,
            MiniportDpcContext,
            NdisReserved1,
            NdisReserved2);
#endif
        break;

        case 5: 
#ifdef fun_MINIPORT_MESSAGE_INTERRUPT_DPC_5
       fun_MINIPORT_MESSAGE_INTERRUPT_DPC_5(
            MiniportInterruptContext,
            MessageId,
            MiniportDpcContext,
            NdisReserved1,
            NdisReserved2);
#endif
        break;

        case 6: 
#ifdef fun_MINIPORT_MESSAGE_INTERRUPT_DPC_6
       fun_MINIPORT_MESSAGE_INTERRUPT_DPC_6(
            MiniportInterruptContext,
            MessageId,
            MiniportDpcContext,
            NdisReserved1,
            NdisReserved2);
#endif
        break;
       default: 0;break;
    }
}


VOID
sdv_MINIPORT_DISABLE_MESSAGE_INTERRUPT(
    IN  NDIS_HANDLE             MiniportInterruptContext,
    IN  ULONG                   MessageId
    )
{
    int choice = SdvMakeChoice();

    SDV_IRQL_PUSH(SDV_DIRQL);

    switch (choice)
    {
        case 1:
#ifdef fun_MINIPORT_DISABLE_MESSAGE_INTERRUPT_1
      fun_MINIPORT_DISABLE_MESSAGE_INTERRUPT_1(MiniportInterruptContext, MessageId);
#endif
        break;

        case 2: 
#ifdef fun_MINIPORT_DISABLE_MESSAGE_INTERRUPT_2
        fun_MINIPORT_DISABLE_MESSAGE_INTERRUPT_2(MiniportInterruptContext, MessageId);
#endif
        break;

        case 3: 
#ifdef fun_MINIPORT_DISABLE_MESSAGE_INTERRUPT_3
       fun_MINIPORT_DISABLE_MESSAGE_INTERRUPT_3(MiniportInterruptContext, MessageId);
#endif
        break;

        case 4: 
#ifdef fun_MINIPORT_DISABLE_MESSAGE_INTERRUPT_4
       fun_MINIPORT_DISABLE_MESSAGE_INTERRUPT_4(MiniportInterruptContext, MessageId);
#endif
        break;

        case 5: 
#ifdef fun_MINIPORT_DISABLE_MESSAGE_INTERRUPT_5
       fun_MINIPORT_DISABLE_MESSAGE_INTERRUPT_5(MiniportInterruptContext, MessageId);
#endif
        break;

        case 6: 
#ifdef fun_MINIPORT_DISABLE_MESSAGE_INTERRUPT_6
       fun_MINIPORT_DISABLE_MESSAGE_INTERRUPT_6(MiniportInterruptContext, MessageId);
#endif
        break;
       default: 0;break;
    }
}
    

VOID
sdv_MINIPORT_ENABLE_MESSAGE_INTERRUPT(
    IN  NDIS_HANDLE             MiniportInterruptContext,
    IN  ULONG                   MessageId
    )
{
    int choice = SdvMakeChoice();

    SDV_IRQL_PUSH(SDV_DIRQL);    

    switch (choice)
    {
        case 1:
#ifdef fun_MINIPORT_ENABLE_MESSAGE_INTERRUPT_1
        fun_MINIPORT_ENABLE_MESSAGE_INTERRUPT_1(MiniportInterruptContext, MessageId);
#endif
        break;

        case 2: 
#ifdef fun_MINIPORT_ENABLE_MESSAGE_INTERRUPT_2
        fun_MINIPORT_ENABLE_MESSAGE_INTERRUPT_2(MiniportInterruptContext, MessageId);
#endif
        break;

        case 3: 
#ifdef fun_MINIPORT_ENABLE_MESSAGE_INTERRUPT_3
       fun_MINIPORT_ENABLE_MESSAGE_INTERRUPT_3(MiniportInterruptContext, MessageId);
#endif
        break;

        case 4: 
#ifdef fun_MINIPORT_ENABLE_MESSAGE_INTERRUPT_4
       fun_MINIPORT_ENABLE_MESSAGE_INTERRUPT_4(MiniportInterruptContext, MessageId);
#endif
        break;

        case 5: 
#ifdef fun_MINIPORT_ENABLE_MESSAGE_INTERRUPT_5
       fun_MINIPORT_ENABLE_MESSAGE_INTERRUPT_5(MiniportInterruptContext, MessageId);
#endif
        break;

        case 6: 
#ifdef fun_MINIPORT_ENABLE_MESSAGE_INTERRUPT_6
       fun_MINIPORT_ENABLE_MESSAGE_INTERRUPT_6(MiniportInterruptContext, MessageId);
#endif
        break;
       default: 0;break;
    }
}


BOOLEAN
sdv_MINIPORT_SYNCHRONIZE_MESSAGE_INTERRUPT(
    IN PVOID            SynchronizeContext
    )
{
    BOOLEAN ret;
    int choice = SdvMakeChoice();

    SDV_IRQL_PUSH(SDV_DIRQL);    

    switch (choice)
    {
        case 1:
#ifdef fun_MINIPORT_SYNCHRONIZE_MESSAGE_INTERRUPT_1
        ret = fun_MINIPORT_SYNCHRONIZE_MESSAGE_INTERRUPT_1(SynchronizeContext);
#else
        ret = (BOOLEAN) sdv_SetReturnBool ();
#endif
        break;

        case 2: 
#ifdef fun_MINIPORT_SYNCHRONIZE_MESSAGE_INTERRUPT_2
        ret = fun_MINIPORT_SYNCHRONIZE_MESSAGE_INTERRUPT_2(SynchronizeContext);
#else
        ret = (BOOLEAN) sdv_SetReturnBool ();       
#endif
        break;

        case 3: 
#ifdef fun_MINIPORT_SYNCHRONIZE_MESSAGE_INTERRUPT_3
        ret = fun_MINIPORT_SYNCHRONIZE_MESSAGE_INTERRUPT_3(SynchronizeContext);
#else
        ret = (BOOLEAN) sdv_SetReturnBool ();
#endif
        break;

        case 4: 
#ifdef fun_MINIPORT_SYNCHRONIZE_MESSAGE_INTERRUPT_4
        ret = fun_MINIPORT_SYNCHRONIZE_MESSAGE_INTERRUPT_4(SynchronizeContext);
#else
        ret = (BOOLEAN) sdv_SetReturnBool ();
#endif
        break;

        case 5: 
#ifdef fun_MINIPORT_SYNCHRONIZE_MESSAGE_INTERRUPT_5
        ret = fun_MINIPORT_SYNCHRONIZE_MESSAGE_INTERRUPT_5(SynchronizeContext);
#else
        ret = (BOOLEAN) sdv_SetReturnBool ();
#endif
        break;

        case 6: 
#ifdef fun_MINIPORT_SYNCHRONIZE_MESSAGE_INTERRUPT_6
        ret = fun_MINIPORT_SYNCHRONIZE_MESSAGE_INTERRUPT_6(SynchronizeContext);
#else
        ret = (BOOLEAN) sdv_SetReturnBool ();
#endif
        break;
       default: 0;break;
    }
    return ret;
}





NDIS_STATUS
sdv_MINIPORT_DIRECT_OID_REQUEST(
    IN  NDIS_HANDLE             MiniportAdapterContext,
    IN  PNDIS_OID_REQUEST       OidRequest
    )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    int irql = SdvMakeChoice();

    switch(irql) {
      case 0:
        SDV_IRQL_PUSH(PASSIVE_LEVEL);
        break;
    default:
        SDV_IRQL_PUSH(DISPATCH_LEVEL);
        break;
    };
    
#ifdef fun_MINIPORT_DIRECT_OID_REQUEST
    status = fun_MINIPORT_DIRECT_OID_REQUEST(MiniportAdapterContext, OidRequest);
#endif

    return status;
}

VOID
sdv_MINIPORT_CANCEL_DIRECT_OID_REQUEST(
    IN NDIS_HANDLE              MiniportAdapterContext,
    IN PVOID                    RequestId
    )
{
    int irql = SdvMakeChoice();

    switch(irql) {
      case 0:
        SDV_IRQL_PUSH(PASSIVE_LEVEL);
        break;
    default:
        SDV_IRQL_PUSH(DISPATCH_LEVEL);
        break;
    };

#ifdef fun_MINIPORT_CANCEL_DIRECT_OID_REQUEST
    fun_MINIPORT_CANCEL_DIRECT_OID_REQUEST(MiniportAdapterContext, RequestId);
#endif
}


void sdv_main()
{
    NDIS_STATUS         status;
    BOOLEAN             statusBool;
    
    int x = SdvMakeChoice();

    Length              = SdvMakeChoice();
    MessageId           = SdvMakeChoice();
    SendFlags           = SdvMakeChoice();
    ReturnFlags         = (ULONG) SdvMakeChoice();
    AddressingReset     = (BOOLEAN) SdvMakeChoice();
    PortNumber          = (NDIS_PORT_NUMBER) SdvMakeChoice();

    HaltAction          = (NDIS_HALT_ACTION) SdvMakeChoice();
    ShutdownAction      = (NDIS_SHUTDOWN_ACTION) SdvMakeChoice();

 

#if (SDV_HARNESS==SDV_FLAT_HARNESS)
    
    status = sdv_MINIPORT_INITIALIZE(
              NdisMiniportHandle,
              MiniportDriverContext,
              MiniportInitParameters
              );
              
    if (status == NDIS_STATUS_SUCCESS)
    {

        switch (x) {
     
           case 0:
            status = sdv_MINIPORT_RESET(
                g_AdapterContext,
                &AddressingReset
                );
            break;

           case 1:
            status = sdv_MINIPORT_PAUSE(
                g_AdapterContext,
                MiniportPauseParameters
                );
            break;


           case 2:
            status = sdv_MINIPORT_RESTART(
                g_AdapterContext,
                MiniportRestartParameters
                );
            break;

              
           case 4: 
              sdv_MINIPORT_HALT(
                  g_AdapterContext,
                  HaltAction
                  );
              break;
              
           case 5: 
              status = sdv_MINIPORT_OID_REQUEST(
                  g_AdapterContext,
                  OidRequest
                  );
              break;

           case 6: 
              sdv_MINIPORT_SHUTDOWN(
                  g_AdapterContext,
                  ShutdownAction
                  );
            break;
         
           case 7: 
              sdv_MINIPORT_DEVICE_PNP_EVENT_NOTIFY(
                  g_AdapterContext,
                  NetDevicePnPEvent
                  );
            break;
            
           case 8: 
              sdv_MINIPORT_CANCEL_SEND(
                  g_AdapterContext,
                  CancelId
                  );
            break;
            
           case 9: 
              statusBool = sdv_MINIPORT_CHECK_FOR_HANG(
                  g_AdapterContext
                  );
            break;
            
           case 10: 
             sdv_MINIPORT_CANCEL_OID_REQUEST(
                  g_AdapterContext,
                  RequestId
                  );
            break;
            
           case 11: 
              sdv_MINIPORT_SEND_NET_BUFFER_LISTS(
                  g_AdapterContext,
                  NetBufferList,
                  PortNumber,
                  SendFlags
                  );
            break;
            
           case 12: 
              sdv_MINIPORT_RETURN_NET_BUFFER_LISTS(
                  g_AdapterContext,
                  NetBufferList,
                  ReturnFlags
                  );
            break;
            
           case 13: 
              sdv_MINIPORT_ISR(
                  MiniportInterruptContext,
                  QueueDefaultInterruptDpc,
                  TargetProcessors
                  );
            break;

           case 14: 
              sdv_MINIPORT_INTERRUPT_DPC(
                  MiniportInterruptContext,
                  MiniportDpcContext,
                  NdisReserved1,
                  NdisReserved2
                  );
            break;
            
           case 15: 
              sdv_MINIPORT_DISABLE_INTERRUPT(
                  MiniportInterruptContext
                  );
            break;
            
           case 16: 
              sdv_MINIPORT_ENABLE_INTERRUPT(
                  MiniportInterruptContext
                  );
            break;
            
            case 17: 
              sdv_MINIPORT_SYNCHRONIZE_INTERRUPT(
                  SynchronizeContext
                  );
            break;

           case 18: 
              sdv_MINIPORT_MESSAGE_INTERRUPT(
                  MiniportInterruptContext,
                  MessageId,
                  QueueDefaultInterruptDpc,
                  TargetProcessors
                  );
            break;

           case 19: 
              sdv_MINIPORT_MESSAGE_INTERRUPT_DPC(
                  MiniportInterruptContext,
                  MessageId,
                  MiniportDpcContext,
                  NdisReserved1,
                  NdisReserved2
                  );
            break;
            
           case 20: 
              sdv_MINIPORT_DISABLE_MESSAGE_INTERRUPT(
                  MiniportInterruptContext,
                  MessageId
                  );
            break;
            
           case 21: 
              sdv_MINIPORT_ENABLE_MESSAGE_INTERRUPT(
                  MiniportInterruptContext,
                  MessageId
                  );
            break;
            
            case 22: 
              sdv_MINIPORT_SYNCHRONIZE_MESSAGE_INTERRUPT(
                  SynchronizeContext
                  );
            break;
            
           case 23: 
              sdv_MINIPORT_UNLOAD(
                  DriverObject
                  );
            break;

           case 24: 
              status = sdv_MINIPORT_SET_OPTIONS(
                  NdisDriverHandle,
                  DriverContext
                  );
            break;

           case 25: 
              sdv_MINIPORT_PROCESS_SG_LIST(
                  pDO,
                  Reserved,
                  pSGL,
                  Context
                  );
            break;

           case 26:
             sdv_MINIPORT_ALLOCATE_SHARED_MEM_COMPLETE(
                    g_AdapterContext,
                    VirtualAddress,
                    PhysicalAddress,
                    Length,
                    Context);
            break;
            
           case 27:
            sdv_MINIPORT_ADD_DEVICE(
                g_AdapterContext,
                MiniportDriverContext
                );
            break;
            
           case 28:
            sdv_MINIPORT_START_DEVICE(
                MiniportAddDeviceContext,
                Irp
                );
            break;
            
           case 29:
            sdv_MINIPORT_REMOVE_DEVICE(
                MiniportAddDeviceContext
                );
            break;
            
           case 30:
            sdv_MINIPORT_FILTER_RESOURCE_REQUIREMENTS(
                MiniportAddDeviceContext,
                Irp
                );
            break;

           case 31:
            sdv_NDIS_IO_WORKITEM_FUNCTION(
                WorkItemContext,
                NdisIoWorkItemHandle
                );
            break;

           case 32:
            sdv_NDIS_TIMER_FUNCTION(
                SystemSpecific1,
                FunctionContext,
                SystemSpecific2,
                SystemSpecific3
                );
            break;
            
           case 33:
            status = sdv_MINIPORT_DIRECT_OID_REQUEST(
                g_AdapterContext,
                OidRequest
                );
             break;

            case 34:
            
             sdv_MINIPORT_CANCEL_DIRECT_OID_REQUEST(
                g_AdapterContext,
                RequestId
                );
             break;
            
           default:
            status = sdv_NDIS_DoNothing();
            break;
        }
    }
#endif

#if (SDV_HARNESS==SDV_PNP_HARNESS_INIT)

    status = sdv_MINIPORT_INITIALIZE(
              NdisMiniportHandle,
              MiniportDriverContext,
              MiniportInitParameters
              );
              
    if (status == NDIS_STATUS_SUCCESS)
    {
        sdv_MINIPORT_HALT(
            g_AdapterContext,
            HaltAction
            );
            
    }

#endif

#if (SDV_HARNESS==SDV_PNP_HARNESS_SHUTDOWN)

    sdv_MINIPORT_SHUTDOWN(
        MiniportAdapterContext,
            ShutdownAction
            );
            
#endif

#if (SDV_HARNESS==SDV_PNP_HARNESS_PAUSE)

    status = sdv_MINIPORT_PAUSE(
        MiniportAdapterContext,
        MiniportPauseParameters
        );

#endif

#if (SDV_HARNESS==SDV_PNP_HARNESS_CHECKFORHANG)

    statusBool = sdv_MINIPORT_CHECK_FOR_HANG(
              MiniportAdapterContext
              );
              
#endif


#if (SDV_HARNESS==SDV_PNP_HARNESS_LIFECYCLE)
    
    status = sdv_MINIPORT_SET_OPTIONS(
              NdisDriverHandle,
              DriverContext
              );

    status = sdv_MINIPORT_INITIALIZE(
              NdisMiniportHandle,
              MiniportDriverContext,
              MiniportInitParameters
              );

    sdv_MINIPORT_CANCEL_OID_REQUEST(
              g_AdapterContext,
              RequestId
              );

    status = sdv_MINIPORT_RESTART(
            g_AdapterContext,
            MiniportRestartParameters
            );

    switch(x)
    {
        case 0:
            sdv_MINIPORT_ISR(
                  MiniportInterruptContext,
                  QueueDefaultInterruptDpc,
                  TargetProcessors
                  );
            break;
            
        case 1:
            sdv_NDIS_TIMER_FUNCTION(
                SystemSpecific1,
                FunctionContext,
                SystemSpecific2,
                SystemSpecific3
                );
            break;
            
        case 2:
            sdv_NDIS_IO_WORKITEM_FUNCTION(
                WorkItemContext,
                NdisIoWorkItemHandle
                );
            break;
       
       case 3:
           sdv_MINIPORT_INTERRUPT_DPC(
                  MiniportInterruptContext,
                  MiniportDpcContext,
                  NdisReserved1,
                  NdisReserved2
                  );
            break;
       case 4:
           sdv_MINIPORT_SYNCHRONIZE_INTERRUPT(
                  g_AdapterContext
           );
           break;
           
    }

    status = sdv_MINIPORT_RESET(
        g_AdapterContext,
        &AddressingReset
        );
        
    status = sdv_MINIPORT_PAUSE(
        g_AdapterContext,
        MiniportPauseParameters
        );
        
    sdv_MINIPORT_HALT(
        g_AdapterContext,
        HaltAction
        );
        
    sdv_MINIPORT_UNLOAD(
        DriverObject
        );
        
#endif

#if (SDV_HARNESS==SDV_PNP_HARNESS_OID_REQUEST)

    status = sdv_MINIPORT_INITIALIZE(
              NdisMiniportHandle,
              MiniportDriverContext,
              MiniportInitParameters
              );
              
    if (status == NDIS_STATUS_SUCCESS)
    {

        status = sdv_MINIPORT_OID_REQUEST(
                  g_AdapterContext,
                  OidRequest
                  );
                  
        if (status == NDIS_STATUS_PENDING)
        {
            sdv_NDIS_IO_WORKITEM_FUNCTION(
                    WorkItemContext,
                    NdisIoWorkItemHandle
                    );
        }
    }

#endif

#if (SDV_HARNESS==SDV_PNP_HARNESS_OID_CHECK)

    /*
      -------------------------------
      For all Net Driver
      -------------------------------
      
      Mandatory OIDs for Query:
        OID_GEN_INTERRUPT_MODERATION
        OID_GEN_LINK_PARAMETERS
        OID_GEN_MAXIMUM_FRAME_SIZE
        OID_GEN_MAXIMUM_TOTAL_SIZE
        OID_GEN_RCV_OK
        OID_GEN_RECEIVE_BLOCK_SIZE
        OID_GEN_RECEIVE_BUFFER_SPACE
        OID_GEN_STATISTICS
        OID_GEN_TRANSMIT_BLOCK_SIZE
        OID_GEN_TRANSMIT_BUFFER_SPACE
        OID_GEN_VENDOR_DESCRIPTION
        OID_GEN_VENDOR_DRIVER_VERSION
        OID_GEN_VENDOR_ID
        OID_GEN_XMIT_OK

      Mandatory OIDs for Set:
        OID_GEN_CURRENT_LOOKAHEAD
        OID_GEN_CURRENT_PACKET_FILTER
        OID_GEN_INTERRUPT_MODERATION
        OID_GEN_LINK_PARAMETERS
        
    */

    status = sdv_MINIPORT_INITIALIZE(
              NdisMiniportHandle,
              MiniportDriverContext,
              MiniportInitParameters
              );
              
    if (status == NDIS_STATUS_SUCCESS)
    {

        switch(x)
        {
        
                
            case 0:
                OidRequest->RequestType = NdisRequestQueryInformation;
                OidRequest->DATA.QUERY_INFORMATION.Oid = OID_GEN_INTERRUPT_MODERATION;
                status = sdv_MINIPORT_OID_REQUEST(
                          g_AdapterContext,
                          OidRequest
                          );
                break;
                
            case 1:
                OidRequest->RequestType = NdisRequestQueryInformation;
                OidRequest->DATA.QUERY_INFORMATION.Oid = OID_GEN_LINK_PARAMETERS;
                status = sdv_MINIPORT_OID_REQUEST(
                          g_AdapterContext,
                          OidRequest
                          );
                break;
                
            case 2:
                OidRequest->RequestType = NdisRequestQueryInformation;
                OidRequest->DATA.QUERY_INFORMATION.Oid = OID_GEN_MAXIMUM_FRAME_SIZE;
                status = sdv_MINIPORT_OID_REQUEST(
                          g_AdapterContext,
                          OidRequest
                          );
                break;
                
            case 3:
                OidRequest->RequestType = NdisRequestQueryInformation;
                OidRequest->DATA.QUERY_INFORMATION.Oid = OID_GEN_MAXIMUM_TOTAL_SIZE;
                status = sdv_MINIPORT_OID_REQUEST(
                          g_AdapterContext,
                          OidRequest
                          );
                break;
                
            case 4:
                OidRequest->RequestType = NdisRequestQueryInformation;
                OidRequest->DATA.QUERY_INFORMATION.Oid = OID_GEN_RCV_OK;
                status = sdv_MINIPORT_OID_REQUEST(
                          g_AdapterContext,
                          OidRequest
                          );
                break;
                
            case 5:
                OidRequest->RequestType = NdisRequestQueryInformation;
                OidRequest->DATA.QUERY_INFORMATION.Oid = OID_GEN_RECEIVE_BLOCK_SIZE;
                status = sdv_MINIPORT_OID_REQUEST(
                          g_AdapterContext,
                          OidRequest
                          );
                break;
                
            case 6:
                OidRequest->RequestType = NdisRequestQueryInformation;
                OidRequest->DATA.QUERY_INFORMATION.Oid = OID_GEN_RECEIVE_BUFFER_SPACE;
                status = sdv_MINIPORT_OID_REQUEST(
                          g_AdapterContext,
                          OidRequest
                          );
                break;
                
            case 7:
                OidRequest->RequestType = NdisRequestQueryInformation;
                OidRequest->DATA.QUERY_INFORMATION.Oid = OID_GEN_STATISTICS;
                status = sdv_MINIPORT_OID_REQUEST(
                          g_AdapterContext,
                          OidRequest
                          );
                break;
                
            case 8:
                OidRequest->RequestType = NdisRequestQueryInformation;
                OidRequest->DATA.QUERY_INFORMATION.Oid = OID_GEN_TRANSMIT_BLOCK_SIZE;
                status = sdv_MINIPORT_OID_REQUEST(
                          g_AdapterContext,
                          OidRequest
                          );
                break;
            
            case 9:
                OidRequest->RequestType = NdisRequestQueryInformation;
                OidRequest->DATA.QUERY_INFORMATION.Oid = OID_GEN_TRANSMIT_BUFFER_SPACE;
                status = sdv_MINIPORT_OID_REQUEST(
                          g_AdapterContext,
                          OidRequest
                          );
                break;
                
            case 10:
                OidRequest->RequestType = NdisRequestQueryInformation;
                OidRequest->DATA.QUERY_INFORMATION.Oid = OID_GEN_VENDOR_DESCRIPTION;
                status = sdv_MINIPORT_OID_REQUEST(
                          g_AdapterContext,
                          OidRequest
                          );
                break;
                
            case 11:
                OidRequest->RequestType = NdisRequestQueryInformation;
                OidRequest->DATA.QUERY_INFORMATION.Oid = OID_GEN_VENDOR_DRIVER_VERSION;
                status = sdv_MINIPORT_OID_REQUEST(
                          g_AdapterContext,
                          OidRequest
                          );
                break;
                
            case 12:
                OidRequest->RequestType = NdisRequestQueryInformation;
                OidRequest->DATA.QUERY_INFORMATION.Oid = OID_GEN_VENDOR_ID;
                status = sdv_MINIPORT_OID_REQUEST(
                          g_AdapterContext,
                          OidRequest
                          );
                break;
                
            case 13:
                OidRequest->RequestType = NdisRequestQueryInformation;
                OidRequest->DATA.QUERY_INFORMATION.Oid = OID_GEN_XMIT_OK;
                status = sdv_MINIPORT_OID_REQUEST(
                          g_AdapterContext,
                          OidRequest
                          );
                break;
            
            
            
            
            
            case 14:
                OidRequest->RequestType = NdisRequestSetInformation;
                OidRequest->DATA.SET_INFORMATION.Oid = OID_GEN_CURRENT_LOOKAHEAD;
                status = sdv_MINIPORT_OID_REQUEST(
                          g_AdapterContext,
                          OidRequest
                          );
                break;
                
            case 15:
                OidRequest->RequestType = NdisRequestSetInformation;
                OidRequest->DATA.SET_INFORMATION.Oid = OID_GEN_CURRENT_PACKET_FILTER;
                status = sdv_MINIPORT_OID_REQUEST(
                          g_AdapterContext,
                          OidRequest
                          );
                break;
                
            case 16:
                OidRequest->RequestType = NdisRequestSetInformation;
                OidRequest->DATA.SET_INFORMATION.Oid = OID_GEN_INTERRUPT_MODERATION;
                status = sdv_MINIPORT_OID_REQUEST(
                          g_AdapterContext,
                          OidRequest
                          );
                break;
                
            case 17:
                OidRequest->RequestType = NdisRequestSetInformation;
                OidRequest->DATA.SET_INFORMATION.Oid = OID_GEN_LINK_PARAMETERS;
                status = sdv_MINIPORT_OID_REQUEST(
                          g_AdapterContext,
                          OidRequest
                          );
                break;
        }
    }              

#endif

#if (SDV_HARNESS==SDV_PNP_HARNESS_WIRELESS_OID_CHECK)

    /*
      
      -------------------------------  
      For Native WiFi
      -------------------------------
      
       Mndatoory OIDs for Query:
         0 - OID_DOT11_BEACON_PERIOD 
         1 - OID_DOT11_CURRENT_REG_DOMAIN 
         2 - OID_DOT11_FRAGMENTATION_THRESHOLD 
         3 - OID_DOT11_OPERATIONAL_RATE_SET 
         4 - OID_DOT11_RTS_THRESHOLD 
         5 - OID_DOT11_CURRENT_OPERATION_MODE 
         6 - OID_DOT11_MULTICAST_LIST 
         7 - OID_DOT11_NIC_POWER_STATE 
         8 - OID_DOT11_ACTIVE_PHY_LIST 
         9 - OID_DOT11_AUTO_CONFIG_ENABLED 
        10 - OID_DOT11_CURRENT_PHY_ID 
        11 - OID_DOT11_DESIRED_BSS_TYPE 
        12 - OID_DOT11_DESIRED_BSSID_LIST 
        13 - OID_DOT11_DESIRED_PHY_LIST 
        14 - OID_DOT11_DESIRED_SSID_LIST 
        15 - OID_DOT11_ENABLED_AUTHENTICATION_ALGORITHM 
        16 - OID_DOT11_ENUM_ASSOCIATION_INFO 
        17 - OID_DOT11_EXCLUDED_MAC_ADDRESS_LIST 
        18 - OID_DOT11_HIDDEN_NETWORK_ENABLED
        19 - OID_DOT11_IBSS_PARAMS
        20 - OID_DOT11_MEDIA_STREAMING_ENABLED 
        21 - OID_DOT11_POWER_MGMT_REQUEST
        22 - OID_DOT11_SAFE_MODE_ENABLED
        23 - OID_DOT11_STATISTICS 
        24 - OID_DOT11_UNREACHABLE_DETECTION_THRESHOLD 
        
      Mandatory OIDs for Set:
        25 - OID_DOT11_FRAGMENTATION_THRESHOLD 
        26 - OID_DOT11_OPERATIONAL_RATE_SET 
        27 - OID_DOT11_RTS_THRESHOLD 
        28 - OID_DOT11_CURRENT_OPERATION_MODE 
        29 - OID_DOT11_MULTICAST_LIST 
        30 - OID_DOT11_NIC_POWER_STATE 
        31 - OID_DOT11_SCAN_REQUEST 
        32 - OID_DOT11_AUTO_CONFIG_ENABLED 
        33 - OID_DOT11_CIPHER_KEY_MAPPING_KEY 
        34 - OID_DOT11_CONNECT_REQUEST 
        35 - OID_DOT11_CURRENT_PHY_ID 
        36 - OID_DOT11_DESIRED_BSS_TYPE 
        37 - OID_DOT11_DESIRED_BSSID_LIST 
        38 - OID_DOT11_DESIRED_PHY_LIST 
        39 - OID_DOT11_DESIRED_SSID_LIST 
        40 - OID_DOT11_DISCONNECT_REQUEST 
        41 - OID_DOT11_ENABLED_AUTHENTICATION_ALGORITHM 
        42 - OID_DOT11_EXCLUDED_MAC_ADDRESS_LIST 
        43 - OID_DOT11_FLUSH_BSS_LIST 
        44 - OID_DOT11_HIDDEN_NETWORK_ENABLED
        45 - OID_DOT11_IBSS_PARAMS
        46 - OID_DOT11_MEDIA_STREAMING_ENABLED 
        47 - OID_DOT11_POWER_MGMT_REQUEST
        48 - OID_DOT11_SAFE_MODE_ENABLED
        49 - OID_DOT11_UNREACHABLE_DETECTION_THRESHOLD 
        
      Mandatory OIDs for Method: [TO-DO]
        50 - OID_DOT11_RESET_REQUEST
        51 - OID_DOT11_ENUM_BSS_LIST 
    */  

    status = sdv_MINIPORT_INITIALIZE(
              NdisMiniportHandle,
              MiniportDriverContext,
              MiniportInitParameters
              );
              
    if (status == NDIS_STATUS_SUCCESS)
    {

        switch(x)
        {
                
            case 0:
                OidRequest->RequestType = NdisRequestQueryInformation;
                OidRequest->DATA.QUERY_INFORMATION.Oid = OID_DOT11_BEACON_PERIOD;
                status = sdv_MINIPORT_OID_REQUEST(
                          g_AdapterContext,
                          OidRequest
                          );
                break;
                
            case 1:
                OidRequest->RequestType = NdisRequestQueryInformation;
                OidRequest->DATA.QUERY_INFORMATION.Oid = OID_DOT11_CURRENT_REG_DOMAIN;
                status = sdv_MINIPORT_OID_REQUEST(
                          g_AdapterContext,
                          OidRequest
                          );
                break;
                
            case 2:
                OidRequest->RequestType = NdisRequestQueryInformation;
                OidRequest->DATA.QUERY_INFORMATION.Oid = OID_DOT11_FRAGMENTATION_THRESHOLD;
                status = sdv_MINIPORT_OID_REQUEST(
                          g_AdapterContext,
                          OidRequest
                          );
                break;
                
            case 3:
                OidRequest->RequestType = NdisRequestQueryInformation;
                OidRequest->DATA.QUERY_INFORMATION.Oid = OID_DOT11_OPERATIONAL_RATE_SET;
                status = sdv_MINIPORT_OID_REQUEST(
                          g_AdapterContext,
                          OidRequest
                          );
                break;
                
            case 4:
                OidRequest->RequestType = NdisRequestQueryInformation;
                OidRequest->DATA.QUERY_INFORMATION.Oid = OID_DOT11_RTS_THRESHOLD;
                status = sdv_MINIPORT_OID_REQUEST(
                          g_AdapterContext,
                          OidRequest
                          );
                break;
                
            case 5:
                OidRequest->RequestType = NdisRequestQueryInformation;
                OidRequest->DATA.QUERY_INFORMATION.Oid = OID_DOT11_CURRENT_OPERATION_MODE;
                status = sdv_MINIPORT_OID_REQUEST(
                          g_AdapterContext,
                          OidRequest
                          );
                break;
                
            case 6:
                OidRequest->RequestType = NdisRequestQueryInformation;
                OidRequest->DATA.QUERY_INFORMATION.Oid = OID_DOT11_MULTICAST_LIST;
                status = sdv_MINIPORT_OID_REQUEST(
                          g_AdapterContext,
                          OidRequest
                          );
                break;
                
            case 7:
                OidRequest->RequestType = NdisRequestQueryInformation;
                OidRequest->DATA.QUERY_INFORMATION.Oid = OID_DOT11_NIC_POWER_STATE;
                status = sdv_MINIPORT_OID_REQUEST(
                          g_AdapterContext,
                          OidRequest
                          );
                break;
                
            case 8:
                OidRequest->RequestType = NdisRequestQueryInformation;
                OidRequest->DATA.QUERY_INFORMATION.Oid = OID_DOT11_ACTIVE_PHY_LIST;
                status = sdv_MINIPORT_OID_REQUEST(
                          g_AdapterContext,
                          OidRequest
                          );
                break;
                
            case 9:
                OidRequest->RequestType = NdisRequestQueryInformation;
                OidRequest->DATA.QUERY_INFORMATION.Oid = OID_DOT11_AUTO_CONFIG_ENABLED;
                status = sdv_MINIPORT_OID_REQUEST(
                          g_AdapterContext,
                          OidRequest
                          );
                break;
                
            case 10:
                OidRequest->RequestType = NdisRequestQueryInformation;
                OidRequest->DATA.QUERY_INFORMATION.Oid = OID_DOT11_CURRENT_PHY_ID;
                status = sdv_MINIPORT_OID_REQUEST(
                          g_AdapterContext,
                          OidRequest
                          );
                break;
                
            case 11:
                OidRequest->RequestType = NdisRequestQueryInformation;
                OidRequest->DATA.QUERY_INFORMATION.Oid = OID_DOT11_DESIRED_BSS_TYPE;
                status = sdv_MINIPORT_OID_REQUEST(
                          g_AdapterContext,
                          OidRequest
                          );
                break;
                
            case 12:
                OidRequest->RequestType = NdisRequestQueryInformation;
                OidRequest->DATA.QUERY_INFORMATION.Oid = OID_DOT11_DESIRED_BSSID_LIST;
                status = sdv_MINIPORT_OID_REQUEST(
                          g_AdapterContext,
                          OidRequest
                          );
                break;
                
            case 13:
                OidRequest->RequestType = NdisRequestQueryInformation;
                OidRequest->DATA.QUERY_INFORMATION.Oid = OID_DOT11_DESIRED_PHY_LIST;
                status = sdv_MINIPORT_OID_REQUEST(
                          g_AdapterContext,
                          OidRequest
                          );
                break;
                
            case 14:
                OidRequest->RequestType = NdisRequestQueryInformation;
                OidRequest->DATA.QUERY_INFORMATION.Oid = OID_DOT11_DESIRED_SSID_LIST;
                status = sdv_MINIPORT_OID_REQUEST(
                          g_AdapterContext,
                          OidRequest
                          );
                break;
                
            case 15:
                OidRequest->RequestType = NdisRequestQueryInformation;
                OidRequest->DATA.QUERY_INFORMATION.Oid = OID_DOT11_ENABLED_AUTHENTICATION_ALGORITHM;
                status = sdv_MINIPORT_OID_REQUEST(
                          g_AdapterContext,
                          OidRequest
                          );
                break;
                
            case 16:
                OidRequest->RequestType = NdisRequestQueryInformation;
                OidRequest->DATA.QUERY_INFORMATION.Oid = OID_DOT11_ENUM_ASSOCIATION_INFO;
                status = sdv_MINIPORT_OID_REQUEST(
                          g_AdapterContext,
                          OidRequest
                          );
                break;
                
            case 17:
                OidRequest->RequestType = NdisRequestQueryInformation;
                OidRequest->DATA.QUERY_INFORMATION.Oid = OID_DOT11_EXCLUDED_MAC_ADDRESS_LIST;
                status = sdv_MINIPORT_OID_REQUEST(
                          g_AdapterContext,
                          OidRequest
                          );
                break;
                
            case 18:
                OidRequest->RequestType = NdisRequestQueryInformation;
                OidRequest->DATA.QUERY_INFORMATION.Oid = OID_DOT11_HIDDEN_NETWORK_ENABLED;
                status = sdv_MINIPORT_OID_REQUEST(
                          g_AdapterContext,
                          OidRequest
                          );
                break;
                
            case 19:
                OidRequest->RequestType = NdisRequestQueryInformation;
                OidRequest->DATA.QUERY_INFORMATION.Oid = OID_DOT11_IBSS_PARAMS;
                status = sdv_MINIPORT_OID_REQUEST(
                          g_AdapterContext,
                          OidRequest
                          );
                break;
                
            case 20:
                OidRequest->RequestType = NdisRequestQueryInformation;
                OidRequest->DATA.QUERY_INFORMATION.Oid = OID_DOT11_MEDIA_STREAMING_ENABLED;
                status = sdv_MINIPORT_OID_REQUEST(
                          g_AdapterContext,
                          OidRequest
                          );
                break;
                
            case 21:
                OidRequest->RequestType = NdisRequestQueryInformation;
                OidRequest->DATA.QUERY_INFORMATION.Oid = OID_DOT11_POWER_MGMT_REQUEST;
                status = sdv_MINIPORT_OID_REQUEST(
                          g_AdapterContext,
                          OidRequest
                          );
                break;
                
            case 22:
                OidRequest->RequestType = NdisRequestQueryInformation;
                OidRequest->DATA.QUERY_INFORMATION.Oid = OID_DOT11_SAFE_MODE_ENABLED;
                status = sdv_MINIPORT_OID_REQUEST(
                          g_AdapterContext,
                          OidRequest
                          );
                break;
                
            case 23:
                OidRequest->RequestType = NdisRequestQueryInformation;
                OidRequest->DATA.QUERY_INFORMATION.Oid = OID_DOT11_STATISTICS;
                status = sdv_MINIPORT_OID_REQUEST(
                          g_AdapterContext,
                          OidRequest
                          );
                break;
                
            case 24:
                OidRequest->RequestType = NdisRequestQueryInformation;
                OidRequest->DATA.QUERY_INFORMATION.Oid = OID_DOT11_UNREACHABLE_DETECTION_THRESHOLD;
                status = sdv_MINIPORT_OID_REQUEST(
                          g_AdapterContext,
                          OidRequest
                          );
                break;

            
            
            
            
            case 25:
                OidRequest->RequestType = NdisRequestSetInformation;
                OidRequest->DATA.SET_INFORMATION.Oid = OID_DOT11_FRAGMENTATION_THRESHOLD;
                status = sdv_MINIPORT_OID_REQUEST(
                          g_AdapterContext,
                          OidRequest
                          );
                break;
                
            case 26:
                OidRequest->RequestType = NdisRequestSetInformation;
                OidRequest->DATA.SET_INFORMATION.Oid = OID_DOT11_OPERATIONAL_RATE_SET;
                status = sdv_MINIPORT_OID_REQUEST(
                          g_AdapterContext,
                          OidRequest
                          );
                break;
                
            case 27:
                OidRequest->RequestType = NdisRequestSetInformation;
                OidRequest->DATA.SET_INFORMATION.Oid = OID_DOT11_RTS_THRESHOLD;
                status = sdv_MINIPORT_OID_REQUEST(
                          g_AdapterContext,
                          OidRequest
                          );
                break;
                
            case 28:
                OidRequest->RequestType = NdisRequestSetInformation;
                OidRequest->DATA.SET_INFORMATION.Oid = OID_DOT11_CURRENT_OPERATION_MODE;
                status = sdv_MINIPORT_OID_REQUEST(
                          g_AdapterContext,
                          OidRequest
                          );
                break;
                
            case 29:
                OidRequest->RequestType = NdisRequestSetInformation;
                OidRequest->DATA.SET_INFORMATION.Oid = OID_DOT11_MULTICAST_LIST;
                status = sdv_MINIPORT_OID_REQUEST(
                          g_AdapterContext,
                          OidRequest
                          );
                break;
                
            case 30:
                OidRequest->RequestType = NdisRequestSetInformation;
                OidRequest->DATA.SET_INFORMATION.Oid = OID_DOT11_NIC_POWER_STATE;
                status = sdv_MINIPORT_OID_REQUEST(
                          g_AdapterContext,
                          OidRequest
                          );
                break;
                
            case 31:
                OidRequest->RequestType = NdisRequestSetInformation;
                OidRequest->DATA.SET_INFORMATION.Oid = OID_DOT11_SCAN_REQUEST;
                status = sdv_MINIPORT_OID_REQUEST(
                          g_AdapterContext,
                          OidRequest
                          );
                break;
                
            case 32:
                OidRequest->RequestType = NdisRequestSetInformation;
                OidRequest->DATA.SET_INFORMATION.Oid = OID_DOT11_AUTO_CONFIG_ENABLED;
                status = sdv_MINIPORT_OID_REQUEST(
                          g_AdapterContext,
                          OidRequest
                          );
                break;
                
            case 33:
                OidRequest->RequestType = NdisRequestSetInformation;
                OidRequest->DATA.SET_INFORMATION.Oid = OID_DOT11_CIPHER_KEY_MAPPING_KEY;
                status = sdv_MINIPORT_OID_REQUEST(
                          g_AdapterContext,
                          OidRequest
                          );
                break;
                
            case 34:
                OidRequest->RequestType = NdisRequestSetInformation;
                OidRequest->DATA.SET_INFORMATION.Oid = OID_DOT11_CONNECT_REQUEST;
                status = sdv_MINIPORT_OID_REQUEST(
                          g_AdapterContext,
                          OidRequest
                          );
                break;
                
            case 35:
                OidRequest->RequestType = NdisRequestSetInformation;
                OidRequest->DATA.SET_INFORMATION.Oid = OID_DOT11_CURRENT_PHY_ID;
                status = sdv_MINIPORT_OID_REQUEST(
                          g_AdapterContext,
                          OidRequest
                          );
                break;
                
            case 36:
                OidRequest->RequestType = NdisRequestSetInformation;
                OidRequest->DATA.SET_INFORMATION.Oid = OID_DOT11_DESIRED_BSS_TYPE;
                status = sdv_MINIPORT_OID_REQUEST(
                          g_AdapterContext,
                          OidRequest
                          );
                break;
                
            case 37:
                OidRequest->RequestType = NdisRequestSetInformation;
                OidRequest->DATA.SET_INFORMATION.Oid = OID_DOT11_DESIRED_BSSID_LIST;
                status = sdv_MINIPORT_OID_REQUEST(
                          g_AdapterContext,
                          OidRequest
                          );
                break;
                
            case 38:
                OidRequest->RequestType = NdisRequestSetInformation;
                OidRequest->DATA.SET_INFORMATION.Oid = OID_DOT11_DESIRED_PHY_LIST;
                status = sdv_MINIPORT_OID_REQUEST(
                          g_AdapterContext,
                          OidRequest
                          );
                break;
                
            case 39:
                OidRequest->RequestType = NdisRequestSetInformation;
                OidRequest->DATA.SET_INFORMATION.Oid = OID_DOT11_DESIRED_SSID_LIST;
                status = sdv_MINIPORT_OID_REQUEST(
                          g_AdapterContext,
                          OidRequest
                          );
                break;
                
            case 40:
                OidRequest->RequestType = NdisRequestSetInformation;
                OidRequest->DATA.SET_INFORMATION.Oid = OID_DOT11_DISCONNECT_REQUEST;
                status = sdv_MINIPORT_OID_REQUEST(
                          g_AdapterContext,
                          OidRequest
                          );
                break;
                
            case 41:
                OidRequest->RequestType = NdisRequestSetInformation;
                OidRequest->DATA.SET_INFORMATION.Oid = OID_DOT11_ENABLED_AUTHENTICATION_ALGORITHM;
                status = sdv_MINIPORT_OID_REQUEST(
                          g_AdapterContext,
                          OidRequest
                          );
                break;
                
            case 42:
                OidRequest->RequestType = NdisRequestSetInformation;
                OidRequest->DATA.SET_INFORMATION.Oid =OID_DOT11_EXCLUDED_MAC_ADDRESS_LIST ;
                status = sdv_MINIPORT_OID_REQUEST(
                          g_AdapterContext,
                          OidRequest
                          );
                break;
                
            case 43:
                OidRequest->RequestType = NdisRequestSetInformation;
                OidRequest->DATA.SET_INFORMATION.Oid = OID_DOT11_FLUSH_BSS_LIST;
                status = sdv_MINIPORT_OID_REQUEST(
                          g_AdapterContext,
                          OidRequest
                          );
                break;
                
            case 44:
                OidRequest->RequestType = NdisRequestSetInformation;
                OidRequest->DATA.SET_INFORMATION.Oid = OID_DOT11_HIDDEN_NETWORK_ENABLED;
                status = sdv_MINIPORT_OID_REQUEST(
                          g_AdapterContext,
                          OidRequest
                          );
                break;
                
            case 45:
                OidRequest->RequestType = NdisRequestSetInformation;
                OidRequest->DATA.SET_INFORMATION.Oid = OID_DOT11_IBSS_PARAMS;
                status = sdv_MINIPORT_OID_REQUEST(
                          g_AdapterContext,
                          OidRequest
                          );
                break;
                
            case 46:
                OidRequest->RequestType = NdisRequestSetInformation;
                OidRequest->DATA.SET_INFORMATION.Oid = OID_DOT11_MEDIA_STREAMING_ENABLED;
                status = sdv_MINIPORT_OID_REQUEST(
                          g_AdapterContext,
                          OidRequest
                          );
                break;
                
            case 47:
                OidRequest->RequestType = NdisRequestSetInformation;
                OidRequest->DATA.SET_INFORMATION.Oid = OID_DOT11_POWER_MGMT_REQUEST;
                status = sdv_MINIPORT_OID_REQUEST(
                          g_AdapterContext,
                          OidRequest
                          );
                break;
                
            case 48:
                OidRequest->RequestType = NdisRequestSetInformation;
                OidRequest->DATA.SET_INFORMATION.Oid = OID_DOT11_SAFE_MODE_ENABLED;
                status = sdv_MINIPORT_OID_REQUEST(
                          g_AdapterContext,
                          OidRequest
                          );
                break;
                
            case 49:
                OidRequest->RequestType = NdisRequestSetInformation;
                OidRequest->DATA.SET_INFORMATION.Oid = OID_DOT11_UNREACHABLE_DETECTION_THRESHOLD;
                status = sdv_MINIPORT_OID_REQUEST(
                          g_AdapterContext,
                          OidRequest
                          );
                break;
                
                
        }
    }

#endif

}

int __cdecl main() 
{
    sdv_main();
}
/* harness-main.c end */

/* ndis.c begin */
VOID
sdv_NdisAcquireSpinLock(
    IN PNDIS_SPIN_LOCK  SpinLock
    )
{
    SDV_IRQL_PUSH(DISPATCH_LEVEL);
    (SpinLock)->OldIrql = sdv_irql_previous;
}

VOID
sdv_NdisReleaseSpinLock(
    IN PNDIS_SPIN_LOCK  SpinLock
    )
{
    
    SDV_IRQL_POP();
}

VOID
sdv_NdisDprAcquireSpinLock(
    IN PNDIS_SPIN_LOCK  SpinLock
    )
{
    SDV_IRQL_PUSH(DISPATCH_LEVEL);
}

VOID
sdv_NdisDprReleaseSpinLock(
    IN PNDIS_SPIN_LOCK  SpinLock
    )
{
    SDV_IRQL_POPTO(DISPATCH_LEVEL);
}

PVOID
sdv_NdisAllocateMemoryWithTagPriority(
    IN NDIS_HANDLE  NdisHandle,
    IN UINT  Length,
    IN ULONG  Tag,
    IN EX_POOL_PRIORITY  Priority
    )
{
    Sdv_SetReturnAlloc
} 

VOID
sdv_NdisFreeMemory(
    IN PVOID  VirtualAddress,
    IN UINT  Length,
    IN UINT  MemoryFlags
    )
{
}
    
NDIS_STATUS
sdv_NdisMRegisterScatterGatherDma(
    IN NDIS_HANDLE  MiniportAdapterHandle,
    IN PNDIS_SG_DMA_DESCRIPTION  DmaDescription,
    OUT PNDIS_HANDLE  NdisMiniportDmaHandle
    )
{
   return sdv_SetReturnNDISStatus();
}

VOID
sdv_NdisMDeregisterScatterGatherDma(
    IN NDIS_HANDLE  NdisMiniportDmaHandle
    )
{
}
    
NDIS_STATUS
sdv_NdisMRegisterInterruptEx(
    IN NDIS_HANDLE  MiniportAdapterHandle,
    IN NDIS_HANDLE  MiniportInterruptContext,
    IN PNDIS_MINIPORT_INTERRUPT_CHARACTERISTICS  MiniportInterruptCharacteristics,
    OUT PNDIS_HANDLE  NdisInterruptHandle
    )
{
   return sdv_SetReturnNDISStatus();
}
    
VOID
sdv_NdisMDeregisterInterruptEx(
    IN NDIS_HANDLE  NdisInterruptHandle
    )
{
}

VOID
sdv_NdisAdvanceNetBufferDataStart(
    IN PNET_BUFFER  NetBuffer,
    IN ULONG  DataOffsetDelta,
    IN  BOOLEAN  FreeMdl,
    IN  NET_BUFFER_FREE_MDL_HANDLER  FreeMdlHandler OPTIONAL
    )
{
} 

VOID

sdv_NdisAdvanceNetBufferListDataStart(
    IN PNET_BUFFER_LIST  NetBufferList,
    IN ULONG  DataOffsetDelta,
    IN BOOLEAN  FreeMdl,
    IN NET_BUFFER_FREE_MDL_HANDLER  FreeMdlHandler OPTIONAL
    )
{
} 

PNET_BUFFER_LIST

sdv_NdisAllocateCloneNetBufferList(
    IN PNET_BUFFER_LIST  OriginalNetBufferList,
    IN NDIS_HANDLE  NetBufferListPoolHandle OPTIONAL,
    IN NDIS_HANDLE  NetBufferPoolHandle OPTIONAL,
    IN ULONG  AllocateCloneFlags
    )
{
    LONG choice = SdvMakeChoice();

    switch(choice) {
        case 0:
            return (PNET_BUFFER_LIST) malloc(1);
            break;
        default:
            return (PNET_BUFFER_LIST) NULL;
            break;
    }
} 


NDIS_STATUS

sdv_NdisAllocateCloneOidRequest(
    IN NDIS_HANDLE  SourceHandle,
    IN PNDIS_OID_REQUEST  Request,
    IN UINT  PoolTag,
    OUT PNDIS_OID_REQUEST  *CloneRequest
    )
{
  return sdv_SetReturnNDISStatus();
} 

PNET_BUFFER_LIST
sdv_NdisAllocateFragmentNetBufferList(
    IN PNET_BUFFER_LIST  OriginalNetBufferList,
    IN PNDIS_HANDLE  NetBufferListPoolHandle OPTIONAL,
    IN PNDIS_HANDLE  NetBufferPoolHandle OPTIONAL,
    IN ULONG  StartOffset,
    IN ULONG  MaximumLength,
    IN ULONG  DataOffsetDelta,
    IN ULONG  DataBackFill,
    IN ULONG  AllocateFragmentFlags
    )
{
    LONG choice = SdvMakeChoice();

    switch(choice) {
        case 0:
            return (PNET_BUFFER_LIST) malloc(1);
            break;
        default:
            return (PNET_BUFFER_LIST) NULL;
            break;
    }
} 


PNDIS_GENERIC_OBJECT
sdv_NdisAllocateGenericObject(
    IN PDRIVER_OBJECT  DriverObject OPTIONAL,
    IN ULONG  Tag,
    IN USHORT  Size
    )
{
  LONG choice = SdvMakeChoice();

    switch(choice) {
        case 0:
            return (PNDIS_GENERIC_OBJECT) malloc(1);
            break;
        default:
            return (PNDIS_GENERIC_OBJECT) NULL;
            break;
    }
}

NDIS_HANDLE

sdv_NdisAllocateIoWorkItem(
    IN NDIS_HANDLE  NdisObjectHandle
    )
{
  sdv_SetReturnNDISHandle
}

PMDL

sdv_NdisAllocateMdl(
    IN  NDIS_HANDLE  NdisHandle,
    IN  PVOID  VirtualAddress,
    IN  UINT  Length
    )
{
   LONG choice = SdvMakeChoice();

    switch(choice) {
        case 0:
            return (PMDL) malloc(1);
            break;
        default:
            return (PMDL) NULL;
            break;
    }
}

PNET_BUFFER

sdv_NdisAllocateNetBuffer(
    IN NDIS_HANDLE  PoolHandle,
    IN PMDL  MdlChain,
    IN ULONG  DataOffset,
    IN SIZE_T  DataLength
    )
{
  LONG choice = SdvMakeChoice();

    switch(choice) {
        case 0:
            return (PNET_BUFFER) malloc(1);
            break;
        default:
            return (PNET_BUFFER) NULL;
            break;
    }
} 


PNET_BUFFER_LIST

sdv_NdisAllocateNetBufferAndNetBufferList (
    IN NDIS_HANDLE  PoolHandle,
    IN USHORT  ContextSize,
    IN USHORT  ContextBackFill,
    IN PMDL  MdlChain,
    IN ULONG  DataOffset,
    IN SIZE_T  DataLength
    )
{
  LONG choice = SdvMakeChoice();

    switch(choice) {
        case 0:
            return (PNET_BUFFER_LIST) malloc(1);
            break;
        default:
            return (PNET_BUFFER_LIST) NULL;
            break;
    }
} 

PNET_BUFFER_LIST

sdv_NdisAllocateNetBufferList(
    IN NDIS_HANDLE  PoolHandle,
    IN USHORT  ContextSize,
    IN USHORT  ContextBackFill
    )
{
    LONG choice = SdvMakeChoice();

    switch(choice) {
        case 0:
            return (PNET_BUFFER_LIST) malloc(1);
            break;
        default:
            return (PNET_BUFFER_LIST) NULL;
            break;
    }
} 

NDIS_STATUS

sdv_NdisAllocateNetBufferListContext(
    IN PNET_BUFFER_LIST  NetBufferList,
    IN USHORT  ContextSize,
    IN USHORT  ContextBackFill,
    IN ULONG  PoolTag
    )
{
  return sdv_SetReturnNDISStatus();
} 

NDIS_HANDLE

sdv_NdisAllocateNetBufferListPool(
    IN NDIS_HANDLE  NdisHandle,
    IN PNET_BUFFER_LIST_POOL_PARAMETERS Parameters
    )
{
  sdv_SetReturnNDISHandle
}

PNET_BUFFER

sdv_NdisAllocateNetBufferMdlAndData(
    IN  NDIS_HANDLE PoolHandle
    )
{
    LONG choice = SdvMakeChoice();

    switch(choice) {
        case 0:
            return (PNET_BUFFER) malloc(1);
            break;
        default:
            return (PNET_BUFFER) NULL;
            break;
    }
}

NDIS_HANDLE

sdv_NdisAllocateNetBufferPool(
    IN NDIS_HANDLE  NdisHandle,
    IN PNET_BUFFER_POOL_PARAMETERS  Parameters
    )
{
  sdv_SetReturnNDISHandle
}

PNET_BUFFER_LIST

sdv_NdisAllocateReassembledNetBufferList(
    IN PNET_BUFFER  FragmentedNetBufferList,
    IN NDIS_HANDLE  NetBufferAndNetBufferListPoolHandle OPTIONAL,
    IN ULONG  StartOffset,
    IN ULONG  DataOffsetDelta,
    IN ULONG  DataBackFill,
    IN ULONG  AllocateReassembleFlags
    )
{
   LONG choice = SdvMakeChoice();

    switch(choice) {
        case 0:
            return (PNET_BUFFER_LIST) malloc(1);
            break;
        default:
            return (PNET_BUFFER_LIST) NULL;
            break;
    }
} 


NDIS_STATUS

sdv_NdisAllocateTimerObject(
    IN NDIS_HANDLE  NdisHandle,
    IN PNDIS_TIMER_CHARACTERISTICS  TimerCharacteristics,
    OUT PNDIS_HANDLE  pTimerObject
    )
{  
    int x = SdvMakeChoice();
    (*pTimerObject) = g_TimerObject;
    
    switch(x)
    {
    case 1:
        return NDIS_STATUS_FAILURE;
		break;
        
    default:
        return NDIS_STATUS_SUCCESS;
		break;
    }      

}

VOID

sdv_NdisCancelOidRequest(
    IN NDIS_HANDLE  NdisBindingHandle,
    IN PVOID  RequestID
    )
{
}

VOID

sdv_NdisCancelSendNetBufferLists(
    IN NDIS_HANDLE  NdisBindingHandle,
    IN ULONG_PTR  CancelId
    )
{
}

BOOLEAN

sdv_NdisCancelTimerObject(
    IN NDIS_HANDLE  TimerObject
    )
{
  return sdv_SetReturnBool();
}


VOID
sdv_NdisCompleteBindAdapterEx(
    IN NDIS_HANDLE  BindContext,
    IN NDIS_STATUS  Status
    )
{
} 

VOID

sdv_NdisCompleteNetPnPEvent(
    IN NDIS_HANDLE  NdisBindingHandle,
    IN PNET_PNP_EVENT_NOTIFICATION  NetPnPEvent,
    IN NDIS_STATUS  Status
    )
{
}


VOID

sdv_NdisCompleteUnbindAdapterEx(
    IN NDIS_HANDLE  UnbindContext
    )
{
} 


NDIS_STATUS

sdv_NdisCopyFromNetBufferToNetBuffer(
    IN PNET_BUFFER  Destination,
    IN ULONG  DestinationOffset,
    IN ULONG  BytesToCopy,
    IN PNET_BUFFER  Source,
    IN ULONG  SourceOffset,
    OUT PULONG  BytesCopied
    )
{
  return sdv_SetReturnNDISStatus();
}

VOID

sdv_NdisCopyReceiveNetBufferListInfo(
    IN PNET_BUFFER_LIST  DestNetBufferList,
    IN PNET_BUFFER_LIST  SrcNetBufferList
    )
{
}

VOID

sdv_NdisCopySendNetBufferListInfo(
    IN PNET_BUFFER_LIST  DestNetBufferList,
    IN PNET_BUFFER_LIST  SrcNetBufferList
    )
{
}

VOID

sdv_NdisDeregisterDeviceEx(
    IN NDIS_HANDLE  NdisDeviceHandle
    )
{
}

VOID

sdv_NdisDeregisterProtocolDriver(
    IN NDIS_HANDLE  NdisProtocolHandle
    )
{
} 


NDIS_STATUS

sdv_NdisEnumerateFilterModules (
    IN  NDIS_HANDLE  NdisHandle,
    IN  PVOID  InterfaceBuffer,
    IN  ULONG  InterfaceBufferLength,
    IN  OUT PULONG  BytesNeeded,
    IN  OUT PULONG  BytesWritten
    )
{
  return sdv_SetReturnNDISStatus();
}

NDIS_STATUS

sdv_NdisFCancelOidRequest(
    IN NDIS_HANDLE  NdisFilterHandle,
    IN PVOID  RequestID
    )
{
return sdv_SetReturnNDISStatus();
}

VOID

sdv_NdisFCancelSendNetBufferLists(
    IN NDIS_HANDLE  NdisFilterHandle,
    IN PVOID  CancelId
    )
{
}

VOID

sdv_NdisFDeregisterFilterDriver(
    IN NDIS_HANDLE  NdisFilterDriverHandle
    )
{
} 


VOID

sdv_NdisFDevicePnPEventNotify(
    IN NDIS_HANDLE  NdisFilterHandle,
    IN PNET_DEVICE_PNP_EVENT  NetDevicePnPEvent
  )
{
}

VOID

sdv_NdisFIndicateReceiveNetBufferLists(
    IN NDIS_HANDLE  NdisFilterHandle,
    IN PNET_BUFFER_LIST  NetBufferLists,
    IN NDIS_PORT_NUMBER  PortNumber,
    IN ULONG  NumberOfNetBufferLists,
    IN ULONG  ReceiveFlags
    )
{
} 

VOID

sdv_NdisFIndicateStatus(
    IN NDIS_HANDLE  NdisFilterHandle,
    IN PNDIS_STATUS_INDICATION  StatusIndication
    )
{
} 

NDIS_STATUS

sdv_NdisFNetPnPEvent(
    IN NDIS_HANDLE  FilterModuleContext,
    IN PNET_PNP_EVENT_NOTIFICATION  NetPnPEvent
  )
{
  return sdv_SetReturnNDISStatus();
} 

NDIS_STATUS

sdv_NdisFOidRequest(
    IN NDIS_HANDLE  NdisFilterHandle,
    IN PNDIS_OID_REQUEST  OidRequest
    )
{
  return sdv_SetReturnNDISStatus();
} 

VOID

sdv_NdisFOidRequestComplete(
    IN NDIS_HANDLE  NdisFilterHandle,
    IN PNDIS_OID_REQUEST  OidRequest,
    IN NDIS_STATUS  Status
    )
{
} 


VOID

sdv_NdisFPauseComplete(
    IN NDIS_HANDLE  NdisFilterHandle
    )
{
} 

VOID

sdv_NdisFreeCloneNetBufferList(
    IN PNET_BUFFER_LIST  CloneNetBufferList,
    IN ULONG  FreeCloneFlags
    )
{
} 


VOID

sdv_NdisFreeCloneOidRequest(
    IN NDIS_HANDLE  SourceHandle,
    IN PNDIS_OID_REQUEST  Request
    )
{
} 

VOID

sdv_NdisFreeFragmentNetBufferList(
    IN PNET_BUFFER_LIST  FragmentNetBufferList,
    IN ULONG  DataOffsetDelta,
    IN ULONG  FreeFragmentFlags
    )
{
} 

VOID

sdv_NdisFreeGenericObject(
    IN PNDIS_GENERIC_OBJECT  NdisGenericObject
    )
{
}


VOID 

sdv_NdisFreeIoWorkItem(
    IN NDIS_HANDLE  NdisIoWorkItemHandle
    )
{
}


VOID

sdv_NdisFreeMdl(
    IN PMDL  Mdl
    )
{
}

VOID

sdv_NdisFreeNetBuffer(
    IN PNET_BUFFER  NetBuffer
    )
{
} 

VOID

sdv_NdisFreeNetBufferList(
    IN PNET_BUFFER_LIST  NetBufferList
    )
{
}

VOID

sdv_NdisFreeNetBufferListContext(
    IN PNET_BUFFER_LIST  NetBufferList,
    IN USHORT  ContextSize
    )
{
} 


VOID

sdv_NdisFreeNetBufferListPool(
    IN NDIS_HANDLE  PoolHandle
    )
{
} 

VOID

sdv_NdisFreeNetBufferPool(
    IN NDIS_HANDLE  PoolHandle
    )
{
}

VOID

sdv_NdisFreeReassembledNetBufferList(
    IN PNET_BUFFER_LIST  ReassembledNetBufferList,
    IN ULONG  DataOffsetDelta,
    IN ULONG  FreeReassembleFlags
    )
{
} 


VOID
sdv_NdisFreeTimerObject(
    IN NDIS_HANDLE  TimerObject
    )
{
}

NDIS_STATUS

sdv_NdisFRegisterFilterDriver(
    IN PDRIVER_OBJECT  DriverObject,
    IN NDIS_HANDLE  FilterDriverContext,
    IN PNDIS_FILTER_DRIVER_CHARACTERISTICS  FilterCharacteristics,
    OUT PNDIS_HANDLE  NdisFilterDriverHandle
    )

{
  return sdv_SetReturnNDISStatus();
} 

VOID

sdv_NdisFRestartComplete(
    IN NDIS_HANDLE  NdisFilterHandle,
    IN NDIS_STATUS  Status
    )

{

} 

NDIS_STATUS

sdv_NdisFRestartFilter(
    IN NDIS_HANDLE  NdisFilterHandle
    )

{
  return sdv_SetReturnNDISStatus(); 
} 


VOID

sdv_NdisFReturnNetBufferLists(
    IN NDIS_HANDLE  NdisFilterHandle,
    IN PNET_BUFFER_LIST  NetBufferLists,
    IN ULONG  ReturnFlags
    )
{

} 


VOID

sdv_NdisFSendNetBufferLists(
    IN NDIS_HANDLE  NdisFilterHandle,
    IN PNET_BUFFER_LIST  NetBufferLists,
    IN NDIS_PORT_NUMBER  PortNumber,
    IN ULONG  SendFlags
    )

{

} 


VOID

sdv_NdisFSendNetBufferListsComplete(
    IN NDIS_HANDLE  NdisFilterHandle,
    IN PNET_BUFFER_LIST  NetBufferLists,
    IN ULONG  SendCompleteFlags
    )

{

} 

NDIS_STATUS

sdv_NdisFSetAttributes(
    IN NDIS_HANDLE  NdisFilterHandle,
    IN NDIS_HANDLE  FilterModuleContext,
    IN PNDIS_FILTER_ATTRIBUTES  FilterAttributes
    )

{
  return sdv_SetReturnNDISStatus();
} 

PVOID

sdv_NdisGetDataBuffer(
    IN PNET_BUFFER  NetBuffer,
    IN ULONG  BytesNeeded,
    IN PVOID  Storage,
    IN UINT   AlignMultiple,
    IN UINT   AlignOffset
    )

{
  Sdv_SetReturnAlloc
}
 

VOID


sdv_NdisGetMdlPhysicalArraySize
(

    IN PMDL  _Mdl,
    OUT PUINT  _ArraySize

    ) 
{
}

NDIS_HANDLE

sdv_NdisGetPoolFromNetBuffer(
    IN PNET_BUFFER  NetBuffer
    )
{
  sdv_SetReturnNDISHandle
}

NDIS_HANDLE
sdv_NdisGetPoolFromNetBufferList(
    IN PNET_BUFFER_LIST  NetBufferList
    )
{
  sdv_SetReturnNDISHandle
}

NDIS_STATUS
sdv_NdisMAllocateNetBufferSGList(
    IN NDIS_HANDLE  NdisMiniportDmaHandle,
    IN PNET_BUFFER  NetBuffer,
    IN PVOID  Context,
    IN ULONG  Flags,
    IN PVOID  ScatterGatherListBuffer OPTIONAL,
    IN ULONG  ScatterGatherListBufferSize OPTIONAL
    )
{
  return sdv_SetReturnNDISStatus();
}


VOID 
sdv_NdisMAllocateSharedMemory(
    IN NDIS_HANDLE  MiniportAdapterHandle,
    IN ULONG  Length,
    IN BOOLEAN  Cached,
    OUT PVOID  *VirtualAddress,
    OUT PNDIS_PHYSICAL_ADDRESS  PhysicalAddress
    )
{
    int x = SdvMakeChoice();
    switch(x)
    {
      case 0:
           (*VirtualAddress) = NULL;
		   break;
      default:
           (*VirtualAddress) = (PVOID) malloc(1);
		   break;
    }
}


NDIS_STATUS
sdv_NdisMAllocateSharedMemoryAsyncEx(
    IN NDIS_HANDLE  MiniportDmaHandle,
    IN ULONG  Length,
    IN BOOLEAN  Cached,
    IN PVOID  Context
    )
{
  return sdv_SetReturnNDISStatus();
}  

VOID
sdv_NdisMDeregisterMiniportDriver(
    IN NDIS_HANDLE  NdisMiniportDriverHandle
    )
{
} 

VOID

sdv_NdisMFreeNetBufferSGList(
    IN NDIS_HANDLE  NdisMiniportDmaHandle,
    IN PSCATTER_GATHER_LIST  pSGL,
    IN PNET_BUFFER  NetBuffer
    )
{
} 

ULONG

sdv_NdisMGetBusData(
    IN NDIS_HANDLE  MiniportAdapterHandle,
    IN ULONG  WhichSpace,
    IN ULONG  Offset,
    IN PVOID  Buffer,
    IN ULONG  Length
    )
{
  return (ULONG) SdvKeepChoice();
} 


VOID

sdv_NdisMIndicateReceiveNetBufferLists(
    IN NDIS_HANDLE  MiniportAdapterHandle,
    IN PNET_BUFFER_LIST  NetBufferLists,
    IN NDIS_PORT_NUMBER  PortNumber,
    IN ULONG  NumberOfNetBufferLists,
    IN ULONG  ReceiveFlags
    )
{
} 


VOID

sdv_NdisMIndicateStatusEx(
    IN NDIS_HANDLE  MiniportAdapterHandle,
    IN PNDIS_STATUS_INDICATION  StatusIndication
    )
{
} 

NDIS_STATUS

sdv_NdisMNetPnPEvent(
    IN  NDIS_HANDLE  MiniportAdapterHandle,
    IN  PNET_PNP_EVENT_NOTIFICATION  NetPnPEvent
    )
{
  return sdv_SetReturnNDISStatus();
}

VOID 

sdv_NdisMOidRequestComplete(
    IN NDIS_HANDLE  MiniportAdapterHandle,
    IN PNDIS_OID_REQUEST  OidRequest,
    IN NDIS_STATUS  Status
    )
{
}

VOID

sdv_NdisMPauseComplete(
    IN NDIS_HANDLE  MiniportAdapterHandle
    )
{
}

ULONG

sdv_NdisMQueueDpc(
    IN  NDIS_HANDLE  NdisInterruptHandle,
    IN  ULONG  MessageId,
    IN  ULONG  TargetProcessors,
    IN  PVOID  MiniportDpcContext
    )
{
  return (ULONG) SdvKeepChoice();
}

NDIS_STATUS

sdv_NdisMRegisterMiniportDriver(
    IN PDRIVER_OBJECT  DriverObject,
    IN PUNICODE_STRING  RegistryPath,
    IN NDIS_HANDLE  MiniportDriverContext,
    IN PNDIS_MINIPORT_DRIVER_CHARACTERISTICS  MiniportDriverCharacteristics,
    OUT PNDIS_HANDLE  NdisMiniportDriverHandle
    )
{
  return sdv_SetReturnNDISStatus();
} 

VOID

sdv_NdisMRestartComplete(
    IN NDIS_HANDLE  MiniportAdapterHandle,
    IN NDIS_STATUS  Status
    )
{
} 

VOID

sdv_NdisMSendNetBufferListsComplete(
    IN NDIS_HANDLE  MiniportAdapterHandle,
    IN PNET_BUFFER_LIST  NetBufferLists,
    IN ULONG  SendCompleteFlags
    )
{
} 

ULONG

sdv_NdisMSetBusData(
    IN NDIS_HANDLE  MiniportAdapterHandle,
    IN ULONG  WhichSpace,
    IN ULONG  Offset,
    IN PVOID  Buffer,
    IN ULONG  Length
    )
{
  return (ULONG) SdvKeepChoice();
} 

NDIS_STATUS
sdv_NdisMSetMiniportAttributes (
    IN NDIS_HANDLE  NdisMiniportAdapterHandle,
    IN PNDIS_MINIPORT_ADAPTER_ATTRIBUTES  MiniportAttributes
    )
{
    PNDIS_MINIPORT_ADAPTER_REGISTRATION_ATTRIBUTES RegistrationAttribute = (PNDIS_MINIPORT_ADAPTER_REGISTRATION_ATTRIBUTES)MiniportAttributes;
    SHORT AttributeType = RegistrationAttribute->Header.Type;

    if (AttributeType  == NDIS_OBJECT_TYPE_MINIPORT_ADAPTER_REGISTRATION_ATTRIBUTES)
    {
        g_AdapterContext = RegistrationAttribute->MiniportAdapterContext;
    }
  return sdv_SetReturnNDISStatus();
} 

NDIS_STATUS

sdv_NdisOpenAdapterEx(
    IN NDIS_HANDLE  NdisProtocolHandle,
    IN NDIS_HANDLE  ProtocolBindingContext,
    IN PNDIS_OPEN_PARAMETERS  OpenParameters,
    IN NDIS_HANDLE  BindContext,
    OUT PNDIS_HANDLE  NdisBindingHandle
    )
{
  return sdv_SetReturnNDISStatus();
}

NDIS_STATUS

sdv_NdisOpenConfigurationEx(
    IN PNDIS_CONFIGURATION_OBJECT  ConfigObject,
    OUT PNDIS_HANDLE  ConfigurationHandle
    )

{
  return sdv_SetReturnNDISStatus();
} 

VOID

sdv_NdisQueryMdl(

    IN PMDL _Mdl,
 
    OUT PVOID _VirtualAddress,
 
    OUT PUINT _Length,
 
    IN MM_PAGE_PRIORITY _Priority)

{
} 

VOID

sdv_NdisQueryMdlOffset(
    IN PMDL _Mdl,   
    OUT PUINT  _Offset,
    OUT PUINT  Length)
{
} 

ULONG

sdv_NdisQueryNetBufferPhysicalCount(
    IN  PNET_BUFFER  NetBuffer
    )
{
  return (ULONG) SdvKeepChoice();
}

VOID

sdv_NdisQueueIoWorkItem(
    IN NDIS_HANDLE  NdisIoWorkItemHandle,
    IN NDIS_IO_WORKITEM_ROUTINE  Routine,
    IN PVOID  WorkItemContext
    )
{
}


NDIS_STATUS

sdv_NdisRegisterDeviceEx(
    IN NDIS_HANDLE  NdisHandle,
    IN PNDIS_DEVICE_OBJECT_ATTRIBUTES  DeviceObjectAttributes,
    OUT PDEVICE_OBJECT  *pDeviceObject,
    OUT PNDIS_HANDLE  NdisDeviceHandle
 )
{
  return sdv_SetReturnNDISStatus();
}

NDIS_STATUS

sdv_NdisRegisterProtocolDriver(
    IN NDIS_HANDLE  ProtocolDriverContext,
    IN PNDIS_PROTOCOL_DRIVER_CHARACTERISTICS  ProtocolCharacteristics,
    OUT PNDIS_HANDLE  NdisProtocolHandle
    )
{
  return sdv_SetReturnNDISStatus();
} 


NDIS_STATUS

sdv_NdisRetreatNetBufferDataStart(
    IN PNET_BUFFER  NetBuffer,
    IN ULONG  DataOffsetDelta,
    IN ULONG  DataBackFill,
    IN NET_BUFFER_ALLOCATE_MDL_HANDLER  AllocateMdlHandler OPTIONAL
    )
{
  return sdv_SetReturnNDISStatus();
} 

NDIS_STATUS

sdv_NdisRetreatNetBufferListDataStart(
    IN PNET_BUFFER_LIST  NetBufferList,
    IN ULONG  DataOffsetDelta,
    IN ULONG  DataBackFill,
    IN NET_BUFFER_ALLOCATE_MDL_HANDLER  AllocateMdlHandler OPTIONAL,
    IN NET_BUFFER_FREE_MDL_HANDLER  FreeMdlHandler OPTIONAL
    )
{
  return sdv_SetReturnNDISStatus();
} 


VOID

sdv_NdisReturnNetBufferLists (
    IN NDIS_HANDLE  NdisBindingHandle,
    IN PNET_BUFFER_LIST  NetBufferLists,
    IN ULONG  ReturnFlags
    )
{
} 


VOID

sdv_NdisSendNetBufferLists(
    IN NDIS_HANDLE  NdisBindingHandle,
    IN PNET_BUFFER_LIST  NetBufferLists,
    IN NDIS_PORT_NUMBER  PortNumber,
    IN ULONG  SendFlags
    )
{
} 


NDIS_STATUS

sdv_NdisSetOptionalHandlers(
    IN NDIS_HANDLE  NdisHandle,
    IN PNDIS_DRIVER_OPTIONAL_HANDLERS  OptionalHandlers
    )
{
  return sdv_SetReturnNDISStatus();
} 


BOOLEAN

sdv_NdisSetTimerObject(
    IN NDIS_HANDLE  TimerObject,
    IN LARGE_INTEGER  DueTime,
    IN ULONG  MillisecondsPeriod OPTIONAL,
    IN PVOID  FunctionContext OPTIONAL
    )
{
  return sdv_SetReturnBool();
}


NDIS_STATUS

sdv_NdisUnbindAdapter(
    IN NDIS_HANDLE  NdisBindingHandle
    )
{
  return sdv_SetReturnNDISStatus();
} 

PMDL
sdv_NetAllocateMdl(
    IN PULONG  BufferSize
    )
{
  LONG choice = SdvMakeChoice();

    switch(choice) {
        case 0:
            return (PMDL) malloc(1);
            break;
        default:
            return (PMDL) NULL;
            break;
    }
}

VOID
sdv_NetFreeMdl(
    IN  PMDL  Mdl
    )
{
}

NDIS_STATUS

sdv_NdisCloseAdapterEx(
    IN NDIS_HANDLE  NdisBindingHandle
    )
{
  return sdv_SetReturnNDISStatus();
} 

NDIS_STATUS

sdv_NdisMRegisterIoPortRange(
    OUT PVOID  *PortOffset,
    IN NDIS_HANDLE  MiniportAdapterHandle,
    IN UINT  InitialPort,
    IN UINT  NumberOfPorts
    )
{
  return sdv_SetReturnNDISStatus();
}

VOID
sdv_NDIS_LOWER_IRQL(
    IN KIRQL _OldIrql_, 
    IN KIRQL _CurIrql_
    )
{
    if (_OldIrql_ != _CurIrql_) 
        SDV_IRQL_POPTO(_OldIrql_);
}

VOID
sdv_NDIS_RAISE_IRQL_TO_DISPATCH(
    OUT PKIRQL _pIrql_
    )
{
    SDV_IRQL_PUSH(DISPATCH_LEVEL);
    *_pIrql_ = sdv_irql_previous;
}

BOOLEAN
sdv_NdisWaitEvent(
    IN PNDIS_EVENT  Event,
    IN UINT  MsToWait
    )
{
  return sdv_SetReturnBool();
}

VOID
sdv_NdisStallExecution(
    IN ULONG MicrosecondsToStall
    )
{
}

LONG
sdv_NDIS_RELEASE_MUTEX(
	IN PRKMUTEX  Mutex
	)
{
  return (LONG) SdvKeepChoice();

}

NTSTATUS 
sdv_NDIS_WAIT_FOR_MUTEX(
	IN PRKMUTEX  Mutex
	)
{
  return sdv_SetReturnNDISStatus();
}

VOID
sdv_NdisAcquireReadWriteLock(
    IN  PNDIS_RW_LOCK           Lock,
    IN  BOOLEAN                 fWrite,
    IN  PLOCK_STATE             LockState
    )
{
    (LockState)->OldIrql = sdv_irql_previous;
    SDV_IRQL_PUSH(DISPATCH_LEVEL);
}

PVOID
sdv_NdisAllocateFromNPagedLookasideList(
    IN PNPAGED_LOOKASIDE_LIST  Lookaside
    )
{
  Sdv_SetReturnAlloc
}

NDIS_STATUS
sdv_NdisAnsiStringToUnicodeString(
    IN OUT PUNICODE_STRING  DestinationString,
    IN PANSI_STRING  SourceString
    )
{
  return sdv_SetReturnNDISStatus();
}

NDIS_STATUS
sdv_NdisClAddParty(
    IN  NDIS_HANDLE             NdisVcHandle,
    IN  NDIS_HANDLE             ProtocolPartyContext,
    IN OUT PCO_CALL_PARAMETERS  CallParameters,
    OUT PNDIS_HANDLE            NdisPartyHandle
    )
{
  return sdv_SetReturnNDISStatus();
}

NDIS_STATUS
sdv_NdisClCloseAddressFamily(
    IN  NDIS_HANDLE             NdisAfHandle
    )
{
  return sdv_SetReturnNDISStatus();
}

NDIS_STATUS
sdv_NdisClCloseCall(
    IN  NDIS_HANDLE             NdisVcHandle,
    IN  NDIS_HANDLE             NdisPartyHandle         OPTIONAL,
    IN  PVOID                   Buffer                  OPTIONAL,
    IN  UINT                    Size                    OPTIONAL
    )
{
  return sdv_SetReturnNDISStatus();
}

NDIS_STATUS
sdv_NdisClDeregisterSap(
    IN  NDIS_HANDLE             NdisSapHandle
    )
{
  return sdv_SetReturnNDISStatus();
}

NDIS_STATUS
sdv_NdisClDropParty(
    IN  NDIS_HANDLE             NdisPartyHandle,
    IN  PVOID                   Buffer      OPTIONAL,
    IN  UINT                    Size        OPTIONAL
    )
{
  return sdv_SetReturnNDISStatus();
}

NDIS_STATUS
sdv_NdisClGetProtocolVcContextFromTapiCallId(
    IN  UNICODE_STRING          TapiCallId,
    OUT PNDIS_HANDLE            ProtocolVcContext
    )
{
  return sdv_SetReturnNDISStatus();
}

VOID
sdv_NdisClIncomingCallComplete(
    IN  NDIS_STATUS             Status,
    IN  NDIS_HANDLE             NdisVcHandle,
    IN  PCO_CALL_PARAMETERS     CallParameters
    )
{
}

NDIS_STATUS
sdv_NdisClMakeCall(
    IN  NDIS_HANDLE             NdisVcHandle,
    IN OUT PCO_CALL_PARAMETERS  CallParameters,
    IN  NDIS_HANDLE             ProtocolPartyContext    OPTIONAL,
    OUT PNDIS_HANDLE            NdisPartyHandle         OPTIONAL
    )
{
  return sdv_SetReturnNDISStatus();
}

NDIS_STATUS
sdv_NdisClModifyCallQoS(
    IN  NDIS_HANDLE             NdisVcHandle,
    IN  PCO_CALL_PARAMETERS     CallParameters
    )
{
  return sdv_SetReturnNDISStatus();
}

VOID
sdv_NdisClNotifyCloseAddressFamilyComplete (
    IN  NDIS_HANDLE             NdisAfHandle,
    IN  NDIS_STATUS              Status
    )
{
}

NDIS_STATUS
sdv_NdisClOpenAddressFamilyEx (
    IN  NDIS_HANDLE             NdisBindingHandle,
    IN  PCO_ADDRESS_FAMILY      AddressFamily,
    IN  NDIS_HANDLE             ClientAfContext,
    OUT PNDIS_HANDLE            NdisAfHandle
    )
{
  return sdv_SetReturnNDISStatus();
}

VOID
sdv_NdisCloseConfiguration(
    IN  NDIS_HANDLE             ConfigurationHandle
    )
{
}

VOID
sdv_NdisCloseFile(
    IN  NDIS_HANDLE             FileHandle
    )
{
}

NDIS_STATUS
sdv_NdisClRegisterSap(
    IN  NDIS_HANDLE             NdisAfHandle,
    IN  NDIS_HANDLE             ProtocolSapContext,
    IN  PCO_SAP                 Sap,
    OUT PNDIS_HANDLE            NdisSapHandle
    )
{
  return sdv_SetReturnNDISStatus();
}

NDIS_STATUS
sdv_NdisCmActivateVc(
    IN  NDIS_HANDLE             NdisVcHandle,
    IN OUT PCO_CALL_PARAMETERS  CallParameters
    )
{
  return sdv_SetReturnNDISStatus();
}

VOID
sdv_NdisCmAddPartyComplete(
    IN  NDIS_STATUS             Status,
    IN  NDIS_HANDLE             NdisPartyHandle,
    IN  NDIS_HANDLE             CallMgrPartyContext OPTIONAL,
    IN  PCO_CALL_PARAMETERS     CallParameters
    )
{
}

VOID
sdv_NdisCmCloseAddressFamilyComplete(
    IN  NDIS_STATUS             Status,
    IN  NDIS_HANDLE             NdisAfHandle
    )
{
}

VOID
sdv_NdisCmCloseCallComplete(
    IN  NDIS_STATUS             Status,
    IN  NDIS_HANDLE             NdisVcHandle,
    IN  NDIS_HANDLE             NdisPartyHandle OPTIONAL
    )
{
}

NDIS_STATUS
sdv_NdisCmDeactivateVc(
    IN  NDIS_HANDLE             NdisVcHandle
    )
{
  return sdv_SetReturnNDISStatus();
}

VOID
sdv_NdisCmDeregisterSapComplete(
    IN  NDIS_STATUS             Status,
    IN  NDIS_HANDLE             NdisSapHandle
    )
{
}

VOID
sdv_NdisCmDispatchCallConnected(
    IN  NDIS_HANDLE             NdisVcHandle
    )
{
}

NDIS_STATUS
sdv_NdisCmDispatchIncomingCall(
    IN  NDIS_HANDLE             NdisSapHandle,
    IN  NDIS_HANDLE             NdisVcHandle,
    IN  PCO_CALL_PARAMETERS     CallParameters
    )
{
  return sdv_SetReturnNDISStatus();
}

VOID
sdv_NdisCmDispatchIncomingCallQoSChange(
    IN  NDIS_HANDLE             NdisVcHandle,
    IN  PCO_CALL_PARAMETERS     CallParameters
    )
{
}

VOID
sdv_NdisCmDispatchIncomingCloseCall(
    IN  NDIS_STATUS             CloseStatus,
    IN  NDIS_HANDLE             NdisVcHandle,
    IN  PVOID                   Buffer      OPTIONAL,
    IN  UINT                    Size        OPTIONAL
    )
{
}

VOID
sdv_NdisCmDispatchIncomingDropParty(
    IN  NDIS_STATUS             DropStatus,
    IN  NDIS_HANDLE             NdisPartyHandle,
    IN  PVOID                   Buffer      OPTIONAL,
    IN  UINT                    Size        OPTIONAL
    )
{
}

VOID
sdv_NdisCmDropPartyComplete(
    IN  NDIS_STATUS             Status,
    IN  NDIS_HANDLE             NdisPartyHandle
    )
{
}

VOID
sdv_NdisCmMakeCallComplete(
    IN  NDIS_STATUS             Status,
    IN  NDIS_HANDLE             NdisVcHandle,
    IN  NDIS_HANDLE             NdisPartyHandle     OPTIONAL,
    IN  NDIS_HANDLE             CallMgrPartyContext OPTIONAL,
    IN  PCO_CALL_PARAMETERS     CallParameters
    )
{
}

VOID
sdv_NdisCmModifyCallQoSComplete(
    IN  NDIS_STATUS             Status,
    IN  NDIS_HANDLE             NdisVcHandle,
    IN  PCO_CALL_PARAMETERS     CallParameters
    )
{
}

NDIS_STATUS
sdv_NdisCmNotifyCloseAddressFamily (
    IN  NDIS_HANDLE             NdisAfHandle 
    )
{
  return sdv_SetReturnNDISStatus();
}

VOID
sdv_NdisCmOpenAddressFamilyComplete(
    IN  NDIS_STATUS             Status,
    IN  NDIS_HANDLE             NdisAfHandle,
    IN  NDIS_HANDLE             CallMgrAfContext
    )
{
}

NDIS_STATUS
sdv_NdisCmRegisterAddressFamilyEx(
    IN  NDIS_HANDLE             NdisBindingHandle,
    IN  PCO_ADDRESS_FAMILY      AddressFamily
    )
{
  return sdv_SetReturnNDISStatus();
}

VOID
sdv_NdisCmRegisterSapComplete(
    IN  NDIS_STATUS             Status,
    IN  NDIS_HANDLE             NdisSapHandle,
    IN  NDIS_HANDLE             CallMgrSapContext
    )
{
}

NDIS_STATUS
sdv_NdisCoAssignInstanceName(
    IN  NDIS_HANDLE             NdisVcHandle,
    IN  PNDIS_STRING            BaseInstanceName,
    OUT PNDIS_STRING            VcInstanceName
    )
{
  return sdv_SetReturnNDISStatus();
}

NDIS_STATUS
sdv_NdisCoCreateVc(
    IN  NDIS_HANDLE             NdisBindingHandle,
    IN  NDIS_HANDLE             NdisAfHandle        OPTIONAL,
    IN  NDIS_HANDLE             ProtocolVcContext,
    IN OUT PNDIS_HANDLE         NdisVcHandle
    )
{
  return sdv_SetReturnNDISStatus();
}

NDIS_STATUS
sdv_NdisCoDeleteVc(
    IN  NDIS_HANDLE             NdisVcHandle
    )
{
  return sdv_SetReturnNDISStatus();
}

NDIS_STATUS
sdv_NdisCoGetTapiCallId(
    IN  NDIS_HANDLE             NdisVcHandle,
    IN  OUT PVAR_STRING         TapiCallId
    )
{
  return sdv_SetReturnNDISStatus();
}

NDIS_STATUS
sdv_NdisCoOidRequest(
    IN  NDIS_HANDLE             NdisBindingHandle,
    IN  NDIS_HANDLE             NdisAfHandle    OPTIONAL,
    IN  NDIS_HANDLE             NdisVcHandle    OPTIONAL,
    IN  NDIS_HANDLE             NdisPartyHandle OPTIONAL,
    IN OUT PNDIS_OID_REQUEST    OidRequest
    )
{
  return sdv_SetReturnNDISStatus();
}

VOID
sdv_NdisCoOidRequestComplete(
    IN  NDIS_HANDLE             NdisAfHandle,
    IN  NDIS_HANDLE             NdisVcHandle    OPTIONAL,
    IN  NDIS_HANDLE             NdisPartyHandle OPTIONAL,
    IN  PNDIS_OID_REQUEST       OidRequest,
    IN  NDIS_STATUS             Status
    )
{
}

VOID
sdv_NdisCoSendNetBufferLists(
    IN  NDIS_HANDLE             NdisVcHandle,
    IN  PNET_BUFFER_LIST        NetBufferLists,
    IN  ULONG                   SendFlags
    )
{
}

VOID
sdv_NdisDeleteNPagedLookasideList(
    IN  PNPAGED_LOOKASIDE_LIST  Lookaside
    )
{
}

ULONG
sdv_NdisEqualMemory (
    CONST VOID  *Source1,
    CONST VOID  *Source2,
    ULONG  Length
    )
{
  return (ULONG) SdvKeepChoice();
}

BOOLEAN
sdv_NdisEqualString(
    IN PNDIS_STRING  String1,
    IN PNDIS_STRING  String2,
    IN BOOLEAN  CaseInsensitive
    )
{
  return sdv_SetReturnBool();
}

BOOLEAN 
sdv_NdisEqualUnicodeString(
    IN PNDIS_STRING  String1,
    IN PNDIS_STRING  String2,
    IN BOOLEAN  CaseInSensitive
    )
{
  return sdv_SetReturnBool();
}

VOID
sdv_NdisFreeSpinLock(
    IN PNDIS_SPIN_LOCK  SpinLock
    )
{
}

VOID
sdv_NdisFreeString(
    IN NDIS_STRING  String
    )
{
}

VOID
sdv_NdisFreeToNPagedLookasideList(
    IN PNPAGED_LOOKASIDE_LIST  Lookaside,
    IN PVOID  Entry
    );

UCHAR
sdv_NdisGeneratePartialCancelId(
    VOID
    )
{
  return (UCHAR) SdvKeepChoice();
}

VOID
sdv_NdisGetCurrentProcessorCounts(
    OUT PULONG                  pIdleCount,
    OUT PULONG                  pKernelAndUser,
    OUT PULONG                  pIndex
    )
{
}

VOID
sdv_NdisGetDriverHandle(
    IN  NDIS_HANDLE             NdisBindingHandle,
    OUT PNDIS_HANDLE            NdisDriverHandle
    )
{
}

PVOID
sdv_NdisGetRoutineAddress(
    IN PNDIS_STRING     NdisRoutineName
    )
{
  Sdv_SetReturnAlloc
}

ULONG
sdv_NdisGetSharedDataAlignment(
    VOID
    )
{
  return (ULONG) SdvKeepChoice();
}

UINT
sdv_NdisGetVersion(
    VOID
    )
{
  return (UINT) SdvKeepChoice();
}

NDIS_STATUS
sdv_NdisIfAddIfStackEntry(
    IN NET_IFINDEX          HigherLayerIfIndex,
    IN NET_IFINDEX          LowerLayerIfIndex
    )
{
  return sdv_SetReturnNDISStatus();
}

NDIS_STATUS
sdv_NdisIfAllocateNetLuidIndex(
    IN  NET_IFTYPE          ifType,
    OUT PUINT32             pNetLuidIndex
    )
{
  return sdv_SetReturnNDISStatus();
}

VOID
sdv_NdisIfDeleteIfStackEntry(
    IN NET_IFINDEX          HigherLayerIfIndex,
    IN NET_IFINDEX          LowerLayerIfIndex
    )
{
}

VOID
sdv_NdisIfDeregisterInterface(
    IN NET_IFINDEX          ifIndex
    )
{
}

VOID
sdv_NdisIfDeregisterProvider(
    IN NDIS_HANDLE          NdisProviderHandle
    )
{
}

NDIS_STATUS
sdv_NdisIfFreeNetLuidIndex(
    IN NET_IFTYPE           ifType,
    IN UINT32               NetLuidIndex
    )
{
  return sdv_SetReturnNDISStatus();
}

NDIS_STATUS
sdv_NdisIfGetInterfaceIndexFromNetLuid(
    IN  NET_LUID            NetLuid,
    OUT PNET_IFINDEX        pIfIndex
    )
{
  return sdv_SetReturnNDISStatus();
}

NDIS_STATUS
sdv_NdisIfGetNetLuidFromInterfaceIndex(
    IN  NET_IFINDEX         ifIndex,
    OUT PNET_LUID           pNetLuid
    )
{
  return sdv_SetReturnNDISStatus();
}

NDIS_STATUS
sdv_NdisIfQueryBindingIfIndex(
    IN  NDIS_HANDLE         NdisBindingHandle,
    OUT PNET_IFINDEX        pBoundIfIndex,
    OUT PNET_LUID           pBoundIfNetLuid,
    OUT PNET_IFINDEX        pLowestIfIndex,
    OUT PNET_LUID           pLowestIfNetLuid
    )
{
  return sdv_SetReturnNDISStatus();
}

NDIS_STATUS
sdv_NdisIfRegisterInterface(
    IN NDIS_HANDLE          NdisProviderHandle,
    IN NET_LUID             NetLuid,
    IN NDIS_HANDLE          ProviderIfContext,
    IN PNET_IF_INFORMATION  pIfInfo,
    OUT PNET_IFINDEX        pfIndex
    )
{
  return sdv_SetReturnNDISStatus();
}

NDIS_STATUS
sdv_NdisIfRegisterProvider(
    IN  PNDIS_IF_PROVIDER_CHARACTERISTICS   ProviderCharacteristics,
    IN  NDIS_HANDLE                         IfProviderContext,
    OUT PNDIS_HANDLE                        pNdisIfProviderHandle
    )
{
  return sdv_SetReturnNDISStatus();
}

VOID
sdv_NdisIMAssociateMiniport(
    IN  NDIS_HANDLE         DriverHandle,
    IN  NDIS_HANDLE         ProtocolHandle
    )
{
}

NDIS_STATUS
sdv_NdisIMCancelInitializeDeviceInstance(
    IN  NDIS_HANDLE             DriverHandle,
    IN  PNDIS_STRING            DeviceInstance
    )
{
  return sdv_SetReturnNDISStatus();
}

NDIS_STATUS
sdv_NdisIMDeInitializeDeviceInstance(
    IN  NDIS_HANDLE             NdisMiniportHandle
    )
{
  return sdv_SetReturnNDISStatus();
}

NDIS_HANDLE
sdv_NdisIMGetBindingContext(
    IN  NDIS_HANDLE             NdisBindingHandle
    )
{
  sdv_SetReturnNDISHandle
}

NDIS_STATUS
sdv_NdisIMInitializeDeviceInstanceEx(
    IN  NDIS_HANDLE             DriverHandle,
    IN  PNDIS_STRING            DriverInstance,
    IN  NDIS_HANDLE             DeviceContext   OPTIONAL
    )
{
  return sdv_SetReturnNDISStatus();
}

VOID
sdv_NdisInitAnsiString(
    IN OUT PANSI_STRING  DestinationString,
    IN PCSTR  SourceString
    )
{
}


VOID
sdv_NdisInitializeListHead(
    IN  PLIST_ENTRY ListHead
    )
{
}

VOID
sdv_NdisInitializeNPagedLookasideList(
    IN PNPAGED_LOOKASIDE_LIST  Lookaside,
    IN PALLOCATE_FUNCTION  Allocate  OPTIONAL,
    IN PFREE_FUNCTION  Free  OPTIONAL,
    IN ULONG  Flags,
    IN ULONG  Size,
    IN ULONG  Tag,
    IN USHORT  Depth
    )
{
}

VOID 
sdv_NdisInitializeSListHead(
    IN PSLIST_HEADER  SListHead
    )
{
}

VOID
sdv_NdisInitializeString(
    OUT PNDIS_STRING    Destination,
    IN  PUCHAR          Source
    )
{
}

VOID
sdv_NdisInitUnicodeString(
    IN OUT PNDIS_STRING  DestinationString,
    IN PCWSTR  SourceString
    )
{
}

VOID
sdv_NdisInterlockedAddUlong(
    IN PULONG  Addend,
    IN ULONG  Increment,
    IN PNDIS_SPIN_LOCK  SpinLock
    )
{
}

LONG
sdv_NdisInterlockedDecrement(
    IN PLONG  Addend
    )
{
  return (ULONG) SdvKeepChoice();
}

PLIST_ENTRY
sdv_NdisInterlockedInsertHeadList(
    IN PLIST_ENTRY  ListHead,
    IN PLIST_ENTRY  ListEntry,
    IN PNDIS_SPIN_LOCK  SpinLock
    )
{
    LONG choice = SdvMakeChoice();

    switch(choice) {
        case 0:
            return (PLIST_ENTRY) malloc(1);
            break;
        default:
            return (PLIST_ENTRY) NULL;
            break;
    }
}

PLIST_ENTRY
sdv_NdisInterlockedInsertTailList(
    IN PLIST_ENTRY  ListHead,
    IN PLIST_ENTRY  ListEntry,
    IN PNDIS_SPIN_LOCK  SpinLock
    )
{
    LONG choice = SdvMakeChoice();

    switch(choice) {
        case 0:
            return (PLIST_ENTRY) malloc(1);
            break;
        default:
            return (PLIST_ENTRY) NULL;
            break;
    }
}

PSINGLE_LIST_ENTRY
sdv_NdisInterlockedPopEntrySList(
    IN PSLIST_HEADER  ListHead,
    IN PKSPIN_LOCK  Lock
    )
{
    LONG choice = SdvMakeChoice();

    switch(choice) {
        case 0:
            return (PSINGLE_LIST_ENTRY) malloc(1);
            break;
        default:
            return (PSINGLE_LIST_ENTRY) NULL;
            break;
    }
}

PSINGLE_LIST_ENTRY
sdv_NdisInterlockedPushEntrySList(
    IN PSLIST_HEADER  ListHead,
    IN PSINGLE_LIST_ENTRY  ListEntry,
    IN PKSPIN_LOCK  Lock
    )
{
    LONG choice = SdvMakeChoice();

    switch(choice) {
        case 0:
            return (PSINGLE_LIST_ENTRY) malloc(1);
            break;
        default:
            return (PSINGLE_LIST_ENTRY) NULL;
            break;
    }
}

PLIST_ENTRY
sdv_NdisInterlockedRemoveHeadList(
    IN PLIST_ENTRY  ListHead,
    IN PNDIS_SPIN_LOCK  SpinLock
    )
{
    LONG choice = SdvMakeChoice();

    switch(choice) {
        case 0:
            return (PLIST_ENTRY) malloc(1);
            break;
        default:
            return (PLIST_ENTRY) NULL;
            break;
    }
}

VOID
sdv_NdisMapFile(
    OUT PNDIS_STATUS            Status,
    OUT PVOID *                 MappedBuffer,
    IN  NDIS_HANDLE             FileHandle
    )
{
}

NDIS_STATUS
sdv_NdisMCmActivateVc(
    IN  NDIS_HANDLE             NdisVcHandle,
    IN  PCO_CALL_PARAMETERS     CallParameters
    )
{
  return sdv_SetReturnNDISStatus();
}

VOID
sdv_NdisMCmAddPartyComplete(
    IN NDIS_STATUS  Status,
    IN NDIS_HANDLE  NdisPartyHandle,
    IN NDIS_HANDLE  CallMgrPartyContext  OPTIONAL,
    IN PCO_CALL_PARAMETERS  CallParameters
    )
{
}

VOID
sdv_NdisMCmCloseAddressFamilyComplete(
    IN NDIS_STATUS  Status,
    IN NDIS_HANDLE  NdisAfHandle
    )
{
}

VOID
sdv_NdisMCmCloseCallComplete(
    IN NDIS_STATUS  Status,
    IN NDIS_HANDLE  NdisVcHandle,
    IN NDIS_HANDLE  NdisPartyHandle  OPTIONAL
    )
{
}

NDIS_STATUS
sdv_NdisMCmCreateVc(
    IN  NDIS_HANDLE             MiniportAdapterHandle,
    IN  NDIS_HANDLE             NdisAfHandle,
    IN  NDIS_HANDLE             MiniportVcContext,
    OUT PNDIS_HANDLE            NdisVcHandle
    )
{
  return sdv_SetReturnNDISStatus();
}

NDIS_STATUS
sdv_NdisMCmDeactivateVc(
    IN  NDIS_HANDLE             NdisVcHandle
    )
{
  return sdv_SetReturnNDISStatus();
}

NDIS_STATUS
sdv_NdisMCmDeleteVc(
    IN  NDIS_HANDLE             NdisVcHandle
    )
{
  return sdv_SetReturnNDISStatus();
}

VOID
sdv_NdisMCmDeregisterSapComplete(
    IN NDIS_STATUS  Status,
    IN NDIS_HANDLE  NdisSapHandle
    )
{
}

VOID
sdv_NdisMCmDispatchCallConnected(
    IN NDIS_HANDLE  NdisVcHandle
    )
{
}

NDIS_STATUS
sdv_NdisMCmDispatchIncomingCall(
    IN NDIS_HANDLE  NdisSapHandle,
    IN NDIS_HANDLE  NdisVcHandle,
    IN PCO_CALL_PARAMETERS  CallParameters
    )
{
  return sdv_SetReturnNDISStatus();
}

VOID
sdv_NdisMCmDispatchIncomingCallQoSChange(
    IN NDIS_HANDLE  NdisVcHandle,
    IN PCO_CALL_PARAMETERS  CallParameters
    )
{
}

VOID
sdv_NdisMCmDispatchIncomingCloseCall(
    IN NDIS_STATUS  CloseStatus,
    IN NDIS_HANDLE  NdisVcHandle,
    IN PVOID  Buffer  OPTIONAL,
    IN UINT  Size
    )
{
}

VOID
sdv_NdisMCmDispatchIncomingDropParty(
    IN NDIS_STATUS  DropStatus,
    IN NDIS_HANDLE  NdisPartyHandle,
    IN PVOID  Buffer  OPTIONAL,
    IN UINT  Size
    )
{
}

VOID
sdv_NdisMCmDropPartyComplete(
    IN NDIS_STATUS  Status,
    IN NDIS_HANDLE  NdisPartyHandle
    )
{
}

VOID
sdv_NdisMCmMakeCallComplete(
    IN NDIS_STATUS  Status,
    IN NDIS_HANDLE  NdisVcHandle,
    IN NDIS_HANDLE  NdisPartyHandle  OPTIONAL,
    IN NDIS_HANDLE  CallMgrPartyContext  OPTIONAL,
    IN PCO_CALL_PARAMETERS  CallParameters
    )
{
}

VOID
sdv_NdisMCmModifyCallQoSComplete(
    IN NDIS_STATUS  Status,
    IN NDIS_HANDLE  NdisVcHandle,
    IN PCO_CALL_PARAMETERS  CallParameters
    )
{
}

NDIS_STATUS
sdv_NdisMCmNotifyCloseAddressFamily(
    IN NDIS_HANDLE  NdisAfHandle
    )
{
  return sdv_SetReturnNDISStatus();
}

NDIS_STATUS
sdv_NdisMCmOidRequest(
    IN  NDIS_HANDLE             NdisAfHandle,
    IN  NDIS_HANDLE             NdisVcHandle    OPTIONAL,
    IN  NDIS_HANDLE             NdisPartyHandle OPTIONAL,
    IN OUT PNDIS_OID_REQUEST    NdisRequest
    )
{
  return sdv_SetReturnNDISStatus();
}

VOID
sdv_NdisMCmOidRequestComplete(
    IN NDIS_HANDLE  NdisAfHandle,
    IN NDIS_HANDLE  NdisVcHandle  OPTIONAL,
    IN NDIS_HANDLE  NdisPartyHandle  OPTIONAL,
    IN PNDIS_OID_REQUEST  OidRequest,
    IN NDIS_STATUS  Status
    )
{
}

VOID
sdv_NdisMCmOpenAddressFamilyComplete(
    IN NDIS_STATUS  Status,
    IN NDIS_HANDLE  NdisAfHandle,
    IN NDIS_HANDLE  CallMgrAfContext
    )
{
}


NDIS_STATUS
sdv_NdisMCmRegisterAddressFamilyEx(
    IN  NDIS_HANDLE             MiniportAdapterHandle,
    IN  PCO_ADDRESS_FAMILY      AddressFamily
    )
{
  return sdv_SetReturnNDISStatus();
}

VOID
sdv_NdisMCmRegisterSapComplete(
    IN NDIS_STATUS  Status,
    IN NDIS_HANDLE  NdisSapHandle,
    IN NDIS_HANDLE  CallMgrSapContext
    )
{
}


VOID
sdv_NdisMCoActivateVcComplete(
    IN  NDIS_STATUS             Status,
    IN  NDIS_HANDLE             NdisVcHandle,
    IN  PCO_CALL_PARAMETERS     CallParameters
    )
{
}

VOID
sdv_NdisMCoDeactivateVcComplete(
    IN  NDIS_STATUS             Status,
    IN  NDIS_HANDLE             NdisVcHandle
    )
{
}

VOID
sdv_NdisMCoIndicateReceiveNetBufferLists(
    IN NDIS_HANDLE          NdisVcHandle,
    IN PNET_BUFFER_LIST     NetBufferLists,
    IN ULONG                NumberOfNetBufferLists,
    IN ULONG                CoReceiveFlags
    )
{
}

VOID
sdv_NdisMCoIndicateStatusEx(
    IN  NDIS_HANDLE             MiniportAdapterHandle,
    IN  NDIS_HANDLE             NdisVcHandle    OPTIONAL,
    IN  PNDIS_STATUS_INDICATION StatusIndication
    )
{
}

VOID
sdv_NdisMCompleteDmaTransfer(
    OUT PNDIS_STATUS  Status,
    IN NDIS_HANDLE  MiniportDmaHandle,
    IN PNDIS_BUFFER  Buffer,
    IN ULONG  Offset,
    IN ULONG  Length,
    IN BOOLEAN  WriteToDevice
    )
{
}

VOID
sdv_NdisMCoOidRequestComplete(
    IN  NDIS_HANDLE             MiniportAdapterHandle,
    IN  NDIS_HANDLE             NdisMiniportVcHandle,
    IN  PNDIS_OID_REQUEST       Request,
    IN  NDIS_STATUS             Status
    )
{
}

VOID
sdv_NdisMCoSendNetBufferListsComplete(
    IN NDIS_HANDLE          NdisVcHandle,
    IN PNET_BUFFER_LIST     NetBufferLists,
    IN ULONG                SendCompleteFlags
    )
{
}

NDIS_STATUS
sdv_NdisMCreateLog(
    IN  NDIS_HANDLE             MiniportAdapterHandle,
    IN  UINT                    Size,
    OUT PNDIS_HANDLE            LogHandle
    )
{
  return sdv_SetReturnNDISStatus();
}

VOID
sdv_NdisMDeregisterDmaChannel(
    IN  NDIS_HANDLE             MiniportDmaHandle
    )
{
}

VOID
sdv_NdisMDeregisterIoPortRange(
    IN  NDIS_HANDLE             MiniportAdapterHandle,
    IN  UINT                    InitialPort,
    IN  UINT                    NumberOfPorts,
    IN  PVOID                   PortOffset
    )
{
}

VOID
sdv_NdisMFlushLog(
    IN  NDIS_HANDLE             LogHandle
    )
{
}

NDIS_STATUS
sdv_NdisMFreePort(
    IN  NDIS_HANDLE                     NdisMiniportHandle,
    IN  NDIS_PORT_NUMBER                PortNumber
    )
{
  return sdv_SetReturnNDISStatus();
}

VOID
sdv_NdisMFreeSharedMemory(
    IN  NDIS_HANDLE             MiniportAdapterHandle,
    IN  ULONG                   Length,
    IN  BOOLEAN                 Cached,
    IN  PVOID                   VirtualAddress,
    IN  NDIS_PHYSICAL_ADDRESS   PhysicalAddress
    )
{
}

VOID
sdv_NdisMGetDeviceProperty(
    IN  NDIS_HANDLE             MiniportAdapterHandle,
    IN OUT PDEVICE_OBJECT *     PhysicalDeviceObject        OPTIONAL,
    IN OUT PDEVICE_OBJECT *     FunctionalDeviceObject      OPTIONAL,
    IN OUT PDEVICE_OBJECT *     NextDeviceObject            OPTIONAL,
    IN OUT PCM_RESOURCE_LIST *  AllocatedResources          OPTIONAL,
    IN OUT PCM_RESOURCE_LIST *  AllocatedResourcesTranslated OPTIONAL
    )
{
}

ULONG
sdv_NdisMGetDmaAlignment(
    IN  NDIS_HANDLE MiniportAdapterHandle
    )
{
  return (ULONG) SdvKeepChoice();
}

NDIS_STATUS
sdv_NdisMMapIoSpace(
    OUT PVOID *                 VirtualAddress,
    IN  NDIS_HANDLE             MiniportAdapterHandle,
    IN  NDIS_PHYSICAL_ADDRESS   PhysicalAddress,
    IN  UINT                    Length
    )
{
  return sdv_SetReturnNDISStatus();
}

NDIS_STATUS
sdv_NdisMQueryAdapterInstanceName(
    OUT PNDIS_STRING            pAdapterInstanceName,
    IN  NDIS_HANDLE             MiniportHandle
    )
{
  return sdv_SetReturnNDISStatus();
}

ULONG
sdv_NdisMReadDmaCounter(
    IN  NDIS_HANDLE             MiniportDmaHandle
    )
{
  return (ULONG) SdvKeepChoice();
}

NDIS_STATUS
sdv_NdisMRegisterDmaChannel(
    OUT PNDIS_HANDLE            MiniportDmaHandle,
    IN  NDIS_HANDLE             MiniportAdapterHandle,
    IN  UINT                    DmaChannel,
    IN  BOOLEAN                 Dma32BitAddresses,
    IN  PNDIS_DMA_DESCRIPTION   DmaDescription,
    IN  ULONG                   MaximumLength
    )
{
  return sdv_SetReturnNDISStatus();
}

NDIS_STATUS
sdv_NdisMRemoveMiniport(
    IN  NDIS_HANDLE             MiniportHandle
    )
{
  return sdv_SetReturnNDISStatus();
}

VOID
sdv_NdisMResetComplete(
    IN  NDIS_HANDLE             MiniportAdapterHandle,
    IN  NDIS_STATUS             Status,
    IN  BOOLEAN                 AddressingReset
    )
{
}

VOID
sdv_NdisMSetupDmaTransfer(
    OUT PNDIS_STATUS  Status,
    IN NDIS_HANDLE  MiniportDmaHandle,
    IN PNDIS_BUFFER  Buffer,
    IN ULONG  Offset,
    IN ULONG  Length,
    IN BOOLEAN  WriteToDevice
    )
{
}

VOID
sdv_NdisMSleep(
    IN  ULONG                   MicrosecondsToSleep
    )
{
}

VOID
sdv_NdisMUnmapIoSpace(
    IN  NDIS_HANDLE             MiniportAdapterHandle,
    IN  PVOID                   VirtualAddress,
    IN  UINT                    Length
    )
{
}

VOID
sdv_NdisMUpdateSharedMemory(
    IN  NDIS_HANDLE             MiniportAdapterHandle,
    IN  ULONG                   Length,
    IN  PVOID                   VirtualAddress,
    IN  NDIS_PHYSICAL_ADDRESS   PhysicalAddress
    )
{
}

NDIS_STATUS
sdv_NdisMWriteLogData(
    IN  NDIS_HANDLE             LogHandle,
    IN  PVOID                   LogBuffer,
    IN  UINT                    LogBufferSize
    )
{
  return sdv_SetReturnNDISStatus();
}

VOID
sdv_NdisOpenConfigurationKeyByIndex(
    OUT PNDIS_STATUS            Status,
    IN  NDIS_HANDLE             ConfigurationHandle,
    IN  ULONG                   Index,
    OUT PNDIS_STRING            KeyName,
    OUT PNDIS_HANDLE            KeyHandle
    )
{
}

VOID
sdv_NdisOpenConfigurationKeyByName(
    OUT PNDIS_STATUS            Status,
    IN  NDIS_HANDLE             ConfigurationHandle,
    IN  PNDIS_STRING            SubKeyName,
    OUT PNDIS_HANDLE            SubKeyHandle
    )
{
}

VOID
sdv_NdisOpenFile(
    OUT PNDIS_STATUS            Status,
    OUT PNDIS_HANDLE            FileHandle,
    OUT PUINT                   FileLength,
    IN  PNDIS_STRING            FileName,
    IN  NDIS_PHYSICAL_ADDRESS   HighestAcceptableAddress
    )
{
}

NDIS_STATUS
sdv_NdisQueryAdapterInstanceName(
    OUT PNDIS_STRING    pAdapterInstanceName,
    IN  NDIS_HANDLE     NdisBindingHandle
    )
{
  return sdv_SetReturnNDISStatus();
}

USHORT
sdv_NdisQueryDepthSList(
    IN PSLIST_HEADER  SListHead
    )
{
  return (USHORT) SdvKeepChoice();
}

VOID
sdv_NdisReadConfiguration(
    OUT PNDIS_STATUS            Status,
    OUT PNDIS_CONFIGURATION_PARAMETER *ParameterValue,
    IN  NDIS_HANDLE             ConfigurationHandle,
    IN  PNDIS_STRING            Keyword,
    IN  NDIS_PARAMETER_TYPE     ParameterType
    )
{
}

VOID
sdv_NdisReadNetworkAddress(
    OUT PNDIS_STATUS            Status,
    OUT PVOID *                 NetworkAddress,
    OUT PUINT                   NetworkAddressLength,
    IN  NDIS_HANDLE             ConfigurationHandle
    )
{
}

VOID
sdv_NdisReEnumerateProtocolBindings(
    IN  NDIS_HANDLE             NdisProtocolHandle
    )
{
}

VOID
sdv_NdisReleaseReadWriteLock(
    IN  PNDIS_RW_LOCK           Lock,
    IN  PLOCK_STATE             LockState
    )
{
    SDV_IRQL_POP();
}

VOID
sdv_NdisResetEvent(
    IN  PNDIS_EVENT             Event
)
{
}

VOID
sdv_NdisSetEvent(
    IN  PNDIS_EVENT             Event
)
{
}

CCHAR
sdv_NdisSystemProcessorCount(
    VOID
    )
{
  return (CCHAR) SdvMakeChoice();
}

NDIS_STATUS
sdv_NdisUnicodeStringToAnsiString(
    IN OUT PANSI_STRING  DestinationString,
    IN PUNICODE_STRING  SourceString
    )
{
  return sdv_SetReturnNDISStatus();
}

VOID
sdv_NdisUnmapFile(
    IN  NDIS_HANDLE             FileHandle
    )
{
}


NTSTATUS
sdv_NdisUpcaseUnicodeString(
    OUT PUNICODE_STRING  DestinationString,  
    IN PUNICODE_STRING  SourceString
    )
{
  return sdv_SetReturnNDISStatus();
}

VOID
sdv_NdisWriteConfiguration(
    OUT PNDIS_STATUS            Status,
    IN  NDIS_HANDLE             ConfigurationHandle,
    IN  PNDIS_STRING            Keyword,
    IN  PNDIS_CONFIGURATION_PARAMETER ParameterValue
    )
{
}

VOID __cdecl
sdv_NdisWriteErrorLogEntry(
    IN  NDIS_HANDLE             NdisAdapterHandle,
    IN  NDIS_ERROR_CODE         ErrorCode,
    IN  ULONG                   NumberOfErrorValues,
    ...
    )
{
}

NDIS_STATUS
sdv_NdisWriteEventLogEntry(
    IN  PVOID                   LogHandle,
    IN  NDIS_STATUS             EventCode,
    IN  ULONG                   UniqueEventValue,
    IN  USHORT                  NumStrings,
    IN  PVOID                   StringsList    OPTIONAL,
    IN  ULONG                   DataSize,
    IN  PVOID                   Data           OPTIONAL
    )
{
  return sdv_SetReturnNDISStatus();
}
 

VOID
  NdisReadPortBufferUchar(
    IN NDIS_HANDLE  NdisAdapterHandle,
    IN ULONG  Port,
    OUT PUCHAR  Buffer,
    IN ULONG  Length
    )
{
}

VOID
  NdisWritePortBufferUchar(
    IN NDIS_HANDLE  NdisAdapterHandle,
    IN ULONG  Port,
    IN PUCHAR  Buffer,
    IN ULONG  Length
    )
{
}

VOID
  NdisReadPortBufferUlong(
    IN NDIS_HANDLE  NdisAdapterHandle,
    IN ULONG  Port,
    OUT PULONG  Buffer,
    IN ULONG  Length
    )
{
}

VOID
  NdisWritePortBufferUlong(
    IN NDIS_HANDLE  NdisAdapterHandle,
    IN ULONG  Port,
    IN PULONG  Buffer,
    IN ULONG  Length
    )
{
}

VOID
  NdisReadPortBufferUshort(
    IN NDIS_HANDLE  NdisAdapterHandle,
    IN ULONG  Port,
    OUT PUSHORT  Buffer,
    IN ULONG  Length
    )
{
}

VOID
  NdisWritePortBufferUshort(
    IN NDIS_HANDLE  NdisAdapterHandle,
    IN ULONG  Port,
    IN PUSHORT  Buffer,
    IN ULONG  Length
    )
{
}
    
VOID
  NdisReadPortUchar(
    IN NDIS_HANDLE  NdisAdapterHandle,
    IN ULONG  Port,
    OUT PUCHAR  Data
    )
{
}
    
VOID
  NdisWritePortUchar(
    IN NDIS_HANDLE  NdisAdapterHandle,
    IN ULONG  Port,
    IN UCHAR  Data
    )
{
}

VOID
  NdisReadPortUlong(
    IN NDIS_HANDLE  NdisAdapterHandle,
    IN ULONG  Port,
    OUT PULONG  Data
    )
{
}
    
VOID
  NdisWritePortUlong(
    IN NDIS_HANDLE  NdisAdapterHandle,
    IN ULONG  Port,
    IN ULONG  Data
    )
{
}

VOID
  NdisWritePortUshort(
    IN NDIS_HANDLE  NdisAdapterHandle,
    IN ULONG  Port,
    IN USHORT  Data
    )
{
}

VOID
  NdisReadPortUshort(
    IN NDIS_HANDLE  NdisAdapterHandle,
    IN ULONG  Port,
    OUT PUSHORT  Data
    )
{
}

BOOLEAN
  sdv_NDIS_TEST_RETURN_AT_DISPATCH_LEVEL(
    IN ULONG  Flags
	)
{
    if(sdv_irql_current == DISPATCH_LEVEL)
    {	
        return 1;
    }
    else
    {
		return 0;
    }
}

BOOLEAN
  sdv_NDIS_TEST_SEND_COMPLETE_AT_DISPATCH_LEVEL(
    IN ULONG  Flags
	)
{
    if(sdv_irql_current == DISPATCH_LEVEL)
    {	
        return 1;
    }
    else
    {
		return 0;
    }
}

BOOLEAN
  sdv_NDIS_TEST_SEND_AT_DISPATCH_LEVEL(
    IN ULONG  Flags
	)
{
    if(sdv_irql_current == DISPATCH_LEVEL)
    {	
        return 1;
    }
    else
    {
		return 0;
    }
}

BOOLEAN
  sdv_NDIS_TEST_RECEIVE_AT_DISPATCH_LEVEL(
    IN ULONG  Flags
	)
{
    if(sdv_irql_current == DISPATCH_LEVEL)
    {	
        return 1;
    }
    else
    {
		return 0;
    }
}


KIRQL
  sdv_NDIS_CURRENT_IRQL(VOID) 
{
    return sdv_irql_current;
}
  

ULONG 
  KeGetCurrentProcessorNumber(VOID)
{
  return (ULONG) SdvKeepChoice();
}

BOOLEAN  
sdv_NdisMSynchronizeWithInterruptEx(    
    NDIS_HANDLE  NdisInterruptHandle,    
    ULONG  MessageId,    
    PVOID  SynchronizeFunction,    
    PVOID  SynchronizeContext    
    )
{
  return sdv_SetReturnBool();
}

VOID sdv_NdisAllocateSpinLock(IN PNDIS_SPIN_LOCK  SpinLock)
{
	SpinLock=(PNDIS_SPIN_LOCK)malloc(1);
}


void 
sdv_save_adapter_context(void ** c)
{
     
     (*c) = g_AdapterContext;   
}
/* ndis.c end */

