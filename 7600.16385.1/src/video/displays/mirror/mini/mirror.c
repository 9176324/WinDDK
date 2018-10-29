
#include "dderror.h"
#include "devioctl.h"
#include "miniport.h"
#include "ntddvdeo.h"
#include "video.h"

#include "mirror.h"

VOID DbgBreakPoint() {};

VOID
MirrorNotImplemented(
    __in  char *s
    )
{
   VideoDebugPrint((0, "Mirror Sample: Not used '%s'.\n", s));
}

BOOLEAN
MirrorResetHW(
    PVOID HwDeviceExtension,
    ULONG Columns,
    ULONG Rows
    )
{
   UNREFERENCED_PARAMETER(HwDeviceExtension);
   UNREFERENCED_PARAMETER(Columns);
   UNREFERENCED_PARAMETER(Rows);
   
   MirrorNotImplemented("MirrorResetHW");
   
   return TRUE;
}

BOOLEAN
MirrorVidInterrupt(
    PVOID HwDeviceExtension
    )
{
   UNREFERENCED_PARAMETER(HwDeviceExtension);
   
   MirrorNotImplemented("MirrorVidInterrupt");

   return TRUE;
}

VP_STATUS
MirrorGetPowerState(
    PVOID                   HwDeviceExtension,
    ULONG                   HwId,
    PVIDEO_POWER_MANAGEMENT VideoPowerControl
    )
{
   UNREFERENCED_PARAMETER(HwDeviceExtension);
   UNREFERENCED_PARAMETER(HwId);
   UNREFERENCED_PARAMETER(VideoPowerControl);
   
   MirrorNotImplemented("MirrorGetPowerState");

   return NO_ERROR;
}

VP_STATUS
MirrorSetPowerState(
    PVOID                   HwDeviceExtension,
    ULONG                   HwId,
    PVIDEO_POWER_MANAGEMENT VideoPowerControl
    )
{
   UNREFERENCED_PARAMETER(HwDeviceExtension);
   UNREFERENCED_PARAMETER(HwId);
   UNREFERENCED_PARAMETER(VideoPowerControl);

   MirrorNotImplemented("MirrorSetPowerState");

   return NO_ERROR;
}

VP_STATUS
MirrorGetChildDescriptor (
    IN  PVOID                  HwDeviceExtension,
    IN  PVIDEO_CHILD_ENUM_INFO ChildEnumInfo,
    OUT PVIDEO_CHILD_TYPE      pChildType,
    OUT PVOID                  pChildDescriptor,
    OUT PULONG                 pUId,
    OUT PULONG                 pUnused
    )
{
   UNREFERENCED_PARAMETER(HwDeviceExtension);
   UNREFERENCED_PARAMETER(ChildEnumInfo);
   UNREFERENCED_PARAMETER(pChildType);
   UNREFERENCED_PARAMETER(pChildDescriptor);
   UNREFERENCED_PARAMETER(pUId);
   UNREFERENCED_PARAMETER(pUnused);

   MirrorNotImplemented("MirrorGetChildDescriptor");

   return ERROR_NO_MORE_DEVICES;
}

VP_STATUS
    __checkReturn
MirrorFindAdapter(
   __in     PVOID   HwDeviceExtension,
   __in     PVOID   HwContext,
   __in     PWSTR   ArgumentString,
   __inout_bcount(sizeof(VIDEO_PORT_CONFIG_INFO))  PVIDEO_PORT_CONFIG_INFO  ConfigInfo,
   __out    PUCHAR  Again
   )
{
   UNREFERENCED_PARAMETER(HwDeviceExtension);
   UNREFERENCED_PARAMETER(HwContext);
   UNREFERENCED_PARAMETER(ArgumentString);
   UNREFERENCED_PARAMETER(ConfigInfo);
   UNREFERENCED_PARAMETER(Again);

   VideoDebugPrint((0,"FindAdapter Called.\n"));
 
   return NO_ERROR;
}

BOOLEAN MirrorInitialize(
   PVOID HwDeviceExtension
   )
{
   UNREFERENCED_PARAMETER(HwDeviceExtension);

   VideoDebugPrint((0,"Initialize Called.\n"));

   return TRUE;
}

BOOLEAN MirrorStartIO(
   PVOID HwDeviceExtension,
   PVIDEO_REQUEST_PACKET RequestPacket
   )
{
   UNREFERENCED_PARAMETER(HwDeviceExtension);
   UNREFERENCED_PARAMETER(RequestPacket);

   VideoDebugPrint((0,"StartIO Called.\n"));

   return TRUE;
}

ULONG
DriverEntry (
    PVOID Context1,
    PVOID Context2
    )
{

    VIDEO_HW_INITIALIZATION_DATA hwInitData;
    ULONG initializationStatus;

    VideoDebugPrint((0, "Mirrored Driver VideoPort [Driver Entry]\n"));

    // Zero out structure.

    VideoPortZeroMemory(&hwInitData, sizeof(VIDEO_HW_INITIALIZATION_DATA));

    // Specify sizes of structure and extension.

    hwInitData.HwInitDataSize = sizeof(VIDEO_HW_INITIALIZATION_DATA);

    // Set entry points.

    hwInitData.HwFindAdapter             = &MirrorFindAdapter;
    hwInitData.HwInitialize              = &MirrorInitialize;
    hwInitData.HwStartIO                 = &MirrorStartIO;
    hwInitData.HwResetHw                 = &MirrorResetHW;
    hwInitData.HwInterrupt               = &MirrorVidInterrupt;
    hwInitData.HwGetPowerState           = &MirrorGetPowerState;
    hwInitData.HwSetPowerState           = &MirrorSetPowerState;
    hwInitData.HwGetVideoChildDescriptor = &MirrorGetChildDescriptor;

    hwInitData.HwLegacyResourceList      = NULL; 
    hwInitData.HwLegacyResourceCount     = 0; 

    // no device extension necessary
    hwInitData.HwDeviceExtensionSize = 0;
    hwInitData.AdapterInterfaceType = 0;

    initializationStatus = VideoPortInitialize(Context1,
                                               Context2,
                                               &hwInitData,
                                               NULL);

    return initializationStatus;

} // end DriverEntry()

