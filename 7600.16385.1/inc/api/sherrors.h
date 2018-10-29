/*****************************************************************************\
*                                                                             *
* sherror.h -  error codes specific to shell APIs                             *
*                                                                             *
* Copyright (c) Microsoft Corporation. All rights reserved.                   *
*                                                                             *
\*****************************************************************************/

#ifndef _SHERROR_
#define _SHERROR_
#if defined (_MSC_VER) && (_MSC_VER >= 1020) && !defined(__midl)
#pragma once
#endif
#include <winerror.h>

//  COPYENGINE codes use FACILITY_SHELL and 0 in the second lowest byte
// Success/Informational codes
#define COPYENGINE_S_YES                         _HRESULT_TYPEDEF_(0x00270001L)
#define COPYENGINE_S_NOT_HANDLED                 _HRESULT_TYPEDEF_(0x00270003L)
#define COPYENGINE_S_USER_RETRY                  _HRESULT_TYPEDEF_(0x00270004L)
#define COPYENGINE_S_USER_IGNORED                _HRESULT_TYPEDEF_(0x00270005L)
#define COPYENGINE_S_MERGE                       _HRESULT_TYPEDEF_(0x00270006L)
#define COPYENGINE_S_DONT_PROCESS_CHILDREN       _HRESULT_TYPEDEF_(0x00270008L)
#define COPYENGINE_S_ALREADY_DONE                _HRESULT_TYPEDEF_(0x0027000AL)
#define COPYENGINE_S_PENDING                     _HRESULT_TYPEDEF_(0x0027000BL)
#define COPYENGINE_S_KEEP_BOTH                   _HRESULT_TYPEDEF_(0x0027000CL)
#define COPYENGINE_S_CLOSE_PROGRAM               _HRESULT_TYPEDEF_(0x0027000DL) // Close the program using the current file
#define COPYENGINE_S_COLLISIONRESOLVED           _HRESULT_TYPEDEF_(0x0027000EL) // Returned by a transfer source when a collision occurs
                                                                                // during a file operation but the source resolved the collision
                                                                                // on the users behalf

// Failure/Error codes
#define COPYENGINE_E_USER_CANCELLED      _HRESULT_TYPEDEF_(0x80270000L)  // User wants to canceled entire job
#define COPYENGINE_E_CANCELLED           _HRESULT_TYPEDEF_(0x80270001L)  // Engine wants to canceled entire job, don't set the CANCELLED bit
#define COPYENGINE_E_REQUIRES_ELEVATION  _HRESULT_TYPEDEF_(0x80270002L)  // Need to elevate the process to complete the operation

#define COPYENGINE_E_SAME_FILE           _HRESULT_TYPEDEF_(0x80270003L)  // Source and destination file are the same
#define COPYENGINE_E_DIFF_DIR            _HRESULT_TYPEDEF_(0x80270004L)  // Trying to rename a file into a different location, use move instead
#define COPYENGINE_E_MANY_SRC_1_DEST     _HRESULT_TYPEDEF_(0x80270005L)  // One source specified, multiple destinations

#define COPYENGINE_E_DEST_SUBTREE        _HRESULT_TYPEDEF_(0x80270009L)  // The destination is a sub-tree of the source
#define COPYENGINE_E_DEST_SAME_TREE      _HRESULT_TYPEDEF_(0x8027000AL)  // The destination is the same folder as the source

#define COPYENGINE_E_FLD_IS_FILE_DEST    _HRESULT_TYPEDEF_(0x8027000BL)  // Existing destination file with same name as folder
#define COPYENGINE_E_FILE_IS_FLD_DEST    _HRESULT_TYPEDEF_(0x8027000CL)  // Existing destination folder with same name as file

#define COPYENGINE_E_FILE_TOO_LARGE      _HRESULT_TYPEDEF_(0x8027000DL)  // File too large for destination file system
#define COPYENGINE_E_REMOVABLE_FULL      _HRESULT_TYPEDEF_(0x8027000EL)  // Destination device is full and happens to be removable

#define COPYENGINE_E_DEST_IS_RO_CD       _HRESULT_TYPEDEF_(0x8027000FL)  // Destination is a Read-Only CDRom, possibly unformatted
#define COPYENGINE_E_DEST_IS_RW_CD       _HRESULT_TYPEDEF_(0x80270010L)  // Destination is a Read/Write CDRom, possibly unformatted
#define COPYENGINE_E_DEST_IS_R_CD        _HRESULT_TYPEDEF_(0x80270011L)  // Destination is a Recordable (AudioL) CDRom, possibly unformatted

