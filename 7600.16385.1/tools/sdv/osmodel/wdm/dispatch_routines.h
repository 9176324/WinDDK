/*
	Copyright (c) Microsoft Corporation.  All rights reserved.
*/

/*
    This file declares the driver entry points.
*/

extern DRIVER_INITIALIZE fun_DriverEntry;

#ifdef fun_IRP_MJ_CREATE
extern DRIVER_DISPATCH fun_IRP_MJ_CREATE; /*SAL:0x00*/
#endif

#ifdef fun_IRP_MJ_CREATE_NAMED_PIPE
extern DRIVER_DISPATCH fun_IRP_MJ_CREATE_NAMED_PIPE; /*SAL:0x01*/
#endif

#ifdef fun_IRP_MJ_CLOSE
extern DRIVER_DISPATCH fun_IRP_MJ_CLOSE; /*SAL:0x02*/
#endif

#ifdef fun_IRP_MJ_READ
extern DRIVER_DISPATCH fun_IRP_MJ_READ; /*SAL:0x03*/
#endif

#ifdef fun_IRP_MJ_WRITE
extern DRIVER_DISPATCH fun_IRP_MJ_WRITE; /*SAL:0x04*/
#endif

#ifdef fun_IRP_MJ_QUERY_INFORMATION
extern DRIVER_DISPATCH fun_IRP_MJ_QUERY_INFORMATION; /*SAL:0x05*/
#endif

#ifdef fun_IRP_MJ_SET_INFORMATION
extern DRIVER_DISPATCH fun_IRP_MJ_SET_INFORMATION; /*SAL:0x06*/
#endif

#ifdef fun_IRP_MJ_QUERY_EA
extern DRIVER_DISPATCH fun_IRP_MJ_QUERY_EA; /*SAL:0x07*/
#endif

#ifdef fun_IRP_MJ_SET_EA
extern DRIVER_DISPATCH fun_IRP_MJ_SET_EA; /*SAL:0x08*/
#endif

#ifdef fun_IRP_MJ_FLUSH_BUFFERS
extern DRIVER_DISPATCH fun_IRP_MJ_FLUSH_BUFFERS; /*SAL:0x09*/
#endif

#ifdef fun_IRP_MJ_QUERY_VOLUME_INFORMATION
extern DRIVER_DISPATCH fun_IRP_MJ_QUERY_VOLUME_INFORMATION; /*SAL:0x0a*/
#endif

#ifdef fun_IRP_MJ_SET_VOLUME_INFORMATION
extern DRIVER_DISPATCH fun_IRP_MJ_SET_VOLUME_INFORMATION; /*SAL:0x0b*/
#endif

#ifdef fun_IRP_MJ_DIRECTORY_CONTROL
extern DRIVER_DISPATCH fun_IRP_MJ_DIRECTORY_CONTROL; /*SAL:0x0c*/
#endif

#ifdef fun_IRP_MJ_FILE_SYSTEM_CONTROL
extern DRIVER_DISPATCH fun_IRP_MJ_FILE_SYSTEM_CONTROL; /*SAL:0x0d*/
#endif

#ifdef fun_IRP_MJ_DEVICE_CONTROL
extern DRIVER_DISPATCH fun_IRP_MJ_DEVICE_CONTROL; /*SAL:0x0e*/
#endif

#ifdef fun_IRP_MJ_INTERNAL_DEVICE_CONTROL
extern DRIVER_DISPATCH fun_IRP_MJ_INTERNAL_DEVICE_CONTROL; /*SAL:0x0f*/
#endif

#ifdef fun_IRP_MJ_SHUTDOWN
extern DRIVER_DISPATCH fun_IRP_MJ_SHUTDOWN; /*SAL:0x10*/
#endif

#ifdef fun_IRP_MJ_LOCK_CONTROL
extern DRIVER_DISPATCH fun_IRP_MJ_LOCK_CONTROL; /*SAL:0x11*/
#endif

#ifdef fun_IRP_MJ_CLEANUP
extern DRIVER_DISPATCH fun_IRP_MJ_CLEANUP; /*SAL:0x12*/
#endif

#ifdef fun_IRP_MJ_CREATE_MAILSLOT
extern DRIVER_DISPATCH fun_IRP_MJ_CREATE_MAILSLOT; /*SAL:0x13*/
#endif

#ifdef fun_IRP_MJ_QUERY_SECURITY
extern DRIVER_DISPATCH fun_IRP_MJ_QUERY_SECURITY; /*SAL:0x14*/
#endif

#ifdef fun_IRP_MJ_SET_SECURITY
extern DRIVER_DISPATCH fun_IRP_MJ_SET_SECURITY; /*SAL:0x15*/
#endif

