//
// Copyright (c) Microsoft Corporation. All rights reserved.
// Copyright 2005 OSR, Open Systems Resources, Inc. All rights Reserved.
// 
// Module Name:
// 
//     smscir.h
// 
// Abstract:
// 
// Author:
//
// Revision History:
//
#ifndef SmscIrH
#define SmscIrH

#include <ntddk.h>
#include <wdf.h>
#include <ntintsafe.h>
#include <initguid.h>
#include <wdmguid.h>
#include <ntintsafe.h>
#include <irclass_ioctl.h>
#include <specstrings.h>
#include <limits.h>

// disable warnings
#if _MSC_VER >= 1200
#pragma warning(push)
#endif
#pragma warning(disable:4201) // nameless struct/union
#pragma warning(disable:4214) // bit field types other than int

//
// Our device type. Randomly chosen from the "non-reserved" range
//
#define FILE_DEVICE_SMSCIR 0x0F70

// {064F8C82-77B2-445e-B85D-C4E20F942FE1}
DEFINE_GUID(GUID_DEVINTERFACE_IRPORT, 
    0x64f8c82, 0x77b2, 0x445e, 0xb8, 0x5d, 0xc4, 0xe2, 0xf, 0x94, 0x2f, 0xe1);

//
// Debugging Output Levels
//
#ifndef TRACE_LEVEL_INFORMATION
#define TRACE_LEVEL_NONE        0 // Tracing is not on
#define TRACE_LEVEL_FATAL       1 // Abnormal exit or termination
#define TRACE_LEVEL_ERROR       2 // Severe errors that need logging
#define TRACE_LEVEL_WARNING     3 // Warnings such as allocation failure
#define TRACE_LEVEL_INFORMATION 4 // Includes non-error cases(e.g.,Entry-Exit)
#define TRACE_LEVEL_VERBOSE     5 // Detailed traces from intermediate steps
#define TRACE_LEVEL_RESERVED6   6
#define TRACE_LEVEL_RESERVED7   7
#define TRACE_LEVEL_RESERVED8   8
#define TRACE_LEVEL_RESERVED9   9
#endif // TRACE_LEVEL_INFORMATION

//
// WPP_TRACING is enabled or disabled in the SOURCES file
//
#ifdef WPP_TRACING

#define WPP_CONTROL_GUIDS \
  WPP_DEFINE_CONTROL_GUID(SMSCIRTRACE,(E4FC8EBB,5287,4aa5,8585,C524C9A00DCC), \
    WPP_DEFINE_BIT(SMSCDBG_PNP_INFO)                \
    WPP_DEFINE_BIT(SMSCDBG_IOCTL_INFO)              \
    WPP_DEFINE_BIT(SMSCDBG_CREATE_CLOSE_INFO)       \
    WPP_DEFINE_BIT(SMSCDBG_RECEIVE_INFO)            \
    WPP_DEFINE_BIT(SMSCDBG_INIT_INFO)               \
    WPP_DEFINE_BIT(SMSCDBG_WAKE_INFO)               \
    WPP_DEFINE_BIT(SMSCDBG_TRANSMIT_INFO)           \
    )

#define WPP_LEVEL_FLAGS_LOGGER(lvl,flags) \
    WPP_LEVEL_LOGGER(flags)

