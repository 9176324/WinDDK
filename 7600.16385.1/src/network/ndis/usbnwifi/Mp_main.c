/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

    THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
    PURPOSE.

Module Name:

   Mp_main.C

Abstract:

    This sample provides an example of minimal driver intended for education
    purposes. Neither the driver or its sample test programs are intended
    for use in a production environment.

Author:  Vishal Manan (Oct 10, 2005)

Revision History:

    Converted the RealText PCI driver to USB 

Notes:

--*/
#include "Precomp.h"
#include "Mp_Oids.h"
#include "Mp_Main.h"
#if DOT11_TRACE_ENABLED
#include "Mp_Main.tmh"
#endif

#include "mp_cntl.h"
//
// Functions that are not tagged PAGEABLE by the pragma as shown below are
// by default resident (NONPAGEABLE) in memory. Make sure you don't acquire
// spinlock or raise the IRQL in a pageable function. It's okay to call
// another nonpageable function that runs at DISPATCH_LEVEL from a
// pageable function.
//
#pragma NDIS_INIT_FUNCTION(DriverEntry)
#pragma NDIS_PAGEABLE_FUNCTION(MPInitialize)
#pragma NDIS_PAGEABLE_FUNCTION(MPHalt)
#pragma NDIS_PAGEABLE_FUNCTION(DriverUnload)

NDIS_HANDLE     NdisMiniportDriverHandle = NULL;
NDIS_HANDLE     GlobalDriverContext = NULL;

MP_REG_ENTRY MPRegTable[] = {
// reg value name                           Offset in ADAPTER               Field size                   Default Value          Min                 Max
    {NDIS_STRING_CONST("*ReceiveBuffers"),  MP_OFFSET(TotalRxMSDU),         MP_SIZE(TotalRxMSDU),       HW11_DEF_RX_MSDUS,  HW11_MIN_RX_MSDUS,  HW11_MAX_RX_MSDUS},
    {NDIS_STRING_CONST("MinRxBuffers"),     MP_OFFSET(MinRxMSDU),           MP_SIZE(MinRxMSDU),            HW11_MIN_RX_MSDUS,  HW11_MIN_RX_MSDUS,  HW11_MIN_RX_MSDUS},
    {NDIS_STRING_CONST("MaxRxBuffers"),     MP_OFFSET(MaxRxMSDU),           MP_SIZE(MaxRxMSDU),            HW11_MAX_RX_MSDUS,  HW11_MAX_RX_MSDUS,  HW11_MAX_RX_MSDUS},
    {NDIS_STRING_CONST("*TransmitBuffers"), MP_OFFSET(TotalTxMSDUs),        MP_SIZE(TotalTxMSDUs),      HW11_DEF_TX_MSDUS,  HW11_MIN_TX_MSDUS,  HW11_MAX_TX_MSDUS},
};

#define MP_NUM_REG_PARAMS (sizeof (MPRegTable) / sizeof(MP_REG_ENTRY))


NTSTATUS
DriverEntry(
    PDRIVER_OBJECT DriverObject,
    PUNICODE_STRING RegistryPath
    )
/*++
Routine Description:

    In the context of its DriverEntry function, a miniport driver associates
    itself with NDIS, specifies the NDIS version that it is using, and
    registers its entry points.


Arguments:
    PVOID DriverObject - pointer to the driver object.
    PVOID RegistryPath - pointer to the driver registry path.

    Return Value:

    NDIS_STATUS_xxx code

--*/
{

    NDIS_STATUS                             Status;
    NDIS_MINIPORT_DRIVER_CHARACTERISTICS    MPChar;
    WDF_DRIVER_CONFIG                       config;
    NTSTATUS                                ntStatus;
    WDFDRIVER                       hDriver; //vm control
    

    #if DOT11_TRACE_ENABLED
        WPP_INIT_TRACING(DriverObject, RegistryPath);
    #endif
    MpTrace(COMP_INIT_PNP, DBG_SERIOUS, ("---> WifiUSB WDF sample built on "__DATE__" at "__TIME__ "\n"));
    MpTrace(COMP_INIT_PNP,DBG_SERIOUS, ("---> Sample is built with NDIS Version %d.%d\n",
                            MP_MAJOR_NDIS_VERSION, MP_MINOR_NDIS_VERSION));

    //
    // Make sure we are compatible with the version of NDIS supported by OS.
    //
    if(NdisGetVersion() < ((MP_MAJOR_NDIS_VERSION << 16) | MP_MINOR_NDIS_VERSION)){
        MpTrace(COMP_INIT_PNP,DBG_SERIOUS, ("This version of driver is not support on this OS\n"));
        return NDIS_STATUS_FAILURE;
    }


    WDF_DRIVER_CONFIG_INIT(&config, WDF_NO_EVENT_CALLBACK);
    //
    // Set WdfDriverInitNoDispatchOverride flag to tell the framework
    // not to provide dispatch routines for the driver. In other words,
    // the framework must not intercept IRPs that the I/O manager has
    // directed to the driver. In this case, it will be handled by NDIS
    // port driver.
    //
    config.DriverInitFlags |= WdfDriverInitNoDispatchOverride;

    ntStatus = WdfDriverCreate(DriverObject,
                               RegistryPath,
                               WDF_NO_OBJECT_ATTRIBUTES,
                               &config,                
                               &hDriver); //vm control
    if(!NT_SUCCESS(ntStatus)){
        MpTrace(COMP_INIT_PNP,DBG_SERIOUS, ("WdfDriverCreate failed\n"));
        return NDIS_STATUS_FAILURE;
    }
    
    //
    // Fill in the Miniport characteristics structure with the version numbers
    // and the entry points for driver-supplied MiniportXxx
    //
    NdisZeroMemory(&MPChar, sizeof(MPChar));


    MPChar.Header.Type = NDIS_OBJECT_TYPE_MINIPORT_DRIVER_CHARACTERISTICS,
    MPChar.Header.Size = sizeof(NDIS_MINIPORT_DRIVER_CHARACTERISTICS);
    MPChar.Header.Revision = NDIS_MINIPORT_DRIVER_CHARACTERISTICS_REVISION_1;
    MPChar.MajorNdisVersion = MP_MAJOR_NDIS_VERSION;
    MPChar.MinorNdisVersion = MP_MINOR_NDIS_VERSION;
    MPChar.MajorDriverVersion           = HW11_MAJOR_DRIVER_VERSION;
    MPChar.MinorDriverVersion           = HW11_MINOR_DRIVER_VERSION;

    //
    // Init/PnP handlers
    //
    MPChar.InitializeHandlerEx      = MPInitialize;
    MPChar.RestartHandler           = MPRestart;
    MPChar.PauseHandler             = MPPause;

    MPChar.ShutdownHandlerEx        = MPAdapterShutdown; 
    MPChar.DevicePnPEventNotifyHandler  = MPDevicePnPEvent;
    MPChar.HaltHandlerEx            = MPHalt;
    MPChar.UnloadHandler            = DriverUnload;
    
    //
    // Query/Set/Method requests handlers
    //
    MPChar.OidRequestHandler        = MPRequest;
    MPChar.CancelOidRequestHandler  = MPCancelRequest;

    //
    // Set optional miniport services handler
    //
    MPChar.SetOptionsHandler        = MPSetOptions;

    //
    // Send/Receive handlers
    //
    MPChar.SendNetBufferListsHandler    = MPSendNetBufferLists;
    MPChar.CancelSendHandler            = MPCancelSendNetBufferLists;
    MPChar.ReturnNetBufferListsHandler  = MPReturnNetBufferLists;
    
    //
    // Fault handling handlers
    //
    MPChar.CheckForHangHandlerEx        = MPCheckForHang;
    MPChar.ResetHandlerEx               = MPReset;
    MPChar.Flags = NDIS_WDM_DRIVER;      

    MpTrace(COMP_INIT_PNP,DBG_LOUD, ("Calling NdisMRegisterMiniportDriver...\n"));

    //
    // Registers miniport's entry points with the NDIS library as the first
    // step in NIC driver initialization. The NDIS will call the
    // MiniportInitialize when the device is actually started by the PNP
    // manager.
    //
    Status = NdisMRegisterMiniportDriver(DriverObject,
                                         RegistryPath,
                                         (PNDIS_HANDLE)GlobalDriverContext,
                                         &MPChar,
                                         &NdisMiniportDriverHandle);

    if (Status != NDIS_STATUS_SUCCESS) {
        MpTrace(COMP_INIT_PNP,DBG_SERIOUS, ("NdisMRegisterMiniportDriver Status = 0x%08x\n", Status));
        #if DOT11_TRACE_ENABLED
            WPP_CLEANUP(DriverObject);
        #endif
    }
    
    MpTrace(COMP_INIT_PNP,DBG_TRACE, ("<--- DriverEntry\n"));
    return Status;

}

INLINE NDIS_STATUS
MpSet80211Attributes(
    __in  PADAPTER                        pAdapter
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    NDIS_MINIPORT_ADAPTER_NATIVE_802_11_ATTRIBUTES  attr;

    __try
    {
        NdisZeroMemory(&attr, sizeof(NDIS_MINIPORT_ADAPTER_NATIVE_802_11_ATTRIBUTES));

        attr.Header.Type = NDIS_OBJECT_TYPE_MINIPORT_ADAPTER_NATIVE_802_11_ATTRIBUTES;
        attr.Header.Revision = NDIS_MINIPORT_ADAPTER_802_11_ATTRIBUTES_REVISION_1;
        attr.Header.Size = sizeof(NDIS_MINIPORT_ADAPTER_NATIVE_802_11_ATTRIBUTES);
        attr.OpModeCapability = (DOT11_OPERATION_MODE_EXTENSIBLE_STATION |
                                 DOT11_OPERATION_MODE_NETWORK_MONITOR);   
        
        //
        // Call Hardware and Station lay to fill the attribute structure.
        //
        ndisStatus = Hw11Fill80211Attributes(pAdapter->Nic, &attr);
        if (ndisStatus != NDIS_STATUS_SUCCESS) {
            __leave;
        }

        ndisStatus = Sta11Fill80211Attributes(pAdapter->Station, &attr);
        if (ndisStatus != NDIS_STATUS_SUCCESS) {
                __leave;
        }

        //
        // Register the 802.11 miniport attributes with NDIS
        //
        ndisStatus = NdisMSetMiniportAttributes(
            pAdapter->MiniportAdapterHandle,
            (PNDIS_MINIPORT_ADAPTER_ATTRIBUTES)&attr
            );
    }
    __finally
    {
        //
        // Call Hardware and Station lay to clean up the attribute structure.
        //
        Hw11Cleanup80211Attributes(pAdapter->Nic, &attr);
        Sta11Cleanup80211Attributes(pAdapter->Station, &attr);
    }

    return ndisStatus;

}


/**
 * This function Deallocates an adapter object, returning all the resources back to
 * the system. Any allocations that are made for Adapter, must be deallocated from
 * here
 * 
 * \param pAdapter The adapter object to deallocate
 * \sa MpAllocateAdapter
 */
VOID
MpFreeAdapter(
    __in __drv_freesMem(Pool) PADAPTER                   pAdapter
    )
{
    //
    // Free the resources allocated for station information
    // 
    if (pAdapter->Station)
    {
        Sta11FreeStation(pAdapter->Station);
    }

    //
    // Free the resources allocated by the NIC
    //
    if (pAdapter->Nic)
    {
        Hw11FreeNic(pAdapter->Nic);
    }

    if(pAdapter->NdisResetWorkItem)
    {  
        WdfObjectDelete(pAdapter->NdisResetWorkItem);
        pAdapter->NdisResetWorkItem = NULL;
    }

    if (pAdapter->Dot11ResetWorkItem)
    {  
        WdfObjectDelete(pAdapter->Dot11ResetWorkItem);
        pAdapter->Dot11ResetWorkItem = NULL;
    }

    //
    // Free the Packet queue
    //
    MpDeinitNBLQueue(&pAdapter->TxQueue);

    if (pAdapter->WdfDevice){
        WdfObjectDelete(pAdapter->WdfDevice);
    }

    MP_FREE_MEMORY(pAdapter);
}

