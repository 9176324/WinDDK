/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    ntddvol.h

Abstract:

    This module contains type and function definitions for NT volumes.

    This module may contain definitions that are exposed in public
    kit headers.

--*/


#ifndef _NTDDVOL_
#define _NTDDVOL_

#if _MSC_VER > 1000
#pragma once
#endif


#ifdef __cplusplus
extern "C" {
#endif

#ifndef IOCTL_VOLUME_BASE
// begin_winioctl

#define IOCTL_VOLUME_BASE                       0x00000056 // 'V'

#if (NTDDI_VERSION >= NTDDI_WIN2K)

//
// IOCTL to obtain the physical location of
// the specified volume on one or more disks.
//

#define IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS    CTL_CODE(IOCTL_VOLUME_BASE, 0, METHOD_BUFFERED, FILE_ANY_ACCESS)

//
// IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS
//
// Input Buffer:
//     None
//
// Output Buffer:
//     Structure of type VOLUME_DISK_EXTENTS
//

typedef struct _DISK_EXTENT {

    //
    // Specifies the storage device number of
    // the disk on which this extent resides.
    //
    ULONG DiskNumber;

    //
    // Specifies the offset and length of this
    // extent relative to the beginning of the
    // disk.
    //
    LARGE_INTEGER StartingOffset;
    LARGE_INTEGER ExtentLength;

} DISK_EXTENT, *PDISK_EXTENT;

typedef struct _VOLUME_DISK_EXTENTS {

    //
    // Specifies one or more contiguous range
    // of sectors that make up this volume.
    //
    ULONG NumberOfDiskExtents;
    DISK_EXTENT Extents[ANYSIZE_ARRAY];

} VOLUME_DISK_EXTENTS, *PVOLUME_DISK_EXTENTS;


//
// IOCTLs to transition the specified volume
// between r/w and non r/w modes.
//

#define IOCTL_VOLUME_ONLINE                     CTL_CODE(IOCTL_VOLUME_BASE, 2, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_VOLUME_OFFLINE                    CTL_CODE(IOCTL_VOLUME_BASE, 3, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

//
// IOCTL_VOLUME_ONLINE
// IOCTL_VOLUME_OFFLINE
//
// Input Buffer:
//     None
//
// Output Buffer:
//     None
//

#endif  // NTDDI_VERSION >= NTDDI_WIN2K


#if (NTDDI_VERSION >= NTDDI_WINXP)

//
// IOCTL to determine  whether  the specified
// volume resides on a disk that is an online
// cluster resource or not.
//

#define IOCTL_VOLUME_IS_CLUSTERED               CTL_CODE(IOCTL_VOLUME_BASE, 12, METHOD_BUFFERED, FILE_ANY_ACCESS)

//
// IOCTL_VOLUME_IS_CLUSTERED
//
// Input Buffer:
//     None
//
// Output Buffer:
//     None
//

//
// IOCTL to query the attributes on volumes.
//

#define IOCTL_VOLUME_GET_GPT_ATTRIBUTES         CTL_CODE(IOCTL_VOLUME_BASE, 14, METHOD_BUFFERED, FILE_ANY_ACCESS)

//
// IOCTL_VOLUME_GET_GPT_ATTRIBUTES
//
// Input Buffer:
//     None
//
// Output Buffer:
//     Structure of type VOLUME_GET_GPT_ATTRIBUTES_INFORMATION
//

typedef struct _VOLUME_GET_GPT_ATTRIBUTES_INFORMATION {

    //
    // Specifies all the attributes
    // associated with this volume.
    //
    ULONGLONG GptAttributes;

} VOLUME_GET_GPT_ATTRIBUTES_INFORMATION, *PVOLUME_GET_GPT_ATTRIBUTES_INFORMATION;

#endif  // NTDDI_VERSION >= NTDDI_WINXP

// end_winioctl
#endif  // IOCTL_VOLUME_BASE


#if (NTDDI_VERSION >= NTDDI_WIN2K)

//
// Legacy IOCTLs that were used to support
// clustering.
//

#define IOCTL_VOLUME_SUPPORTS_ONLINE_OFFLINE    CTL_CODE(IOCTL_VOLUME_BASE, 1, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VOLUME_IS_OFFLINE                 CTL_CODE(IOCTL_VOLUME_BASE, 4, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VOLUME_IS_IO_CAPABLE              CTL_CODE(IOCTL_VOLUME_BASE, 5, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VOLUME_QUERY_FAILOVER_SET         CTL_CODE(IOCTL_VOLUME_BASE, 6, METHOD_BUFFERED, FILE_ANY_ACCESS)

//
// IOCTL_VOLUME_SUPPORTS_ONLINE_OFFLINE
// IOCTL_VOLUME_IS_OFFLINE
// IOCTL_VOLUME_IS_IO_CAPABLE
//
// Input Buffer:
//     None
//
// Output Buffer:
//     None
//

//
// IOCTL_VOLUME_QUERY_FAILOVER_SET
//
// Input Buffer:
//     None
//
// Output Buffer:
//     Structure of type VOLUME_FAILOVER_SET
//

typedef struct _VOLUME_FAILOVER_SET {

    //
    // Specifies all of the  disks that are
    // part of the pack that this volume is
    // part of.
    //
    ULONG NumberOfDisks;
    ULONG DiskNumbers[ANYSIZE_ARRAY];

} VOLUME_FAILOVER_SET, *PVOLUME_FAILOVER_SET;


//
// IOCTL to obtain general information about
// the specified volume.
//

#define IOCTL_VOLUME_QUERY_VOLUME_NUMBER        CTL_CODE(IOCTL_VOLUME_BASE, 7, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VOLUME_LOGICAL_TO_PHYSICAL        CTL_CODE(IOCTL_VOLUME_BASE, 8, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VOLUME_PHYSICAL_TO_LOGICAL        CTL_CODE(IOCTL_VOLUME_BASE, 9, METHOD_BUFFERED, FILE_ANY_ACCESS)

//
// IOCTL_VOLUME_QUERY_VOLUME_NUMBER
//
// Input Buffer:
//     None
//
// Output Buffer:
//     Structure of type VOLUME_NUMBER
//

typedef struct _VOLUME_NUMBER {

    //
    // Specifies the  number associated
    // with this volume for the current
    // session.
    //
    ULONG VolumeNumber;

    //
    // Specifies the name of the volume
    // manager driver.  If this is less
    // than 8, it is padded with blanks.
    //
    WCHAR VolumeManagerName[8];

} VOLUME_NUMBER, *PVOLUME_NUMBER;

//
// IOCTL_VOLUME_LOGICAL_TO_PHYSICAL
//
// Input Buffer:
//     Structure of type VOLUME_LOGICAL_OFFSET
//
// Output Buffer:
//     Structure of type VOLUME_PHYSICAL_OFFSETS
//

typedef struct _VOLUME_LOGICAL_OFFSET {

    //
    // Specifies the logical location
    // that needs to be translated to
    // a physical location.
    //
    LONGLONG LogicalOffset;

} VOLUME_LOGICAL_OFFSET, *PVOLUME_LOGICAL_OFFSET;

typedef struct _VOLUME_PHYSICAL_OFFSET {

    //
    // Specifies the physical location
    // that needs to be  translated to
    // a logical location.
    //
    ULONG DiskNumber;
    LONGLONG Offset;

} VOLUME_PHYSICAL_OFFSET, *PVOLUME_PHYSICAL_OFFSET;

typedef struct _VOLUME_PHYSICAL_OFFSETS {

    //
    // Specifies one or more physical
    // locations on which the logical
    // location resides.
    //
    ULONG NumberOfPhysicalOffsets;
    VOLUME_PHYSICAL_OFFSET PhysicalOffset[ANYSIZE_ARRAY];

} VOLUME_PHYSICAL_OFFSETS, *PVOLUME_PHYSICAL_OFFSETS;

//
// IOCTL_VOLUME_PHYSICAL_TO_LOGICAL
//
// Input Buffer:
//     Structure of type VOLUME_PHYSICAL_OFFSET
//
// Output Buffer:
//     Structure of type VOLUME_LOGICAL_OFFSET
//

#endif  // NTDDI_VERSION >= NTDDI_WIN2K


#if (NTDDI_VERSION >= NTDDI_WINXP)

//
// IOCTL to determine whether  the volume
// is suitable for OS installation or not.
//

#define IOCTL_VOLUME_IS_PARTITION               CTL_CODE(IOCTL_VOLUME_BASE, 10, METHOD_BUFFERED, FILE_ANY_ACCESS)

//
// IOCTL_VOLUME_IS_PARTITION
//
// Input Buffer:
//     None
//
// Output Buffer:
//     None
//


//
// IOCTL to perform a  read operation from
// the specified plex of a mirrored volume.
//

#define IOCTL_VOLUME_READ_PLEX                  CTL_CODE(IOCTL_VOLUME_BASE, 11, METHOD_OUT_DIRECT, FILE_READ_ACCESS)

//
// IOCTL_VOLUME_READ_PLEX
//
// Input Buffer:
//     Structure of type VOLUME_READ_PLEX_INPUT
//
// Output Buffer:
//     None
//

typedef struct _VOLUME_READ_PLEX_INPUT {

    //
    // Specifies the offset within the
    // volume from where to read data.
    //
    LARGE_INTEGER ByteOffset;

    //
    // Specifies the amount of data in
    // bytes to be read in.
    //
    ULONG Length;

    //
    // Specifies the plex from which the
    // data is to be read in.
    //
    ULONG PlexNumber;

} VOLUME_READ_PLEX_INPUT, *PVOLUME_READ_PLEX_INPUT;


//
// IOCTL to modify the attributes on volumes.
//

#define IOCTL_VOLUME_SET_GPT_ATTRIBUTES         CTL_CODE(IOCTL_VOLUME_BASE, 13, METHOD_BUFFERED, FILE_ANY_ACCESS)

//
// IOCTL_VOLUME_SET_GPT_ATTRIBUTES
//
// Input Buffer:
//     Structure of type VOLUME_SET_GPT_ATTRIBUTES_INFORMATION
//
// Output Buffer:
//     None
//

typedef struct _VOLUME_SET_GPT_ATTRIBUTES_INFORMATION {

    //
    // Specifies  the  attributes that
    // are to be applied to the volume
    //
    ULONGLONG GptAttributes;

    //
    // Indicates whether  this is to be
    // undone when the handle is closed
    //
    BOOLEAN RevertOnClose;

    //
    // Indicates  whether the  attributes
    // apply  to all the  volumes  on the
    // disk that  this  volume resides on
    // Required if the disk layout is MBR
    //
    BOOLEAN ApplyToAllConnectedVolumes;

    //
    // For alignment purposes.
    //
    USHORT Reserved1;
    ULONG  Reserved2;

} VOLUME_SET_GPT_ATTRIBUTES_INFORMATION, *PVOLUME_SET_GPT_ATTRIBUTES_INFORMATION;

#endif  // NTDDI_VERSION >= NTDDI_WINXP


#if (NTDDI_VERSION >= NTDDI_VISTA)

//
// IOCTLs for bandwidth contracts
// on volumes.
//

#define IOCTL_VOLUME_GET_BC_PROPERTIES          CTL_CODE(IOCTL_VOLUME_BASE, 15, METHOD_BUFFERED, FILE_READ_ACCESS)
#define IOCTL_VOLUME_ALLOCATE_BC_STREAM         CTL_CODE(IOCTL_VOLUME_BASE, 16, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_VOLUME_FREE_BC_STREAM             CTL_CODE(IOCTL_VOLUME_BASE, 17, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

#define IOCTL_VOLUME_BC_VERSION                 1

//
// IOCTL_VOLUME_GET_BC_PROPERTIES
//
// Input Buffer:
//     Structure of type VOLUME_GET_BC_PROPERTIES_INPUT
//
// Output Buffer:
//     Structure of type VOLUME_GET_BC_PROPERTIES_OUTPUT
//

typedef struct _VOLUME_GET_BC_PROPERTIES_INPUT {

    //
    // Specifies the corresponding structure
    // version.
    //
    ULONG Version;

    //
    // For alignment purposes.
    //
    ULONG Reserved1;

    //
    // Specifies the lowest byte offset of
    // the file that is to be streamed.
    //
    ULONGLONG LowestByteOffset;

    //
    // Specifies the highest byte offset of
    // the file that is to be streamed.
    //
    ULONGLONG HighestByteOffset;

    //
    // Indicates whether requests shall be
    // comprised of reads, writes or both.
    //
    ULONG AccessType;

    //
    // Indicates whether the Io requests to
    // the file will be sequential or not.
    //
    ULONG AccessMode;

} VOLUME_GET_BC_PROPERTIES_INPUT, *PVOLUME_GET_BC_PROPERTIES_INPUT;

typedef struct _VOLUME_GET_BC_PROPERTIES_OUTPUT {

    //
    // Specifies the maximum number of requests
    // that can be scheduled per period of time.
    //
    ULONG MaximumRequestsPerPeriod;

    //
    // Specifies the minimum period that the
    // device uses when scheduling requests.
    //
    ULONG MinimumPeriod;

    //
    // Specifies the maximum transfer size supported
    // for  bandwidth contracts  on this  device. To
    // achieve the highest level of performance, all
    // requests should be of this size.
    //
    ULONGLONG MaximumRequestSize;

    //
    // Specifies the estimated time taken to
    // perform an  Io operation. This  field
    // is for informational purposes only.
    //
    ULONG EstimatedTimePerRequest;

    //
    // Specifies the number of requests that should be
    // kept outstanding.  This helps  keep the  device
    // device busy  and thus obtain maximum throughput
    // This will only be filled in if the target  file
    // has an outstanding contract.
    //
    ULONG NumOutStandingRequests;

    //
    // Specifies the required size of requests in this
    // stream.  This  will  only  be filled in  if the
    // target file has an outstanding contract.
    //
    ULONGLONG RequestSize;

} VOLUME_GET_BC_PROPERTIES_OUTPUT, *PVOLUME_GET_BC_PROPERTIES_OUTPUT;

//
// IOCTL_VOLUME_ALLOCATE_BC_STREAM
//
// Input Buffer:
//     Structure of type VOLUME_ALLOCATE_BC_STREAM_INPUT
//
// Output Buffer:
//     Structure of type VOLUME_ALLOCATE_BC_STREAM_OUTPUT
//

typedef struct _VOLUME_ALLOCATE_BC_STREAM_INPUT {

    //
    // Specifies the corresponding structure
    // version.
    //
    ULONG Version;

    //
    // Specifies the number of requests that
    // need to complete per period of time.
    //
    ULONG RequestsPerPeriod;

    //
    // Specifies the period of time wherein the
    // above number of requests must complete.
    //
    ULONG Period;

    //
    // Indicates whether failures should be
    // be retried or not.
    //
    BOOLEAN RetryFailures;

    //
    // Indicates whether reqests that miss
    // their deadline should  be discarded
    // or not.
    //
    BOOLEAN Discardable;

    //
    // For alignment purposes.
    //
    BOOLEAN Reserved1[2];

    //
    // Specifies the lowest byte offset of
    // the file that is to be streamed.
    //
    ULONGLONG LowestByteOffset;

    //
    // Specifies the highest byte offset of
    // the file that is to be streamed.
    //
    ULONGLONG HighestByteOffset;

    //
    // Indicates whether requests shall be
    // comprised of reads, writes or both.
    //
    ULONG AccessType;

    //
    // Indicates whether the Io requests to
    // the file will be sequential or not.
    //
    ULONG AccessMode;

} VOLUME_ALLOCATE_BC_STREAM_INPUT, *PVOLUME_ALLOCATE_BC_STREAM_INPUT;

typedef struct _VOLUME_ALLOCATE_BC_STREAM_OUTPUT {

    //
    // Specifies the required size
    // of requests in this stream.
    //
    ULONGLONG RequestSize;

    //
    // Specifies the number of requests that should be
    // kept outstanding.  This helps  keep the  device
    // device busy and thus obtain maximum throughput.
    //
    ULONG NumOutStandingRequests;

} VOLUME_ALLOCATE_BC_STREAM_OUTPUT, *PVOLUME_ALLOCATE_BC_STREAM_OUTPUT;

//
// IOCTL_VOLUME_FREE_BC_STREAM
//
// Input Buffer:
//     None
//
// Output Buffer:
//     None
//


//
// IOCTL to differentiate between basic and
// dynamic volumes.
//

#define IOCTL_VOLUME_IS_DYNAMIC                 CTL_CODE(IOCTL_VOLUME_BASE, 18, METHOD_BUFFERED, FILE_ANY_ACCESS)

//
// IOCTL_VOLUME_IS_DYNAMIC
//
// Input Buffer:
//     None
//
// Output Buffer:
//     Structure of type BOOLEAN
//


//
// IOCTL to inform the storage stack of
// runs that contain a special file.
//

#define IOCTL_VOLUME_PREPARE_FOR_CRITICAL_IO    CTL_CODE(IOCTL_VOLUME_BASE, 19, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

//
// IOCTL_VOLUME_PREPARE_FOR_CRITICAL_IO
//
// Input Buffer:
//     Structure of type VOLUME_CRITICAL_IO
//
// Output Buffer:
//     None
//

typedef struct _FILE_EXTENT {

    ULONGLONG VolumeOffset;
    ULONGLONG ExtentLength;

} FILE_EXTENT, *PFILE_EXTENT;

typedef struct _VOLUME_CRITICAL_IO {

    //
    // Indicates whether requests shall be
    // comprised of reads, writes or both.
    //
    ULONG AccessType;

    //
    // Count of the number of extents that
    // are contained within this structure.
    //
    ULONG ExtentsCount;

    //
    // Specifies the extents along which
    // the critical Io will be performed.
    //
    FILE_EXTENT Extents[ANYSIZE_ARRAY];

} VOLUME_CRITICAL_IO, *PVOLUME_CRITICAL_IO;


//
// IOCTL to provide hints to the file system
// as to where to place new file allocations.
//

#define IOCTL_VOLUME_QUERY_ALLOCATION_HINT      CTL_CODE(IOCTL_VOLUME_BASE, 20, METHOD_OUT_DIRECT, FILE_READ_ACCESS)

//
// IOCTL_VOLUME_QUERY_ALLOCATION_HINT
//
// Input Buffer:
//     Structure of type VOLUME_ALLOCATION_HINT_INPUT
//
// Output Buffer:
//     Structure of type VOLUME_ALLOCATION_HINT_OUTPUT
//

typedef struct _VOLUME_ALLOCATION_HINT_INPUT {

    //
    // Specifies the granularity of
    // the bitmap being requested.
    //
    ULONG ClusterSize;

    //
    // Specifies the count of bits
    // that are being requested.
    //
    ULONG NumberOfClusters;

    //
    // Specifies the first cluster
    // that is to be evaluated.
    //
    LONGLONG StartingClusterNumber;

} VOLUME_ALLOCATION_HINT_INPUT, *PVOLUME_ALLOCATION_HINT_INPUT;

typedef struct _VOLUME_ALLOCATION_HINT_OUTPUT {

    //
    // Specifies the bits, 1 per cluster
    // indicating all of the good places
    // where file data  may be allocated
    // These bits are represented by a 0.
    //
    ULONG Bitmap[ANYSIZE_ARRAY];

} VOLUME_ALLOCATION_HINT_OUTPUT, *PVOLUME_ALLOCATION_HINT_OUTPUT;


//
// IOCTL to inform  the storage stack to
// discard and  re-query any information
// about the specified volume that might
// have been cached.
//

#define IOCTL_VOLUME_UPDATE_PROPERTIES          CTL_CODE(IOCTL_VOLUME_BASE, 21, METHOD_BUFFERED, FILE_ANY_ACCESS)

//
// IOCTL_VOLUME_UPDATE_PROPERTIES
//
// Input Buffer:
//     None
//
// Output Buffer:
//     None
//


//
// IOCTLs to coordinate a shrink operation
// with the drivers in the storage stack.
//

#define IOCTL_VOLUME_QUERY_MINIMUM_SHRINK_SIZE  CTL_CODE(IOCTL_VOLUME_BASE, 22, METHOD_BUFFERED, FILE_READ_ACCESS)
#define IOCTL_VOLUME_PREPARE_FOR_SHRINK         CTL_CODE(IOCTL_VOLUME_BASE, 23, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

//
// IOCTL_VOLUME_QUERY_MINIMUM_SHRINK_SIZE
//
// Input Buffer:
//     None
//
// Output Buffer:
//     Structure of type VOLUME_SHRINK_INFO
//

//
// IOCTL_VOLUME_PREPARE_FOR_SHRINK
//
// Input Buffer:
//     Structure of type VOLUME_SHRINK_INFO
//
// Output Buffer:
//     None
//

typedef struct _VOLUME_SHRINK_INFO {

    //
    // Specifies the size in bytes to
    // which the volume  can be or is
    // going to be reduced down to.
    //
    ULONGLONG VolumeSize;

} VOLUME_SHRINK_INFO, *PVOLUME_SHRINK_INFO;

#endif  // NTDDI_VERSION >= NTDDI_VISTA

#ifdef __cplusplus
}
#endif

#endif  // _NTDDVOL_