#define WPP_LEVEL_FLAGS_ENABLED(lvl, flags) \
    (WPP_LEVEL_ENABLED(flags) && WPP_CONTROL(WPP_BIT_ ## flags).Level >= lvl)

#else // WPP_TRACING

extern ULONG DefaultDebugLevel;
extern ULONG DefaultDebugFlags;

#define SMSCDBG_PNP_INFO                0x00000001
#define SMSCDBG_IOCTL_INFO              0x00000002
#define SMSCDBG_CREATE_CLOSE_INFO       0x00000004
#define SMSCDBG_RECEIVE_INFO            0x00000008
#define SMSCDBG_INIT_INFO               0x00000010
#define SMSCDBG_WAKE_INFO               0x00000020
#define SMSCDBG_TRANSMIT_INFO           0x00000040

#define SmscIrTracePrint(_level_,_flag_,_msg_)  \
    if (DefaultDebugLevel >= (_level_) &&       \
        DefaultDebugFlags & (_flag_)) {         \
        DbgPrint ("SMSCIR: ");                  \
        DbgPrint _msg_;                         \
        DbgPrint("\n");                         \
    }

#endif //WPP_TRACING




//
// Register definitions
//

///////////////////////////////////////////////
// Definitions for Consumer IR RECEIVE block //
///////////////////////////////////////////////

//
// Naster block control available at address 7
//  of all register blocks
//
#define CIRCC2_MASTER_BLOCK_CONTROL         7


//
// Register block 0 registers
//
#define CIRCC2_DATA                         0
#define CIRCC2_INTERRUPT_ID                 1
#define CIRCC2_INTERRUPT_ENABLE             2
#define CIRCC2_LINE_STATUS_R                3
#define CIRCC2_LINE_STATUS_ADDRESS_W        3
#define CIRCC2_LINE_CONTROL_A               4
#define CIRCC2_LINE_CONTROL_B               5
#define CIRCC2_BUS_STATUS                   6


//
// Register block 1 registers
//
#define CIRCC2_SCE_CONFIG_A                 0
#define CIRCC2_SCE_CONFIG_B                 1
#define CIRCC2_FIFO_THRESHOLD               2
#define CIRCC2_FIFO_COUNT                   3
#define CIRCC2_MSG_BYTE_COUNT_LOW           4
#define CIRCC2_MSG_BYTE_COUNT_HIGH          5
#define CIRCC2_SCE_CONFIG_C                 6

//
// Register block 2 registers
//
#define CIRCC2_CONSUMER_IR_CTRL             0
#define CIRCC2_CONSUMER_IR_CARRIER          1
#define CIRCC2_CONSUMER_IR_BIT_RATE         2
#define CIRCC2_CUSTOM_CODE                  3
#define CIRCC2_CUSTOM_CODE_PRIME            4
#define CIRCC2_DATA_CODE                    5
#define CIRCC2_CONSUMER_PROTOCOL_SELECT     6

//
// Register block 3 registers
//
#define CIRCC2_SMSC_ID_HIGH                 0
#define CIRCC2_SMSC_ID_LOW                  1
#define CIRCC2_CHIP_ID                      2
#define CIRCC2_VERSION_NUMBER               3
#define CIRCC2_IRQ_LEVEL_DMA_CHANNEL        4
#define CIRCC2_SOFTWARE_SELECT_A            5
//
// Software Select A is also a special "was system
//  woken by remote" BOOLEAN register. If the 
//  device was used to wake the system, this register
//  will have a value of 1, 0 otherwise.
//
#define CIRCC2_WAS_REMOTE_WAKE              5
#define CIRCC2_SOFTWARE_SELECT_B            6

//
// Register block 4 registers
//
#define CIRCC2_RX_DATA_SIZE_HIGH            5
#define CIRCC2_RX_DATA_SIZE_LOW             6

//
// Register block 5 registers
//
#define CIRCC2_HALF_DUPLEX_TIMEOUT          1
#define CIRCC2_SCE_TRANSMIT_DELAY_TIMER     2
#define CIRCC2_CARRIER_CAPTURE_CONTROL      3
#define CIRCC2_CARRIER_CAPTURE_MEASURE      4

//
// Register block 6 registers
//
#define CIRCC2_HEADER_VALUE_AND_MASK        0
#define CIRCC2_DATA_VALUE_1                 1
#define CIRCC2_DATA_MASK_1                  2
#define CIRCC2_DATA_VALUE_2                 3
#define CIRCC2_DATA_MASK_2                  4
#define CIRCC2_DATA_VALUE_3                 5
#define CIRCC2_DATA_MASK_3                  6

//
// Register block 7 registers
//
#define CIRCC2_DATA_VALUE_4                 0
#define CIRCC2_DATA_MASK_4                  1
#define CIRCC2_DATA_VALUE_5                 2
#define CIRCC2_DATA_MASK_5                  3
#define CIRCC2_DOWN_COUNTER_CONTROL         5
#define CIRCC2_DOWN_COUNTER                 6


//
// Structure definitions to match registers of interest
//
#include <pshpack1.h>

//
// CIRCC2_MASTER_BLOCK_CONTROL
// 
typedef struct _MASTER_BLOCK_CONTROL {

    union {
        struct {
            UCHAR RegisterBlockSelect   : 3;
            UCHAR Reserved              : 1;
            UCHAR ErrorReset            : 1;
            UCHAR MasterInterruptEnable : 1;
            UCHAR MasterReset           : 1;
            UCHAR PowerDown             : 1;
        };
        UCHAR AsUChar;
    };

}MASTER_BLOCK_CONTROL, *PMASTER_BLOCK_CONTROL;
    
//
//  CIRCC2_INTERRUPT_ID
//  CIRCC2_INTERRUPT_ENABLE
//
typedef struct _INTERRUPT_ID_OR_ENABLE {

    union {
        struct {
            UCHAR Reserved          : 1;
            UCHAR CarrierCapture    : 1;
            UCHAR GpCp              : 1;
            UCHAR IrBusy            : 1;
            UCHAR Fifo              : 1;
            UCHAR RawMode           : 1;
            UCHAR Eom               : 1;
            UCHAR ActiveFrame       : 1;
        };
        UCHAR AsUChar;
    };

}INTERRUPT_ID_OR_ENABLE, *PINTERRUPT_ID_OR_ENABLE;

//
//  CIRCC2_LINE_STATUS_R
//
typedef struct _LINE_STATUS_READ {
    
    union {
        struct {
            UCHAR NECRepeat   : 1;
            UCHAR Reserved    : 1;
            UCHAR FrameAbort  : 1;
            UCHAR CRCError    : 1;
            UCHAR SizeError   : 1;
            UCHAR FrameError  : 1;
            UCHAR OverRun     : 1;
            UCHAR UnderRun    : 1;
        };
        UCHAR AsUChar;
    };

}LINE_STATUS_READ, *PLINE_STATUS_READ;


//
//  CIRCC2_LINE_STATUS_ADDRESS_W
//
typedef struct _LINE_STATUS_ADDRESS_WRITE {
    
    union {
        struct {
            UCHAR StatusRegisterAddress : 3;
            UCHAR Reserved              : 5;
        };
        UCHAR AsUChar;
    };

}LINE_STATUS_ADDRESS_WRITE, *PLINE_STATUS_ADDRESS_WRITE;

// 
//  CIRCC2_LINE_CONTROL_A
//
typedef struct _LINE_CONTROL_A {

    union {
        struct {
            UCHAR Reserved0  : 3;
            UCHAR RawRx      : 1;
            UCHAR RawTx      : 1;
            UCHAR Reserved1  : 2;
            UCHAR FifoReset  : 1;
        };
        UCHAR AsUChar;
    };

}LINE_CONTROL_A, *PLINE_CONTROL_A;

//
//  CIRCC2_LINE_CONTROL_B
//
typedef struct _LINE_CONTROL_B {

    union {
        struct {
            UCHAR MessageCount  : 4;
            UCHAR Reserved      : 2;
            UCHAR SCEModeBits   : 2;
        };
        UCHAR AsUChar;
    };

}LINE_CONTROL_B, *PLINE_CONTROL_B;

#define SCE_MODE_TRANSMIT        1
#define SCE_MODE_RECEIVE         2

//
//  CIRCC2_BUS_STATUS
//
typedef struct _BUS_STATUS {

    union {
        struct {
            UCHAR ValidFrame : 1;
            UCHAR Reserved   : 4;
            UCHAR Timeout    : 1;
            UCHAR FifoFull   : 1;
            UCHAR NotEmpty   : 1;
        };
        UCHAR AsUChar;
    };

}BUS_STATUS, *PBUS_STATUS;

//
// CIRCC2_SCE_CONFIG_A
//
typedef struct _SCE_CONFIG_A {

    union {
        struct {
            UCHAR RxPolarity   : 1;
            UCHAR TxPolarity   : 1;
            UCHAR HalfDuplex   : 1;
            UCHAR BlockControl : 4;
            UCHAR AuxIr        : 1;
        };
        UCHAR AsUChar;
    };

}SCE_CONFIG_A, *PSCE_CONFIG_A;

//
// CIRCC2_SCE_CONFIG_B
//
typedef struct _SCE_CONFIG_B {

    union {
        struct {
            UCHAR DmaEnable  : 1;
            UCHAR DmaBurst   : 1;
            UCHAR StringMove : 1;
            UCHAR NoWait     : 1;
            UCHAR LoopBack   : 2;
            UCHAR OutputMux  : 2;
        };
        UCHAR AsUChar;
    };

}SCE_CONFIG_B, *PSCE_CONFIG_B;

#define SCE_CONFIB_B_NORMAL_MUX     1
#define SCE_CONFIB_B_LEARN_MUX      2


//
// CIRCC2_SCE_CONFIG_C
//
typedef struct _SCE_CONFIG_C {

    union {
        struct {
            UCHAR DmaRefreshCount   : 2;
            UCHAR Reserved          : 4;
            UCHAR TxPulseWidthLimit : 1;
            UCHAR Reserved2         : 1;
        };
        UCHAR AsUChar;
    };

}SCE_CONFIG_C, *PSCE_CONFIG_C;


//
// CIRCC2_CONSUMER_IR_CTRL
//
typedef struct _CONSUMER_IR_CTRL {

    union {
        struct {
            UCHAR CarrierRange     : 2;
            UCHAR CarrierOff       : 1;
            UCHAR NoCareDataCode   : 1;
            UCHAR NoCareCustomCode : 1;
            UCHAR PMEWake          : 1;
            UCHAR Frame            : 1;
            UCHAR ReceiveSync      : 1;
        };
        UCHAR AsUChar;
    };

}CONSUMER_IR_CTRL, *PCONSUMER_IR_CTRL;

//
// CIRCC2_CONSUMER_PROTOCOL_SELECT
//
typedef struct _CONSUMER_PROTOCOL_SELECT {

    union {
        struct {
            UCHAR FrameType       : 3;
            UCHAR Reserved        : 2;
            UCHAR SingleByteCCode : 2;
            UCHAR RxNECRepeated   : 1;
        };
        UCHAR AsUChar;
    };

}CONSUMER_PROTOCOL_SELECT, *PCONSUMER_PROTOCOL_SELECT;

//
// CIRCC2_CARRIER_CAPTURE_CONTROL
//
typedef struct _CARRIER_CAPTURE_CONTROL {

    union {
        struct {
            UCHAR CaptureMeasurement : 1;
            UCHAR CaptureReset       : 1;
            UCHAR Reserved           : 6;
        };
        UCHAR AsUChar;
    };

}CARRIER_CAPTURE_CONTROL, *PCARRIER_CAPTURE_CONTROL;


////////////////////////////////////////////////
// Definitions for Consumer IR BLASTING block //
////////////////////////////////////////////////


//
// Naster block control available at address 7
//  of all register blocks
//
#define BIRCC2_MASTER_BLOCK_CONTROL_ADDR         7


//
// Register block 0 registers
//
#define BIRCC2_DATA_ADDR                         0
#define BIRCC2_INTERRUPT_ID_ADDR                 1
#define BIRCC2_INTERRUPT_ENABLE_ADDR             2
#define BIRCC2_LINE_CONTROL_ADDR                 3
#define BIRCC2_DOWN_COUNTER_CONTROL_ADDR         4
#define BIRCC2_DOWN_COUNTER_ADDR                 5
#define BIRCC2_TX_CHANNEL_ENABLES_ADDR           6


//
// Register block 1 registers
//
#define BIRCC2_MODE_ADDR                         0
#define BIRCC2_CONSUMER_IR_CARRIER               1
#define BIRCC2_CONSUMER_IR_BIT_RATE              2
#define BIRCC2_FIFO_THRESHOLD_ADDR               3
#define BIRCC2_FIFO_COUNT_ADDR                   4
#define BIRCC2_TX_DELAY_TIMER_ADDR               5

//
// Register block 2 registers
//
#define BIRCC2_SMSC_ID_HIGH_ADDR                 0
#define BIRCC2_SMSC_ID_LOW_ADDR                  1
#define BIRCC2_BLOCK_ID_ADDR                     2
#define BIRCC2_VERSION_ADDR                      3
#define BIRCC2_BLOCK_CONFIG_ADDR                 4
#define BIRCC2_SOFTWARE_SELECT_A_ADDR            5
#define BIRCC2_SOFTWARE_SELECT_B_ADDR            6


//
// BIRCC2_MASTER_BLOCK_CONTROL_ADDR
// 
typedef struct _BIRCC2_MASTER_BLOCK_CONTROL {

    union {
        struct {
            UCHAR RegisterBlockSelect   : 2;
            UCHAR Reserved              : 3;
            UCHAR MasterInterruptEnable : 1;
            UCHAR MasterReset           : 1;
            UCHAR PowerDown             : 1;
        };
        UCHAR AsUChar;
    };

}BIRCC2_MASTER_BLOCK_CONTROL, *PBIRCC2_MASTER_BLOCK_CONTROL;
    
//
//  BIRCC2_INTERRUPT_ID_ADDR
//  BIRCC2_INTERRUPT_ENABLE_ADDR
//
typedef struct _BIRCC2_INTERRUPT_ID_OR_ENABLE {

    union {
        struct {
            UCHAR Reserved    : 4;
            UCHAR DownCounter : 1;
            UCHAR TxActivated : 1;
            UCHAR TxComplete  : 1;
            UCHAR Fifo        : 1;
        };
        UCHAR AsUChar;
    };

}BIRCC2_INTERRUPT_ID_OR_ENABLE, *PBIRCC2_INTERRUPT_ID_OR_ENABLE;

//
// BIRCC2_LINE_CONTROL_ADDR
//
typedef struct _BIRCC2_LINE_CONTROL {

    union {
        struct {
            UCHAR TxEnable     : 1;
            UCHAR RawTx        : 1;
            UCHAR Reserved     : 2;
            UCHAR TxActive     : 1;
            UCHAR FifoReset    : 1;
            UCHAR FifoNotEmpty : 1;
            UCHAR FifoFull     : 1;
        };
        UCHAR AsUChar;
    };

}BIRCC2_LINE_CONTROL, *PBIRCC2_LINE_CONTROL;

//
// BIRCC2_DOWN_COUNTER_CONTROL_ADDR
//
typedef struct _BIRCC2_DOWN_COUNTER_CONTROL {

    union {
        struct {
            UCHAR DownCounterEnable : 1;
            UCHAR Reserved          : 7;
        };
        UCHAR AsUChar;
    };

}BIRCC2_DOWN_COUNTER_CONTROL, *PBIRCC2_DOWN_COUNTER_CONTROL;

//
// BIRCC2_TX_CHANNEL_ENABLES_ADDR
//
typedef struct _BIRCC2_TX_CHANNEL_ENABLES {

    union {
        struct {
            
            UCHAR TxChannel0Enable  : 1;
            UCHAR TxChannel1Enable  : 1;
            UCHAR TxChannel2Enable  : 1;
            UCHAR TxChannel3Enable  : 1;
            
            UCHAR TxEmitter0Present : 1;
            UCHAR TxEmitter1Present : 1;
            UCHAR TxEmitter2Present : 1;
            UCHAR TxEmitter3Present : 1;

        };
        UCHAR AsUChar;
    };

}BIRCC2_TX_CHANNEL_ENABLES, *PBIRCC2_TX_CHANNEL_ENABLES;

//
// BIRCC2_MODE_ADDR
//
typedef struct _BIRCC2_MODE {

    union {
        struct {
            UCHAR CarrierOff : 1;
            UCHAR RawMode    : 1;
            UCHAR Reserved   : 6;
        };
        UCHAR AsUChar;
    };

}BIRCC2_MODE, *PBIRCC2_MODE;

#include <poppack.h>

//
// Returns TRUE if the given bit is set in the byte, FALSE
//  otherwise
//
#define BIT_ON(_BYTE_, _BIT_)   (((_BYTE_) & (1 << (_BIT_))) ? TRUE : FALSE)

//
// This macro will turn the carrier frequency
//  divider value in the CIRCC2_CONSUMER_IR_CARRIER
//  register into a carrier frequency in Hz 
//
#define SMSCIR_CFD_TO_HZ(_CFD_) (1600000/((_CFD_)+1))

//
// Turn a usec Period value into an Frequency in KHz
//
#define SMSCIR_USEC_P_TO_KHZ_F(_PERIOD_) (1000/(_PERIOD_))

//
// Now we need to be able to take a KHz frequency and turn
//  it into a CFD (CIRCC2_CONSUMER_IR_CARRIER) value. 
//
#define SMSCIR_KHZ_TO_CFD(_KHZ_) ((1600/(_KHZ_))-1)


//
// Macro to convert the value in CIRCC2_CARRIER_CAPTURE_MEASURE to 
//  a kHZ frequency
//
#define SMSCIR_C_C_MEASURE_TO_KHZ_F(_CCM_) ((1600 / ((_CCM_) / 2)) * 1000)

//
// See runtables. for an explanation of this struct's usage
//  
typedef struct _SAMPLE_RUN {

    LONG    Result;
    LONG    BitsConsumed;
    BOOLEAN On;

}SAMPLE_RUN, *PSAMPLE_RUN;

//
// Some config constants
//
#define DEVCAPS_NUM_RECEIVE_PORTS        2
#define DEVCAPS_LEARNING_INDEX           1
#define DEVCAPS_LEARNING_MASK            2
#define DEVCAPS_TRANSMIT_PORTS           4


//
// Receive thresholds are negative, we'll set
//  this to fill the FIFO entirely before interrupting
//  to let us know
//
#define FIFO_THRESHOLD_FOR_RECEIVE       ((UCHAR)-31)


//
// +0 for transmit numbers.  We'll set this so that we
//  have to full the FIFO entirely before blasting
//
#define FIFO_THRESHOLD_FOR_TRANSMIT      0

//
// The class driver wants samples at a rate of 20kHz.
//  According to Table 2.3, this requires a bit 
//  rate divider of 4.
//
#define SMSCIR_BIT_RATE_DIVIDER          4

//
// The number of consecutive FFs that we need to
//  see before we mark it as a data end by default
//
#define SMSCIR_NUM_FFS_FOR_DATA_END      250

//
// Size of the FIFO on the CIRCC2 device
//
#define SMSCIR_FIFO_SIZE                 32

//
// The maximum buffer size that we're willing to use
//  as a local for FIFO data storage
//
#define SMSCIR_MAX_LOCAL_BUFFER_SIZE     (SMSCIR_FIFO_SIZE * 2)

//
// Sampling period to use by default for IR port drivers
//
#define IR_SAMPLE_PERIOD                 50

//
// Size of the circular buffer used to hold bytes pulled out
//  of the RX fifo
//
#define SMSCIR_FIFO_BUFFER_SIZE          0x1000

//
// Buffer size that we'll use as our circular buffer of 
//  RLC data for the class driver
//
#define RLC_RECEIVER_BUFFER_LENGTH       0x3000

//
// Period for our blasting deadman timer (in millisec)
//
#define SMSCIR_DEADMAN_TIMER_PERIOD      1500


//
// We support wake from various different protocols.
//  First we'll define a structure that contains
//  all the information we'll need to program the
//  device. We;ll then define constants for the
//  fields for currently supported protocols.
//
typedef struct _SMSCIR_ARM_FOR_WAKE_DATA {

    //
    // CIRCC2_CONSUMER_IR_CARRIER for this protocol
    //
    UCHAR CarrierDivider;

    //
    // CIRCC2_CONSUMER_IR_BIT_RATE for this protocol
    //
    UCHAR BitRateDivider;

    //
    // CIRCC2_CUSTOM_CODE
    //
    UCHAR CustomCode;

    //
    // CIRCC2_CUSTOM_CODE_PRIME
    //
    UCHAR CustomCodePrime;

    //
    // CIRCC2_DATA_CODE
    //
    UCHAR DataCode;

    //
    // CIRCC2_CONSUMER_PROTOCOL_SELECT
    //
    UCHAR ProtocolSelect;
    
    //
    // CIRCC2_HEADER_VALUE_AND_MASK
    // 
    UCHAR HeaderValueAndMask;

    //
    // CIRCC2_DATA_VALUE_1
    //
    UCHAR DataValue1;
    
    // 
    // CIRCC2_DATA_MASK_1
    //
    UCHAR DataMask1;

    //
    // CIRCC2_DATA_VALUE_2
    //
    UCHAR DataValue2;

    // 
    // CIRCC2_DATA_MASK_2
    //
    UCHAR DataMask2;

    //
    // CIRCC2_DATA_VALUE_3
    //
    UCHAR DataValue3;

    // 
    // CIRCC2_DATA_MASK_3
    //
    UCHAR DataMask3;

    //
    // CIRCC2_DATA_VALUE_4
    //
    UCHAR DataValue4;

    // 
    // CIRCC2_DATA_MASK_4
    //
    UCHAR DataMask4;

    //
    // CIRCC2_DATA_VALUE_5
    //
    UCHAR DataValue5;

    // 
    // CIRCC2_DATA_MASK_5
    //
    UCHAR DataMask5;

}SMSCIR_ARM_FOR_WAKE_DATA, *PSMSCIR_ARM_FOR_WAKE_DATA;


//
//  Constants for the fields in the SMSCIR_ARM_FOR_WAKE_DATA
//  for known protocols to wake via the POWER button.
//
//

/////////////////////
// RC6 WAKE FIELDS //
/////////////////////
#define SMSCIR_RC6_CARRIER_DIVIDER       0x2B // 2:1
#define SMSCIR_RC6_BIT_RATE_DIVIDER      0x2B // 2:2
#define SMSCIR_RC6_CUSTOM_CODE           0x80 // 2:3
#define SMSCIR_RC6_CUSTOM_CODE_PRIME     0x0F // 2:4
#define SMSCIR_RC6_DATA_CODE             0x00 // 2:5
#define SMSCIR_RC6_PROTOCOL_SELECT       0x03 // 2:6

#define SMSCIR_RC6_HEADER_VALUE_AND_MASK 0x0C // 6:0
#define SMSCIR_RC6_DATA_VALUE_1          0x04 // 6:1
#define SMSCIR_RC6_DATA_MASK_1           0x80 // 6:2
#define SMSCIR_RC6_DATA_VALUE_2          0x0C // 6:3
#define SMSCIR_RC6_DATA_MASK_2           0x00 // 6:4
#define SMSCIR_RC6_DATA_VALUE_3          0x00 // 6:5
#define SMSCIR_RC6_DATA_MASK_3           0xFF // 6:6

#define SMSCIR_RC6_DATA_VALUE_4          0x00 // 7:0
#define SMSCIR_RC6_DATA_MASK_4           0x00 // 7:1
#define SMSCIR_RC6_DATA_VALUE_5          0x00 // 7:2
#define SMSCIR_RC6_DATA_MASK_5           0x00 // 7:3


/////////////////////
// RC5 WAKE FIELDS //
/////////////////////
#define SMSCIR_RC5_CARRIER_DIVIDER       0x2B // 2:1
#define SMSCIR_RC5_BIT_RATE_DIVIDER      0x58 // 2:2
#define SMSCIR_RC5_CUSTOM_CODE           0x00 // 2:3
#define SMSCIR_RC5_CUSTOM_CODE_PRIME     0x00 // 2:4
#define SMSCIR_RC5_DATA_CODE             0x00 // 2:5
#define SMSCIR_RC5_PROTOCOL_SELECT       0x02 // 2:6

#define SMSCIR_RC5_HEADER_VALUE_AND_MASK 0xF0 // 6:0
#define SMSCIR_RC5_DATA_VALUE_1          0x0C // 6:1
#define SMSCIR_RC5_DATA_MASK_1           0xC0 // 6:2
#define SMSCIR_RC5_DATA_VALUE_2          0x00 // 6:3
#define SMSCIR_RC5_DATA_MASK_2           0x00 // 6:4
#define SMSCIR_RC5_DATA_VALUE_3          0x00 // 6:5
#define SMSCIR_RC5_DATA_MASK_3           0x00 // 6:6

#define SMSCIR_RC5_DATA_VALUE_4          0x00 // 7:0
#define SMSCIR_RC5_DATA_MASK_4           0x00 // 7:1
#define SMSCIR_RC5_DATA_VALUE_5          0x00 // 7:2
#define SMSCIR_RC5_DATA_MASK_5           0x00 // 7:3


/////////////////////
// NEC WAKE FIELDS //
/////////////////////
#define SMSCIR_NEC_CARRIER_DIVIDER       0x29 // 2:1
#define SMSCIR_NEC_BIT_RATE_DIVIDER      0x37 // 2:2
#define SMSCIR_NEC_CUSTOM_CODE           0x45 // 2:3
#define SMSCIR_NEC_CUSTOM_CODE_PRIME     0xBA // 2:4
#define SMSCIR_NEC_DATA_CODE             0x12 // 2:5
#define SMSCIR_NEC_PROTOCOL_SELECT       0x80 // 2:6

#define SMSCIR_NEC_HEADER_VALUE_AND_MASK 0x00 // 6:0
#define SMSCIR_NEC_DATA_VALUE_1          0x00 // 6:1
#define SMSCIR_NEC_DATA_MASK_1           0x00 // 6:2
#define SMSCIR_NEC_DATA_VALUE_2          0x00 // 6:3
#define SMSCIR_NEC_DATA_MASK_2           0x00 // 6:4
#define SMSCIR_NEC_DATA_VALUE_3          0x00 // 6:5
#define SMSCIR_NEC_DATA_MASK_3           0x00 // 6:6

#define SMSCIR_NEC_DATA_VALUE_4          0x00 // 7:0
#define SMSCIR_NEC_DATA_MASK_4           0x00 // 7:1
#define SMSCIR_NEC_DATA_VALUE_5          0x00 // 7:2
#define SMSCIR_NEC_DATA_MASK_5           0x00 // 7:3


//
// Location of configured wake key in the registry
//
#define SMSCIR_POWER_KEY_SUBKEY         L"PowerKey" 
#define SMSCIR_POWER_KEY_VALUE          L"PowerKey"



//
// A container that holds RLC data to be returned
//  to the class driver or a "priority" user mode
//  reader. It holds not only the RLC data to return,
//  but also the current receive requent and the 
//  list of queued receive requests
//
typedef struct _SMSCIR_RLC_RECEIVER {

    //
    // Is this the priority receiver?
    //
    BOOLEAN                     IsPriorityReceiver;


    //
    // Buffer to hold the RLC data produced by the device/driver
    //
    UCHAR                       RLCBuffer[RLC_RECEIVER_BUFFER_LENGTH];

    //
    // CurrentBufferSize is the amount of RLC data held by the receiver
    //
    ULONG                       CurrentBufferSize;

    //
    // We need to keep track of the point in the buffer
    //  where a DataEnd exists. When copying out of the 
    //  receiver, we'll use this (if set) to determine the
    //  amount of bytes to copy out and give to the caller in 
    //  place of the CurrentBufferSize
    //
    ULONG                       BytesToDataEnd;

    //
    // The RLCBuffer is circular, so this is where the next data should
    //  be queued starting from
    //
    ULONG                       NextIndex;

    //
    // When copying data out of the receiver, this is the place to look
    // 
    ULONG                       CurrentIndex;

    //
    // Spinlock for the receiver
    //
    KSPIN_LOCK                  ListSpinLock;

    //
    // Number of readers. Currently only support ONE reader.
    //
    LONG                        OpenCount;

    //
    // Queue of pending readers
    //
    WDFQUEUE                    PendingReceiveQueue;

    //
    // The currently active reader. Stays active until the
    //  buffer is exhausted or a data end occurs
    //
    WDFREQUEST                  CurrentIrReceiveRequest;

    //
    // Data buffer for the CurrentIrReceiveRequest
    //
    PUCHAR                      ReceiveBuffer;

    //
    // Size of the data buffer for the CurrentIrReceiveRequest
    //
    ULONG                       ReceiveBufferSize;

    //
    // Amount of data copied into the CurrentIrReceiveRequest
    //
    ULONG                       ReceiveCurrentOffset;
    
    union {
    
        //
        // The parameters passed to the CurrentIrReceiveRequest.
        //  Which one is used is based on the IsPriorityReceiver
        //  flag
        //
        PIR_RECEIVE_PARAMS          ReceiveParams;
        PIR_PRIORITY_RECEIVE_PARAMS PriorityReceiveParams;

    };


    //
    // Every time we calculate RLC, we save the last piece and
    //  don't give it to the reader immediately. See isr_dpc.c
    //
    LONG                        LastRLCPieceOfPreviousPacket;

    //
    // Backpointer to the device extension
    // 
    struct _SMSCIR_DATA*        DeviceData;

    //
    // Should we ignore the first period of silence?
    //
    BOOLEAN                 IgnoreFirstSilence;


}SMSCIR_RLC_RECEIVER, *PSMSCIR_RLC_RECEIVER;

#define SMSCIR_INITIALIZE_RECEIVER(__PRCV__, __DEVDATA__, __ISPRIORITY__) {   \
    (__PRCV__)->IsPriorityReceiver = (__ISPRIORITY__);                        \
    (__PRCV__)->DeviceData         = (__DEVDATA__);                           \
    (__PRCV__)->OpenCount          = 0;                                       \
    (__PRCV__)->CurrentBufferSize  = 0;                                       \
    (__PRCV__)->CurrentIndex       = 0;                                       \
    (__PRCV__)->NextIndex          = 0;                                       \
    KeInitializeSpinLock(&(__PRCV__)->ListSpinLock);                          \
}                                                                             \

#define SMSCIR_RESET_RECEIVER(__PRCV__) {   \
    (__PRCV__)->CurrentBufferSize = 0;              \
    (__PRCV__)->BytesToDataEnd    = 0;              \
    (__PRCV__)->CurrentIndex      = 0;              \
    (__PRCV__)->NextIndex         = 0;              \
    (__PRCV__)->LastRLCPieceOfPreviousPacket = 0;   \
    (__PRCV__)->IgnoreFirstSilence = FALSE;     \
}                                                   \



