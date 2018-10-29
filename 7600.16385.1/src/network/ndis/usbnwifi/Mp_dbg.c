/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:
    Mp_dbg.c

Abstract:
    Debugging functions
    
Revision History:
      When        What
    ----------    ----------------------------------------------
    08-01-2005    Created

Notes:

--*/
#include "Precomp.h"

#if DOT11_TRACE_ENABLED
#include "Mp_Dbg.tmh"
#endif


#if DBG

/**
 * This SpinLock is used to synchronize the list of allocated memory blocks.
 * This is a debug version only feature
 */
NDIS_SPIN_LOCK      GlobalMemoryLock;

/**
 * This is a list of all the memory allocations that were made by the driver.
 * This is a debug version only feature.
 */
LIST_ENTRY          GlobalMemoryList;

/** 
 * This variable is set if the memory mananger has been initialized.
 * This is available only for debug version of the driver
 */
BOOLEAN             GlobalMemoryManagerInitialized = FALSE;

/**
 * This is a debug only function that performs memory management operations for us.
 * Memory allocated using this function is tracked, flagged when leaked, and caught for
 * overflows and underflows.
 * \warning Do not use this function directly. Using MP_ALLOCATE_MEMORY ensures that
 * this function gets called for debug version of the driver. Retail builds will use Ndis API
 * for allocation of memory
 * 
 * \param ulSize            The size in bytes of memory to allocate
 * \param pstrFileName  The full path of file where this function is invoked from
 * \param ulLineNumber The line number in the file where this method was called from
 * \param ulFlags           Flags for special memory insturctions. Currently unused.
 * \return Pointer to the allocated memory or NULL in case of a failure
 * \sa MpFreeMemory, MP_ALLOCATE_MEMORY, MP_FREE_MEMORY, NdisAllocateMemoryWithTagPriority, MpFreeAllocatedBlocks
 */
PVOID 
MpAllocateMemory (
      NDIS_HANDLE           AllocateHandle,
      ULONG                 ulSize,
      ULONG                 ulTag,
      EX_POOL_PRIORITY      Priority,
      PCSTR                 pstrFileName,
      ULONG                 ulLineNumber,
      ULONG                 ulFlags
      )
{
   PVOID   pMemory = NULL;
    
#ifdef SIMULATE_LOW_MEMORY_RESOURCES

   //
   // Ensure that loss percentage is valid. Post a compiler error if it is not
   //
   #if SIMULATE_LOW_MEMORY_RESOURCES <= 0
      #error The defined value of SIMULATE_LOW_MEMORY_RESOURCES in sources must be greater than zero
   #elif SIMULATE_LOW_MEMORY_RESOURCES > 100
      #error The defined value of SIMULATE_LOW_MEMORY_RESOURCES in sources must be less than 100
   #endif

   //
   // Generate a random number and see if this allocation request should be failed
   //
   if ((MP_GENERATE_RANDOM_INTEGER(1000) % (1000 / SIMULATE_LOW_MEMORY_RESOURCES)) == 0)
   {
      // Fail the request
      return NULL;
   }
   
#endif   // SIMULATE_LOW_MEMORY_RESOURCES

   //
   // If the memory manager has not been initialized, do so now
   //
   if (!GlobalMemoryManagerInitialized)
   {
      //
      // NOTE: If two thread allocate the very first allocation simultaneously
      // it could cause double initialization of the memory manager. This is a
      // highly unlikely scenario and will occur in debug versions only.
      //
      NdisAllocateSpinLock(&GlobalMemoryLock);
      InitializeListHead(&GlobalMemoryList);
      GlobalMemoryManagerInitialized = TRUE;
   }
   
   //
   // Allocate the required memory chunk plus header and trailer bytes
   //
   pMemory = NdisAllocateMemoryWithTagPriority(
      AllocateHandle,
      ulSize + sizeof(MP_MEMORY_BLOCK) + sizeof(ULONG),
      ulTag,
      Priority
      );

   if (pMemory != NULL)
   {
      //
      // Memory allocation succeeded. Add information about the allocated
      // block in the list that tracks all allocations.
      //
      PMP_MEMORY_BLOCK  pMemoryBlockHeader;

      // Fill in the memory header and trailer
      pMemoryBlockHeader = (PMP_MEMORY_BLOCK) pMemory;
      pMemoryBlockHeader->pstrFile = (PCHAR)pstrFileName;
      pMemoryBlockHeader->ulLine = ulLineNumber;
      pMemoryBlockHeader->ulLength = ulSize;
      pMemoryBlockHeader->ulFlags = ulFlags;
      pMemoryBlockHeader->ulHeaderPattern = MP_HEADER_PATTERN;
      *((PULONG) (((PUCHAR)(pMemory))+ulSize + sizeof(MP_MEMORY_BLOCK))) = MP_TRAILER_PATTERN;

      // Jump ahead by memory header so pointer returned to caller points at the right place
      pMemory = ((PUCHAR)pMemory) + sizeof (MP_MEMORY_BLOCK);

      // Store a reference to this block in the list
      NdisAcquireSpinLock (&GlobalMemoryLock);
      InsertHeadList (&GlobalMemoryList, &pMemoryBlockHeader->ListEntry);
      NdisReleaseSpinLock (&GlobalMemoryLock);
   }
   
   return pMemory;
}