#define COPYENGINE_E_DEST_IS_RO_DVD      _HRESULT_TYPEDEF_(0x80270012L)  // Destination is a Read-Only DVD, possibly unformatted
#define COPYENGINE_E_DEST_IS_RW_DVD      _HRESULT_TYPEDEF_(0x80270013L)  // Destination is a Read/Wrote DVD, possibly unformatted
#define COPYENGINE_E_DEST_IS_R_DVD       _HRESULT_TYPEDEF_(0x80270014L)  // Destination is a Recordable (AudioL) DVD, possibly unformatted

#define COPYENGINE_E_SRC_IS_RO_CD        _HRESULT_TYPEDEF_(0x80270015L)  // Source is a Read-Only CDRom, possibly unformatted
#define COPYENGINE_E_SRC_IS_RW_CD        _HRESULT_TYPEDEF_(0x80270016L)  // Source is a Read/Write CDRom, possibly unformatted
#define COPYENGINE_E_SRC_IS_R_CD         _HRESULT_TYPEDEF_(0x80270017L)  // Source is a Recordable (AudioL) CDRom, possibly unformatted

#define COPYENGINE_E_SRC_IS_RO_DVD       _HRESULT_TYPEDEF_(0x80270018L)  // Source is a Read-Only DVD, possibly unformatted
#define COPYENGINE_E_SRC_IS_RW_DVD       _HRESULT_TYPEDEF_(0x80270019L)  // Source is a Read/Wrote DVD, possibly unformatted
#define COPYENGINE_E_SRC_IS_R_DVD        _HRESULT_TYPEDEF_(0x8027001AL)  // Source is a Recordable (AudioL) DVD, possibly unformatted

#define COPYENGINE_E_INVALID_FILES_SRC   _HRESULT_TYPEDEF_(0x8027001BL)  // Invalid source path
#define COPYENGINE_E_INVALID_FILES_DEST  _HRESULT_TYPEDEF_(0x8027001CL)  // Invalid destination path
#define COPYENGINE_E_PATH_TOO_DEEP_SRC   _HRESULT_TYPEDEF_(0x8027001DL)  // Source Files within folders where the overall path is longer than MAX_PATH
#define COPYENGINE_E_PATH_TOO_DEEP_DEST  _HRESULT_TYPEDEF_(0x8027001EL)  // Destination files would be within folders where the overall path is longer than MAX_PATH
#define COPYENGINE_E_ROOT_DIR_SRC        _HRESULT_TYPEDEF_(0x8027001FL)  // Source is a root directory, cannot be moved or renamed
#define COPYENGINE_E_ROOT_DIR_DEST       _HRESULT_TYPEDEF_(0x80270020L)  // Destination is a root directory, cannot be renamed
#define COPYENGINE_E_ACCESS_DENIED_SRC   _HRESULT_TYPEDEF_(0x80270021L)  // Security problem on source
#define COPYENGINE_E_ACCESS_DENIED_DEST  _HRESULT_TYPEDEF_(0x80270022L)  // Security problem on destination
#define COPYENGINE_E_PATH_NOT_FOUND_SRC  _HRESULT_TYPEDEF_(0x80270023L)  // Source file does not exist, or is unavailable
#define COPYENGINE_E_PATH_NOT_FOUND_DEST _HRESULT_TYPEDEF_(0x80270024L)  // Destination file does not exist, or is unavailable
#define COPYENGINE_E_NET_DISCONNECT_SRC  _HRESULT_TYPEDEF_(0x80270025L)  // Source file is on a disconnected network location
#define COPYENGINE_E_NET_DISCONNECT_DEST        _HRESULT_TYPEDEF_(0x80270026L)  // Destination file is on a disconnected network location
#define COPYENGINE_E_SHARING_VIOLATION_SRC      _HRESULT_TYPEDEF_(0x80270027L)  // Sharing Violation on source
#define COPYENGINE_E_SHARING_VIOLATION_DEST     _HRESULT_TYPEDEF_(0x80270028L)  // Sharing Violation on destination