//
// Data structure used for blasting.
//
// When we receive a TRANSMIT request from the user, it
//  will be in RLC format. The first thing we'll need to/
//  do is unpack that RLC data into data that is appropriate
//  for our FIFO. This FIFO data will be described by a list
//  of SMSCIR_TX_FIFO_DATA structures
//
typedef struct _SMSCIR_TX_FIFO_DATA {

    LIST_ENTRY ListEntry;

    
    //
    // Length of the FifoBuffer field. This will always
    //  be a multiple of the FIFO size
    //
    ULONG      FifoBufferLength;

    //
    // How much of FifoBuffer we've blasted so far
    //
    ULONG      CurrentOffset;

    //
    // Number of times to repeat blasting the data in
    //  FifoBuffer
    //
    ULONG      RepeatCount;

    //
    // Number of times we've repeated the data in FifoBuffer
    //
    ULONG      TimesRepeated;

    //
    // The RLC data converted into ON/OFF bit samples
    //
    UCHAR      FifoBuffer[1];

}SMSCIR_TX_FIFO_DATA, *PSMSCIR_TX_FIFO_DATA;


//
// The device extension
//
typedef struct _SMSCIR_DATA {

    //
    // Our WDFDRIVER object
    // 
    WDFDRIVER              WdfDriverObject;


    //
    // Need a pointer to our interrupt object so that
    //  we can acquire our interrupt spinlock
    //
    WDFINTERRUPT           Interrupt;

    //
    // Base address for the ports we used to communicate
    //  with our device
    //
    PUCHAR                 BaseAddress;

    //
    // See pnp.c or READ/WRITE_HARDWARE_UCHAR
    //
    BOOLEAN                PortIsMapped;


    //
    // Resource information for TRANSMIT
    //
    PUCHAR                 TransmitBaseAddress;
    BOOLEAN                TransmitPortIsMapped;



    //
    // Has the class driver sent us the handshake IOCTL
    //  yet? Informational use only.
    // 
    BOOLEAN                HandshakeAccepted;

    //
    // When the device is in priority mode (due to the 
    //  receiving of an ENTER_PRIORITY_MODE IOCTL) 
    //  normal receives are starved while priority receives
    //  are processed. Once we exit priority mode (because
    //  of an EXIT_PRIORITY_MODE IOCTL) we cancel any
    //  outstanding priority receivers and switch back
    //  to normal receive mode.
    //
    BOOLEAN                InPriorityMode;

    //
    // The individual receiver containers
    // 
    SMSCIR_RLC_RECEIVER    Receiver;
    SMSCIR_RLC_RECEIVER    PriorityReceiver;
    
    //
    // The number of consecutive FFs (i.e. silence) we have seen 
    //  from the device after a receive has started. We use this
    //  to determine when a data end event occurs
    //
    ULONG                  ConsecutiveFFs;

    //
    // The number of FFs we need to see before we 
    //  consider the sample to be over. Defaults to 
    //  SMSCIR_NUM_FFS_FOR_DATA_END
    //
    ULONG                  NumFFsForDataEnd;


    //
    // There are several IOCTLs that we wish to process serially.
    //  Instead of taking out a lock to protect them against
    //  either the same IOCTL or other IOCTLs we wish to serialize
    //  against, we take request from our default parallel queue
    //  and forward them into our serial queue.
    //
    WDFQUEUE               SerialQueue;


    //
    // We need to loop in our ISR pulling data bytes out of
    //  our FIFO. Once we take a byte from the FIFO, we'll
    //  enqueue it onto a circular buffer one byte at a time
    //  The DPC will then run, take the raw FIFO data, RLC
    //  it, then enqueue it onto the currently active receiver.
    //
    // Note that we need to carry about the "BytesToDataEnd"
    //  field with us everywhere, the watermark is queued in 
    //  the FIFO buffer, then queued into the receiver buffer
    //  (see isr_dpc.c). 
    //
    UCHAR                  FifoBuffer[SMSCIR_FIFO_BUFFER_SIZE];
    ULONG                  CurrentFifoBufferSize;
    ULONG                  NextFifoIndex;
    ULONG                  CurrentFifoIndex;
    ULONG                  BytesToDataEnd;


    //
    // When we pull data from the CIRCC2 FIFO, we copy it
    //  into a SMSCIR_MAX_LOCAL_BUFFER_SIZE sized UCHAR array.
    //  When we RLC this data, we use this storage as the 
    //  worker buffer. The reason for the size is that in the
    //  worst case every byte will represent 8 on/off samples
    //  PLUS one more space for the previous sample (see isr_dpc.c)
    //
    LONG                   RLCResultBuffer[SMSCIR_MAX_LOCAL_BUFFER_SIZE * 9];

    //
    // If we're doing a learning operation (a.k.a. if we're in priority
    //  mode) then we'll need to return the carrier of the sampled
    //  IR to the reader. We'll get this info from the carrier measure
    //  register on the device, which we'll store here
    //
    UCHAR                  CarrierMeasure;

    //
    // When it comes time to blast, we're going to pre-convert the user
    //  data into something applicable for our device. We'll build
    //  the converted data into SMSCIR_TX_FIFO_DATA structures linked
    //  together via the TxFifoDataList field and protected by the 
    //  TxFifoDataLock. The structure currently being blasted is 
    //  both pointed to by CurrentTxFifoData and at the head
    //  of the TxFifoDataList
    //
    PSMSCIR_TX_FIFO_DATA   CurrentTxFifoData;
    LIST_ENTRY             TxFifoDataList;
    KSPIN_LOCK             TxFifoDataLock;
    
    //
    // We pend the user's blasting request so that we can do it 
    //  asynchronously. The pended reques tis here
    //
    WDFREQUEST             CurrentTxRequest;
    ULONG                  BytesTransmitted;

    //
    // Shadow copy of the last interrupt ID we received from the 
    //  transmitter
    //
    BIRCC2_INTERRUPT_ID_OR_ENABLE TxIntId;

    //
    // Blasting can have a tendency to underrun on a heavily loaded
    //  system. If an underrun occurs we can't necessarily detect
    //  it, so we must keep some sort of deadman in order to reset
    //  the device should something go wrong. When we first 
    //  start blasting we start the deadman and when the blasting
    //  is completed we end it. For every piece that we blast we
    //  update LastBlastSentTime in the DPC. If the device doesn't
    //  interrupt within X amount of time after that the deadman
    //  timer will notice and reset the transmitter (and potentially
    //  reset the receiver)
    //
    WDFTIMER               DeadManTimer;
    LARGE_INTEGER          LastBlastSentTime;


    //
    // The key used to wake the sytem via the CIR block is configurable.
    //  ArmForWakeData contains the information about which key will
    //  be used to wake the sytem
    //
    SMSCIR_ARM_FOR_WAKE_DATA ArmForWakeData;


}SMSCIR_DATA, *PSMSCIR_DATA;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(SMSCIR_DATA, GetIrData)