/**
 * This routine is called to free memory which was previously allocated using MpAllocateMemory function.
 * Before freeing the memory, this function checks and makes sure that no overflow or underflows have
 * happened and will also try to detect multiple frees of the same memory chunk.
 * 
 * \warning Do not use this function directly. Using MP_FREE_MEMORY ensures that
 * this function gets called for debug version of the driver. Retail builds will use Ndis API
 * for freeing of memory
 * \param pMemory    Pointer to memory allocated using MP_FREE_MEMORY
 * \sa MpAllocateMemory, MP_ALLOCATE_MEMORY, MP_FREE_MEMORY, NdisFreeMemory, MpFreeAllocatedBlocks
 */
VOID MpFreeMemory (
   PVOID    pMemory
   )
{
   PMP_MEMORY_BLOCK  pMemoryBlockHeader;

   MPASSERTMSG ("NULL memory being freed", pMemory);
   MPASSERTMSG ("Allocated blocks list is empty. This is an extra free\n", !IsListEmpty(&GlobalMemoryList));

   // Jump back by memory header size so we can get to the header
   pMemoryBlockHeader = (PMP_MEMORY_BLOCK) (((PUCHAR)pMemory) - sizeof(MP_MEMORY_BLOCK));
   
   //
   // Check that header was not corrupted
   //
   if (pMemoryBlockHeader->ulHeaderPattern != MP_HEADER_PATTERN)
   {
      if (pMemoryBlockHeader->ulHeaderPattern == MP_FREED_PATTERN)
      {
         MpTrace(COMP_DBG, DBG_SERIOUS, ("Possible double free of memory block at %p\n", pMemoryBlockHeader));
      }
      else
      {
         MpTrace(COMP_DBG, DBG_SERIOUS, ("Memory corruption due to underflow detected at memory block %p\n", pMemoryBlockHeader));
      }
      
      MpTrace(COMP_DBG, DBG_SERIOUS, ("Dumping information about memory block. This information may itself have been corrupted and could cause machine to bugcheck.\n"));
      MpTrace(COMP_DBG, DBG_SERIOUS, ("Memory was allocated from %s at line %d\n", pMemoryBlockHeader->pstrFile, pMemoryBlockHeader->ulLength));
      MPASSERT (FALSE);
   }

   //
   // Now corrupt the header so that double frees will fail.
   // Note simultaneous frees of same memory will not get caught this way!
   //
   pMemoryBlockHeader->ulHeaderPattern = MP_FREED_PATTERN;

   //
   // Check that trailer was not corrupted
   //
   if (*(PULONG) ((PUCHAR)pMemory + pMemoryBlockHeader->ulLength) != MP_TRAILER_PATTERN)
   {
      MpTrace(COMP_DBG, DBG_SERIOUS, ("Memory corruption due to overflow detected at %p\n", pMemory));
      MpTrace(COMP_DBG, DBG_SERIOUS, ("Dumping information about memory block. This information may itself have been corrupted and could cause machine to bugcheck.\n"));
      MpTrace(COMP_DBG, DBG_SERIOUS, ("Memory was allocated from %s at line %d\n", pMemoryBlockHeader->pstrFile, pMemoryBlockHeader->ulLength));
      MPASSERT (FALSE);
   }
   
   //
   // Remove this memory block from the list of allocations
   //
   NdisAcquireSpinLock (&GlobalMemoryLock);
   RemoveEntryList (&pMemoryBlockHeader->ListEntry);
   NdisReleaseSpinLock (&GlobalMemoryLock);

   //
   // Zero out data and trailer and then free the memory chunk back to the OS
   //
   NdisZeroMemory (pMemory, sizeof (ULONG) + (pMemoryBlockHeader->ulLength));
   NdisFreeMemory (pMemoryBlockHeader, 0, 0);
}


/**
 * This function will dump out the contents of the GlobalMemoryList and free them as well.
 * Used to dump out leaking memory when driver is exiting
 * 
 * \sa MpAllocateMemory, MpFreeMemory
 */
