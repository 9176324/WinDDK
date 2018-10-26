#include "miniport.h"
#include "aha154x.h"           // includes scsi.h
#include "wmistr.h"             // WMI definitions

#include "support.h"           // ScsiPortZeroMemory(), ScsiPortCompareMemory()
#include "aha154dt.h"

#define Aha154xWmi_MofResourceName        L"MofResource"

#define AHA154x_SETUP_GUID_INDEX 0

GUID A154xWmiSetupDataGuid = Aha154xWmi_SetupData_Guid;

UCHAR
A154xReadSetupData(
   IN  PHW_DEVICE_EXTENSION HwDeviceExtension,
   OUT PUCHAR               Buffer
   );

BOOLEAN
A154xQueryWmiDataBlock(
    IN PVOID Context,
    IN PSCSIWMI_REQUEST_CONTEXT RequestContext,
    IN ULONG GuidIndex,
    IN ULONG InstanceIndex,
    IN ULONG InstanceCount,
    IN OUT PULONG InstanceLengthArray,
    IN ULONG OutBufferSize,
    OUT PUCHAR Buffer
    );

UCHAR
A154xQueryWmiRegInfo(
    IN PVOID Context,
    IN PSCSIWMI_REQUEST_CONTEXT RequestContext,
    OUT PWCHAR *MofResourceName
    );

SCSIWMIGUIDREGINFO A154xGuidList[] =
{
   {&A154xWmiSetupDataGuid,
    1,
    0
   },
};

#define A154xGuidCount (sizeof(A154xGuidList) / sizeof(SCSIWMIGUIDREGINFO))


void A154xWmiInitialize(
    IN PHW_DEVICE_EXTENSION HwDeviceExtension
    )
{
    PSCSI_WMILIB_CONTEXT WmiLibContext;

    WmiLibContext = &HwDeviceExtension->WmiLibContext;

    WmiLibContext->GuidList = A154xGuidList;
    WmiLibContext->GuidCount = A154xGuidCount;
    WmiLibContext->QueryWmiRegInfo = A154xQueryWmiRegInfo;
    WmiLibContext->QueryWmiDataBlock = A154xQueryWmiDataBlock;
    WmiLibContext->SetWmiDataItem = NULL;
    WmiLibContext->SetWmiDataBlock = NULL;
    WmiLibContext->WmiFunctionControl = NULL;
    WmiLibContext->ExecuteWmiMethod = NULL;
}



BOOLEAN
A154xWmiSrb(
    IN     PHW_DEVICE_EXTENSION    HwDeviceExtension,
    IN OUT PSCSI_WMI_REQUEST_BLOCK Srb
    )
/*++

Routine Description:

   Process an SRB_FUNCTION_WMI request packet.

   This routine is called from the SCSI port driver synchronized with the
   kernel via Aha154xStartIo.   On completion of WMI processing, the SCSI
   port driver is notified that the adapter can take another request,  if
   any are available.

Arguments:

   HwDeviceExtension - HBA miniport driver's adapter data storage.

   Srb               - IO request packet.

Return Value:

   Value to return to Aha154xStartIo caller.   Always TRUE.

--*/
{
   UCHAR status;
   SCSIWMI_REQUEST_CONTEXT requestContext;
   ULONG retSize;
   BOOLEAN pending;

   //
   // Validate our assumptions.
   //

   ASSERT(Srb->Function == SRB_FUNCTION_WMI);
   ASSERT(Srb->Length == sizeof(SCSI_WMI_REQUEST_BLOCK));
   ASSERT(Srb->DataTransferLength >= sizeof(ULONG));
   ASSERT(Srb->DataBuffer);

   //
   // Check if the WMI SRB is targetted for the adapter or one of the disks
   if (!(Srb->WMIFlags & SRB_WMI_FLAGS_ADAPTER_REQUEST)) {

      //
      // This is targetted to one of the disks, since there are no per disk
      // wmi information we return an error. Note that if there was per
      // disk information, then you'd likely have a differen WmiLibContext
      // and a different set of guids.
      //
      Srb->DataTransferLength = 0;
      Srb->SrbStatus = SRB_STATUS_SUCCESS;

   } else {

       //
       // Process the incoming WMI request.
       //

       pending = ScsiPortWmiDispatchFunction(&HwDeviceExtension->WmiLibContext,
                                                Srb->WMISubFunction,
                                                HwDeviceExtension,
                                                &requestContext,
                                                Srb->DataPath,
                                                Srb->DataTransferLength,
                                                Srb->DataBuffer);

       //
       // We assune that the wmi request will never pend so that we can
       // allocate the requestContext from stack. If the WMI request could
       // ever pend then we'd need to allocate the request context from
       // the SRB extension.
       //
       ASSERT(! pending);

       retSize =  ScsiPortWmiGetReturnSize(&requestContext);
       status =  ScsiPortWmiGetReturnStatus(&requestContext);

       // We can do this since we assume it is done synchronously

       Srb->DataTransferLength = retSize;

       //
       // Adapter ready for next request.
       //

       Srb->SrbStatus = status;
   }

   ScsiPortNotification(RequestComplete, HwDeviceExtension, Srb);
   ScsiPortNotification(NextRequest,     HwDeviceExtension, NULL);

   return TRUE;
}