NDIS_STATUS
MpAllocateAdapterWorkItem(
    PADAPTER pAdapter
    )
{
    WDF_OBJECT_ATTRIBUTES   attributes;
    WDF_WORKITEM_CONFIG     workitemConfig;    
    NTSTATUS                ntStatus;
    NDIS_STATUS             ndisStatus = NDIS_STATUS_SUCCESS;
    PADAPTER_WORKITEM_CONTEXT   adapterWorkitemContext;
    //
    // Allocate the work item (MpDot11ResetWorkItem)
    //
    WDF_OBJECT_ATTRIBUTES_INIT(&attributes);
    attributes.ParentObject = pAdapter->WdfDevice;
    WDF_OBJECT_ATTRIBUTES_SET_CONTEXT_TYPE(&attributes, ADAPTER_WORKITEM_CONTEXT);

    WDF_WORKITEM_CONFIG_INIT(&workitemConfig, MpDot11ResetWorkItem);
    
    ntStatus = WdfWorkItemCreate(&workitemConfig,
                                &attributes,
                                &pAdapter->Dot11ResetWorkItem);

    if (!NT_SUCCESS(ntStatus))
    {
        
        MpTrace (COMP_INIT_PNP, DBG_SERIOUS, ("Failed to allocate Dot11 Reset Workitem \n"));
        NT_STATUS_TO_NDIS_STATUS(ntStatus, &ndisStatus);
        goto out;
    } 

    adapterWorkitemContext = GetAdapterWorkItemContext(pAdapter->Dot11ResetWorkItem);
    adapterWorkitemContext->Adapter = pAdapter;
    //
    // Specify the context type for the WDF workitem object.
    //
    WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&attributes, ADAPTER_WORKITEM_CONTEXT);
    attributes.ParentObject = pAdapter->WdfDevice;

    WDF_WORKITEM_CONFIG_INIT(&workitemConfig, MpNdisResetWorkItem);
    ntStatus = WdfWorkItemCreate(&workitemConfig,
                                &attributes,
                                &pAdapter->NdisResetWorkItem);
    if (!NT_SUCCESS(ntStatus))
    {
        
        MpTrace (COMP_INIT_PNP, DBG_SERIOUS, ("Failed to allocate Ndis Reset Workitem \n"));
        NT_STATUS_TO_NDIS_STATUS(ntStatus, &ndisStatus);
        goto out;
    }
    adapterWorkitemContext = GetAdapterWorkItemContext(pAdapter->NdisResetWorkItem);
    adapterWorkitemContext->Adapter = pAdapter;

out:        
    return    ndisStatus;
}
/**
 * This routine allocates and initializes an Adapter data structure.
 * This DS represents a Physical Network Adapter present on the
 * system and allows us to work with it.
 * 
 * \param MiniportAdapterHandle     The handle needed to call NDIS API on this adapter
 * \param ppAdapter     The pointer to newly created adapter is returned in this variable
 * \return NDIS_STATUS_SUCCESS if all goes well, else appropriate error code
 * \sa MpFreeAdapter, MpInitialize
 */
NDIS_STATUS
MpAllocateAdapter(
    __in NDIS_HANDLE MiniportAdapterHandle,
    __deref_out_opt PADAPTER* ppAdapter
    )
/*++

Routine Description:
    This routine allocates and initialize an adapter structure.

Arguments:

Return Value:

--*/

{
    PADAPTER                pAdapter = NULL;
    PNIC                    pNic = NULL;
    PSTATION                pStation = NULL;
    NTSTATUS                ntStatus;
    NDIS_STATUS             ndisStatus = NDIS_STATUS_SUCCESS;
    ULONG                   nameLength;
    WDF_OBJECT_ATTRIBUTES   attributes;

    *ppAdapter = NULL;
    
    do
    {
        //
        // Allocate a ADAPTER data structure.
        //
        MP_ALLOCATE_MEMORY(MiniportAdapterHandle, &pAdapter, sizeof(ADAPTER), MP_MEMORY_TAG);
        if (pAdapter == NULL)
        {
            MpTrace(COMP_INIT_PNP, DBG_SERIOUS, ("MpAllocateAdapter: Failed to allocate %d bytes for ADAPTER\n",
                                 sizeof(ADAPTER)));
            ndisStatus = NDIS_STATUS_RESOURCES;
            break;
        }        
        
        //
        // Initialize it.
        //
        NdisZeroMemory(pAdapter, sizeof(ADAPTER));
        NdisInitializeListHead(&pAdapter->Link);

        NdisInitializeEvent(&pAdapter->MiniportRefCountZeroEvent);  // Will be reset during Restart
        NdisInitializeEvent(&pAdapter->ActiveSendersDoneEvent);
        NdisResetEvent(&pAdapter->ActiveSendersDoneEvent);
        NdisInitializeEvent(&pAdapter->HaltWaitEvent);
        NdisResetEvent(&pAdapter->HaltWaitEvent);
        NdisInitializeEvent(&pAdapter->AllReceivesHaveReturnedEvent);
        NdisResetEvent(&pAdapter->AllReceivesHaveReturnedEvent);
        NDIS_INIT_MUTEX(&pAdapter->ResetPnPMutex);
        //
        // Set power state of miniport to D0.
        //
        pAdapter->DevicePowerState = NdisDeviceStateD0;

        //
        // NdisMGetDeviceProperty function enables us to get the:
        // PDO - created by the bus driver to represent our device.
        // FDO - created by NDIS to represent our miniport as a function
        //              driver.
        // NextDeviceObject - deviceobject of another driver (filter)
        //                      attached to us at the bottom.
        //  But if we were to talk to a driver that we
        // are attached to as part of the devicestack then NextDeviceObject
        // would be our target DeviceObject for sending read/write Requests.
        //

        NdisMGetDeviceProperty(MiniportAdapterHandle,
                           &pAdapter->Pdo,
                           &pAdapter->Fdo,
                           &pAdapter->NextDeviceObject,
                           NULL,
                           NULL);

        ntStatus = IoGetDeviceProperty (pAdapter->Pdo,
                                      DevicePropertyDeviceDescription,
                                      NIC_ADAPTER_NAME_SIZE,
                                      pAdapter->AdapterDesc,
                                      &nameLength);

        if (!NT_SUCCESS (ntStatus))
        {
            MpTrace(COMP_INIT_PNP,DBG_SERIOUS, ("IoGetDeviceProperty failed (0x%x)\n",
                                            ntStatus));
            ndisStatus = NDIS_STATUS_FAILURE;
            break;
        }

        WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&attributes, WDF_DEVICE_INFO);

        ntStatus = WdfDeviceMiniportCreate(WdfGetDriver(),
                                         &attributes,
                                         pAdapter->Fdo,
                                         pAdapter->NextDeviceObject,
                                         pAdapter->Pdo,
                                         &pAdapter->WdfDevice);
        if (!NT_SUCCESS (ntStatus))
        {
            MpTrace(COMP_INIT_PNP,DBG_SERIOUS, ("WdfDeviceMiniportCreate failed (0x%x)\n",
                                            ntStatus));
            ndisStatus = NDIS_STATUS_FAILURE;
            break;
        }

        WDF_OBJECT_ATTRIBUTES_INIT(&attributes);
        attributes.ParentObject = pAdapter->WdfDevice;

        ntStatus = WdfSpinLockCreate(&attributes, &pAdapter->SendLock);
        NT_STATUS_TO_NDIS_STATUS(ntStatus,&ndisStatus);
        if (ndisStatus != NDIS_STATUS_SUCCESS) {
            MpTrace(COMP_RECV, DBG_SERIOUS, ("WdfSpinLockCreate   fail!\n"));
            break;
        }

        //
        // Get WDF miniport device context.
        //
        GetWdfDeviceInfo(pAdapter->WdfDevice)->Adapter = pAdapter;

        ndisStatus = MpAllocateAdapterWorkItem(pAdapter);
        if (ndisStatus != NDIS_STATUS_SUCCESS)
        {    
            break;
        }

        //
        // Allocate the Station data structure. This will maintain all 
        // MAC state
        // 
        ndisStatus = Sta11AllocateStation(MiniportAdapterHandle, &pStation);
        if (ndisStatus != NDIS_STATUS_SUCCESS)
        {
            MpTrace (COMP_INIT_PNP, DBG_SERIOUS, ("Sta11AllocateStation failed with status 0x%08x\n", ndisStatus));
            break;
        }

        pAdapter->Station = pStation;

        //
        // Allocate the NIC data structure. This will include the hardware
        // specific data that Hw11 functions will use
        //
        ndisStatus = Hw11AllocateNic(MiniportAdapterHandle, &pNic, pAdapter, pStation);
        if(ndisStatus != NDIS_STATUS_SUCCESS)
        {
            MpTrace (COMP_INIT_PNP, DBG_SERIOUS, ("Hw11AllocateNic failed with status 0x%08x\n", ndisStatus));
            break;
        }
        
        pAdapter->Nic = pNic;

        //
        // Initialize the station state. The NIC is not ready, station cannot yet call any interfaces
        // 
        ndisStatus = Sta11InitializeStation(pStation, pAdapter, pNic);
        if (ndisStatus != NDIS_STATUS_SUCCESS)
        {
            MpTrace (COMP_INIT_PNP, DBG_SERIOUS, ("Sta11InitializeState failed with status 0x%08x\n", ndisStatus));
            break;
        }
        MpInitNBLQueue(&(pAdapter->TxQueue));
        
    }while (FALSE);
    
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        if (pAdapter)
        {
            MpFreeAdapter(pAdapter);
            pAdapter = NULL;
        }

        *ppAdapter = NULL;
    }
    else
    {    
        //
        // Return the allocated Adapter refernce
        //
        *ppAdapter = pAdapter;
    }
    
    return ndisStatus;
}

VOID
MpReadRegistryConfiguration(
    __inout PADAPTER pAdapter
    )
/**
 * This helper function uses NDIS API to read keys and values from the registry.
 * Use these values to make any configurations that the user may want.
 * It is always recommended to access the registry and initialize everything while
 * in the context on MiniportInitialize. Reading and writing to registry while NIC is
 * up and running always causes a performace hit.
 * 
 * \param pAdapter          The adapter whose configuration will be read
 * \sa MpInitialize
 */

{
    NDIS_STATUS                     ndisStatus = NDIS_STATUS_SUCCESS;
    ULONG                           i, ValueRead;
    PUCHAR                          pucDestination;
    PMP_REG_ENTRY                   pRegEntry;
    PNDIS_CONFIGURATION_PARAMETER   Parameter = NULL;
    BOOLEAN                         bRegistryOpened;
    NDIS_HANDLE                     RegistryConfigurationHandle;
    NDIS_CONFIGURATION_OBJECT       ConfigObject;

    ConfigObject.Header.Type = NDIS_OBJECT_TYPE_CONFIGURATION_OBJECT;
    ConfigObject.Header.Revision = NDIS_CONFIGURATION_OBJECT_REVISION_1;
    ConfigObject.Header.Size = sizeof( NDIS_CONFIGURATION_OBJECT );
    ConfigObject.NdisHandle = pAdapter->MiniportAdapterHandle;
    ConfigObject.Flags = 0;

    ndisStatus = NdisOpenConfigurationEx(
                    &ConfigObject,
                    &RegistryConfigurationHandle
                    );

    if (ndisStatus == NDIS_STATUS_SUCCESS)
    {
        bRegistryOpened = TRUE;
    }
    else
    {
        MpTrace (COMP_INIT_PNP, DBG_SERIOUS, ("Unable to Open Configuration. Will revert to defaults for all values\n"));
        bRegistryOpened = FALSE;
    }


    for(i=0; i<MP_NUM_REG_PARAMS; i++)
    {
        //
        // Get the registry entry we will be reading
        //
        pRegEntry= &MPRegTable[i];

        //
        // Figure out where in the adapter structure this value will be placed
        //
        pucDestination = (PUCHAR) pAdapter + pRegEntry->FieldOffset;

        //
        // Read this entry from the registry. All parameters under NT are DWORDs
        //
        if (bRegistryOpened == TRUE)
        {
            NdisReadConfiguration(
                &ndisStatus,
                &Parameter,
                RegistryConfigurationHandle,
                &pRegEntry->RegName,
                NdisParameterInteger
                );
        }
        else
        {
            //
            // Report failure of reading registry. Will revert to defaults
            //
            ndisStatus = NDIS_STATUS_FAILURE;
        }

        if (ndisStatus == NDIS_STATUS_SUCCESS)
        {
            if(Parameter->ParameterData.IntegerData < pRegEntry->Min || 
                Parameter->ParameterData.IntegerData > pRegEntry->Max)
            {
                MpTrace(COMP_INIT_PNP, DBG_SERIOUS, ("A bad value %d read from registry. Reverting to default value %d\n",
                                            Parameter->ParameterData.IntegerData,
                                            pRegEntry->Default
                                            ));
                ValueRead = pRegEntry->Default;
            }
            else
            {
                ValueRead = Parameter->ParameterData.IntegerData;
            }
        }
        else
        {
            MpTrace(COMP_INIT_PNP, DBG_NORMAL, ("Unable to read from registry. Reverting to default value: %d\n",
                                        pRegEntry->Default
                                        ));
            ValueRead = pRegEntry->Default;
        }

        //
        // Moving the registry values in the adapter data structure
        //
        switch(pRegEntry->FieldSize)
        {
            case 1:
                *((PUCHAR) pucDestination)  = (UCHAR) ValueRead;
                break;
            case 2:
                *((PUSHORT) pucDestination) = (USHORT) ValueRead;
                break;
            case 4:
                *((PULONG) pucDestination)  = (ULONG) ValueRead;
                break;
            default:
                MpTrace(COMP_INIT_PNP, DBG_SERIOUS, ("Bogus field size %d\n", pRegEntry->FieldSize));
                break;
        }
    }


    //
    // Now allow the hardware to read its own parameters from registry
    //
    Hw11ReadRegistryConfiguration(pAdapter->Nic, RegistryConfigurationHandle);

    //
    // Now allow the station to read its own parameters from registry
    //
    Sta11ReadRegistryConfiguration(pAdapter->Station, RegistryConfigurationHandle);

    //
    // Close the handle to the registry
    //
    if (RegistryConfigurationHandle)
    {
        NdisCloseConfiguration(RegistryConfigurationHandle);
    }

    
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        MpTrace (COMP_INIT_PNP, DBG_SERIOUS, ("Failed to read from registry. Status = 0x%08x\n", ndisStatus));
    }
    
}

