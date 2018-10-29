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

/* wdf-harness-parts.c begin */
#include "wdf.h"


#define NUMBER_OF_DEVICES 1


KIRQL stored_irql; 

KIRQL device_stored_irql;
KIRQL driver_stored_irql;
KIRQL device_stored_queue_irql;
BOOLEAN device_queue_irql=0;
BOOLEAN Device_No_QueueAttributes=0;

KIRQL default_queue_stored_irql;
KIRQL read_queue_stored_irql;
KIRQL write_queue_stored_irql;
KIRQL device_control_queue_stored_irql;
KIRQL internal_device_control_queue_stored_irql;
KIRQL stop_queue_stored_irql;
KIRQL resume_queue_stored_irql;
KIRQL io_cancel_on_queue_stored_irql;

BOOLEAN default_no_attrib=0;
BOOLEAN read_no_attrib=0;
BOOLEAN write_no_attrib=0;
BOOLEAN device_control_no_attrib=0;
BOOLEAN internal_device_control_no_attrib=0;
BOOLEAN stop_queue_no_attrib=0;
BOOLEAN resume_queue_no_attrib=0;
BOOLEAN io_cancel_on_queue_no_attrib=0;


BOOLEAN sdv_inside_io_read_callback = 0;
BOOLEAN sdv_inside_io_write_callback = 0;
BOOLEAN sdv_inside_io_default_callback = 0;
BOOLEAN sdv_inside_io_device_control_callback = 0;
BOOLEAN sdv_inside_io_internal_device_control_callback = 0;

BOOLEAN sdv_io_device_queue_configured = 0;  


int notused_4p_devobj;
PDEVICE_OBJECT p_devobj = (PDEVICE_OBJECT) &notused_4p_devobj;


PUNICODE_STRING hregistry_path;
PDRIVER_OBJECT hp_driver;

int notused_4hdriver;
WDFDRIVER hdriver = (WDFDRIVER) &notused_4hdriver;

int notused_4hdrctx;
PVOID hdrctx = (PVOID) &notused_4hdrctx;

int init_4hdevice_init;
int notused_4hdevice_init;
PWDFDEVICE_INIT hdevice_init = (PWDFDEVICE_INIT) &init_4hdevice_init;   

int init_4hcontrol_deviceinit;
int notused_4hcontrol_deviceinit;
int control=0;
PWDFDEVICE_INIT hcontrol_deviceinit = (PWDFDEVICE_INIT) &init_4hcontrol_deviceinit;

int init_4hpdo_deviceinit;
int pdo=0;
int notused_4hpdo_deviceinit;
PWDFDEVICE_INIT hpdo_deviceinit = (PWDFDEVICE_INIT) &init_4hpdo_deviceinit;

int init_4hcontrol_deviceinit1;
int notused_4hcontrol_deviceinit1;
PWDFDEVICE_INIT hcontrol_deviceinit1 =(PWDFDEVICE_INIT) &init_4hcontrol_deviceinit1;

int init_4hpdo_deviceinit1;
int notused_4hpdo_deviceinit1;
PWDFDEVICE_INIT hpdo_deviceinit1 = (PWDFDEVICE_INIT) &init_4hpdo_deviceinit1;


int init_4hcontrol_deviceinit2;
int notused_4hcontrol_deviceinit2;
PWDFDEVICE_INIT hcontrol_deviceinit2_notused =(PWDFDEVICE_INIT) &init_4hcontrol_deviceinit2;

int init_4hpdo_deviceinit2;
int notused_4hpdo_deviceinit2;
PWDFDEVICE_INIT hpdo_deviceinit2_notused = (PWDFDEVICE_INIT) &init_4hpdo_deviceinit2;

int init_4hdevice, init_4hdevice1, init_4hdevice2;
int notused_4hdevice, notused_4hdevice1, notused_4hdevice2;
WDFDEVICE hdevice  = (WDFDEVICE)&init_4hdevice, 
          hdevice1 = (WDFDEVICE)&init_4hdevice1, 
		  hdevice2 = (WDFDEVICE)&init_4hdevice2; 

int init_4hcontrol_device;
int notused_4hcontrol_device;  
WDFDEVICE hcontrol_device = (WDFDEVICE)&init_4hcontrol_device;

int init_4hpdo_device;
int notused_4hpdo_device;
WDFDEVICE hpdo_device = (WDFDEVICE)&init_4hpdo_device; 


int init_4hcontrol_device1;
int notused_4hcontrol_device1;  
WDFDEVICE hcontrol_device1 = (WDFDEVICE)&init_4hcontrol_device1;

int init_4hpdo_device1;
int notused_4hpdo_device1;
WDFDEVICE hpdo_device1 = (WDFDEVICE)&init_4hpdo_device1; 

int init_4husb_device;
int notused_4husb_device;
WDFUSBDEVICE husb_device = (WDFUSBDEVICE)&init_4husb_device; 

int init_4hiotarget;
int notused_4hiotarget;
WDFIOTARGET hiotarget = (WDFIOTARGET) &init_4hiotarget;

WDFCOLLECTION hResourceList, hResourceListTranslated;
PVOID hdctx;

WDF_DEVICE_POWER_POLICY_STATE powerstate = (WDF_DEVICE_POWER_POLICY_STATE) 0;
WDFDMAENABLER  dmaenabler=(WDFDMAENABLER)0;

int notused_4hdevcollection;
WDFCOLLECTION hdevcollection = (WDFCOLLECTION) &notused_4hdevcollection;   

ULONG hdevcollection_count = 0;

int notused_4hdefqueue;
WDFQUEUE hdefqueue = (WDFQUEUE) &notused_4hdefqueue;

int notused_4hqctx;
PVOID hqctx = (PVOID) &notused_4hqctx;

int notused_4hrequest, notused1_4hrequest1;

#ifndef SDV_COMPLEX_REQUEST_OBJ
    WDFREQUEST hrequest = (WDFREQUEST) &notused_4hrequest, hrequest1 = (WDFREQUEST) &notused1_4hrequest1;
#endif

int notused_4hrctx;
PVOID hrctx = (PVOID) &notused_4hrctx;

PWDF_REQUEST_PARAMETERS hp_params;

int notused_4htimer;
WDFTIMER htimer = (WDFTIMER)&notused_4htimer;

int notused_4hwork_item;
WDFWORKITEM hwork_item = (WDFWORKITEM)&notused_4hwork_item;

int notused_4hdpc;     
WDFDPC hdpc = (WDFDPC)&notused_4hdpc;

int notused_4hinterrupt;     
WDFINTERRUPT hinterrupt = (WDFINTERRUPT)&notused_4hinterrupt;

ULONG hmessageID = 0;

int notused_4htctx;
PVOID htctx = (PVOID) &notused_4htctx;

size_t hln, hln2;

ULONG hln3;

int notused_4hbuffer;
int init_4hbuffer;
PVOID hbuffer = &init_4hbuffer;

int init_4hcur_buffer;
PVOID hcur_buffer = &init_4hcur_buffer;

int notused_4hmemory;
int init_4hmemory;
WDFMEMORY hmemory = (WDFMEMORY)&init_4hmemory;

int init_4hcur_memory;
PVOID hcur_memory = &init_4hcur_memory;

int notused_4hmdl;
PMDL pNotused_4mdl=(PMDL) &notused_4hmdl;
int init_4hmdl;
PMDL hmdl = (PMDL)&init_4hmdl;

int init_4hcur_mdl;
PVOID hcur_mdl = &init_4hcur_mdl;

int notused_4hfileobj; 
WDFFILEOBJECT hfileobj = (WDFFILEOBJECT) &notused_4hfileobj;

int notused_4hfctx;
PVOID hfctx = (PVOID) &notused_4hfctx;

int* pnotused = &sdv_MmMapIoSpace_int;

int notused1 = 0;
int* pnotused1 = &notused1;

int notused_4wdfobject1;

ULONG hact =WdfRequestStopActionSuspend;
ULONG hact_suspend =WdfRequestStopActionSuspend;
ULONG hact_purge =WdfRequestStopActionPurge;



int rwcalled = 0;
int rwcalled1 = 0;

int reqpresented = 0;
int reqpresented1 = 0;

int reqcompleted = 0;
int reqcompleted1 = 0;


BOOLEAN sdv_WdfIoQueueNoRequests = 1;
BOOLEAN sdv_WdfIoQueueDriverNoRequests = 1;


typedef struct _complexRequest{
	BOOLEAN reqSentSuccessfully;
} complexRequest;
complexRequest notused_4ComplexHrequest, notused1_4ComplexHrequest1;




int notused_4usbpipe;
WDFUSBPIPE hUsbPipe= (WDFUSBPIPE) &notused_4usbpipe;

#ifdef SDV_COMPLEX_REQUEST_OBJ
    WDFREQUEST hrequest =(WDFREQUEST) &notused_4ComplexHrequest, hrequest1 = (WDFREQUEST) &notused1_4ComplexHrequest1;
#endif





NTSTATUS
sdv_SetReturnStatus()
{
	LONG choice = SdvMakeChoice();

    switch(choice) {
        case 0:
            return (NTSTATUS) STATUS_NOT_SUPPORTED;
            break;
        default:
            return (NTSTATUS) STATUS_SUCCESS;
            break;
    }
}


WDF_DEVICE_POWER_POLICY_STATE
sdv_SetDevicePowerPolicyState()
{
	WDF_DEVICE_POWER_POLICY_STATE powerstate = (WDF_DEVICE_POWER_POLICY_STATE) SdvMakeChoice();
	return powerstate;
    /*switch(choice) {
        case 0:
            return (WDF_DEVICE_POWER_POLICY_STATE) ;
            break;
		case 1:
            return (WDF_DEVICE_POWER_POLICY_STATE) ;
            break;
		case 2:
            return (WDF_DEVICE_POWER_POLICY_STATE) ;
            break;
		case 3:
            return (WDF_DEVICE_POWER_POLICY_STATE) ;
            break;
		case 4:
            return (WDF_DEVICE_POWER_POLICY_STATE) ;
            break;
		case 5:
            return (WDF_DEVICE_POWER_POLICY_STATE) ;
            break;
		case 6:
            return (WDF_DEVICE_POWER_POLICY_STATE) ;
            break;
		case 7:
            return (WDF_DEVICE_POWER_POLICY_STATE) ;
            break;
		case 8:
            return (WDF_DEVICE_POWER_POLICY_STATE) ;
            break;
		case 9:
            return (WDF_DEVICE_POWER_POLICY_STATE) ;
            break;
		case 10:
            return (WDF_DEVICE_POWER_POLICY_STATE) ;
            break;
		case 11:
            return (WDF_DEVICE_POWER_POLICY_STATE) ;
            break;
		case 12:
            return (WDF_DEVICE_POWER_POLICY_STATE) ;
            break;
		case 13:
            return (WDF_DEVICE_POWER_POLICY_STATE) ;
            break;
		case 14:
            return (WDF_DEVICE_POWER_POLICY_STATE) ;
            break;
		case 15:
            return (WDF_DEVICE_POWER_POLICY_STATE) ;
            break;
		case 16:
            return (WDF_DEVICE_POWER_POLICY_STATE) ;
            break;
		case 17:
            return (WDF_DEVICE_POWER_POLICY_STATE) ;
            break;
		case 18:
            return (WDF_DEVICE_POWER_POLICY_STATE) ;
            break;
		case 19:
            return (WDF_DEVICE_POWER_POLICY_STATE) ;
            break;
		case 20:
            return (WDF_DEVICE_POWER_POLICY_STATE) ;
            break;
		case 21:
            return (WDF_DEVICE_POWER_POLICY_STATE) ;
            break;
		case 22:
            return (WDF_DEVICE_POWER_POLICY_STATE) ;
            break;
		case 23:
            return (WDF_DEVICE_POWER_POLICY_STATE) ;
            break;
		case 24:
            return (WDF_DEVICE_POWER_POLICY_STATE) ;
            break;
		case 25:
            return (WDF_DEVICE_POWER_POLICY_STATE) ;
            break;
		case 26:
            return (WDF_DEVICE_POWER_POLICY_STATE) ;
            break;
		case 27:
            return (WDF_DEVICE_POWER_POLICY_STATE) ;
            break;
		case 28:
            return (WDF_DEVICE_POWER_POLICY_STATE) ;
            break;
		case 29:
            return (WDF_DEVICE_POWER_POLICY_STATE) ;
            break;
		case 30:
            return (WDF_DEVICE_POWER_POLICY_STATE) ;
            break;
		case 31:
            return (WDF_DEVICE_POWER_POLICY_STATE) ;
            break;
		case 32:
            return (WDF_DEVICE_POWER_POLICY_STATE) ;
            break;
		case 33:
            return (WDF_DEVICE_POWER_POLICY_STATE) ;
            break;
		case 34:
            return (WDF_DEVICE_POWER_POLICY_STATE) ;
            break;
		case 35:
            return (WDF_DEVICE_POWER_POLICY_STATE) ;
            break;
		case 36:
            return (WDF_DEVICE_POWER_POLICY_STATE) ;
            break;
		case 37:
            return (WDF_DEVICE_POWER_POLICY_STATE) ;
            break;
		case 38:
            return (WDF_DEVICE_POWER_POLICY_STATE) ;
            break;
		case 39:
            return (WDF_DEVICE_POWER_POLICY_STATE) ;
            break;

		case 40:
            return (WDF_DEVICE_POWER_POLICY_STATE) ;
            break;
		case 41:
            return (WDF_DEVICE_POWER_POLICY_STATE) ;
            break;
		case 42:
            return (WDF_DEVICE_POWER_POLICY_STATE) ;
            break;
		case 43:
            return (WDF_DEVICE_POWER_POLICY_STATE) ;
            break;
		case 44:
            return (WDF_DEVICE_POWER_POLICY_STATE) ;
            break;
		case 45:
            return (WDF_DEVICE_POWER_POLICY_STATE) ;
            break;
		case 46:
            return (WDF_DEVICE_POWER_POLICY_STATE) ;
            break;
		case 47:
            return (WDF_DEVICE_POWER_POLICY_STATE) ;
            break;
		case 48:
            return (WDF_DEVICE_POWER_POLICY_STATE) ;
            break;
		case 49:
            return (WDF_DEVICE_POWER_POLICY_STATE) ;
            break;
		case 50:
            return (WDF_DEVICE_POWER_POLICY_STATE) ;
            break;
		case 51:
            return (WDF_DEVICE_POWER_POLICY_STATE) ;
            break;
		case 52:
            return (WDF_DEVICE_POWER_POLICY_STATE) ;
            break;
		case 53:
            return (WDF_DEVICE_POWER_POLICY_STATE) ;
            break;
		case 54:
            return (WDF_DEVICE_POWER_POLICY_STATE) ;
            break;
		case 55:
            return (WDF_DEVICE_POWER_POLICY_STATE) ;
            break;
		case 56:
            return (WDF_DEVICE_POWER_POLICY_STATE) ;
            break;
		case 57:
            return (WDF_DEVICE_POWER_POLICY_STATE) ;
            break;
		case 58:
            return (WDF_DEVICE_POWER_POLICY_STATE) ;
            break;
		case 59:
            return (WDF_DEVICE_POWER_POLICY_STATE) ;
            break;
		case 60:
            return (WDF_DEVICE_POWER_POLICY_STATE) ;
            break;
		case 61:
            return (WDF_DEVICE_POWER_POLICY_STATE) ;
            break;
		case 62:
            return (WDF_DEVICE_POWER_POLICY_STATE) ;
            break;
		case 63:
            return (WDF_DEVICE_POWER_POLICY_STATE) ;
            break;
		case 64:
            return (WDF_DEVICE_POWER_POLICY_STATE) ;
            break;
		case 65:
            return (WDF_DEVICE_POWER_POLICY_STATE) ;
            break;
		case 66:
            return (WDF_DEVICE_POWER_POLICY_STATE) ;
            break;
		case 67:
            return (WDF_DEVICE_POWER_POLICY_STATE) ;
            break;
		case 68:
            return (WDF_DEVICE_POWER_POLICY_STATE) ;
            break;
		case 69:
            return (WDF_DEVICE_POWER_POLICY_STATE) ;
            break;
		case 70:
            return (WDF_DEVICE_POWER_POLICY_STATE) ;
            break;
		case 71:
            return (WDF_DEVICE_POWER_POLICY_STATE) ;
            break;
		case 72:
            return (WDF_DEVICE_POWER_POLICY_STATE) ;
            break;
		case 73:
            return (WDF_DEVICE_POWER_POLICY_STATE) ;
            break;
		case 74:
            return (WDF_DEVICE_POWER_POLICY_STATE) ;
            break;
		case 75:
            return (WDF_DEVICE_POWER_POLICY_STATE) ;
            break;
		case 76:
            return (WDF_DEVICE_POWER_POLICY_STATE) ;
            break;
		case 77:
            return (WDF_DEVICE_POWER_POLICY_STATE) ;
            break;
		case 78:
            return (WDF_DEVICE_POWER_POLICY_STATE) ;
            break;
		case 79:
            return (WDF_DEVICE_POWER_POLICY_STATE) ;
            break;
		case 80:
            return (WDF_DEVICE_POWER_POLICY_STATE) ;
            break;
		case 81:
            return (WDF_DEVICE_POWER_POLICY_STATE) ;
            break;
		case 82:
            return (WDF_DEVICE_POWER_POLICY_STATE) ;
            break;
		case 83:
            return (WDF_DEVICE_POWER_POLICY_STATE) ;
            break;
		case 84:
            return (WDF_DEVICE_POWER_POLICY_STATE) ;
            break;
		case 85:
        default:
            return (WDF_DEVICE_POWER_POLICY_STATE) WdfDevStatePowerNull;
            break;
    }*/
}/* wdf-harness-parts.c end */

/* wdf-harness-simple.c begin */
#if SDV_HARNESS==SDV_HARNESS_DRIVER_CREATE




void sdv_main ()
{
 
  NTSTATUS  ntstatus = STATUS_SUCCESS;


    ntstatus = fun_DriverEntry(hp_driver, hregistry_path);
    SdvAssume (NT_SUCCESS(ntstatus));

  /*  status = fun_WDF_DRIVER_DEVICE_ADD(hdriver, hdevice_init);
    SdvAssume (NT_SUCCESS(status));*/



#ifdef fun_WDF_DRIVER_UNLOAD
	fun_WDF_DRIVER_UNLOAD(hdriver);
#endif
	} 
#endif
/* wdf-harness-simple.c end */

/* wdf-harness-flat-simple.c begin */
/*
	Copyright (c) Microsoft Corporation.  All rights reserved.
*/

/*****************************************************************************

    wdf-harness-flat-simple.c provides a flat harness for exercising a WDF driver.

    The flat harness is useful for checking issues that are of a local
    nature, for example IRQL checking, simple resource allocation and
    synchronization primitives.

    The flat harness only calls the driver once.  For a more complete
    and realistic harness that calls the driver repeatedly see the PNP
    harness in pnp-harness.c

    The following variations of the flat harness are available:
        SDV_WDF_FLAT_SIMPLE_HARNESS

    The harnesses exercises the driver as follows:
        SDV_WDF_FLAT_SIMPLE_HARNESS =
            ALL KMDF callbacks

     
*****************************************************************************/

#define SDV_WDF_IS_FLAT_HARNESS() \
    ( \
		SDV_HARNESS==SDV_WDF_FLAT_SIMPLE_HARNESS \
	)


#if SDV_WDF_IS_FLAT_HARNESS()



WDFCHILDLIST										jchildlist;
PWDF_CHILD_IDENTIFICATION_DESCRIPTION_HEADER		jPWDF_CHILD_IDENTIFICATION_DESCRIPTION_HEADER;
PWDF_CHILD_IDENTIFICATION_DESCRIPTION_HEADER		sPWDF_CHILD_IDENTIFICATION_DESCRIPTION_HEADER;
PWDF_CHILD_IDENTIFICATION_DESCRIPTION_HEADER		dPWDF_CHILD_IDENTIFICATION_DESCRIPTION_HEADER;
PWDF_CHILD_IDENTIFICATION_DESCRIPTION_HEADER		jPWDF_CHILD_IDENTIFICATION_DESCRIPTION_HEADER;
PCWDF_DEVICE_PNP_NOTIFICATION_DATA					j_pnp_not_data;
PCWDF_DEVICE_POWER_NOTIFICATION_DATA				j_power_not_data;
PCWDF_DEVICE_POWER_POLICY_NOTIFICATION_DATA			j_power_pol_not_data;
WDF_POWER_DEVICE_STATE								pWDF_POWER_DEVICE_STATE;
WDF_POWER_DEVICE_STATE								tWDF_POWER_DEVICE_STATE;
WDFCMRESLIST										rawResources;
WDFCMRESLIST										transResources;
WDF_SPECIAL_FILE_TYPE								notificationType;
BOOLEAN												jbool;
DEVICE_RELATION_TYPE								relationType;
WDFDMAENABLER											jWdfEnabler;
WDFIORESREQLIST										jIoResourceRequirementsList;
WDFOBJECT											jAssociatedObject;
ULONG												jActionFlags;
size_t												jlength;
size_t												joblength;
size_t												jiblength;
ULONG												jIoControlCode;
WDFCONTEXT											jContext;
WDFOBJECT											jObject;
WDFCMRESLIST										jResources;
SYSTEM_POWER_STATE									jPowerState;
WDFDMATRANSACTION									jTransaction;
PVOID												jContext;
WDF_DMA_DIRECTION									jDirection;
PSCATTER_GATHER_LIST								jSglist;
WDFDPC												jDpc;
WDFTIMER											jTimer;
WDFWMIINSTANCE										jWmiInstance;
ULONG												jOutBufferSize;
PVOID												jOutBuffer;
PULONG												jBufferUsed;
ULONG												jInBufferSize;
PVOID												jInBuffer;
ULONG												jDataItemId;
ULONG												jMethodId;
PVOID												jBuffer;
WDFWORKITEM											jWorkItem;
LPGUID												jInterfaceType;
PINTERFACE											jExposedInterface;
PVOID												jExposedInterfaceSpecificData;
WDFWMIPROVIDER										jWmiProvider;
WDF_WMI_PROVIDER_CONTROL							jControl;
BOOLEAN												jEnable;
WDFFILEOBJECT                 jfileobject;
WDFMEMORY											Buffer;
NTSTATUS                      ntstatus_error=STATUS_UNSUCCESSFUL;
WDFUSBPIPE										hUsbPipe;
USBD_STATUS                   usbstatus_error=USBD_STATUS_DEV_NOT_RESPONDING;