BOOLEAN
A154xQueryWmiDataBlock(
    IN PVOID Context,
    IN PSCSIWMI_REQUEST_CONTEXT RequestContext,
    IN ULONG GuidIndex,
    IN ULONG InstanceIndex,
    IN ULONG InstanceCount,
    IN OUT PULONG InstanceLengthArray,
    IN ULONG OutBufferSize,
    OUT PUCHAR Buffer
    )
/*++

Routine Description:

    This routine is a callback into the miniport to query for the contents of
    one or more instances of a data block. This callback may be called with
    an output buffer that is too small to return all of the data queried.
    In this case the callback is responsible to report the correct output
        buffer size needed.

    If the request can be completed immediately without pending,
        ScsiPortWmiPostProcess should be called from within this callback and
    FALSE returned.

    If the request cannot be completed within this callback then TRUE should
    be returned. Once the pending operations are finished the miniport should
    call ScsiPortWmiPostProcess and then complete the srb.

Arguments:

    DeviceContext is a caller specified context value originally passed to
        ScsiPortWmiDispatchFunction.

    RequestContext is a context associated with the srb being processed.

    GuidIndex is the index into the list of guids provided when the
        miniport registered

    InstanceIndex is the index that denotes first instance of the data block
        is being queried.

    InstanceCount is the number of instances expected to be returned for
        the data block.

    InstanceLengthArray is a pointer to an array of ULONG that returns the
        lengths of each instance of the data block. This may be NULL when
        there is not enough space in the output buffer to fufill the request.
        In this case the miniport should call ScsiPortWmiPostProcess with
        a status of SRB_STATUS_DATA_OVERRUN and the size of the output buffer
        needed to fufill the request.

    BufferAvail on entry has the maximum size available to write the data
        blocks in the output buffer. If the output buffer is not large enough
        to return all of the data blocks then the miniport should call
        ScsiPortWmiPostProcess with a status of SRB_STATUS_DATA_OVERRUN
        and the size of the output buffer needed to fufill the request.

    Buffer on return is filled with the returned data blocks. Note that each
        instance of the data block must be aligned on a 8 byte boundry. This
        may be NULL when there is not enough space in the output buffer to
        fufill the request. In this case the miniport should call
        ScsiPortWmiPostProcess with a status of SRB_STATUS_DATA_OVERRUN and
        the size of the output buffer needed to fufill the request.


Return Value:

    TRUE if request is pending else FALSE

--*/
{
    PHW_DEVICE_EXTENSION HwDeviceExtension = (PHW_DEVICE_EXTENSION)Context;
    ULONG size = 0;
    UCHAR status;

    //
    // Only ever registers 1 instance per guid
    ASSERT((InstanceIndex == 0) &&
           (InstanceCount == 1));

    switch (GuidIndex)
    {
        case AHA154x_SETUP_GUID_INDEX:
        {
            size = sizeof(AHA154SetupData)-1;
            if (OutBufferSize < size)
            {
                //
                // The buffer passed to return the data is too small
                //
                status = SRB_STATUS_DATA_OVERRUN;
                break;
            }

            if ( !A154xReadSetupData(HwDeviceExtension,
                                     Buffer))
            {
                ASSERT(FALSE);
                size = 0;
                status = SRB_STATUS_ERROR;
            } else {
                *InstanceLengthArray = size;
                status = SRB_STATUS_SUCCESS;
            }

            break;
        }

        default:
        {
            status = SRB_STATUS_ERROR;
        }
    }

    ScsiPortWmiPostProcess(RequestContext,
                           status,
                           size);

    return status;
}