INLINE NDIS_STATUS
MpSetGeneralAttributes(
    __in  PADAPTER                        pAdapter
    )
{
    NDIS_MINIPORT_ADAPTER_ATTRIBUTES        MiniportAttributes;
    NDIS_PNP_CAPABILITIES                   PnpCapabilities;
    
    NdisZeroMemory(&MiniportAttributes, sizeof(MiniportAttributes));
    MiniportAttributes.GeneralAttributes.Header.Type = NDIS_OBJECT_TYPE_MINIPORT_ADAPTER_GENERAL_ATTRIBUTES;
    MiniportAttributes.GeneralAttributes.Header.Revision = NDIS_MINIPORT_ADAPTER_GENERAL_ATTRIBUTES_REVISION_1;
    MiniportAttributes.GeneralAttributes.Header.Size = sizeof(NDIS_MINIPORT_ADAPTER_GENERAL_ATTRIBUTES);

    MiniportAttributes.GeneralAttributes.MediaType = MP_MEDIA_TYPE;
    MiniportAttributes.GeneralAttributes.PhysicalMediumType = MP_PHYSICAL_MEDIA_TYPE;    
    MiniportAttributes.GeneralAttributes.MtuSize = MP_802_11_MAX_FRAME_SIZE - MP_802_11_SHORT_HEADER_SIZE;
    MiniportAttributes.GeneralAttributes.MaxXmitLinkSpeed = HW11_LINK_SPEED;
    MiniportAttributes.GeneralAttributes.MaxRcvLinkSpeed = HW11_LINK_SPEED;
    MiniportAttributes.GeneralAttributes.XmitLinkSpeed = NDIS_LINK_SPEED_UNKNOWN;
    MiniportAttributes.GeneralAttributes.RcvLinkSpeed = NDIS_LINK_SPEED_UNKNOWN;
    MiniportAttributes.GeneralAttributes.MediaConnectState = MediaConnectStateConnected;
    MiniportAttributes.GeneralAttributes.MediaDuplexState = MediaDuplexStateFull;
    MiniportAttributes.GeneralAttributes.LookaheadSize = MP_802_11_MAXIMUM_LOOKAHEAD;
    MiniportAttributes.GeneralAttributes.PowerManagementCapabilities = &PnpCapabilities;
    Hw11QueryPnPCapabilities(pAdapter->Nic, MiniportAttributes.GeneralAttributes.PowerManagementCapabilities);
    
    MiniportAttributes.GeneralAttributes.MacOptions = NDIS_MAC_OPTION_COPY_LOOKAHEAD_DATA | 
                                         NDIS_MAC_OPTION_TRANSFERS_NOT_PEND |
                                         NDIS_MAC_OPTION_NO_LOOPBACK;
    
    MiniportAttributes.GeneralAttributes.SupportedPacketFilters = NDIS_PACKET_TYPE_DIRECTED |
                                                     NDIS_PACKET_TYPE_MULTICAST |
                                                     NDIS_PACKET_TYPE_ALL_MULTICAST |
                                                     NDIS_PACKET_TYPE_BROADCAST;
    
    MiniportAttributes.GeneralAttributes.MaxMulticastListSize = HW11_MAX_MCAST_LIST_SIZE;
    MiniportAttributes.GeneralAttributes.MacAddressLength = ETH_LENGTH_OF_ADDRESS;
    
    NdisMoveMemory(
        &MiniportAttributes.GeneralAttributes.PermanentMacAddress,
        pAdapter->PermanentAddress,
        ETH_LENGTH_OF_ADDRESS
        );
    
    NdisMoveMemory(
        &MiniportAttributes.GeneralAttributes.CurrentMacAddress,
        pAdapter->CurrentAddress,
        ETH_LENGTH_OF_ADDRESS
        );
    
    MiniportAttributes.GeneralAttributes.RecvScaleCapabilities = NULL;
    MiniportAttributes.GeneralAttributes.AccessType = NET_IF_ACCESS_BROADCAST;
    MiniportAttributes.GeneralAttributes.DirectionType = NET_IF_DIRECTION_SENDRECEIVE;
    MiniportAttributes.GeneralAttributes.IfType = IF_TYPE_IEEE80211;
    MiniportAttributes.GeneralAttributes.IfConnectorPresent = TRUE;

    MiniportAttributes.GeneralAttributes.DataBackFillSize = HW11_REQUIRED_BACKFILL_SIZE;
    MpQuerySupportedOidsList(
        &MiniportAttributes.GeneralAttributes.SupportedOidList,
        &MiniportAttributes.GeneralAttributes.SupportedOidListLength
        );
    
    //
    // Register the Generic miniport attributes with NDIS
    //
    return NdisMSetMiniportAttributes(
        pAdapter->MiniportAdapterHandle,
        &MiniportAttributes
        );
}


INLINE VOID
MpResetPhyMIBs(
    __in  PADAPTER    pAdapter
    )
{
    Hw11ResetPhyMIBs(pAdapter->Nic);
}

INLINE VOID
MpResetMacMIBs(
    __in  PADAPTER    pAdapter
    )
{
    Hw11ResetMacMIBs(pAdapter->Nic);

    ETH_COPY_NETWORK_ADDRESS(pAdapter->CurrentAddress, pAdapter->PermanentAddress);
    pAdapter->OperationMode     = DOT11_OPERATION_MODE_EXTENSIBLE_STATION;
    pAdapter->State             = INIT_STATE;
    pAdapter->Dot11RunningMode  = RUNNING_MODE_UNKNOWN;
    pAdapter->MaxRxLifeTime     = 512;

    MP_CLEAR_STATUS_FLAG(pAdapter, MP_ADAPTER_NETWORK_MONITOR_MODE);

    // Choose the correct receive processing handler
    MpAdjustReceiveHandler(pAdapter);

    //
    // Reset all counters. Some of them are not MIBs but we will reset
    // them anyways
    //
    pAdapter->TotalTransmits = 0;
    pAdapter->TotalTransmitsSnapShot = 0;
    pAdapter->NumStalledSendTicks = 0;
    pAdapter->TotalGoodTransmits = 0;
    pAdapter->TotalGoodReceives = 0;
    pAdapter->ReassemblyFailedReceives = 0;
    pAdapter->TotalBadTransmits = 0;
    pAdapter->TotalBadReceives = 0;
    pAdapter->MPDUMaxLength = Hw11GetMPDUMaxLength(pAdapter->Nic);
}

NDIS_STATUS
MPInitialize(
    __in  NDIS_HANDLE                        MiniportAdapterHandle,
    __in  NDIS_HANDLE                        MiniportDriverContext,
    __in  PNDIS_MINIPORT_INIT_PARAMETERS     MiniportInitParameters
    )