void sdv_main ()
{
 
	LONG choice;
	
	NTSTATUS  ntstatus = STATUS_SUCCESS;
  
	choice = SdvMakeChoice();

    switch (choice) {
    
	case 0: 
		ntstatus = fun_DriverEntry(hp_driver, hregistry_path);
		SdvAssume (NT_SUCCESS(ntstatus));
		break;
	
	case 1: 
		#ifdef fun_WDF_DEVICE_CONTEXT_CLEANUP
			
			fun_WDF_DEVICE_CONTEXT_CLEANUP(hdriver);
		#endif
        break;

	case 2: 
        #ifdef fun_WDF_DEVICE_CONTEXT_DESTROY
			
			fun_WDF_DEVICE_CONTEXT_DESTROY(hdriver);
		#endif
        break;

	case 3: 
        #ifdef fun_WDF_IO_QUEUE_CONTEXT_CLEANUP_CALLBACK
			
			fun_WDF_IO_QUEUE_CONTEXT_CLEANUP_CALLBACK(hdriver);
		#endif
        break;

	case 4: 
 
        #ifdef fun_WDF_IO_QUEUE_CONTEXT_DESTROY_CALLBACK
			
			fun_WDF_IO_QUEUE_CONTEXT_DESTROY_CALLBACK(hdriver);
		#endif
		break;

	case 5: 
        #ifdef fun_WDF_CHILD_LIST_CREATE_DEVICE 
			
			fun_WDF_CHILD_LIST_CREATE_DEVICE(jchildlist,jPWDF_CHILD_IDENTIFICATION_DESCRIPTION_HEADER,hdevice_init);
		#endif
        break;

	case 6: 

        #ifdef fun_WDF_CHILD_LIST_SCAN_FOR_CHILDREN
			
			fun_WDF_CHILD_LIST_SCAN_FOR_CHILDREN(jchildlist);
		#endif
        break;

	case 7:  
        #ifdef fun_WDF_CHILD_LIST_IDENTIFICATION_DESCRIPTION_COPY

		
		fun_WDF_CHILD_LIST_IDENTIFICATION_DESCRIPTION_COPY(jchildlist,sPWDF_CHILD_IDENTIFICATION_DESCRIPTION_HEADER,dPWDF_CHILD_IDENTIFICATION_DESCRIPTION_HEADER);
		
		#endif
        break;
        
	case 8: 
        #ifdef fun_WDF_CHILD_LIST_IDENTIFICATION_DESCRIPTION_DUPLICATE
			
			
			fun_WDF_CHILD_LIST_IDENTIFICATION_DESCRIPTION_DUPLICATE(jchildlist,sPWDF_CHILD_IDENTIFICATION_DESCRIPTION_HEADER,dPWDF_CHILD_IDENTIFICATION_DESCRIPTION_HEADER);
		
		#endif
        break;

	case 9: 
        #ifdef fun_WDF_CHILD_LIST_IDENTIFICATION_DESCRIPTION_COMPARE

			
			fun_WDF_CHILD_LIST_IDENTIFICATION_DESCRIPTION_COMPARE(jchildlist,sPWDF_CHILD_IDENTIFICATION_DESCRIPTION_HEADER,dPWDF_CHILD_IDENTIFICATION_DESCRIPTION_HEADER);

		#endif
        break;

	case 10: 
        #ifdef fun_WDF_CHILD_LIST_IDENTIFICATION_DESCRIPTION_CLEANUP

			
			fun_WDF_CHILD_LIST_IDENTIFICATION_DESCRIPTION_CLEANUP(jchildlist,jPWDF_CHILD_IDENTIFICATION_DESCRIPTION_HEADER);

		#endif
        break;

	case 11: 
        #ifdef fun_WDF_CHILD_LIST_ADDRESS_DESCRIPTION_COPY
			
			fun_WDF_CHILD_LIST_ADDRESS_DESCRIPTION_COPY(jchildlist,sPWDF_CHILD_IDENTIFICATION_DESCRIPTION_HEADER,dPWDF_CHILD_IDENTIFICATION_DESCRIPTION_HEADER);
		#endif
        break;

	case 12: 
        #ifdef fun_WDF_CHILD_LIST_ADDRESS_DESCRIPTION_DUPLICATE
			
			fun_WDF_CHILD_LIST_ADDRESS_DESCRIPTION_DUPLICATE(jchildlist,sPWDF_CHILD_IDENTIFICATION_DESCRIPTION_HEADER,dPWDF_CHILD_IDENTIFICATION_DESCRIPTION_HEADER);
		#endif
        break;

	case 13: 
        #ifdef fun_WDF_CHILD_LIST_ADDRESS_DESCRIPTION_CLEANUP
			
			fun_WDF_CHILD_LIST_ADDRESS_DESCRIPTION_CLEANUP(jchildlist,jPWDF_CHILD_IDENTIFICATION_DESCRIPTION_HEADER);
		#endif
        break;

	case 14: 
        #ifdef fun_WDF_CHILD_LIST_DEVICE_REENUMERATED
			
			fun_WDF_CHILD_LIST_DEVICE_REENUMERATED(jchildlist,hdevice,sPWDF_CHILD_IDENTIFICATION_DESCRIPTION_HEADER,dPWDF_CHILD_IDENTIFICATION_DESCRIPTION_HEADER);
		#endif
        break;

	case 15:
        /*#ifdef fun_WDF_CLASS_EXTENSIONIN_BIND
			
			fun_WDF_CLASS_EXTENSIONIN_BIND(hdriver);
		#endif*/
        break;

	case 16:
        /*#ifdef fun_WDF_CLASS_EXTENSIONIN_UNBIND
			
			fun_WDF_CLASS_EXTENSIONIN_UNBIND(hdriver);
		#endif*/
        break;

	case 17: 
        #ifdef fun_WDF_DEVICE_SHUTDOWN_NOTIFICATION
			
			fun_WDF_DEVICE_SHUTDOWN_NOTIFICATION(hdevice);
		#endif
        break;

	case 18: 
        #ifdef fun_WDF_DRIVER_DEVICE_ADD
			
			fun_WDF_DRIVER_DEVICE_ADD(hdriver, hdevice_init);
		#endif
        break;

	case 19: 
        #ifdef fun_WDF_FILE_CLOSE
			
			fun_WDF_FILE_CLOSE(hfileobj);
		#endif
        break;

	case 20: 
        #ifdef fun_WDF_FILE_CLEANUP
			
			fun_WDF_FILE_CLEANUP(hfileobj);
		#endif
        break;

	case 21: 
        #ifdef fun_WDF_DEVICE_PNP_STATE_CHANGE_NOTIFICATION
			
			fun_WDF_DEVICE_PNP_STATE_CHANGE_NOTIFICATION(hdevice,j_pnp_not_data);
		#endif
        break;

	case 22: 
        #ifdef fun_WDF_DEVICE_POWER_STATE_CHANGE_NOTIFICATION
			
			fun_WDF_DEVICE_POWER_STATE_CHANGE_NOTIFICATION(hdevice,j_power_not_data);
		#endif
        break;

	case 23: 
        #ifdef fun_WDF_DEVICE_POWER_POLICY_STATE_CHANGE_NOTIFICATION
			
			fun_WDF_DEVICE_POWER_POLICY_STATE_CHANGE_NOTIFICATION(hdevice, j_power_pol_not_data);
		#endif
        break;

	case 24: 
        #ifdef fun_WDF_DEVICE_D0_ENTRY
			
			fun_WDF_DEVICE_D0_ENTRY(hdevice,pWDF_POWER_DEVICE_STATE);
		#endif
        break;

	case 25: 
		#ifdef fun_WDF_DEVICE_D0_ENTRY_POST_INTERRUPTS_ENABLED
			
			fun_WDF_DEVICE_D0_ENTRY_POST_INTERRUPTS_ENABLED(hdevice,pWDF_POWER_DEVICE_STATE);
		#endif
        break;

	case 26: 
        #ifdef fun_WDF_DEVICE_D0_EXIT
			
			fun_WDF_DEVICE_D0_EXIT(hdevice,tWDF_POWER_DEVICE_STATE);
		#endif
        break;

	case 27: 
        #ifdef fun_WDF_DEVICE_D0_EXIT_PRE_INTERRUPTS_DISABLED
			
			fun_WDF_DEVICE_D0_EXIT_PRE_INTERRUPTS_DISABLED(hdevice,tWDF_POWER_DEVICE_STATE);
		#endif
        break;

	case 28: 
        #ifdef fun_WDF_DEVICE_PREPARE_HARDWARE
			
			fun_WDF_DEVICE_PREPARE_HARDWARE(hdevice,rawResources,transResources);
		#endif
        break;

	case 29: 
        #ifdef fun_WDF_DEVICE_RELEASE_HARDWARE
			
			fun_WDF_DEVICE_RELEASE_HARDWARE(hdevice,transResources);
		#endif
        break;

	case 30: 
        #ifdef fun_WDF_DEVICE_SELF_MANAGED_IO_CLEANUP
			
			fun_WDF_DEVICE_SELF_MANAGED_IO_CLEANUP(hdevice);
		#endif
        break;

	case 31: 
        #ifdef fun_WDF_DEVICE_SELF_MANAGED_IO_FLUSH
			
			fun_WDF_DEVICE_SELF_MANAGED_IO_FLUSH(hdevice);
		#endif
        break;

	case 32: 
        #ifdef fun_WDF_DEVICE_SELF_MANAGED_IO_INIT
			
			fun_WDF_DEVICE_SELF_MANAGED_IO_INIT(hdevice);
		#endif
        break;

	case 33: 
        #ifdef fun_WDF_DEVICE_SELF_MANAGED_IO_SUSPEND
			
			fun_WDF_DEVICE_SELF_MANAGED_IO_SUSPEND(hdevice);
		#endif
        break;

	case 34: 
        #ifdef fun_WDF_DEVICE_SELF_MANAGED_IO_RESTART
			
			fun_WDF_DEVICE_SELF_MANAGED_IO_RESTART(hdevice);
		#endif
        break;

	case 35: 
        #ifdef fun_WDF_DEVICE_QUERY_STOP
			
			fun_WDF_DEVICE_QUERY_STOP(hdevice);
		#endif
        break;

	case 36: 
        #ifdef fun_WDF_DEVICE_QUERY_REMOVE
			
			fun_WDF_DEVICE_QUERY_REMOVE(hdevice);
		#endif
        break;

	case 37: 
        #ifdef fun_WDF_DEVICE_SURPRISE_REMOVAL
			
			fun_WDF_DEVICE_SURPRISE_REMOVAL(hdevice);
		#endif
        break;

	case 38: 
        #ifdef fun_WDF_DEVICE_USAGE_NOTIFICATION
			
			fun_WDF_DEVICE_USAGE_NOTIFICATION(hdevice, notificationType, jbool);
		#endif
        break;

	case 39: 
        #ifdef fun_WDF_DEVICE_RELATIONS_QUERY
			
			fun_WDF_DEVICE_RELATIONS_QUERY(hdevice, relationType);
		#endif
        break;

	case 40: 
        #ifdef fun_WDF_DEVICE_ARM_WAKE_FROM_S0
			
			fun_WDF_DEVICE_ARM_WAKE_FROM_S0(hdevice);
		#endif
        break;

	case 41: 
        #ifdef fun_WDF_DEVICE_ARM_WAKE_FROM_SX
			
			fun_WDF_DEVICE_ARM_WAKE_FROM_SX(hdevice);
		#endif
        break;

	case 42: 
        #ifdef fun_WDF_DEVICE_DISARM_WAKE_FROM_S0
			
			fun_WDF_DEVICE_DISARM_WAKE_FROM_S0(hdevice);
		#endif
        break;

	case 43: 
        #ifdef fun_WDF_DEVICE_DISARM_WAKE_FROM_SX
			
			fun_WDF_DEVICE_DISARM_WAKE_FROM_SX(hdevice);
		#endif
        break;

	case 44: 
        #ifdef fun_WDF_DEVICE_WAKE_FROM_S0_TRIGGERED
			fun_WDF_DEVICE_WAKE_FROM_S0_TRIGGERED(hdevice);
		#endif
        break;

	case 45: 
        #ifdef fun_WDF_DEVICE_WAKE_FROM_SX_TRIGGERED
			
			fun_WDF_DEVICE_WAKE_FROM_SX_TRIGGERED(hdevice);
		#endif
        break;

	case 46: 
        #ifdef fun_WDF_IO_IN_CALLER_CONTEXT
			
			fun_WDF_IO_IN_CALLER_CONTEXT(hdevice,hrequest);
		#endif
        break;

	case 47: 
        #ifdef fun_WDF_DMA_ENABLER_FILL
			
			fun_WDF_DMA_ENABLER_FILL(jWdfEnabler);
		#endif
        break;

	case 48: 
        #ifdef fun_WDF_DMA_ENABLER_FLUSH
			
			fun_WDF_DMA_ENABLER_FLUSH(jWdfEnabler);
		#endif
        break;

	case 49: 
        #ifdef fun_WDF_DMA_ENABLER_ENABLE
			
			fun_WDF_DMA_ENABLER_ENABLE(jWdfEnabler);
		#endif
        break;

	case 50: 
        #ifdef fun_WDF_DMA_ENABLER_DISABLE
			
			fun_WDF_DMA_ENABLER_DISABLE(jWdfEnabler);
		#endif
        break;

	case 51: 
        #ifdef fun_WDF_DMA_ENABLER_SELFMANAGED_IO_START
			
			fun_WDF_DMA_ENABLER_SELFMANAGED_IO_START(jWdfEnabler);
		#endif
        break;

	case 52: 
        #ifdef fun_WDF_DMA_ENABLER_SELFMANAGED_IO_STOP
			
			fun_WDF_DMA_ENABLER_SELFMANAGED_IO_STOP(jWdfEnabler);
		#endif
        break;

	case 53: 
        #ifdef fun_WDF_DRIVER_DEVICE_ADD
			
			fun_WDF_DRIVER_DEVICE_ADD(hdriver, hdevice_init);
		#endif
        break;

	case 54: 
        #ifdef fun_WDF_DRIVER_UNLOAD
			
			fun_WDF_DRIVER_UNLOAD(hdriver);
		#endif
        break;

	case 55: 
        /*#ifdef fun_WDF_TRACE_CALLBACK
			fun_WDF_TRACE_CALLBACK(hdriver);
		#endif*/
        break;

	case 56: 
        #ifdef fun_WDF_DEVICE_FILTER_RESOURCE_REQUIREMENTS_1
			
			fun_WDF_DEVICE_FILTER_RESOURCE_REQUIREMENTS_1(hdevice,jIoResourceRequirementsList);
		#endif
        break;

	case 57: 
        #ifdef fun_WDF_DEVICE_REMOVE_ADDED_RESOURCES
			
			fun_WDF_DEVICE_REMOVE_ADDED_RESOURCES(hdevice,rawResources,transResources);
		#endif
        break;

	case 58: 
        #ifdef fun_WDF_INTERRUPT_ISR
			
			fun_WDF_INTERRUPT_ISR(hinterrupt,hmessageID);
		#endif
        break;

	case 59: 
        #ifdef fun_WDF_INTERRUPT_DPC
			
			fun_WDF_INTERRUPT_DPC(hinterrupt,jAssociatedObject);
		#endif
        break;

	case 60: 
		#ifdef fun_WDF_INTERRUPT_ENABLE
			
			fun_WDF_INTERRUPT_ENABLE(hinterrupt, hdevice);
		#endif
        break;

	case 61: 
		#ifdef fun_WDF_INTERRUPT_DISABLE
			
			fun_WDF_INTERRUPT_DISABLE(hinterrupt, hdevice);
		#endif
        break;

	case 62: 
		#ifdef fun_WDF_IO_QUEUE_IO_DEFAULT
			
			fun_WDF_IO_QUEUE_IO_DEFAULT(hdefqueue, hrequest);
		#endif
        break;

	case 63: 
        #ifdef fun_WDF_IO_QUEUE_IO_STOP
			
			fun_WDF_IO_QUEUE_IO_STOP(hdefqueue, hrequest,jActionFlags);
		#endif
        break;

	case 64: 
        #ifdef fun_WDF_IO_QUEUE_IO_RESUME
			
			fun_WDF_IO_QUEUE_IO_RESUME(hdefqueue, hrequest);
		#endif
        break;

	case 65: 
        #ifdef fun_WDF_IO_QUEUE_IO_READ
			
			fun_WDF_IO_QUEUE_IO_READ(hdefqueue, hrequest, jlength);
		#endif
        break;

	case 66: 
        #ifdef fun_WDF_IO_QUEUE_IO_WRITE
			
			fun_WDF_IO_QUEUE_IO_WRITE(hdefqueue, hrequest, jlength);
		#endif
        break;

	case 67: 
        #ifdef fun_WDF_IO_QUEUE_IO_DEVICE_CONTROL
			
			fun_WDF_IO_QUEUE_IO_DEVICE_CONTROL(hdefqueue, hrequest, joblength, jiblength, jIoControlCode);
		#endif
        break;

	case 68: 
        #ifdef fun_WDF_IO_QUEUE_IO_INTERNAL_DEVICE_CONTROL
			
			fun_WDF_IO_QUEUE_IO_INTERNAL_DEVICE_CONTROL(hdefqueue, hrequest, joblength, jiblength, jIoControlCode);
		#endif
        break;

	case 69: 
        #ifdef fun_WDF_IO_QUEUE_IO_CANCELED_ON_QUEUE
			
			fun_WDF_IO_QUEUE_IO_CANCELED_ON_QUEUE(hdefqueue, hrequest);
		#endif
        break;

	case 70: 
		#ifdef fun_SDV_WDF_IO_QUEUE_STATE
			
			fun_SDV_WDF_IO_QUEUE_STATE(hdefqueue, jContext);
		#endif
        break;

	case 71: 
        #ifdef fun_WDF_IO_TARGET_QUERY_REMOVE
			
			fun_WDF_IO_TARGET_QUERY_REMOVE(hiotarget);
		#endif
        break;

	case 72: 
        #ifdef fun_WDF_IO_TARGET_REMOVE_CANCELED
			
			fun_WDF_IO_TARGET_REMOVE_CANCELED(hiotarget);
		#endif
        break;

	case 73: 
        #ifdef fun_WDF_IO_TARGET_REMOVE_COMPLETE
			
			fun_WDF_IO_TARGET_REMOVE_COMPLETE(hiotarget);
		#endif
        break;

	case 74: 
		#ifdef fun_WDF_OBJECT_CONTEXT_CLEANUP
			
			fun_WDF_OBJECT_CONTEXT_CLEANUP(jObject);
		#endif
        break;

	case 75: 
        #ifdef fun_WDF_OBJECT_CONTEXT_DESTROY
			
			fun_WDF_OBJECT_CONTEXT_DESTROY(jObject);
		#endif
        break;

	case 76: 
        #ifdef fun_WDF_DEVICE_RESOURCES_QUERY
			
			fun_WDF_DEVICE_RESOURCES_QUERY(hdevice, jResources);
		#endif
        break;

	case 77: 
        #ifdef fun_WDF_DEVICE_RESOURCE_REQUIREMENTS_QUERY
			
			fun_WDF_DEVICE_RESOURCE_REQUIREMENTS_QUERY(hdevice, jIoResourceRequirementsList);
		#endif
        break;

	case 78: 
        #ifdef fun_WDF_DEVICE_EJECT
			
			fun_WDF_DEVICE_EJECT(hdevice);
		#endif
        break;

	case 79: 
        #ifdef fun_WDF_DEVICE_SET_LOCK
			
			fun_WDF_DEVICE_SET_LOCK(hdevice, jbool);
		#endif
        break;

	case 80: 
        #ifdef fun_WDF_DEVICE_ENABLE_WAKE_AT_BUS
			
			fun_WDF_DEVICE_ENABLE_WAKE_AT_BUS(hdevice, jPowerState);
		#endif
        break;

	case 81: 
        #ifdef fun_WDF_DEVICE_DISABLE_WAKE_AT_BUS
			
			fun_WDF_DEVICE_DISABLE_WAKE_AT_BUS(hdevice);
		#endif
        break;

	case 82: 
        #ifdef fun_WDF_DEVICE_PROCESS_QUERY_INTERFACE_REQUEST
			
			fun_WDF_DEVICE_PROCESS_QUERY_INTERFACE_REQUEST(hdevice, jInterfaceType, jExposedInterface, jExposedInterfaceSpecificData);
		#endif
        break;

	case 83: 

        #ifdef fun_WDF_WMI_PROVIDER_FUNCTION_CONTROL
			
			fun_WDF_WMI_PROVIDER_FUNCTION_CONTROL(jWmiProvider, jControl, jEnable);
		#endif
        break;

	case 84:
        #ifdef fun_WDF_PROGRAM_DMA_1
			
			fun_WDF_PROGRAM_DMA_1(jTransaction,hdevice,jContext,jDirection,jSglist);
		#endif
        break;

	case 85:
        #ifdef fun_WDF_PROGRAM_DMA_2
			
			fun_WDF_PROGRAM_DMA_2(jTransaction,hdevice,jContext,jDirection,jSglist);
		#endif
        break;

	case 86:
        #ifdef fun_WDF_PROGRAM_DMA_3
			
			fun_WDF_PROGRAM_DMA_3(jTransaction,hdevice,jContext,jDirection,jSglist);
		#endif
        break;

	case 87:
        #ifdef fun_WDF_PROGRAM_DMA_4
			
			fun_WDF_PROGRAM_DMA_4(jTransaction,hdevice,jContext,jDirection,jSglist);
		#endif
        break;

	case 88:
        #ifdef fun_WDF_PROGRAM_DMA_5
			
			fun_WDF_PROGRAM_DMA_5(jTransaction,hdevice,jContext,jDirection,jSglist);
		#endif
        break;

	case 89:
        #ifdef fun_WDF_PROGRAM_DMA_6
			
			fun_WDF_PROGRAM_DMA_6(jTransaction,hdevice,jContext,jDirection,jSglist);
		#endif
        break;


	case 90:
        #ifdef fun_WDF_DPC_1
			
			fun_WDF_DPC_1(jDpc);
		#endif
        break;

	case 91:
        #ifdef fun_WDF_DPC_2
			
			fun_WDF_DPC_2(jDpc);
		#endif
        break;

	case 92:
        #ifdef fun_WDF_DPC_3
			
			fun_WDF_DPC_3(jDpc);
		#endif
        break;

	case 93:
        #ifdef fun_WDF_DPC_4
			
			fun_WDF_DPC_4(jDpc);
		#endif
        break;

	case 94:
        #ifdef fun_WDF_DPC_5
			
			fun_WDF_DPC_5(jDpc);
		#endif
        break;

	case 95:
        #ifdef fun_WDF_DPC_6
			
			fun_WDF_DPC_6(jDpc);
		#endif
        break;

	case 96:
        #ifdef fun_WDF_DPC_7
			
			fun_WDF_DPC_7(jDpc);
		#endif
        break;

	case 97:
        #ifdef fun_WDF_INTERRUPT_SYNCHRONIZE_1
			
			fun_WDF_INTERRUPT_SYNCHRONIZE_1(hinterrupt,jContext);
		#endif
        break;

	case 98:
        #ifdef fun_WDF_INTERRUPT_SYNCHRONIZE_2
			
			fun_WDF_INTERRUPT_SYNCHRONIZE_2(hinterrupt,jContext);
		#endif
        break;

	case 99:
        #ifdef fun_WDF_INTERRUPT_SYNCHRONIZE_3
			
			fun_WDF_INTERRUPT_SYNCHRONIZE_3(hinterrupt,jContext);
		#endif
        break;

	case 100:
        #ifdef fun_WDF_INTERRUPT_SYNCHRONIZE_4
			
			fun_WDF_INTERRUPT_SYNCHRONIZE_4(hinterrupt,jContext);
		#endif
        break;

	case 101:
        #ifdef fun_WDF_INTERRUPT_SYNCHRONIZE_5
			
			fun_WDF_INTERRUPT_SYNCHRONIZE_5(hinterrupt,jContext);
		#endif
        break;

	case 102:
        #ifdef fun_WDF_INTERRUPT_SYNCHRONIZE_6
			
			fun_WDF_INTERRUPT_SYNCHRONIZE_6(hinterrupt,jContext);
		#endif
        break;

	case 103:
        #ifdef fun_WDF_INTERRUPT_SYNCHRONIZE_7
			
			fun_WDF_INTERRUPT_SYNCHRONIZE_7(hinterrupt,jContext);
		#endif
        break;

	case 104:
        #ifdef fun_WDF_INTERRUPT_SYNCHRONIZE_8
			
			fun_WDF_INTERRUPT_SYNCHRONIZE_8(hinterrupt,jContext);
		#endif
        break;

	case 105:
        #ifdef fun_WDF_INTERRUPT_SYNCHRONIZE_9
			
			fun_WDF_INTERRUPT_SYNCHRONIZE_9(hinterrupt,jContext);
		#endif
        break;

	case 106:
        #ifdef fun_WDF_INTERRUPT_SYNCHRONIZE_10
			
			fun_WDF_INTERRUPT_SYNCHRONIZE_10(hinterrupt,jContext);
		#endif
        break;

	case 107:
        #ifdef fun_WDF_INTERRUPT_SYNCHRONIZE_11
			
			fun_WDF_INTERRUPT_SYNCHRONIZE_11(hinterrupt,jContext);
		#endif
        break;

	case 108:
        #ifdef fun_WDF_REQUEST_CANCEL_1
			
			fun_WDF_REQUEST_CANCEL_1(hrequest);
		#endif
        break;

	case 109:
        #ifdef fun_WDF_REQUEST_CANCEL_2
			
			fun_WDF_REQUEST_CANCEL_2(hrequest);
		#endif
        break;

	case 110:
        #ifdef fun_WDF_REQUEST_CANCEL_3
			
			fun_WDF_REQUEST_CANCEL_3(hrequest);
		#endif
        break;

	case 111:
        #ifdef fun_WDF_REQUEST_CANCEL_4
			
			fun_WDF_REQUEST_CANCEL_4(hrequest);
		#endif
        break;

	case 112:
        #ifdef fun_WDF_REQUEST_CANCEL_5
			
			fun_WDF_REQUEST_CANCEL_5(hrequest);
		#endif
        break;

	case 113:
        #ifdef fun_WDF_REQUEST_CANCEL_6
			
			fun_WDF_REQUEST_CANCEL_6(hrequest);
		#endif
        break;


	case 114:
        #ifdef fun_WDF_TIMER_1
			
			fun_WDF_TIMER_1(jTimer);
		#endif
        break;

	case 115:
        #ifdef fun_WDF_TIMER_2
			
			fun_WDF_TIMER_2(jTimer);
		#endif
        break;

	case 116:
        #ifdef fun_WDF_TIMER_3
			
			fun_WDF_TIMER_3(jTimer);
		#endif
        break;

	case 117:
        #ifdef fun_WDF_TIMER_4
			
			fun_WDF_TIMER_4(jTimer);
		#endif
        break;

	case 118:
        #ifdef fun_WDF_TIMER_5
			
			fun_WDF_TIMER_5(jTimer);
		#endif
        break;

	case 119:
        #ifdef fun_WDF_TIMER_6
			
			fun_WDF_TIMER_6(jTimer);
		#endif
        break;

	case 120:
        #ifdef fun_WDF_WMI_INSTANCE_QUERY_INSTANCE_1
			
			fun_WDF_WMI_INSTANCE_QUERY_INSTANCE_1(jWmiInstance,jOutBufferSize,jOutBuffer,jBufferUsed);
		#endif
        break;

	case 121:
        #ifdef fun_WDF_WMI_INSTANCE_QUERY_INSTANCE_2
			
			fun_WDF_WMI_INSTANCE_QUERY_INSTANCE_2(jWmiInstance,jOutBufferSize,jOutBuffer,jBufferUsed);
		#endif
        break;

	case 122:
        #ifdef fun_WDF_WMI_INSTANCE_QUERY_INSTANCE_3
			
			fun_WDF_WMI_INSTANCE_QUERY_INSTANCE_3(jWmiInstance,jOutBufferSize,jOutBuffer,jBufferUsed);
		#endif
        break;

	case 123:
        #ifdef fun_WDF_WMI_INSTANCE_QUERY_INSTANCE_4
			
			fun_WDF_WMI_INSTANCE_QUERY_INSTANCE_4(jWmiInstance,jOutBufferSize,jOutBuffer,jBufferUsed);
		#endif
        break;

	case 124:
        #ifdef fun_WDF_WMI_INSTANCE_QUERY_INSTANCE_5
			
			fun_WDF_WMI_INSTANCE_QUERY_INSTANCE_5(jWmiInstance,jOutBufferSize,jOutBuffer,jBufferUsed);
		#endif
        break;

	case 125:
        #ifdef fun_WDF_WMI_INSTANCE_SET_INSTANCE_1
			
			fun_WDF_WMI_INSTANCE_SET_INSTANCE_1(jWmiInstance,jInBufferSize,jInBuffer);
		#endif
        break;

	case 126:
        #ifdef fun_WDF_WMI_INSTANCE_SET_INSTANCE_2
			
			fun_WDF_WMI_INSTANCE_SET_INSTANCE_2(jWmiInstance,jInBufferSize,jInBuffer);
		#endif
        break;

	case 127:
        #ifdef fun_WDF_WMI_INSTANCE_SET_INSTANCE_3
			
			fun_WDF_WMI_INSTANCE_SET_INSTANCE_3(jWmiInstance,jInBufferSize,jInBuffer);
		#endif
        break;

	case 128:
        #ifdef fun_WDF_WMI_INSTANCE_SET_INSTANCE_4
			
			fun_WDF_WMI_INSTANCE_SET_INSTANCE_4(jWmiInstance,jInBufferSize,jInBuffer);
		#endif
        break;

	case 129:
        #ifdef fun_WDF_WMI_INSTANCE_SET_INSTANCE_5
			
			fun_WDF_WMI_INSTANCE_SET_INSTANCE_5(jWmiInstance,jInBufferSize,jInBuffer);
		#endif
        break;

	case 130:
        #ifdef fun_WDF_WMI_INSTANCE_SET_ITEM_1
			
			fun_WDF_WMI_INSTANCE_SET_ITEM_1(jWmiInstance,jDataItemId,jInBufferSize,jInBuffer);
		#endif
        break;

	case 131:
        #ifdef fun_WDF_WMI_INSTANCE_SET_ITEM_2
			
			fun_WDF_WMI_INSTANCE_SET_ITEM_2(jWmiInstance,jDataItemId,jInBufferSize,jInBuffer);
		#endif
        break;

	case 132:
        #ifdef fun_WDF_WMI_INSTANCE_SET_ITEM_3
			
			fun_WDF_WMI_INSTANCE_SET_ITEM_3(jWmiInstance,jDataItemId,jInBufferSize,jInBuffer);
		#endif
        break;

	case 133:
        #ifdef fun_WDF_WMI_INSTANCE_SET_ITEM_4
			
			fun_WDF_WMI_INSTANCE_SET_ITEM_4(jWmiInstance,jDataItemId,jInBufferSize,jInBuffer);
		#endif
        break;

	case 134:
        #ifdef fun_WDF_WMI_INSTANCE_SET_ITEM_5
			
			fun_WDF_WMI_INSTANCE_SET_ITEM_5(jWmiInstance,jDataItemId,jInBufferSize,jInBuffer);
		#endif
        break;

	case 135:
        #ifdef fun_WDF_WMI_INSTANCE_EXECUTE_METHOD_1
			
			fun_WDF_WMI_INSTANCE_EXECUTE_METHOD_1(jWmiInstance,jMethodId,jInBufferSize,jOutBufferSize,jBuffer,jBufferUsed);
		#endif
        break;

	case 136:
        #ifdef fun_WDF_WMI_INSTANCE_EXECUTE_METHOD_2
			
			fun_WDF_WMI_INSTANCE_EXECUTE_METHOD_2(jWmiInstance,jMethodId,jInBufferSize,jOutBufferSize,jBuffer,jBufferUsed);
		#endif
        break;


	case 137:
        #ifdef fun_WDF_WMI_INSTANCE_EXECUTE_METHOD_3
			
			fun_WDF_WMI_INSTANCE_EXECUTE_METHOD_3(jWmiInstance,jMethodId,jInBufferSize,jOutBufferSize,jBuffer,jBufferUsed);
		#endif
        break;


	case 138:
        #ifdef fun_WDF_WMI_INSTANCE_EXECUTE_METHOD_4
			
			fun_WDF_WMI_INSTANCE_EXECUTE_METHOD_4(jWmiInstance,jMethodId,jInBufferSize,jOutBufferSize,jBuffer,jBufferUsed);
		#endif
        break;


	case 139:
        #ifdef fun_WDF_WMI_INSTANCE_EXECUTE_METHOD_5
			
			fun_WDF_WMI_INSTANCE_EXECUTE_METHOD_5(jWmiInstance,jMethodId,jInBufferSize,jOutBufferSize,jBuffer,jBufferUsed);
		#endif
        break;

	case 140:
        #ifdef fun_WDF_WORKITEM_1
			
			fun_WDF_WORKITEM_1(jWorkItem);
		#endif
        break;

	case 141:
        #ifdef fun_WDF_WORKITEM_2
			
			fun_WDF_WORKITEM_2(jWorkItem);
		#endif
        break;

	case 142:
        #ifdef fun_WDF_WORKITEM_3
			
			fun_WDF_WORKITEM_3(jWorkItem);
		#endif
        break;

	case 143:
        #ifdef fun_WDF_WORKITEM_4
			
			fun_WDF_WORKITEM_4(jWorkItem);
		#endif
        break;

	case 144:
        #ifdef fun_WDF_WORKITEM_5
			
			fun_WDF_WORKITEM_5(jWorkItem);
		#endif
        break;

	case 145:
        #ifdef fun_WDF_WORKITEM_6
			
			fun_WDF_WORKITEM_6(jWorkItem);
		#endif
        break;


	case 146:
		#ifdef fun_WDFDEVICE_WDM_IRP_PREPROCESS_1
			fun_WDFDEVICE_WDM_IRP_PREPROCESS_1(hdevice,sdv_irp);
		#endif
        break;

	case 147:
		#ifdef fun_WDFDEVICE_WDM_IRP_PREPROCESS_2
			fun_WDFDEVICE_WDM_IRP_PREPROCESS_2(hdevice,sdv_irp);
		#endif
        break;

	case 148:
		#ifdef fun_WDFDEVICE_WDM_IRP_PREPROCESS_3
			fun_WDFDEVICE_WDM_IRP_PREPROCESS_3(hdevice,sdv_irp);
		#endif
        break;

	case 149:
		#ifdef fun_WDFDEVICE_WDM_IRP_PREPROCESS_4
			fun_WDFDEVICE_WDM_IRP_PREPROCESS_4(hdevice,sdv_irp);
		#endif
        break;

	case 150:
		#ifdef fun_WDFDEVICE_WDM_IRP_PREPROCESS_5
			fun_WDFDEVICE_WDM_IRP_PREPROCESS_5(hdevice,sdv_irp);
		#endif
        break;

	case 151:
		#ifdef fun_WDFDEVICE_WDM_IRP_PREPROCESS_6
			fun_WDFDEVICE_WDM_IRP_PREPROCESS_6(hdevice,sdv_irp);
		#endif
        break;
    case 152: 
		#ifdef fun_WDF_DEVICE_FILE_CREATE
			fun_WDF_DEVICE_FILE_CREATE(hdevice,hrequest,jfileobject);
		#endif
        break;
    case 153: 
        #ifdef fun_WDF_DEVICE_FILTER_RESOURCE_REQUIREMENTS_2
			
			fun_WDF_DEVICE_FILTER_RESOURCE_REQUIREMENTS_2(hdevice,jIoResourceRequirementsList);
		#endif
        break;
    case 154: 
        #ifdef fun_WDF_DEVICE_FILTER_RESOURCE_REQUIREMENTS_3
			
			fun_WDF_DEVICE_FILTER_RESOURCE_REQUIREMENTS_3(hdevice,jIoResourceRequirementsList);
		#endif
        break;
    case 155: 
        #ifdef fun_WDF_DEVICE_FILTER_RESOURCE_REQUIREMENTS_4
			
			fun_WDF_DEVICE_FILTER_RESOURCE_REQUIREMENTS_4(hdevice,jIoResourceRequirementsList);
		#endif
        break;
    case 156: 
        #ifdef fun_WDF_DEVICE_FILTER_RESOURCE_REQUIREMENTS_5
			
			fun_WDF_DEVICE_FILTER_RESOURCE_REQUIREMENTS_5(hdevice,jIoResourceRequirementsList);
		#endif
        break;
    case 157: 
        #ifdef fun_WDF_DEVICE_FILTER_RESOURCE_REQUIREMENTS_6
			
			fun_WDF_DEVICE_FILTER_RESOURCE_REQUIREMENTS_6(hdevice,jIoResourceRequirementsList);
		#endif
    break;
    case 158:
    #ifdef fun_WDF_USB_READERS_FAILED_1
		    fun_WDF_USB_READERS_FAILED_1(hUsbPipe,
											             ntstatus_error,
											             usbstatus_error);
	  #endif
	  break;
	  case 159:
    #ifdef fun_WDF_USB_READERS_FAILED_2
		    fun_WDF_USB_READERS_FAILED_2(hUsbPipe,
											             ntstatus_error,
											             usbstatus_error);
	  #endif
	  break;
	  case 160:
    #ifdef fun_WDF_USB_READERS_FAILED_3
			fun_WDF_USB_READERS_FAILED_3(hUsbPipe,
											             ntstatus_error,
											             usbstatus_error);
	  #endif
	  break;
	  case 161:
		#ifdef fun_WDF_USB_READER_COMPLETION_ROUTINE_1
		    fun_WDF_USB_READER_COMPLETION_ROUTINE_1 (hUsbPipe,
                                               Buffer,  
                                               jlength,   
                                               jContext);
	  #endif
		break;
	  case 162:
		#ifdef fun_WDF_USB_READER_COMPLETION_ROUTINE_2
		    fun_WDF_USB_READER_COMPLETION_ROUTINE_2 (hUsbPipe,
                                               Buffer,  
                                               jlength,   
                                               jContext);
	  #endif
		break;
	  case 163:
		#ifdef fun_WDF_USB_READER_COMPLETION_ROUTINE_3
		    fun_WDF_USB_READER_COMPLETION_ROUTINE_3 (hUsbPipe,
                                               Buffer,  
                                               jlength,   
                                               jContext);
	    #endif
		break;
	default:
		break;

	}
}

#endif
/* wdf-harness-flat-simple.c end */

/* wdf-harness-irql-semi-flat.c begin */
/*
	Copyright (c) Microsoft Corporation.  All rights reserved.
*/

/*****************************************************************************

    wdf-harness-irql-semi-flat.c provides a semi flat harness for exercising a WDF driver.

    The semi flat harness is useful for checking issues that are of a local
    nature, for example IRQL checking.

    The semi flat harness only calls the driver once.  For a more complete
    and realistic harness that calls the driver repeatedly see the PNP
    harness in pnp-harness.c

    The following variations of the flat harness are available:
        SDV_WDF_IRQL_SEMI_FLAT_HARNESS

    The harnesses exercises the driver as follows:
        SDV_WDF_IRQL_SEMI_FLAT_HARNESS =
            DriverEntry
            KMDF DeviceDriverAdd Callback
            then aLL KMDF callbacks.

     
*****************************************************************************/
        
#define SDV_WDF_IS_IRQL_SEMI_FLAT_HARNESS() \
    ( \
		SDV_HARNESS==SDV_WDF_IRQL_SEMI_FLAT_HARNESS \
	)


#if SDV_WDF_IS_IRQL_SEMI_FLAT_HARNESS()



WDFCHILDLIST										jchildlist;
PWDF_CHILD_IDENTIFICATION_DESCRIPTION_HEADER		jPWDF_CHILD_IDENTIFICATION_DESCRIPTION_HEADER;
PWDF_CHILD_IDENTIFICATION_DESCRIPTION_HEADER		sPWDF_CHILD_IDENTIFICATION_DESCRIPTION_HEADER;
PWDF_CHILD_IDENTIFICATION_DESCRIPTION_HEADER		dPWDF_CHILD_IDENTIFICATION_DESCRIPTION_HEADER;
PWDF_CHILD_IDENTIFICATION_DESCRIPTION_HEADER		jPWDF_CHILD_IDENTIFICATION_DESCRIPTION_HEADER;
PCWDF_DEVICE_PNP_NOTIFICATION_DATA					j_pnp_not_data;
PCWDF_DEVICE_POWER_NOTIFICATION_DATA				j_power_not_data;
PCWDF_DEVICE_POWER_POLICY_NOTIFICATION_DATA			j_power_pol_not_data;
WDF_POWER_DEVICE_STATE								pWDF_POWER_DEVICE_STATE;
WDF_POWER_DEVICE_STATE								tWDF_POWER_DEVICE_STATE;
WDFCMRESLIST										rawResources;
WDFCMRESLIST										transResources;
WDF_SPECIAL_FILE_TYPE								notificationType;
BOOLEAN												jbool;
DEVICE_RELATION_TYPE								relationType;
WDFDMAENABLER											jWdfEnabler;
WDFIORESREQLIST										jIoResourceRequirementsList;
WDFOBJECT											jAssociatedObject;
ULONG												jActionFlags;
size_t												jlength;
size_t												joblength;
size_t												jiblength;
ULONG												jIoControlCode;
WDFCONTEXT											jContext;
WDFOBJECT											jObject;
WDFCMRESLIST										jResources;
SYSTEM_POWER_STATE									jPowerState;
WDFDMATRANSACTION									jTransaction;
PVOID												jContext;
WDF_DMA_DIRECTION									jDirection;
PSCATTER_GATHER_LIST								jSglist;
WDFDPC												jDpc;
WDFTIMER											jTimer;
WDFWMIINSTANCE										jWmiInstance;
ULONG												jOutBufferSize;
PVOID												jOutBuffer;
PULONG												jBufferUsed;
ULONG												jInBufferSize;
PVOID												jInBuffer;
ULONG												jDataItemId;
ULONG												jMethodId;
PVOID												jBuffer;
WDFWORKITEM											jWorkItem;
LPGUID												jInterfaceType;
PINTERFACE											jExposedInterface;
PVOID												jExposedInterfaceSpecificData;
WDFWMIPROVIDER										jWmiProvider;
WDF_WMI_PROVIDER_CONTROL							jControl;
BOOLEAN												jEnable;
WDFFILEOBJECT                 jfileobject;
WDFIOTARGET                   jiotarget;
PWDF_REQUEST_COMPLETION_PARAMS jparams;
NTSTATUS                      ntstatus_error=STATUS_UNSUCCESSFUL;
WDFUSBPIPE										hUsbPipe;
USBD_STATUS                   usbstatus_error=USBD_STATUS_DEV_NOT_RESPONDING;
WDFMEMORY											  Buffer;



void Choice_Dispatch()
{
		int irql = SdvMakeChoice();
    switch(irql)
    {
    case 0:
      SDV_IRQL_PUSH(PASSIVE_LEVEL);
      break;
    default:
      SDV_IRQL_PUSH(DISPATCH_LEVEL);
      break;
    }

}