#ifdef fun_IRP_MJ_POWER
extern DRIVER_DISPATCH fun_IRP_MJ_POWER; /*SAL:0x16*/
#endif

#ifdef fun_IRP_MJ_SYSTEM_CONTROL
extern DRIVER_DISPATCH fun_IRP_MJ_SYSTEM_CONTROL; /*SAL:0x17*/
#endif

#ifdef fun_IRP_MJ_DEVICE_CHANGE
extern DRIVER_DISPATCH fun_IRP_MJ_DEVICE_CHANGE; /*SAL:0x18*/
#endif

#ifdef fun_IRP_MJ_QUERY_QUOTA
extern DRIVER_DISPATCH fun_IRP_MJ_QUERY_QUOTA; /*SAL:0x19*/
#endif

#ifdef fun_IRP_MJ_SET_QUOTA
extern DRIVER_DISPATCH fun_IRP_MJ_SET_QUOTA; /*SAL:0x1a*/
#endif

#ifdef fun_IRP_MJ_PNP
extern DRIVER_DISPATCH fun_IRP_MJ_PNP; /*SAL:0x1b*/
#endif

#ifdef fun_IRP_MJ_OTHER
extern DRIVER_DISPATCH fun_IRP_MJ_OTHER; /*SAL:-1*/
#endif

#ifdef fun_DriverStartIo
extern DRIVER_STARTIO fun_DriverStartIo;
#endif

#ifdef fun_AddDevice
extern DRIVER_ADD_DEVICE fun_AddDevice;
#endif

#ifdef fun_DriverUnload
extern DRIVER_UNLOAD fun_DriverUnload;
#endif

#ifdef fun_IO_COMPLETION_ROUTINE_1
extern IO_COMPLETION_ROUTINE fun_IO_COMPLETION_ROUTINE_1; /*multiple*/
#endif

#ifdef fun_IO_COMPLETION_ROUTINE_2
extern IO_COMPLETION_ROUTINE fun_IO_COMPLETION_ROUTINE_2; /*multiple*/
#endif

#ifdef fun_IO_COMPLETION_ROUTINE_3
extern IO_COMPLETION_ROUTINE fun_IO_COMPLETION_ROUTINE_3; /*multiple*/
#endif

#ifdef fun_IO_COMPLETION_ROUTINE_4
extern IO_COMPLETION_ROUTINE fun_IO_COMPLETION_ROUTINE_4; /*multiple*/
#endif

#ifdef fun_IO_COMPLETION_ROUTINE_5
extern IO_COMPLETION_ROUTINE fun_IO_COMPLETION_ROUTINE_5; /*multiple*/
#endif

#ifdef fun_IO_COMPLETION_ROUTINE_6
extern IO_COMPLETION_ROUTINE fun_IO_COMPLETION_ROUTINE_6; /*multiple*/
#endif

#ifdef fun_IO_DPC_ROUTINE_1
extern IO_DPC_ROUTINE fun_IO_DPC_ROUTINE_1; /*multiple*/
#endif

#ifdef fun_IO_DPC_ROUTINE_2
extern IO_DPC_ROUTINE fun_IO_DPC_ROUTINE_2; /*multiple*/
#endif

#ifdef fun_IO_DPC_ROUTINE_3
extern IO_DPC_ROUTINE fun_IO_DPC_ROUTINE_3; /*multiple*/
#endif

#ifdef fun_IO_DPC_ROUTINE_4
extern IO_DPC_ROUTINE fun_IO_DPC_ROUTINE_4; /*multiple*/
#endif

#ifdef fun_IO_DPC_ROUTINE_5
extern IO_DPC_ROUTINE fun_IO_DPC_ROUTINE_5; /*multiple*/
#endif

#ifdef fun_IO_DPC_ROUTINE_6
extern IO_DPC_ROUTINE fun_IO_DPC_ROUTINE_6; /*multiple*/
#endif


#ifdef fun_KDEFERRED_ROUTINE_1
extern KDEFERRED_ROUTINE fun_KDEFERRED_ROUTINE_1; /*multiple*/
#endif

#ifdef fun_KDEFERRED_ROUTINE_2
extern KDEFERRED_ROUTINE fun_KDEFERRED_ROUTINE_2; /*multiple*/
#endif

#ifdef fun_KDEFERRED_ROUTINE_3
extern KDEFERRED_ROUTINE fun_KDEFERRED_ROUTINE_3; /*multiple*/
#endif

#ifdef fun_KDEFERRED_ROUTINE_4
extern KDEFERRED_ROUTINE fun_KDEFERRED_ROUTINE_4; /*multiple*/
#endif