/*++
Routine Description:

    The MiniportInitialize function is a required function. Here is the
    list of things this function typically performs:

        Set the miniport attributes.
        Read configuration parameters from the registry.
        Allocate memory.
        Allocate the NET_BUFFER_LIST pool and the NET_BUFFER pool.
        Do hardware specific initialization like registering interrupt handlers/ set DMA resources etc.
        Read and write to the bus-specific configuration space.

    MiniportInitialize runs at IRQL = PASSIVE_LEVEL.

Arguments:

    MiniportAdapterHandle   The handle NDIS uses to refer to us
    MiniportDriverContext   Handle passed to NDIS when we registered the driver
    MiniportInitParameters  Initialization parameters contains a pointer
                            to a list of the allocated hardware resources
                            for the miniport adapter

Return Value:

    NDIS_STATUS_xxx code

--*/
{
    NDIS_STATUS         ndisStatus = NDIS_STATUS_SUCCESS;
    PADAPTER            pAdapter = NULL;
    NDIS_MINIPORT_ADAPTER_ATTRIBUTES    MiniportAttributes;                             
    BOOLEAN                        bHw11Started;
    BOOLEAN                        bSta11Started;
    BOOLEAN                        bHw11Initialized;
    NDIS_ERROR_CODE                 ErrorCode = NDIS_STATUS_SUCCESS;    
    ULONG                           ErrorValue = 0;
    BOOLEAN                            bSendEngineInitialized = FALSE, bReceiveEngineInitialized = FALSE;
    BOOLEAN                             bCustomInterfacesInitialized = FALSE;

    UNREFERENCED_PARAMETER(MiniportDriverContext);
    UNREFERENCED_PARAMETER(MiniportInitParameters);

    PAGED_CODE();

    bHw11Started = FALSE;
    bSta11Started = FALSE;
    bHw11Initialized = FALSE;
    MpTrace(COMP_INIT_PNP,DBG_TRACE, ("---> MPInitialize\n"));
    MpTrace(COMP_INIT_PNP, DBG_LOUD, ("MiniportAdapterHandle: %p\n", MiniportAdapterHandle));
        
    do {
        //
        // Allocate adapter context structure and initialize all the
        // memory resources for sending and receiving packets.
        //
        ndisStatus = MpAllocateAdapter (MiniportAdapterHandle, &pAdapter);
        if(ndisStatus != NDIS_STATUS_SUCCESS)
        {
            pAdapter = NULL;
            break;
        }
        
        pAdapter->MiniportAdapterHandle = MiniportAdapterHandle;

        //
        // Set the miniport registration attributes with NDIS
        //
        NdisZeroMemory(&MiniportAttributes, sizeof(MiniportAttributes));
        MiniportAttributes.RegistrationAttributes.Header.Type = NDIS_OBJECT_TYPE_MINIPORT_ADAPTER_REGISTRATION_ATTRIBUTES;
        MiniportAttributes.RegistrationAttributes.Header.Revision = NDIS_MINIPORT_ADAPTER_REGISTRATION_ATTRIBUTES_REVISION_1;
        MiniportAttributes.RegistrationAttributes.Header.Size = sizeof(NDIS_MINIPORT_ADAPTER_REGISTRATION_ATTRIBUTES);
        MiniportAttributes.RegistrationAttributes.MiniportAdapterContext = pAdapter;
        MiniportAttributes.RegistrationAttributes.AttributeFlags = NDIS_MINIPORT_ATTRIBUTES_NDIS_WDM ;
        MiniportAttributes.RegistrationAttributes.CheckForHangTimeInSeconds = 64;//HW11_CHECK_FOR_HANG_TIME_IN_SECONDS;
        MiniportAttributes.RegistrationAttributes.InterfaceType = HW11_BUS_INTERFACE_TYPE;    

        NdisMSetMiniportAttributes(
            MiniportAdapterHandle,
            &MiniportAttributes
            );
#ifdef USE_SAMPLE_PACKET_BUFFERING_IMPLEMENTATION
        //
        // We precalculate this value as we will be using it frequently in the driver
        //
        pAdapter->NumTicksPer100TU = ((10240 * 100) / KeQueryTimeIncrement());
        
#endif
        MP_SET_FLAG(pAdapter, fMP_INIT_IN_PROGRESS);

        //
        // Read Advanced configuration information from the registry
        //

        MpReadRegistryConfiguration(
                    pAdapter
                    ); 
        //
        // Find the adapter on the bus and read its bus configuration space
        //
        ndisStatus = Hw11FindAdapterAndResources(pAdapter->Nic, &ErrorCode, &ErrorValue);
        if (ndisStatus != NDIS_STATUS_SUCCESS)
        {
            break;
        }
        //
        // Initialize the hardware.
        //
        ndisStatus = Hw11InitializeNic(pAdapter->Nic, pAdapter->WdfDevice);
        if(ndisStatus != NDIS_STATUS_SUCCESS)
        {
            MpTrace(COMP_INIT_PNP, DBG_SERIOUS, ("Failed to initialize NIC successfully.\n"));
            break;
        } 

        //
        // Fill the Miniport 802.11 Attributes, we can do so as soon as NIC is initialized.
        //
        ndisStatus = MpSet80211Attributes(pAdapter);
        if (ndisStatus != NDIS_STATUS_SUCCESS)
        {
            MpTrace(COMP_INIT_PNP, DBG_SERIOUS, ("Failed to set 80211 attributes\n"));
            break;
        }
        //
        // Initialize the Mp Send Engine. This will also initialize Hw11 Send engine
        // 
        ndisStatus = MpInitializeSendEngine(pAdapter);
        if (ndisStatus != NDIS_STATUS_SUCCESS)
        {
            MpTrace(COMP_INIT_PNP, DBG_SERIOUS, ("Failed to initialize the Send Engine\n"));
            break;
        }
        bSendEngineInitialized = TRUE;

        //
        // Initialize the Hw11 Receive Engine. This will also initialize Mp Receive engine
        // 
        ndisStatus = Hw11InitializeReceive(pAdapter->Nic, pAdapter->TotalRxMSDU);
        if (ndisStatus != NDIS_STATUS_SUCCESS)
        {
            MpTrace(COMP_INIT_PNP, DBG_SERIOUS, ("Failed to initialize the Receive Engine\n"));
            break;
        }
        bReceiveEngineInitialized = TRUE;

        //
        // Initialize Custom Interfaces for this miniport driver
        //
        ndisStatus = MpEventInitialize(pAdapter);
        if (ndisStatus != NDIS_STATUS_SUCCESS)
        {
            MpTrace(COMP_INIT_PNP, DBG_SERIOUS, ("Failed to initialize the Custom Interfaces\n"));
            break;
        }
        
       
        pAdapter->AdapterState = NicRunning;  
        //
        // Start the NIC. If anything fails after this point,
        // we must issue a Halt to the NIC before returning
        // from initialize
        //
        ndisStatus = Hw11StartNic(pAdapter->Nic);
        if(ndisStatus != NDIS_STATUS_SUCCESS)
        {
            MpTrace(COMP_INIT_PNP, DBG_SERIOUS, ("Failed to start NIC successfully.\n"));
            break;
        }
        bHw11Started = TRUE;
        //
        //register an ioctl interface
        //vm control
        //
        ndisStatus = NICRegisterDevice(NdisMiniportDriverHandle, pAdapter);
        if (!NT_SUCCESS(ndisStatus)) {
            MpTrace(COMP_INIT_PNP, DBG_SERIOUS, ("NdisProtCreateControlDevice failed with status 0x%x\n", ndisStatus));
            break;
        }
        //
        // Initialize the MIB values we maintain at MP level
        //
        ETH_COPY_NETWORK_ADDRESS(pAdapter->PermanentAddress, Hw11GetMACAddress(pAdapter->Nic));

        MpResetMacMIBs(pAdapter);
        MpResetPhyMIBs(pAdapter);
        //
        // Fill the Miniport Generic Attributes
        //
        ndisStatus = MpSetGeneralAttributes(pAdapter);
        if (ndisStatus != NDIS_STATUS_SUCCESS)
        {
            MpTrace(COMP_INIT_PNP, DBG_SERIOUS, ("Failed to set general attributes\n"));
            break;
        }

        //
        // The miniport will start in the Paused PnP state        
        // MpRestart is called after MpInitialize so the state wil change to MINIPORT_RUNNING there
        //
        MP_SET_NDIS_PNP_STATE(pAdapter, MINIPORT_PAUSED);

        //
        // We are started as paused. Add the paused counter. When we restart
        // again, we will be adding refcounts for active receives, pending
        // SG operations, etc
        //
        MP_INCREMENT_RESET_PAUSE_COUNT(pAdapter);        

        //
        // This Ref count should be incremented if there are any
        // operations outstanding on this miniport that should prevent
        // miniport from halting
        //
        MP_INCREMENT_REF(pAdapter);
        MpTrace(COMP_INIT_PNP, DBG_LOUD, ("Starting USB pipes!\n"));
        Hw11EnableNotification(pAdapter->Nic);
        //
        // Run NIC self-test if it is needed
        //
        ndisStatus = Hw11SelfTest(pAdapter->Nic);
        if(ndisStatus != NDIS_STATUS_SUCCESS)
        {
            Hw11StopNotification(pAdapter->Nic);
            MpTrace(COMP_INIT_PNP, DBG_SERIOUS, ("Hardware failed self test. Could not initialize\n"));
            break;
        }

        //
        // The NIC is ready to go, initialize the station
        //
        ndisStatus = Sta11StartStation(pAdapter->Station);
        if(ndisStatus != NDIS_STATUS_SUCCESS)
        {
            Hw11StopNotification(pAdapter->Nic);
            MpTrace(COMP_INIT_PNP, DBG_SERIOUS, ("Station failed initialize\n"));
            break;
        }
        bSta11Started = TRUE;
    } WHILE (FALSE);

    if(pAdapter && ndisStatus != NDIS_STATUS_SUCCESS) {
        MpTrace(COMP_INIT_PNP, DBG_SERIOUS, ("MPInitialize failed! Error code: 0x%08x\n", ndisStatus));
        
        //
        // Initialization failed. Free up any resources that are currently allocated
        //
        if (pAdapter)
        {
            if (bSta11Started) {                              // Stop the station
                Sta11StopStation(pAdapter->Station, NdisHaltDeviceInitializationFailed);       
            }
            if (bHw11Initialized) {            
                Hw11TerminateNic(pAdapter->Nic);           // Uninitialize the NIC
            }
            if (bReceiveEngineInitialized) {            
                Hw11TerminateReceive(pAdapter->Nic);       // Free all receive resources
            }
            if (bSendEngineInitialized) {            
                MpTerminateSendEngine(pAdapter);            // Free all send resources
            }
            if(bCustomInterfacesInitialized) {            
                MpEventTerminate(pAdapter); 
            }
            //
            // Now free allocated resources. This also frees resources allocated by NIC
            //
            MpFreeAdapter(pAdapter);
        }

        if (ErrorCode != NDIS_STATUS_SUCCESS)
        {
            NdisWriteErrorLogEntry(
                MiniportAdapterHandle,
                ErrorCode,
                1,
                ErrorValue
                );
        }
        
    }

    MpTrace(COMP_INIT_PNP,DBG_TRACE, ("<--- MPInitialize ndisStatus = 0x%08x%\n", ndisStatus));

    return ndisStatus;

}

INLINE VOID
MpOnSurpriseRemoval(
    __in  PADAPTER        pAdapter
    )
{
    //
    // Set the surprise removal flag. We need to remember this
    // in case other requests get submitted to the miniport
    //
    MP_SET_STATUS_FLAG(pAdapter, MP_ADAPTER_SURPRISE_REMOVED);

    MP_ACQUIRE_SEND_LOCK(pAdapter, FALSE);

    //
    // Any NBL that may be pending in the hardware or driver
    // must be failed back to NDIS with the appropriate error code
    //
    MpCompleteQueuedTxMSDUs(pAdapter);
    
    MpCompleteQueuedNBL(pAdapter);

    MP_RELEASE_SEND_LOCK(pAdapter, FALSE);
}


VOID
MPHalt(
    __in  NDIS_HANDLE             MiniportAdapterContext,
    __in  NDIS_HALT_ACTION        HaltAction
    )
/*++

Routine Description:

    Halt handler is called when NDIS receives IRP_MN_STOP_DEVICE,
    IRP_MN_SUPRISE_REMOVE or IRP_MN_REMOVE_DEVICE requests from the
    PNP manager. Here, the driver should free all the resources acquired
    in MiniportInitialize and stop access to the hardware. NDIS will
    not submit any further request once this handler is invoked.

    1) Free and unmap all I/O resources.
    2) Disable notification(interrupts) and deregister notification(interrupt) handler.
    3) Cancel all queued up timer callbacks.
    4) Finally wait indefinitely for all the outstanding receive
        packets indicated to the protocol to return.

    MiniportHalt runs at IRQL = PASSIVE_LEVEL.


Arguments:

    MiniportAdapterContext  Pointer to our adapter
    HaltAction              The reason adapter is being halted

Return Value:

    None.

--*/
{
    
//    BOOLEAN           bDone=TRUE;
//    LONG              nHaltCount = 0;
    PADAPTER          pAdapter = (PADAPTER) MiniportAdapterContext;
    ULONG           CurrentStatus;

//    UNREFERENCED_PARAMETER(HaltAction);
//    UNREFERENCED_PARAMETER(MiniportAdapterContext);

    MpTrace(COMP_INIT_PNP,DBG_NORMAL, ("---> MPHalt\n"));

    MpTrace(COMP_INIT_PNP,DBG_SERIOUS, ("Calling MpHalt with action 0x%x\n",HaltAction));
    
    PAGED_CODE();

    MP_SET_FLAG(pAdapter, fMP_HALT_IN_PROGRESS);

    MP_CLEAR_FLAG(pAdapter, fMP_POST_WRITES);
    MP_CLEAR_FLAG(pAdapter, fMP_POST_READS);
    
    //
    // Mark the miniport as going in halt
    //
    CurrentStatus = MP_SET_STATUS_FLAG(pAdapter, MP_ADAPTER_HALT_IN_PROGRESS);
    if (CurrentStatus & MP_ADAPTER_RESET_IN_PROGRESS)
    {
        //
        // A reset operation is currently occuring. We cannot halt till it completes
        //

        // TODO: Determine if I need to wait for reset to finish or NDIS guarantees
        // halt doesnt happen with reset
        while (MP_TEST_STATUS_FLAG(pAdapter, MP_ADAPTER_RESET_IN_PROGRESS) == TRUE)
        {
            NdisMSleep(20 * 1000);      // 20 seconds
        }
    }
    
    MPASSERTMSG("No sends should have been pended on the miniport when halt is called\n",
                  (pAdapter->PendingSends == 0));
    
    //
    // Scan should not be in progress during halt
    //
    MPASSERT(MP_DOT11_SCAN_IN_PROGRESS(pAdapter) == FALSE);

    //
    // Unregister the ioctl interface.  vm control
    //
    NICDeregisterDevice(pAdapter);
    
    //
    // First issue a shutdown turn off hardware
    //
    MPAdapterShutdown(MiniportAdapterContext, NdisShutdownPowerOff);
    
    //
    // PCI -- Deregister interrupts as soon as possible
    //   
    
    if (MP_TEST_STATUS_FLAG(pAdapter, MP_ADAPTER_SURPRISE_REMOVED) == FALSE)
    {
         //
        // Pause the beaconing
        //
        Hw11StopAdHoc(pAdapter->Nic);
    }
    
    //
    // Drop any Rx MSDUs that have exceeded RxLifeTime value. We cannot
    // free all receive resources without this.
    // TODO: Can this be done in MPPause?
    MpExpireReassemblingPackets(pAdapter, FALSE);
    
    //
    // Invoke the terminate event
    //
    MpEventTerminate(pAdapter);

    //
    // Stop the station
    //
    Sta11StopStation(pAdapter->Station, HaltAction);
    
    //
    // If ref count is not zero yet that implies:
    // 1. A pending Async Shared memory allocation
    // 2. We are in the middle of a scan
    //
    MP_DECREMENT_REF(pAdapter);
    if (pAdapter->RefCount > 0)
    {
        do
        {
            if(NdisWaitEvent(&pAdapter->HaltWaitEvent, 2000))
            {
                break;
            }

            MpTrace(COMP_INIT_PNP, DBG_SERIOUS, ("Waiting 2 more seconds for all Miniport Ref Count to go to zero\n"));
        } while(TRUE);
    }
    MPVERIFY(pAdapter->NumActiveReceivers == 0);
    
    //
    // By the time Halt is called as indicated received should have been returned
    //
    MPASSERT(pAdapter->PendingReceives == 0);

    Hw11StopNotification(pAdapter->Nic);
    if (MP_TEST_STATUS_FLAG(pAdapter, MP_ADAPTER_SURPRISE_REMOVED) == FALSE)
    {
        //
        // Inform the hardware that it is being halted.
        //
        Hw11HaltNic(pAdapter->Nic);
    }
    //
    // Release all resources kept for send and receive
    //
    Hw11TerminateReceive(pAdapter->Nic);
    MpTerminateSendEngine(pAdapter);
    //
    // Uninitialize the NIC
    //
    Hw11TerminateNic(pAdapter->Nic);
    
    //
    // Free adapter resources. Also frees up NIC resources.
    //
    MpFreeAdapter(pAdapter);

    MpTrace(COMP_INIT_PNP,DBG_NORMAL, ("<--- MPHalt\n"));
}