void sdv_main ()
{
 
  LONG choice;
	
  NTSTATUS  ntstatus = STATUS_SUCCESS;
  
  ntstatus = fun_DriverEntry(hp_driver, hregistry_path);
  SdvAssume (NT_SUCCESS(ntstatus));
  
    
  #ifdef fun_WDF_DRIVER_DEVICE_ADD
    SDV_IRQL_PUSH(PASSIVE_LEVEL);
	ntstatus=fun_WDF_DRIVER_DEVICE_ADD(hdriver, hdevice_init);
  #endif
  

  if(NT_SUCCESS(ntstatus))
  {
	choice = SdvMakeChoice();

  switch (choice) {
    
	case 0: 
		#ifdef fun_WDF_DEVICE_CONTEXT_CLEANUP
			SDV_IRQL_PUSH(PASSIVE_LEVEL);
			fun_WDF_DEVICE_CONTEXT_CLEANUP(hdriver);
		#endif
        break;
  
  case 1: 
		#ifdef fun_WDF_DEVICE_FILE_CREATE
			SDV_IRQL_PUSH(PASSIVE_LEVEL);
			fun_WDF_DEVICE_FILE_CREATE(hdevice,hrequest,jfileobject);
		#endif
        break;

	case 2: 
                #ifdef fun_WDF_DEVICE_CONTEXT_DESTROY
			Choice_Dispatch();
			fun_WDF_DEVICE_CONTEXT_DESTROY(hdriver);
		#endif
        break;

	case 3: 
                #ifdef fun_WDF_IO_QUEUE_CONTEXT_CLEANUP_CALLBACK
			SDV_IRQL_PUSH(PASSIVE_LEVEL);
			fun_WDF_IO_QUEUE_CONTEXT_CLEANUP_CALLBACK(hdriver);
		#endif
        break;

	case 4: 
 
                #ifdef fun_WDF_IO_QUEUE_CONTEXT_DESTROY_CALLBACK
			Choice_Dispatch();
			fun_WDF_IO_QUEUE_CONTEXT_DESTROY_CALLBACK(hdriver);
			#endif
			break;

	case 5: 
                #ifdef fun_WDF_CHILD_LIST_CREATE_DEVICE 
			SDV_IRQL_PUSH(PASSIVE_LEVEL);
			fun_WDF_CHILD_LIST_CREATE_DEVICE(jchildlist,jPWDF_CHILD_IDENTIFICATION_DESCRIPTION_HEADER,hdevice_init);
		#endif
        break;

	case 6: 

                #ifdef fun_WDF_CHILD_LIST_SCAN_FOR_CHILDREN
			SDV_IRQL_PUSH(PASSIVE_LEVEL);
			fun_WDF_CHILD_LIST_SCAN_FOR_CHILDREN(jchildlist);
		#endif
        break;

	case 7:  
                #ifdef fun_WDF_CHILD_LIST_IDENTIFICATION_DESCRIPTION_COPY
                        Choice_Dispatch();
                        fun_WDF_CHILD_LIST_IDENTIFICATION_DESCRIPTION_COPY(jchildlist,sPWDF_CHILD_IDENTIFICATION_DESCRIPTION_HEADER,dPWDF_CHILD_IDENTIFICATION_DESCRIPTION_HEADER);
		#endif
        break;
        
	case 8: 
                #ifdef fun_WDF_CHILD_LIST_IDENTIFICATION_DESCRIPTION_DUPLICATE
			Choice_Dispatch();
			fun_WDF_CHILD_LIST_IDENTIFICATION_DESCRIPTION_DUPLICATE(jchildlist,sPWDF_CHILD_IDENTIFICATION_DESCRIPTION_HEADER,dPWDF_CHILD_IDENTIFICATION_DESCRIPTION_HEADER);
		#endif
        break;

	case 9: 
                #ifdef fun_WDF_CHILD_LIST_IDENTIFICATION_DESCRIPTION_COMPARE
			Choice_Dispatch();
			fun_WDF_CHILD_LIST_IDENTIFICATION_DESCRIPTION_COMPARE(jchildlist,sPWDF_CHILD_IDENTIFICATION_DESCRIPTION_HEADER,dPWDF_CHILD_IDENTIFICATION_DESCRIPTION_HEADER);
		#endif
        break;

	case 10: 
                #ifdef fun_WDF_CHILD_LIST_IDENTIFICATION_DESCRIPTION_CLEANUP
			Choice_Dispatch();
			fun_WDF_CHILD_LIST_IDENTIFICATION_DESCRIPTION_CLEANUP(jchildlist,jPWDF_CHILD_IDENTIFICATION_DESCRIPTION_HEADER);
		#endif
        break;

	case 11: 
                #ifdef fun_WDF_CHILD_LIST_ADDRESS_DESCRIPTION_COPY
			Choice_Dispatch();
			fun_WDF_CHILD_LIST_ADDRESS_DESCRIPTION_COPY(jchildlist,sPWDF_CHILD_IDENTIFICATION_DESCRIPTION_HEADER,dPWDF_CHILD_IDENTIFICATION_DESCRIPTION_HEADER);
		#endif
        break;

	case 12: 
                #ifdef fun_WDF_CHILD_LIST_ADDRESS_DESCRIPTION_DUPLICATE
			Choice_Dispatch();
			fun_WDF_CHILD_LIST_ADDRESS_DESCRIPTION_DUPLICATE(jchildlist,sPWDF_CHILD_IDENTIFICATION_DESCRIPTION_HEADER,dPWDF_CHILD_IDENTIFICATION_DESCRIPTION_HEADER);
		#endif
        break;

	case 13: 
                #ifdef fun_WDF_CHILD_LIST_ADDRESS_DESCRIPTION_CLEANUP
			Choice_Dispatch();
			fun_WDF_CHILD_LIST_ADDRESS_DESCRIPTION_CLEANUP(jchildlist,jPWDF_CHILD_IDENTIFICATION_DESCRIPTION_HEADER);
		#endif
        break;

	case 14: 
                #ifdef fun_WDF_CHILD_LIST_DEVICE_REENUMERATED
			Choice_Dispatch();
			fun_WDF_CHILD_LIST_DEVICE_REENUMERATED(jchildlist,hdevice,sPWDF_CHILD_IDENTIFICATION_DESCRIPTION_HEADER,dPWDF_CHILD_IDENTIFICATION_DESCRIPTION_HEADER);
		#endif
        break;

	case 15:
        /*#ifdef fun_WDF_CLASS_EXTENSIONIN_BIND
			SDV_IRQL_PUSH(PASSIVE_LEVEL);
			fun_WDF_CLASS_EXTENSIONIN_BIND(hdriver);
		#endif*/
        break;

	case 16:
        /*#ifdef fun_WDF_CLASS_EXTENSIONIN_UNBIND
			SDV_IRQL_PUSH(PASSIVE_LEVEL);
			fun_WDF_CLASS_EXTENSIONIN_UNBIND(hdriver);
		#endif*/
        break;

	case 17: 
                #ifdef fun_WDF_DEVICE_SHUTDOWN_NOTIFICATION
			SDV_IRQL_PUSH(PASSIVE_LEVEL);
			fun_WDF_DEVICE_SHUTDOWN_NOTIFICATION(hdevice);
		#endif
        break;

	case 18: 
		

	case 19: 
        #ifdef fun_WDF_FILE_CLOSE
			SDV_IRQL_PUSH(PASSIVE_LEVEL);
			fun_WDF_FILE_CLOSE(hfileobj);
		#endif
        break;

	case 20: 
        #ifdef fun_WDF_FILE_CLEANUP
			SDV_IRQL_PUSH(PASSIVE_LEVEL);
			fun_WDF_FILE_CLEANUP(hfileobj);
		#endif
        break;

	case 21: 
                #ifdef fun_WDF_DEVICE_PNP_STATE_CHANGE_NOTIFICATION
			SDV_IRQL_PUSH(PASSIVE_LEVEL);
			fun_WDF_DEVICE_PNP_STATE_CHANGE_NOTIFICATION(hdevice,j_pnp_not_data);
		#endif
        break;

	case 22: 
                #ifdef fun_WDF_DEVICE_POWER_STATE_CHANGE_NOTIFICATION
			SDV_IRQL_PUSH(PASSIVE_LEVEL);
			fun_WDF_DEVICE_POWER_STATE_CHANGE_NOTIFICATION(hdevice,j_power_not_data);
		#endif
        break;

	case 23: 
                #ifdef fun_WDF_DEVICE_POWER_POLICY_STATE_CHANGE_NOTIFICATION
			SDV_IRQL_PUSH(PASSIVE_LEVEL);
			fun_WDF_DEVICE_POWER_POLICY_STATE_CHANGE_NOTIFICATION(hdevice, j_power_pol_not_data);
		#endif
        break;

	case 24: 
                #ifdef fun_WDF_DEVICE_D0_ENTRY
			SDV_IRQL_PUSH(PASSIVE_LEVEL);
			fun_WDF_DEVICE_D0_ENTRY(hdevice,pWDF_POWER_DEVICE_STATE);
		#endif
        break;

	case 25: 
		#ifdef fun_WDF_DEVICE_D0_ENTRY_POST_INTERRUPTS_ENABLED
			SDV_IRQL_PUSH(PASSIVE_LEVEL);
			fun_WDF_DEVICE_D0_ENTRY_POST_INTERRUPTS_ENABLED(hdevice,pWDF_POWER_DEVICE_STATE);
		#endif
        break;

	case 26: 
                #ifdef fun_WDF_DEVICE_D0_EXIT
			SDV_IRQL_PUSH(PASSIVE_LEVEL);
			fun_WDF_DEVICE_D0_EXIT(hdevice,tWDF_POWER_DEVICE_STATE);
		#endif
        break;

	case 27: 
                #ifdef fun_WDF_DEVICE_D0_EXIT_PRE_INTERRUPTS_DISABLED
			SDV_IRQL_PUSH(PASSIVE_LEVEL);
			fun_WDF_DEVICE_D0_EXIT_PRE_INTERRUPTS_DISABLED(hdevice,tWDF_POWER_DEVICE_STATE);
		#endif
        break;

	case 28: 
                #ifdef fun_WDF_DEVICE_PREPARE_HARDWARE
			SDV_IRQL_PUSH(PASSIVE_LEVEL);
			fun_WDF_DEVICE_PREPARE_HARDWARE(hdevice,rawResources,transResources);
		#endif
        break;

	case 29: 
                #ifdef fun_WDF_DEVICE_RELEASE_HARDWARE
			SDV_IRQL_PUSH(PASSIVE_LEVEL);
			fun_WDF_DEVICE_RELEASE_HARDWARE(hdevice,transResources);
		#endif
        break;

	case 30: 
                #ifdef fun_WDF_DEVICE_SELF_MANAGED_IO_CLEANUP
			SDV_IRQL_PUSH(PASSIVE_LEVEL);
			fun_WDF_DEVICE_SELF_MANAGED_IO_CLEANUP(hdevice);
		#endif
        break;

	case 31: 
                #ifdef fun_WDF_DEVICE_SELF_MANAGED_IO_FLUSH
			SDV_IRQL_PUSH(PASSIVE_LEVEL);
			fun_WDF_DEVICE_SELF_MANAGED_IO_FLUSH(hdevice);
		#endif
        break;

	case 32: 
                #ifdef fun_WDF_DEVICE_SELF_MANAGED_IO_INIT
			SDV_IRQL_PUSH(PASSIVE_LEVEL);
			fun_WDF_DEVICE_SELF_MANAGED_IO_INIT(hdevice);
		#endif
        break;

	case 33: 
                #ifdef fun_WDF_DEVICE_SELF_MANAGED_IO_SUSPEND
			SDV_IRQL_PUSH(PASSIVE_LEVEL);
			fun_WDF_DEVICE_SELF_MANAGED_IO_SUSPEND(hdevice);
		#endif
        break;

	case 34: 
                #ifdef fun_WDF_DEVICE_SELF_MANAGED_IO_RESTART
			SDV_IRQL_PUSH(PASSIVE_LEVEL);
			fun_WDF_DEVICE_SELF_MANAGED_IO_RESTART(hdevice);
		#endif
        break;

	case 35: 
                #ifdef fun_WDF_DEVICE_QUERY_STOP
			SDV_IRQL_PUSH(PASSIVE_LEVEL);
			fun_WDF_DEVICE_QUERY_STOP(hdevice);
		#endif
        break;

	case 36: 
                #ifdef fun_WDF_DEVICE_QUERY_REMOVE
			SDV_IRQL_PUSH(PASSIVE_LEVEL);
			fun_WDF_DEVICE_QUERY_REMOVE(hdevice);
		#endif
        break;

	case 37: 
                #ifdef fun_WDF_DEVICE_SURPRISE_REMOVAL
			SDV_IRQL_PUSH(PASSIVE_LEVEL);
			fun_WDF_DEVICE_SURPRISE_REMOVAL(hdevice);
		#endif
        break;

	case 38: 
                #ifdef fun_WDF_DEVICE_USAGE_NOTIFICATION
			SDV_IRQL_PUSH(PASSIVE_LEVEL);
			fun_WDF_DEVICE_USAGE_NOTIFICATION(hdevice, notificationType, jbool);
		#endif
        break;

	case 39: 
                #ifdef fun_WDF_DEVICE_RELATIONS_QUERY
			SDV_IRQL_PUSH(PASSIVE_LEVEL);
			fun_WDF_DEVICE_RELATIONS_QUERY(hdevice, relationType);
		#endif
        break;

	case 40: 
                #ifdef fun_WDF_DEVICE_ARM_WAKE_FROM_S0
			SDV_IRQL_PUSH(PASSIVE_LEVEL);
			fun_WDF_DEVICE_ARM_WAKE_FROM_S0(hdevice);
		#endif
        break;

	case 41: 
                #ifdef fun_WDF_DEVICE_ARM_WAKE_FROM_SX
			SDV_IRQL_PUSH(PASSIVE_LEVEL);
			fun_WDF_DEVICE_ARM_WAKE_FROM_SX(hdevice);
		#endif
        break;

	case 42: 
                #ifdef fun_WDF_DEVICE_DISARM_WAKE_FROM_S0
			SDV_IRQL_PUSH(PASSIVE_LEVEL);
			fun_WDF_DEVICE_DISARM_WAKE_FROM_S0(hdevice);
		#endif
        break;

	case 43: 
                #ifdef fun_WDF_DEVICE_DISARM_WAKE_FROM_SX
			SDV_IRQL_PUSH(PASSIVE_LEVEL);
			fun_WDF_DEVICE_DISARM_WAKE_FROM_SX(hdevice);
		#endif
        break;

	case 44: 
                #ifdef fun_WDF_DEVICE_WAKE_FROM_S0_TRIGGERED
			SDV_IRQL_PUSH(PASSIVE_LEVEL);
			fun_WDF_DEVICE_WAKE_FROM_S0_TRIGGERED(hdevice);
		#endif
        break;

	case 45: 
                #ifdef fun_WDF_DEVICE_WAKE_FROM_SX_TRIGGERED
			SDV_IRQL_PUSH(PASSIVE_LEVEL);
			fun_WDF_DEVICE_WAKE_FROM_SX_TRIGGERED(hdevice);
		#endif
        break;

	case 46: 
                #ifdef fun_WDF_IO_IN_CALLER_CONTEXT
			SDV_IRQL_PUSH(PASSIVE_LEVEL);
			fun_WDF_IO_IN_CALLER_CONTEXT(hdevice,hrequest);
		#endif
        break;

	case 47: 
                #ifdef fun_WDF_DMA_ENABLER_FILL
			SDV_IRQL_PUSH(PASSIVE_LEVEL);
			fun_WDF_DMA_ENABLER_FILL(jWdfEnabler);
		#endif
        break;

	case 48: 
                #ifdef fun_WDF_DMA_ENABLER_FLUSH
			SDV_IRQL_PUSH(PASSIVE_LEVEL);
			fun_WDF_DMA_ENABLER_FLUSH(jWdfEnabler);
		#endif
        break;

	case 49: 
                #ifdef fun_WDF_DMA_ENABLER_ENABLE
			SDV_IRQL_PUSH(PASSIVE_LEVEL);
			fun_WDF_DMA_ENABLER_ENABLE(jWdfEnabler);
		#endif
        break;

	case 50: 
                #ifdef fun_WDF_DMA_ENABLER_DISABLE
			SDV_IRQL_PUSH(PASSIVE_LEVEL);
			fun_WDF_DMA_ENABLER_DISABLE(jWdfEnabler);
		#endif
        break;

	case 51: 
                #ifdef fun_WDF_DMA_ENABLER_SELFMANAGED_IO_START
			SDV_IRQL_PUSH(PASSIVE_LEVEL);
			fun_WDF_DMA_ENABLER_SELFMANAGED_IO_START(jWdfEnabler);
		#endif
        break;

	case 52: 
                #ifdef fun_WDF_DMA_ENABLER_SELFMANAGED_IO_STOP
			SDV_IRQL_PUSH(PASSIVE_LEVEL);
			fun_WDF_DMA_ENABLER_SELFMANAGED_IO_STOP(jWdfEnabler);
		#endif
        break;

	case 53: 
                 #ifdef fun_WDF_DRIVER_DEVICE_ADD
			SDV_IRQL_PUSH(PASSIVE_LEVEL);
			fun_WDF_DRIVER_DEVICE_ADD(hdriver, hdevice_init);
		#endif
        break;

	case 54: 
                #ifdef fun_WDF_DRIVER_UNLOAD
			SDV_IRQL_PUSH(PASSIVE_LEVEL);
			fun_WDF_DRIVER_UNLOAD(hdriver);
		#endif
        break;

	case 55: 
        /*#ifdef fun_WDF_TRACE_CALLBACK
			fun_WDF_TRACE_CALLBACK(hdriver);
		#endif*/
        break;

	case 56: 
        #ifdef fun_WDF_DEVICE_FILTER_RESOURCE_REQUIREMENTS_1
			SDV_IRQL_PUSH(PASSIVE_LEVEL);
			fun_WDF_DEVICE_FILTER_RESOURCE_REQUIREMENTS_1(hdevice,jIoResourceRequirementsList);
		#endif
        break;

	case 57: 
                #ifdef fun_WDF_DEVICE_REMOVE_ADDED_RESOURCES
			SDV_IRQL_PUSH(PASSIVE_LEVEL);
			fun_WDF_DEVICE_REMOVE_ADDED_RESOURCES(hdevice,rawResources,transResources);
		#endif
        break;

	case 58: 
                #ifdef fun_WDF_INTERRUPT_ISR
			SDV_IRQL_PUSH(DISPATCH_LEVEL + 1);
			fun_WDF_INTERRUPT_ISR(hinterrupt,hmessageID);
		#endif
        break;

	/*case 59: 
                #ifdef fun_WDF_INTERRUPT_DPC
			SDV_IRQL_PUSH(DISPATCH_LEVEL);
			fun_WDF_INTERRUPT_DPC(hinterrupt,jAssociatedObject);
		#endif
        break;*/

	case 60: 
		#ifdef fun_WDF_INTERRUPT_ENABLE
			SDV_IRQL_PUSH(DISPATCH_LEVEL + 1);
			fun_WDF_INTERRUPT_ENABLE(hinterrupt, hdevice);
		#endif
        break;

	case 61: 
		#ifdef fun_WDF_INTERRUPT_DISABLE
			SDV_IRQL_PUSH(DISPATCH_LEVEL + 1);
			fun_WDF_INTERRUPT_DISABLE(hinterrupt, hdevice);
		#endif
        break;

	case 62: 
                #ifdef fun_WDF_IO_QUEUE_IO_STOP
                if(!stop_queue_no_attrib)
                {
		    SDV_IRQL_PUSH(stop_queue_stored_irql);
		    fun_WDF_IO_QUEUE_IO_STOP(hdefqueue, hrequest,jActionFlags);
	        }
		#endif
        break;

	case 63: 
                #ifdef fun_WDF_IO_QUEUE_IO_RESUME
                if(!resume_queue_no_attrib)
                {
		    SDV_IRQL_PUSH(resume_queue_stored_irql);
	            fun_WDF_IO_QUEUE_IO_RESUME(hdefqueue, hrequest);
		}
		#endif
        break;

	case 64: 
                #ifdef fun_WDF_IO_QUEUE_IO_CANCELED_ON_QUEUE
                if(!io_cancel_on_queue_no_attrib)
                {
		    SDV_IRQL_PUSH(io_cancel_on_queue_stored_irql);
		    fun_WDF_IO_QUEUE_IO_CANCELED_ON_QUEUE(hdefqueue, hrequest);
	        } 
		#endif
        break;

	case 65: 
		#ifdef fun_SDV_WDF_IO_QUEUE_STATE
			Choice_Dispatch();
			fun_SDV_WDF_IO_QUEUE_STATE(hdefqueue, jContext);
		#endif
        break;

	case 66: 
                #ifdef fun_WDF_IO_TARGET_QUERY_REMOVE
			SDV_IRQL_PUSH(PASSIVE_LEVEL);
			fun_WDF_IO_TARGET_QUERY_REMOVE(hiotarget);
		#endif
        break;

	case 67: 
                #ifdef fun_WDF_IO_TARGET_REMOVE_CANCELED
			SDV_IRQL_PUSH(PASSIVE_LEVEL);
			fun_WDF_IO_TARGET_REMOVE_CANCELED(hiotarget);
		#endif
        break;

	case 68: 
                #ifdef fun_WDF_IO_TARGET_REMOVE_COMPLETE
			SDV_IRQL_PUSH(PASSIVE_LEVEL);
			fun_WDF_IO_TARGET_REMOVE_COMPLETE(hiotarget);
		#endif
        break;

	case 69: 
		#ifdef fun_WDF_OBJECT_CONTEXT_CLEANUP
		    #ifndef NO_fun_WDF_OBJECT_CONTEXT_CLEANUP 
			    Choice_Dispatch();
			    fun_WDF_OBJECT_CONTEXT_CLEANUP(jObject);
			  #endif
		#endif
        break;

	case 70: 
                #ifdef fun_WDF_OBJECT_CONTEXT_DESTROY
			Choice_Dispatch();
			fun_WDF_OBJECT_CONTEXT_DESTROY(jObject);
		#endif
        break;

	case 71: 
                #ifdef fun_WDF_DEVICE_RESOURCES_QUERY
			SDV_IRQL_PUSH(PASSIVE_LEVEL);
			fun_WDF_DEVICE_RESOURCES_QUERY(hdevice, jResources);
		#endif
        break;

	case 72: 
                #ifdef fun_WDF_DEVICE_RESOURCE_REQUIREMENTS_QUERY
			SDV_IRQL_PUSH(PASSIVE_LEVEL);
			fun_WDF_DEVICE_RESOURCE_REQUIREMENTS_QUERY(hdevice, jIoResourceRequirementsList);
		#endif
        break;

	case 73: 
                #ifdef fun_WDF_DEVICE_EJECT
			SDV_IRQL_PUSH(PASSIVE_LEVEL);
			fun_WDF_DEVICE_EJECT(hdevice);
		#endif
        break;

	case 74: 
                #ifdef fun_WDF_DEVICE_SET_LOCK
			SDV_IRQL_PUSH(PASSIVE_LEVEL);
			fun_WDF_DEVICE_SET_LOCK(hdevice, jbool);
		#endif
        break;

	case 75: 
                #ifdef fun_WDF_DEVICE_ENABLE_WAKE_AT_BUS
			SDV_IRQL_PUSH(PASSIVE_LEVEL);
			fun_WDF_DEVICE_ENABLE_WAKE_AT_BUS(hdevice, jPowerState);
		#endif
        break;

	case 76: 
                #ifdef fun_WDF_DEVICE_DISABLE_WAKE_AT_BUS
			SDV_IRQL_PUSH(PASSIVE_LEVEL);
			fun_WDF_DEVICE_DISABLE_WAKE_AT_BUS(hdevice);
		#endif
        break;

	case 77: 
                #ifdef fun_WDF_DEVICE_PROCESS_QUERY_INTERFACE_REQUEST
			SDV_IRQL_PUSH(PASSIVE_LEVEL);
			fun_WDF_DEVICE_PROCESS_QUERY_INTERFACE_REQUEST(hdevice, jInterfaceType, jExposedInterface, jExposedInterfaceSpecificData);
		#endif
        break;

	case 78: 
                #ifdef fun_WDF_WMI_PROVIDER_FUNCTION_CONTROL
			SDV_IRQL_PUSH(PASSIVE_LEVEL);
			fun_WDF_WMI_PROVIDER_FUNCTION_CONTROL(jWmiProvider, jControl, jEnable);
		#endif
        break;

	case 79:
                #ifdef fun_WDF_PROGRAM_DMA_1
			SDV_IRQL_PUSH(DISPATCH_LEVEL);
			fun_WDF_PROGRAM_DMA_1(jTransaction,hdevice,jContext,jDirection,jSglist);
		#endif
        break;

	case 80:
                #ifdef fun_WDF_PROGRAM_DMA_2
			SDV_IRQL_PUSH(DISPATCH_LEVEL);
			fun_WDF_PROGRAM_DMA_2(jTransaction,hdevice,jContext,jDirection,jSglist);
		#endif
        break;

	case 81:
                #ifdef fun_WDF_PROGRAM_DMA_3
			SDV_IRQL_PUSH(DISPATCH_LEVEL);
			fun_WDF_PROGRAM_DMA_3(jTransaction,hdevice,jContext,jDirection,jSglist);
		#endif
        break;

	case 82:
                #ifdef fun_WDF_PROGRAM_DMA_4
			SDV_IRQL_PUSH(DISPATCH_LEVEL);
			fun_WDF_PROGRAM_DMA_4(jTransaction,hdevice,jContext,jDirection,jSglist);
		#endif
        break;

	case 83:
                #ifdef fun_WDF_PROGRAM_DMA_5
			SDV_IRQL_PUSH(DISPATCH_LEVEL);
			fun_WDF_PROGRAM_DMA_5(jTransaction,hdevice,jContext,jDirection,jSglist);
		#endif
        break;

	case 84:
                #ifdef fun_WDF_PROGRAM_DMA_6
			SDV_IRQL_PUSH(DISPATCH_LEVEL);
			fun_WDF_PROGRAM_DMA_6(jTransaction,hdevice,jContext,jDirection,jSglist);
		#endif
        break;


	/*case 85:
                #ifdef fun_WDF_DPC_1
			SDV_IRQL_PUSH(DISPATCH_LEVEL);
			fun_WDF_DPC_1(jDpc);
		#endif
        break;

	case 86:
                #ifdef fun_WDF_DPC_2
			SDV_IRQL_PUSH(DISPATCH_LEVEL);
			fun_WDF_DPC_2(jDpc);
		#endif
        break;

	case 87:
                #ifdef fun_WDF_DPC_3
			SDV_IRQL_PUSH(DISPATCH_LEVEL);
			fun_WDF_DPC_3(jDpc);
		#endif
        break;

	case 88:
                #ifdef fun_WDF_DPC_4
			SDV_IRQL_PUSH(DISPATCH_LEVEL);
			fun_WDF_DPC_4(jDpc);
		#endif
        break;

	case 89:
                #ifdef fun_WDF_DPC_5
			SDV_IRQL_PUSH(DISPATCH_LEVEL);
			fun_WDF_DPC_5(jDpc);
		#endif
        break;

	case 90:
                #ifdef fun_WDF_DPC_6
			SDV_IRQL_PUSH(DISPATCH_LEVEL);
			fun_WDF_DPC_6(jDpc);
		#endif
        break;

	case 91:
               #ifdef fun_WDF_DPC_7
			SDV_IRQL_PUSH(DISPATCH_LEVEL);
			fun_WDF_DPC_7(jDpc);
		#endif
        break;*/

	case 92:
                #ifdef fun_WDF_INTERRUPT_SYNCHRONIZE_1
			SDV_IRQL_PUSH(DISPATCH_LEVEL + 1);
			fun_WDF_INTERRUPT_SYNCHRONIZE_1(hinterrupt,jContext);
		#endif
        break;

	case 93:
                #ifdef fun_WDF_INTERRUPT_SYNCHRONIZE_2
			SDV_IRQL_PUSH(DISPATCH_LEVEL + 1);
			fun_WDF_INTERRUPT_SYNCHRONIZE_2(hinterrupt,jContext);
		#endif
        break;

	case 94:
                #ifdef fun_WDF_INTERRUPT_SYNCHRONIZE_3
			SDV_IRQL_PUSH(DISPATCH_LEVEL + 1);
			fun_WDF_INTERRUPT_SYNCHRONIZE_3(hinterrupt,jContext);
		#endif
        break;

	case 95:
                #ifdef fun_WDF_INTERRUPT_SYNCHRONIZE_4
			SDV_IRQL_PUSH(DISPATCH_LEVEL + 1);
			fun_WDF_INTERRUPT_SYNCHRONIZE_4(hinterrupt,jContext);
		#endif
        break;

	case 96:
                #ifdef fun_WDF_INTERRUPT_SYNCHRONIZE_5
			SDV_IRQL_PUSH(DISPATCH_LEVEL + 1);
			fun_WDF_INTERRUPT_SYNCHRONIZE_5(hinterrupt,jContext);
		#endif
        break;

	case 97:
                #ifdef fun_WDF_INTERRUPT_SYNCHRONIZE_6
			SDV_IRQL_PUSH(DISPATCH_LEVEL + 1);
			fun_WDF_INTERRUPT_SYNCHRONIZE_6(hinterrupt,jContext);
		#endif
        break;

	case 98:
                #ifdef fun_WDF_INTERRUPT_SYNCHRONIZE_7
			SDV_IRQL_PUSH(DISPATCH_LEVEL + 1);
			fun_WDF_INTERRUPT_SYNCHRONIZE_7(hinterrupt,jContext);
		#endif
        break;

	case 99:
                #ifdef fun_WDF_INTERRUPT_SYNCHRONIZE_8
			SDV_IRQL_PUSH(DISPATCH_LEVEL + 1);
			fun_WDF_INTERRUPT_SYNCHRONIZE_8(hinterrupt,jContext);
		#endif
        break;

	case 100:
                #ifdef fun_WDF_INTERRUPT_SYNCHRONIZE_9
			SDV_IRQL_PUSH(DISPATCH_LEVEL + 1);
			fun_WDF_INTERRUPT_SYNCHRONIZE_9(hinterrupt,jContext);
		#endif
        break;

	case 101:
                #ifdef fun_WDF_INTERRUPT_SYNCHRONIZE_10
			SDV_IRQL_PUSH(DISPATCH_LEVEL + 1);
			fun_WDF_INTERRUPT_SYNCHRONIZE_10(hinterrupt,jContext);
		#endif
        break;

	case 102:
                #ifdef fun_WDF_INTERRUPT_SYNCHRONIZE_11
			SDV_IRQL_PUSH(DISPATCH_LEVEL + 1);
			fun_WDF_INTERRUPT_SYNCHRONIZE_11(hinterrupt,jContext);
		#endif
        break;

	/*case 103:
                #ifdef fun_WDF_TIMER_1
			SDV_IRQL_PUSH(DISPATCH_LEVEL);
			fun_WDF_TIMER_1(jTimer);
		#endif
        break;

	case 104:
                #ifdef fun_WDF_TIMER_2
			SDV_IRQL_PUSH(DISPATCH_LEVEL);
			fun_WDF_TIMER_2(jTimer);
		#endif
        break;

	case 105:
                #ifdef fun_WDF_TIMER_3
			SDV_IRQL_PUSH(DISPATCH_LEVEL);
			fun_WDF_TIMER_3(jTimer);
		#endif
        break;

	case 106:
                #ifdef fun_WDF_TIMER_4
			SDV_IRQL_PUSH(DISPATCH_LEVEL);
			fun_WDF_TIMER_4(jTimer);
		#endif
        break;

	case 107:
                #ifdef fun_WDF_TIMER_5
			SDV_IRQL_PUSH(DISPATCH_LEVEL);
			fun_WDF_TIMER_5(jTimer);
		#endif
        break;

	case 108:
                #ifdef fun_WDF_TIMER_6
			SDV_IRQL_PUSH(DISPATCH_LEVEL);
			fun_WDF_TIMER_6(jTimer);
		#endif
        break;*/

	case 109:
                #ifdef fun_WDF_WMI_INSTANCE_QUERY_INSTANCE_1
			SDV_IRQL_PUSH(PASSIVE_LEVEL);
			fun_WDF_WMI_INSTANCE_QUERY_INSTANCE_1(jWmiInstance,jOutBufferSize,jOutBuffer,jBufferUsed);
		#endif
        break;

	case 110:
                #ifdef fun_WDF_WMI_INSTANCE_QUERY_INSTANCE_2
			SDV_IRQL_PUSH(PASSIVE_LEVEL);
			fun_WDF_WMI_INSTANCE_QUERY_INSTANCE_2(jWmiInstance,jOutBufferSize,jOutBuffer,jBufferUsed);
		#endif
        break;

	case 111:
                #ifdef fun_WDF_WMI_INSTANCE_QUERY_INSTANCE_3
			SDV_IRQL_PUSH(PASSIVE_LEVEL);
			fun_WDF_WMI_INSTANCE_QUERY_INSTANCE_3(jWmiInstance,jOutBufferSize,jOutBuffer,jBufferUsed);
		#endif
        break;

	case 112:
                #ifdef fun_WDF_WMI_INSTANCE_QUERY_INSTANCE_4
			SDV_IRQL_PUSH(PASSIVE_LEVEL);
			fun_WDF_WMI_INSTANCE_QUERY_INSTANCE_4(jWmiInstance,jOutBufferSize,jOutBuffer,jBufferUsed);
		#endif
        break;

	case 113:
                #ifdef fun_WDF_WMI_INSTANCE_QUERY_INSTANCE_5
			SDV_IRQL_PUSH(PASSIVE_LEVEL);
			fun_WDF_WMI_INSTANCE_QUERY_INSTANCE_5(jWmiInstance,jOutBufferSize,jOutBuffer,jBufferUsed);
		#endif
        break;

	case 114:
                #ifdef fun_WDF_WMI_INSTANCE_SET_INSTANCE_1
			SDV_IRQL_PUSH(PASSIVE_LEVEL);
			fun_WDF_WMI_INSTANCE_SET_INSTANCE_1(jWmiInstance,jInBufferSize,jInBuffer);
		#endif
        break;

	case 115:
               #ifdef fun_WDF_WMI_INSTANCE_SET_INSTANCE_2
			SDV_IRQL_PUSH(PASSIVE_LEVEL);
			fun_WDF_WMI_INSTANCE_SET_INSTANCE_2(jWmiInstance,jInBufferSize,jInBuffer);
		#endif
        break;

	case 116:
                #ifdef fun_WDF_WMI_INSTANCE_SET_INSTANCE_3
			SDV_IRQL_PUSH(PASSIVE_LEVEL);
			fun_WDF_WMI_INSTANCE_SET_INSTANCE_3(jWmiInstance,jInBufferSize,jInBuffer);
		#endif
        break;

	case 117:
                #ifdef fun_WDF_WMI_INSTANCE_SET_INSTANCE_4
			SDV_IRQL_PUSH(PASSIVE_LEVEL);
			fun_WDF_WMI_INSTANCE_SET_INSTANCE_4(jWmiInstance,jInBufferSize,jInBuffer);
		#endif
        break;

	case 118:
                #ifdef fun_WDF_WMI_INSTANCE_SET_INSTANCE_5
			SDV_IRQL_PUSH(PASSIVE_LEVEL);
			fun_WDF_WMI_INSTANCE_SET_INSTANCE_5(jWmiInstance,jInBufferSize,jInBuffer);
		#endif
        break;

	case 119:
                #ifdef fun_WDF_WMI_INSTANCE_SET_ITEM_1
			SDV_IRQL_PUSH(PASSIVE_LEVEL);
			fun_WDF_WMI_INSTANCE_SET_ITEM_1(jWmiInstance,jDataItemId,jInBufferSize,jInBuffer);
		#endif
        break;

	case 120:
                #ifdef fun_WDF_WMI_INSTANCE_SET_ITEM_2
			SDV_IRQL_PUSH(PASSIVE_LEVEL);
			fun_WDF_WMI_INSTANCE_SET_ITEM_2(jWmiInstance,jDataItemId,jInBufferSize,jInBuffer);
		#endif
        break;

	case 121:
                #ifdef fun_WDF_WMI_INSTANCE_SET_ITEM_3
			SDV_IRQL_PUSH(PASSIVE_LEVEL);
			fun_WDF_WMI_INSTANCE_SET_ITEM_3(jWmiInstance,jDataItemId,jInBufferSize,jInBuffer);
		#endif
        break;

	case 122:
                #ifdef fun_WDF_WMI_INSTANCE_SET_ITEM_4
			SDV_IRQL_PUSH(PASSIVE_LEVEL);
			fun_WDF_WMI_INSTANCE_SET_ITEM_4(jWmiInstance,jDataItemId,jInBufferSize,jInBuffer);
		#endif
        break;

	case 123:
        #ifdef fun_WDF_WMI_INSTANCE_SET_ITEM_5
			SDV_IRQL_PUSH(PASSIVE_LEVEL);
			fun_WDF_WMI_INSTANCE_SET_ITEM_5(jWmiInstance,jDataItemId,jInBufferSize,jInBuffer);
		#endif
        break;

	case 124:
                #ifdef fun_WDF_WMI_INSTANCE_EXECUTE_METHOD_1
			SDV_IRQL_PUSH(PASSIVE_LEVEL);
			fun_WDF_WMI_INSTANCE_EXECUTE_METHOD_1(jWmiInstance,jMethodId,jInBufferSize,jOutBufferSize,jBuffer,jBufferUsed);
		#endif
        break;

	case 125:
                #ifdef fun_WDF_WMI_INSTANCE_EXECUTE_METHOD_2
			SDV_IRQL_PUSH(PASSIVE_LEVEL);
			fun_WDF_WMI_INSTANCE_EXECUTE_METHOD_2(jWmiInstance,jMethodId,jInBufferSize,jOutBufferSize,jBuffer,jBufferUsed);
		#endif
        break;


	case 126:
                #ifdef fun_WDF_WMI_INSTANCE_EXECUTE_METHOD_3
			SDV_IRQL_PUSH(PASSIVE_LEVEL);
			fun_WDF_WMI_INSTANCE_EXECUTE_METHOD_3(jWmiInstance,jMethodId,jInBufferSize,jOutBufferSize,jBuffer,jBufferUsed);
		#endif
        break;


	case 127:
                #ifdef fun_WDF_WMI_INSTANCE_EXECUTE_METHOD_4
			SDV_IRQL_PUSH(PASSIVE_LEVEL);
			fun_WDF_WMI_INSTANCE_EXECUTE_METHOD_4(jWmiInstance,jMethodId,jInBufferSize,jOutBufferSize,jBuffer,jBufferUsed);
		#endif
        break;


	case 128:
                #ifdef fun_WDF_WMI_INSTANCE_EXECUTE_METHOD_5
			SDV_IRQL_PUSH(PASSIVE_LEVEL);
			fun_WDF_WMI_INSTANCE_EXECUTE_METHOD_5(jWmiInstance,jMethodId,jInBufferSize,jOutBufferSize,jBuffer,jBufferUsed);
		#endif
        break;

	/*case 129:
                #ifdef fun_WDF_WORKITEM_1
			SDV_IRQL_PUSH(PASSIVE_LEVEL);
			fun_WDF_WORKITEM_1(jWorkItem);
		#endif
        break;

	case 130:
                #ifdef fun_WDF_WORKITEM_2
			SDV_IRQL_PUSH(PASSIVE_LEVEL);
			fun_WDF_WORKITEM_2(jWorkItem);
		#endif
        break;

	case 131:
                #ifdef fun_WDF_WORKITEM_3
			SDV_IRQL_PUSH(PASSIVE_LEVEL);
			fun_WDF_WORKITEM_3(jWorkItem);
		#endif
        break;

	case 132:
                #ifdef fun_WDF_WORKITEM_4
			SDV_IRQL_PUSH(PASSIVE_LEVEL);
			fun_WDF_WORKITEM_4(jWorkItem);
		#endif
        break;

	case 133:
                #ifdef fun_WDF_WORKITEM_5
			SDV_IRQL_PUSH(PASSIVE_LEVEL);
			fun_WDF_WORKITEM_5(jWorkItem);
		#endif
        break;

	case 134:
                #ifdef fun_WDF_WORKITEM_6
			SDV_IRQL_PUSH(PASSIVE_LEVEL);
			fun_WDF_WORKITEM_6(jWorkItem);
		#endif
        break;*/


	case 135:
		#ifdef fun_WDFDEVICE_WDM_IRP_PREPROCESS_1
			fun_WDFDEVICE_WDM_IRP_PREPROCESS_1(hdevice,sdv_irp);
		#endif
        break;

	case 136:
		#ifdef fun_WDFDEVICE_WDM_IRP_PREPROCESS_2
			fun_WDFDEVICE_WDM_IRP_PREPROCESS_2(hdevice,sdv_irp);
		#endif
        break;

	case 137:
		#ifdef fun_WDFDEVICE_WDM_IRP_PREPROCESS_3
			fun_WDFDEVICE_WDM_IRP_PREPROCESS_3(hdevice,sdv_irp);
		#endif
        break;

	case 138:
		#ifdef fun_WDFDEVICE_WDM_IRP_PREPROCESS_4
			fun_WDFDEVICE_WDM_IRP_PREPROCESS_4(hdevice,sdv_irp);
		#endif
        break;

	case 139:
		#ifdef fun_WDFDEVICE_WDM_IRP_PREPROCESS_5
			fun_WDFDEVICE_WDM_IRP_PREPROCESS_5(hdevice,sdv_irp);
		#endif
        break;

	case 140:
		#ifdef fun_WDFDEVICE_WDM_IRP_PREPROCESS_6
			fun_WDFDEVICE_WDM_IRP_PREPROCESS_6(hdevice,sdv_irp);
		#endif
        break;
  case 141:
                #ifdef fun_WDF_IO_QUEUE_IO_DEFAULT
                       if(!default_no_attrib)
                       {
		           sdv_io_default_begin();
		           SDV_IRQL_PUSH(default_queue_stored_irql);
			   fun_WDF_IO_QUEUE_IO_DEFAULT(hdefqueue, hrequest);
			   sdv_io_default_end();
                       }
	        #endif
	    break;
	    
	case 142:
                #ifdef fun_WDF_IO_QUEUE_IO_READ
                       if(!read_no_attrib)
                       {
                           sdv_io_read_begin();
			   SDV_IRQL_PUSH(read_queue_stored_irql);
			   fun_WDF_IO_QUEUE_IO_READ(hdefqueue, hrequest, jlength);
			   sdv_io_read_end();
		       }
	        #endif
	    break;
	    
	case 143:
                #ifdef fun_WDF_IO_QUEUE_IO_WRITE
                    if(!write_no_attrib)
                    {
                        sdv_io_write_begin();
			SDV_IRQL_PUSH(write_queue_stored_irql);
			fun_WDF_IO_QUEUE_IO_WRITE(hdefqueue, hrequest, jlength);
			sdv_io_write_end();
		    }
                #endif
        break;
      
  case 144:
                #ifdef fun_WDF_IO_QUEUE_IO_DEVICE_CONTROL
                    if((!device_control_no_attrib)&&sdv_io_device_queue_configured)
                    {
                        sdv_device_control_begin();
		        SDV_IRQL_PUSH(device_control_queue_stored_irql);
			fun_WDF_IO_QUEUE_IO_DEVICE_CONTROL(hdefqueue, hrequest, joblength, jiblength, jIoControlCode);
			sdv_device_control_end();
		    }
	         #endif
	    break;
	    
	case 145:
                 #ifdef fun_WDF_IO_QUEUE_IO_INTERNAL_DEVICE_CONTROL
                    if(!internal_device_control_no_attrib)
                    {
                        sdv_internal_device_control_begin();
			SDV_IRQL_PUSH(internal_device_control_queue_stored_irql);
			fun_WDF_IO_QUEUE_IO_INTERNAL_DEVICE_CONTROL(hdefqueue, hrequest, joblength, jiblength, jIoControlCode);
			sdv_internal_device_control_end();
	            }
	         #endif
	    break;
	case 146: 
        #ifdef fun_WDF_DEVICE_FILTER_RESOURCE_REQUIREMENTS_2
			SDV_IRQL_PUSH(PASSIVE_LEVEL);
			fun_WDF_DEVICE_FILTER_RESOURCE_REQUIREMENTS_2(hdevice,jIoResourceRequirementsList);
		#endif
        break;
    case 147: 
        #ifdef fun_WDF_DEVICE_FILTER_RESOURCE_REQUIREMENTS_3
			SDV_IRQL_PUSH(PASSIVE_LEVEL);
			fun_WDF_DEVICE_FILTER_RESOURCE_REQUIREMENTS_3(hdevice,jIoResourceRequirementsList);
		#endif
        break;
    case 148: 
        #ifdef fun_WDF_DEVICE_FILTER_RESOURCE_REQUIREMENTS_4
			SDV_IRQL_PUSH(PASSIVE_LEVEL);
			fun_WDF_DEVICE_FILTER_RESOURCE_REQUIREMENTS_4(hdevice,jIoResourceRequirementsList);
		#endif
        break;
    case 149: 
        #ifdef fun_WDF_DEVICE_FILTER_RESOURCE_REQUIREMENTS_5
			SDV_IRQL_PUSH(PASSIVE_LEVEL);
			fun_WDF_DEVICE_FILTER_RESOURCE_REQUIREMENTS_5(hdevice,jIoResourceRequirementsList);
		#endif
        break;
    case 150: 
        #ifdef fun_WDF_DEVICE_FILTER_RESOURCE_REQUIREMENTS_6
			SDV_IRQL_PUSH(PASSIVE_LEVEL);
			fun_WDF_DEVICE_FILTER_RESOURCE_REQUIREMENTS_6(hdevice,jIoResourceRequirementsList);
		#endif
        break;

	
    case 151: 
		#ifdef fun_WDF_REQUEST_COMPLETION_ROUTINE_1
			Choice_Dispatch();
			fun_WDF_REQUEST_COMPLETION_ROUTINE_1(hrequest, jiotarget,jparams,jContext);
		#endif
        break;
    case 152: 
		#ifdef fun_WDF_REQUEST_COMPLETION_ROUTINE_2
			Choice_Dispatch();
			fun_WDF_REQUEST_COMPLETION_ROUTINE_2(hrequest, jiotarget,jparams,jContext);
		#endif
        break;
    case 153: 
		#ifdef fun_WDF_REQUEST_COMPLETION_ROUTINE_3
			Choice_Dispatch();
			fun_WDF_REQUEST_COMPLETION_ROUTINE_3(hrequest, jiotarget,jparams,jContext);
		#endif
        break;
    case 154: 
		#ifdef fun_WDF_REQUEST_COMPLETION_ROUTINE_4
			Choice_Dispatch();
			fun_WDF_REQUEST_COMPLETION_ROUTINE_4(hrequest, jiotarget,jparams,jContext);
		#endif
        break;
    case 155: 
		#ifdef fun_WDF_REQUEST_COMPLETION_ROUTINE_5
			Choice_Dispatch();
			fun_WDF_REQUEST_COMPLETION_ROUTINE_5(hrequest, jiotarget,jparams,jContext);
		#endif
        break;
    case 156: 
		#ifdef fun_WDF_REQUEST_COMPLETION_ROUTINE_6
			Choice_Dispatch();
			fun_WDF_REQUEST_COMPLETION_ROUTINE_6(hrequest, jiotarget,jparams,jContext);
		#endif
        break;
     case 157: 
		#ifdef fun_WDF_FILE_CONTEXT_DESTROY_CALLBACK
			Choice_Dispatch();
			fun_WDF_FILE_CONTEXT_DESTROY_CALLBACK(jfileobject);
		#endif
        break;
     case 158: 
		#ifdef fun_WDF_FILE_CONTEXT_DESTROY_CALLBACK
			Choice_Dispatch();
			fun_WDF_FILE_CONTEXT_CLEANUP_CALLBACK(jfileobject);
		#endif
      break;
    	case 159:
    #ifdef fun_WDF_USB_READERS_FAILED_1
            SDV_IRQL_PUSH(PASSIVE_LEVEL);
			      fun_WDF_USB_READERS_FAILED_1(hUsbPipe,
											             ntstatus_error,
											             usbstatus_error);
	  #endif
	    break;
	    case 160:
    #ifdef fun_WDF_USB_READERS_FAILED_2
            SDV_IRQL_PUSH(PASSIVE_LEVEL);
			      fun_WDF_USB_READERS_FAILED_2(hUsbPipe,
											             ntstatus_error,
											             usbstatus_error);
	  #endif
	    break;
	    case 161:
    #ifdef fun_WDF_USB_READERS_FAILED_3
            SDV_IRQL_PUSH(PASSIVE_LEVEL);
			      fun_WDF_USB_READERS_FAILED_3(hUsbPipe,
											             ntstatus_error,
											             usbstatus_error);
	  #endif
	    break;

	    case 162:
		#ifdef fun_WDF_USB_READER_COMPLETION_ROUTINE_1
            SDV_IRQL_PUSH(PASSIVE_LEVEL);
			      fun_WDF_USB_READER_COMPLETION_ROUTINE_1 (  hUsbPipe,
														 Buffer,  
														 jlength,   
														 jContext);
	  #endif
		  break;
	    case 163:
		#ifdef fun_WDF_USB_READER_COMPLETION_ROUTINE_2
            SDV_IRQL_PUSH(PASSIVE_LEVEL);
			      fun_WDF_USB_READER_COMPLETION_ROUTINE_2 (  hUsbPipe,
														 Buffer,  
														 jlength,   
														 jContext);
	  #endif
		  break;
	    case 164:
		#ifdef fun_WDF_USB_READER_COMPLETION_ROUTINE_3
            SDV_IRQL_PUSH(PASSIVE_LEVEL);
			      fun_WDF_USB_READER_COMPLETION_ROUTINE_3 (  hUsbPipe,
														 Buffer,  
														 jlength,   
														 jContext);
	  #endif
		  break;

	default:
		break;

	}
  }
}