//
// Here we define the functions that we use to write to the device
// in a non-processor architecture specific way. We call the correct
// function based on whether the port is in memory or port space.  This is
// determined when we claim our resources.
//
// You might be concerned about the overhead incurred by using these functions.
// Note that we've declared them as "forceinline" which should result in code
// generation that's equivalent to a macro.  While this function adds a test
// for every read/write operation, that's the price we pay for not knowing
// until run-time whether our registers are in port or memory space.  Note that
// you can't directly call the appropriate function via a pointer, because
// you don't know if WRITE_REGISTER_UCHAR (for example) is implemented as a
// function or a macro on any given processor architecture (it's a macro on
// IA64, by the way).
//
__forceinline
VOID
WRITE_HARDWARE_UCHAR(PSMSCIR_DATA DevData, UCHAR Register, UCHAR Value)  {

    if(DevData->PortIsMapped) {
        WRITE_REGISTER_UCHAR(DevData->BaseAddress+Register, Value);
    } else  {
        WRITE_PORT_UCHAR(DevData->BaseAddress+Register, Value);
    }
}

__forceinline
UCHAR
READ_HARDWARE_UCHAR(PSMSCIR_DATA DevData, UCHAR Register)  {
    if(DevData->PortIsMapped) {
        return (READ_REGISTER_UCHAR(DevData->BaseAddress+Register));
    } else  {
        return (READ_PORT_UCHAR(DevData->BaseAddress+Register));
    }
}