NDIS_STATUS
MpGetAdapterStatus(
    __in PADAPTER         pAdapter
    )
{
    NDIS_STATUS ndisStatus;

    if (MP_TEST_STATUS_FLAG(pAdapter, MP_ADAPTER_NDIS_PAUSE_IN_PROGRESS))
        ndisStatus = NDIS_STATUS_PAUSED;
    else if (MP_TEST_STATUS_FLAG(pAdapter, MP_ADAPTER_RESET_IN_PROGRESS))
        ndisStatus = NDIS_STATUS_RESET_IN_PROGRESS;
    else if (MP_TEST_STATUS_FLAG(pAdapter, MP_ADAPTER_HALT_IN_PROGRESS))
        ndisStatus = NDIS_STATUS_CLOSING;
    else if (MP_TEST_STATUS_FLAG(pAdapter, MP_ADAPTER_SURPRISE_REMOVED))
        ndisStatus = NDIS_STATUS_CLOSING;
    else if (MP_TEST_STATUS_FLAG(pAdapter, MP_ADAPTER_REMOVE_IN_PROGRESS))
        ndisStatus = NDIS_STATUS_CLOSING;
    else if MP_TEST_STATUS_FLAG(pAdapter, MP_ADAPTER_HARDWARE_ERROR)
        ndisStatus = NDIS_STATUS_DEVICE_FAILED;
    else
        ndisStatus = NDIS_STATUS_FAILURE;       // return a generc error

    return ndisStatus;
}


INLINE 
BOOLEAN
MpRemoveAdapter(
    __in PADAPTER pAdapter
    )
{
    if (!(MP_SET_CLEAR_STATUS_FLAG(
            pAdapter,
            MP_ADAPTER_REMOVE_IN_PROGRESS,      // Set this bit
            MP_ADAPTER_RESET_IN_PROGRESS        // Clear this bit
            ) & MP_ADAPTER_REMOVE_IN_PROGRESS))  // Test this bit
    {
        //
        // Request a removal.
        //
        NdisWriteErrorLogEntry(
            pAdapter->MiniportAdapterHandle,
            NDIS_ERROR_CODE_HARDWARE_FAILURE,
            1,
            ERRLOG_REMOVE_MINIPORT
            );
        
        NdisMRemoveMiniport(pAdapter->MiniportAdapterHandle);        
        return TRUE;
    }
    else
    {
        //
        // Adapter is already in removal. No need to request one.
        //
        return FALSE;
    }
}


NDIS_STATUS
MPReset(
    __in  NDIS_HANDLE     MiniportAdapterContext,
    __out PBOOLEAN        AddressingReset
    )
/*++

Routine Description:

    An NDIS 6.0 miniport driver's reset function can cancel pending OID
    requests and send requests in the context of a reset.
    NDIS no longer performs such cancellations. Alternatively,
    the miniport driver can complete pending OID requests
    and send requests after the reset completes.

    MiniportReset function can be called at either IRQL PASSIVE_LEVEL
    or IRQL = DISPATCH_LEVEL.

Arguments:

    AddressingReset         To let NDIS know whether we need help from it with our reset
    MiniportAdapterContext  Pointer to our adapter

Return Value:

    NDIS_STATUS_SUCCESS
    NDIS_STATUS_PENDING
    NDIS_STATUS_RESET_IN_PROGRESS
    NDIS_STATUS_HARD_ERRORS

Note:

--*/
{
    PADAPTER       pAdapter = (PADAPTER) MiniportAdapterContext;
    NDIS_STATUS    ndisStatus = NDIS_STATUS_PENDING;
    ULONG           CurrentStatus;

//    UNREFERENCED_PARAMETER(AddressingReset);
//    UNREFERENCED_PARAMETER(MiniportAdapterContext);
    
    MpTrace(COMP_INIT_PNP,DBG_TRACE, ("---> MPReset\n"));


    MpTrace(COMP_MISC, DBG_SERIOUS, ("The miniport driver has been reset\n"));
    
    //
    // Request NDIS to add the addresses on the miniport through OIDs
    // once the reset is complete.
    //
    *AddressingReset = HW11_REQUIRES_ADDRESS_RESET;
    
    MP_ACQUIRE_SEND_LOCK(pAdapter, FALSE);
    
    do
    {
        CurrentStatus = MP_SET_STATUS_FLAG(pAdapter, MP_ADAPTER_RESET_IN_PROGRESS);
                
        if (CurrentStatus & MP_ADAPTER_RESET_IN_PROGRESS)
        {
            //
            // The "in reset" bit is already set. Tell NDIS we are already in reset and
            // return immediately.
            //
            ndisStatus = NDIS_STATUS_RESET_IN_PROGRESS;
            break;
        }
        
        //
        // If our halt handler has been called, we should not reset
        //
        if (CurrentStatus & MP_ADAPTER_HALT_IN_PROGRESS)
        {
            MPASSERT(FALSE);    // Would be an interesting scenario to investigate
            MP_CLEAR_STATUS_FLAG(pAdapter, MP_ADAPTER_RESET_IN_PROGRESS);
            ndisStatus = NDIS_STATUS_SUCCESS;
            break;
        }
        
        //
        // If the adapter has Non-Recoverable hardware errors, request removal
        //
        if (CurrentStatus & MP_ADAPTER_NON_RECOVER_ERROR)
        {
            MP_CLEAR_STATUS_FLAG(pAdapter, MP_ADAPTER_RESET_IN_PROGRESS);
            ndisStatus = NDIS_STATUS_HARD_ERRORS;
            MpRemoveAdapter(pAdapter);
            break;
        }
    } while(FALSE);
    
    MP_RELEASE_SEND_LOCK(pAdapter, FALSE);
    
    if (ndisStatus == NDIS_STATUS_PENDING)
    {
        MpTrace(COMP_MISC, DBG_NORMAL, ("Scheduling a work item to complete the reset\n"));

        WdfWorkItemEnqueue(pAdapter->NdisResetWorkItem);
    }
    MpTrace(COMP_INIT_PNP,DBG_TRACE, ("<--- MPReset\n"));
    return ndisStatus;
}

VOID
DriverUnload(
    __in  PDRIVER_OBJECT  DriverObject
    )
/*++

Routine Description:

    The unload handler is called during driver unload to free up resources
    acquired in DriverEntry. Note that an unload handler differs from
    a MiniportHalt function in that the unload handler has a more global
    scope, whereas the scope of the MiniportHalt function is restricted
    to a particular miniport driver instance.

    During an unload operation, an NDIS 6.0 miniport driver must deregister
    the miniport driver by calling NdisMDeregisterMiniportDriver
    and passing the MiniportDriverHandle as a parameter.
    The driver obtained the MiniportDriverHandle when it called
    NdisMRegisterMiniportDriver.

    Runs at IRQL = PASSIVE_LEVEL.

Arguments:

    DriverObject        Not used

Return Value:

    None

--*/
{
    MpTrace(COMP_INIT_PNP,DBG_TRACE, ("--> DriverUnload\n"));

    UNREFERENCED_PARAMETER(DriverObject);

    PAGED_CODE();

    WdfDriverMiniportUnload(WdfGetDriver());

    NdisMDeregisterMiniportDriver(NdisMiniportDriverHandle);

    #if DOT11_TRACE_ENABLED
        WPP_CLEANUP(DriverObject);
    #endif
    MpTrace(COMP_INIT_PNP,DBG_TRACE, ("<--- DriverUnload\n"));
}

BOOLEAN
MPCheckForHang(
    __in NDIS_HANDLE MiniportAdapterContext
    )
/*++

Routine Description:

    The MiniportCheckForHang handler is called to report the state of the
    NIC, or to monitor the responsiveness of an underlying device driver.
    This is an optional function. If this handler is not specified, NDIS
    judges the driver unresponsive when the driver holds
    MiniportQueryInformation or MiniportSetInformation requests for a
    time-out interval (deafult 4 sec), and then calls the driver's
    MiniportReset function. A NIC driver's MiniportInitialize function can
    extend NDIS's time-out interval by calling NdisMSetAttributesEx to
    avoid unnecessary resets.

    Always runs at IRQL = DISPATCH_LEVEL.

Arguments:

    MiniportAdapterContext  Pointer to our adapter

Return Value:

    TRUE    NDIS calls the driver's MiniportReset function.
    FALSE   Everything is fine

Note:
    CheckForHang handler is called in the context of a timer DPC.
    take advantage of this fact when acquiring/releasing spinlocks

--*/
{
    PADAPTER        pAdapter = (PADAPTER) MiniportAdapterContext;
    BOOLEAN         bNeedReset = FALSE;
//    UNREFERENCED_PARAMETER(MiniportAdapterContext);
    
    do
    {
        //
        // Invoke MpEventCheckForHang. See if reset is needed.
        //
        if (MpEventCheckForHang(pAdapter))
        {
            MpTrace(COMP_MISC, DBG_SERIOUS, ("MpEventCheckForHang has requested a reset\n"));
            bNeedReset = TRUE;
            break;            
        }
        //
        // If a hardware error has occured, ask NDIS to reset us
        //
        if (MP_TEST_STATUS_FLAG(pAdapter, (MP_ADAPTER_NON_RECOVER_ERROR | MP_ADAPTER_HARDWARE_ERROR)))
        {
            MpTrace(COMP_MISC, DBG_SERIOUS, ("Requesting reset since a hardware error has occured\n"));
            bNeedReset = TRUE;
            break;
        }
        
        //
        // Check to see if packet are not getting sent. Ask for reset if stalled.
        //
        if (pAdapter->PendingSends > 0)
        {
            if (pAdapter->TotalTransmitsSnapShot == 0)
            {
                //
                // Take a snap shot of the total transmits done so far. Sampling begins now
                //
                MpTrace(COMP_MISC, DBG_LOUD, ("Restart monitoring the total transmit count\n"));
                pAdapter->TotalTransmitsSnapShot = pAdapter->TotalTransmits;
            }
            else if (pAdapter->TotalTransmitsSnapShot == pAdapter->TotalTransmits)
            {
                MpTrace(COMP_MISC, DBG_LOUD, ("%d sample periods of total transmits have occured\n", pAdapter->NumStalledSendTicks));
                
                //
                // If the number of sends pended on the Hw11 has not changed we will
                // count a tick interval of stalled send period.
                //
                pAdapter->NumStalledSendTicks++;

                MPASSERTMSG(
                    "If one more ChecForHang detects stalled sends, we will reset! Investigate\n",
                    pAdapter->NumStalledSendTicks != MP_SENDS_HAVE_STALLED_PERIOD - 1
                    );
                
                if(pAdapter->NumStalledSendTicks >= MP_SENDS_HAVE_STALLED_PERIOD)
                {
                    MpTrace(COMP_MISC, DBG_SERIOUS, ("Send Engine seems to be stalled. Requesting reset\n"));
                    MpTrace(COMP_TESTING, DBG_SERIOUS, ("Requesting reset from NDIS\n"));
                    MPASSERTMSG("Reset should not occur normally! Investigate\n", FALSE);
                    bNeedReset = TRUE;
                    break;
                }
            }
            else
            {
                //
                // Everything is ok. Pended sends have been completing.
                //
                MpTrace(COMP_MISC, DBG_LOUD, ("Transmissions are working properly.\n"));
                MPVERIFY(pAdapter->TotalTransmitsSnapShot < pAdapter->TotalTransmits);
                pAdapter->TotalTransmitsSnapShot = 0;
                pAdapter->NumStalledSendTicks = 0;
            }
        }
            
        //
        // Sample the usage of Rx MSDU list. Will be used during MpReturnPackets to determine
        // if we need to shrink the Rx MSDU list.
        //
        if (pAdapter->RxMSDUListSampleTicks < MP_RX_MSDU_LIST_SAMPLING_PERIOD)
        {
            pAdapter->RxMSDUListSampleTicks++;
            pAdapter->NumRxMSDUNotUtilized += (pAdapter->TotalRxMSDUAllocated - pAdapter->PendingReceives);
        }
        else
        {
            MpTrace(COMP_MISC, DBG_LOUD, ("Percentage of Under Utilization = %d\n", (pAdapter->NumRxMSDUNotUtilized * 100) / (pAdapter->RxMSDUListSampleTicks * pAdapter->TotalRxMSDUAllocated)));
            pAdapter->RxMSDUListSampleTicks = 0;
            pAdapter->NumRxMSDUNotUtilized = 0;
        }
        MpTrace(COMP_MISC, DBG_FUNCTION, ("%d sample periods of Rx MSDU list have passed\n", pAdapter->RxMSDUListSampleTicks));
    } while(FALSE);
    
    return bNeedReset;
}