#endif
/* wdf-harness-irql-semi-flat.c end */

/* wdf-harness-flat-io.c begin */
/*
	Copyright (c) Microsoft Corporation.  All rights reserved.
*/

/*****************************************************************************

    wdf-harness-flat-io.c provides a flat harness for exercising a WDF driver Evt IO callbacks.

    The flat harness is useful for checking issues that are of a local
    nature.

    The following variations of the flat harness are available:
        SDV_WDF_FLAT_SIMPLE_IO_HARNESS

    The harnesses exercises the driver as follows:
        SDV_WDF_FLAT_SIMPLE_IO_HARNESS =
            ALL Evt IO KMDF callbacks

     
*****************************************************************************/

#define SDV_WDF_IS_FLAT_IO_HARNESS() \
    ( \
		SDV_HARNESS==SDV_WDF_FLAT_SIMPLE_IO_HARNESS \
	)


#if SDV_WDF_IS_FLAT_IO_HARNESS()

void sdv_main ()
{
 
	LONG choice;
	
	choice = SdvMakeChoice();

    switch (choice) {
    
	case 0: 
		#ifdef fun_WDF_IO_QUEUE_IO_READ
		    fun_WDF_IO_QUEUE_IO_READ(hdefqueue,hrequest,hln);
		#endif
        break;
    case 1: 
		#ifdef fun_WDF_IO_QUEUE_IO_WRITE
			fun_WDF_IO_QUEUE_IO_WRITE(hdefqueue,hrequest,hln);
		#endif
        break;
	case 2: 
		#ifdef fun_WDF_IO_QUEUE_IO_DEVICE_CONTROL
			fun_WDF_IO_QUEUE_IO_DEVICE_CONTROL(hdefqueue,hrequest,hln,hln2,hln3);
		#endif
        break;
	case 3: 
		#ifdef fun_WDF_IO_QUEUE_IO_INTERNAL_DEVICE_CONTROL
			fun_WDF_IO_QUEUE_IO_INTERNAL_DEVICE_CONTROL(hdefqueue,hrequest,hln,hln2,hln3);
		#endif
        break;
	case 4: 
		#ifdef fun_WDF_IO_QUEUE_IO_DEFAULT
			fun_WDF_IO_QUEUE_IO_DEFAULT(hdefqueue,hrequest);
		#endif
        break;
	default:
		break;

	}
}

#endif
/* wdf-harness-flat-io.c end */

/* wdf-harness-flat-adddevice.c begin */
/*
	Copyright (c) Microsoft Corporation.  All rights reserved.
*/

/*****************************************************************************

    wdf-harness-flat-adddevice.c provides a flat harness for driver entry and 
	AddDevice callback in a WDF driver.
    
    The flat harness only calls Driver Entry and AddDeviceHarness. 

    The harnesses exercises the driver as follows:
        SDV_WDF_FLAT_SIMPLE_AddDevice_HARNESS =
            DriverEntry and AddDevice KMDF callback

     
*****************************************************************************/

#define SDV_WDF_IS_FLAT_AddDevice() \
    ( \
		SDV_HARNESS==SDV_WDF_FLAT_SIMPLE_AddDevice_HARNESS \
	)


#if SDV_WDF_IS_FLAT_AddDevice()

void sdv_main ()
{
 	NTSTATUS  ntstatus = STATUS_SUCCESS;
        ntstatus = fun_DriverEntry(hp_driver, hregistry_path);
	SdvAssume (NT_SUCCESS(ntstatus));
        #ifdef fun_WDF_DRIVER_DEVICE_ADD
	ntstatus = fun_WDF_DRIVER_DEVICE_ADD(hdriver, hdevice_init);
        SdvAssume (NT_SUCCESS(ntstatus));
        #endif
}

#endif
/* wdf-harness-flat-adddevice.c end */

/* wdf-harness-flat-powerup.c begin */
/*
	Copyright (c) Microsoft Corporation.  All rights reserved.
*/

/*****************************************************************************

    wdf-harness-flat-powerup.c provides a flat harness for exercising KMDF powerup callbacks.

    
    This only calls the driver once.  

    The following variations of the flat harness are available:
    SDV_WDF_IS_FLAT_POWERUP

    The harnesses exercises the driver as follows:
        SDV_WDF_IS_FLAT_POWERUP =
            ALL KMDF POWERUP callbacks

     
*****************************************************************************/

#define SDV_WDF_IS_FLAT_POWERUP() \
    ( \
		SDV_HARNESS==SDV_WDF_FLAT_POWERUP_HARNESS \
	)

#if SDV_WDF_IS_FLAT_POWERUP()



WDFCHILDLIST										jchildlist;
PWDF_CHILD_IDENTIFICATION_DESCRIPTION_HEADER		jPWDF_CHILD_IDENTIFICATION_DESCRIPTION_HEADER;
PWDF_CHILD_IDENTIFICATION_DESCRIPTION_HEADER		sPWDF_CHILD_IDENTIFICATION_DESCRIPTION_HEADER;
PWDF_CHILD_IDENTIFICATION_DESCRIPTION_HEADER		dPWDF_CHILD_IDENTIFICATION_DESCRIPTION_HEADER;
PWDF_CHILD_IDENTIFICATION_DESCRIPTION_HEADER		jPWDF_CHILD_IDENTIFICATION_DESCRIPTION_HEADER;
PCWDF_DEVICE_PNP_NOTIFICATION_DATA					j_pnp_not_data;
PCWDF_DEVICE_POWER_NOTIFICATION_DATA				j_power_not_data;
PCWDF_DEVICE_POWER_POLICY_NOTIFICATION_DATA			j_power_pol_not_data;
WDF_POWER_DEVICE_STATE								pWDF_POWER_DEVICE_STATE;
WDF_POWER_DEVICE_STATE								tWDF_POWER_DEVICE_STATE;
WDFCMRESLIST										rawResources;
WDFCMRESLIST										transResources;
WDF_SPECIAL_FILE_TYPE								notificationType;
BOOLEAN												jbool;
DEVICE_RELATION_TYPE								relationType;
WDFREQUEST											jRequest;
WDFDMAENABLER											jWdfEnabler;
WDFIORESREQLIST										jIoResourceRequirementsList;
WDFOBJECT											jAssociatedObject;
ULONG												jActionFlags;
size_t												jlength;
size_t												joblength;
size_t												jiblength;
ULONG												jIoControlCode;
WDFCONTEXT											jContext;
WDFOBJECT											jObject;
WDFCMRESLIST										jResources;
SYSTEM_POWER_STATE									jPowerState;
WDFDMATRANSACTION									jTransaction;
PVOID												jContext;
WDF_DMA_DIRECTION									jDirection;
PSCATTER_GATHER_LIST								jSglist;
WDFDPC												jDpc;
WDFTIMER											jTimer;
WDFWMIINSTANCE										jWmiInstance;
ULONG												jOutBufferSize;
PVOID												jOutBuffer;
PULONG												jBufferUsed;
ULONG												jInBufferSize;
PVOID												jInBuffer;
ULONG												jDataItemId;
ULONG												jMethodId;
PVOID												jBuffer;
WDFWORKITEM											jWorkItem;
LPGUID												jInterfaceType;
PINTERFACE											jExposedInterface;
PVOID												jExposedInterfaceSpecificData;
WDFWMIPROVIDER										jWmiProvider;
WDF_WMI_PROVIDER_CONTROL							jControl;
BOOLEAN												jEnable;




void sdv_main ()
{
    LONG choicep;
    choicep = SdvMakeChoice();
    switch (choicep) {
    	
	#ifdef fun_WDF_CHILD_LIST_SCAN_FOR_CHILDREN
	case 0: 
		fun_WDF_CHILD_LIST_SCAN_FOR_CHILDREN(jchildlist);
		break;
        #endif
	#ifdef fun_WDF_DEVICE_D0_ENTRY
	case 1: 
		fun_WDF_DEVICE_D0_ENTRY(hdevice,pWDF_POWER_DEVICE_STATE);
		break;
        #endif
	#ifdef fun_WDF_DEVICE_D0_ENTRY_POST_INTERRUPTS_ENABLED
	case 2: 
		fun_WDF_DEVICE_D0_ENTRY_POST_INTERRUPTS_ENABLED(hdevice,pWDF_POWER_DEVICE_STATE);
	    break;
	#endif
	#ifdef fun_WDF_DEVICE_SELF_MANAGED_IO_INIT
	case 3: 
		fun_WDF_DEVICE_SELF_MANAGED_IO_INIT(hdevice);
	    break;
        #endif
	#ifdef fun_WDF_DEVICE_SELF_MANAGED_IO_RESTART
	case 4: 
		fun_WDF_DEVICE_SELF_MANAGED_IO_RESTART(hdevice);
		break;
	#endif
	#ifdef fun_WDF_DEVICE_DISARM_WAKE_FROM_S0
	case 5: 
		fun_WDF_DEVICE_DISARM_WAKE_FROM_S0(hdevice);
		break;
        #endif
	#ifdef fun_WDF_DEVICE_DISARM_WAKE_FROM_SX
	case 6: 
		fun_WDF_DEVICE_DISARM_WAKE_FROM_SX(hdevice);
		break;
	#endif
	#ifdef fun_WDF_DEVICE_WAKE_FROM_SX_TRIGGERED
        case 7: 
		fun_WDF_DEVICE_WAKE_FROM_SX_TRIGGERED(hdevice);
		break;
	#endif
	#ifdef fun_WDF_DMA_ENABLER_FILL
        case 8: 
		fun_WDF_DMA_ENABLER_FILL(jWdfEnabler);
		break;
	#endif
	#ifdef fun_WDF_DMA_ENABLER_ENABLE
	case 9: 
		fun_WDF_DMA_ENABLER_ENABLE(jWdfEnabler);
		break;
	#endif
	#ifdef fun_WDF_DMA_ENABLER_SELFMANAGED_IO_START
	case 10: 
		fun_WDF_DMA_ENABLER_SELFMANAGED_IO_START(jWdfEnabler);
		break;
	#endif
	#ifdef fun_WDF_INTERRUPT_ENABLE
	case 11: 
		fun_WDF_INTERRUPT_ENABLE(hinterrupt, hdevice);
		break;
    
	#endif
	#ifdef fun_WDF_IO_QUEUE_IO_RESUME
	case 12: 
		fun_WDF_IO_QUEUE_IO_RESUME(hdefqueue, jRequest);
		break;
        #endif
	#ifdef fun_WDF_DEVICE_DISABLE_WAKE_AT_BUS
	case 13: 
		fun_WDF_DEVICE_DISABLE_WAKE_AT_BUS(hdevice);
		break;
        #endif
	default:
		break;
	
	}
}

#endif
/* wdf-harness-flat-powerup.c end */

/* wdf-pnp-harness.c begin */
/*
	Copyright (c) Microsoft Corporation.  All rights reserved.
*/

/*****************************************************************************

    wdf-pnp-harness.c provides a PNP harness for exercising a driver.

    

    The following variations of the KMDF PNP harness are available:
        SDV_HARNESS_PNP_DEFERRED_IO_REQUESTS
        SDV_HARNESS_PNP_IO_REQUESTS
        
*****************************************************************************/

#define SDV_IS_KMDF_PNP_HARNESS() \
    ( \
        SDV_HARNESS==SDV_HARNESS_PNP_DEFERRED_IO_REQUESTS || \
        SDV_HARNESS==SDV_HARNESS_PNP_IO_REQUESTS \
    )

#if SDV_IS_KMDF_PNP_HARNESS()

int h_i;

void sdv_main ()
{
  int sdv_choice = SdvMakeChoice(); 
  
  #if SDV_HARNESS==SDV_HARNESS_PNP_DEFERRED_IO_REQUESTS
	int sdv_choice1 = SdvMakeChoice();
  #endif 
  
  NTSTATUS  ntstatus = STATUS_SUCCESS;
  NTSTATUS status = STATUS_SUCCESS;

  
/*****************************************************************************
    For all the harness(es):
        SDV_HARNESS_PNP_DEFERRED_IO_REQUESTS
        SDV_HARNESS_PNP_IO_REQUESTS
    Exercise:
        DriverEntry;
		fun_WDF_DRIVER_DEVICE_ADD;
*****************************************************************************/
    ntstatus = fun_DriverEntry(hp_driver, hregistry_path);
    SdvAssume (NT_SUCCESS(ntstatus));


	for (h_i=1; h_i<=NUMBER_OF_DEVICES; h_i++)
	{   
      #ifdef fun_WDF_DRIVER_DEVICE_ADD
         status = fun_WDF_DRIVER_DEVICE_ADD(hdriver, hdevice_init);
         SdvAssume (NT_SUCCESS(status));
      #endif
	}

if (NT_SUCCESS(status)) {
  #ifdef fun_WDF_DEVICE_SELF_MANAGED_IO_INIT
		status = fun_WDF_DEVICE_SELF_MANAGED_IO_INIT(hdevice);
		SdvAssume (NT_SUCCESS(status));
  #endif

switch (sdv_choice) {
#ifdef fun_WDF_IO_QUEUE_IO_DEFAULT 
#define SWITCH1MACRO_DEFINED
	  case 0:
	     rwcalled = 1;
		 reqpresented = 1;
		 fun_WDF_IO_QUEUE_IO_DEFAULT(hdefqueue,hrequest);
	  break;
#endif
#ifdef fun_WDF_IO_QUEUE_IO_READ
#define SWITCH1MACRO_DEFINED
	  case 1:
	    rwcalled = 1;
	    reqpresented = 1;
	    fun_WDF_IO_QUEUE_IO_READ(hdefqueue,hrequest,hln);
	  break;
#endif
#ifdef fun_WDF_IO_QUEUE_IO_WRITE
#define SWITCH1MACRO_DEFINED
	  case 2: 
	    rwcalled = 1;
	    reqpresented = 1;
	    fun_WDF_IO_QUEUE_IO_WRITE(hdefqueue,hrequest,hln);
	  break;
#endif
#ifdef fun_WDF_IO_QUEUE_IO_DEVICE_CONTROL
#define SWITCH1MACRO_DEFINED
	  case 3: 
	    rwcalled = 1;
	    reqpresented = 1;
	    fun_WDF_IO_QUEUE_IO_DEVICE_CONTROL(hdefqueue,hrequest,hln,hln2,hln3);
	  break;
#endif
#ifdef fun_WDF_IO_QUEUE_IO_INTERNAL_DEVICE_CONTROL
#define SWITCH1MACRO_DEFINED
	  case 4:
	    rwcalled = 1;
	    reqpresented = 1;
	    fun_WDF_IO_QUEUE_IO_INTERNAL_DEVICE_CONTROL(hdefqueue,hrequest,hln,hln2,hln3);
	  break;
#endif
#if SDV_HARNESS==SDV_HARNESS_DRIVER_CREATE
	#ifdef fun_WDF_IO_QUEUE_IO_STOP
	#define SWITCH1MACRO_DEFINED
		  case 5: 
			SdvAssume(reqpresented);
			SdvAssume(!reqcompleted);
			fun_WDF_IO_QUEUE_IO_STOP(hdefqueue,hrequest,hact);
			#ifdef fun_WDF_IO_QUEUE_IO_RESUME
			  fun_WDF_IO_QUEUE_IO_RESUME(hdefqueue,hrequest);
			#endif
		  break;
	#endif
#endif

#if defined (SWITCH1MACRO_DEFINED)
	default: 
	  SdvAssume(0);             
	  break;
#endif
}



#if SDV_HARNESS==SDV_HARNESS_PNP_DEFERRED_IO_REQUESTS
#undef SWITCH1MACRO_DEFINED

switch (sdv_choice1) {
#ifdef fun_WDF_REQUEST_CANCEL_1
  case 0:
	  SdvAssume(reqpresented);
 	  SdvAssume(!reqcompleted);
	  fun_WDF_REQUEST_CANCEL_1(hrequest);
	  break;
#endif
#ifdef fun_WDF_REQUEST_CANCEL_2
  case 1:
	  SdvAssume(reqpresented);
 	  SdvAssume(!reqcompleted);
	  fun_WDF_REQUEST_CANCEL_2(hrequest);
	  break;
#endif
#ifdef fun_WDF_REQUEST_CANCEL_3
  case 2:
	  SdvAssume(reqpresented);
 	  SdvAssume(!reqcompleted);
	  fun_WDF_REQUEST_CANCEL_3(hrequest);
	  break;
#endif
#ifdef fun_WDF_REQUEST_CANCEL_4
  case 3:
	  SdvAssume(reqpresented);
 	  SdvAssume(!reqcompleted);
	  fun_WDF_REQUEST_CANCEL_4(hrequest);
	  break;
#endif
#ifdef fun_WDF_REQUEST_CANCEL_5
  case 4:
	  SdvAssume(reqpresented);
 	  SdvAssume(!reqcompleted);
	  fun_WDF_REQUEST_CANCEL_5(hrequest);
	  break;
#endif
#ifdef fun_WDF_REQUEST_CANCEL_6
  case 5:
	  SdvAssume(reqpresented);
 	  SdvAssume(!reqcompleted);
	  fun_WDF_REQUEST_CANCEL_6(hrequest);
	  break;
#endif


#ifdef fun_WDF_IO_QUEUE_IO_STOP
  case 6: 
	  SdvAssume(reqpresented);
	  SdvAssume(!reqcompleted);
	  fun_WDF_IO_QUEUE_IO_STOP(hdefqueue,hrequest,hact_suspend);
	  fun_WDF_IO_QUEUE_IO_STOP(hdefqueue,hrequest,hact_purge);
	  #ifdef fun_WDF_IO_QUEUE_IO_RESUME
	    fun_WDF_IO_QUEUE_IO_RESUME(hdefqueue,hrequest);
    #endif
      break;
#endif
#ifdef fun_WDF_TIMER_1
#define SWITCH2MACRO_DEFINED
  case 7:
      SdvAssume(reqpresented);
      SdvAssume(!reqcompleted);
      fun_WDF_TIMER_1(htimer);
	  break;
#endif
#ifdef fun_WDF_TIMER_2
#define SWITCH2MACRO_DEFINED
  case 8:
      SdvAssume(reqpresented);
      SdvAssume(!reqcompleted);
      fun_WDF_TIMER_2(htimer);
	  break;
#endif
#ifdef fun_WDF_TIMER_3
#define SWITCH2MACRO_DEFINED
  case 9:
      SdvAssume(reqpresented);
      SdvAssume(!reqcompleted);
      fun_WDF_TIMER_3(htimer);
	  break;
#endif
#ifdef fun_WDF_TIMER_4
#define SWITCH2MACRO_DEFINED
  case 10:
      SdvAssume(reqpresented);
      SdvAssume(!reqcompleted);
      fun_WDF_TIMER_4(htimer);
	  break;
#endif
#ifdef fun_WDF_TIMER_5
#define SWITCH2MACRO_DEFINED
  case 11:
      SdvAssume(reqpresented);
      SdvAssume(!reqcompleted);
      fun_WDF_TIMER_5(htimer);
	  break;
#endif
#ifdef fun_WDF_TIMER_6
#define SWITCH2MACRO_DEFINED
  case 12:
      SdvAssume(reqpresented);
      SdvAssume(!reqcompleted);
      fun_WDF_TIMER_6(htimer);
	  break;
#endif
#ifdef fun_WDF_INTERRUPT_ISR
#define SWITCH2MACRO_DEFINED
 case 13:
      fun_WDF_INTERRUPT_ISR(hinterrupt, hmessageID);
#ifdef fun_WDF_INTERRUPT_DPC
      SdvAssume(reqpresented);
      SdvAssume(!reqcompleted);
      fun_WDF_INTERRUPT_DPC(hinterrupt, hdevice);
#endif
#ifdef fun_WDF_INTERRUPT_ENABLE  
      SdvAssume(reqpresented);
      SdvAssume(!reqcompleted);
      status = fun_WDF_INTERRUPT_ENABLE(hinterrupt, hdevice);	  
#endif
#ifdef fun_WDF_INTERRUPT_DISABLE  
      SdvAssume(reqpresented);
      SdvAssume(!reqcompleted);
      status = fun_WDF_INTERRUPT_DISABLE(hinterrupt, hdevice);
#endif
	  break;
#endif
#ifdef fun_WDF_DPC_1
#define SWITCH2MACRO_DEFINED
 case 14:  
      SdvAssume(reqpresented);
      SdvAssume(!reqcompleted);
      fun_WDF_DPC_1(hdpc);
      break;	  
#endif
#ifdef fun_WDF_DPC_2
#define SWITCH2MACRO_DEFINED
 case 15:  
      SdvAssume(reqpresented);
      SdvAssume(!reqcompleted);
      fun_WDF_DPC_2(hdpc);
      break;  
#endif
#ifdef fun_WDF_DPC_3
#define SWITCH2MACRO_DEFINED
 case 16:  
      SdvAssume(reqpresented);
      SdvAssume(!reqcompleted);
      fun_WDF_DPC_3(hdpc);
      break;  
#endif
#ifdef fun_WDF_DPC_4
#define SWITCH2MACRO_DEFINED
 case 17:  
      SdvAssume(reqpresented);
      SdvAssume(!reqcompleted);
      fun_WDF_DPC_4(hdpc);
      break;  
#endif
#ifdef fun_WDF_DPC_5
#define SWITCH2MACRO_DEFINED
 case 18:  
      SdvAssume(reqpresented);
      SdvAssume(!reqcompleted);
      fun_WDF_DPC_5(hdpc);
      break;  
#endif
#ifdef fun_WDF_DPC_6
#define SWITCH2MACRO_DEFINED
 case 19:  
      SdvAssume(reqpresented);
      SdvAssume(!reqcompleted);
      fun_WDF_DPC_6(hdpc);
      break;  
#endif
#ifdef fun_WDF_DPC_7
#define SWITCH2MACRO_DEFINED
 case 20:  
      SdvAssume(reqpresented);
      SdvAssume(!reqcompleted);
      fun_WDF_DPC_7(hdpc);
      break;  
#endif
#ifdef fun_WDF_WORKITEM_1
#define SWITCH2MACRO_DEFINED
 case 21:
      SdvAssume(reqpresented);
      SdvAssume(!reqcompleted);
      fun_WDF_WORKITEM_1(hwork_item);
	  break;
#endif
#ifdef fun_WDF_WORKITEM_2
#define SWITCH2MACRO_DEFINED
 case 22:
      SdvAssume(reqpresented);
      SdvAssume(!reqcompleted);
      fun_WDF_WORKITEM_2(hwork_item);
	  break;
#endif
#ifdef fun_WDF_WORKITEM_3
#define SWITCH2MACRO_DEFINED
 case 23:
      SdvAssume(reqpresented);
      SdvAssume(!reqcompleted);
      fun_WDF_WORKITEM_3(hwork_item);
	  break;
#endif
#ifdef fun_WDF_WORKITEM_4
#define SWITCH2MACRO_DEFINED
 case 24:
      SdvAssume(reqpresented);
      SdvAssume(!reqcompleted);
      fun_WDF_WORKITEM_4(hwork_item);
	  break;
#endif
#ifdef fun_WDF_WORKITEM_5
#define SWITCH2MACRO_DEFINED
 case 25:
      SdvAssume(reqpresented);
      SdvAssume(!reqcompleted);
      fun_WDF_WORKITEM_5(hwork_item);
	  break;
#endif
#ifdef fun_WDF_WORKITEM_6
#define SWITCH2MACRO_DEFINED
 case 26:
      SdvAssume(reqpresented);
      SdvAssume(!reqcompleted);
      fun_WDF_WORKITEM_6(hwork_item);
	  break;
#endif




#if defined (SWITCH2MACRO_DEFINED)
default: 
	  SdvAssume(0);   
	  break;
#endif
}


#endif


#if SDV_HARNESS==SDV_HARNESS_PNP_DEFERRED_IO_REQUESTS
	#undef SWITCH2MACRO_DEFINED
#endif

#if SDV_HARNESS==SDV_HARNESS_PNP_IO_REQUESTS
#undef SWITCH1MACRO_DEFINED
#ifdef fun_WDF_REQUEST_CANCEL_1
      SdvAssume(reqpresented);
	  SdvAssume(!reqcompleted);
	  fun_WDF_REQUEST_CANCEL_1(hrequest);
#endif
#endif



#ifdef fun_WDFDEVICE_WDM_IRP_PREPROCESS_1
	status=fun_WDFDEVICE_WDM_IRP_PREPROCESS_1(hdevice,sdv_irp);
#endif

#ifdef fun_WDFDEVICE_WDM_IRP_PREPROCESS_2
	status=fun_WDFDEVICE_WDM_IRP_PREPROCESS_2(hdevice,sdv_irp);
#endif

#ifdef fun_WDFDEVICE_WDM_IRP_PREPROCESS_3
	status=fun_WDFDEVICE_WDM_IRP_PREPROCESS_3(hdevice,sdv_irp);
#endif

#ifdef fun_WDFDEVICE_WDM_IRP_PREPROCESS_4
	status=fun_WDFDEVICE_WDM_IRP_PREPROCESS_4(hdevice,sdv_irp);
#endif

#ifdef fun_WDFDEVICE_WDM_IRP_PREPROCESS_5
	status=fun_WDFDEVICE_WDM_IRP_PREPROCESS_5(hdevice,sdv_irp);
#endif

#ifdef fun_WDFDEVICE_WDM_IRP_PREPROCESS_6
	status=fun_WDFDEVICE_WDM_IRP_PREPROCESS_6(hdevice,sdv_irp);
#endif


#ifdef fun_WDF_DEVICE_WAKE_FROM_SX_TRIGGERED
      fun_WDF_DEVICE_WAKE_FROM_SX_TRIGGERED(hdevice);
#endif

#ifdef fun_WDF_DEVICE_D0_ENTRY
      fun_WDF_DEVICE_D0_ENTRY(hdevice,powerstate);
#endif

#ifdef fun_WDF_DEVICE_D0_ENTRY_POST_INTERRUPTS_ENABLED
      fun_WDF_DEVICE_D0_ENTRY_POST_INTERRUPTS_ENABLED(hdevice,powerstate);
#endif


#ifdef fun_WDF_CHILD_LIST_SCAN_FOR_CHILDREN
      fun_WDF_CHILD_LIST_SCAN_FOR_CHILDREN(dmaenabler);
#endif

#ifdef fun_WDF_DMA_ENABLER_FILL
      fun_WDF_DMA_ENABLER_FILL(dmaenabler);
#endif

#ifdef fun_WDF_DMA_ENABLER_ENABLE
      fun_WDF_DMA_ENABLER_ENABLE(dmaenabler);
#endif

#ifdef fun_WDF_DMA_ENABLER_SELFMANAGED_IO_START
      fun_WDF_DMA_ENABLER_SELFMANAGED_IO_START(dmaenabler);
#endif

#ifdef fun_WDF_DEVICE_DISARM_WAKE_FROM_S0
      fun_WDF_DEVICE_DISARM_WAKE_FROM_S0(hdevice);
#endif
#ifdef fun_WDF_DEVICE_DISARM_WAKE_FROM_SX
      fun_WDF_DEVICE_DISARM_WAKE_FROM_SX(hdevice);
#endif

#ifdef fun_WDF_DEVICE_SELF_MANAGED_IO_INIT
      fun_WDF_DEVICE_SELF_MANAGED_IO_INIT(hdevice);
#endif

#ifdef fun_WDF_DEVICE_SELF_MANAGED_IO_RESTART
      fun_WDF_DEVICE_SELF_MANAGED_IO_RESTART(hdevice);
#endif
#ifdef fun_WDF_DEVICE_DISABLE_WAKE_AT_BUS
      fun_WDF_DEVICE_DISABLE_WAKE_AT_BUS(hdevice);
#endif

#ifdef fun_WDF_IO_QUEUE_CONTEXT_CLEANUP_CALLBACK
	fun_WDF_IO_QUEUE_CONTEXT_CLEANUP_CALLBACK(hqctx);
#endif

#ifdef fun_WDF_IO_QUEUE_CONTEXT_DESTROY_CALLBACK
	fun_WDF_IO_QUEUE_CONTEXT_DESTROY_CALLBACK(hqctx);
#endif

#ifdef fun_WDF_FILE_CONTEXT_CLEANUP_CALLBACK
	fun_WDF_FILE_CONTEXT_CLEANUP_CALLBACK(hfctx);
#endif

#ifdef fun_WDF_FILE_CONTEXT_DESTROY_CALLBACK
	fun_WDF_FILE_CONTEXT_DESTROY_CALLBACK(hfctx);
#endif

#ifdef fun_WDF_FILE_CLOSE
        fun_WDF_FILE_CLOSE(hfileobj);
#endif
#ifdef fun_WDF_FILE_CLEANUP
	fun_WDF_FILE_CLEANUP(hdevice);
#endif

#ifdef fun_WDF_DEVICE_SELF_MANAGED_IO_SUSPEND
	ntstatus = fun_WDF_DEVICE_SELF_MANAGED_IO_SUSPEND(hdevice);
#endif

#ifdef fun_WDF_DEVICE_SELF_MANAGED_IO_FLUSH
	fun_WDF_DEVICE_SELF_MANAGED_IO_FLUSH(hdevice);
#endif

#ifdef fun_WDF_DEVICE_SELF_MANAGED_IO_CLEANUP
	fun_WDF_DEVICE_SELF_MANAGED_IO_CLEANUP(hdevice);
#endif

#ifdef fun_WDF_DEVICE_SURPRISE_REMOVAL
	fun_WDF_DEVICE_SURPRISE_REMOVAL(hdevice);
#endif


#ifdef fun_WDF_DEVICE_CONTEXT_CLEANUP  
	fun_WDF_DEVICE_CONTEXT_CLEANUP(hdevice);
	
	#if SDV_HARNESS==SDV_HARNESS_PNP_IO_REQUESTS
	if (hdevcollection_count != 0)
	{
		for (h_i=hdevcollection_count-1; h_i>0; h_i--)
		{
			fun_WDF_DEVICE_CONTEXT_CLEANUP(hdevice);
		}
	}
	#endif
#endif

#ifdef fun_WDF_DEVICE_CONTEXT_DESTROY  
	fun_WDF_DEVICE_CONTEXT_DESTROY(hdevice);
	
	#if SDV_HARNESS==SDV_HARNESS_PNP_IO_REQUESTS
	if (hdevcollection_count != 0)
	{
		for (h_i=hdevcollection_count-1; h_i>0; h_i--)
		{
			fun_WDF_DEVICE_CONTEXT_DESTROY(hdevice);
		}
	}
	#endif
#endif


#ifdef fun_WDF_DEVICE_SHUTDOWN_NOTIFICATION
	fun_WDF_DEVICE_SHUTDOWN_NOTIFICATION(hdevice);
#endif

#ifdef fun_WDF_DRIVER_UNLOAD
	fun_WDF_DRIVER_UNLOAD(hdriver);
#endif

	}
}