__forceinline
VOID
WRITE_TRANSMIT_UCHAR(PSMSCIR_DATA DevData, UCHAR Register, UCHAR Value)  {

    if(DevData->TransmitPortIsMapped) {
        WRITE_REGISTER_UCHAR(DevData->TransmitBaseAddress+Register, Value);
    } else  {
        WRITE_PORT_UCHAR(DevData->TransmitBaseAddress+Register, Value);
    }
}

__forceinline
UCHAR
READ_TRANSMIT_UCHAR(PSMSCIR_DATA DevData, UCHAR Register)  {
    if(DevData->TransmitPortIsMapped) {
        return (READ_REGISTER_UCHAR(DevData->TransmitBaseAddress+Register));
    } else  {
        return (READ_PORT_UCHAR(DevData->TransmitBaseAddress+Register));
    }
}

__forceinline
VOID
WRITE_TRANSMIT_BUFFER_UCHAR(PSMSCIR_DATA DevData, 
                            UCHAR Register, 
                            PUCHAR Buffer,
                            ULONG Count){

    if(DevData->TransmitPortIsMapped) {

        ULONG i;
        //
        // Memory mapped I/O...Need to fake it.
        //
        for (i = 0; i < Count; i++) {
            WRITE_REGISTER_UCHAR(DevData->TransmitBaseAddress+Register, 
                                 Buffer[i]);
        }
    } else {
        WRITE_PORT_BUFFER_UCHAR(DevData->TransmitBaseAddress+Register, 
                                Buffer,
                                Count);
    }

}