VOID 
MpInternalPause(
    __in  PADAPTER                            pAdapter,
    __in  PNDIS_MINIPORT_PAUSE_PARAMETERS     MiniportPauseParameters
    )
{
    UNREFERENCED_PARAMETER(MiniportPauseParameters);
    
    MPVERIFY(MP_GET_NDIS_PNP_STATE(pAdapter) == MINIPORT_PAUSING);

    //
    // Notify hardware about pause. As an optimization, hardware can
    // turn off send receive units etc.
    //
    Hw11MiniportPause(pAdapter->Nic, MiniportPauseParameters);
    
    //
    // Send lock to serialize against other operations
    //
    MP_ACQUIRE_SEND_LOCK(pAdapter, FALSE);

    //
    // Cancel all pending sends
    //
    MpCompleteQueuedTxMSDUs(pAdapter);
    
    //
    // Complete all packets pending in the TxQueue
    //
    MpCompleteQueuedNBL(pAdapter);

    MP_RELEASE_SEND_LOCK(pAdapter, FALSE);

}

NDIS_STATUS
MPPause(
    __in  NDIS_HANDLE                         MiniportAdapterContext,
    __in  PNDIS_MINIPORT_PAUSE_PARAMETERS     MiniportPauseParameters
    )
/*++

Routine Description:

    Miniport pause and restart are new features introduced in NDIS 6.0.
    NDIS calls a miniport driver's MiniportPause function to stop
    data flow before a Plug and Play operation, such as adding or
    removing a filter driver or binding or unbinding a protocol driver,
    is performed. The adapter remains in the Pausing state until the
    pause operation has completed.

    When in the Paused state, a miniport driver can indicate status by
    calling the NdisMIndicateStatusEx function. In addition, the miniport
    driver should do the following:
    - Wait for all calls to the NdisMIndicateReceiveNetBufferLists
        function to return.
    - Wait for NDIS to return the ownership of all NET_BUFFER_LIST
        structures from outstanding receive indications to the miniport
        driver's MiniportReturnNetBufferLists function.
    - Complete all outstanding send requests by calling NdisMSendNetBufferListsComplete.
    - Reject all new send requests made to its MiniportSendNetBufferLists
        function by immediately calling the NdisMSendNetBufferListsComplete
        function. The driver should set the completion status in each
        NET_BUFFER_LIST structure to NDIS_STATUS_PAUSED.
    - Continue to handle OID requests in its MiniportOidRequest function.
    - Not stop the adapter completely if doing so prevents the driver
        from handling OID requests or providing status indications.
    - Not free the resources that the driver allocated during initialization.
    - Continue to handle DevicePnPEvent notifications.

    NDIS calls the miniport driver's MiniportRestart function to cause
    the miniport to return the adapter to the Running state.
    During the restart, the miniport driver must complete any tasks
    that are required to resume send and receive operations before
    completing the restart request.

Argument:

    MiniportAdapterContext  Pointer to our adapter

Return Value:

    NDIS_STATUS_SUCCESS
    NDIS_STATUS_PENDING

NOTE: A miniport can't fail the pause request

--*/
{
    PADAPTER         pAdapter = (PADAPTER) MiniportAdapterContext;
    NDIS_STATUS      ndisStatus;
    ULONG           CurrentStatus, uReceiveCount;

    UNREFERENCED_PARAMETER(MiniportPauseParameters);
    UNREFERENCED_PARAMETER(pAdapter);

    MpTrace(COMP_INIT_PNP,DBG_TRACE, ("---> MPPause\n"));

    MP_VERIFY_PASSIVE_IRQL();
    
    MPASSERT(MP_GET_NDIS_PNP_STATE(pAdapter) == MINIPORT_RUNNING);


    MP_ACQUIRE_RESET_PNP_LOCK(pAdapter);
    
    do
    {
        //
        // Acquire the lock and make sure that all sends will fail from this point on
        //
        MP_ACQUIRE_SEND_LOCK(pAdapter, FALSE);

        //
        // Mark adapter as going in pause. The lock helps synchronize with send operations
        //
        MP_SET_NDIS_PNP_STATE(pAdapter, MINIPORT_PAUSING);
        CurrentStatus = MP_SET_STATUS_FLAG(pAdapter, MP_ADAPTER_NDIS_PAUSE_IN_PROGRESS);
        
        MP_RELEASE_SEND_LOCK(pAdapter, FALSE);

        //
        // If a scan operation is in progress, cancel it first! All kinds
        // of sends and receives must cease.
        //
        if (MP_TEST_STATUS_FLAG(pAdapter, MP_ADAPTER_HW_IS_SCANNING))
        {
            Hw11CancelScan(pAdapter->Nic);
        }
        MPVERIFY(!MP_TEST_STATUS_FLAG(pAdapter, MP_ADAPTER_HW_IS_SCANNING));

        //
        // If we are already performing a Reset/Dot11Reset, the active
        // senders, etc is down
        //
        if (MP_INCREMENT_RESET_PAUSE_COUNT(pAdapter) == 1)
        {
            //
            // Remove the Active Sender count we added during MPRestart
            //
            MP_DECREMENT_ACTIVE_SENDERS(pAdapter);
            NdisWaitEvent(&pAdapter->ActiveSendersDoneEvent, MP_NEVER_TIMEOUT);
            
            //
            // Remove the Active Receivers count added during Restart.
            // We can pause when the count reaches 1 or less.
            //
            MP_DECREMENT_ACTIVE_RECEIVERS(pAdapter);
            while(pAdapter->NumActiveReceivers >= 1)
            {
                NdisMSleep(10 * 1000);  // 10 sec
            }
        }
        
        //
        // Remove the ref we added in MPRestart on receives indicated up to OS
        //
        if ((uReceiveCount = MP_DECREMENT_PENDING_RECEIVES(pAdapter)) == 0)
        {
            NdisSetEvent(&pAdapter->AllReceivesHaveReturnedEvent);
        }
        NdisWaitEvent(&pAdapter->AllReceivesHaveReturnedEvent, MP_NEVER_TIMEOUT);
        
        //
        // Pause the miniport by dropping pending send packets etc.
        //
        MpInternalPause(pAdapter, MiniportPauseParameters);
        
        //
        // Set the miniport state as paused
        // 
        MP_SET_NDIS_PNP_STATE(pAdapter, MINIPORT_PAUSED);
        
        ndisStatus = NDIS_STATUS_SUCCESS;
    } while(FALSE);
    
    MP_RELEASE_RESET_PNP_LOCK(pAdapter);  

    MpTrace(COMP_INIT_PNP,DBG_TRACE, ("<--- MPPause 0x%x\n", ndisStatus));
    return ndisStatus;
}

NDIS_STATUS
MPRestart(
    __in  NDIS_HANDLE                         MiniportAdapterContext,
    __in  PNDIS_MINIPORT_RESTART_PARAMETERS   MiniportRestartParameters
    )
/*++

Routine Description:

    NDIS calls the miniport driver's MiniportRestart function to cause
    the miniport to return the adapter to the Running state.
    During the restart, the miniport driver must complete any tasks
    that are required to resume send and receive operations before
    completing the restart request.

Argument:

    MiniportAdapterContext  Pointer to our adapter

Return Value:

    NDIS_STATUS_SUCCESS
    NDIS_STATUS_PENDING  Can it return pending
    NDIS_STATUS_XXX      The driver fails to restart


--*/
{
    PADAPTER                  pAdapter = (PADAPTER)MiniportAdapterContext;
    PNDIS_RESTART_ATTRIBUTES     NdisRestartAttributes;
    //PNDIS_RESTART_GENERAL_ATTRIBUTES  NdisGeneralAttributes;
    NDIS_STATUS     ndisStatus;
    
    MP_VERIFY_PASSIVE_IRQL();

    MPASSERT(MP_GET_NDIS_PNP_STATE(pAdapter) == MINIPORT_PAUSED);

    MpTrace(COMP_INIT_PNP,DBG_TRACE, ("---> MPRestart\n"));
    NdisRestartAttributes = MiniportRestartParameters->RestartAttributes;

    //
    // Synchronize the Restart operation with Pause and Reset. 
    // This is not a spin lock as these functions are going to be called at 
    // PASSIVE_LEVEL only. 
    //
    MP_ACQUIRE_RESET_PNP_LOCK(pAdapter);

    do
    {
        //
        // Notify the hardware so it can undone any ops done during Pause
        //
        ndisStatus = Hw11MiniportRestart(pAdapter->Nic, MiniportRestartParameters);
        if (ndisStatus != NDIS_STATUS_SUCCESS)
        {
            MpTrace(COMP_INIT_PNP, DBG_SERIOUS, ("Hw11MiniportRestart failed with status 0x%08x\n", ndisStatus));
            break;
        }
        
        MP_ACQUIRE_SEND_LOCK(pAdapter, FALSE);

        //
        // If no Reset/Dot11Reset is pending, we can restart
        //
        if (MP_DECREMENT_RESET_PAUSE_COUNT(pAdapter) == 0)
        {
            //
            // This Ref Count will be removed on Pause. Needed to make sure no receives
            // are occuring when Pause occurs
            //
            MP_INCREMENT_ACTIVE_RECEIVERS(pAdapter);
            
            //
            // We always keep one reference on active senders unless we are pausing/halting
            // or resetting. This avoid repeated signal of the associated event. We will
            // be resetting that event as well
            //
            MP_INCREMENT_ACTIVE_SENDERS(pAdapter);
            NdisResetEvent(&pAdapter->ActiveSendersDoneEvent);
        }
        
        //
        // Increment the receive Ref count on the miniport. The reciprocal for this
        // call is in MiniportPause. This way we will not signal AllReceivesHaveReturned
        // event till MiniportPause is called.
        //
        MP_INCREMENT_PENDING_RECEIVES(pAdapter);
        NdisResetEvent(&pAdapter->AllReceivesHaveReturnedEvent);
        
        //
        // The miniport is now operational
        //
        MP_CLEAR_STATUS_FLAG(pAdapter, MP_ADAPTER_NDIS_PAUSE_IN_PROGRESS);
        MP_SET_NDIS_PNP_STATE(pAdapter, MINIPORT_RUNNING);
    
        MP_RELEASE_SEND_LOCK(pAdapter, FALSE);
    }while(FALSE);
    
    MP_RELEASE_RESET_PNP_LOCK(pAdapter);    MpTrace(COMP_INIT_PNP,DBG_TRACE, ("<--- MPRestart\n"));

    
    return ndisStatus;

}


VOID
MPPnPEventNotify(
    __in  NDIS_HANDLE             MiniportAdapterContext,
    __in  PNET_DEVICE_PNP_EVENT   NetDevicePnPEvent
    )