#endif

/* wdf-pnp-harness.c end */

/* wdf-usb-harness.c begin */
#if SDV_HARNESS==SDV_USB_HARNESS

WDFCMRESLIST										rawResources;
WDFCMRESLIST										transResources;

void sdv_main ()
{
 
  LONG choice;
	
  NTSTATUS  ntstatus = STATUS_SUCCESS;
  
  ntstatus = fun_DriverEntry(hp_driver, hregistry_path);
  SdvAssume (NT_SUCCESS(ntstatus));



    choice= SdvMakeChoice();

#ifdef SDV_WDF_IS_USB_HARNESS_RULE_USBDEVICECREATETARGET
	choice=2;
#endif

    switch (choice) {
    	
	case 0: 
			#ifdef fun_WDF_DRIVER_DEVICE_ADD
				ntstatus=fun_WDF_DRIVER_DEVICE_ADD(hdriver, hdevice_init);
			#else
				#pragma message("device add callback not registered")
			#endif
			break;
	case 1:
	
			#ifdef fun_WDF_DEVICE_PREPARE_HARDWARE
				fun_WDF_DEVICE_PREPARE_HARDWARE(hdevice,rawResources,transResources);

			#else
				#pragma message("device prepare hardware callback not registered")
			#endif
			break;
	case 2:
			#ifdef fun_WDF_DEVICE_PREPARE_HARDWARE
				ntstatus=fun_WDF_DEVICE_PREPARE_HARDWARE(hdevice,rawResources,transResources);
				SdvAssume (NT_SUCCESS(ntstatus));
			#else
				#pragma message("device prepare hardware callback not registered")
			#endif
			#ifdef fun_WDF_DEVICE_RELEASE_HARDWARE
				fun_WDF_DEVICE_RELEASE_HARDWARE(hdevice,transResources);

			#else
				#pragma message("device release hardware callback not registered")
			#endif
			#ifdef fun_WDF_DEVICE_PREPARE_HARDWARE
				fun_WDF_DEVICE_PREPARE_HARDWARE(hdevice,rawResources,transResources);

			#else
				#pragma message("device prepare hardware callback not registered")
			#endif
			break;

	case 3:
			
			#ifdef fun_WDF_DRIVER_DEVICE_ADD
				ntstatus=fun_WDF_DRIVER_DEVICE_ADD(hdriver, hdevice_init);
			#else
				#pragma message("device add callback not registered")
			#endif

			#ifdef fun_WDF_DEVICE_PREPARE_HARDWARE
				fun_WDF_DEVICE_PREPARE_HARDWARE(hdevice,rawResources,transResources);

			#else
				#pragma message("device prepare hardware callback not registered")
			#endif
			#ifdef fun_WDF_DEVICE_D0_ENTRY
				fun_WDF_DEVICE_D0_ENTRY(hdevice,WdfPowerDeviceInvalid);
			#else
				#pragma message("d0 entry callback not registered")
			#endif

			#ifdef fun_WDF_DEVICE_D0_EXIT
				fun_WDF_DEVICE_D0_EXIT(hdevice,WdfPowerDeviceInvalid);
			#else
				#pragma message("d0 exit callback not registered")
			#endif
			break;
	case 4:
			#ifdef fun_WDF_DEVICE_D0_EXIT
				fun_WDF_DEVICE_D0_EXIT(hdevice,WdfPowerDeviceInvalid);
			#else
				#pragma message("d0 exit callback not registered")
			#endif
			break;
	case 5:
			#ifdef fun_WDF_DEVICE_D0_ENTRY
				fun_WDF_DEVICE_D0_ENTRY(hdevice,WdfPowerDeviceInvalid);
			#else
				#pragma message("d0 entry callback not registered")
			#endif
			break;
	default:
			break;
	}
}

#endif
/* wdf-usb-harness.c end */

/* wdf_sdv_stubs.c begin */
/*
	Copyright (c) Microsoft Corporation.  All rights reserved.
*/

void __cdecl main() 
{
    sdv_main();
}

void
  DummyIoRead (
    IN WDFQUEUE  Queue,
    IN WDFREQUEST  Request,
    IN size_t  Length
    )
{
}

void  
  DummyIoWrite (
    IN WDFQUEUE  Queue,    
    IN WDFREQUEST  Request,    
    IN size_t  Length    
    )
{
}

void  
  DummyIoDefault (
    IN WDFQUEUE  Queue,
    IN WDFREQUEST  Request
    )  
{
}

void  
  DummyIoDeviceControl (
    IN WDFQUEUE  Queue,    
    IN WDFREQUEST  Request,    
    IN size_t  OutputBufferLength,    
    IN size_t  InputBufferLength,    
    IN ULONG  IoControlCode    )  
{
}

void  
  DummyIoInternalDeviceControl (
    IN WDFQUEUE  Queue,    
    IN WDFREQUEST  Request,    
    IN size_t  OutputBufferLength,    
    IN size_t  InputBufferLength,    
    IN ULONG  IoControlCode    
    )  
{
}

void  
  DummyIoStop (
    IN WDFQUEUE  Queue,
    IN WDFREQUEST  Request,    
    IN ULONG  ActionFlags    
    )
{
}

void  
  DummyIoResume (
    IN WDFQUEUE  Queue,
    IN WDFREQUEST  Request
    )
{
}

void  
  DummyIoCanceledOnQueue (
    IN WDFQUEUE  Queue,    
    IN WDFREQUEST  Request    
    )  
{
}

KIRQL Choice_Irql()
{
    int irql = SdvMakeChoice();
    switch(irql)
    {
      case 0:
         return PASSIVE_LEVEL;
         break;
      default:
         return DISPATCH_LEVEL;
         break;
     }

}

KIRQL Choose_Queue_CallBack_Irql_Level(PWDF_OBJECT_ATTRIBUTES QueueAttributes,BOOLEAN* No_QueueAttributes)
{

       if(QueueAttributes==NULL||QueueAttributes==WDF_NO_OBJECT_ATTRIBUTES)
       {
                *No_QueueAttributes=TRUE;
                if(device_queue_irql)
                {
                    return device_stored_queue_irql;
                }
                else
                {
                    return device_stored_irql;
                }
       }
       else if(QueueAttributes->SynchronizationScope==WdfSynchronizationScopeInheritFromParent)
       {
            if(QueueAttributes->ExecutionLevel==WdfExecutionLevelInheritFromParent)
            {
                *No_QueueAttributes=Device_No_QueueAttributes;
                if(device_queue_irql)
                {
                    return device_stored_queue_irql;
                }
                else
                {
                    return device_stored_irql;
                }
                
            }
            else if(QueueAttributes->ExecutionLevel==WdfExecutionLevelDispatch)
            {
                *No_QueueAttributes=Device_No_QueueAttributes;
                return DISPATCH_LEVEL;
            }
            else if(QueueAttributes->ExecutionLevel==WdfExecutionLevelPassive)
            {
                *No_QueueAttributes=FALSE;
                return PASSIVE_LEVEL;
            }
            else
            {
                *No_QueueAttributes=TRUE;
                return Choice_Irql();
            }
            
      }
      else if(QueueAttributes->SynchronizationScope==WdfSynchronizationScopeQueue)
      {
            if(QueueAttributes->ExecutionLevel==WdfExecutionLevelInheritFromParent)
            {
                *No_QueueAttributes=FALSE;
                if(device_queue_irql)
                {
                    return device_stored_queue_irql;
                }
                else
                {
                    return device_stored_irql;
                }
            }
            else if(QueueAttributes->ExecutionLevel==WdfExecutionLevelDispatch)
            {
                *No_QueueAttributes=FALSE;
                return DISPATCH_LEVEL;
            }
            else if(QueueAttributes->ExecutionLevel==WdfExecutionLevelPassive)
            {
                *No_QueueAttributes=FALSE;
                return PASSIVE_LEVEL;
            }
            else
            {
                *No_QueueAttributes=FALSE;
                return DISPATCH_LEVEL;
            }
      }
      else if(QueueAttributes->SynchronizationScope==WdfSynchronizationScopeNone)
      {
            *No_QueueAttributes=TRUE;
            if(QueueAttributes->ExecutionLevel==WdfExecutionLevelInheritFromParent)
            {
                *No_QueueAttributes=Device_No_QueueAttributes;
                if(device_queue_irql)
                {
                    return device_stored_queue_irql;
                }
                else
                {
                    return device_stored_irql;
                }
            }
            else if(QueueAttributes->ExecutionLevel==WdfExecutionLevelDispatch)
            {
               
               return DISPATCH_LEVEL;
            }
            else if(QueueAttributes->ExecutionLevel==WdfExecutionLevelPassive)
            {
                *No_QueueAttributes=FALSE;
                return PASSIVE_LEVEL;
            }
            else
            {
                *No_QueueAttributes=TRUE;
                return Choice_Irql();
            }
      }
      else 
      {
            *No_QueueAttributes=TRUE;
            return Choice_Irql();
      }
 
}


void Choose_Device_Wide_CallBacks_Irql_Level(PWDF_OBJECT_ATTRIBUTES DeviceAttributes,PWDFDEVICE_INIT* DeviceInit)
{
    if(*DeviceInit==hcontrol_deviceinit)
    {
       Device_No_QueueAttributes=FALSE;
       device_stored_irql=PASSIVE_LEVEL;
       
    }
    else if(DeviceAttributes!=NULL)
    {
        if(DeviceAttributes==WDF_NO_OBJECT_ATTRIBUTES)
        {
          Device_No_QueueAttributes=TRUE;
          device_stored_irql=Choice_Irql();
        }
        else if(DeviceAttributes->SynchronizationScope==WdfSynchronizationScopeDevice)
        {
            if(DeviceAttributes->ExecutionLevel==WdfExecutionLevelInheritFromParent)
            {
                Device_No_QueueAttributes=FALSE;
                device_stored_irql = DISPATCH_LEVEL;
            }
            else if(DeviceAttributes->ExecutionLevel==WdfExecutionLevelDispatch)
            {
                Device_No_QueueAttributes=FALSE;
                device_stored_irql=DISPATCH_LEVEL;
            }
            else if(DeviceAttributes->ExecutionLevel==WdfExecutionLevelPassive)
            {
                Device_No_QueueAttributes=FALSE;
                device_stored_irql=PASSIVE_LEVEL;
            }
            else
            {
                Device_No_QueueAttributes=TRUE;
                device_stored_irql=DISPATCH_LEVEL;
            }
        }
        else if (DeviceAttributes->SynchronizationScope==WdfSynchronizationScopeQueue)
        {
            device_queue_irql=1;
            
            if(DeviceAttributes->ExecutionLevel==WdfExecutionLevelInheritFromParent)
            {
                Device_No_QueueAttributes=FALSE;
                device_stored_queue_irql = DISPATCH_LEVEL;
            }
            else if(DeviceAttributes->ExecutionLevel==WdfExecutionLevelDispatch)
            {
                Device_No_QueueAttributes=FALSE;
                device_stored_queue_irql=DISPATCH_LEVEL;
            }
            else if(DeviceAttributes->ExecutionLevel==WdfExecutionLevelPassive)
            {
                Device_No_QueueAttributes=FALSE;
                device_stored_irql=PASSIVE_LEVEL;
            }
            else 
            {
                Device_No_QueueAttributes=TRUE;
                device_stored_queue_irql=DISPATCH_LEVEL;
            }
        }
        else if (DeviceAttributes->SynchronizationScope==WdfSynchronizationScopeNone)
        {
            device_queue_irql=1;
            Device_No_QueueAttributes=TRUE;
            if(DeviceAttributes->ExecutionLevel==WdfExecutionLevelInheritFromParent)
            {
                device_stored_queue_irql = Choice_Irql();
            }
            else if(DeviceAttributes->ExecutionLevel==WdfExecutionLevelDispatch)
            {
                device_stored_queue_irql=DISPATCH_LEVEL;
            }
            else if(DeviceAttributes->ExecutionLevel==WdfExecutionLevelPassive)
            {
                Device_No_QueueAttributes=FALSE;
                device_stored_irql=PASSIVE_LEVEL;
            }
            else 
            {
                device_stored_queue_irql=Choice_Irql();
            }
        }
        else if (DeviceAttributes->SynchronizationScope==WdfSynchronizationScopeInheritFromParent)
        {
            device_queue_irql=1;
            Device_No_QueueAttributes=TRUE;
            if(DeviceAttributes->ExecutionLevel==WdfExecutionLevelInheritFromParent)
            {
                device_stored_queue_irql = Choice_Irql();
            }
            else if(DeviceAttributes->ExecutionLevel==WdfExecutionLevelDispatch)
            {
                device_stored_queue_irql=DISPATCH_LEVEL;
            }
            else if(DeviceAttributes->ExecutionLevel==WdfExecutionLevelPassive)
            {
                Device_No_QueueAttributes=FALSE;
                device_stored_irql=PASSIVE_LEVEL;
            }
            else 
            {
                device_stored_queue_irql=Choice_Irql();
            }
        }
        else
        {
            Device_No_QueueAttributes=TRUE;
            device_stored_irql=Choice_Irql();
            
        }
    }
    else
    {
        
        Device_No_QueueAttributes=TRUE;
        device_stored_irql=Choice_Irql();

        
    }
}


/*void Choose_Driver_Device_Wide_CallBacks_Irql_Level(PWDF_OBJECT_ATTRIBUTES DriverAttributes)
{
  if(DriverAttributes==WDF_NO_OBJECT_ATTRIBUTES)
  {
    driver_stored_irql=Choice_Irql();
  }
  else if(DriverAttributes->SynchronizationScope==WdfSynchronizationScopeDevice)
  {
    if(DriverAttributes->ExecutionLevel==WdfExecutionLevelDispatch)
    {
      driver_stored_irql=DISPATCH_LEVEL;
    }
    else if(DriverAttributes->ExecutionLevel==WdfExecutionLevelPassive)
    {
      driver_stored_irql=PASSIVE_LEVEL;
    }
    else
    {
      driver_stored_irql=Choice_Irql();
    }
     
  }
  else
  {
    driver_stored_irql=Choice_Irql();
  }
}*/



VOID
sdv_WDF_DRIVER_CONFIG_INIT(
    OUT PWDF_DRIVER_CONFIG Config,
    IN PFN_WDF_DRIVER_DEVICE_ADD EvtDriverDeviceAdd
    )
{
}
WDFAPI
NTSTATUS
sdv_WdfDriverCreate(
    IN           PDRIVER_OBJECT          DriverObject,
    IN           PCUNICODE_STRING         RegistryPath,
    IN OPTIONAL  PWDF_OBJECT_ATTRIBUTES  DriverAttributes,
    IN           PWDF_DRIVER_CONFIG      DriverConfig,
    OUT OPTIONAL WDFDRIVER*              Driver
    )
{

if(Driver!=NULL)
{ 
	  int x = SdvMakeChoice();
#if SDV_HARNESS==SDV_WDF_IRQL_SEMI_FLAT_HARNESS
#endif
    switch (x) { 
        case 0: 
	        hdriver = (WDFDRIVER) &notused_4hdriver;
			    *Driver = hdriver;
			    return STATUS_SUCCESS;
			    break;
#ifdef SLAM_PRECISE_STATUS
        case 1: 
			     hdriver = (WDFDRIVER) &notused_4hdriver;
			    *Driver = hdriver;
			    return STATUS_INVALID_PARAMETER;
			    break;
        default:
			     hdriver = (WDFDRIVER) &notused_4hdriver;
			    *Driver = hdriver;
			    return STATUS_INSUFFICIENT_RESOURCES;
			    break;
#else
        default: 
			     hdriver = (WDFDRIVER) &notused_4hdriver;
			     *Driver = hdriver;
			     return STATUS_UNSUCCESSFUL;
			     break;
#endif
                                      
	}
}
else
{
    int x = SdvMakeChoice();
    switch (x) 
    { 
        case 0: 
           hdriver = (WDFDRIVER) &notused_4hdriver;
			     return STATUS_SUCCESS;
			     break;
        default:
           hdriver = (WDFDRIVER) &notused_4hdriver; 
			     return STATUS_UNSUCCESSFUL;
			     break;
    }
}
}
/* Suppress C4715: 'function' : not all control paths return a value.
       In the os model stubs must check a parameter for null before it is dereferenced.
       SLAM-2 considers all the traces where a null pointer is dereferenced as infeasible ones.
       This is acceptable as the OS Model will not be executed: */
#pragma warning(disable:4715)
NTSTATUS
sdv_WdfDriverOpenParametersRegistryKey(
    IN WDFDRIVER Driver,
    IN ACCESS_MASK DesiredAccess,
    IN OPTIONAL PWDF_OBJECT_ATTRIBUTES KeyAttributes,
    OUT WDFKEY* Key
    )
{

  if(Key!=NULL)
  {
    LONG choice = SdvMakeChoice();
    switch(choice) 
    {
        case 0:
              *Key=NULL;
              return (NTSTATUS) STATUS_NOT_SUPPORTED;
              break;
        default:
              *Key = (WDFKEY)malloc(1);
              return (NTSTATUS) STATUS_SUCCESS;
              break;
    }
  }
}
#pragma warning(default:4715)
PDRIVER_OBJECT
sdv_WdfDriverWdmGetDriverObject(
    IN WDFDRIVER Driver
	)
{
	return hp_driver;
}
/* Suppress C4715: 'function' : not all control paths return a value.
       In the os model stubs must check a parameter for null before it is dereferenced.
       SLAM-2 considers all the traces where a null pointer is dereferenced as infeasible ones.
       This is acceptable as the OS Model will not be executed: */
#pragma warning(disable:4715) 
WDFAPI
NTSTATUS
sdv_WdfDeviceCreate(
    IN OUT PWDFDEVICE_INIT* DeviceInit,
    IN OPTIONAL PWDF_OBJECT_ATTRIBUTES DeviceAttributes,
    OUT WDFDEVICE* Device
    )
{
if(DeviceInit!=NULL)
{
    int x = SdvMakeChoice();
    #if SDV_HARNESS==SDV_WDF_IRQL_SEMI_FLAT_HARNESS
      Choose_Device_Wide_CallBacks_Irql_Level(DeviceAttributes,DeviceInit);
    #endif
    switch (x) 
	{ 
      case 0: 
			if (*DeviceInit == hdevice_init)
			{
				#ifdef SDV_DEVICE_INIT_NULL
					hdevice_init=NULL;
					*DeviceInit=NULL;
				#endif
				SdvAssume(*DeviceInit == hdevice_init);
				hdevice = (WDFDEVICE) &notused_4hdevice;
                *Device = hdevice;
        
			
			}
			else
			{
				if (*DeviceInit == hcontrol_deviceinit) 
				{
					#ifdef SDV_DEVICE_INIT_NULL
						hcontrol_deviceinit=NULL;
						*DeviceInit=NULL;
					#endif
					SdvAssume(*DeviceInit == hcontrol_deviceinit);
					hcontrol_device = (WDFDEVICE) &notused_4hcontrol_device;
					*Device = hcontrol_device;
				}
				else if (*DeviceInit == hcontrol_deviceinit1) 
				{
					#ifdef SDV_DEVICE_INIT_NULL
						hcontrol_deviceinit1=NULL;
						*DeviceInit=NULL;
					#endif
					SdvAssume(*DeviceInit == hcontrol_deviceinit1);
					hcontrol_device1 = (WDFDEVICE) &notused_4hcontrol_device1;
					*Device = hcontrol_device1;
				}
				else if (*DeviceInit == hpdo_deviceinit) 
				{
						#ifdef SDV_DEVICE_INIT_NULL
							hpdo_deviceinit=NULL;
							*DeviceInit=NULL;
						#endif
						SdvAssume(*DeviceInit == hpdo_deviceinit);
						hpdo_device = (WDFDEVICE) &notused_4hpdo_device;
						*Device = hpdo_device;
				}
				else
				{
					if (*DeviceInit == hpdo_deviceinit1) 
					{
						#ifdef SDV_DEVICE_INIT_NULL
							hpdo_deviceinit1=NULL;
							*DeviceInit=NULL;
						#endif
						SdvAssume(*DeviceInit == hpdo_deviceinit1);
						hpdo_device1 = (WDFDEVICE) &notused_4hpdo_device1;
						*Device = hpdo_device1;
					}
					
				}
			}
			return STATUS_SUCCESS;
			break;
			
#ifdef SLAM_PRECISE_STATUS
        case 1: 
        if (*DeviceInit == hdevice_init)
				{
			    SdvAssume(*DeviceInit == hdevice_init);
				hdevice = (WDFDEVICE) &notused_4hdevice;
                *Device = hdevice;
				}
				else
				{
					if (*DeviceInit == hcontrol_deviceinit) 
					{   
						SdvAssume(*DeviceInit == hcontrol_deviceinit);
						hcontrol_device = (WDFDEVICE) &notused_4hcontrol_device;
						*Device = hcontrol_device;
					}
					else if (*DeviceInit == hcontrol_deviceinit1) 
					{   
						SdvAssume(*DeviceInit == hcontrol_deviceinit1);
						hcontrol_device1 = (WDFDEVICE) &notused_4hcontrol_device1;
						*Device = hcontrol_device1;
					}
					else if (*DeviceInit == hpdo_deviceinit) 
					{
						SdvAssume(*DeviceInit == hpdo_deviceinit);
						hpdo_device = (WDFDEVICE) &notused_4hpdo_device;
						*Device = hpdo_device;
						
					}
					else
					{
						if (*DeviceInit == hpdo_deviceinit1) {
						SdvAssume(*DeviceInit == hpdo_deviceinit1);
						hpdo_device1 = (WDFDEVICE) &notused_4hpdo_device1;
						*Device = hpdo_device1;
						
						}
					}
				}
				return STATUS_INVALID_PARAMETER;
				break;
        case 2: if (*DeviceInit == hdevice_init){
			    hdevice = (WDFDEVICE) &notused_4hdevice;
                *Device = hdevice;
				}
				else
				{
					if (*DeviceInit == hcontrol_deviceinit)
					{
						hcontrol_device = (WDFDEVICE) &notused_4hcontrol_device;
						*Device = hcontrol_device;
					}
					else if (*DeviceInit == hcontrol_deviceinit1)
					{
						hcontrol_device1 = (WDFDEVICE) &notused_4hcontrol_device1;
						*Device = hcontrol_device1;
					}
					else if (*DeviceInit == hpdo_deviceinit) 
					{
							hpdo_device = (WDFDEVICE) &notused_4hpdo_device;
							*Device = hpdo_device;
					}
					else
					{
						if (*DeviceInit == hpdo_deviceinit1) {
							hpdo_device1 = (WDFDEVICE) &notused_4hpdo_device1;
							*Device = hpdo_device1;
						}
						
					}
				}
			    return STATUS_INVALID_DEVICE_STATE;
			    break;
		case 3: 
		    if (*DeviceInit == hdevice_init)
				{
					hdevice = (WDFDEVICE) &notused_4hdevice;
					*Device = hdevice;
				}
				else
				{
					if (*DeviceInit == hcontrol_deviceinit) 
					{
						hcontrol_device = (WDFDEVICE) &notused_4hcontrol_device;
						*Device = hcontrol_device;
					}
					else if (*DeviceInit == hcontrol_deviceinit1)
					{
						hcontrol_device1 = (WDFDEVICE) &notused_4hcontrol_device1;
						*Device = hcontrol_device1;
					}
					else if (*DeviceInit == hpdo_deviceinit) 
					{
							hpdo_device = (WDFDEVICE) &notused_4hpdo_device;
							*Device = hpdo_device;
					}
					else
					{
						if (*DeviceInit == hpdo_deviceinit1) 
						{
							hpdo_device1 = (WDFDEVICE) &notused_4hpdo_device1;
							*Device = hpdo_device1;
						
						}
						
					}
				}
			    return STATUS_INVALID_SECURITY_DESCR;
			    break;
        default: 
        if (*DeviceInit == hdevice_init)
				 {
					hdevice = (WDFDEVICE) &notused_4hdevice;
					*Device = hdevice;
				}
				else
				{
					if (*DeviceInit == hcontrol_deviceinit) 
					{
						hcontrol_device = (WDFDEVICE) &notused_4hcontrol_device;
						*Device = hcontrol_device;
					}
					else if (*DeviceInit == hcontrol_deviceinit1)
					{
						hcontrol_device1 = (WDFDEVICE) &notused_4hcontrol_device1;
						*Device = hcontrol_device1;
					}
					else if (*DeviceInit == hpdo_deviceinit) 
					{
							hpdo_device = (WDFDEVICE) &notused_4hpdo_device;
							*Device = hpdo_device;
					}
					else
					{
						if (*DeviceInit == hpdo_deviceinit1) 
						{
							hpdo_device1 = (WDFDEVICE) &notused_4hpdo_device1;
							*Device = hpdo_device1;
						
						}
					}
				}
			    return STATUS_INSUFFICIENT_RESOURCES;
			    break;
#else
        default: 
        if (*DeviceInit == hdevice_init)
				{
				SdvAssume(*DeviceInit == hdevice_init);
				hdevice = (WDFDEVICE) &notused_4hdevice;
                *Device = hdevice;
				}
				else
				{
					if (*DeviceInit == hcontrol_deviceinit) 
					{
						SdvAssume(*DeviceInit == hcontrol_deviceinit);
						hcontrol_device = (WDFDEVICE) &notused_4hcontrol_device;
						*Device = hcontrol_device;
					}
					else if (*DeviceInit == hcontrol_deviceinit1) 
					{
						SdvAssume(*DeviceInit == hcontrol_deviceinit1);
						hcontrol_device1 = (WDFDEVICE) &notused_4hcontrol_device1;
						*Device = hcontrol_device1;
					}
					else if (*DeviceInit == hpdo_deviceinit) 
					{
						SdvAssume(*DeviceInit == hpdo_deviceinit);
						hpdo_device = (WDFDEVICE) &notused_4hpdo_device;
						*Device = hpdo_device;
					}
					else
					{
						if (*DeviceInit == hpdo_deviceinit1) 
						{
							SdvAssume(*DeviceInit == hpdo_deviceinit1);
							hpdo_device1 = (WDFDEVICE) &notused_4hpdo_device1;
							*Device = hpdo_device1;
						}
					}
				}
			    return STATUS_UNSUCCESSFUL;
			    break;
#endif
    }
}
else
{
return sdv_SetReturnStatus();
}
}
#pragma warning(default:4715)
WDFAPI NTSTATUS
sdv_WdfDeviceCreateSymbolicLink(
    IN WDFDEVICE  Device,
    IN PCUNICODE_STRING  SymbolicLinkName
    )
{
	return sdv_SetReturnStatus();
}
WDFAPI
NTSTATUS
sdv_WdfDeviceCreateDeviceInterface(
    IN WDFDEVICE Device,
    IN CONST GUID* InterfaceClassGUID,
    IN OPTIONAL PCUNICODE_STRING ReferenceString
    )
{ 
    return sdv_SetReturnStatus(); 
}
WDFAPI WDFDRIVER
sdv_WdfDeviceGetDriver(
   IN WDFDEVICE  Device
   )
{
	return hdriver;
}
WDFQUEUE
sdv_WdfDeviceGetDefaultQueue(
    IN WDFDEVICE Device
    )
{
	return hdefqueue;
}
/* Suppress C4715: 'function' : not all control paths return a value.
       In the os model stubs must check a parameter for null before it is dereferenced.
       SLAM-2 considers all the traces where a null pointer is dereferenced as infeasible ones.
       This is acceptable as the OS Model will not be executed: */
#pragma warning(disable:4715)
NTSTATUS
sdv_WdfDeviceOpenRegistryKey(
    IN WDFDEVICE Device,
    IN ULONG DeviceInstanceKeyType,
    IN ACCESS_MASK DesiredAccess,
    IN OPTIONAL PWDF_OBJECT_ATTRIBUTES KeyAttributes,
    OUT WDFKEY* Key
    )
{
if(Key!=NULL)
{
    LONG choice = SdvMakeChoice();
    switch(choice) 
    {
        case 0:
              *Key=NULL;
              return (NTSTATUS) STATUS_NOT_SUPPORTED;
              break;
        default:
              *Key = (WDFKEY)malloc(1);
              return (NTSTATUS) STATUS_SUCCESS;
              break;
    }
}
}
#pragma warning(default:4715)
PVOID
sdv_WdfObjectGet_DEVICE_CONTEXT(
   IN WDFDEVICE Device
   )
{  
	return hdctx;
}
PVOID
sdv_WdfObjectGet_CONTROL_DEVICE_CONTEXT(
   IN WDFDEVICE ControlDevice
   )
{  
	return hdctx;
}
PVOID
sdv_WdfObjectGet_QUEUE_CONTEXT(
   IN WDFQUEUE Queue
   )
{ 
	return hqctx;
}
PVOID
sdv_WdfObjectGet_TIMER_CONTEXT(
   IN WDFTIMER Timer
   )
{  
	return htctx;
}
PVOID
sdv_WdfObjectGet_DRIVER_CONTEXT(
    IN WDFDRIVER Driver
	)
{ 
	return hdrctx;
}
VOID
sdv_WDF_IO_QUEUE_CONFIG_INIT(
    OUT PWDF_IO_QUEUE_CONFIG      Config,
    IN WDF_IO_QUEUE_DISPATCH_TYPE DispatchType
    )
{
	Config->EvtIoRead=DummyIoRead;
	Config->EvtIoWrite=DummyIoWrite;
	Config->EvtIoDeviceControl=DummyIoDeviceControl;
	Config->EvtIoInternalDeviceControl=DummyIoInternalDeviceControl;
	Config->EvtIoStop=DummyIoStop;
	Config->EvtIoResume=DummyIoResume;
	Config->EvtIoCanceledOnQueue=DummyIoCanceledOnQueue;
	Config->EvtIoDefault=DummyIoDefault;
}
VOID
sdv_WDF_IO_QUEUE_CONFIG_INIT_DEFAULT_QUEUE(
    OUT PWDF_IO_QUEUE_CONFIG      Config,
    IN WDF_IO_QUEUE_DISPATCH_TYPE DispatchType
    )
{
#ifdef SDV_WDF_IRQL_SEMI_FLAT_HARNESS_MODIFIER_NO_MALLOC_sdv_WDF_IO_QUEUE_CONFIG_INIT_DEFAULT_QUEUE
	Config->EvtIoRead=DummyIoRead;
	Config->EvtIoWrite=DummyIoWrite;
	Config->EvtIoDeviceControl=DummyIoDeviceControl;
	Config->EvtIoInternalDeviceControl=DummyIoInternalDeviceControl;
	Config->EvtIoStop=DummyIoStop;
	Config->EvtIoResume=DummyIoResume;
	Config->EvtIoCanceledOnQueue=DummyIoCanceledOnQueue;
	Config->EvtIoDefault=DummyIoDefault;
#endif
}

void sdv_Set_Queue_Irql_Level(IN PWDF_IO_QUEUE_CONFIG Config,IN OPTIONAL PWDF_OBJECT_ATTRIBUTES QueueAttributes)
{
KIRQL tempIrql;
BOOLEAN NO_QueueAttributes=FALSE;
tempIrql=Choose_Queue_CallBack_Irql_Level(QueueAttributes,&NO_QueueAttributes);
#ifdef SDV_AGGRESSIVE_IRQL
NO_QueueAttributes=FALSE;
#endif
#ifdef fun_WDF_IO_QUEUE_IO_DEFAULT
  if(Config->EvtIoDefault==(PFN_WDF_IO_QUEUE_IO_DEFAULT)fun_WDF_IO_QUEUE_IO_DEFAULT)
  {
      
      default_no_attrib=NO_QueueAttributes;
      default_queue_stored_irql=tempIrql;
  } 
#endif
#ifdef fun_WDF_IO_QUEUE_IO_READ
  if(Config->EvtIoRead==(PFN_WDF_IO_QUEUE_IO_READ)fun_WDF_IO_QUEUE_IO_READ)
  {
      read_no_attrib=NO_QueueAttributes;
      read_queue_stored_irql=tempIrql;
  }
#endif
#ifdef fun_WDF_IO_QUEUE_IO_WRITE
  if(Config->EvtIoWrite==(PFN_WDF_IO_QUEUE_IO_WRITE)fun_WDF_IO_QUEUE_IO_WRITE)
  {
      write_no_attrib=NO_QueueAttributes;
      write_queue_stored_irql=tempIrql;
  }
#endif
#ifdef fun_WDF_IO_QUEUE_IO_DEVICE_CONTROL
  if(Config->EvtIoDeviceControl==(PFN_WDF_IO_QUEUE_IO_DEVICE_CONTROL)fun_WDF_IO_QUEUE_IO_DEVICE_CONTROL)
  {
      device_control_no_attrib=NO_QueueAttributes;
      device_control_queue_stored_irql=tempIrql;
      sdv_io_device_queue_configured=TRUE;
  }
#endif
#ifdef fun_WDF_IO_QUEUE_IO_INTERNAL_DEVICE_CONTROL
  if(Config->EvtIoInternalDeviceControl==(PFN_WDF_IO_QUEUE_IO_INTERNAL_DEVICE_CONTROL)fun_WDF_IO_QUEUE_IO_INTERNAL_DEVICE_CONTROL)
  {
      internal_device_control_no_attrib=NO_QueueAttributes;
      internal_device_control_queue_stored_irql=tempIrql;
  }
#endif
#ifdef fun_WDF_IO_QUEUE_IO_STOP
  if(Config->EvtIoStop==(PFN_WDF_IO_QUEUE_IO_STOP)fun_WDF_IO_QUEUE_IO_STOP)
  {
      stop_queue_no_attrib=NO_QueueAttributes;
      stop_queue_stored_irql=tempIrql;
  }
#endif
#ifdef fun_WDF_IO_QUEUE_IO_RESUME
  if(Config->EvtIoResume==(PFN_WDF_IO_QUEUE_IO_RESUME)fun_WDF_IO_QUEUE_IO_RESUME)
  {
      resume_queue_no_attrib=NO_QueueAttributes;
      resume_queue_stored_irql=tempIrql;
  }
#endif
#ifdef fun_WDF_IO_QUEUE_IO_CANCELED_ON_QUEUE
  if(Config->EvtIoCanceledOnQueue==(PFN_WDF_IO_QUEUE_IO_CANCELED_ON_QUEUE)fun_WDF_IO_QUEUE_IO_CANCELED_ON_QUEUE)
  {
      io_cancel_on_queue_no_attrib=NO_QueueAttributes;
      io_cancel_on_queue_stored_irql=tempIrql;
  }
#endif

}

void sdv_io_read_begin()
{
    sdv_inside_io_read_callback = 1;
}
void sdv_io_read_end()
{
    sdv_inside_io_read_callback = 0;
}

void sdv_io_write_begin()
{
    sdv_inside_io_write_callback = 1;
}
void sdv_io_write_end()
{
    sdv_inside_io_write_callback = 0;
}

void sdv_io_default_begin()
{
    sdv_inside_io_default_callback = 1;
}
void sdv_io_default_end()
{
    sdv_inside_io_default_callback = 0;
}

void sdv_device_control_begin()
{
    sdv_inside_io_device_control_callback = 1;
}
void sdv_device_control_end()
{
    sdv_inside_io_device_control_callback = 0;
}

void sdv_internal_device_control_begin()
{
    sdv_inside_io_internal_device_control_callback = 1;
}
void sdv_internal_device_control_end()
{
    sdv_inside_io_internal_device_control_callback = 0;
}

BOOLEAN sdv_Set_Read_Request_Cancel_Irql_Level(IN PFN_WDF_REQUEST_CANCEL EvtRequestCancel)
{
if(sdv_inside_io_read_callback)
{
#ifdef fun_WDF_REQUEST_CANCEL_1
  if(EvtRequestCancel==fun_WDF_REQUEST_CANCEL_1)
  {
	  SDV_IRQL_PUSH(read_queue_stored_irql);
    fun_WDF_REQUEST_CANCEL_1(hrequest);
    return TRUE;
  }
#endif
#ifdef fun_WDF_REQUEST_CANCEL_2
  if(EvtRequestCancel==fun_WDF_REQUEST_CANCEL_2)
  {
	  SDV_IRQL_PUSH(read_queue_stored_irql);
    fun_WDF_REQUEST_CANCEL_2(hrequest);
    return TRUE;
  }
#endif
#ifdef fun_WDF_REQUEST_CANCEL_3
  if(EvtRequestCancel==fun_WDF_REQUEST_CANCEL_3)
  {
	  SDV_IRQL_PUSH(read_queue_stored_irql);
    fun_WDF_REQUEST_CANCEL_3(hrequest);
    return TRUE;
  }
#endif
#ifdef fun_WDF_REQUEST_CANCEL_4
  if(EvtRequestCancel==fun_WDF_REQUEST_CANCEL_4)
  {
	  SDV_IRQL_PUSH(read_queue_stored_irql);
    fun_WDF_REQUEST_CANCEL_4(hrequest);
    return TRUE;
  }
#endif
#ifdef fun_WDF_REQUEST_CANCEL_5
  if(EvtRequestCancel==fun_WDF_REQUEST_CANCEL_5)
  {
	  SDV_IRQL_PUSH(read_queue_stored_irql);
    fun_WDF_REQUEST_CANCEL_5(hrequest);
    return TRUE;
  }
#endif
#ifdef fun_WDF_REQUEST_CANCEL_6
  if(EvtRequestCancel==fun_WDF_REQUEST_CANCEL_6)
  {
	  SDV_IRQL_PUSH(read_queue_stored_irql);
    fun_WDF_REQUEST_CANCEL_6(hrequest);
    return TRUE;
  }
#endif
  return FALSE;
}
return FALSE;
}