//
// Handy Dandy Time Macros
//

#define ABSOLUTE(wait) (wait)

#define RELATIVE(wait) (-(wait))

#define NANOSECONDS(nanos) \
(((signed __int64)(nanos)) / 100L)

#define MICROSECONDS(micros) \
(((signed __int64)(micros)) * NANOSECONDS(1000L))

#define MILLISECONDS(milli) \
(((signed __int64)(milli)) * MICROSECONDS(1000L))

#define SECONDS(seconds) \
(((signed __int64)(seconds)) * MILLISECONDS(1000L))


//
// Shared globals
//
extern UNICODE_STRING SmscIrRegistryPath;
extern SAMPLE_RUN SmscIrSampleTable[256][8];

//
// Prototypes
//

//
// smscir.c
//
DRIVER_INITIALIZE DriverEntry;

EVT_WDF_DRIVER_UNLOAD SmscIrEvtDriverUnload;
/*
VOID 
SmscIrEvtDriverUnload(
    WDFDRIVER Driver
    );
*/

//
// io.c
//

EVT_WDF_IO_QUEUE_IO_DEVICE_CONTROL SmscIrEvtIoDeviceControl;
/*
VOID
SmscIrEvtIoDeviceControl(
    IN WDFQUEUE  Queue,
    IN WDFREQUEST  Request,
    IN size_t  OutputBufferLength,
    IN size_t  InputBufferLength,
    IN ULONG  IoControlCode
    );
*/

