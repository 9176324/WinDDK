/*
	Copyright (c) Microsoft Corporation.  All rights reserved.
*/

#ifndef _SDV_CSQ_H_
#define _SDV_CSQ_H_

#ifdef IoCsqInitialize
#undef IoCsqInitialize
#endif
#define IoCsqInitialize sdv_IoCsqInitialize
NTSTATUS
sdv_IoCsqInitialize(
    IN PIO_CSQ                          Csq,
    IN PIO_CSQ_INSERT_IRP               CsqInsertIrp,
    IN PIO_CSQ_REMOVE_IRP               CsqRemoveIrp,
    IN PIO_CSQ_PEEK_NEXT_IRP            CsqPeekNextIrp,
    IN PIO_CSQ_ACQUIRE_LOCK             CsqAcquireLock,
    IN PIO_CSQ_RELEASE_LOCK             CsqReleaseLock,
    IN PIO_CSQ_COMPLETE_CANCELED_IRP    CsqCompleteCanceledIrp
    );

#ifdef IoCsqInitializeEx
#undef IoCsqInitializeEx
#endif
#define IoCsqInitializeEx sdv_IoCsqInitializeEx
NTSTATUS
sdv_IoCsqInitializeEx(
    IN PIO_CSQ                          Csq,
    IN PIO_CSQ_INSERT_IRP_EX            CsqInsertIrp,
    IN PIO_CSQ_REMOVE_IRP               CsqRemoveIrp,
    IN PIO_CSQ_PEEK_NEXT_IRP            CsqPeekNextIrp,
    IN PIO_CSQ_ACQUIRE_LOCK             CsqAcquireLock,
    IN PIO_CSQ_RELEASE_LOCK             CsqReleaseLock,
    IN PIO_CSQ_COMPLETE_CANCELED_IRP    CsqCompleteCanceledIrp
    );

#ifdef IoCsqInsertIrp
#undef IoCsqInsertIrp
#endif
#define IoCsqInsertIrp(arg1, arg2, arg3) sdv_IoCsqInsertIrp(arg1, arg2, arg3);
VOID
sdv_IoCsqInsertIrp(
    IN  PIO_CSQ             Csq,
    IN  PIRP                Irp,
    IN  PIO_CSQ_IRP_CONTEXT Context
    );


#ifdef IoCsqInsertIrpEx
#undef IoCsqInsertIrpEx
#endif
#define IoCsqInsertIrpEx sdv_IoCsqInsertIrpEx

NTSTATUS
sdv_IoCsqInsertIrpEx(
    IN  PIO_CSQ             Csq,
    IN  PIRP                Irp,
    IN  PIO_CSQ_IRP_CONTEXT Context,
    IN  PVOID               InsertContext
    );

#ifdef IoCsqRemoveNextIrp
#undef IoCsqRemoveNextIrp
#endif
#define IoCsqRemoveNextIrp sdv_IoCsqRemoveNextIrp
PIRP
sdv_IoCsqRemoveNextIrp(
    IN  PIO_CSQ   Csq,
    IN  PVOID     PeekContext
    );

#ifdef IoCsqRemoveIrp
#undef IoCsqRemoveIrp
#endif
#define IoCsqRemoveIrp sdv_IoCsqRemoveIrp
PIRP
sdv_IoCsqRemoveIrp(
    IN  PIO_CSQ             Csq,
    IN  PIO_CSQ_IRP_CONTEXT Context
    );

#endif