#define COPYENGINE_E_ALREADY_EXISTS_NORMAL      _HRESULT_TYPEDEF_(0x80270029L) // Destination exists, cannot replace
#define COPYENGINE_E_ALREADY_EXISTS_READONLY    _HRESULT_TYPEDEF_(0x8027002AL) // Destination with read-only attribute exists, cannot replace
#define COPYENGINE_E_ALREADY_EXISTS_SYSTEM      _HRESULT_TYPEDEF_(0x8027002BL) // Destination with system attribute exists, cannot replace
#define COPYENGINE_E_ALREADY_EXISTS_FOLDER      _HRESULT_TYPEDEF_(0x8027002CL) // Destination folder exists, cannot replace
#define COPYENGINE_E_STREAM_LOSS                _HRESULT_TYPEDEF_(0x8027002DL) // Secondary Stream information would be lost
#define COPYENGINE_E_EA_LOSS                    _HRESULT_TYPEDEF_(0x8027002EL) // Extended Attributes would be lost
#define COPYENGINE_E_PROPERTY_LOSS              _HRESULT_TYPEDEF_(0x8027002FL) // Property would be lost
#define COPYENGINE_E_PROPERTIES_LOSS            _HRESULT_TYPEDEF_(0x80270030L) // Properties would be lost
#define COPYENGINE_E_ENCRYPTION_LOSS            _HRESULT_TYPEDEF_(0x80270031L) // Encryption would be lost
#define COPYENGINE_E_DISK_FULL                  _HRESULT_TYPEDEF_(0x80270032L) // Entire operation likely won't fit
#define COPYENGINE_E_DISK_FULL_CLEAN            _HRESULT_TYPEDEF_(0x80270033L) // Entire operation likely won't fit, clean-up wizard available
#define COPYENGINE_E_EA_NOT_SUPPORTED           _HRESULT_TYPEDEF_(0x80270034L) // Volume does not support Extended Attributes
#define COPYENGINE_E_CANT_REACH_SOURCE          _HRESULT_TYPEDEF_(0x80270035L) // Can't reach source folder

#define COPYENGINE_E_RECYCLE_UNKNOWN_ERROR      _HRESULT_TYPEDEF_(0x80270035L) // ???
#define COPYENGINE_E_RECYCLE_FORCE_NUKE         _HRESULT_TYPEDEF_(0x80270036L) // Recycling not available (usually turned offL)
#define COPYENGINE_E_RECYCLE_SIZE_TOO_BIG       _HRESULT_TYPEDEF_(0x80270037L) // Item is too large for the recycle-bin
#define COPYENGINE_E_RECYCLE_PATH_TOO_LONG      _HRESULT_TYPEDEF_(0x80270038L) // Folder is too deep to fit in the recycle-bin
#define COPYENGINE_E_RECYCLE_BIN_NOT_FOUND      _HRESULT_TYPEDEF_(0x8027003AL) // Recycle bin could not be found or is unavailable
#define COPYENGINE_E_NEWFILE_NAME_TOO_LONG      _HRESULT_TYPEDEF_(0x8027003BL) // Name of the new file being created is too long
#define COPYENGINE_E_NEWFOLDER_NAME_TOO_LONG    _HRESULT_TYPEDEF_(0x8027003CL) // Name of the new folder being created is too long
#define COPYENGINE_E_DIR_NOT_EMPTY              _HRESULT_TYPEDEF_(0x8027003DL) // The directory being processed is not empty
#define COPYENGINE_E_FAT_MAX_IN_ROOT            _HRESULT_TYPEDEF_(0x8027003EL) // A Fat drive cannot only store and rename a limited number of items on the root.
#define COPYENGINE_E_ACCESSDENIED_READONLY      _HRESULT_TYPEDEF_(0x8027003FL) // The item cannot be modified because it is set to readonly.

#define COPYENGINE_E_REDIRECTED_TO_WEBPAGE      _HRESULT_TYPEDEF_(0x80270040L) // The server redirected the download request to a web page.
#define COPYENGINE_E_SERVER_BAD_FILE_TYPE       _HRESULT_TYPEDEF_(0x80270041L) // The server returned data with an unexpected MIME type or extension.

//  error codes without a more specific group use FACILITY_SHELL and 0x01 in the second lowest byte.
#define NETCACHE_E_NEGATIVE_CACHE           _HRESULT_TYPEDEF_(0x80270100L) // The item requested is in the negative net parsing cache
#define EXECUTE_E_LAUNCH_APPLICATION        _HRESULT_TYPEDEF_(0x80270101L) // for returned by command delegates to indicate that they did no work 
#define SHELL_E_WRONG_BITDEPTH              _HRESULT_TYPEDEF_(0x80270102L) // returned when trying to create a thumbnail extractor at too low a bitdepth for high fidelity
#define LINK_E_DELETE                       _HRESULT_TYPEDEF_(0x80270103L) // returned from IShellLink::Resolve when SLR_OFFER_DELETE_WITHOUT_FILE is passed and the user requested to delete the item

// Library error/failure code
#define LIBRARY_E_NO_SAVE_LOCATION          _HRESULT_TYPEDEF_(0x80270200L)
#define LIBRARY_E_NO_ACCESSIBLE_LOCATION    _HRESULT_TYPEDEF_(0x80270201L)

#endif //  _SHERROR_