UCHAR
A154xQueryWmiRegInfo(
    IN PVOID Context,
    IN PSCSIWMI_REQUEST_CONTEXT RequestContext,
    OUT PWCHAR *MofResourceName
    )
/*++

Routine Description:

    This routine is a callback into the driver to retrieve information about
    the guids being registered.

    Implementations of this routine may be in paged memory

Arguments:

    DeviceObject is the device whose registration information is needed

    *RegFlags returns with a set of flags that describe all of the guids being
        registered for this device. If the device wants enable and disable
        collection callbacks before receiving queries for the registered
        guids then it should return the WMIREG_FLAG_EXPENSIVE flag. Also the
        returned flags may specify WMIREG_FLAG_INSTANCE_PDO in which case
        the instance name is determined from the PDO associated with the
        device object. Note that the PDO must have an associated devnode. If
        WMIREG_FLAG_INSTANCE_PDO is not set then Name must return a unique
        name for the device. These flags are ORed into the flags specified
        by the GUIDREGINFO for each guid.

    InstanceName returns with the instance name for the guids if
        WMIREG_FLAG_INSTANCE_PDO is not set in the returned *RegFlags. The
        caller will call ExFreePool with the buffer returned.

    *RegistryPath returns with the registry path of the driver. This is
        required

    *MofResourceName returns with the name of the MOF resource attached to
        the binary file. If the driver does not have a mof resource attached
        then this can be returned as NULL.

    *Pdo returns with the device object for the PDO associated with this
        device if the WMIREG_FLAG_INSTANCE_PDO flag is retured in
        *RegFlags.

Return Value:

    status

--*/
{
    *MofResourceName = Aha154xWmi_MofResourceName;
    return SRB_STATUS_SUCCESS;
}


UCHAR
A154xReadSetupData(
   IN  PHW_DEVICE_EXTENSION HwDeviceExtension,
   OUT PUCHAR               Buffer
   )
/*++

Routine Description:

   Read the adapter setup information into the supplied buffer.  The buffer
   must be RM_CFG_MAX_SIZE (255) bytes large.

Arguments:

   HwDeviceExtension - HBA miniport driver's adapter data storage.

   Buffer - Buffer to hold adapter's setup information structure [manual 5-10].

Return Value:

   TRUE on success, FALSE on failure.

--*/
{
   UCHAR numberOfBytes = sizeof(AHA154SetupData)-1;

   //
   // Read off config data from AHA154X...
   //

   if (!WriteCommandRegister(HwDeviceExtension, AC_RETURN_SETUP_DATA, TRUE))
      return FALSE;

   if (!WriteDataRegister(HwDeviceExtension, numberOfBytes))
      return FALSE;

   for (; numberOfBytes; numberOfBytes--) {
      if (!(ReadCommandRegister(HwDeviceExtension, Buffer, TRUE)))
          return FALSE;
      Buffer++;
   }

   //
   // ...and wait for interrupt
   //

   if (!SpinForInterrupt(HwDeviceExtension, TRUE))
      return FALSE;

   return TRUE;
}