#ifdef fun_KDEFERRED_ROUTINE_5
extern KDEFERRED_ROUTINE fun_KDEFERRED_ROUTINE_5; /*multiple*/
#endif

#ifdef fun_KDEFERRED_ROUTINE_6
extern KDEFERRED_ROUTINE fun_KDEFERRED_ROUTINE_6; /*multiple*/
#endif

#ifdef fun_KDEFERRED_ROUTINE_7
extern KDEFERRED_ROUTINE fun_KDEFERRED_ROUTINE_7; /*multiple*/
#endif

#ifdef fun_KDEFERRED_ROUTINE_8
extern KDEFERRED_ROUTINE fun_KDEFERRED_ROUTINE_8; /*multiple*/
#endif

#ifdef fun_KDEFERRED_ROUTINE_9
extern KDEFERRED_ROUTINE fun_KDEFERRED_ROUTINE_9; /*multiple*/
#endif

#ifdef fun_KSERVICE_ROUTINE_1
extern KSERVICE_ROUTINE fun_KSERVICE_ROUTINE_1; /*multiple*/
#endif

#ifdef fun_KSERVICE_ROUTINE_2
extern KSERVICE_ROUTINE fun_KSERVICE_ROUTINE_2; /*multiple*/
#endif

#ifdef fun_KSERVICE_ROUTINE_3
extern KSERVICE_ROUTINE fun_KSERVICE_ROUTINE_3; /*multiple*/
#endif

#ifdef fun_KSERVICE_ROUTINE_4
extern KSERVICE_ROUTINE fun_KSERVICE_ROUTINE_4; /*multiple*/
#endif

#ifdef fun_KSERVICE_ROUTINE_5
extern KSERVICE_ROUTINE fun_KSERVICE_ROUTINE_5; /*multiple*/
#endif

#ifdef fun_KSERVICE_ROUTINE_6
extern KSERVICE_ROUTINE fun_KSERVICE_ROUTINE_6; /*multiple*/
#endif

#ifdef fun_DRIVER_CANCEL_1
extern DRIVER_CANCEL fun_DRIVER_CANCEL_1; /*multiple*/ /*callable_from_driver_via_function_pointer*/
#endif

#ifdef fun_DRIVER_CANCEL_2
extern DRIVER_CANCEL fun_DRIVER_CANCEL_2; /*multiple*/ /*callable_from_driver_via_function_pointer*/
#endif

#ifdef fun_DRIVER_CANCEL_3
extern DRIVER_CANCEL fun_DRIVER_CANCEL_3; /*multiple*/ /*callable_from_driver_via_function_pointer*/
#endif

#ifdef fun_DRIVER_CANCEL_4
extern DRIVER_CANCEL fun_DRIVER_CANCEL_4; /*multiple*/ /*callable_from_driver_via_function_pointer*/
#endif

#ifdef fun_DRIVER_CANCEL_5
extern DRIVER_CANCEL fun_DRIVER_CANCEL_5; /*multiple*/ /*callable_from_driver_via_function_pointer*/
#endif

#ifdef fun_DRIVER_CANCEL_6
extern DRIVER_CANCEL fun_DRIVER_CANCEL_6; /*multiple*/ /*callable_from_driver_via_function_pointer*/
#endif

#ifdef fun_DRIVER_CANCEL_7
extern DRIVER_CANCEL fun_DRIVER_CANCEL_7; /*multiple*/ /*callable_from_driver_via_function_pointer*/
#endif

#ifdef fun_WORKER_THREAD_ROUTINE_1
extern WORKER_THREAD_ROUTINE fun_WORKER_THREAD_ROUTINE_1; /*multiple*/
#endif

#ifdef fun_WORKER_THREAD_ROUTINE_2
extern WORKER_THREAD_ROUTINE fun_WORKER_THREAD_ROUTINE_2; /*multiple*/
#endif

#ifdef fun_WORKER_THREAD_ROUTINE_3
extern WORKER_THREAD_ROUTINE fun_WORKER_THREAD_ROUTINE_3; /*multiple*/
#endif

#ifdef fun_WORKER_THREAD_ROUTINE_4
extern WORKER_THREAD_ROUTINE fun_WORKER_THREAD_ROUTINE_4; /*multiple*/
#endif

#ifdef fun_WORKER_THREAD_ROUTINE_5
extern WORKER_THREAD_ROUTINE fun_WORKER_THREAD_ROUTINE_5; /*multiple*/
#endif

#ifdef fun_WORKER_THREAD_ROUTINE_6
extern WORKER_THREAD_ROUTINE fun_WORKER_THREAD_ROUTINE_6; /*multiple*/
#endif