BOOLEAN sdv_Set_Write_Request_Cancel_Irql_Level(IN PFN_WDF_REQUEST_CANCEL EvtRequestCancel)
{
if(sdv_inside_io_write_callback)
{
#ifdef fun_WDF_REQUEST_CANCEL_1
  if(EvtRequestCancel==fun_WDF_REQUEST_CANCEL_1)
  {      
     SDV_IRQL_PUSH(write_queue_stored_irql);
     fun_WDF_REQUEST_CANCEL_1(hrequest);
     return TRUE;
  }
#endif
#ifdef fun_WDF_REQUEST_CANCEL_2
  if(EvtRequestCancel==fun_WDF_REQUEST_CANCEL_2)
  {
     SDV_IRQL_PUSH(write_queue_stored_irql);
     fun_WDF_REQUEST_CANCEL_2(hrequest);
     return TRUE;
  }
#endif
#ifdef fun_WDF_REQUEST_CANCEL_3
  if(EvtRequestCancel==fun_WDF_REQUEST_CANCEL_3)
  {
     SDV_IRQL_PUSH(write_queue_stored_irql);
     fun_WDF_REQUEST_CANCEL_3(hrequest);
     return TRUE;
  }
#endif
#ifdef fun_WDF_REQUEST_CANCEL_4
  if(EvtRequestCancel==fun_WDF_REQUEST_CANCEL_4)
  {
     SDV_IRQL_PUSH(write_queue_stored_irql);
     fun_WDF_REQUEST_CANCEL_4(hrequest);
     return TRUE;
  }
#endif
#ifdef fun_WDF_REQUEST_CANCEL_5
  if(EvtRequestCancel==fun_WDF_REQUEST_CANCEL_5)
  {
     SDV_IRQL_PUSH(write_queue_stored_irql);
     fun_WDF_REQUEST_CANCEL_5(hrequest);
     return TRUE;
  }
#endif
#ifdef fun_WDF_REQUEST_CANCEL_6
  if(EvtRequestCancel==fun_WDF_REQUEST_CANCEL_6)
  {
     SDV_IRQL_PUSH(write_queue_stored_irql);
     fun_WDF_REQUEST_CANCEL_6(hrequest);
     return TRUE;
  }
#endif
return FALSE;
}
return FALSE;
}

BOOLEAN sdv_Set_Device_Control_Request_Cancel_Irql_Level(IN PFN_WDF_REQUEST_CANCEL EvtRequestCancel)
{
if(sdv_inside_io_device_control_callback)
{
#ifdef fun_WDF_REQUEST_CANCEL_1
  if(EvtRequestCancel==fun_WDF_REQUEST_CANCEL_1)
  {      
     SDV_IRQL_PUSH(device_control_queue_stored_irql);
     fun_WDF_REQUEST_CANCEL_1(hrequest);
     return TRUE;
  }
#endif
#ifdef fun_WDF_REQUEST_CANCEL_2
  if(EvtRequestCancel==fun_WDF_REQUEST_CANCEL_2)
  {
     SDV_IRQL_PUSH(device_control_queue_stored_irql);
     fun_WDF_REQUEST_CANCEL_2(hrequest);
     return TRUE;
  }
#endif
#ifdef fun_WDF_REQUEST_CANCEL_3
  if(EvtRequestCancel==fun_WDF_REQUEST_CANCEL_3)
  {
     SDV_IRQL_PUSH(device_control_queue_stored_irql);
     fun_WDF_REQUEST_CANCEL_3(hrequest);
     return TRUE;
  }
#endif
#ifdef fun_WDF_REQUEST_CANCEL_4
  if(EvtRequestCancel==fun_WDF_REQUEST_CANCEL_4)
  {
     SDV_IRQL_PUSH(device_control_queue_stored_irql);
     fun_WDF_REQUEST_CANCEL_4(hrequest);
     return TRUE;
  }
#endif
#ifdef fun_WDF_REQUEST_CANCEL_5
  if(EvtRequestCancel==fun_WDF_REQUEST_CANCEL_5)
  {
     SDV_IRQL_PUSH(device_control_queue_stored_irql);
     fun_WDF_REQUEST_CANCEL_5(hrequest);
     return TRUE;
  }
#endif
#ifdef fun_WDF_REQUEST_CANCEL_6
  if(EvtRequestCancel==fun_WDF_REQUEST_CANCEL_6)
  {
     SDV_IRQL_PUSH(device_control_queue_stored_irql);
     fun_WDF_REQUEST_CANCEL_6(hrequest);
     return TRUE;
  }
#endif
return FALSE;
}
return FALSE;
}

BOOLEAN sdv_Set_Internal_Device_Control_Request_Cancel_Irql_Level(IN PFN_WDF_REQUEST_CANCEL EvtRequestCancel)
{
if(sdv_inside_io_internal_device_control_callback)
{
#ifdef fun_WDF_REQUEST_CANCEL_1
  if(EvtRequestCancel==fun_WDF_REQUEST_CANCEL_1)
  {      
     SDV_IRQL_PUSH(internal_device_control_queue_stored_irql);
     fun_WDF_REQUEST_CANCEL_1(hrequest);
     return TRUE;
  }
#endif
#ifdef fun_WDF_REQUEST_CANCEL_2
  if(EvtRequestCancel==fun_WDF_REQUEST_CANCEL_2)
  {
     SDV_IRQL_PUSH(internal_device_control_queue_stored_irql);
     fun_WDF_REQUEST_CANCEL_2(hrequest);
     return TRUE;
  }
#endif
#ifdef fun_WDF_REQUEST_CANCEL_3
  if(EvtRequestCancel==fun_WDF_REQUEST_CANCEL_3)
  {
     SDV_IRQL_PUSH(internal_device_control_queue_stored_irql);
     fun_WDF_REQUEST_CANCEL_3(hrequest);
     return TRUE;
  }
#endif
#ifdef fun_WDF_REQUEST_CANCEL_4
  if(EvtRequestCancel==fun_WDF_REQUEST_CANCEL_4)
  {
     SDV_IRQL_PUSH(internal_device_control_queue_stored_irql);
     fun_WDF_REQUEST_CANCEL_4(hrequest);
     return TRUE;
  }
#endif
#ifdef fun_WDF_REQUEST_CANCEL_5
  if(EvtRequestCancel==fun_WDF_REQUEST_CANCEL_5)
  {
     SDV_IRQL_PUSH(internal_device_control_queue_stored_irql);
     fun_WDF_REQUEST_CANCEL_5(hrequest);
     return TRUE;
  }
#endif
#ifdef fun_WDF_REQUEST_CANCEL_6
  if(EvtRequestCancel==fun_WDF_REQUEST_CANCEL_6)
  {
     SDV_IRQL_PUSH(internal_device_control_queue_stored_irql);
     fun_WDF_REQUEST_CANCEL_6(hrequest);
     return TRUE;
  }
#endif
return FALSE;
}
return FALSE;
}

BOOLEAN sdv_Set_Default_Request_Cancel_Irql_Level(IN PFN_WDF_REQUEST_CANCEL EvtRequestCancel)
{
if(sdv_inside_io_default_callback)
{
#ifdef fun_WDF_REQUEST_CANCEL_1
  if(EvtRequestCancel==fun_WDF_REQUEST_CANCEL_1)
  {      
     SDV_IRQL_PUSH(default_queue_stored_irql);
     fun_WDF_REQUEST_CANCEL_1(hrequest);
     return TRUE;
  }
#endif
#ifdef fun_WDF_REQUEST_CANCEL_2
  if(EvtRequestCancel==fun_WDF_REQUEST_CANCEL_2)
  {
     SDV_IRQL_PUSH(default_queue_stored_irql);
     fun_WDF_REQUEST_CANCEL_2(hrequest);
     return TRUE;
  }
#endif
#ifdef fun_WDF_REQUEST_CANCEL_3
  if(EvtRequestCancel==fun_WDF_REQUEST_CANCEL_3)
  {
     SDV_IRQL_PUSH(default_queue_stored_irql);
     fun_WDF_REQUEST_CANCEL_3(hrequest);
     return TRUE;
  }
#endif
#ifdef fun_WDF_REQUEST_CANCEL_4
  if(EvtRequestCancel==fun_WDF_REQUEST_CANCEL_4)
  {
     SDV_IRQL_PUSH(default_queue_stored_irql);
     fun_WDF_REQUEST_CANCEL_4(hrequest);
     return TRUE;
  }
#endif
#ifdef fun_WDF_REQUEST_CANCEL_5
  if(EvtRequestCancel==fun_WDF_REQUEST_CANCEL_5)
  {
     SDV_IRQL_PUSH(default_queue_stored_irql);
     fun_WDF_REQUEST_CANCEL_5(hrequest);
     return TRUE;
  }
#endif
#ifdef fun_WDF_REQUEST_CANCEL_6
  if(EvtRequestCancel==fun_WDF_REQUEST_CANCEL_6)
  {
     SDV_IRQL_PUSH(default_queue_stored_irql);
     fun_WDF_REQUEST_CANCEL_6(hrequest);
     return TRUE;
  }
#endif
return FALSE;
}
return FALSE;
}


WDFAPI
NTSTATUS
sdv_WdfIoQueueCreate(
    IN WDFDEVICE Device,
    IN PWDF_IO_QUEUE_CONFIG Config,
    IN OPTIONAL PWDF_OBJECT_ATTRIBUTES QueueAttributes,
    OUT WDFQUEUE* Queue
    )
{ 
if(Queue!=NULL)
{
    int x = SdvMakeChoice();
#if SDV_HARNESS==SDV_WDF_IRQL_SEMI_FLAT_HARNESS
    sdv_Set_Queue_Irql_Level(Config,QueueAttributes);
#endif
    switch (x) { 
        case 0:  *Queue = hdefqueue;
			     return STATUS_SUCCESS;
			     break;
#ifdef SLAM_PRECISE_STATUS
        case 1:  
			     *Queue = hdefqueue;
			     return STATUS_INSUFFICIENT_RESOURCES;
			     break;
        default: 
			     *Queue = hdefqueue;
			     return STATUS_INFO_LENGTH_MISMATCH;
			     break;
#else
        default: 
			     *Queue = hdefqueue;
			     return STATUS_UNSUCCESSFUL;
			     break;
#endif
	}
}
else
{
    int x = SdvMakeChoice();
    #if SDV_HARNESS==SDV_WDF_IRQL_SEMI_FLAT_HARNESS
        sdv_Set_Queue_Irql_Level(Config,QueueAttributes);
    #endif
    switch (x) 
    { 
        case 0:  
			     return STATUS_SUCCESS;
			     break;
        default: 
			     return STATUS_UNSUCCESSFUL;
			     break;
    }
}
}

WDFDEVICE
sdv_WdfIoQueueGetDevice(
    IN WDFQUEUE Queue
    )
{
	return hdevice;
}
PVOID
sdv_WdfObjectGetTypedContextWorker(
    IN WDFOBJECT Handle,
    IN PCWDF_OBJECT_CONTEXT_TYPE_INFO TypeInfo
    )
{
	return hqctx;
}
VOID
sdv_WDF_TIMER_CONFIG_INIT_PERIODIC(
    IN PWDF_TIMER_CONFIG Config,
    IN PFN_WDF_TIMER     EvtTimerFunc,
    IN ULONG             Period
    )
{
}
/* Suppress C4715: 'function' : not all control paths return a value.
       In the os model stubs must check a parameter for null before it is dereferenced.
       SLAM-2 considers all the traces where a null pointer is dereferenced as infeasible ones.
       This is acceptable as the OS Model will not be executed: */