/*++

Routine Description:

    MPPnPEventNotify is to handle PnP notification messages.
    An NDIS miniport driver with a WDM lower edge should export a
    MiniportPnPEventNotify function so that it can be notified
    when its NIC is removed without prior notification through
    the user interface. When a miniport driver receives
    notification of a surprise removal, it should note internally
    that the device has been removed and cancel any pending Requests
    that it sent down to the underlying bus driver. After calling
    the MiniportPnPEventNotify function to indicate the surprise
    removal, NDIS calls the miniport’s MiniportHalt function.
    If the miniport driver receives any requests to send packets
    or query or set OIDs before its MiniportHalt function is
    called, it should immediately complete such requests with a
    status value of NDIS_STATUS_NOT_ACCEPTED.

    All NDIS 5.1 miniport drivers must export a MPPnPEventNotify
    function.

    Runs at IRQL = PASSIVE_LEVEL in the context of system thread.

    Available - NDIS5.1 (WinXP) and later.

Arguments:

    MiniportAdapterContext      Pointer to our adapter
    PnPEvent                    Self-explanatory

Return Value:

    None

--*/
{
    PADAPTER             Adapter;
    PNDIS_POWER_PROFILE     NdisPowerProfile;
    NDIS_DEVICE_PNP_EVENT   PnPEvent;
    PVOID                   InformationBuffer;
    ULONG                   InformationBufferLength;

    MpTrace(COMP_INIT_PNP,DBG_TRACE, ("---> MPPnPEventNotify\n"));


    Adapter = (PADAPTER)MiniportAdapterContext;
    PnPEvent = NetDevicePnPEvent->DevicePnPEvent;
    InformationBuffer = NetDevicePnPEvent->InformationBuffer;
    InformationBufferLength = NetDevicePnPEvent->InformationBufferLength;

    //
    // NDIS currently sends only SurpriseRemoved and
    // PowerProfileChange Notification events.
    //
    switch (PnPEvent)
    {
        case NdisDevicePnPEventQueryRemoved:
            //
            // Called when NDIS receives IRP_MN_QUERY_REMOVE_DEVICE.
            //
            MpTrace(COMP_INIT_PNP,DBG_LOUD, ("MPPnPEventNotify: NdisDevicePnPEventQueryRemoved\n"));
            break;

        case NdisDevicePnPEventRemoved:
            //
            // Called when NDIS receives IRP_MN_REMOVE_DEVICE.
            // NDIS calls MiniportHalt function after this call returns.
            //
            MpTrace(COMP_INIT_PNP,DBG_LOUD, ("MPPnPEventNotify: NdisDevicePnPEventRemoved\n"));
            break;

        case NdisDevicePnPEventSurpriseRemoved:
            //
            // Called when NDIS receives IRP_MN_SUPRISE_REMOVAL.
            // NDIS calls MiniportHalt function after this call returns.
            //
            MP_SET_FLAG(Adapter, fMP_SURPRISE_REMOVED);
            MpTrace(COMP_INIT_PNP,DBG_LOUD, ("MPPnPEventNotify: NdisDevicePnPEventSurpriseRemoved\n"));
            break;

        case NdisDevicePnPEventQueryStopped:
            //
            // Called when NDIS receives IRP_MN_QUERY_STOP_DEVICE. ??
            //
            MpTrace(COMP_INIT_PNP,DBG_LOUD, ("MPPnPEventNotify: NdisDevicePnPEventQueryStopped\n"));
            break;

        case NdisDevicePnPEventStopped:
            //
            // Called when NDIS receives IRP_MN_STOP_DEVICE.
            // NDIS calls MiniportHalt function after this call returns.
            //
            //
            MpTrace(COMP_INIT_PNP,DBG_LOUD, ("MPPnPEventNotify: NdisDevicePnPEventStopped\n"));
            break;

        case NdisDevicePnPEventPowerProfileChanged:
            //
            // After initializing a miniport driver and after miniport driver
            // receives an OID_PNP_SET_POWER notification that specifies
            // a device power state of NdisDeviceStateD0 (the powered-on state),
            // NDIS calls the miniport's MiniportPnPEventNotify function with
            // PnPEvent set to NdisDevicePnPEventPowerProfileChanged.
            //
            MpTrace(COMP_INIT_PNP,DBG_LOUD, ("MPPnPEventNotify: NdisDevicePnPEventPowerProfileChanged\n"));

            if(InformationBufferLength == sizeof(NDIS_POWER_PROFILE)) {
                NdisPowerProfile = (PNDIS_POWER_PROFILE)InformationBuffer;
                if(*NdisPowerProfile == NdisPowerProfileBattery) {
                    MpTrace(COMP_INIT_PNP,DBG_LOUD,
                        ("The host system is running on battery power\n"));
                }
                if(*NdisPowerProfile == NdisPowerProfileAcOnLine) {
                    MpTrace(COMP_INIT_PNP,DBG_LOUD,
                        ("The host system is running on AC power\n"));
               }
            }
            break;

        default:
            MpTrace(COMP_INIT_PNP,DBG_SERIOUS, ("MPPnPEventNotify: unknown PnP event %x \n", PnPEvent));
            break;
    }

    MpTrace(COMP_INIT_PNP,DBG_TRACE, ("<--- MPPnPEventNotify\n"));

}

NDIS_STATUS
MPSetOptions(
    NDIS_HANDLE  NdisDriverHandle,
    NDIS_HANDLE  DriverContext
    )
/*++

Routine Description:

    MiniportSetOptions function is called by NDIS to ask the
    miniport driver to register its optional handlers.
    A miniport driver must call NdisSetOptionalHandler 
    from its MiniportSetOptions function to register its
    optional handlers with NDIS.

Arguments:


Return Value:

    None

--*/
{
    NDIS_STATUS         ndisStatus;
    
    MpEntry;

    ndisStatus = Hw11SetOptionalServices(NdisDriverHandle, DriverContext);
    
    MpExit;
    
    return ndisStatus;
}

NDIS_STATUS
MPRequest(
    __in  NDIS_HANDLE         MiniportAdapterContext,
    __in  PNDIS_OID_REQUEST   NdisRequest
    )
/*++
Routine Description:

    NDIS calls a miniport driver's MiniportOidRequest function to send an OID
    request to the driver. These OID requests are serialized so the
    MiniportOidRequest function is not called until the pending OID request
    has completed. MiniportOidRequest can be called at either IRQL 
    PASSIVE_LEVEL or IRQL DISPATCH_LEVEL. 
    
    If the MiniportOidRequest function returns NDIS_STATUS_PENDING, 
    the miniport driver must subsequently call the NdisMOidRequestComplete
    function to complete the request. NdisMOidRequestComplete can be called 
    at either IRQL DISPATCH_LEVEL or IRQL PASSIVE_LEVEL. 

Arguments:

    MiniportAdapterContext  Pointer to the adapter structure
    NdisRequest             Pointer to NDIS_OID_REQUEST
    
Return Value:
    
    NDIS_STATUS_SUCCESS
    NDIS_STATUS_NOT_SUPPORTED
    NDIS_STATUS_BUFFER_TOO_SHORT
    
--*/
{
    PADAPTER        pAdapter = (PADAPTER)   MiniportAdapterContext;
    NDIS_STATUS     ndisStatus;

    //
    // If the adapter has been surprise removed, fail request
    //
    if (MP_TEST_STATUS_FLAG(pAdapter, MP_ADAPTER_SURPRISE_REMOVED))
    {
        ndisStatus = NDIS_STATUS_FAILURE;
        MpTrace(COMP_OID, DBG_SERIOUS, ("NdisRequest failed as surprise removal is in progress\n"));
        return ndisStatus;
    }

    //
    // Assume by default, the request will pend. Covers the worst case
    //
    pAdapter->PendedRequest = NdisRequest;

    switch(NdisRequest->RequestType)
    {
        case NdisRequestQueryInformation:
        case NdisRequestQueryStatistics:
            ndisStatus = MpQueryInformation(
                            pAdapter,
                            NdisRequest->DATA.QUERY_INFORMATION.Oid,
                            NdisRequest->DATA.QUERY_INFORMATION.InformationBuffer,
                            NdisRequest->DATA.QUERY_INFORMATION.InformationBufferLength,
                            (PULONG)&NdisRequest->DATA.QUERY_INFORMATION.BytesWritten,
                            (PULONG)&NdisRequest->DATA.QUERY_INFORMATION.BytesNeeded
                            );
            break;

        case NdisRequestSetInformation:
            ndisStatus = MpSetInformation(
                            pAdapter,
                            NdisRequest->DATA.SET_INFORMATION.Oid,
                            NdisRequest->DATA.SET_INFORMATION.InformationBuffer,
                            NdisRequest->DATA.SET_INFORMATION.InformationBufferLength,
                            (PULONG)&NdisRequest->DATA.SET_INFORMATION.BytesRead,
                            (PULONG)&NdisRequest->DATA.SET_INFORMATION.BytesNeeded
                            );
            break;

        case NdisRequestMethod:
            ndisStatus = MpQuerySetInformation(
                            pAdapter,
                            NdisRequest->DATA.METHOD_INFORMATION.Oid,
                            NdisRequest->DATA.METHOD_INFORMATION.InformationBuffer,
                            NdisRequest->DATA.METHOD_INFORMATION.InputBufferLength,
                            NdisRequest->DATA.METHOD_INFORMATION.OutputBufferLength,
                            NdisRequest->DATA.METHOD_INFORMATION.MethodId,
                            (PULONG)&NdisRequest->DATA.METHOD_INFORMATION.BytesWritten,
                            (PULONG)&NdisRequest->DATA.METHOD_INFORMATION.BytesRead,
                            (PULONG)&NdisRequest->DATA.METHOD_INFORMATION.BytesNeeded
                            );
            break;

        default:
            ndisStatus = NDIS_STATUS_NOT_SUPPORTED;
            break;
    }
    
    if (ndisStatus != NDIS_STATUS_PENDING)
    {
        //
        // Request has completed
        //
        pAdapter->PendedRequest = NULL;
    }
    else
    {
        MpTrace(COMP_OID, DBG_NORMAL, ("Request has been pended. Will complete asynchronously\n"));
    }
    
    return ndisStatus;
}

VOID MPCancelRequest(
    __in NDIS_HANDLE hMiniportAdapterContext,
    __in PVOID       RequestId
)
/*++
Routine Description:

    An NDIS 6.0 miniport driver must provide a MiniportCancelOidRequest
    function if it pends any OID request. MiniportCancelOidRequest 
    is called when NDIS or an overlying driver calls NdisCancelOidRequest 
    or NdisFCancelOidRequest. If the miniport driver finds the OID 
    request to be cancelled in its queue, it should complete the OID request
    by calling NdisMOidRequestComplete with NDIS_STATUS_REQUEST_ABORTED.

Arguments:

    MiniportAdapterContext  Pointer to the adapter structure
    RequestId               Specify the request to be cancelled.
    
Return Value:
    
--*/
{
    UNREFERENCED_PARAMETER(hMiniportAdapterContext);
    UNREFERENCED_PARAMETER(RequestId);

}




VOID MPAdapterShutdown(
    __in  NDIS_HANDLE             MiniportAdapterContext,
    __in  NDIS_SHUTDOWN_ACTION    ShutdownAction
    )
/*++

Routine Description:

    The MiniportShutdown handler restores a NIC to its initial state when
    the system is shut down, whether by the user or because an unrecoverable
    system error occurred. This is to ensure that the NIC is in a known
    state and ready to be reinitialized when the machine is rebooted after
    a system shutdown occurs for any reason, including a crash dump.

    Here just disable the interrupt and stop the DMA engine.
    Do not free memory resources or wait for any packet transfers
    to complete.


    Runs at an arbitrary IRQL <= DIRQL. So do not call any passive-level
    function.

Arguments:

    MiniportAdapterContext  Pointer to our adapter

Return Value:

    None

--*/
{
    PADAPTER        pAdapter = (PADAPTER) MiniportAdapterContext;
//    UNREFERENCED_PARAMETER(MiniportAdapterContext);
    UNREFERENCED_PARAMETER(ShutdownAction);

    //
    // TODO: Any place where we are reading registers and making major
    // decisions should consider protecting against FFFF for surprise 
    // removal case
    //

    MpEntry;

    if (MP_TEST_STATUS_FLAG(pAdapter, MP_ADAPTER_SURPRISE_REMOVED) == FALSE) {
        //
        // NOTe: PCI Disable Interrupts only if adapter has not been removed
        //
        //
        // Issue a shutdown to the NIC. NIC should go into a known state
        // and shut off power to the antenna. If surprise removal has
        // occurred, we will not do this.
        //
        Hw11Shutdown(pAdapter->Nic);
    }       

    MpExit;
}

