#include "miniport.h"  //
#include "ata.h"  //
#include "irb.h"  // contains IDE_CONTROLLER_INTERFACE,IDE_CHANNEL_CONFIGURATION

   // contains function definitions for this file

#include "FIS.h"
#include "ahci_reg.h"
#include "state.h" // contains AHCI_CONTROLLER_EXTENSION and AHCI_CHANNEL_EXTENSION
#include "entrypts.h"
#include "pwrmgmt.h"
#include "hbastat.h"
#include "util.h"
#include "io.h"