EVT_WDF_DEVICE_FILE_CREATE SmscIrEvtDeviceFileCreate;
/*
VOID
SmscIrEvtDeviceFileCreate(
    IN WDFDEVICE            Device,
    IN WDFREQUEST           Request,
    IN WDFFILEOBJECT        FileObject
    );
*/

EVT_WDF_FILE_CLOSE SmscIrEvtFileClose;
/*
VOID
SmscIrEvtFileClose(
    IN WDFFILEOBJECT    FileObject
    );
*/

EVT_WDF_IO_QUEUE_IO_DEVICE_CONTROL SmscIrEvtIoDeviceControlSerial;
/*
VOID
SmscIrEvtIoDeviceControlSerial(
    IN WDFQUEUE  Queue,
    IN WDFREQUEST  Request,
    IN size_t  OutputBufferLength,
    IN size_t  InputBufferLength,
    IN ULONG  IoControlCode
    );
*/

VOID
IrHandshake(
    PSMSCIR_DATA DeviceData,
    WDFREQUEST Request
    );

VOID
IrReceive(
    PSMSCIR_DATA DeviceData,
    WDFREQUEST Request, 
    SIZE_T OutputBufferLength
    );

VOID
IrPriorityReceive(
    PSMSCIR_DATA DeviceData,
    WDFREQUEST Request, 
    SIZE_T OutputBufferLength
    );

VOID
IrTransmit(
    PSMSCIR_DATA DeviceData,
    WDFREQUEST Request, 
    SIZE_T OutputBufferLength, 
    SIZE_T InputBufferLength
    );