VOID MpFreeAllocatedBlocks (
    VOID
    )
{
   if (GlobalMemoryManagerInitialized)
   {
      NdisAcquireSpinLock (&GlobalMemoryLock);   
      {
         if (!IsListEmpty (&GlobalMemoryList))
         {
            PLIST_ENTRY pCurrentEntry;
            PMP_MEMORY_BLOCK pCurrentMemory;

            while (!IsListEmpty (&GlobalMemoryList))
            {
               pCurrentEntry = RemoveHeadList(&GlobalMemoryList);

               pCurrentMemory = CONTAINING_RECORD (pCurrentEntry, MP_MEMORY_BLOCK, ListEntry);
               MpTrace(COMP_DBG, DBG_SERIOUS, ("LEAK in %s on line %u\n", pCurrentMemory->pstrFile,
                                      pCurrentMemory->ulLine));

               NdisFreeMemory (pCurrentMemory, 0, 0);
            }
         }
      }
      NdisReleaseSpinLock (&GlobalMemoryLock);

      NdisFreeSpinLock(&GlobalMemoryLock);
      GlobalMemoryManagerInitialized = FALSE;
   }
}

#if !DOT11_TRACE_ENABLED

//
// If tracing is not turned on, Initialize these global macro
//
//ULONG     GlobalDebugLevel = DBG_SERIOUS;
ULONG       GlobalDebugLevel = DBG_NORMAL;    //todo
ULONG       GlobalDebugComponents = (COMP_EVENTS | COMP_SEND | COMP_RECV  | COMP_DBG   | COMP_SCAN | COMP_INIT_PNP | 
                                     COMP_POWER | COMP_ASSOC | COMP_MISC | COMP_OID | COMP_PS_PACKETS);

   /**
 * This helper function uses NDIS API to read debug mask information from the
 * registry (and uses defaults it not available in the registry)
 *
 * \param pAdapter          The adapter whose configuration will be read
 * \sa 
 */
VOID MpReadGlobalDebugMask(
    NDIS_HANDLE  NdisMiniportDriverHandle
    )
{
    NDIS_STATUS                     ndisStatus = NDIS_STATUS_SUCCESS;
    PNDIS_CONFIGURATION_PARAMETER   Parameter = NULL;
    NDIS_HANDLE                     RegistryConfigurationHandle;
    NDIS_CONFIGURATION_OBJECT       ConfigObject;
    NDIS_STRING                     DebugLevelRegName = NDIS_STRING_CONST("MPGlobalDebugLevel");
    NDIS_STRING                     DebugComponentRegName = NDIS_STRING_CONST("MPGlobalDebugComponents");
    
    ConfigObject.Header.Type = NDIS_OBJECT_TYPE_CONFIGURATION_OBJECT;
    ConfigObject.Header.Revision = NDIS_CONFIGURATION_OBJECT_REVISION_1;
    ConfigObject.Header.Size = sizeof( NDIS_CONFIGURATION_OBJECT );
    ConfigObject.NdisHandle = NdisMiniportDriverHandle;
    ConfigObject.Flags = 0;

    ndisStatus = NdisOpenConfigurationEx(
                    &ConfigObject,
                    &RegistryConfigurationHandle
                    );

    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        MpTrace (COMP_INIT_PNP, DBG_SERIOUS, ("Unable to Open Configuration. Will revert to defaults for all values\n"));

        // Defaults are globals        
        return;
    }

    //
    // Read GlobalDebugLevel
    //
    NdisReadConfiguration(
        &ndisStatus,
        &Parameter,
        RegistryConfigurationHandle,
        &DebugLevelRegName,
        NdisParameterInteger
        );
    if (ndisStatus == NDIS_STATUS_SUCCESS)
    {
        // We dont do strict verification of data (as long as its an integer)
        GlobalDebugLevel = Parameter->ParameterData.IntegerData;
        MpTrace (COMP_INIT_PNP, DBG_NORMAL, ("GlobalDebugLevel = 0x%x\n", GlobalDebugLevel));
    }

    //
    // Read GlobalDebugComponents
    //
    NdisReadConfiguration(
        &ndisStatus,
        &Parameter,
        RegistryConfigurationHandle,
        &DebugComponentRegName,
        NdisParameterInteger
        );
    if (ndisStatus == NDIS_STATUS_SUCCESS)
    {
        // We dont do strict verification of data (as long as its an integer)
        GlobalDebugComponents = Parameter->ParameterData.IntegerData;
        MpTrace (COMP_INIT_PNP, DBG_NORMAL, ("GlobalDebugComponents = 0x%x\n", GlobalDebugComponents));
    }

    //
    // Close the handle to the registry
    //
    if (RegistryConfigurationHandle)
    {
        NdisCloseConfiguration(RegistryConfigurationHandle);
    }
}
    
#endif

#endif   // DBG