#pragma warning(disable:4715)
WDFAPI
NTSTATUS
sdv_WdfTimerCreate(
    IN  PWDF_TIMER_CONFIG            Config,
    IN  PWDF_OBJECT_ATTRIBUTES Attributes,
    OUT WDFTIMER*              Timer
    )
{ 
if(Timer!=NULL)
{
	int x = SdvMakeChoice();
    switch (x) { 
        case 0: htimer = (WDFTIMER) &notused_4htimer;
			    *Timer = htimer;				
			    return STATUS_SUCCESS;
			    break;
#ifdef SLAM_PRECISE_STATUS
        case 1: 
			    htimer = (WDFTIMER) &notused_4htimer;
			    *Timer = htimer;
			    return STATUS_INSUFFICIENT_RESOURCES;
			    break;
        default: 
			    htimer = (WDFTIMER) &notused_4htimer;
			    *Timer = htimer;
			    return STATUS_INVALID_PARAMETER;
			    break;
#else
        default: 
			    htimer = (WDFTIMER) &notused_4htimer;
			    *Timer = htimer;
			    return STATUS_UNSUCCESSFUL;
			    break;
#endif
	}
}
}
#pragma warning(default:4715)
BOOLEAN
sdv_WdfTimerStart(
    IN WDFTIMER Timer,
    IN LONGLONG DueTime
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
BOOLEAN
sdv_WdfTimerStop(
    IN WDFTIMER Timer,
    IN BOOLEAN Wait
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
WDFOBJECT
FORCEINLINE
sdv_WdfTimerGetParentObject(
    IN WDFTIMER Timer
    )
{
	#pragma warning(disable:4312)
	return (WDFOBJECT)(SdvMakeChoice());
	#pragma warning(default:4312)
}
VOID
sdv_WDF_WORKITEM_CONFIG_INIT(
    OUT PWDF_WORKITEM_CONFIG Config,
    IN PFN_WDF_WORKITEM     EvtWorkItemFunc
    )
{
}
/* Suppress C4715: 'function' : not all control paths return a value.
       In the os model stubs must check a parameter for null before it is dereferenced.
       SLAM-2 considers all the traces where a null pointer is dereferenced as infeasible ones.
       This is acceptable as the OS Model will not be executed: */
#pragma warning(disable:4715)
NTSTATUS
sdv_WdfWorkItemCreate(
    IN PWDF_WORKITEM_CONFIG Config,
	  IN OPTIONAL PWDF_OBJECT_ATTRIBUTES Attributes,
    OUT WDFWORKITEM* WorkItem
    )
{
if(WorkItem!=NULL)
{
	int x = SdvMakeChoice();
    switch (x) { 
        case 0: hwork_item = (WDFWORKITEM) &notused_4hwork_item;
			    *WorkItem = hwork_item;				
			    return STATUS_SUCCESS;
			    break;
		default: 
			    hwork_item = (WDFWORKITEM) &notused_4hwork_item;
			    *WorkItem = hwork_item;
			    return STATUS_UNSUCCESSFUL;
			    break;
	}
}
}
#pragma warning(default:4715)
VOID
sdv_WdfWorkItemEnqueue(
    IN WDFWORKITEM WorkItem
    )
{
}
VOID
sdv_WDF_DPC_CONFIG_INIT(
    IN PWDF_DPC_CONFIG Config,
    IN PFN_WDF_DPC     EvtDpcFunc
    )
{
}
/* Suppress C4715: 'function' : not all control paths return a value.
       In the os model stubs must check a parameter for null before it is dereferenced.
       SLAM-2 considers all the traces where a null pointer is dereferenced as infeasible ones.
       This is acceptable as the OS Model will not be executed: */
#pragma warning(disable:4715)
NTSTATUS
sdv_WdfDpcCreate(
    IN PWDF_DPC_CONFIG Config,
    IN PWDF_OBJECT_ATTRIBUTES Attributes,
    OUT WDFDPC* Dpc)
{
if(Dpc!=NULL)
{
	int x = SdvMakeChoice();
    switch (x) { 
        case 0: hdpc = (WDFDPC) &notused_4hdpc;
			    *Dpc = hdpc;				
			    return STATUS_SUCCESS;
			    break;
		default: 
			    hdpc = (WDFDPC) &notused_4hdpc;
			    *Dpc = hdpc;
			    return STATUS_UNSUCCESSFUL;
			    break;
	}
}
}
#pragma warning(default:4715)
BOOLEAN
sdv_WdfDpcEnqueue(
    IN WDFDPC Dpc
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
BOOLEAN
sdv_WdfDpcCancel(
    IN WDFDPC Dpc,
    IN BOOLEAN Wait
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
size_t
sdv_WdfCommonBufferGetLength(
    IN WDFCOMMONBUFFER CommonBuffer
    )
{ 
  return (size_t) SdvMakeChoice();
}

PHYSICAL_ADDRESS
sdv_WdfCommonBufferGetAlignedLogicalAddress(
    IN WDFCOMMONBUFFER CommonBuffer
    )
{
	PHYSICAL_ADDRESS x = {0};
	return x;
}

PVOID
sdv_WdfCommonBufferGetAlignedVirtualAddress(
    IN WDFCOMMONBUFFER CommonBuffer
	)
{
	return (PVOID) pnotused;
}
/* Suppress C4715: 'function' : not all control paths return a value.
       In the os model stubs must check a parameter for null before it is dereferenced.
       SLAM-2 considers all the traces where a null pointer is dereferenced as infeasible ones.
       This is acceptable as the OS Model will not be executed: */
#pragma warning(disable:4715)
NTSTATUS
sdv_WdfCommonBufferCreate(
    IN WDFDMAENABLER DmaEnabler,
    IN size_t Length,
	  IN OPTIONAL PWDF_OBJECT_ATTRIBUTES Attributes,
    OUT WDFCOMMONBUFFER* CommonBufferHandle
    )
{
if(CommonBufferHandle!=NULL)
{
    LONG choice = SdvMakeChoice();
    switch(choice) 
    {
          case 0:
              *CommonBufferHandle=NULL;
              return (NTSTATUS) STATUS_NOT_SUPPORTED;
              break;
          default:
              *CommonBufferHandle = (WDFCOMMONBUFFER)malloc(1);
              return (NTSTATUS) STATUS_SUCCESS;
              break;
    }
}
}
#pragma warning(default:4715)

VOID
sdv_WdfRequestSetCompletionRoutine(
    IN WDFREQUEST Request,
    IN PFN_WDF_REQUEST_COMPLETION_ROUTINE CompletionRoutine,
    IN WDFCONTEXT CompletionContext
    )
{
}
/* Suppress C4715: 'function' : not all control paths return a value.
       In the os model stubs must check a parameter for null before it is dereferenced.
       SLAM-2 considers all the traces where a null pointer is dereferenced as infeasible ones.
       This is acceptable as the OS Model will not be executed: */
#pragma warning(disable:4715)
NTSTATUS
sdv_WdfRequestCreate(
    IN OPTIONAL PWDF_OBJECT_ATTRIBUTES RequestAttributes,
    IN OPTIONAL WDFIOTARGET IoTarget,
    OUT WDFREQUEST* Request
    )
{
if(Request!=NULL)
{
    LONG choice = SdvMakeChoice();
    switch(choice) 
    {
          case 0:
              *Request=NULL;
              return (NTSTATUS) STATUS_NOT_SUPPORTED;
              break;
          default:
              *Request = (WDFREQUEST) malloc(1);
              return (NTSTATUS) STATUS_SUCCESS;
              break;
    }
}
}
#pragma warning(default:4715)
WDFFILEOBJECT
sdv_WdfRequestGetFileObject(
    IN WDFREQUEST Request
    )
{
	return hfileobj;
}
/* Suppress C4715: 'function' : not all control paths return a value.
       In the os model stubs must check a parameter for null before it is dereferenced.
       SLAM-2 considers all the traces where a null pointer is dereferenced as infeasible ones.
       This is acceptable as the OS Model will not be executed: */
#pragma warning(disable:4715)
NTSTATUS
sdv_WdfRequestRetrieveOutputBuffer(
	IN WDFREQUEST Request,
	IN size_t  MinimumRequiredSize,
	OUT PVOID* Buffer,
	OUT size_t* Length
	)
{
if(Length!=NULL)
{
	int x = SdvMakeChoice();
	*Length = (size_t)malloc(1);
    switch (x) { 
        case 0:  hbuffer = (PVOID) &notused_4hbuffer;
			     *Buffer = (PVOID) hbuffer;      
			     return STATUS_SUCCESS;
			     break;
        default: 
			     hbuffer = (PVOID) &notused_4hbuffer;
			     *Buffer = (PVOID) hbuffer;  
			     return STATUS_UNSUCCESSFUL;
			     break;
	}
}
else
{
	int x = SdvMakeChoice();
	switch (x) 
  { 
        case 0:  hbuffer = (PVOID) &notused_4hbuffer;
			     *Buffer = (PVOID) hbuffer;      
			     return STATUS_SUCCESS;
			     break;
        default: 
			     hbuffer = (PVOID) &notused_4hbuffer;
			     *Buffer = (PVOID) hbuffer;  
			     return STATUS_UNSUCCESSFUL;
			     break;
	}
}
}
#pragma warning(default:4715)
/* Suppress C4715: 'function' : not all control paths return a value.
       In the os model stubs must check a parameter for null before it is dereferenced.
       SLAM-2 considers all the traces where a null pointer is dereferenced as infeasible ones.
       This is acceptable as the OS Model will not be executed: */
#pragma warning(disable:4715)
NTSTATUS
sdv_WdfRequestRetrieveInputBuffer(
    IN WDFREQUEST Request,
    IN size_t  MinimumRequiredSize,
    OUT PVOID* Buffer,
    OUT size_t* Length
    )
{
if(Length!=NULL)
{
    int x = SdvMakeChoice();
    *Length = (size_t)malloc(1);
    switch (x) 
    { 
        case 0:  hbuffer = (PVOID) &notused_4hbuffer;
			     *Buffer = (PVOID) hbuffer;      
			     return STATUS_SUCCESS;
			     break;
        default: 
			     hbuffer = (PVOID) &notused_4hbuffer;
			     *Buffer = (PVOID) hbuffer;
			     return STATUS_UNSUCCESSFUL;
			     break;
	}
}
else
{
    int x = SdvMakeChoice();
	switch (x) 
    { 
        case 0:  hbuffer = (PVOID) &notused_4hbuffer;
			     *Buffer = (PVOID) hbuffer;      
			     return STATUS_SUCCESS;
			     break;
        default: 
			     hbuffer = (PVOID) &notused_4hbuffer;
			     *Buffer = (PVOID) hbuffer;
			     return STATUS_UNSUCCESSFUL;
			     break;
	}
}
}
NTSTATUS
sdv_WdfRequestRetrieveUnsafeUserInputBuffer(
    IN WDFREQUEST Request,
    IN size_t MinimumRequiredLength,
    OUT PVOID* InputBuffer,
    OUT OPTIONAL size_t* Length
    )
{
if(Length!=NULL)
{

	  int x = SdvMakeChoice();
	  *Length = (size_t)malloc(1);
    switch (x) 
    { 
        case 0:  hbuffer = (PVOID) &notused_4hbuffer;
			     *InputBuffer = (PVOID) hbuffer;      
			     return STATUS_SUCCESS;
			     break;
        default: 
			     hbuffer = (PVOID) &notused_4hbuffer;
			     *InputBuffer = (PVOID) hbuffer;
			     return STATUS_UNSUCCESSFUL;
			     break;
	  }
}
else
{
    int x = SdvMakeChoice();
	  switch (x) 
    { 
        case 0:  hbuffer = (PVOID) &notused_4hbuffer;
			     *InputBuffer = (PVOID) hbuffer;      
			     return STATUS_SUCCESS;
			     break;
        default: 
			     hbuffer = (PVOID) &notused_4hbuffer;
			     *InputBuffer = (PVOID) hbuffer;
			     return STATUS_UNSUCCESSFUL;
			     break;
	  }
}
}
NTSTATUS
sdv_WdfRequestRetrieveUnsafeUserOutputBuffer(
    IN WDFREQUEST Request,
    IN size_t MinimumRequiredLength,
    OUT PVOID* OutputBuffer,
    OUT OPTIONAL size_t* Length
    )
{
if(Length!=NULL)
{
	  int x = SdvMakeChoice();
	  *Length = (size_t)malloc(1);
    switch (x) 
    { 
        case 0:  hbuffer = (PVOID) &notused_4hbuffer;
			     *OutputBuffer = (PVOID) hbuffer;      
			     return STATUS_SUCCESS;
			     break;
        default: 
			     hbuffer = (PVOID) &notused_4hbuffer;
			     *OutputBuffer = (PVOID) hbuffer;
			     return STATUS_UNSUCCESSFUL;
			     break;
	 }
}
else
{
	  int x = SdvMakeChoice();
    switch (x) 
    { 
        case 0:  hbuffer = (PVOID) &notused_4hbuffer;
			     *OutputBuffer = (PVOID) hbuffer;      
			     return STATUS_SUCCESS;
			     break;
        default: 
			     hbuffer = (PVOID) &notused_4hbuffer;
			     *OutputBuffer = (PVOID) hbuffer;
			     return STATUS_UNSUCCESSFUL;
			     break;
	 }

}

}
/* Suppress C4715: 'function' : not all control paths return a value.
       In the os model stubs must check a parameter for null before it is dereferenced.
       SLAM-2 considers all the traces where a null pointer is dereferenced as infeasible ones.
       This is acceptable as the OS Model will not be executed: */
#pragma warning(disable:4715)
NTSTATUS
sdv_WdfRequestRetrieveOutputMemory(
	IN WDFREQUEST Request,
	OUT WDFMEMORY* Memory
	)
{
if(Memory!=NULL)
{
	int x = SdvMakeChoice();
    switch (x) { 
        case 0:  hmemory = (WDFMEMORY) &notused_4hmemory;
			     *Memory = hmemory;      
			     return STATUS_SUCCESS;
			     break;
        default: 
			     hmemory = (WDFMEMORY) &notused_4hmemory;
			     *Memory = hmemory; 
			     return STATUS_NOT_SUPPORTED;
			     break;
	}
}
}
#pragma warning(default:4715)
/* Suppress C4715: 'function' : not all control paths return a value.
       In the os model stubs must check a parameter for null before it is dereferenced.
       SLAM-2 considers all the traces where a null pointer is dereferenced as infeasible ones.
       This is acceptable as the OS Model will not be executed: */
#pragma warning(disable:4715)
NTSTATUS
sdv_WdfRequestRetrieveInputMemory(
	IN WDFREQUEST Request,
	OUT WDFMEMORY* Memory
	)
{
if(Memory!=NULL)
{
	int x = SdvMakeChoice();
    switch (x) { 
        case 0:  hmemory = (WDFMEMORY) &notused_4hmemory;
			     *Memory = hmemory;      
			     return STATUS_SUCCESS;
			     break;
        default: 
			     hmemory = (WDFMEMORY) &notused_4hmemory;
			     *Memory = hmemory;
			     return STATUS_NOT_SUPPORTED;
			     break;
	}
}
}
#pragma warning(default:4715)
/* Suppress C4715: 'function' : not all control paths return a value.
       In the os model stubs must check a parameter for null before it is dereferenced.
       SLAM-2 considers all the traces where a null pointer is dereferenced as infeasible ones.
       This is acceptable as the OS Model will not be executed: */
#pragma warning(disable:4715)
NTSTATUS
sdv_WdfRequestRetrieveOutputWdmMdl(
	IN WDFREQUEST Request,
	OUT PMDL* Mdl
	)
{
if(Mdl!=NULL)
{
	int x = SdvMakeChoice();
    switch (x) { 
        case 0:  hmdl = (PMDL) &notused_4hmdl;
			     *Mdl = hmdl;      
			     return STATUS_SUCCESS;
			     break;
        default: 
			     hmdl = (PMDL) &notused_4hmdl;
			     *Mdl = hmdl;
			     return STATUS_NOT_SUPPORTED;
			     break;
	}
}
}
#pragma warning(default:4715)
/* Suppress C4715: 'function' : not all control paths return a value.
       In the os model stubs must check a parameter for null before it is dereferenced.
       SLAM-2 considers all the traces where a null pointer is dereferenced as infeasible ones.
       This is acceptable as the OS Model will not be executed: */
#pragma warning(disable:4715)
NTSTATUS
sdv_WdfRequestRetrieveInputWdmMdl(
	IN WDFREQUEST Request,
	OUT PMDL* Mdl
	)
{
if(Mdl!=NULL)
{
	int x = SdvMakeChoice();
    switch (x) { 
        case 0:  hmdl = (PMDL) &notused_4hmdl;
			     *Mdl = hmdl;      
			     return STATUS_SUCCESS;
			     break;
        default: 
			     hmdl = (PMDL) &notused_4hmdl;
			     *Mdl = hmdl; 
			     return STATUS_NOT_SUPPORTED;
			     break;
	}
}
}
#pragma warning(default:4715)
PVOID
sdv_WdfMemoryGetBuffer(
    IN WDFMEMORY Memory,
    OUT OPTIONAL size_t* BufferSize
    )
{
if(BufferSize!=NULL)
{
    *BufferSize = (size_t)malloc(1);
	return (PVOID) pnotused;
}
else
{
  return (PVOID) pnotused;
}
}
VOID
sdv_WDF_MEMORY_DESCRIPTOR_INIT_HANDLE(
    OUT PWDF_MEMORY_DESCRIPTOR Descriptor,
    IN WDFMEMORY Memory,
    IN OPTIONAL PWDFMEMORY_OFFSET Offsets
    )
{
}
VOID
sdv_WDF_MEMORY_DESCRIPTOR_INIT_BUFFER(
    OUT PWDF_MEMORY_DESCRIPTOR Descriptor,
    IN PVOID Buffer,
    IN ULONG BufferLength
    )
{
}
/* Suppress C4715: 'function' : not all control paths return a value.
       In the os model stubs must check a parameter for null before it is dereferenced.
       SLAM-2 considers all the traces where a null pointer is dereferenced as infeasible ones.
       This is acceptable as the OS Model will not be executed: */
#pragma warning(disable:4715)
NTSTATUS
sdv_WdfMemoryCreate(
    IN OPTIONAL PWDF_OBJECT_ATTRIBUTES Attributes,
    IN POOL_TYPE PoolType,
    IN OPTIONAL ULONG PoolTag,
    IN size_t BufferSize,
    OUT WDFMEMORY* Memory,
    OUT OPTIONAL PVOID* Buffer
    )
{
if(Memory!=NULL)
{
  LONG choice = SdvMakeChoice();

  switch(choice) 
	{
        case 0:
			      *Memory=NULL;
            return (NTSTATUS) STATUS_NOT_SUPPORTED;
            break;
        default:
			      *Memory = (WDFMEMORY)malloc(1);
            return (NTSTATUS) STATUS_SUCCESS;
            break;
    }
}
}
#pragma warning(default:4715)
/* Suppress C4715: 'function' : not all control paths return a value.
       In the os model stubs must check a parameter for null before it is dereferenced.
       SLAM-2 considers all the traces where a null pointer is dereferenced as infeasible ones.
       This is acceptable as the OS Model will not be executed: */
#pragma warning(disable:4715)
NTSTATUS
sdv_WdfMemoryCreatePreallocated(
    IN OPTIONAL PWDF_OBJECT_ATTRIBUTES Attributes,
    IN PVOID Buffer,
    IN size_t BufferSize,
    OUT WDFMEMORY* Memory
    )
{
if(Memory!=NULL)
{
  LONG choice = SdvMakeChoice();

  switch(choice) 
	{
        case 0:
			*Memory=NULL;
            return (NTSTATUS) STATUS_NOT_SUPPORTED;
            break;
        default:
			*Memory = (WDFMEMORY)malloc(1);
            return (NTSTATUS) STATUS_SUCCESS;
            break;
    }
}
}
#pragma warning(default:4715)

NTSTATUS
sdv_WdfMemoryAssignBuffer(
    IN WDFMEMORY Memory,
    IN PVOID Buffer,
    IN size_t BufferSize
    )
{
	return sdv_SetReturnStatus();
}

NTSTATUS
sdv_WdfMemoryCopyFromBuffer(
    IN WDFMEMORY DestinationMemory,
    IN size_t DestinationOffset,
    IN PVOID Buffer,
    IN size_t NumBytesToCopyFrom
    )
{
	return sdv_SetReturnStatus();
}

NTSTATUS
sdv_WdfMemoryCopyToBuffer(
    IN WDFMEMORY SourceMemory,
    IN size_t SourceOffset,
    IN OUT PVOID Buffer,
    IN size_t NumBytesToCopyTo
    )
{
	return sdv_SetReturnStatus();
}

VOID
sdv_WDF_MEMORY_DESCRIPTOR_INIT_MDL(
    OUT PWDF_MEMORY_DESCRIPTOR Descriptor,
    IN PMDL Mdl,
    IN ULONG BufferLength
    )
{
}

NTSTATUS
sdv_WdfDmaTransactionInitialize(
    IN WDFDMATRANSACTION DmaTransaction,
    IN PFN_WDF_PROGRAM_DMA EvtProgramDmaFunction,
    IN WDF_DMA_DIRECTION DmaDirection,
    IN PMDL Mdl,
    IN PVOID VirtualAddress,
    IN size_t  Length
    )
{
	return sdv_SetReturnStatus();
}
/* Suppress C4715: 'function' : not all control paths return a value.
       In the os model stubs must check a parameter for null before it is dereferenced.
       SLAM-2 considers all the traces where a null pointer is dereferenced as infeasible ones.
       This is acceptable as the OS Model will not be executed: */
#pragma warning(disable:4715)
NTSTATUS
sdv_WdfDmaTransactionCreate(
    IN WDFDMAENABLER DmaEnabler,
    IN OPTIONAL PWDF_OBJECT_ATTRIBUTES Attributes,
    OUT WDFDMATRANSACTION* DmaTransaction
    )
{
if(DmaTransaction!=NULL)
{
  LONG choice = SdvMakeChoice();

  switch(choice) 
	{
        case 0:
			*DmaTransaction=NULL;
            return (NTSTATUS) STATUS_NOT_SUPPORTED;
            break;
        default:
			*DmaTransaction = (WDFDMATRANSACTION)malloc(1);
            return (NTSTATUS) STATUS_SUCCESS;
            break;
    }
}
}
#pragma warning(default:4715)

WDFDEVICE
sdv_WdfDmaTransactionGetDevice(
    IN WDFDMATRANSACTION DmaTransaction
    )
{
	return (WDFDEVICE) pnotused;
}

size_t
sdv_WdfDmaTransactionGetCurrentDmaTransferLength(
    __in
    WDFDMATRANSACTION DmaTransaction
    )
{
	return (size_t) SdvMakeChoice();
}

WDFREQUEST
sdv_WdfDmaTransactionGetRequest(
    IN WDFDMATRANSACTION DmaTransaction
    )
{
	return (WDFREQUEST) pnotused;
}

VOID
sdv_WdfDmaTransactionSetMaximumLength(
    IN WDFDMATRANSACTION DmaTransaction,
    IN size_t MaximumLength
    )
{
}

size_t
sdv_WdfDmaTransactionGetBytesTransferred(
    IN WDFDMATRANSACTION DmaTransaction
    )
{
	return (size_t) SdvMakeChoice();
}
/* Suppress C4715: 'function' : not all control paths return a value.
       In the os model stubs must check a parameter for null before it is dereferenced.
       SLAM-2 considers all the traces where a null pointer is dereferenced as infeasible ones.
       This is acceptable as the OS Model will not be executed: */
#pragma warning(disable:4715)
BOOLEAN
sdv_WdfDmaTransactionDmaCompletedFinal(
    IN WDFDMATRANSACTION DmaTransaction,
    IN size_t FinalTransferredLength,
    OUT NTSTATUS* Status
    )
{
if(Status!=NULL)
{
  LONG choice = SdvMakeChoice();
  switch(choice) 
  {
      case 0:
            *Status=STATUS_MORE_PROCESSING_REQUIRED;
            return FALSE;
            break;
      case 1:
            *Status=STATUS_UNSUCCESSFUL;
            return TRUE;
            break;
      default:
            *Status = STATUS_SUCCESS;
            return TRUE;
            break;
  }
}
}
#pragma warning(default:4715)
/* Suppress C4715: 'function' : not all control paths return a value.
       In the os model stubs must check a parameter for null before it is dereferenced.
       SLAM-2 considers all the traces where a null pointer is dereferenced as infeasible ones.
       This is acceptable as the OS Model will not be executed: */
#pragma warning(disable:4715)
BOOLEAN
sdv_WdfDmaTransactionDmaCompletedWithLength(
    IN WDFDMATRANSACTION DmaTransaction,
    IN size_t TransferredLength,
    OUT NTSTATUS* Status
    )
{
if(Status!=NULL)
{
  LONG choice = SdvMakeChoice();
  switch(choice) 
  {
      case 0:
            *Status=STATUS_MORE_PROCESSING_REQUIRED;
            return FALSE;
            break;
      case 1:
            *Status=STATUS_UNSUCCESSFUL;
            return TRUE;
            break;
      default:
            *Status = STATUS_SUCCESS;
            return TRUE;
            break;
  }
}
}
#pragma warning(default:4715)
/* Suppress C4715: 'function' : not all control paths return a value.
       In the os model stubs must check a parameter for null before it is dereferenced.
       SLAM-2 considers all the traces where a null pointer is dereferenced as infeasible ones.
       This is acceptable as the OS Model will not be executed: */
#pragma warning(disable:4715)
BOOLEAN
sdv_WdfDmaTransactionDmaCompleted(
    IN WDFDMATRANSACTION DmaTransaction,
    OUT NTSTATUS* Status
    )
{
if(Status!=NULL)
{
  LONG choice = SdvMakeChoice();
  switch(choice) 
  {
      case 0:
            *Status=STATUS_MORE_PROCESSING_REQUIRED;
            return FALSE;
            break;
      case 1:
            *Status=STATUS_UNSUCCESSFUL;
            return TRUE;
            break;
      default:
            *Status = STATUS_SUCCESS;
            return TRUE;
            break;
  }
}
}
#pragma warning(default:4715)

NTSTATUS
sdv_WdfDmaTransactionRelease(
    IN WDFDMATRANSACTION DmaTransaction
    )
{
	return sdv_SetReturnStatus();
}


NTSTATUS
sdv_WdfDmaTransactionExecute(
    IN WDFDMATRANSACTION DmaTransaction,
    IN OPTIONAL PVOID Context
	)
{
    return sdv_SetReturnStatus();
}

NTSTATUS
sdv_WdfDmaTransactionInitializeUsingRequest(
    IN WDFDMATRANSACTION DmaTransaction,
    IN WDFREQUEST Request,
    IN PFN_WDF_PROGRAM_DMA EvtProgramDmaFunction,
    IN WDF_DMA_DIRECTION DmaDirection
    )
{
	return sdv_SetReturnStatus();
}

VOID
sdv_WdfDmaEnablerSetMaximumScatterGatherElements(
    IN WDFDMAENABLER DmaEnabler,
    IN size_t MaximumFragments
    )
{
}

size_t
sdv_WdfDmaEnablerGetMaximumScatterGatherElements(
    IN WDFDMAENABLER DmaEnabler
    )
{
	return (size_t) SdvMakeChoice();
}

size_t
sdv_WdfDmaEnablerGetMaximumLength(
    IN WDFDMAENABLER DmaEnabler
    )
{
	return (size_t) SdvMakeChoice();
}
/* Suppress C4715: 'function' : not all control paths return a value.
       In the os model stubs must check a parameter for null before it is dereferenced.
       SLAM-2 considers all the traces where a null pointer is dereferenced as infeasible ones.
       This is acceptable as the OS Model will not be executed: */
#pragma warning(disable:4715)
NTSTATUS
sdv_WdfDmaEnablerCreate(
    IN WDFDEVICE Device,
    IN PWDF_DMA_ENABLER_CONFIG Config,
    IN OPTIONAL PWDF_OBJECT_ATTRIBUTES Attributes,
    OUT WDFDMAENABLER* DmaEnablerHandle
    )
{
if(DmaEnablerHandle!=NULL)
{
    LONG choice = SdvMakeChoice();
    switch(choice) 
    {
        case 0:
              *DmaEnablerHandle=NULL;
              return (NTSTATUS) STATUS_NOT_SUPPORTED;
              break;
        default:
              *DmaEnablerHandle = (WDFDMAENABLER)malloc(1);
              return (NTSTATUS) STATUS_SUCCESS;
              break;
    }
}
}
#pragma warning(default:4715)

VOID
sdv_WDF_DMA_ENABLER_CONFIG_INIT(
    IN  PWDF_DMA_ENABLER_CONFIG  Config,
    IN  WDF_DMA_PROFILE          Profile,
    IN  size_t                   MaximumLength
    )
{
}
NTSTATUS
sdv_WdfRegistryAssignULong(
    IN WDFKEY Key,
    IN PCUNICODE_STRING ValueName,
    IN ULONG Value
    )
{
	return sdv_SetReturnStatus();
}


NTSTATUS
sdv_WdfRegistryAssignString(
    IN WDFKEY Key,
    IN PCUNICODE_STRING ValueName,
    IN WDFSTRING String
    )
{
	return sdv_SetReturnStatus();
}

NTSTATUS
sdv_WdfRegistryAssignMultiString(
    IN WDFKEY Key,
    IN PCUNICODE_STRING ValueName,
    IN WDFCOLLECTION StringsCollection
	)
{
	return sdv_SetReturnStatus();
}

NTSTATUS
sdv_WdfRegistryAssignMemory(
    IN WDFKEY Key,
    IN PCUNICODE_STRING ValueName,
    IN ULONG ValueType,
    IN WDFMEMORY Memory,
    IN OPTIONAL PWDFMEMORY_OFFSET MemoryOffsets
    )
{
	return sdv_SetReturnStatus();
}

NTSTATUS
sdv_WdfRegistryAssignValue(
    IN WDFKEY Key,
    IN PCUNICODE_STRING ValueName,
    IN ULONG ValueType,
    IN ULONG ValueLength,
    IN PVOID Value
    )
{
	return sdv_SetReturnStatus();
}

NTSTATUS
sdv_WdfRegistryQueryULong(
    IN WDFKEY Key,
    IN PCUNICODE_STRING ValueName,
    OUT PULONG Value
    )
{
#pragma warning(disable:4312)
#pragma warning(default:4312)
	return sdv_SetReturnStatus();
}

NTSTATUS
sdv_WdfRegistryQueryString(
    IN WDFKEY Key,
    IN PCUNICODE_STRING ValueName,
    IN WDFSTRING  String    
    )
{
#pragma warning(disable:4312)
#pragma warning(default:4312)
	return sdv_SetReturnStatus();
}
VOID
sdv_WdfRegistryClose(
    IN WDFKEY Key
    )
{
	
}
VOID
sdv_WDF_OBJECT_ATTRIBUTES_INIT(
    OUT PWDF_OBJECT_ATTRIBUTES Attributes
    )
{
    Attributes->ExecutionLevel=WdfExecutionLevelInheritFromParent;
    Attributes->SynchronizationScope=WdfSynchronizationScopeInheritFromParent;
}


VOID
sdv_WdfObjectReference(
    IN WDFOBJECT Handle)
{
	
}

VOID
sdv_WdfObjectDereference(
    IN WDFOBJECT Handle)
{
	
}

WDFAPI
VOID
sdv_WdfRequestComplete(
   IN  WDFREQUEST Request,
   IN  NTSTATUS  RequestStatus
   )
{
	if (Request == hrequest) {
		if (reqpresented == 1) reqcompleted = 1;}
	if (Request == hrequest1) {           
	          if (reqpresented1 == 1) reqcompleted1 = 1;}
	
}

WDFAPI
VOID
sdv_WdfRequestCompleteWithInformation(
   IN  WDFREQUEST Request,
   IN  NTSTATUS  RequestStatus,
   IN ULONG_PTR Information
   )
{
    if (Request == hrequest) {
		if (reqpresented == 1) reqcompleted = 1;}
	if (Request == hrequest1) {           
	          if (reqpresented1 == 1) reqcompleted1 = 1;}
	
}

VOID
sdv_WdfRequestCompleteWithPriorityBoost(
    IN WDFREQUEST Request,
    IN NTSTATUS Status,
    IN CCHAR PriorityBoost
    )
{
    if (Request == hrequest) {
		if (reqpresented == 1) reqcompleted = 1;}
	if (Request == hrequest1) {           
	          if (reqpresented1 == 1) reqcompleted1 = 1;}
	
}

VOID
sdv_WDF_REQUEST_PARAMETERS_INIT(
    OUT PWDF_REQUEST_PARAMETERS Parameters
    )
{
}

VOID
sdv_WdfRequestGetParameters(
    IN WDFREQUEST Request,
    OUT PWDF_REQUEST_PARAMETERS Parameters
    )
{
}

VOID
sdv_WdfRequestMarkCancelable(
    IN WDFREQUEST Request,
    IN PFN_WDF_REQUEST_CANCEL EvtRequestCancel
    )
{
#if SDV_HARNESS==SDV_WDF_IRQL_SEMI_FLAT_HARNESS
   BOOLEAN temp=0;
   temp=sdv_Set_Read_Request_Cancel_Irql_Level(EvtRequestCancel);
   if(!temp)
   {
       temp=sdv_Set_Write_Request_Cancel_Irql_Level(EvtRequestCancel);
   }
   if(!temp)
   {
       temp=sdv_Set_Device_Control_Request_Cancel_Irql_Level(EvtRequestCancel);
   }
   if(!temp)
   {
       temp=sdv_Set_Internal_Device_Control_Request_Cancel_Irql_Level(EvtRequestCancel);
   }
   if(!temp)
   {
       temp=sdv_Set_Default_Request_Cancel_Irql_Level(EvtRequestCancel);
   }
#endif
   hact_suspend = WdfRequestStopActionSuspend | WdfRequestStopRequestCancelable;
}

NTSTATUS
sdv_WdfRequestUnmarkCancelable(
    IN WDFREQUEST Request
    )
{
	LONG choice = SdvMakeChoice();

    switch(choice) {
        case 0:
            return (NTSTATUS) STATUS_NOT_SUPPORTED;
            break;
        case 1:
           hact_suspend = WdfRequestStopActionSuspend;
			     return (NTSTATUS) STATUS_CANCELLED;
			     break;
        default:
            hact_suspend = WdfRequestStopActionSuspend;
            return (NTSTATUS) STATUS_SUCCESS;
            break;
	}
	
}

BOOLEAN
sdv_WdfRequestIsCanceled(
    IN WDFREQUEST Request
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

WDFAPI
BOOLEAN
sdv_WdfRequestSend(
    IN WDFREQUEST Request,
    IN WDFIOTARGET Target,
    IN OPTIONAL PWDF_REQUEST_SEND_OPTIONS RequestOptions
    )
{
	LONG choice = SdvMakeChoice();
  switch(choice) 
  {
      case 0:
          #ifdef SDV_COMPLEX_REQUEST_OBJ
     		      ((complexRequest*)Request)->reqSentSuccessfully=FALSE;
		      #endif
		      return FALSE;	  
          break;
      default:
          #ifdef SDV_COMPLEX_REQUEST_OBJ
			        ((complexRequest*)Request)->reqSentSuccessfully=TRUE;
			    #endif		
          return TRUE;
          break;
  }

}
NTSTATUS
sdv_WdfRequestGetStatus(
    IN WDFREQUEST Request
    )
{
	#ifdef SDV_COMPLEX_REQUEST_OBJ
  if ( ((complexRequest*)Request)->reqSentSuccessfully==FALSE)
	{ 
			return (NTSTATUS) STATUS_INVALID_PARAMETER;
	}
	#endif	 
	return sdv_SetReturnStatus();
}
WDFQUEUE
sdv_WdfRequestGetIoQueue(
    IN WDFREQUEST Request
    )
{
	return hdefqueue;
}
WDFAPI
NTSTATUS
sdv_WdfIoTargetSendReadSynchronously (
    IN WDFIOTARGET IoTarget,
    IN OPTIONAL WDFREQUEST Request,
    IN OPTIONAL PWDF_MEMORY_DESCRIPTOR OutputBuffer,
    IN OPTIONAL PLONGLONG  DeviceOffset,
    IN OPTIONAL PWDF_REQUEST_SEND_OPTIONS RequestOptions,
    OUT OPTIONAL PULONG_PTR BytesRead
    )
{
    LONG choice = SdvMakeChoice();
    switch(choice) 
    {
        case 0:
              return (NTSTATUS) STATUS_NOT_SUPPORTED;
              break;
        default:
              return (NTSTATUS) STATUS_SUCCESS;
              break;
    }
}
WDFAPI
NTSTATUS
sdv_WdfIoTargetSendWriteSynchronously (
    IN WDFIOTARGET IoTarget,
    IN OPTIONAL WDFREQUEST Request,
    IN OPTIONAL PWDF_MEMORY_DESCRIPTOR InputBuffer,
    IN OPTIONAL PLONGLONG DeviceOffset,
    IN OPTIONAL PWDF_REQUEST_SEND_OPTIONS RequestOptions,
    OUT OPTIONAL PULONG_PTR BytesWritten
    )
{
    LONG choice = SdvMakeChoice();
    switch(choice) 
    {
        case 0:
              return (NTSTATUS) STATUS_NOT_SUPPORTED;
              break;
        default:
              return (NTSTATUS) STATUS_SUCCESS;
              break;
    }
}
WDFAPI
NTSTATUS
sdv_WdfIoTargetSendIoctlSynchronously (
    IN WDFIOTARGET IoTarget,
    IN OPTIONAL WDFREQUEST Request,
    IN ULONG IoctlCode,
    IN OPTIONAL PWDF_MEMORY_DESCRIPTOR InputBuffer,
    IN OPTIONAL PWDF_MEMORY_DESCRIPTOR OutputBuffer,
    IN OPTIONAL PWDF_REQUEST_SEND_OPTIONS RequestOptions,
    OUT OPTIONAL PULONG_PTR BytesReturned
    )
{
    LONG choice = SdvMakeChoice();
    switch(choice) 
    {
        case 0:
              return (NTSTATUS) STATUS_NOT_SUPPORTED;
              break;
        default:
              return (NTSTATUS) STATUS_SUCCESS;
              break;
    }
}
WDFAPI
NTSTATUS
sdv_WdfIoTargetSendInternalIoctlSynchronously (
    IN WDFIOTARGET IoTarget,
    IN OPTIONAL WDFREQUEST Request,
    IN ULONG IoctlCode,
    IN OPTIONAL PWDF_MEMORY_DESCRIPTOR InputBuffer,
    IN OPTIONAL PWDF_MEMORY_DESCRIPTOR OutputBuffer,
    IN OPTIONAL PWDF_REQUEST_SEND_OPTIONS RequestOptions,
    OUT OPTIONAL PULONG_PTR BytesReturned
    )
{
  LONG choice = SdvMakeChoice();
  switch(choice) 
  {
      case 0:
            return (NTSTATUS) STATUS_NOT_SUPPORTED;
            break;
      default:
            return (NTSTATUS) STATUS_SUCCESS;
            break;
  }
}

WDFAPI
NTSTATUS
sdv_WdfIoTargetSendInternalIoctlOthersSynchronously (
    IN WDFIOTARGET IoTarget,
    IN OPTIONAL WDFREQUEST Request,
    IN ULONG IoctlCode,
    IN OPTIONAL PVOID OtherArg1,
    IN OPTIONAL PVOID OtherArg2,
    IN OPTIONAL PVOID OtherArg4,
    IN OPTIONAL PWDF_REQUEST_SEND_OPTIONS RequestOptions,
    OUT PULONG_PTR BytesReturned
    )
{
	LONG choice = SdvMakeChoice();
  switch(choice) 
  {
      case 0:
            return (NTSTATUS) STATUS_NOT_SUPPORTED;
            break;
      default:
            return (NTSTATUS) STATUS_SUCCESS;
            break;
  }
}

VOID
sdv_WdfIoTargetStop(
    OUT WDFIOTARGET IoTarget,
    IN WDF_IO_TARGET_SENT_IO_ACTION  Action
    )
{
	
}
WDFAPI
NTSTATUS
sdv_WdfDeviceEnqueueRequest(
    IN WDFDEVICE Device,
    IN WDFREQUEST Request
    )
{
	return sdv_SetReturnStatus();
}
NTSTATUS
sdv_WdfRequestForwardToIoQueue(
    IN WDFREQUEST Request,
    IN WDFQUEUE DestinationQueue
    )
{
	return sdv_SetReturnStatus();
}
VOID
sdv_WdfRequestStopAcknowledge(
    IN WDFREQUEST Request,
    IN BOOLEAN Requeue
    )
{
	
}
/* Suppress C4715: 'function' : not all control paths return a value.
       In the os model stubs must check a parameter for null before it is dereferenced.
       SLAM-2 considers all the traces where a null pointer is dereferenced as infeasible ones.
       This is acceptable as the OS Model will not be executed: */
#pragma warning(disable:4715)
NTSTATUS
sdv_WdfIoQueueRetrieveRequestByFileObject(
    IN WDFQUEUE Queue,
    IN WDFFILEOBJECT FileObject,
    OUT WDFREQUEST* OutRequest
    )
{
if(OutRequest!=NULL)
{
    LONG choice = SdvMakeChoice();
    switch(choice) 
    {
        case 0:
              *OutRequest=NULL;
              return (NTSTATUS) STATUS_NOT_SUPPORTED;
              break;
        default:
              *OutRequest = (WDFREQUEST)malloc(1);
              return (NTSTATUS) STATUS_SUCCESS;
              break;
    }
}
}
#pragma warning(default:4715)
/* Suppress C4715: 'function' : not all control paths return a value.
       In the os model stubs must check a parameter for null before it is dereferenced.
       SLAM-2 considers all the traces where a null pointer is dereferenced as infeasible ones.
       This is acceptable as the OS Model will not be executed: */
#pragma warning(disable:4715)
NTSTATUS
sdv_WdfIoQueueFindRequest(
    IN WDFQUEUE Queue,
    IN WDFREQUEST FoundRequest,
    IN WDFFILEOBJECT FileObject,
    OUT PWDF_REQUEST_PARAMETERS Parameters,
    OUT WDFREQUEST* OutRequest
    )
{
if(OutRequest!=NULL)
{
    LONG choice = SdvMakeChoice();
    switch(choice) 
    {
        case 0:
              *OutRequest=NULL;
              return (NTSTATUS) STATUS_NOT_SUPPORTED;
              break;
        default:
	            *OutRequest = (WDFREQUEST)malloc(1);
              return (NTSTATUS) STATUS_SUCCESS;
              break;
    }
}
}
#pragma warning(default:4715)
/* Suppress C4715: 'function' : not all control paths return a value.
       In the os model stubs must check a parameter for null before it is dereferenced.
       SLAM-2 considers all the traces where a null pointer is dereferenced as infeasible ones.
       This is acceptable as the OS Model will not be executed: */
#pragma warning(disable:4715)
NTSTATUS
sdv_WdfIoQueueRetrieveFoundRequest(
    IN WDFQUEUE Queue,
    IN WDFREQUEST FoundRequest,
    OUT WDFREQUEST* OutRequest
    )
{
if(OutRequest!=NULL)
{
    LONG choice = SdvMakeChoice();
    switch(choice) 
    {
        case 0:
              *OutRequest=NULL;
              return (NTSTATUS) STATUS_NOT_SUPPORTED;
              break;
        default:
              *OutRequest = (WDFREQUEST)malloc(1);
              return (NTSTATUS) STATUS_SUCCESS;
              break;
    }
}
}
#pragma warning(default:4715)
VOID
sdv_WdfIoQueueDrainSynchronously(
    IN WDFQUEUE Queue
    )
{
}
VOID
sdv_WdfIoQueueDrain(
    IN WDFQUEUE Queue,
    IN PFN_WDF_IO_QUEUE_STATE DrainComplete,
    IN WDFCONTEXT Context
    )
{
}
VOID
sdv_WdfIoQueuePurge(
    WDFQUEUE Queue,
    PFN_WDF_IO_QUEUE_STATE PurgeComplete,
    WDFCONTEXT Context
    )
{
}
NTSTATUS
sdv_WdfIoQueueReadyNotify(
    IN WDFQUEUE Queue,
    IN OPTIONAL PFN_WDF_IO_QUEUE_STATE QueueReady,
    IN OPTIONAL WDFCONTEXT Context
    )
{
	return sdv_SetReturnStatus();
}
/* Suppress C4715: 'function' : not all control paths return a value.
       In the os model stubs must check a parameter for null before it is dereferenced.
       SLAM-2 considers all the traces where a null pointer is dereferenced as infeasible ones.
       This is acceptable as the OS Model will not be executed: */
#pragma warning(disable:4715)
NTSTATUS
sdv_WdfIoQueueRetrieveNextRequest(
    IN WDFQUEUE Queue,
    OUT WDFREQUEST* OutRequest
    )
{
if(OutRequest!=NULL)
{
    LONG choice = SdvMakeChoice();
    switch(choice) 
    {
        case 0:
              *OutRequest=NULL;
              return (NTSTATUS) STATUS_NOT_SUPPORTED;
              break;
        default:
	            *OutRequest = (WDFREQUEST)malloc(1);
              return (NTSTATUS) STATUS_SUCCESS;
              break;
    }
}
}
#pragma warning(default:4715)
VOID
sdv_WdfIoQueueStart(
    IN WDFQUEUE Queue
    )
{
}
VOID
sdv_WdfIoQueueStop(
    IN WDFQUEUE Queue,
    IN OPTIONAL PFN_WDF_IO_QUEUE_STATE StopComplete,
    IN OPTIONAL WDFCONTEXT Context
    )
{
}
VOID
sdv_WdfIoQueueStopSynchronously(
    IN WDFQUEUE Queue
    )
{
}
WDF_IO_QUEUE_STATE
sdv_WdfIoQueueGetState(
    IN WDFQUEUE Queue,
    OUT OPTIONAL PULONG QueueRequests,
    OUT OPTIONAL PULONG DriverRequests
    )
{
	return (WDF_IO_QUEUE_STATE) SdvMakeChoice();
}
BOOLEAN
sdv_WDF_IO_QUEUE_IDLE(
IN WDF_IO_QUEUE_STATE Status)
{
      return((Status & sdv_WdfIoQueueNoRequests) &&    
        (Status & sdv_WdfIoQueueDriverNoRequests))? TRUE: FALSE;
}
VOID
sdv_WDF_PNPPOWER_EVENT_CALLBACKS_INIT(
    OUT PWDF_PNPPOWER_EVENT_CALLBACKS Callbacks
    )
{
}
VOID
sdv_WdfDeviceInitSetPnpPowerEventCallbacks(
    IN PWDFDEVICE_INIT DeviceInit,
    IN PWDF_PNPPOWER_EVENT_CALLBACKS PnpPowerEventCallbacks
    )
{
}
VOID
sdv_WdfDeviceInitSetRequestAttributes(
    IN PWDFDEVICE_INIT DeviceInit,
    IN PWDF_OBJECT_ATTRIBUTES RequestAttributes
    )
{
}
NTSTATUS
sdv_WdfDeviceInitAssignWdmIrpPreprocessCallback(
    IN PWDFDEVICE_INIT DeviceInit,
    IN PFN_WDFDEVICE_WDM_IRP_PREPROCESS EvtDevicePreprocessWdmIrp,
    UCHAR MajorFunction,
    IN OPTIONAL PUCHAR MinorFunctions,
    ULONG NumMinorFunctions
    )
{
	return sdv_SetReturnStatus();
}
VOID
sdv_WdfDeviceInitSetIoInCallerContextCallback(
    IN PWDFDEVICE_INIT DeviceInit,
    IN PFN_WDF_IO_IN_CALLER_CONTEXT EvtIoInCallerContext
    )
{
}
PUNICODE_STRING
sdv_WdfFileObjectGetFileName(
    IN WDFFILEOBJECT FileObject
    )
{
	return (PUNICODE_STRING) pnotused1;
}
WDFAPI VOID
sdv_WdfDeviceInitSetExclusive(
    IN PWDFDEVICE_INIT  DeviceInit,
    IN BOOLEAN  IsExclusive
    )
{
}
NTSTATUS
sdv_WdfDeviceInitAssignName(
	IN PWDFDEVICE_INIT  DeviceInit,
    IN PCUNICODE_STRING  DeviceName
     )
{
	return sdv_SetReturnStatus();
}
WDFAPI VOID
sdv_WdfDeviceInitFree(
    OUT PWDFDEVICE_INIT  DeviceInit
    )
{
	/*if(DeviceInit==hpdo_deviceinit)
    {
        hpdo_deviceinit = (PWDFDEVICE_INIT)malloc(1);
    }
    else if(DeviceInit==hpdo_deviceinit1)
    {
        hpdo_deviceinit1 = (PWDFDEVICE_INIT)malloc(1);
    }
    else if(DeviceInit==hpdo_deviceinit2_notused)
    {
        hpdo_deviceinit2_notused = (PWDFDEVICE_INIT)malloc(1);
    }
    if(DeviceInit==hcontrol_deviceinit)
    {
        hcontrol_deviceinit= (PWDFDEVICE_INIT)malloc(1);
    }
    else if(DeviceInit==hcontrol_deviceinit1)
    {
        hcontrol_deviceinit1= (PWDFDEVICE_INIT)malloc(1);
    }
    else if(DeviceInit==hcontrol_deviceinit2_notused)
    {
        hcontrol_deviceinit2_notused= (PWDFDEVICE_INIT)malloc(1);
    }*/
}
WDFAPI VOID
sdv_WdfDeviceInitSetPowerPolicyOwnership(
    IN PWDFDEVICE_INIT  DeviceInit,
    IN BOOLEAN  IsPowerPolicyOwner
    )
{
}
VOID
FORCEINLINE
sdv_WdfDeviceInitSetFileObjectConfig(
    IN PWDFDEVICE_INIT DeviceInit,
    IN PWDF_FILEOBJECT_CONFIG  FileObjectConfig,
    IN OPTIONAL PWDF_OBJECT_ATTRIBUTES FileObjectAttributes
    )
{
}
VOID
sdv_WdfDeviceInitSetPowerPolicyEventCallbacks(
    IN PWDFDEVICE_INIT DeviceInit,
    IN PWDF_POWER_POLICY_EVENT_CALLBACKS PowerPolicyEventCallbacks
    )
{
}
NTSTATUS
sdv_WdfDeviceInitRegisterPnpStateChangeCallback(
    PWDFDEVICE_INIT DeviceInit,
    WDF_DEVICE_PNP_STATE PnpState,
    PFN_WDF_DEVICE_PNP_STATE_CHANGE_NOTIFICATION  EvtDevicePnpStateChange,
    ULONG CallbackTypes
    )
{
	return sdv_SetReturnStatus();
}
NTSTATUS
sdv_WdfDeviceInitRegisterPowerStateChangeCallback(
    PWDFDEVICE_INIT DeviceInit,
    WDF_DEVICE_POWER_STATE PowerState,
    PFN_WDF_DEVICE_POWER_STATE_CHANGE_NOTIFICATION  EvtDevicePowerStateChange,
    ULONG CallbackTypes
    )
{
	return sdv_SetReturnStatus();
}
NTSTATUS
sdv_WdfDeviceInitRegisterPowerPolicyStateChangeCallback(
    PWDFDEVICE_INIT DeviceInit,
    WDF_DEVICE_POWER_POLICY_STATE PowerPolicyState,
	PFN_WDF_DEVICE_POWER_POLICY_STATE_CHANGE_NOTIFICATION  EvtDevicePowerPolicyStateChange,
    ULONG CallbackTypes
    )
{
	return sdv_SetReturnStatus();
}
VOID
sdv_WdfDeviceInitSetIoType(
    IN PWDFDEVICE_INIT DeviceInit,
    IN WDF_DEVICE_IO_TYPE IoType
    )
{
}
VOID
sdv_WdfDeviceInitSetPowerNotPageable(
    IN PWDFDEVICE_INIT DeviceInit
    )
{
}
VOID
sdv_WdfDeviceInitSetPowerPageable(
    IN PWDFDEVICE_INIT DeviceInit
    )
{
}
VOID
sdv_WdfDeviceInitSetPowerInrush(
    IN PWDFDEVICE_INIT DeviceInit
    )
{
}
VOID
sdv_WdfDeviceInitSetDeviceType(
    IN PWDFDEVICE_INIT DeviceInit,
    IN DEVICE_TYPE DeviceType
    )
{
}
NTSTATUS
sdv_WdfDeviceInitAssignSDDLString(
    IN PWDFDEVICE_INIT DeviceInit,
    IN PCUNICODE_STRING SDDLString
    )
{
	return sdv_SetReturnStatus();
}
VOID
sdv_WdfDeviceInitSetDeviceClass(
    IN PWDFDEVICE_INIT DeviceInit,
    IN CONST GUID* DeviceClassGuid
    )
{
}
VOID
sdv_WdfDeviceInitSetCharacteristics(
    IN PWDFDEVICE_INIT DeviceInit,
    IN ULONG DeviceCharacteristics,
    IN BOOLEAN OrInValues
    )
{
}
PDEVICE_OBJECT
sdv_WdfFdoInitWdmGetPhysicalDevice(
    IN PWDFDEVICE_INIT  DeviceInit
	)
{
	return (p_devobj);
}
VOID
sdv_WDF_DEVICE_POWER_POLICY_IDLE_SETTINGS_INIT(
    PWDF_DEVICE_POWER_POLICY_IDLE_SETTINGS Settings,
    WDF_POWER_POLICY_S0_IDLE_CAPABILITIES IdleCaps
    )
{
}
VOID
sdv_WdfFdoInitSetFilter(
    IN PWDFDEVICE_INIT  DeviceInit
	)
{ 
}
/* Suppress C4715: 'function' : not all control paths return a value.
       In the os model stubs must check a parameter for null before it is dereferenced.
       SLAM-2 considers all the traces where a null pointer is dereferenced as infeasible ones.
       This is acceptable as the OS Model will not be executed: */
#pragma warning(disable:4715)
NTSTATUS
sdv_WdfFdoInitOpenRegistryKey(
    IN PWDFDEVICE_INIT DeviceInit,
    IN ULONG DeviceInstanceKeyType,
    IN ACCESS_MASK DesiredAccess,
    IN OPTIONAL PWDF_OBJECT_ATTRIBUTES KeyAttributes,
    OUT WDFKEY* Key
    )
{
if(Key!=NULL)
{
    LONG choice = SdvMakeChoice();
    switch(choice) 
    {
        case 0:
              *Key=NULL;
              return (NTSTATUS) STATUS_NOT_SUPPORTED;
              break;
        default:
              *Key = (WDFKEY)malloc(1);
              return (NTSTATUS) STATUS_SUCCESS;
              break;
    }
}
}
/* Suppress C4715: 'function' : not all control paths return a value.
       In the os model stubs must check a parameter for null before it is dereferenced.
       SLAM-2 considers all the traces where a null pointer is dereferenced as infeasible ones.
       This is acceptable as the OS Model will not be executed: */
#pragma warning(default:4715)
NTSTATUS
sdv_WdfFdoInitQueryProperty(
    IN PWDFDEVICE_INIT DeviceInit,
    IN DEVICE_REGISTRY_PROPERTY DeviceProperty,
    IN ULONG BufferLength,
    OUT PVOID PropertyBuffer,
    OUT PULONG ResultLength
    )
{
    LONG choice = SdvMakeChoice();
    switch(choice) 
    {
        case 0:
              return (NTSTATUS) STATUS_NOT_SUPPORTED;
              break;
        default:
              return (NTSTATUS) STATUS_SUCCESS;
              break;
    }
}
/* Suppress C4715: 'function' : not all control paths return a value.
       In the os model stubs must check a parameter for null before it is dereferenced.
       SLAM-2 considers all the traces where a null pointer is dereferenced as infeasible ones.
       This is acceptable as the OS Model will not be executed: */
#pragma warning(disable:4715)
NTSTATUS
sdv_WdfFdoInitAllocAndQueryProperty(
    IN PWDFDEVICE_INIT DeviceInit,
    IN DEVICE_REGISTRY_PROPERTY DeviceProperty,
    IN POOL_TYPE PoolType,
    IN OPTIONAL PWDF_OBJECT_ATTRIBUTES PropertyMemoryAttributes,
    OUT WDFMEMORY* PropertyMemory
    )
{
if(PropertyMemory!=NULL)
{
    LONG choice = SdvMakeChoice();
    switch(choice) 
    {
        case 0:
              *PropertyMemory=NULL;
              return (NTSTATUS) STATUS_NOT_SUPPORTED;
              break;
        default:
              *PropertyMemory = (WDFMEMORY)malloc(1);
              return (NTSTATUS) STATUS_SUCCESS;
              break;
    }
}
}
#pragma warning(default:4715)
VOID
sdv_WdfFdoInitSetEventCallbacks(
    IN PWDFDEVICE_INIT DeviceInit,
    IN PWDF_FDO_EVENT_CALLBACKS FdoEventCallbacks
    )
{
}
VOID
sdv_WdfFdoInitSetDefaultChildListConfig(
    IN OUT PWDFDEVICE_INIT DeviceInit,
    IN PWDF_CHILD_LIST_CONFIG Config,
    IN OPTIONAL PWDF_OBJECT_ATTRIBUTES DefaultChildListAttributes
    )
{
}
VOID
sdv_WdfPdoInitSetEventCallbacks(
    IN PWDFDEVICE_INIT DeviceInit,
    IN PWDF_PDO_EVENT_CALLBACKS DispatchTable
    )
{
}
NTSTATUS
sdv_WdfPdoInitAssignDeviceID(
    IN PWDFDEVICE_INIT DeviceInit,
    IN PCUNICODE_STRING DeviceID
    )
{
	return sdv_SetReturnStatus();
}
NTSTATUS
sdv_WdfPdoInitAssignInstanceID(
    IN PWDFDEVICE_INIT DeviceInit,
    IN PCUNICODE_STRING InstanceID
    )
{
	return sdv_SetReturnStatus();
}
NTSTATUS
sdv_WdfPdoInitAddHardwareID(
    IN PWDFDEVICE_INIT DeviceInit,
    IN PCUNICODE_STRING HardwareID
    )
{
	return sdv_SetReturnStatus();
}
NTSTATUS
sdv_WdfPdoInitAddCompatibleID(
    IN PWDFDEVICE_INIT DeviceInit,
    IN PCUNICODE_STRING CompatibleID
    )
{
	return sdv_SetReturnStatus();
}
NTSTATUS
sdv_WdfPdoInitAddDeviceText(
    IN PWDFDEVICE_INIT DeviceInit,
    IN PCUNICODE_STRING DeviceDescription,
    IN PCUNICODE_STRING DeviceLocation,
    IN LCID LocaleId
    )
{
	return sdv_SetReturnStatus();
}
VOID
sdv_WdfPdoInitSetDefaultLocale(
    IN PWDFDEVICE_INIT DeviceInit,
    IN LCID LocaleId
    )
{
}
NTSTATUS
sdv_WdfPdoInitAssignRawDevice(
    IN PWDFDEVICE_INIT DeviceInit,
    IN CONST GUID* DeviceClassGuid
    )
{
	return sdv_SetReturnStatus();
}
NTSTATUS
sdv_WdfDeviceAssignS0IdleSettings(
    WDFDEVICE Device,
    PWDF_DEVICE_POWER_POLICY_IDLE_SETTINGS Settings
    )
{
	return sdv_SetReturnStatus();
}
VOID
sdv_WDF_DEVICE_POWER_POLICY_WAKE_SETTINGS_INIT(
    PWDF_DEVICE_POWER_POLICY_WAKE_SETTINGS Settings
    )
{
}
NTSTATUS
sdv_WdfDeviceAssignSxWakeSettings(
    WDFDEVICE Device,
    PWDF_DEVICE_POWER_POLICY_WAKE_SETTINGS Settings
    )
{
	return sdv_SetReturnStatus();
}
VOID
sdv_WDF_POWER_POLICY_EVENT_CALLBACKS_INIT(
    PWDF_POWER_POLICY_EVENT_CALLBACKS Callbacks
    )
{
}
/* Suppress C4715: 'function' : not all control paths return a value.
       In the os model stubs must check a parameter for null before it is dereferenced.
       SLAM-2 considers all the traces where a null pointer is dereferenced as infeasible ones.
       This is acceptable as the OS Model will not be executed: */
#pragma warning(disable:4715)
NTSTATUS
sdv_WdfCollectionCreate(
	IN OPTIONAL PWDF_OBJECT_ATTRIBUTES CollectionAttributes,
    OUT WDFCOLLECTION *Collection
	)
{
if(Collection!=NULL)
{
    LONG choice = SdvMakeChoice();
    switch(choice) 
    {
        case 0:
              *Collection=NULL;
              return (NTSTATUS) STATUS_NOT_SUPPORTED;
              break;
        default:
              *Collection = (WDFCOLLECTION)malloc(1);
              return (NTSTATUS) STATUS_SUCCESS;
              break;
    }
}
}
#pragma warning(default:4715)
WDFOBJECT
sdv_WdfCollectionGetFirstItem(
    WDFCOLLECTION Collection
    )
{
	return ((WDFOBJECT) &notused_4wdfobject1);
}
WDFOBJECT
sdv_WdfCollectionGetItem(
    IN WDFCOLLECTION Collection,
    IN ULONG Index
    )
{
	return ((WDFOBJECT) &notused_4wdfobject1);
}
WDFAPI NTSTATUS
sdv_WdfCollectionAdd(
    IN WDFCOLLECTION Collection,
    IN WDFOBJECT Object
    )
{ 
	int x = SdvMakeChoice();
	switch (x) { 
        case 0: 
	           hdevcollection_count += 1;
			   return STATUS_SUCCESS;
			   break;
        default: 
			     hdevcollection_count = 0;
			     return STATUS_UNSUCCESSFUL;
			     break;
	}
}
VOID
sdv_WdfCollectionRemoveItem(
    IN WDFCOLLECTION Collection,
    IN WDFOBJECT Item
    )
{
	int x = SdvMakeChoice();
	switch (x) { 
        case 0: 
	           hdevcollection_count -= 1;
	           break;
        default: 
			     hdevcollection_count = 0;
			     break;
	}
}
WDFAPI ULONG
sdv_WdfCollectionGetCount(
    IN WDFCOLLECTION Collection
    )
{
	return (ULONG) hdevcollection_count;
}
VOID
sdv_WdfObjectDelete(
    IN WDFOBJECT Object
    )
{
}
NTSTATUS
sdv_WdfFdoQueryForInterface(
    IN WDFDEVICE Device,
    IN LPCGUID InterfaceType,
    IN OUT PINTERFACE Interface,
    IN USHORT Size,
    IN USHORT Version,
    IN OUT PVOID InterfaceSpecificData
    )
{
	LONG choice = SdvMakeChoice();
  switch(choice) 
  {
      case 0:
            return (NTSTATUS) STATUS_NOT_SUPPORTED;
            break;
      default:
            return (NTSTATUS) STATUS_SUCCESS;
            break;
  }
	
}
VOID
sdv_WDF_DEVICE_STATE_INIT(
    OUT PWDF_DEVICE_STATE PnpDeviceState
    )
{
}
VOID
sdv_WdfDeviceSetDeviceState(
    IN WDFDEVICE Device,
    IN PWDF_DEVICE_STATE DeviceState
    )
{
}
NTSTATUS
sdv_WdfDeviceAssignMofResourceName(
    IN WDFDEVICE Device,
    IN PCUNICODE_STRING MofResourceName
    )
{
	return sdv_SetReturnStatus();
}
PDEVICE_OBJECT
sdv_WdfDeviceWdmGetDeviceObject(
    IN WDFDEVICE Device
    )
{
	return ((PDEVICE_OBJECT) pnotused);
}
PDEVICE_OBJECT
sdv_WdfDeviceWdmGetPhysicalDevice(
    IN WDFDEVICE Device
    )
{
	return ((PDEVICE_OBJECT) pnotused);
}
#ifndef init_free
WDFAPI 
PWDFDEVICE_INIT
sdv_WdfControlDeviceInitAllocate(
    IN WDFDRIVER  Driver,
    IN CONST UNICODE_STRING*  SDDLString
    )
{
	int x = SdvMakeChoice();
  switch (x) 
  { 
    case 0: hcontrol_deviceinit = (PWDFDEVICE_INIT) &notused_4hcontrol_deviceinit;
	           return (hcontrol_deviceinit); 
	           break;		    
		default: 
			    hcontrol_deviceinit = (PWDFDEVICE_INIT) &notused_4hcontrol_deviceinit;
          return (hcontrol_deviceinit);
          break;
	}

}
#endif

#ifdef init_free
WDFAPI 
PWDFDEVICE_INIT
sdv_WdfControlDeviceInitAllocate(
    IN WDFDRIVER  Driver,
    IN CONST UNICODE_STRING*  SDDLString
    )
{

	if(control==0)
	{
		control=1;
		return (hcontrol_deviceinit);
	}
	else if(control==1)
	{
		control++;
		return (hcontrol_deviceinit1);
	}
	else 
	{
		control++;
		return (hcontrol_deviceinit2_notused);
	}
}
#endif

VOID
sdv_WdfControlDeviceInitSetShutdownNotification(
    IN PWDFDEVICE_INIT DeviceInit,
    IN PFN_WDF_DEVICE_SHUTDOWN_NOTIFICATION Notification,
    IN UCHAR Flags
    )
{
}
#ifdef init_free
PWDFDEVICE_INIT
sdv_WdfPdoInitAllocate(
IN WDFDEVICE ParentDevice
)
{
	if(pdo==0)
	{
		pdo=1;
		return (hpdo_deviceinit);
		
	}
	else if(pdo==1)
	{
		pdo++;
		return (hpdo_deviceinit1);
	}
	else 
	{
		pdo++;
		return (hpdo_deviceinit2_notused);
	}
	
}

#endif

#ifndef init_free
PWDFDEVICE_INIT
sdv_WdfPdoInitAllocate(
    IN WDFDEVICE ParentDevice
    )
{
	int x = SdvMakeChoice();
    switch (x) { 
       case 0: hpdo_deviceinit = (PWDFDEVICE_INIT) &notused_4hpdo_deviceinit;
	           return (hpdo_deviceinit); 
	           break;		    
		default: 
			    hpdo_deviceinit = (PWDFDEVICE_INIT) &notused_4hpdo_deviceinit;
                return (hpdo_deviceinit);
                break;
	}
}
#endif
VOID
sdv_WdfControlFinishInitializing(
    WDFDEVICE Device
    )
{
}
VOID
sdv_WDF_INTERRUPT_CONFIG_INIT(
    OUT PWDF_INTERRUPT_CONFIG Configuration,
    IN  PFN_WDF_INTERRUPT_ISR EvtInterruptIsr,
    IN  PFN_WDF_INTERRUPT_DPC EvtInterruptDpc
    )
{
}
/* Suppress C4715: 'function' : not all control paths return a value.
       In the os model stubs must check a parameter for null before it is dereferenced.
       SLAM-2 considers all the traces where a null pointer is dereferenced as infeasible ones.
       This is acceptable as the OS Model will not be executed: */
#pragma warning(disable:4715)
NTSTATUS
sdv_WdfInterruptCreate(
    IN WDFDEVICE Device,
    IN PWDF_INTERRUPT_CONFIG Configuration,
    IN OPTIONAL PWDF_OBJECT_ATTRIBUTES Attributes,
    OUT WDFINTERRUPT* Interrupt
    )
{
if(Interrupt!=NULL)
{
	int x = SdvMakeChoice();
    switch (x) { 
        case 0: hinterrupt = (WDFINTERRUPT) &notused_4hinterrupt;
			    *Interrupt = hinterrupt;				
			    return STATUS_SUCCESS;
			    break;
		default: 
			     hinterrupt = (WDFINTERRUPT) &notused_4hinterrupt;
			     *Interrupt = hinterrupt;
			     return STATUS_UNSUCCESSFUL;
			     break;
	}
}
}
#pragma warning(default:4715)
BOOLEAN
sdv_WdfInterruptSynchronize(
    IN WDFINTERRUPT Interrupt,
    IN PFN_WDF_INTERRUPT_SYNCHRONIZE Callback,
    IN WDFCONTEXT Context
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
/* Suppress C4715: 'function' : not all control paths return a value.
       In the os model stubs must check a parameter for null before it is dereferenced.
       SLAM-2 considers all the traces where a null pointer is dereferenced as infeasible ones.
       This is acceptable as the OS Model will not be executed: */
#pragma warning(disable:4715)
NTSTATUS
sdv_WdfSpinLockCreate(
    IN OPTIONAL PWDF_OBJECT_ATTRIBUTES SpinLockAttributes,
    OUT WDFSPINLOCK* SpinLock
    )
{
  if(SpinLock!=NULL)
  {
    LONG choice = SdvMakeChoice();
    switch(choice) 
    {
          case 0:
              *SpinLock=NULL;
              return (NTSTATUS) STATUS_NOT_SUPPORTED;
              break;
          default:
              * SpinLock = (WDFSPINLOCK)malloc(1);
              return (NTSTATUS) STATUS_SUCCESS;
              break;
    }
  }
}
#pragma warning(default:4715)
VOID
sdv_WdfSpinLockAcquire(
    IN WDFSPINLOCK SpinLock
    )
{
	stored_irql = sdv_irql_current;
	SDV_IRQL_PUSH(DISPATCH_LEVEL);
}
VOID
sdv_WdfSpinLockRelease(
    IN WDFSPINLOCK SpinLock
    )
{
	SDV_IRQL_POP();
}
VOID
sdv_WdfObjectAcquireLock(
    IN WDFOBJECT Object
    )
{
}
VOID
sdv_WdfObjectReleaseLock(
    IN WDFOBJECT Object
    )
{
}
VOID
sdv_WdfInterruptAcquireLock(
	IN WDFINTERRUPT Interrupt
	)
{
	stored_irql = sdv_irql_current;
	SDV_IRQL_PUSH(DISPATCH_LEVEL + 1);
}

VOID
sdv_WdfInterruptReleaseLock(
	IN WDFINTERRUPT Interrupt
	)
{
	SDV_IRQL_POP();
}

VOID
sdv_WPP_INIT_TRACING(
	 IN PDRIVER_OBJECT DriverObject,
     IN PUNICODE_STRING RegistryPath
           )
{
}
VOID
sdv_WdfStringGetUnicodeString(
    IN WDFSTRING String,
    PUNICODE_STRING UnicodeString
    )
{
}
WDF_DEVICE_POWER_STATE
sdv_WdfDeviceGetDevicePowerState(
    __in WDFDEVICE Device
    )
{
	return sdv_SetDevicePowerPolicyState();
}
WDF_DEVICE_POWER_POLICY_STATE
sdv_WdfDeviceGetDevicePowerPolicyState(
    __in WDFDEVICE Device
    )
{
	return sdv_SetDevicePowerPolicyState();
}
VOID
sdv_WdfDeviceSetPowerCapabilities(
    __in WDFDEVICE Device,
    __in PWDF_DEVICE_POWER_CAPABILITIES PowerCapabilities
    )
{
}
WDFDEVICE
sdv_WdfWdmDeviceGetWdfDeviceHandle(
    IN PDEVICE_OBJECT DeviceObject
    )
{
	return hdevice;
}
void 
sdv_save_request(void *r)
{

}

NTSTATUS  
sdv_WdfWaitLockAcquire(  
__in  WDFWAITLOCK  Lock,
__in_opt  PLONGLONG  Timeout)
{
    LONG choice = SdvMakeChoice();
    if(Timeout!=NULL)
    {
      switch(choice) 
      {
            case 0:
                return (NTSTATUS) STATUS_TIMEOUT;
                break;
            default:
                return (NTSTATUS) STATUS_SUCCESS;
                break;
      }
    }
    else
    {
        return (NTSTATUS) STATUS_SUCCESS;
    }
}
/* Suppress C4715: 'function' : not all control paths return a value.
       In the os model stubs must check a parameter for null before it is dereferenced.
       SLAM-2 considers all the traces where a null pointer is dereferenced as infeasible ones.
       This is acceptable as the OS Model will not be executed: */
#pragma warning(disable:4715)
void 
sdv_retrieve_request(void **r)
{

	(*r)=(void *)hrequest;
}
#pragma warning(default:4715)
NTSTATUS  
sdv_WdfDeviceWdmDispatchPreprocessedIrp(WDFDEVICE  Device,PIRP  Irp)
{
	return sdv_SetReturnStatus(); 
}

/* wdf_sdv_usb_stubs.c begin */
#ifndef SDV_OMIT_USB_SUPPORT
NTSTATUS
sdv_WdfUsbTargetDeviceRetrieveInformation(
    WDFUSBDEVICE UsbDevice,
    PWDF_USB_DEVICE_INFORMATION Information
    )
{
	return sdv_SetReturnStatus();
}

NTSTATUS
sdv_WdfUsbTargetDeviceCreate (
    WDFDEVICE Device,
    PWDF_OBJECT_ATTRIBUTES Attributes,
    WDFUSBDEVICE* UsbDevice
    )
{
	int x = SdvMakeChoice();
    switch (x) { 
        case 0: husb_device = (WDFUSBDEVICE) &notused_4husb_device;
			    *UsbDevice = husb_device;				
			    return STATUS_SUCCESS;
		default: husb_device = NULL;
			    *UsbDevice = NULL;
			     return STATUS_NOT_SUPPORTED;
	}
}

NTSTATUS  
sdv_WdfIoTargetFormatRequestForRead(    
								WDFIOTARGET  IoTarget, 
								WDFREQUEST  Request,  
								WDFMEMORY  OutputBuffer,  
								PWDFMEMORY_OFFSET  OutputBufferOffset,  
								PLONGLONG  DeviceOffset    )
{
	return sdv_SetReturnStatus();
}
NTSTATUS  
sdv_WdfIoTargetFormatRequestForWrite(    
								WDFIOTARGET  IoTarget, 
								WDFREQUEST  Request,  
								WDFMEMORY  InputBuffer,  
								PWDFMEMORY_OFFSET  InputBufferOffset,  
								PLONGLONG  DeviceOffset    )
{
	return sdv_SetReturnStatus();
}
NTSTATUS  
sdv_WdfIoTargetFormatRequestForIoctl(  
								 WDFIOTARGET  IoTarget,    
								 WDFREQUEST  Request,   
								 ULONG  IoctlCode,   
								 WDFMEMORY  InputBuffer,  
								 PWDFMEMORY_OFFSET  InputBufferOffset,   
								 WDFMEMORY  OutputBuffer,  
								 PWDFMEMORY_OFFSET  OutputBufferOffset    )
{
	return sdv_SetReturnStatus();
}
NTSTATUS  
sdv_WdfIoTargetFormatRequestForInternalIoctlOthers(  
	WDFIOTARGET  IoTarget,   
	WDFREQUEST  Request,   
	ULONG  IoctlCode,   
	WDFMEMORY  OtherArg1,  
	PWDFMEMORY_OFFSET  OtherArg1Offset,
	WDFMEMORY  OtherArg2,  
	PWDFMEMORY_OFFSET  OtherArg2Offset,
	WDFMEMORY  OtherArg4,   
	PWDFMEMORY_OFFSET  OtherArg4Offset    )
{
	return sdv_SetReturnStatus();
}
NTSTATUS 
sdv_WdfIoTargetFormatRequestForInternalIoctl( 
	WDFIOTARGET  IoTarget,   
	WDFREQUEST  Request,   
	ULONG  IoctlCode,   
	WDFMEMORY  InputBuffer, 
	PWDFMEMORY_OFFSET  InputBufferOffset,  
	WDFMEMORY  OutputBuffer,  
	PWDFMEMORY_OFFSET  OutputBufferOffset    )
{
	return sdv_SetReturnStatus();
}




VOID  
sdv_WDF_REQUEST_SEND_OPTIONS_INIT(   
	PWDF_REQUEST_SEND_OPTIONS  Options,   
	IN ULONG  Flags    )
{
	Options->Size=sizeof(WDF_REQUEST_SEND_OPTIONS);
	Options->Timeout=0;
	Options->Flags=Flags;
}

WDFIOTARGET  
sdv_WdfDeviceGetIoTarget(    
					 IN WDFDEVICE  Device    
					 )
{
	return (WDFIOTARGET) Device;
}

NTSTATUS 
sdv_WdfIoTargetStart(    
				 WDFIOTARGET  IoTarget 
				 )
{
	int x = SdvMakeChoice();
    switch(x)
    {
      case 0:
         return STATUS_SUCCESS;
         break;
      default:
         return STATUS_INVALID_DEVICE_STATE ;
		 break;
     }
}

WDFIOTARGET
sdv_WdfUsbTargetPipeGetIoTarget(
    IN WDFUSBPIPE Pipe)
{
    return (WDFIOTARGET)Pipe;
}



VOID  sdv_WDF_REQUEST_SEND_OPTIONS_SET_TIMEOUT(   
	PWDF_REQUEST_SEND_OPTIONS  Options,   
	LONGLONG  Timeout    )
{
	Options->Timeout=Timeout; 

}





VOID  sdv_WdfUsbTargetDeviceGetDeviceDescriptor(   
	 WDFUSBDEVICE  UsbDevice,  
	 PUSB_DEVICE_DESCRIPTOR  UsbDeviceDescriptor 
	)
{
}
NTSTATUS
sdv_WdfUsbTargetDeviceRetrieveConfigDescriptor(
    WDFUSBDEVICE UsbDevice,
    PVOID ConfigDescriptor,
    PUSHORT ConfigDescriptorLength
    )
{
	return sdv_SetReturnStatus();
}
NTSTATUS  
sdv_WdfUsbTargetDeviceQueryString(    
							  IN WDFUSBDEVICE  UsbDevice,    
							  IN OPTIONAL WDFREQUEST  Request,   
							  IN OPTIONAL PWDF_REQUEST_SEND_OPTIONS  RequestOptions,  
							  OUT OPTIONAL PUSHORT  String, 
							  IN OUT PUSHORT  NumCharacters, 
							  IN UCHAR  StringIndex,    
							  IN OPTIONAL USHORT  LangID    
							  )
{
	return sdv_SetReturnStatus(); 
}
NTSTATUS  
sdv_WdfUsbTargetDeviceAllocAndQueryString(   
									  IN WDFUSBDEVICE  UsbDevice,  
									  IN OPTIONAL PWDF_OBJECT_ATTRIBUTES  StringMemoryAttributes,  
									  OUT WDFMEMORY*  StringMemory,   
									  OUT OPTIONAL PUSHORT  NumCharacters,   
									  IN UCHAR  StringIndex,   
									  IN OPTIONAL USHORT  LangID  
									  )
{
	return sdv_SetReturnStatus(); 
}

NTSTATUS 
sdv_WdfUsbTargetDeviceFormatRequestForString(  
	IN WDFUSBDEVICE  UsbDevice,   
	IN WDFREQUEST  Request,  
	IN WDFMEMORY  Memory,  
	IN OPTIONAL PWDFMEMORY_OFFSET  Offset,  
	IN UCHAR  StringIndex,   
	IN OPTIONAL USHORT  LangID   
	)
{
	return sdv_SetReturnStatus(); 
}

USBD_CONFIGURATION_HANDLE 
sdv_WdfUsbTargetDeviceWdmGetConfigurationHandle(  
	IN WDFUSBDEVICE  UsbDevice   
	)
{int x = SdvMakeChoice();
    switch(x)
    {
      case 0:
         return NULL;
         break;
      default:
         return (USBD_CONFIGURATION_HANDLE)x;
     }

}


NTSTATUS  
sdv_WdfUsbTargetDeviceSendControlTransferSynchronously(  
	IN WDFUSBDEVICE  UsbDevice,   
	IN OPTIONAL WDFREQUEST  Request,
	IN OPTIONAL PWDF_REQUEST_SEND_OPTIONS  RequestOptions, 
	IN PWDF_USB_CONTROL_SETUP_PACKET  SetupPacket, 
	IN OPTIONAL PWDF_MEMORY_DESCRIPTOR  MemoryDescriptor,    
	OUT OPTIONAL PULONG  BytesTransferred    
	)
{
	*BytesTransferred=SdvMakeChoice();
	return sdv_SetReturnStatus(); 
}


NTSTATUS 
sdv_WdfUsbTargetDeviceFormatRequestForControlTransfer(  
	IN WDFUSBDEVICE  UsbDevice,  
	IN WDFREQUEST  Request,   
	IN PWDF_USB_CONTROL_SETUP_PACKET  SetupPacket, 
	IN OPTIONAL WDFMEMORY  TransferMemory,   
	IN OPTIONAL PWDFMEMORY_OFFSET  TransferOffset 
	)
{
	return sdv_SetReturnStatus(); 
}


NTSTATUS  
sdv_WdfUsbTargetDeviceCyclePortSynchronously( 
	IN WDFUSBDEVICE  UsbDevice  
	)
{
	return sdv_SetReturnStatus(); 
}

NTSTATUS  
sdv_WdfUsbTargetDeviceFormatRequestForCyclePort(   
	IN WDFUSBDEVICE  UsbDevice,  
	IN WDFREQUEST  Request  
	)
{
	return sdv_SetReturnStatus(); 
}

NTSTATUS  
sdv_WdfUsbTargetDeviceSendUrbSynchronously(  
	IN WDFUSBDEVICE  UsbDevice, 
	IN OPTIONAL WDFREQUEST  Request,   
	IN OPTIONAL PWDF_REQUEST_SEND_OPTIONS  RequestOptions, 
	IN PURB  Urb    
	)
{
	return sdv_SetReturnStatus(); 
}

NTSTATUS  
sdv_WdfUsbTargetDeviceFormatRequestForUrb(  
									  IN WDFUSBDEVICE  UsbDevice,  
									  IN WDFREQUEST  Request,  
									  IN WDFMEMORY  UrbMemory,   
									  IN OPTIONAL PWDFMEMORY_OFFSET  UrbMemoryOffset   
									  )
{
	return sdv_SetReturnStatus(); 
}

BOOLEAN  
sdv_WdfUsbTargetPipeIsInEndpoint( 
							 IN WDFUSBPIPE  Pipe   
							 )
{	int x = SdvMakeChoice();
    switch(x)
    {
      case 0:
         return FALSE;
         break;
      default:
         return TRUE;
     }

}

BOOLEAN  
sdv_WdfUsbTargetPipeIsOutEndpoint( 
							 IN WDFUSBPIPE  Pipe   
							 )
{	int x = SdvMakeChoice();
    switch(x)
    {
      case 0:
         return FALSE;
         break;
      default:
         return TRUE;
     }

}

WDF_USB_PIPE_TYPE  
sdv_WdfUsbTargetPipeGetType(    IN WDFUSBPIPE  Pipe    )
{	int x = SdvMakeChoice();
    switch(x)
    {
      case 0:
         return (WDF_USB_PIPE_TYPE)WdfUsbPipeTypeInvalid ;
         break;
	case 1:
         return (WDF_USB_PIPE_TYPE)WdfUsbPipeTypeControl ;
         break;
	case 2:
         return (WDF_USB_PIPE_TYPE)WdfUsbPipeTypeIsochronous ;
         break;
	case 3:
         return (WDF_USB_PIPE_TYPE)WdfUsbPipeTypeBulk ;
         break;
    default:
         return (WDF_USB_PIPE_TYPE)WdfUsbPipeTypeInterrupt;
     }

}

NTSTATUS 
sdv_WdfUsbTargetPipeWriteSynchronously( 
								   IN WDFUSBPIPE  Pipe, 
								   IN OPTIONAL WDFREQUEST  Request, 
								   IN OPTIONAL PWDF_REQUEST_SEND_OPTIONS  RequestOptions,  
								   IN OPTIONAL PWDF_MEMORY_DESCRIPTOR  MemoryDescriptor, 
								   OUT OPTIONAL PULONG  BytesWritten   
								   )
{
		int choice = SdvMakeChoice();
    switch(choice) 
    {
        case 0:
              BytesWritten=NULL;
              return (NTSTATUS) STATUS_NOT_SUPPORTED;
              break;
        default:
              BytesWritten = (PULONG)malloc(1);
              return (NTSTATUS) STATUS_SUCCESS;
              break;
    }
}


NTSTATUS  
sdv_WdfUsbTargetPipeFormatRequestForWrite(    
									  IN WDFUSBPIPE  Pipe,  
									  IN WDFREQUEST  Request, 
									  IN OPTIONAL WDFMEMORY  WriteMemory,  
									  IN OPTIONAL PWDFMEMORY_OFFSET  WriteOffset 
									  )
{
	return sdv_SetReturnStatus(); 
}

NTSTATUS  
sdv_WdfUsbTargetPipeReadSynchronously(  
	IN WDFUSBPIPE  Pipe,  
	IN OPTIONAL WDFREQUEST  Request,  
	IN OPTIONAL PWDF_REQUEST_SEND_OPTIONS  RequestOptions, 
	IN OPTIONAL PWDF_MEMORY_DESCRIPTOR  MemoryDescriptor,  
	OUT OPTIONAL PULONG  BytesRead    )
{
	int choice = SdvMakeChoice();
    switch(choice) 
    {
        case 0:
              BytesRead=NULL;
              return (NTSTATUS) STATUS_NOT_SUPPORTED;
              break;
        default:
              BytesRead = (PULONG)malloc(1);
              return (NTSTATUS) STATUS_SUCCESS;
              break;
    }
}

NTSTATUS  
sdv_WdfUsbTargetPipeFormatRequestForRead(  
									 IN WDFUSBPIPE  Pipe,    
									 IN WDFREQUEST  Request,  
									 IN OPTIONAL WDFMEMORY  ReadMemory,  
									 IN OPTIONAL PWDFMEMORY_OFFSET  ReadOffset 
									 )
{
	return sdv_SetReturnStatus(); 
}

NTSTATUS 
sdv_WdfUsbTargetPipeConfigContinuousReader(  
									   IN WDFUSBPIPE  Pipe,
									   IN PWDF_USB_CONTINUOUS_READER_CONFIG  Config 
									   )
{
	/*int x = SdvMakeChoice();
    switch(x)
    {
      case 0:
         return (NTSTATUS)STATUS_INFO_LENGTH_MISMATCH ;
         break;
	case 1:
         return (NTSTATUS)STATUS_INVALID_PARAMETER ;
         break;
	case 2:
         return (NTSTATUS)STATUS_INSUFFICIENT_RESOURCES ;
         break;
	case 3:
         return (NTSTATUS)STATUS_INVALID_DEVICE_REQUEST ;
         break;
	case 4:
         return (NTSTATUS)STATUS_INTEGER_OVERFLOW ;
         break;
	case 5:
         return (NTSTATUS)STATUS_INVALID_BUFFER_SIZE ;
         break;
	case 6:
         return (NTSTATUS)STATUS_SUCCESS ;
         break;
    default:
         return sdv_SetReturnStatus();
     }*/
	return sdv_SetReturnStatus(); 
}

NTSTATUS 
sdv_WdfUsbTargetPipeFormatRequestForAbort(   
									  IN WDFUSBPIPE  Pipe,  
									  IN WDFREQUEST  Request  
									  )
{
 return sdv_SetReturnStatus();
}


NTSTATUS  
sdv_WdfUsbTargetPipeFormatRequestForReset(   
	IN WDFUSBPIPE  Pipe,   
	IN WDFREQUEST  Request   
	)
{
return sdv_SetReturnStatus();
}


NTSTATUS  
sdv_WdfUsbTargetPipeSendUrbSynchronously(
	IN WDFUSBPIPE  Pipe,  
	IN OPTIONAL WDFREQUEST  Request, 
	IN OPTIONAL PWDF_REQUEST_SEND_OPTIONS  RequestOptions,    
	IN PURB  Urb    )
{
return sdv_SetReturnStatus();
}



BYTE  
sdv_WdfUsbInterfaceGetInterfaceNumber(  
									  IN WDFUSBINTERFACE  UsbInterface   
									  )
{
	return (BYTE) SdvMakeChoice();
}


BYTE  
sdv_WdfUsbInterfaceGetNumEndpoints(   
							   IN WDFUSBINTERFACE  UsbInterface, 
							   UCHAR  SettingIndex   
							   )
{
	return (BYTE) SdvMakeChoice();
}


VOID  
sdv_WdfUsbInterfaceGetDescriptor(  
							 IN WDFUSBINTERFACE  UsbInterface,   
							 UCHAR  SettingIndex,    
							 OUT PUSB_INTERFACE_DESCRIPTOR  InterfaceDescriptor  
							 )
{
	InterfaceDescriptor->bLength				=(UCHAR) SdvMakeChoice();
	InterfaceDescriptor->bDescriptorType		=(UCHAR) SdvMakeChoice();
	InterfaceDescriptor->bInterfaceNumber		=(UCHAR) SdvMakeChoice();
	InterfaceDescriptor->bAlternateSetting		=(UCHAR) SdvMakeChoice(); 
	InterfaceDescriptor->bNumEndpoints			=(UCHAR) SdvMakeChoice(); 
	InterfaceDescriptor->bInterfaceClass		=(UCHAR) SdvMakeChoice();
	InterfaceDescriptor->bInterfaceSubClass		=(UCHAR) SdvMakeChoice();
	InterfaceDescriptor->bInterfaceProtocol		=(UCHAR) SdvMakeChoice();
	InterfaceDescriptor->iInterface				=(UCHAR) SdvMakeChoice();

}

BYTE  
sdv_WdfUsbInterfaceGetNumSettings(   
							  IN WDFUSBINTERFACE  UsbInterface 
							  )
{
	return (BYTE) SdvMakeChoice();
}

NTSTATUS  
sdv_WdfUsbInterfaceSelectSetting(   
							 IN WDFUSBINTERFACE  UsbInterface,  
							 IN OPTIONAL PWDF_OBJECT_ATTRIBUTES  PipesAttributes, 
							 IN PWDF_USB_INTERFACE_SELECT_SETTING_PARAMS  Params    
							 )
{
	return sdv_SetReturnStatus();
}

VOID  
sdv_WdfUsbInterfaceGetEndpointInformation(   
									  IN WDFUSBINTERFACE  UsbInterface, 
									  UCHAR  SettingIndex,    
									  UCHAR  EndpointIndex,   
									  IN PWDF_USB_PIPE_INFORMATION  EndpointInfo 
									  )
{
int x = SdvMakeChoice();
    switch(x)
    {
      case 0:
         EndpointInfo->PipeType=(WDF_USB_PIPE_TYPE)WdfUsbPipeTypeInvalid ;
         break;
	case 1:
         EndpointInfo->PipeType= (WDF_USB_PIPE_TYPE)WdfUsbPipeTypeControl ;
         break;
	case 2:
         EndpointInfo->PipeType= (WDF_USB_PIPE_TYPE)WdfUsbPipeTypeIsochronous ;
         break;
	case 3:
         EndpointInfo->PipeType= (WDF_USB_PIPE_TYPE)WdfUsbPipeTypeBulk ;
         break;
    default:
         EndpointInfo->PipeType= (WDF_USB_PIPE_TYPE)WdfUsbPipeTypeInterrupt;
     }
EndpointInfo->Size		=(ULONG) SdvMakeChoice();
EndpointInfo->MaximumPacketSize		=(ULONG) SdvMakeChoice();
EndpointInfo->EndpointAddress		=(UCHAR) SdvMakeChoice();
EndpointInfo->Interval				=(UCHAR) SdvMakeChoice();
EndpointInfo->SettingIndex			=(UCHAR) SdvMakeChoice();
EndpointInfo->MaximumTransferSize	=(ULONG) SdvMakeChoice();
}



BYTE 
sdv_WdfUsbInterfaceGetConfiguredSettingIndex( 
	IN WDFUSBINTERFACE  Interface  
	)
{
	return (BYTE) SdvMakeChoice();
}


BYTE  
sdv_WdfUsbInterfaceGetNumConfiguredPipes(   
									 IN WDFUSBINTERFACE  UsbInterface  
									 )
{
	return (BYTE) SdvMakeChoice();
}

WDFUSBPIPE  
sdv_WdfUsbInterfaceGetConfiguredPipe(  
								 IN WDFUSBINTERFACE  UsbInterface,
								 IN UCHAR  PipeIndex,   
								 OUT OPTIONAL PWDF_USB_PIPE_INFORMATION  PipeInfo   
								 )
{	int x = SdvMakeChoice();
	int y = SdvMakeChoice();
    switch(x)
    {
      case 0:
			if (PipeInfo!=NULL)
			{
				PipeInfo->Size					=(ULONG) SdvMakeChoice();
				PipeInfo->MaximumPacketSize		=(ULONG) SdvMakeChoice();
				PipeInfo->EndpointAddress		=(UCHAR) SdvMakeChoice();
				PipeInfo->Interval				=(UCHAR) SdvMakeChoice();
				PipeInfo->SettingIndex			=(UCHAR) SdvMakeChoice();
				PipeInfo->MaximumTransferSize	=(ULONG) SdvMakeChoice();
				
					switch(y)
					{
					  case 0:
						 PipeInfo->PipeType=(WDF_USB_PIPE_TYPE)WdfUsbPipeTypeInvalid ;
						 break;
					case 1:
						 PipeInfo->PipeType= (WDF_USB_PIPE_TYPE)WdfUsbPipeTypeControl ;
						 break;
					case 2:
						 PipeInfo->PipeType= (WDF_USB_PIPE_TYPE)WdfUsbPipeTypeIsochronous ;
						 break;
					case 3:
						 PipeInfo->PipeType= (WDF_USB_PIPE_TYPE)WdfUsbPipeTypeBulk ;
						 break;
					default:
						 PipeInfo->PipeType= (WDF_USB_PIPE_TYPE)WdfUsbPipeTypeInterrupt;
					}
			 }

			return (WDFUSBPIPE)malloc(1);
			break;

	  default: 
			return NULL;


	}

}


USBD_PIPE_HANDLE  
sdv_WdfUsbTargetPipeWdmGetPipeHandle(  
								 WDFUSBPIPE  UsbPipe   
								 )
{
	return (USBD_PIPE_HANDLE)malloc(1);
}

UCHAR
sdv_WdfUsbTargetDeviceGetNumInterfaces(
   WDFUSBDEVICE UsbDevice
    )
{
	return (UCHAR) SdvMakeChoice();
}
NTSTATUS
sdv_WdfUsbTargetDeviceSelectConfig(
    WDFUSBDEVICE UsbDevice,
    PWDF_OBJECT_ATTRIBUTES PipeAttributes,
    PWDF_USB_DEVICE_SELECT_CONFIG_PARAMS Params
    )
{
	return (NTSTATUS) sdv_SetReturnStatus();
}

NTSTATUS
sdv_WdfUsbTargetDeviceRetrieveCurrentFrameNumber(
    WDFUSBDEVICE UsbDevice,
    PULONG CurrentFrameNumber
    )
{
	int x = SdvMakeChoice();
    switch (x) { 
        case 0: CurrentFrameNumber = (PULONG) pnotused;			
			    return STATUS_SUCCESS;
		default: CurrentFrameNumber = NULL;
			     return STATUS_NOT_SUPPORTED;
	}
}

NTSTATUS
sdv_WdfUsbTargetDeviceIsConnectedSynchronous(
    WDFUSBDEVICE UsbDevice
    )
{
	return sdv_SetReturnStatus();
}
NTSTATUS
sdv_WdfUsbTargetDeviceResetPortSynchronously(
    WDFUSBDEVICE UsbDevice
    )
{
	return sdv_SetReturnStatus();
}
VOID
sdv_WdfUsbTargetPipeGetInformation(
    WDFUSBPIPE Pipe,
    PUSBD_PIPE_INFORMATION PipeInformation
    )
{
}
VOID
sdv_WdfUsbTargetPipeSetNoMaximumPacketSizeCheck(
    WDFUSBPIPE Pipe
    )
{
}
NTSTATUS
sdv_WdfUsbTargetPipeAbortSynchronously(
    WDFUSBPIPE Pipe,
    IN OPTIONAL WDFREQUEST Request,
    PWDF_REQUEST_SEND_OPTIONS RequestOptions
    )
{
	return sdv_SetReturnStatus();
}
NTSTATUS
sdv_WdfUsbTargetPipeResetSynchronously(
    WDFUSBPIPE Pipe,
	IN OPTIONAL WDFREQUEST Request,
    PWDF_REQUEST_SEND_OPTIONS RequestOptions
    )
{
	return sdv_SetReturnStatus();
}

NTSTATUS
sdv_WdfUsbTargetPipeFormatRequestForUrb(
   WDFUSBPIPE PIPE,
   WDFREQUEST Request,
    WDFMEMORY UrbMemory,
    PWDFMEMORY_OFFSET UrbMemoryOffset
    )
{
	return sdv_SetReturnStatus();
}
UCHAR
sdv_WdfUsbTargetDeviceGetInterfaceNumberByIndex(
    WDFUSBDEVICE UsbDevice,
    IN UCHAR InterfaceIndex
    )
{
	return (UCHAR) 0;
}

#endif
/* wdf_sdv_usb_stubs.c end */