EVT_WDF_REQUEST_CANCEL IrReceiveInProgressCancel;
/* 
VOID
IrReceiveInProgressCancel(
    IN WDFREQUEST  Request
    );
*/

EVT_WDF_REQUEST_CANCEL IrPriorityReceiveInProgressCancel;
/*
VOID
IrPriorityReceiveInProgressCancel(
    IN WDFREQUEST  Request
    );
*/

VOID
IrUserOpen(
    PSMSCIR_DATA DeviceData,
    WDFREQUEST Request
    );

VOID
IrUserClose(
    PSMSCIR_DATA DeviceData,
    WDFREQUEST Request
    );

VOID
IrEnterPriorityMode(
    PSMSCIR_DATA DeviceData,
    WDFREQUEST Request, 
    SIZE_T InputBufferLength
    );
VOID
IrExitPriorityMode(
    PSMSCIR_DATA DeviceData,
    WDFREQUEST Request
    );

VOID
IrGetDevCaps(
    PSMSCIR_DATA DeviceData,
    WDFREQUEST Request, 
    SIZE_T OutputBufferLength
    );

VOID
IrGetEmitters(
    PSMSCIR_DATA DeviceData,
    WDFREQUEST Request, 
    SIZE_T OutputBufferLength
    );

VOID
IrFlashReceiver(
    PSMSCIR_DATA DeviceData,
    WDFREQUEST Request, 
    SIZE_T InputBufferLength
    );

VOID
IrResetDevice(
    PSMSCIR_DATA DeviceData,
    WDFREQUEST Request
    );

//
// pnp.c
//
EVT_WDF_DRIVER_DEVICE_ADD SmscIrEvtDeviceAdd;
/*
NTSTATUS
SmscIrEvtDeviceAdd(
    IN WDFDRIVER Driver,
    IN PWDFDEVICE_INIT DeviceInit
    );
*/

EVT_WDF_DEVICE_PREPARE_HARDWARE SmscIrEvtDevicePrepareHardware;
/*
NTSTATUS
SmscIrEvtDevicePrepareHardware(
    IN WDFDEVICE    Device,
    IN WDFCMRESLIST ResourceList,
    IN WDFCMRESLIST ResourceListTranslated
    );
*/

EVT_WDF_DEVICE_RELEASE_HARDWARE SmscIrEvtDeviceReleaseHardware;
/*
NTSTATUS
SmscIrEvtDeviceReleaseHardware(
    IN WDFDEVICE    Device,
    IN WDFCMRESLIST ResourceListTranslated
    );
*/

EVT_WDF_DEVICE_D0_ENTRY SmscIrEvtDeviceD0Entry;
/*
NTSTATUS
SmscIrEvtDeviceD0Entry(
    IN  WDFDEVICE Device,
    IN  WDF_POWER_DEVICE_STATE PreviousState
    );
*/

EVT_WDF_DEVICE_D0_EXIT SmscIrEvtDeviceD0Exit;
/*
NTSTATUS
SmscIrEvtDeviceD0Exit(
    IN  WDFDEVICE Device,
    IN  WDF_POWER_DEVICE_STATE TargetState
    );
*/

//
// isr.c
//
EVT_WDF_INTERRUPT_ENABLE SmscIrEvtInterruptEnable;
/*
NTSTATUS
SmscIrEvtInterruptEnable(
    IN WDFINTERRUPT Interrupt,
    IN WDFDEVICE    Device
    );
*/

EVT_WDF_INTERRUPT_ISR SmscIrEvtInterruptIsr;
/*
BOOLEAN
SmscIrEvtInterruptIsr(
    IN WDFINTERRUPT Interrupt,
    IN ULONG        MessageID
    );
*/

ULONG 
SmscIrFifoBytesRemaining(
    IN PSMSCIR_DATA DeviceData
    );

ULONG 
SmscIrDequeueFifoData(
    IN PSMSCIR_DATA DeviceData,
    __out_bcount(MaxLen) PUCHAR Buffer,
    ULONG MaxLen,
    __out PBOOLEAN DataEnd
    );


EVT_WDF_INTERRUPT_DISABLE SmscIrEvtInterruptDisable;
/*
NTSTATUS
SmscIrEvtInterruptDisable(
    IN WDFINTERRUPT Interrupt,
    IN WDFDEVICE    Device
    );
*/


//
// Dpc.c
//
EVT_WDF_INTERRUPT_DPC SmscIrEvtInterruptDpc;
/*
VOID
SmscIrEvtInterruptDpc(
    IN WDFINTERRUPT Interrupt,
    IN WDFOBJECT    Object
    );
*/

VOID 
SmscIrEnqueueRLCData(
    PSMSCIR_RLC_RECEIVER RLCReceiver,
    __in_bcount(Length) PUCHAR RLCData,
    ULONG Length,
    IN BOOLEAN DataEnd
    );

ULONG 
SmscIrDequeueRLCData(
    PSMSCIR_RLC_RECEIVER RLCReceiver,
    __out_bcount(MaxLen) PUCHAR Buffer,
    ULONG MaxLen,
    __out PBOOLEAN DataEnd
    );

VOID
SmscIrProcessPendingReceives(
    IN PSMSCIR_RLC_RECEIVER RLCReceiver,
    OUT WDFREQUEST *CompleteWithSuccess,
    OUT WDFREQUEST *CompleteWithFailure,
    __out PNTSTATUS FailureStatus
    );

NTSTATUS
IrSetupNextRequest(
    IN PSMSCIR_RLC_RECEIVER RLCReceiver,
    OUT WDFREQUEST *CompleteWithFailure,
    OUT PNTSTATUS FailureStatus
    );

VOID
SmscIrContinueBlasting(
    IN PSMSCIR_DATA DeviceData,
    OUT WDFREQUEST *TxRequestToComplete,
    OUT PNTSTATUS CompletionStatus
    );

EVT_WDF_TIMER SmscIrEvtDeadManTimerForTransmit;
/*
VOID
SmscIrEvtDeadManTimerForTransmit(
    IN WDFTIMER Timer
    );
*/

//
// Wake.c
//
VOID
SmscIrSetWakeKeyInfo(
    IN PSMSCIR_DATA DeviceData
    );

EVT_WDF_DEVICE_ARM_WAKE_FROM_SX SmscIrEvtDeviceWakeArm;
/*
NTSTATUS
SmscIrEvtDeviceWakeArm(
    IN WDFDEVICE Device
    );
*/

BOOLEAN
SmscIrDeviceWokeSystem(
    IN PSMSCIR_DATA DeviceData
    );

//re-enable warnings
#if _MSC_VER >= 1200
#pragma warning(pop)
#else
#pragma warning(default:4201)
#pragma warning(default:4214)
#endif



#endif // ndef SmscIrH