VOID
MPDevicePnPEvent(
    __in NDIS_HANDLE                  MiniportAdapterContext,
    __in PNET_DEVICE_PNP_EVENT        NetDevicePnPEvent
    )
{
    PADAPTER            pAdapter = (PADAPTER) MiniportAdapterContext;
    NDIS_DEVICE_PNP_EVENT        DevicePnPEvent = NetDevicePnPEvent->DevicePnPEvent;
    PVOID                        InformationBuffer = NetDevicePnPEvent->InformationBuffer;
    ULONG                        InformationBufferLength = NetDevicePnPEvent->InformationBufferLength;

    MpEntry;

    switch (DevicePnPEvent)
    {
        case NdisDevicePnPEventQueryRemoved:
            MpTrace(COMP_INIT_PNP, DBG_SERIOUS, ("MPDevicePnPEventNotify: NdisDevicePnPEventQueryRemoved\n"));
            break;

        case NdisDevicePnPEventRemoved:
            MpTrace(COMP_INIT_PNP, DBG_SERIOUS, ("MPDevicePnPEventNotify: NdisDevicePnPEventRemoved\n"));
            break;       

        case NdisDevicePnPEventSurpriseRemoved:
            MpTrace(COMP_INIT_PNP, DBG_SERIOUS, ("MPDevicePnPEventNotify: NdisDevicePnPEventSurpriseRemoved\n"));
            MpOnSurpriseRemoval(pAdapter);
            break;

        case NdisDevicePnPEventQueryStopped:
            MpTrace(COMP_INIT_PNP, DBG_SERIOUS, ("MPDevicePnPEventNotify: NdisDevicePnPEventQueryStopped\n"));
            break;

        case NdisDevicePnPEventStopped:
            MpTrace(COMP_INIT_PNP, DBG_SERIOUS, ("MPDevicePnPEventNotify: NdisDevicePnPEventStopped\n"));
            break;      
            
        case NdisDevicePnPEventPowerProfileChanged:
            MpTrace(COMP_INIT_PNP, DBG_SERIOUS, ("MPDevicePnPEventNotify: NdisDevicePnPEventPowerProfileChanged\n"));
            break;      
            
        default:
            MpTrace(COMP_INIT_PNP, DBG_SERIOUS, ("MPDevicePnPEventNotify: unknown PnP event %x \n", DevicePnPEvent));
            MpExit;
            return;
    }

    //
    // This is a valid PnPEvent. Notify Hw11 about it.
    //
    Hw11DevicePnPEvent(
        pAdapter->Nic,
        DevicePnPEvent,
        InformationBuffer,
        InformationBufferLength
        );

    MpExit;
}


WDFDEVICE
Mp11GetWdfDevice(
    __in  PADAPTER        pAdapter
    )
{
    return   pAdapter->WdfDevice;
}


/**
 * This routine resets the miniport. As part of reset, the NIC is asked to stop its
 * interrupts, send and receive units (and any other units NIC feels should be reset).
 * All submitted TX_MSDUs that have not yet been sent are failed. Any packets waiting in
 * the TxQueue are also failed.
 *
 * \param pAdapter      The adapter being reset
 * \param ResetType     The type of reset to perform 
 * \sa MpReset, MpNdisResetWorkItem
 */
__drv_requiresIRQL(DISPATCH_LEVEL)
NDIS_STATUS
MpResetInternalRoutine(
    __in PADAPTER             pAdapter,
    __in MP_RESET_TYPE        ResetType
    )
{
    NDIS_STATUS ndisStatus;
    
    MpEntry;

    do
    {
        //
        // Note : we release the send lock. Because Stop Notificaton causes the WriteCOmpletionROutine to 
        // be invoked for cancelled IO which call's the MP Tx completion handler and in turn acquires the same lock . 
        //
        MP_RELEASE_SEND_LOCK(pAdapter, FALSE);   
        Hw11StopNotification(pAdapter->Nic);    
        
        //
        // Issue a reset start to the NIC. Nic should suspend transmit and receive units
        // among other things.
        // NOTE: For USB, this requires sending sync. request to the device hence acquire the lock later.
        //
        ndisStatus = Hw11ResetStep2(pAdapter->Nic);
        if (ndisStatus != NDIS_STATUS_SUCCESS)
        {
            MpTrace(COMP_INIT_PNP, DBG_SERIOUS, ("An error 0x%08x occured during Hw11ResetStart. Will attempt to continue\n", ndisStatus));
            if (ndisStatus != NDIS_STATUS_SOFT_ERRORS)
            {
                MpTrace(COMP_INIT_PNP, DBG_SERIOUS, ("Unable to recover from the error. Aborting reset request\n"));
                break;
            }
        }

        MP_ACQUIRE_SEND_LOCK(pAdapter, FALSE);      
        //
        // Notify CustomInterfaces about the reset operation
        //
        MpEventMiniportReset(pAdapter); 

        //
        // Ask the station to reset
        //
        Sta11ResetStep2(pAdapter->Station, ResetType);

        //
        // Try to free the unsent packets that have been submitted to the hardware
        //
        MpCompleteQueuedTxMSDUs(pAdapter);

        //
        // Free the packets that are waiting in the TxQueue.
        //
        MpCompleteQueuedNBL(pAdapter);

        //
        // Reintialize the send engine to bring to init state
        //
        MpReinitializeSendEngine(pAdapter);
       
        MP_RELEASE_SEND_LOCK(pAdapter, FALSE);
    } WHILE(FALSE);

    //
    // Everything has been cleaned up, we dont need the lock to reset the hardware
    //

    //
    // Reset Step 3 - Reinitialize MAC and PHY state
    //
    ndisStatus = Hw11ResetStep3(pAdapter->Nic, ResetType);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        MpTrace(COMP_INIT_PNP, DBG_SERIOUS, ("Failure occured while resetting the NIC. Reset Type: %d, Error: 0x%08x\n", ResetType, ndisStatus));

        Hw11EnableNotification(pAdapter->Nic);     
        MP_ACQUIRE_SEND_LOCK(pAdapter, FALSE);
        return ndisStatus;  // Send lock held
    }

    //
    // Reset Step 4 - Restart anything that may have been stopped
    //
       
    //
    // Issue a reset end to the NIC. Hw11 should reinitialize all the Rx descriptors
    // and restart send/receive units among other things.  
    // NOTE: For USB, this requires sending sync. request to the device hence acquire the lock later.
    //
    ndisStatus = Hw11ResetStep4(pAdapter->Nic);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        MpTrace(COMP_INIT_PNP, DBG_SERIOUS, ("An error 0x%08x occured during Hw11ResetStep4.\n", ndisStatus));
    }
    Hw11EnableNotification(pAdapter->Nic);

    MP_ACQUIRE_SEND_LOCK(pAdapter, FALSE);

    MpExit;
    return ndisStatus;
}


/**
 * This is the work item in which NDIS reset operation will be executed if
 * it was pended.
 *
 * This routine waits for any active senders doing send to complete. When
 * they do complete, MpResetInternalRoutine is invoked which executes the
 * actual reset of the miniport driver and the hardware.
 * 
 * \param Context       The context we registered when initializing the NIC
 * \sa MpReset, MpResetInternalRoutine
 */
VOID
MpNdisResetWorkItem(
    __in WDFWORKITEM  WorkItem
    )
{
    NDIS_STATUS     ndisStatus;
    PADAPTER        pAdapter ;
    PADAPTER_WORKITEM_CONTEXT adapterWorkItemContext;

    adapterWorkItemContext = GetAdapterWorkItemContext(WorkItem);
    pAdapter = adapterWorkItemContext->Adapter;

    //
    // This lock synchronizes between Reset/Pause/Restart
    //
    MP_ACQUIRE_RESET_PNP_LOCK(pAdapter);

    //
    // If the scan operation is in progress, cancel it.
    //
    if (MP_TEST_STATUS_FLAG(pAdapter, MP_ADAPTER_HW_IS_SCANNING))
    {
        Hw11CancelScan(pAdapter->Nic);
    }
    MPVERIFY(!MP_TEST_STATUS_FLAG(pAdapter, MP_ADAPTER_HW_IS_SCANNING));

    //
    // Stop the recv. notification
    //
    Hw11StopNotification(pAdapter->Nic);

    //
    // Reset Step 1 - Wait for things to cleanup
    //

    //
    // Dont redo the work that is already done by
    // the Dot11Reset/Pause routines (protected by PNP lock)
    //
    if (MP_INCREMENT_RESET_PAUSE_COUNT(pAdapter) == 1)
    {
        //
        // Remove the Active Sender count we added during Initialize
        //
        MP_DECREMENT_ACTIVE_SENDERS(pAdapter);
        NdisWaitEvent(&pAdapter->ActiveSendersDoneEvent, MP_NEVER_TIMEOUT);

        //
        // Remove the Active Receivers count added during Initialize
        // We can Reset when the count reaches 1 or less.
        //
        MP_DECREMENT_ACTIVE_RECEIVERS(pAdapter);
        while(pAdapter->NumActiveReceivers >= 1)
        {
            NdisMSleep(10 * 1000);  // 10 sec
        }
    }
    
    //
    // Have the station reset
    //
    Sta11ResetStep1(pAdapter->Station, MP_RESET_TYPE_NDIS_RESET);

    MP_ACQUIRE_SEND_LOCK(pAdapter, FALSE);
    ndisStatus = MpResetInternalRoutine(pAdapter, MP_RESET_TYPE_NDIS_RESET);

    //
    // Reset the variables that may have potentially caused the reset to occur
    //
    pAdapter->TotalTransmitsSnapShot = 0;
    pAdapter->NumStalledSendTicks = 0;

    //
    // Undo the pause/reset work from before
    //
    if (MP_DECREMENT_RESET_PAUSE_COUNT(pAdapter) == 0)
    {
        //
        // Reset the ActiveSenders Event and put the reference back on
        //
        MP_INCREMENT_ACTIVE_SENDERS(pAdapter);
        NdisResetEvent(&pAdapter->ActiveSendersDoneEvent);

        //
        // Place the Ref Count back on ActiveReceivers.
        //
        MP_INCREMENT_ACTIVE_RECEIVERS(pAdapter);
    }

    //
    // After NdisReset, the station may reassociate if it was already
    // associated
    //
    Sta11ResetStep4(pAdapter->Station, MP_RESET_TYPE_NDIS_RESET);

    if (ndisStatus == NDIS_STATUS_SUCCESS || ndisStatus == NDIS_STATUS_SOFT_ERRORS)
    {
        //
        // Reset completed successfully. Soft errors are recoverable.
        // Reset the NumResetAttempts counter and clear any error flags if set.
        //
        pAdapter->NumResetAttempts = 0;
        MP_CLEAR_STATUS_FLAG(pAdapter, (MP_ADAPTER_RESET_IN_PROGRESS | MP_ADAPTER_HARDWARE_ERROR | MP_ADAPTER_NON_RECOVER_ERROR));
        ndisStatus = NDIS_STATUS_SUCCESS;
    }
    else
    {
        //
        // Reset did not occur successfully.
        //
        if (pAdapter->NumResetAttempts < MP_RESET_ATTEMPTS_THRESHOLD)
        {
            //
            // Another attempt failed. Will try again later.
            //
            pAdapter->NumResetAttempts++;
        }
        else
        {
            //
            // Repeated attempts to reset have failed. Remove this adapter.
            //
            MpRemoveAdapter(pAdapter);
        }

        //
        // Reset completed but not successfully. Leave error bits on
        //
        MP_CLEAR_STATUS_FLAG(pAdapter, MP_ADAPTER_RESET_IN_PROGRESS);
    }

    MP_RELEASE_SEND_LOCK(pAdapter, FALSE);
    
    MP_RELEASE_RESET_PNP_LOCK(pAdapter);

    //
    // notify NDIS that reset is complete
    //
    NdisMResetComplete(
        pAdapter->MiniportAdapterHandle,
        ndisStatus,
        HW11_REQUIRES_ADDRESS_RESET
        );
}

VOID
Mp11InternalReset(
    __in  PADAPTER        pAdapter,
    __in  MP_RESET_TYPE   ResetType
    )
{
    MP_ACQUIRE_SEND_LOCK(pAdapter, FALSE);    
    MpResetInternalRoutine(pAdapter, ResetType);
    MP_RELEASE_SEND_LOCK(pAdapter, FALSE);
}

VOID
Mp11IndicateStatusIndication(
    __in  PADAPTER        pAdapter,
    __in  PNDIS_STATUS_INDICATION pStatusIndication
    )
{
    NdisMIndicateStatusEx(
        pAdapter->MiniportAdapterHandle,
        pStatusIndication
        );
}






