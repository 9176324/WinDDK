/*++ BUILD Version: 0001    // Increment this if a change has global effects

Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name:

    ntpoapi.h

Abstract:

    This module contains the user APIs for the NT Power Management.

Author:

Revision History:

--*/

#if (NTDDI_VERSION >= NTDDI_VISTA)
// begin_ntminiport begin_wdm begin_winnt

//
// =========================================
// Define GUIDs which represent well-known power schemes
// =========================================
//

//
// Maximum Power Savings - indicates that very aggressive power savings measures will be used to help
//                         stretch battery life.
//
// {a1841308-3541-4fab-bc81-f71556f20b4a}
//
DEFINE_GUID( GUID_MAX_POWER_SAVINGS, 0xA1841308, 0x3541, 0x4FAB, 0xBC, 0x81, 0xF7, 0x15, 0x56, 0xF2, 0x0B, 0x4A );

//
// No Power Savings - indicates that almost no power savings measures will be used.
//
// {8c5e7fda-e8bf-4a96-9a85-a6e23a8c635c}
//
DEFINE_GUID( GUID_MIN_POWER_SAVINGS, 0x8C5E7FDA, 0xE8BF, 0x4A96, 0x9A, 0x85, 0xA6, 0xE2, 0x3A, 0x8C, 0x63, 0x5C );

//
// Typical Power Savings - indicates that fairly aggressive power savings measures will be used.
//
// {381b4222-f694-41f0-9685-ff5bb260df2e}
//
DEFINE_GUID( GUID_TYPICAL_POWER_SAVINGS, 0x381B4222, 0xF694, 0x41F0, 0x96, 0x85, 0xFF, 0x5B, 0xB2, 0x60, 0xDF, 0x2E );

//
// This is a special GUID that represents "no subgroup" of settings.  That is, it indicates
// that settings that are in the root of the power policy hierarchy as opposed to settings
// that are buried under a subgroup of settings.  This should be used when querying for
// power settings that may not fall into a subgroup.
//
DEFINE_GUID( NO_SUBGROUP_GUID, 0xFEA3413E, 0x7E05, 0x4911, 0x9A, 0x71, 0x70, 0x03, 0x31, 0xF1, 0xC2, 0x94 );

//
// This is a special GUID that represents "every power scheme".  That is, it indicates
// that any write to this power scheme should be reflected to every scheme present.
// This allows users to write a single setting once and have it apply to all schemes.  They
// can then apply custom settings to specific power schemes that they care about.
//
DEFINE_GUID( ALL_POWERSCHEMES_GUID, 0x68A1E95E, 0x13EA, 0x41E1, 0x80, 0x11, 0x0C, 0x49, 0x6C, 0xA4, 0x90, 0xB0 );

//
// This is a special GUID that represents a 'personality' that each power scheme will have.
// In other words, each power scheme will have this key indicating "I'm most like *this* base
// power scheme."  This individual setting will have one of three settings:
// GUID_MAX_POWER_SAVINGS
// GUID_MIN_POWER_SAVINGS
// GUID_TYPICAL_POWER_SAVINGS
//
// This allows several features:
// 1. Drivers and applications can register for notification of this GUID.  So when this power
//    scheme is activiated, this GUID's setting will be sent across the system and drivers/applications
//    can see "GUID_MAX_POWER_SAVINGS" which will tell them in a generic fashion "get real aggressive
//    about conserving power".
// 2. UserB may install a driver or application which creates power settings, and UserB may modify
//    those power settings.  Now UserA logs in.  How does he see those settings?  They simply don't
//    exist in his private power key.  Well they do exist over in the system power key.  When we
//    enumerate all the power settings in this system power key and don't find a corresponding entry
//    in the user's private power key, then we can go look at this "personality" key in the users
//    power scheme.  We can then go get a default value for the power setting, depending on which
//    "personality" power scheme is being operated on.  Here's an example:
//    A. UserB installs an application that creates a power setting Seetting1
//    B. UserB changes Setting1 to have a value of 50 because that's one of the possible settings
//       available for setting1.
//    C. UserB logs out
//    D. UserA logs in and his active power scheme is some custom scheme that was derived from
//       the GUID_TYPICAL_POWER_SAVINGS.  But remember that UserA has no setting1 in his
//       private power key.
//    E. When activating UserA's selected power scheme, all power settings in the system power key will
//       be enumerated (including Setting1).
//    F. The power manager will see that UserA has no Setting1 power setting in his private power scheme.
//    G. The power manager will query UserA's power scheme for its personality and retrieve
//       GUID_TYPICAL_POWER_SAVINGS.
//    H. The power manager then looks in Setting1 in the system power key and looks in its set of default
//       values for the corresponding value for GUID_TYPICAL_POWER_SAVINGS power schemes.
//    I. This derived power setting is applied.
DEFINE_GUID( GUID_POWERSCHEME_PERSONALITY, 0x245D8541, 0x3943, 0x4422, 0xB0, 0x25, 0x13, 0xA7, 0x84, 0xF6, 0x79, 0xB7 );

//
// Define a special GUID which will be used to define the active power scheme.
// User will register for this power setting GUID, and when the active power
// scheme changes, they'll get a callback where the payload is the GUID
// representing the active powerscheme.
// ( 31F9F286-5084-42FE-B720-2B0264993763 }
//
DEFINE_GUID( GUID_ACTIVE_POWERSCHEME, 0x31F9F286, 0x5084, 0x42FE, 0xB7, 0x20, 0x2B, 0x02, 0x64, 0x99, 0x37, 0x63 );

//
// =========================================
// Define GUIDs which represent well-known power settings
// =========================================
//

// Video settings
// --------------
//
// Specifies the subgroup which will contain all of the video
// settings for a single policy.
//
DEFINE_GUID( GUID_VIDEO_SUBGROUP, 0x7516B95F, 0xF776, 0x4464, 0x8C, 0x53, 0x06, 0x16, 0x7F, 0x40, 0xCC, 0x99 );

//
// Specifies (in seconds) how long we wait after the last user input has been
// recieved before we power off the video.
//
DEFINE_GUID( GUID_VIDEO_POWERDOWN_TIMEOUT, 0x3C0BC021, 0xC8A8, 0x4E07, 0xA9, 0x73, 0x6B, 0x14, 0xCB, 0xCB, 0x2B, 0x7E );

//
// Specifies whether adaptive display dimming is turned on or off.
// 82DBCF2D-CD67-40C5-BFDC-9F1A5CCD4663
//
DEFINE_GUID( GUID_VIDEO_ANNOYANCE_TIMEOUT, 0x82DBCF2D, 0xCD67, 0x40C5, 0xBF, 0xDC, 0x9F, 0x1A, 0x5C, 0xCD, 0x46, 0x63 );

//
// Specifies how much adaptive dim time out will be increased by.
// EED904DF-B142-4183-B10B-5A1197A37864
//
DEFINE_GUID( GUID_VIDEO_ADAPTIVE_PERCENT_INCREASE, 0xEED904DF, 0xB142, 0x4183, 0xB1, 0x0B, 0x5A, 0x11, 0x97, 0xA3, 0x78, 0x64 );

//
// Specifies (in seconds) how long we wait after the last user input has been
// recieved before we dim the video.
//
DEFINE_GUID( GUID_VIDEO_DIM_TIMEOUT, 0x17aaa29b, 0x8b43, 0x4b94, 0xaa, 0xfe, 0x35, 0xf6, 0x4d, 0xaa, 0xf1, 0xee);

//
// Specifies if the operating system should use adaptive timers (based on
// previous behavior) to power down the video,
//
DEFINE_GUID( GUID_VIDEO_ADAPTIVE_POWERDOWN, 0x90959D22, 0xD6A1, 0x49B9, 0xAF, 0x93, 0xBC, 0xE8, 0x85, 0xAD, 0x33, 0x5B );

//
// Specifies if the monitor is currently being powered or not.
// 02731015-4510-4526-99E6-E5A17EBD1AEA
//
DEFINE_GUID( GUID_MONITOR_POWER_ON, 0x02731015, 0x4510, 0x4526, 0x99, 0xE6, 0xE5, 0xA1, 0x7E, 0xBD, 0x1A, 0xEA );

//
// Monitor brightness policy when in normal state
// {aded5e82-b909-4619-9949-f5d71dac0bcb}
DEFINE_GUID(GUID_DEVICE_POWER_POLICY_VIDEO_BRIGHTNESS, 0xaded5e82L, 0xb909, 0x4619, 0x99, 0x49, 0xf5, 0xd7, 0x1d, 0xac, 0x0b, 0xcb);

//
//
// Monitor brightness policy when in dim state
// {f1fbfde2-a960-4165-9f88-50667911ce96}
DEFINE_GUID(GUID_DEVICE_POWER_POLICY_VIDEO_DIM_BRIGHTNESS, 0xf1fbfde2, 0xa960, 0x4165, 0x9f, 0x88, 0x50, 0x66, 0x79, 0x11, 0xce, 0x96);

//
// Current Monitor brightness
// {8ffee2c6-2d01-46be-adb9-398addc5b4ff}
DEFINE_GUID(GUID_VIDEO_CURRENT_MONITOR_BRIGHTNESS, 0x8ffee2c6, 0x2d01, 0x46be, 0xad, 0xb9, 0x39, 0x8a, 0xdd, 0xc5, 0xb4, 0xff);


//
// Specifies if the operating system should use ambient light sensor to change
// disply brightness adatively.
// {FBD9AA66-9553-4097-BA44-ED6E9D65EAB8}
DEFINE_GUID(GUID_VIDEO_ADAPTIVE_DISPLAY_BRIGHTNESS, 0xFBD9AA66, 0x9553, 0x4097, 0xBA, 0x44, 0xED, 0x6E, 0x9D, 0x65, 0xEA, 0xB8);

//
// Specifies a change in the session's display state.
// 73A5E93A-5BB1-4F93-895B-DBD0DA855967
//
// N.B. This is a session-specific notification, sent only to interactive
//      session registrants. Session 0 and kernel mode consumers do not receive
//      this notification.
DEFINE_GUID( GUID_SESSION_DISPLAY_STATE, 0x73A5E93A, 0x5BB1, 0x4F93, 0x89, 0x5B, 0xDB, 0xD0, 0xDA, 0x85, 0x59, 0x67 );

//
// Specifies a change in the current monitor's display state.
// 6fe69556-704a-47a0-8f24-c28d936fda47
//
DEFINE_GUID(GUID_CONSOLE_DISPLAY_STATE, 0x6fe69556, 0x704a, 0x47a0, 0x8f, 0x24, 0xc2, 0x8d, 0x93, 0x6f, 0xda, 0x47);

//
// Defines a guid for enabling/disabling the ability to create display required 
// power requests.
//
// {A9CEB8DA-CD46-44FB-A98B-02AF69DE4623}
//
DEFINE_GUID( GUID_ALLOW_DISPLAY_REQUIRED, 0xA9CEB8DA, 0xCD46, 0x44FB, 0xA9, 0x8B, 0x02, 0xAF, 0x69, 0xDE, 0x46, 0x23 );

// Harddisk settings
// -----------------
//
// Specifies the subgroup which will contain all of the harddisk
// settings for a single policy.
//
DEFINE_GUID( GUID_DISK_SUBGROUP, 0x0012EE47, 0x9041, 0x4B5D, 0x9B, 0x77, 0x53, 0x5F, 0xBA, 0x8B, 0x14, 0x42 );

//
// Specifies (in seconds) how long we wait after the last disk access
// before we power off the disk.
//
DEFINE_GUID( GUID_DISK_POWERDOWN_TIMEOUT, 0x6738E2C4, 0xE8A5, 0x4A42, 0xB1, 0x6A, 0xE0, 0x40, 0xE7, 0x69, 0x75, 0x6E );

//
// Specifies the amount of contiguous disk activity time to ignore when
// calculating disk idleness.
//
// 80e3c60e-bb94-4ad8-bbe0-0d3195efc663
//

DEFINE_GUID( GUID_DISK_BURST_IGNORE_THRESHOLD, 0x80e3c60e, 0xbb94, 0x4ad8, 0xbb, 0xe0, 0x0d, 0x31, 0x95, 0xef, 0xc6, 0x63 );

//
// Specifies if the operating system should use adaptive timers (based on
// previous behavior) to power down the disk,
//
DEFINE_GUID( GUID_DISK_ADAPTIVE_POWERDOWN, 0x396A32E1, 0x499A, 0x40B2, 0x91, 0x24, 0xA9, 0x6A, 0xFE, 0x70, 0x76, 0x67 );

// System sleep settings
// ---------------------
//
// Specifies the subgroup which will contain all of the sleep
// settings for a single policy.
// { 238C9FA8-0AAD-41ED-83F4-97BE242C8F20 }
//
DEFINE_GUID( GUID_SLEEP_SUBGROUP, 0x238C9FA8, 0x0AAD, 0x41ED, 0x83, 0xF4, 0x97, 0xBE, 0x24, 0x2C, 0x8F, 0x20 );

//
// Specifies an idle treshold percentage (0-100). The system must be this idle
// over a period of time in order to idle to sleep.
//
// N.B. DEPRECATED IN WINDOWS 6.1
//
DEFINE_GUID( GUID_SLEEP_IDLE_THRESHOLD, 0x81cd32e0, 0x7833, 0x44f3, 0x87, 0x37, 0x70, 0x81, 0xf3, 0x8d, 0x1f, 0x70 );

//
// Specifies (in seconds) how long we wait after the system is deemed
// "idle" before moving to standby (S1, S2 or S3).
//
DEFINE_GUID( GUID_STANDBY_TIMEOUT, 0x29F6C1DB, 0x86DA, 0x48C5, 0x9F, 0xDB, 0xF2, 0xB6, 0x7B, 0x1F, 0x44, 0xDA );

//
// Specifies (in seconds) how long the system should go back to sleep after
// waking unattended. 0 indicates that the standard standby/hibernate idle
// policy should be used instead.
//
// {7bc4a2f9-d8fc-4469-b07b-33eb785aaca0}
//
DEFINE_GUID( GUID_UNATTEND_SLEEP_TIMEOUT, 0x7bc4a2f9, 0xd8fc, 0x4469, 0xb0, 0x7b, 0x33, 0xeb, 0x78, 0x5a, 0xac, 0xa0 );

//
// Specifies (in seconds) how long we wait after the system is deemed
// "idle" before moving to hibernate (S4).
//
DEFINE_GUID( GUID_HIBERNATE_TIMEOUT, 0x9D7815A6, 0x7EE4, 0x497E, 0x88, 0x88, 0x51, 0x5A, 0x05, 0xF0, 0x23, 0x64 );

//
// Specifies whether or not Fast S4 should be enabled if the system supports it
// 94AC6D29-73CE-41A6-809F-6363BA21B47E
//
DEFINE_GUID( GUID_HIBERNATE_FASTS4_POLICY, 0x94AC6D29, 0x73CE, 0x41A6, 0x80, 0x9F, 0x63, 0x63, 0xBA, 0x21, 0xB4, 0x7E );

//
// Define a GUID for controlling the criticality of sleep state transitions.
// Critical sleep transitions do not query applications, services or drivers
// before transitioning the platform to a sleep state.
//
// {B7A27025-E569-46c2-A504-2B96CAD225A1}
//
DEFINE_GUID( GUID_CRITICAL_POWER_TRANSITION,  0xB7A27025, 0xE569, 0x46c2, 0xA5, 0x04, 0x2B, 0x96, 0xCA, 0xD2, 0x25, 0xA1);

//
// Specifies if the system is entering or exiting 'away mode'.
// 98A7F580-01F7-48AA-9C0F-44352C29E5C0
//
DEFINE_GUID( GUID_SYSTEM_AWAYMODE, 0x98A7F580, 0x01F7, 0x48AA, 0x9C, 0x0F, 0x44, 0x35, 0x2C, 0x29, 0xE5, 0xC0 );

// Specify whether away mode is allowed
//
// {25DFA149-5DD1-4736-B5AB-E8A37B5B8187}
//
DEFINE_GUID( GUID_ALLOW_AWAYMODE, 0x25dfa149, 0x5dd1, 0x4736, 0xb5, 0xab, 0xe8, 0xa3, 0x7b, 0x5b, 0x81, 0x87 );

//
// Defines a guid for enabling/disabling standby (S1-S3) states. This does not
// affect hibernation (S4).
//
// {abfc2519-3608-4c2a-94ea-171b0ed546ab}
//
DEFINE_GUID( GUID_ALLOW_STANDBY_STATES, 0xabfc2519, 0x3608, 0x4c2a, 0x94, 0xea, 0x17, 0x1b, 0x0e, 0xd5, 0x46, 0xab );

//
// Defines a guid for enabling/disabling the ability to wake via RTC.
//
// {BD3B718A-0680-4D9D-8AB2-E1D2B4AC806D}
//
DEFINE_GUID( GUID_ALLOW_RTC_WAKE, 0xBD3B718A, 0x0680, 0x4D9D, 0x8A, 0xB2, 0xE1, 0xD2, 0xB4, 0xAC, 0x80, 0x6D );

//
// Defines a guid for enabling/disabling the ability to create system required 
// power requests.
//
// {A4B195F5-8225-47D8-8012-9D41369786E2}
//
DEFINE_GUID( GUID_ALLOW_SYSTEM_REQUIRED, 0xA4B195F5, 0x8225, 0x47D8, 0x80, 0x12, 0x9D, 0x41, 0x36, 0x97, 0x86, 0xE2 );
  
// System button actions
// ---------------------
//
//
// Specifies the subgroup which will contain all of the system button
// settings for a single policy.
//
DEFINE_GUID( GUID_SYSTEM_BUTTON_SUBGROUP, 0x4F971E89, 0xEEBD, 0x4455, 0xA8, 0xDE, 0x9E, 0x59, 0x04, 0x0E, 0x73, 0x47 );

// Specifies (in a POWER_ACTION_POLICY structure) the appropriate action to
// take when the system power button is pressed.
//
DEFINE_GUID( GUID_POWERBUTTON_ACTION, 0x7648EFA3, 0xDD9C, 0x4E3E, 0xB5, 0x66, 0x50, 0xF9, 0x29, 0x38, 0x62, 0x80 );
DEFINE_GUID( GUID_POWERBUTTON_ACTION_FLAGS, 0x857E7FAC, 0x034B, 0x4704, 0xAB, 0xB1, 0xBC, 0xA5, 0x4A, 0xA3, 0x14, 0x78 );

//
// Specifies (in a POWER_ACTION_POLICY structure) the appropriate action to
// take when the system sleep button is pressed.
//
DEFINE_GUID( GUID_SLEEPBUTTON_ACTION, 0x96996BC0, 0xAD50, 0x47EC, 0x92, 0x3B, 0x6F, 0x41, 0x87, 0x4D, 0xD9, 0xEB );
DEFINE_GUID( GUID_SLEEPBUTTON_ACTION_FLAGS, 0x2A160AB1, 0xB69D, 0x4743, 0xB7, 0x18, 0xBF, 0x14, 0x41, 0xD5, 0xE4, 0x93 );

//
// Specifies (in a POWER_ACTION_POLICY structure) the appropriate action to
// take when the system sleep button is pressed.
// { A7066653-8D6C-40A8-910E-A1F54B84C7E5 }
//
DEFINE_GUID( GUID_USERINTERFACEBUTTON_ACTION, 0xA7066653, 0x8D6C, 0x40A8, 0x91, 0x0E, 0xA1, 0xF5, 0x4B, 0x84, 0xC7, 0xE5 );

//
// Specifies (in a POWER_ACTION_POLICY structure) the appropriate action to
// take when the system lid is closed.
//
DEFINE_GUID( GUID_LIDCLOSE_ACTION, 0x5CA83367, 0x6E45, 0x459F, 0xA2, 0x7B, 0x47, 0x6B, 0x1D, 0x01, 0xC9, 0x36 );
DEFINE_GUID( GUID_LIDCLOSE_ACTION_FLAGS, 0x97E969AC, 0x0D6C, 0x4D08, 0x92, 0x7C, 0xD7, 0xBD, 0x7A, 0xD7, 0x85, 0x7B );
DEFINE_GUID( GUID_LIDOPEN_POWERSTATE, 0x99FF10E7, 0x23B1, 0x4C07, 0xA9, 0xD1, 0x5C, 0x32, 0x06, 0xD7, 0x41, 0xB4 );


// Battery Discharge Settings
// --------------------------
//
// Specifies the subgroup which will contain all of the battery discharge
// settings for a single policy.
//
DEFINE_GUID( GUID_BATTERY_SUBGROUP, 0xE73A048D, 0xBF27, 0x4F12, 0x97, 0x31, 0x8B, 0x20, 0x76, 0xE8, 0x89, 0x1F );

//
// 4 battery discharge alarm settings.
//
// GUID_BATTERY_DISCHARGE_ACTION_x - This is the action to take.  It is a value
//                                   of type POWER_ACTION
// GUID_BATTERY_DISCHARGE_LEVEL_x  - This is the battery level (%)
// GUID_BATTERY_DISCHARGE_FLAGS_x  - Flags defined below:
//                                   POWER_ACTION_POLICY->EventCode flags
//                                   BATTERY_DISCHARGE_FLAGS_EVENTCODE_MASK
//                                   BATTERY_DISCHARGE_FLAGS_ENABLE
DEFINE_GUID( GUID_BATTERY_DISCHARGE_ACTION_0, 0x637EA02F, 0xBBCB, 0x4015, 0x8E, 0x2C, 0xA1, 0xC7, 0xB9, 0xC0, 0xB5, 0x46 );
DEFINE_GUID( GUID_BATTERY_DISCHARGE_LEVEL_0, 0x9A66D8D7, 0x4FF7, 0x4EF9, 0xB5, 0xA2, 0x5A, 0x32, 0x6C, 0xA2, 0xA4, 0x69 );
DEFINE_GUID( GUID_BATTERY_DISCHARGE_FLAGS_0, 0x5dbb7c9f, 0x38e9, 0x40d2, 0x97, 0x49, 0x4f, 0x8a, 0x0e, 0x9f, 0x64, 0x0f );

DEFINE_GUID( GUID_BATTERY_DISCHARGE_ACTION_1, 0xD8742DCB, 0x3E6A, 0x4B3C, 0xB3, 0xFE, 0x37, 0x46, 0x23, 0xCD, 0xCF, 0x06 );
DEFINE_GUID( GUID_BATTERY_DISCHARGE_LEVEL_1, 0x8183BA9A, 0xE910, 0x48DA, 0x87, 0x69, 0x14, 0xAE, 0x6D, 0xC1, 0x17, 0x0A );
DEFINE_GUID( GUID_BATTERY_DISCHARGE_FLAGS_1, 0xbcded951, 0x187b, 0x4d05, 0xbc, 0xcc, 0xf7, 0xe5, 0x19, 0x60, 0xc2, 0x58 );

DEFINE_GUID( GUID_BATTERY_DISCHARGE_ACTION_2, 0x421CBA38, 0x1A8E, 0x4881, 0xAC, 0x89, 0xE3, 0x3A, 0x8B, 0x04, 0xEC, 0xE4 );
DEFINE_GUID( GUID_BATTERY_DISCHARGE_LEVEL_2, 0x07A07CA2, 0xADAF, 0x40D7, 0xB0, 0x77, 0x53, 0x3A, 0xAD, 0xED, 0x1B, 0xFA );
DEFINE_GUID( GUID_BATTERY_DISCHARGE_FLAGS_2, 0x7fd2f0c4, 0xfeb7, 0x4da3, 0x81, 0x17, 0xe3, 0xfb, 0xed, 0xc4, 0x65, 0x82 );

DEFINE_GUID( GUID_BATTERY_DISCHARGE_ACTION_3, 0x80472613, 0x9780, 0x455E, 0xB3, 0x08, 0x72, 0xD3, 0x00, 0x3C, 0xF2, 0xF8 );
DEFINE_GUID( GUID_BATTERY_DISCHARGE_LEVEL_3, 0x58AFD5A6, 0xC2DD, 0x47D2, 0x9F, 0xBF, 0xEF, 0x70, 0xCC, 0x5C, 0x59, 0x65 );
DEFINE_GUID( GUID_BATTERY_DISCHARGE_FLAGS_3, 0x73613ccf, 0xdbfa, 0x4279, 0x83, 0x56, 0x49, 0x35, 0xf6, 0xbf, 0x62, 0xf3 );

// Processor power settings
// ------------------------
//

// Specifies the subgroup which will contain all of the processor
// settings for a single policy.
//
DEFINE_GUID( GUID_PROCESSOR_SETTINGS_SUBGROUP, 0x54533251, 0x82BE, 0x4824, 0x96, 0xC1, 0x47, 0xB6, 0x0B, 0x74, 0x0D, 0x00 );

//
// Specifies various attributes that control processor performance/throttle
// states.
// 
DEFINE_GUID( GUID_PROCESSOR_THROTTLE_POLICY, 0x57027304, 0x4AF6, 0x4104, 0x92, 0x60, 0xE3, 0xD9, 0x52, 0x48, 0xFC, 0x36 );

#define PERFSTATE_POLICY_CHANGE_IDEAL  0
#define PERFSTATE_POLICY_CHANGE_SINGLE 1
#define PERFSTATE_POLICY_CHANGE_ROCKET 2
#define PERFSTATE_POLICY_CHANGE_MAX PERFSTATE_POLICY_CHANGE_ROCKET

//
// Specifies a percentage (between 0 and 100) that the processor frequency
// should never go above.  For example, if this value is set to 80, then
// the processor frequency will never be throttled above 80 percent of its
// maximum frequency by the system.
//
DEFINE_GUID( GUID_PROCESSOR_THROTTLE_MAXIMUM, 0xBC5038F7, 0x23E0, 0x4960, 0x96, 0xDA, 0x33, 0xAB, 0xAF, 0x59, 0x35, 0xEC );

//
// Specifies a percentage (between 0 and 100) that the processor frequency
// should not drop below.  For example, if this value is set to 50, then the
// processor frequency will never be throttled below 50 percent of its
// maximum frequency by the system.
//
DEFINE_GUID( GUID_PROCESSOR_THROTTLE_MINIMUM, 0x893DEE8E, 0x2BEF, 0x41E0, 0x89, 0xC6, 0xB5, 0x5D, 0x09, 0x29, 0x96, 0x4C );

//
// Specifies whether throttle states are allowed to be used even when
// performance states are available.
//
// {3b04d4fd-1cc7-4f23-ab1c-d1337819c4bb}
//
DEFINE_GUID( GUID_PROCESSOR_ALLOW_THROTTLING, 0x3b04d4fd, 0x1cc7, 0x4f23, 0xab, 0x1c, 0xd1, 0x33, 0x78, 0x19, 0xc4, 0xbb );

//
// Specifies processor power settings for CState policy data
// {68F262A7-F621-4069-B9A5-4874169BE23C}
//
DEFINE_GUID( GUID_PROCESSOR_IDLESTATE_POLICY, 0x68f262a7, 0xf621, 0x4069, 0xb9, 0xa5, 0x48, 0x74, 0x16, 0x9b, 0xe2, 0x3c);

//
// Specifies processor power settings for PerfState policy data
// {BBDC3814-18E9-4463-8A55-D197327C45C0}
//
DEFINE_GUID( GUID_PROCESSOR_PERFSTATE_POLICY, 0xBBDC3814, 0x18E9, 0x4463, 0x8A, 0x55, 0xD1, 0x97, 0x32, 0x7C, 0x45, 0xC0);

//
// Specifies the increase busy percentage threshold that must be met before
// increasing the processor performance state.
//
// {06cadf0e-64ed-448a-8927-ce7bf90eb35d}
//
DEFINE_GUID( GUID_PROCESSOR_PERF_INCREASE_THRESHOLD, 0x06cadf0e, 0x64ed, 0x448a, 0x89, 0x27, 0xce, 0x7b, 0xf9, 0x0e, 0xb3, 0x5d );

//
// Specifies the decrease busy percentage threshold that must be met before
// decreasing the processor performance state.
//
// {12a0ab44-fe28-4fa9-b3bd-4b64f44960a6}
//
DEFINE_GUID( GUID_PROCESSOR_PERF_DECREASE_THRESHOLD, 0x12a0ab44, 0xfe28, 0x4fa9, 0xb3, 0xbd, 0x4b, 0x64, 0xf4, 0x49, 0x60, 0xa6 );

//
// Specifies, either as ideal, single or rocket, how aggressive performance
// states should be selected when increasing the processor performance state.
//
// {465E1F50-B610-473a-AB58-00D1077DC418}
//
DEFINE_GUID( GUID_PROCESSOR_PERF_INCREASE_POLICY, 0x465e1f50, 0xb610, 0x473a, 0xab, 0x58, 0x0, 0xd1, 0x7, 0x7d, 0xc4, 0x18);

//
// Specifies, either as ideal, single or rocket, how aggressive performance
// states should be selected when decreasing the processor performance state.
//
// {40FBEFC7-2E9D-4d25-A185-0CFD8574BAC6}
//
DEFINE_GUID( GUID_PROCESSOR_PERF_DECREASE_POLICY, 0x40fbefc7, 0x2e9d, 0x4d25, 0xa1, 0x85, 0xc, 0xfd, 0x85, 0x74, 0xba, 0xc6);

//
// Specifies, in milliseconds, the minimum amount of time that must elapse after
// the last processor performance state change before increasing the processor
// performance state.
//
// {984CF492-3BED-4488-A8F9-4286C97BF5AA}
//
DEFINE_GUID( GUID_PROCESSOR_PERF_INCREASE_TIME, 0x984cf492, 0x3bed, 0x4488, 0xa8, 0xf9, 0x42, 0x86, 0xc9, 0x7b, 0xf5, 0xaa);

//
// Specifies, in milliseconds, the minimum amount of time that must elapse after
// the last processor performance state change before increasing the processor
// performance state.
//
// {D8EDEB9B-95CF-4f95-A73C-B061973693C8}
//
DEFINE_GUID( GUID_PROCESSOR_PERF_DECREASE_TIME, 0xd8edeb9b, 0x95cf, 0x4f95, 0xa7, 0x3c, 0xb0, 0x61, 0x97, 0x36, 0x93, 0xc8);

//
// Specifies the time, in milliseconds, that must expire before considering
// a change in the processor performance states or parked core set.
//
// {4D2B0152-7D5C-498b-88E2-34345392A2C5}
//
DEFINE_GUID( GUID_PROCESSOR_PERF_TIME_CHECK, 0x4d2b0152, 0x7d5c, 0x498b, 0x88, 0xe2, 0x34, 0x34, 0x53, 0x92, 0xa2, 0xc5);

//
// Specifies whether a processor may opportunistically increase frequency above
// the maximum when operating contitions allow it to do so safely.
//
// {45BCC044-D885-43e2-8605-EE0EC6E96B59}
//
DEFINE_GUID(GUID_PROCESSOR_PERF_BOOST_POLICY, 
0x45bcc044, 0xd885, 0x43e2, 0x86, 0x5, 0xee, 0xe, 0xc6, 0xe9, 0x6b, 0x59);

#define PROCESSOR_PERF_BOOST_POLICY_DISABLED 0
#define PROCESSOR_PERF_BOOST_POLICY_MAX 100

//
// Specifies if idle state promotion and demotion values should be scaled based
// on the current peformance state.
//
// {6C2993B0-8F48-481f-BCC6-00DD2742AA06}
//
DEFINE_GUID( GUID_PROCESSOR_IDLE_ALLOW_SCALING, 0x6c2993b0, 0x8f48, 0x481f, 0xbc, 0xc6, 0x0, 0xdd, 0x27, 0x42, 0xaa, 0x6);

//
// Specifies if idle states should be disabled.
//
// {5D76A2CA-E8C0-402f-A133-2158492D58AD}
//
DEFINE_GUID( GUID_PROCESSOR_IDLE_DISABLE, 0x5d76a2ca, 0xe8c0, 0x402f, 0xa1, 0x33, 0x21, 0x58, 0x49, 0x2d, 0x58, 0xad);

//
// Specifies the time that elapsed since the last idle state promotion or
// demotion before idle states may be promoted or demoted again (in 
// microseconds).
//
// {C4581C31-89AB-4597-8E2B-9C9CAB440E6B}
//
DEFINE_GUID( GUID_PROCESSOR_IDLE_TIME_CHECK, 0xc4581c31, 0x89ab, 0x4597, 0x8e, 0x2b, 0x9c, 0x9c, 0xab, 0x44, 0xe, 0x6b);


//
// Specifies the upper busy threshold that must be met before demoting the
// processor to a lighter idle state (in percentage).
//
// {4B92D758-5A24-4851-A470-815D78AEE119}
//
DEFINE_GUID( GUID_PROCESSOR_IDLE_DEMOTE_THRESHOLD, 0x4b92d758, 0x5a24, 0x4851, 0xa4, 0x70, 0x81, 0x5d, 0x78, 0xae, 0xe1, 0x19);

//
// Specifies the lower busy threshold that must be met before promoting the 
// processor to a deeper idle state (in percentage).
//
// {7B224883-B3CC-4d79-819F-8374152CBE7C}
//
DEFINE_GUID( GUID_PROCESSOR_IDLE_PROMOTE_THRESHOLD, 0x7b224883, 0xb3cc, 0x4d79, 0x81, 0x9f, 0x83, 0x74, 0x15, 0x2c, 0xbe, 0x7c);

//
// Specifies the utilization threshold in percent that must be crossed in order to un-park cores.
// 
// {df142941-20f3-4edf-9a4a-9c83d3d717d1}
// 
DEFINE_GUID( GUID_PROCESSOR_CORE_PARKING_INCREASE_THRESHOLD, 0xdf142941, 0x20f3, 0x4edf, 0x9a, 0x4a, 0x9c, 0x83, 0xd3, 0xd7, 0x17, 0xd1 );

//
// Specifies the utilization threshold in percent that must be crossed in order to park cores.
// 
// {68dd2f27-a4ce-4e11-8487-3794e4135dfa}
// 
DEFINE_GUID( GUID_PROCESSOR_CORE_PARKING_DECREASE_THRESHOLD, 0x68dd2f27, 0xa4ce, 0x4e11, 0x84, 0x87, 0x37, 0x94, 0xe4, 0x13, 0x5d, 0xfa);

//
// Specifies, either as ideal, single or rocket, how aggressive core parking is when cores must be unparked.
// 
// {c7be0679-2817-4d69-9d02-519a537ed0c6}
// 
DEFINE_GUID( GUID_PROCESSOR_CORE_PARKING_INCREASE_POLICY, 0xc7be0679, 0x2817, 0x4d69, 0x9d, 0x02, 0x51, 0x9a, 0x53, 0x7e, 0xd0, 0xc6);

#define CORE_PARKING_POLICY_CHANGE_IDEAL  0
#define CORE_PARKING_POLICY_CHANGE_SINGLE 1
#define CORE_PARKING_POLICY_CHANGE_ROCKET 2
#define CORE_PARKING_POLICY_CHANGE_MAX CORE_PARKING_POLICY_CHANGE_ROCKET

// 
// Specifies, either as ideal, single or rocket, how aggressive core parking is when cores must be parked.
// 
// {71021b41-c749-4d21-be74-a00f335d582b}
// 
DEFINE_GUID( GUID_PROCESSOR_CORE_PARKING_DECREASE_POLICY, 0x71021b41, 0xc749, 0x4d21, 0xbe, 0x74, 0xa0, 0x0f, 0x33, 0x5d, 0x58, 0x2b);

//
// Specifies, on a per processor group basis, the maximum number of cores that can be kept unparked. 
// 
// {ea062031-0e34-4ff1-9b6d-eb1059334028}
// 
DEFINE_GUID( GUID_PROCESSOR_CORE_PARKING_MAX_CORES, 0xea062031, 0x0e34, 0x4ff1, 0x9b, 0x6d, 0xeb, 0x10, 0x59, 0x33, 0x40, 0x28);

//
// Specifies, on a per processor group basis, the minimum number of cores that must be kept unparked.
// 
// {0cc5b647-c1df-4637-891a-dec35c318583}
// 
DEFINE_GUID( GUID_PROCESSOR_CORE_PARKING_MIN_CORES, 0x0cc5b647, 0xc1df, 0x4637, 0x89, 0x1a, 0xde, 0xc3, 0x5c, 0x31, 0x85, 0x83);

//
// Specifies, in milliseconds, the minimum amount of time a core must be parked before it can be unparked.
// 
// {2ddd5a84-5a71-437e-912a-db0b8c788732}
//
DEFINE_GUID( GUID_PROCESSOR_CORE_PARKING_INCREASE_TIME, 0x2ddd5a84, 0x5a71, 0x437e, 0x91, 0x2a, 0xdb, 0x0b, 0x8c, 0x78, 0x87, 0x32);

//
// Specifies, in milliseconds, the minimum amount of time a core must be unparked before it can be parked.
// 
// {dfd10d17-d5eb-45dd-877a-9a34ddd15c82}
// 
DEFINE_GUID( GUID_PROCESSOR_CORE_PARKING_DECREASE_TIME, 0xdfd10d17, 0xd5eb, 0x45dd, 0x87, 0x7a, 0x9a, 0x34, 0xdd, 0xd1, 0x5c, 0x82);

//
// Specifies the factor by which to decrease affinity history on each core after each check.
// 
// {8f7b45e3-c393-480a-878c-f67ac3d07082}
// 
DEFINE_GUID( GUID_PROCESSOR_CORE_PARKING_AFFINITY_HISTORY_DECREASE_FACTOR, 0x8f7b45e3, 0xc393, 0x480a, 0x87, 0x8c, 0xf6, 0x7a, 0xc3, 0xd0, 0x70, 0x82);

//
// Specifies the threshold above which a core is considered to have had significant affinitized work scheduled to it while parked.
// 
// {5b33697b-e89d-4d38-aa46-9e7dfb7cd2f9}
// 
DEFINE_GUID( GUID_PROCESSOR_CORE_PARKING_AFFINITY_HISTORY_THRESHOLD, 0x5b33697b, 0xe89d, 0x4d38, 0xaa, 0x46, 0x9e, 0x7d, 0xfb, 0x7c, 0xd2, 0xf9);

//
// Specifies the weighting given to each occurence where affinitized work was scheduled to a parked core.
// 
// {e70867f1-fa2f-4f4e-aea1-4d8a0ba23b20}
// 
DEFINE_GUID( GUID_PROCESSOR_CORE_PARKING_AFFINITY_WEIGHTING, 0xe70867f1, 0xfa2f, 0x4f4e, 0xae, 0xa1, 0x4d, 0x8a, 0x0b, 0xa2, 0x3b, 0x20);

//
// Specifies the factor by which to decrease the over utilization history on each core after the current performance check.
// 
// {1299023c-bc28-4f0a-81ec-d3295a8d815d}
// 
DEFINE_GUID( GUID_PROCESSOR_CORE_PARKING_OVER_UTILIZATION_HISTORY_DECREASE_FACTOR, 0x1299023c, 0xbc28, 0x4f0a, 0x81, 0xec, 0xd3, 0x29, 0x5a, 0x8d, 0x81, 0x5d);

//
// Specifies the threshold above which a core is considered to have been recently over utilized while parked.
// 
// {9ac18e92-aa3c-4e27-b307-01ae37307129}
// 
DEFINE_GUID( GUID_PROCESSOR_CORE_PARKING_OVER_UTILIZATION_HISTORY_THRESHOLD, 0x9ac18e92, 0xaa3c, 0x4e27, 0xb3, 0x07, 0x01, 0xae, 0x37, 0x30, 0x71, 0x29);

//
// Specifies the weighting given to each occurence where a parked core is found to be over utilized.
// 
// {8809c2d8-b155-42d4-bcda-0d345651b1db}
// 
DEFINE_GUID( GUID_PROCESSOR_CORE_PARKING_OVER_UTILIZATION_WEIGHTING, 0x8809c2d8, 0xb155, 0x42d4, 0xbc, 0xda, 0x0d, 0x34, 0x56, 0x51, 0xb1, 0xdb);

//
// Specifies, in percentage, the busy threshold that must be met before a parked core is considered over utilized.
// 
// {943c8cb6-6f93-4227-ad87-e9a3feec08d1}
// 
DEFINE_GUID( GUID_PROCESSOR_CORE_PARKING_OVER_UTILIZATION_THRESHOLD, 0x943c8cb6, 0x6f93, 0x4227, 0xad, 0x87, 0xe9, 0xa3, 0xfe, 0xec, 0x08, 0xd1);

//
// Specifies if at least one processor per core should always remain unparked.
// 
// {a55612aa-f624-42c6-a443-7397d064c04f}
// 

DEFINE_GUID( GUID_PROCESSOR_PARKING_CORE_OVERRIDE, 0xa55612aa, 0xf624, 0x42c6, 0xa4, 0x43, 0x73, 0x97, 0xd0, 0x64, 0xc0, 0x4f);

//
// Specifies what performance state a processor should enter when first parked.
// 
// {447235c7-6a8d-4cc0-8e24-9eaf70b96e2b}
// 

DEFINE_GUID( GUID_PROCESSOR_PARKING_PERF_STATE, 0x447235c7, 0x6a8d, 0x4cc0, 0x8e, 0x24, 0x9e, 0xaf, 0x70, 0xb9, 0x6e, 0x2b);

//
// Specifies the number of perf time check intervals to average utility over.
//
// {7d24baa7-0b84-480f-840c-1b0743c00f5f}
//
DEFINE_GUID( GUID_PROCESSOR_PERF_HISTORY, 0x7d24baa7, 0x0b84, 0x480f, 0x84, 0x0c, 0x1b, 0x07, 0x43, 0xc0, 0x0f, 0x5f);

//
// Specifies active vs passive cooling.  Although not directly related to
// processor settings, it is the processor that gets throttled if we're doing
// passive cooling, so it is fairly strongly related.
// {94D3A615-A899-4AC5-AE2B-E4D8F634367F}
//
DEFINE_GUID( GUID_SYSTEM_COOLING_POLICY, 0x94D3A615, 0xA899, 0x4AC5, 0xAE, 0x2B, 0xE4, 0xD8, 0xF6, 0x34, 0x36, 0x7F);

// Lock Console on Wake
// --------------------
//

// Specifies the behavior of the system when we wake from standby or
// hibernate.  If this is set, then we will cause the console to lock
// after we resume.
//
DEFINE_GUID( GUID_LOCK_CONSOLE_ON_WAKE, 0x0E796BDB, 0x100D, 0x47D6, 0xA2, 0xD5, 0xF7, 0xD2, 0xDA, 0xA5, 0x1F, 0x51 );

// Device idle characteristics
// ---------------------------
//
// Specifies whether to use the "performance" or "conservative" timeouts for
// device idle management.
//
// 4faab71a-92e5-4726-b531-224559672d19
//
DEFINE_GUID( GUID_DEVICE_IDLE_POLICY, 0x4faab71a, 0x92e5, 0x4726, 0xb5, 0x31, 0x22, 0x45, 0x59, 0x67, 0x2d, 0x19 );

#define POWER_DEVICE_IDLE_POLICY_PERFORMANCE  0
#define POWER_DEVICE_IDLE_POLICY_CONSERVATIVE 1

// AC/DC power source
// ------------------
//

// Specifies the power source for the system.  consumers may register for
// notification when the power source changes and will be notified with
// one of 3 values:
// 0 - Indicates the system is being powered by an AC power source.
// 1 - Indicates the system is being powered by a DC power source.
// 2 - Indicates the system is being powered by a short-term DC power
//     source.  For example, this would be the case if the system is
//     being powed by a short-term battery supply in a backing UPS
//     system.  When this value is recieved, the consumer should make
//     preparations for either a system hibernate or system shutdown.
//
// { 5D3E9A59-E9D5-4B00-A6BD-FF34FF516548 }
DEFINE_GUID( GUID_ACDC_POWER_SOURCE, 0x5D3E9A59, 0xE9D5, 0x4B00, 0xA6, 0xBD, 0xFF, 0x34, 0xFF, 0x51, 0x65, 0x48 );

// Lid state changes
// -----------------
//
// Specifies the current state of the lid (open or closed). The callback won't
// be called at all until a lid device is found and its current state is known.
//
// Values:
//
// 0 - closed
// 1 - opened
//
// { BA3E0F4D-B817-4094-A2D1-D56379E6A0F3 }
//

DEFINE_GUID( GUID_LIDSWITCH_STATE_CHANGE,  0xBA3E0F4D, 0xB817, 0x4094, 0xA2, 0xD1, 0xD5, 0x63, 0x79, 0xE6, 0xA0, 0xF3 );

// Battery life remaining
// ----------------------
//

// Specifies the percentage of battery life remaining.  The consumer
// may register for notification in order to track battery life in
// a fine-grained manner.
//
// Once registered, the consumer can expect to be notified as the battery
// life percentage changes.
//
// The consumer will recieve a value between 0 and 100 (inclusive) which
// indicates percent battery life remaining.
//
// { A7AD8041-B45A-4CAE-87A3-EECBB468A9E1 }
DEFINE_GUID( GUID_BATTERY_PERCENTAGE_REMAINING, 0xA7AD8041, 0xB45A, 0x4CAE, 0x87, 0xA3, 0xEE, 0xCB, 0xB4, 0x68, 0xA9, 0xE1 );


// Notification to listeners that the system is fairly busy and won't be moving
// into an idle state any time soon.  This can be used as a hint to listeners
// that now might be a good time to do background tasks.
//
DEFINE_GUID( GUID_IDLE_BACKGROUND_TASK, 0x515C31D8, 0xF734, 0x163D, 0xA0, 0xFD, 0x11, 0xA0, 0x8C, 0x91, 0xE8, 0xF1 );

// Notification to listeners that the system is fairly busy and won't be moving
// into an idle state any time soon.  This can be used as a hint to listeners
// that now might be a good time to do background tasks.
//
// { CF23F240-2A54-48D8-B114-DE1518FF052E }
DEFINE_GUID( GUID_BACKGROUND_TASK_NOTIFICATION, 0xCF23F240, 0x2A54, 0x48D8, 0xB1, 0x14, 0xDE, 0x15, 0x18, 0xFF, 0x05, 0x2E );

// Define a GUID that will represent the action of a direct experience button
// on the platform.  Users will register for this DPPE setting and recieve
// notification when the h/w button is pressed.
//
// { 1A689231-7399-4E9A-8F99-B71F999DB3FA }
//
DEFINE_GUID( GUID_APPLAUNCH_BUTTON, 0x1A689231, 0x7399, 0x4E9A, 0x8F, 0x99, 0xB7, 0x1F, 0x99, 0x9D, 0xB3, 0xFA );

// PCI Express power settings
// ------------------------
//

// Specifies the subgroup which will contain all of the PCI Express
// settings for a single policy.
//
// {501a4d13-42af-4429-9fd1-a8218c268e20}
//
DEFINE_GUID( GUID_PCIEXPRESS_SETTINGS_SUBGROUP, 0x501a4d13, 0x42af,0x4429, 0x9f, 0xd1, 0xa8, 0x21, 0x8c, 0x26, 0x8e, 0x20 );

// Specifies the PCI Express ASPM power policy.
//
// {ee12f906-d277-404b-b6da-e5fa1a576df5}
//
DEFINE_GUID( GUID_PCIEXPRESS_ASPM_POLICY, 0xee12f906, 0xd277, 0x404b, 0xb6, 0xda, 0xe5, 0xfa, 0x1a, 0x57, 0x6d, 0xf5 );

// POWER Shutdown settings
// ------------------------
//

// Specifies if forced shutdown should be used for all button and lid initiated
// shutdown actions.
//
// {833a6b62-dfa4-46d1-82f8-e09e34d029d6}
//

DEFINE_GUID( GUID_ENABLE_SWITCH_FORCED_SHUTDOWN, 0x833a6b62, 0xdfa4, 0x46d1, 0x82, 0xf8, 0xe0, 0x9e, 0x34, 0xd0, 0x29, 0xd6 );

// end_winnt  end_wdm end_ntminiport
#endif // (NTDDI_VERSION >= NTDDI_VISTA)




#ifndef _NTPOAPI_
#define _NTPOAPI_

#if _MSC_VER >= 1200
#pragma warning(push)
#endif
#pragma warning(disable:4201) // nameless struct/union
#pragma warning(disable:4214) // bit field types other than int


#ifdef __cplusplus
extern "C" {
#endif

//
// Power Management user APIs
//

// begin_ntminiport begin_wdm

#ifndef _PO_DDK_
#define _PO_DDK_

// begin_winnt

typedef enum _SYSTEM_POWER_STATE {
    PowerSystemUnspecified = 0,
    PowerSystemWorking     = 1,
    PowerSystemSleeping1   = 2,
    PowerSystemSleeping2   = 3,
    PowerSystemSleeping3   = 4,
    PowerSystemHibernate   = 5,
    PowerSystemShutdown    = 6,
    PowerSystemMaximum     = 7
} SYSTEM_POWER_STATE, *PSYSTEM_POWER_STATE;

#define POWER_SYSTEM_MAXIMUM 7

typedef enum {
    PowerActionNone = 0,
    PowerActionReserved,
    PowerActionSleep,
    PowerActionHibernate,
    PowerActionShutdown,
    PowerActionShutdownReset,
    PowerActionShutdownOff,
    PowerActionWarmEject
} POWER_ACTION, *PPOWER_ACTION;

typedef enum _DEVICE_POWER_STATE {
    PowerDeviceUnspecified = 0,
    PowerDeviceD0,
    PowerDeviceD1,
    PowerDeviceD2,
    PowerDeviceD3,
    PowerDeviceMaximum
} DEVICE_POWER_STATE, *PDEVICE_POWER_STATE;

typedef enum _MONITOR_DISPLAY_STATE {
    PowerMonitorOff = 0,
    PowerMonitorOn,
    PowerMonitorDim
} MONITOR_DISPLAY_STATE, *PMONITOR_DISPLAY_STATE;

// end_winnt

typedef union _POWER_STATE {
    SYSTEM_POWER_STATE SystemState;
    DEVICE_POWER_STATE DeviceState;
} POWER_STATE, *PPOWER_STATE;

typedef enum _POWER_STATE_TYPE {
    SystemPowerState = 0,
    DevicePowerState
} POWER_STATE_TYPE, *PPOWER_STATE_TYPE;

#if (NTDDI_VERSION >= NTDDI_VISTA)
typedef struct _SYSTEM_POWER_STATE_CONTEXT {
    union {
        struct {
            ULONG   Reserved1             : 8;
            ULONG   TargetSystemState     : 4;
            ULONG   EffectiveSystemState  : 4;
            ULONG   CurrentSystemState    : 4;
            ULONG   IgnoreHibernationPath : 1;
            ULONG   PseudoTransition      : 1;
            ULONG   Reserved2             : 10;
        } DUMMYSTRUCTNAME;

        ULONG ContextAsUlong;
    } DUMMYUNIONNAME;
} SYSTEM_POWER_STATE_CONTEXT, *PSYSTEM_POWER_STATE_CONTEXT;
#endif // (NTDDI_VERSION >= NTDDI_VISTA)

#if (NTDDI_VERSION >= NTDDI_WIN7)

typedef struct _COUNTED_REASON_CONTEXT {
    ULONG Version;
    ULONG Flags;
    union {
        struct {
            UNICODE_STRING ResourceFileName;
            USHORT ResourceReasonId;
            ULONG StringCount;
            PUNICODE_STRING __field_ecount(StringCount) ReasonStrings;
        } DUMMYSTRUCTNAME;

        UNICODE_STRING SimpleString;
    } DUMMYUNIONNAME;
} COUNTED_REASON_CONTEXT, *PCOUNTED_REASON_CONTEXT;

#endif // (NTDDI_VERSION >= NTDDI_WIN7)

//
// Generic power related IOCTLs
//

#define IOCTL_QUERY_DEVICE_POWER_STATE  \
        CTL_CODE(FILE_DEVICE_BATTERY, 0x0, METHOD_BUFFERED, FILE_READ_ACCESS)

#define IOCTL_SET_DEVICE_WAKE           \
        CTL_CODE(FILE_DEVICE_BATTERY, 0x1, METHOD_BUFFERED, FILE_WRITE_ACCESS)

#define IOCTL_CANCEL_DEVICE_WAKE        \
        CTL_CODE(FILE_DEVICE_BATTERY, 0x2, METHOD_BUFFERED, FILE_WRITE_ACCESS)


//
// Defines for W32 interfaces
//

// begin_winnt

#define ES_SYSTEM_REQUIRED   ((ULONG)0x00000001)
#define ES_DISPLAY_REQUIRED  ((ULONG)0x00000002)
#define ES_USER_PRESENT      ((ULONG)0x00000004)
#define ES_AWAYMODE_REQUIRED ((ULONG)0x00000040)
#define ES_CONTINUOUS        ((ULONG)0x80000000)

typedef ULONG EXECUTION_STATE, *PEXECUTION_STATE;

typedef enum {
    LT_DONT_CARE,
    LT_LOWEST_LATENCY
} LATENCY_TIME;

#if (_WIN32_WINNT >= _WIN32_WINNT_WIN7)

#define DIAGNOSTIC_REASON_VERSION              0

#define DIAGNOSTIC_REASON_SIMPLE_STRING        0x00000001
#define DIAGNOSTIC_REASON_DETAILED_STRING      0x00000002
#define DIAGNOSTIC_REASON_NOT_SPECIFIED        0x80000000
#define DIAGNOSTIC_REASON_INVALID_FLAGS        (~0x80000003)

#endif // (_WIN32_WINNT >= _WIN32_WINNT_WIN7)

//
// Defines for power request APIs
//

#define POWER_REQUEST_CONTEXT_VERSION          0

#define POWER_REQUEST_CONTEXT_SIMPLE_STRING    0x00000001
#define POWER_REQUEST_CONTEXT_DETAILED_STRING  0x00000002

//
// N.B. The maximum is a macro (rather than part of enum) for cgen to be able
// to parse power.h correctly. When a new power request type is added, the
// PowerRequestMaximum should be manually incremented.
//

typedef enum _POWER_REQUEST_TYPE {
    PowerRequestDisplayRequired,
    PowerRequestSystemRequired,
    PowerRequestAwayModeRequired
} POWER_REQUEST_TYPE, *PPOWER_REQUEST_TYPE;

#define PowerRequestMaximum 3

// end_ntminiport

#if (NTDDI_VERSION >= NTDDI_WINXP)

//-----------------------------------------------------------------------------
// Device Power Information
// Accessable via CM_Get_DevInst_Registry_Property_Ex(CM_DRP_DEVICE_POWER_DATA)
//-----------------------------------------------------------------------------

#define PDCAP_D0_SUPPORTED              0x00000001
#define PDCAP_D1_SUPPORTED              0x00000002
#define PDCAP_D2_SUPPORTED              0x00000004
#define PDCAP_D3_SUPPORTED              0x00000008
#define PDCAP_WAKE_FROM_D0_SUPPORTED    0x00000010
#define PDCAP_WAKE_FROM_D1_SUPPORTED    0x00000020
#define PDCAP_WAKE_FROM_D2_SUPPORTED    0x00000040
#define PDCAP_WAKE_FROM_D3_SUPPORTED    0x00000080
#define PDCAP_WARM_EJECT_SUPPORTED      0x00000100

typedef struct CM_Power_Data_s {
    ULONG               PD_Size;
    DEVICE_POWER_STATE  PD_MostRecentPowerState;
    ULONG               PD_Capabilities;
    ULONG               PD_D1Latency;
    ULONG               PD_D2Latency;
    ULONG               PD_D3Latency;
    DEVICE_POWER_STATE  PD_PowerStateMapping[POWER_SYSTEM_MAXIMUM];
    SYSTEM_POWER_STATE  PD_DeepestSystemWake;
} CM_POWER_DATA, *PCM_POWER_DATA;

#endif // (NTDDI_VERSION >= NTDDI_WINXP)

// begin_wdm

typedef enum {
    SystemPowerPolicyAc,
    SystemPowerPolicyDc,
    VerifySystemPolicyAc,
    VerifySystemPolicyDc,
    SystemPowerCapabilities,
    SystemBatteryState,
    SystemPowerStateHandler,
    ProcessorStateHandler,
    SystemPowerPolicyCurrent,
    AdministratorPowerPolicy,
    SystemReserveHiberFile,
    ProcessorInformation,
    SystemPowerInformation,
    ProcessorStateHandler2,
    LastWakeTime,                                   // Compare with KeQueryInterruptTime()
    LastSleepTime,                                  // Compare with KeQueryInterruptTime()
    SystemExecutionState,
    SystemPowerStateNotifyHandler,
    ProcessorPowerPolicyAc,
    ProcessorPowerPolicyDc,
    VerifyProcessorPowerPolicyAc,
    VerifyProcessorPowerPolicyDc,
    ProcessorPowerPolicyCurrent,
    SystemPowerStateLogging,
    SystemPowerLoggingEntry,
    SetPowerSettingValue,
    NotifyUserPowerSetting,
    PowerInformationLevelUnused0,
    PowerInformationLevelUnused1,
    SystemVideoState,
    TraceApplicationPowerMessage,
    TraceApplicationPowerMessageEnd,
    ProcessorPerfStates,
    ProcessorIdleStates,
    ProcessorCap,
    SystemWakeSource,
    SystemHiberFileInformation,
    TraceServicePowerMessage,
    ProcessorLoad,
    PowerShutdownNotification,
    MonitorCapabilities,
    SessionPowerInit,
    SessionDisplayState,
    PowerRequestCreate,
    PowerRequestAction,
    GetPowerRequestList,
    ProcessorInformationEx,
    NotifyUserModeLegacyPowerEvent,
    GroupPark,
    ProcessorIdleDomains,
    WakeTimerList,
    SystemHiberFileSize,
    PowerInformationLevelMaximum
} POWER_INFORMATION_LEVEL;

//
// Power Setting definitions
//

typedef enum {
    PoAc,
    PoDc,
    PoHot,
    PoConditionMaximum
} SYSTEM_POWER_CONDITION;

typedef struct {

    //
    // Version of this structure.  Currently should be set to
    // POWER_SETTING_VALUE_VERSION.
    //
    ULONG       Version;


    //
    // GUID representing the power setting being applied.
    //
    GUID        Guid;


    //
    // What power state should this setting be applied to?  E.g.
    // AC, DC, thermal, ...
    //
    SYSTEM_POWER_CONDITION PowerCondition;

    //
    // Length (in bytes) of the 'Data' member.
    //
    ULONG       DataLength;

    //
    // Data which contains the actual setting value.
    //
    UCHAR   Data[ANYSIZE_ARRAY];
} SET_POWER_SETTING_VALUE, *PSET_POWER_SETTING_VALUE;

#define POWER_SETTING_VALUE_VERSION (0x1)

typedef struct {
    GUID Guid;
} NOTIFY_USER_POWER_SETTING, *PNOTIFY_USER_POWER_SETTING;

//
// Package definition for an experience button device notification.  When
// someone registers for GUID_EXPERIENCE_BUTTON, this is the definition of
// the setting data they'll get.
//
typedef struct _APPLICATIONLAUNCH_SETTING_VALUE {

    //
    // System time when the most recent button press ocurred.  Note that this is
    // specified in 100ns internvals since January 1, 1601.
    //
    LARGE_INTEGER       ActivationTime;

    //
    // Reserved for internal use.
    //
    ULONG               Flags;

    //
    // which instance of this device was pressed?
    //
    ULONG               ButtonInstanceID;


} APPLICATIONLAUNCH_SETTING_VALUE, *PAPPLICATIONLAUNCH_SETTING_VALUE;

//
// define platform roles
//

typedef enum {
    PlatformRoleUnspecified = 0,
    PlatformRoleDesktop,
    PlatformRoleMobile,
    PlatformRoleWorkstation,
    PlatformRoleEnterpriseServer,
    PlatformRoleSOHOServer,
    PlatformRoleAppliancePC,
    PlatformRolePerformanceServer,
    PlatformRoleMaximum
} POWER_PLATFORM_ROLE;

//
// System power manager capabilities
//

#if (NTDDI_VERSION >= NTDDI_WINXP) || !defined(_BATCLASS_)
typedef struct {
    ULONG       Granularity;
    ULONG       Capacity;
} BATTERY_REPORTING_SCALE, *PBATTERY_REPORTING_SCALE;
#endif // (NTDDI_VERSION >= NTDDI_WINXP) || !defined(_BATCLASS_)

// end_winnt
#endif // !_PO_DDK_
// end_wdm end_ntminiport
// begin_nthal

#define POWER_PERF_SCALE    100
#define PERF_LEVEL_TO_PERCENT(_x_) ((_x_ * 1000) / (POWER_PERF_SCALE * 10))
#define PERCENT_TO_PERF_LEVEL(_x_) ((_x_ * POWER_PERF_SCALE * 10) / 1000)

//
// Policy manager state handler interfaces
//

// power state handlers

typedef enum {
    PowerStateSleeping1 = 0,
    PowerStateSleeping2 = 1,
    PowerStateSleeping3 = 2,
    PowerStateSleeping4 = 3,
    PowerStateShutdownOff = 4,
    PowerStateShutdownReset = 5,
    PowerStateSleeping4Firmware = 6,
    PowerStateMaximum = 7
} POWER_STATE_HANDLER_TYPE, *PPOWER_STATE_HANDLER_TYPE;

#define POWER_STATE_HANDLER_TYPE_MAX 8

__drv_functionClass(ENTER_STATE_SYSTEM_HANDLER)
__drv_sameIRQL
typedef
NTSTATUS
(ENTER_STATE_SYSTEM_HANDLER)(
    __in PVOID SystemContext
    );

typedef ENTER_STATE_SYSTEM_HANDLER *PENTER_STATE_SYSTEM_HANDLER;

__drv_functionClass(ENTER_STATE_HANDLER)
__drv_sameIRQL
typedef
NTSTATUS
(ENTER_STATE_HANDLER)(
    __in_opt PVOID Context,
    __in_opt PENTER_STATE_SYSTEM_HANDLER SystemHandler,
    __in_opt PVOID SystemContext,
    __in LONG NumberProcessors,
    __in_opt LONG volatile *Number
    );

typedef ENTER_STATE_HANDLER *PENTER_STATE_HANDLER;

typedef struct {
    POWER_STATE_HANDLER_TYPE    Type;
    BOOLEAN                     RtcWake;
    UCHAR                       Spare[3];
    PENTER_STATE_HANDLER        Handler;
    PVOID                       Context;
} POWER_STATE_HANDLER, *PPOWER_STATE_HANDLER;


#if (NTDDI_VERSION >= NTDDI_WINXP)

__drv_functionClass(ENTER_STATE_NOTIFY_HANDLER)
__drv_sameIRQL
typedef
NTSTATUS
(ENTER_STATE_NOTIFY_HANDLER)(
    __in POWER_STATE_HANDLER_TYPE State,
    __in PVOID Context,
    __in BOOLEAN Entering
    );

typedef ENTER_STATE_NOTIFY_HANDLER *PENTER_STATE_NOTIFY_HANDLER;

typedef struct {
    PENTER_STATE_NOTIFY_HANDLER Handler;
    PVOID                       Context;
} POWER_STATE_NOTIFY_HANDLER, *PPOWER_STATE_NOTIFY_HANDLER;

#endif // (NTDDI_VERSION >= NTDDI_WINXP)

__drv_maxIRQL(APC_LEVEL)
__kernel_entry NTSYSCALLAPI
NTSTATUS
NTAPI
NtPowerInformation(
    __in POWER_INFORMATION_LEVEL InformationLevel,
    __in_bcount_opt(InputBufferLength) PVOID InputBuffer,
    __in ULONG InputBufferLength,
    __out_bcount_opt(OutputBufferLength) PVOID OutputBuffer,
    __in ULONG OutputBufferLength
    );

//
// Processor Power Management external interface definitions / info.
//

typedef struct {
    ULONG64 StartTime;
    ULONG64 EndTime;
    ULONG Reserved[4];
} PROCESSOR_IDLE_TIMES, *PPROCESSOR_IDLE_TIMES;

__drv_functionClass(PROCESSOR_IDLE_HANDLER)
typedef
NTSTATUS
(FASTCALL PROCESSOR_IDLE_HANDLER) (
    __in ULONG_PTR Context,
    __inout PPROCESSOR_IDLE_TIMES IdleTimes
    );

typedef PROCESSOR_IDLE_HANDLER *PPROCESSOR_IDLE_HANDLER;

//
// PROCESSOR_IDLE_STATE "StateFlags" definitions.
//

#define IDLE_STATE_FLAGS_C1_HLT     0x01        // describes C1 only
#define IDLE_STATE_FLAGS_C1_IO_HLT  0x02        // describes C1 only
#define IDLE_STATE_FLAGS_IO         0x04        // describes C2 and C3 only
#define IDLE_STATE_FLAGS_MWAIT      0x08        // describes C1, C2, C3, C4, ...

typedef struct _PROCESSOR_IDLE_STATE {
    UCHAR StateType;
    ULONG StateFlags;
    ULONG HardwareLatency;
    ULONG Power;
    ULONG_PTR Context;
    PPROCESSOR_IDLE_HANDLER Handler;
} PROCESSOR_IDLE_STATE, *PPROCESSOR_IDLE_STATE;

typedef struct _PROCESSOR_IDLE_STATES {
    ULONG Size;
    ULONG Revision;
    ULONG Count;
    ULONG Type;
    KAFFINITY TargetProcessors;
    PROCESSOR_IDLE_STATE State[ANYSIZE_ARRAY];
} PROCESSOR_IDLE_STATES, *PPROCESSOR_IDLE_STATES;

// PPM Perf.

#define PROCESSOR_STATE_TYPE_PERFORMANCE    0x1
#define PROCESSOR_STATE_TYPE_THROTTLE       0x2


//
// begin_winnt
//

typedef struct {
    ULONG   Frequency;
    ULONG   Flags;
    ULONG   PercentFrequency;
} PPM_WMI_LEGACY_PERFSTATE, *PPPM_WMI_LEGACY_PERFSTATE;

typedef struct {
    ULONG Latency;
    ULONG Power;
    ULONG TimeCheck;
    UCHAR PromotePercent;
    UCHAR DemotePercent;
    UCHAR StateType;
    UCHAR Reserved;
    ULONG StateFlags;
    ULONG Context;
    ULONG IdleHandler;
    ULONG Reserved1;            // reserved for future use
} PPM_WMI_IDLE_STATE, *PPPM_WMI_IDLE_STATE;

typedef struct {
    ULONG Type;
    ULONG Count;
    ULONG TargetState;          // current idle state
    ULONG OldState;             // previous idle state
    ULONG64 TargetProcessors;
    PPM_WMI_IDLE_STATE State[ANYSIZE_ARRAY];
} PPM_WMI_IDLE_STATES, *PPPM_WMI_IDLE_STATES;

typedef struct {
    ULONG Type;
    ULONG Count;
    ULONG TargetState;          // current idle state
    ULONG OldState;             // previous idle state
    PVOID TargetProcessors;
    PPM_WMI_IDLE_STATE State[ANYSIZE_ARRAY];
} PPM_WMI_IDLE_STATES_EX, *PPPM_WMI_IDLE_STATES_EX;

typedef struct {
    ULONG Frequency;            // in Mhz
    ULONG Power;                // in milliwatts
    UCHAR PercentFrequency;
    UCHAR IncreaseLevel;        // goto higher state
    UCHAR DecreaseLevel;        // goto lower state
    UCHAR Type;                 // performance or throttle
    ULONG IncreaseTime;         // in tick counts
    ULONG DecreaseTime;         // in tick counts
    ULONG64 Control;            // control value
    ULONG64 Status;             // control value
    ULONG HitCount;
    ULONG Reserved1;            // reserved for future use
    ULONG64 Reserved2;
    ULONG64 Reserved3;
} PPM_WMI_PERF_STATE, *PPPM_WMI_PERF_STATE;

typedef struct {
    ULONG Count;
    ULONG MaxFrequency;
    ULONG CurrentState;         // current state
    ULONG MaxPerfState;         // fastest state considering policy restrictions
    ULONG MinPerfState;         // slowest state considering policy restrictions
    ULONG LowestPerfState;      // slowest perf state, fixed, aka the "knee"
    ULONG ThermalConstraint;
    UCHAR BusyAdjThreshold;
    UCHAR PolicyType;           // domain coordination
    UCHAR Type;
    UCHAR Reserved;
    ULONG TimerInterval;
    ULONG64 TargetProcessors;   // domain affinity
    ULONG PStateHandler;
    ULONG PStateContext;
    ULONG TStateHandler;
    ULONG TStateContext;
    ULONG FeedbackHandler;
    ULONG Reserved1;
    ULONG64 Reserved2;
    PPM_WMI_PERF_STATE State[ANYSIZE_ARRAY];
} PPM_WMI_PERF_STATES, *PPPM_WMI_PERF_STATES;

typedef struct {
    ULONG Count;
    ULONG MaxFrequency;
    ULONG CurrentState;         // current state
    ULONG MaxPerfState;         // fastest state considering policy restrictions
    ULONG MinPerfState;         // slowest state considering policy restrictions
    ULONG LowestPerfState;      // slowest perf state, fixed, aka the "knee"
    ULONG ThermalConstraint;
    UCHAR BusyAdjThreshold;
    UCHAR PolicyType;           // domain coordination
    UCHAR Type;
    UCHAR Reserved;
    ULONG TimerInterval;
    PVOID TargetProcessors;     // domain affinity
    ULONG PStateHandler;
    ULONG PStateContext;
    ULONG TStateHandler;
    ULONG TStateContext;
    ULONG FeedbackHandler;
    ULONG Reserved1;
    ULONG64 Reserved2;
    PPM_WMI_PERF_STATE State[ANYSIZE_ARRAY];
} PPM_WMI_PERF_STATES_EX, *PPPM_WMI_PERF_STATES_EX;

//
// Legacy processor idle accounting.
//

#define PROC_IDLE_BUCKET_COUNT 6

typedef struct {
    ULONG IdleTransitions;
    ULONG FailedTransitions;
    ULONG InvalidBucketIndex;
    ULONG64 TotalTime;
    ULONG IdleTimeBuckets[PROC_IDLE_BUCKET_COUNT];
} PPM_IDLE_STATE_ACCOUNTING, *PPPM_IDLE_STATE_ACCOUNTING;

typedef struct {
    ULONG StateCount;
    ULONG TotalTransitions;
    ULONG ResetCount;
    ULONG64 StartTime;
    PPM_IDLE_STATE_ACCOUNTING State[ANYSIZE_ARRAY];
} PPM_IDLE_ACCOUNTING, *PPPM_IDLE_ACCOUNTING;

//
// Processor idle accounting.
//

#define PROC_IDLE_BUCKET_COUNT_EX 16

typedef struct {
    ULONG64 TotalTimeUs;
    ULONG MinTimeUs;
    ULONG MaxTimeUs;
    ULONG Count;
} PPM_IDLE_STATE_BUCKET_EX, *PPPM_IDLE_STATE_BUCKET_EX;

typedef struct {
    ULONG64 TotalTime;
    ULONG IdleTransitions;
    ULONG FailedTransitions;
    ULONG InvalidBucketIndex;
    ULONG MinTimeUs;
    ULONG MaxTimeUs;
    PPM_IDLE_STATE_BUCKET_EX IdleTimeBuckets[PROC_IDLE_BUCKET_COUNT_EX];
} PPM_IDLE_STATE_ACCOUNTING_EX, *PPPM_IDLE_STATE_ACCOUNTING_EX;

typedef struct {
    ULONG StateCount;
    ULONG TotalTransitions;
    ULONG ResetCount;
    ULONG64 StartTime;
    __field_ecount(StateCount) PPM_IDLE_STATE_ACCOUNTING_EX State[ANYSIZE_ARRAY];
} PPM_IDLE_ACCOUNTING_EX, *PPPM_IDLE_ACCOUNTING_EX;

//
// Definitions of coordination types for _PSD, _TSD, and _CSD BIOS objects from
// the Acpi 3.0 specification
//

#define ACPI_PPM_SOFTWARE_ALL     0xFC
#define ACPI_PPM_SOFTWARE_ANY     0xFD
#define ACPI_PPM_HARDWARE_ALL     0xFE

//
// Definition of Microsoft PPM coordination types.
//

#define MS_PPM_SOFTWARE_ALL       0x1

//
// Processor firmware rundown feature bit definitions.
//

#define PPM_FIRMWARE_ACPI1C2      0x00000001
#define PPM_FIRMWARE_ACPI1C3      0x00000002
#define PPM_FIRMWARE_ACPI1TSTATES 0x00000004
#define PPM_FIRMWARE_CST          0x00000008
#define PPM_FIRMWARE_CSD          0x00000010
#define PPM_FIRMWARE_PCT          0x00000020
#define PPM_FIRMWARE_PSS          0x00000040
#define PPM_FIRMWARE_XPSS         0x00000080
#define PPM_FIRMWARE_PPC          0x00000100
#define PPM_FIRMWARE_PSD          0x00000200
#define PPM_FIRMWARE_PTC          0x00000400
#define PPM_FIRMWARE_TSS          0x00000800
#define PPM_FIRMWARE_TPC          0x00001000
#define PPM_FIRMWARE_TSD          0x00002000
#define PPM_FIRMWARE_PCCH         0x00004000
#define PPM_FIRMWARE_PCCP         0x00008000

//
// Processor Power Management WMI interface.
//

// {A5B32DDD-7F39-4abc-B892-900E43B59EBB}
DEFINE_GUID(PPM_PERFSTATE_CHANGE_GUID,
0xa5b32ddd, 0x7f39, 0x4abc, 0xb8, 0x92, 0x90, 0xe, 0x43, 0xb5, 0x9e, 0xbb);

// {995e6b7f-d653-497a-b978-36a30c29bf01}
DEFINE_GUID(PPM_PERFSTATE_DOMAIN_CHANGE_GUID,
0x995e6b7f, 0xd653, 0x497a, 0xb9, 0x78, 0x36, 0xa3, 0xc, 0x29, 0xbf, 0x1);

// {4838fe4f-f71c-4e51-9ecc-8430a7ac4c6c}
DEFINE_GUID(PPM_IDLESTATE_CHANGE_GUID,
0x4838fe4f, 0xf71c, 0x4e51, 0x9e, 0xcc, 0x84, 0x30, 0xa7, 0xac, 0x4c, 0x6c);

// {5708cc20-7d40-4bf4-b4aa-2b01338d0126}
DEFINE_GUID(PPM_PERFSTATES_DATA_GUID,
0x5708cc20, 0x7d40, 0x4bf4, 0xb4, 0xaa, 0x2b, 0x01, 0x33, 0x8d, 0x01, 0x26);

// {ba138e10-e250-4ad7-8616-cf1a7ad410e7}
DEFINE_GUID(PPM_IDLESTATES_DATA_GUID,
0xba138e10, 0xe250, 0x4ad7, 0x86, 0x16, 0xcf, 0x1a, 0x7a, 0xd4, 0x10, 0xe7);

// {e2a26f78-ae07-4ee0-a30f-ce354f5a94cd}
DEFINE_GUID(PPM_IDLE_ACCOUNTING_GUID,
0xe2a26f78, 0xae07, 0x4ee0, 0xa3, 0x0f, 0xce, 0x54, 0xf5, 0x5a, 0x94, 0xcd);

// {d67abd39-81f8-4a5e-8152-72e31ec912ee}
DEFINE_GUID(PPM_IDLE_ACCOUNTING_EX_GUID,
0xd67abd39, 0x81f8, 0x4a5e, 0x81, 0x52, 0x72, 0xe3, 0x1e, 0xc9, 0x12, 0xee);

// {a852c2c8-1a4c-423b-8c2c-f30d82931a88}
DEFINE_GUID(PPM_THERMALCONSTRAINT_GUID,
0xa852c2c8, 0x1a4c, 0x423b, 0x8c, 0x2c, 0xf3, 0x0d, 0x82, 0x93, 0x1a, 0x88);

// {7fd18652-0cfe-40d2-b0a1-0b066a87759e}
DEFINE_GUID(PPM_PERFMON_PERFSTATE_GUID,
0x7fd18652, 0xcfe, 0x40d2, 0xb0, 0xa1, 0xb, 0x6, 0x6a, 0x87, 0x75, 0x9e);

// {48f377b8-6880-4c7b-8bdc-380176c6654d}
DEFINE_GUID(PPM_THERMAL_POLICY_CHANGE_GUID,
0x48f377b8, 0x6880, 0x4c7b, 0x8b, 0xdc, 0x38, 0x1, 0x76, 0xc6, 0x65, 0x4d);


typedef struct {
    ULONG State;
    ULONG Status;
    ULONG Latency;
    ULONG Speed;
    ULONG Processor;
} PPM_PERFSTATE_EVENT, *PPPM_PERFSTATE_EVENT;

typedef struct {
    ULONG State;
    ULONG Latency;
    ULONG Speed;
    ULONG64 Processors;
} PPM_PERFSTATE_DOMAIN_EVENT, *PPPM_PERFSTATE_DOMAIN_EVENT;

typedef struct {
    ULONG NewState;
    ULONG OldState;
    ULONG64 Processors;
} PPM_IDLESTATE_EVENT, *PPPM_IDLESTATE_EVENT;

typedef struct {
    ULONG ThermalConstraint;
    ULONG64 Processors;
} PPM_THERMALCHANGE_EVENT, *PPPM_THERMALCHANGE_EVENT;

#pragma warning(push)
#pragma warning(disable:4121)

typedef struct {
    UCHAR Mode;
    ULONG64 Processors;
} PPM_THERMAL_POLICY_EVENT, *PPPM_THERMAL_POLICY_EVENT;

#pragma warning(pop)

// Power Policy Management interfaces
//

typedef struct {
    POWER_ACTION    Action;
    ULONG           Flags;
    ULONG           EventCode;
} POWER_ACTION_POLICY, *PPOWER_ACTION_POLICY;

// POWER_ACTION_POLICY->Flags:
#define POWER_ACTION_QUERY_ALLOWED      0x00000001
#define POWER_ACTION_UI_ALLOWED         0x00000002
#define POWER_ACTION_OVERRIDE_APPS      0x00000004
#define POWER_ACTION_PSEUDO_TRANSITION  0x08000000
#define POWER_ACTION_LIGHTEST_FIRST     0x10000000
#define POWER_ACTION_LOCK_CONSOLE       0x20000000
#define POWER_ACTION_DISABLE_WAKES      0x40000000
#define POWER_ACTION_CRITICAL           0x80000000

// POWER_ACTION_POLICY->EventCode flags
#define POWER_LEVEL_USER_NOTIFY_TEXT      0x00000001
#define POWER_LEVEL_USER_NOTIFY_SOUND     0x00000002
#define POWER_LEVEL_USER_NOTIFY_EXEC      0x00000004
#define POWER_USER_NOTIFY_BUTTON          0x00000008
#define POWER_USER_NOTIFY_SHUTDOWN        0x00000010
#define POWER_USER_NOTIFY_FORCED_SHUTDOWN 0x00000020
#define POWER_FORCE_TRIGGER_RESET         0x80000000

// Note: for battery alarm EventCodes, the ID of the battery alarm << 16 is ORed
// into the flags.  For example: DISCHARGE_POLICY_LOW << 16

//
// The GUID_BATTERY_DISCHARGE_FLAGS_x power settings use a subset of EventCode
// flags.  The POWER_FORCE_TRIGGER_RESET flag doesn't make sense for a battery
// alarm so it is overloaded for other purposes (gerneral enable/disable).
#define BATTERY_DISCHARGE_FLAGS_EVENTCODE_MASK  0x00000007
#define BATTERY_DISCHARGE_FLAGS_ENABLE  0x80000000

// system battery drain policies
typedef struct {
    BOOLEAN                 Enable;
    UCHAR                   Spare[3];
    ULONG                   BatteryLevel;
    POWER_ACTION_POLICY     PowerPolicy;
    SYSTEM_POWER_STATE      MinSystemState;
} SYSTEM_POWER_LEVEL, *PSYSTEM_POWER_LEVEL;

// Discharge policy constants
#define NUM_DISCHARGE_POLICIES      4
#define DISCHARGE_POLICY_CRITICAL   0
#define DISCHARGE_POLICY_LOW        1


// system power policies
typedef struct _SYSTEM_POWER_POLICY {
    ULONG                   Revision;       // 1

    // events
    POWER_ACTION_POLICY     PowerButton;
    POWER_ACTION_POLICY     SleepButton;
    POWER_ACTION_POLICY     LidClose;
    SYSTEM_POWER_STATE      LidOpenWake;
    ULONG                   Reserved;

    // "system idle" detection
    POWER_ACTION_POLICY     Idle;
    ULONG                   IdleTimeout;
    UCHAR                   IdleSensitivity;

    UCHAR                   DynamicThrottle;
    UCHAR                   Spare2[2];

    // meaning of power action "sleep"
    SYSTEM_POWER_STATE      MinSleep;
    SYSTEM_POWER_STATE      MaxSleep;
    SYSTEM_POWER_STATE      ReducedLatencySleep;
    ULONG                   WinLogonFlags;

    ULONG                   Spare3;

    // parameters for dozing
    //
    ULONG                   DozeS4Timeout;

    // battery policies
    ULONG                   BroadcastCapacityResolution;
    SYSTEM_POWER_LEVEL      DischargePolicy[NUM_DISCHARGE_POLICIES];

    // video policies
    ULONG                   VideoTimeout;
    BOOLEAN                 VideoDimDisplay;
    ULONG                   VideoReserved[3];

    // hard disk policies
    ULONG                   SpindownTimeout;

    // processor policies
    BOOLEAN                 OptimizeForPower;
    UCHAR                   FanThrottleTolerance;
    UCHAR                   ForcedThrottle;
    UCHAR                   MinThrottle;
    POWER_ACTION_POLICY     OverThrottled;

} SYSTEM_POWER_POLICY, *PSYSTEM_POWER_POLICY;


// processor power policy state

//
// Processor Idle State Policy.
//

#define PROCESSOR_IDLESTATE_POLICY_COUNT 0x3

typedef struct {
    ULONG TimeCheck;
    UCHAR DemotePercent;
    UCHAR PromotePercent;
    UCHAR Spare[2];
} PROCESSOR_IDLESTATE_INFO, *PPROCESSOR_IDLESTATE_INFO;

typedef struct {
    USHORT Revision;
    union {
        USHORT AsUSHORT;
        struct {
            USHORT AllowScaling : 1;
            USHORT Disabled : 1;
            USHORT Reserved : 14;
        } DUMMYSTRUCTNAME;
    } Flags;

    ULONG PolicyCount;
    PROCESSOR_IDLESTATE_INFO Policy[PROCESSOR_IDLESTATE_POLICY_COUNT];
} PROCESSOR_IDLESTATE_POLICY, *PPROCESSOR_IDLESTATE_POLICY;

//
// Legacy Processor Policy.  This is only provided to allow legacy
// applications to compile.  New applications must use
// PROCESSOR_IDLESTATE_POLICY.
//

#define PO_THROTTLE_NONE            0
#define PO_THROTTLE_CONSTANT        1
#define PO_THROTTLE_DEGRADE         2
#define PO_THROTTLE_ADAPTIVE        3
#define PO_THROTTLE_MAXIMUM         4   // not a policy, just a limit


typedef struct _PROCESSOR_POWER_POLICY_INFO {

    // Time based information (will be converted to kernel units)
    ULONG                   TimeCheck;                      // in US
    ULONG                   DemoteLimit;                    // in US
    ULONG                   PromoteLimit;                   // in US

    // Percentage based information
    UCHAR                   DemotePercent;
    UCHAR                   PromotePercent;
    UCHAR                   Spare[2];

    // Flags
    ULONG                   AllowDemotion:1;
    ULONG                   AllowPromotion:1;
    ULONG                   Reserved:30;

} PROCESSOR_POWER_POLICY_INFO, *PPROCESSOR_POWER_POLICY_INFO;

// processor power policy
typedef struct _PROCESSOR_POWER_POLICY {
    ULONG                       Revision;       // 1

    // Dynamic Throttling Policy
    UCHAR                       DynamicThrottle;
    UCHAR                       Spare[3];

    // Flags
    ULONG                       DisableCStates:1;
    ULONG                       Reserved:31;

    // System policy information
    // The Array is last, in case it needs to be grown and the structure
    // revision incremented.
    ULONG                       PolicyCount;
    PROCESSOR_POWER_POLICY_INFO Policy[3];

} PROCESSOR_POWER_POLICY, *PPROCESSOR_POWER_POLICY;

//
// Processor Perf State Policy.
//

typedef struct {
    ULONG Revision;
    UCHAR MaxThrottle;
    UCHAR MinThrottle;
    UCHAR BusyAdjThreshold;
    union {
        UCHAR Spare;
        union {
            UCHAR AsUCHAR;
            struct {
                UCHAR NoDomainAccounting : 1;
                UCHAR IncreasePolicy: 2;
                UCHAR DecreasePolicy: 2;
                UCHAR Reserved : 3;
            } DUMMYSTRUCTNAME;
        } Flags;
    } DUMMYUNIONNAME;
    
    ULONG TimeCheck;
    ULONG IncreaseTime;
    ULONG DecreaseTime;
    ULONG IncreasePercent;
    ULONG DecreasePercent;
} PROCESSOR_PERFSTATE_POLICY, *PPROCESSOR_PERFSTATE_POLICY;

// administrator power policy overrides
typedef struct _ADMINISTRATOR_POWER_POLICY {

    // meaning of power action "sleep"
    SYSTEM_POWER_STATE      MinSleep;
    SYSTEM_POWER_STATE      MaxSleep;

    // video policies
    ULONG                   MinVideoTimeout;
    ULONG                   MaxVideoTimeout;

    // disk policies
    ULONG                   MinSpindownTimeout;
    ULONG                   MaxSpindownTimeout;
} ADMINISTRATOR_POWER_POLICY, *PADMINISTRATOR_POWER_POLICY;

// end_winnt

__drv_maxIRQL(APC_LEVEL)
__kernel_entry NTSYSCALLAPI
NTSTATUS
NTAPI
NtSetThreadExecutionState(
    __in EXECUTION_STATE NewFlags,               // ES_xxx flags
    __out PEXECUTION_STATE PreviousFlags
    );

__drv_maxIRQL(APC_LEVEL)
__kernel_entry NTSYSCALLAPI
NTSTATUS
NTAPI
NtInitiatePowerAction(
    __in POWER_ACTION SystemAction,
    __in SYSTEM_POWER_STATE LightestSystemState,
    __in ULONG Flags,                 // POWER_ACTION_xxx flags
    __in BOOLEAN Asynchronous
    );

__drv_maxIRQL(APC_LEVEL)
__kernel_entry NTSYSCALLAPI
NTSTATUS
NTAPI
NtSetSystemPowerState (
    __in POWER_ACTION SystemAction,
    __in SYSTEM_POWER_STATE LightestSystemState,
    __in ULONG Flags                  // POWER_ACTION_xxx flags
    );

__drv_maxIRQL(APC_LEVEL)
__kernel_entry NTSYSCALLAPI
NTSTATUS
NTAPI
NtGetDevicePowerState(
    __in HANDLE Device,
    __out PDEVICE_POWER_STATE State
    );

__drv_maxIRQL(APC_LEVEL)
__kernel_entry NTSYSCALLAPI
BOOLEAN
NTAPI
NtIsSystemResumeAutomatic(
    VOID
    );

// WinLogonFlags:
#define WINLOGON_LOCK_ON_SLEEP  0x00000001

// begin_winnt

typedef struct {
    // Misc supported system features
    BOOLEAN             PowerButtonPresent;
    BOOLEAN             SleepButtonPresent;
    BOOLEAN             LidPresent;
    BOOLEAN             SystemS1;
    BOOLEAN             SystemS2;
    BOOLEAN             SystemS3;
    BOOLEAN             SystemS4;           // hibernate
    BOOLEAN             SystemS5;           // off
    BOOLEAN             HiberFilePresent;
    BOOLEAN             FullWake;
    BOOLEAN             VideoDimPresent;
    BOOLEAN             ApmPresent;
    BOOLEAN             UpsPresent;

    // Processors
    BOOLEAN             ThermalControl;
    BOOLEAN             ProcessorThrottle;
    UCHAR               ProcessorMinThrottle;

#if (NTDDI_VERSION < NTDDI_WINXP)
    UCHAR               ProcessorThrottleScale;
    UCHAR               spare2[4];
#else
    UCHAR               ProcessorMaxThrottle;
    BOOLEAN             FastSystemS4;
    UCHAR               spare2[3];
#endif // (NTDDI_VERSION < NTDDI_WINXP)

    // Disk
    BOOLEAN             DiskSpinDown;
    UCHAR               spare3[8];

    // System Battery
    BOOLEAN             SystemBatteriesPresent;
    BOOLEAN             BatteriesAreShortTerm;
    BATTERY_REPORTING_SCALE BatteryScale[3];

    // Wake
    SYSTEM_POWER_STATE  AcOnLineWake;
    SYSTEM_POWER_STATE  SoftLidWake;
    SYSTEM_POWER_STATE  RtcWake;
    SYSTEM_POWER_STATE  MinDeviceWakeState; // note this may change on driver load
    SYSTEM_POWER_STATE  DefaultLowLatencyWake;
} SYSTEM_POWER_CAPABILITIES, *PSYSTEM_POWER_CAPABILITIES;

typedef struct {
    BOOLEAN             AcOnLine;
    BOOLEAN             BatteryPresent;
    BOOLEAN             Charging;
    BOOLEAN             Discharging;
    BOOLEAN             Spare1[4];

    ULONG               MaxCapacity;
    ULONG               RemainingCapacity;
    ULONG               Rate;
    ULONG               EstimatedTime;

    ULONG               DefaultAlert1;
    ULONG               DefaultAlert2;
} SYSTEM_BATTERY_STATE, *PSYSTEM_BATTERY_STATE;

// end_winnt

//
// valid flags for SYSTEM_POWER_STATE_DISABLE_REASON.PowerReasonCode
//
#define SPSD_REASON_NONE                        0x00000000
#define SPSD_REASON_NOBIOSSUPPORT               0x00000001
#define SPSD_REASON_BIOSINCOMPATIBLE            0x00000002
#define SPSD_REASON_NOOSPM                      0x00000003
#define SPSD_REASON_LEGACYDRIVER                0x00000004
#define SPSD_REASON_HIBERSTACK                  0x00000005
#define SPSD_REASON_HIBERFILE                   0x00000006
#define SPSD_REASON_POINTERNAL                  0x00000007
#define SPSD_REASON_MEMORYLIMIT                 0x00000008
#define SPSD_REASON_MPOVERRIDE                  0x00000009
#define SPSD_REASON_DRIVERDOWNGRADE             0x0000000A
#define SPSD_REASON_PREVIOUSATTEMPTFAILED       0x0000000B
#define SPSD_REASON_UNKNOWN                     0x0000000C
#define SPSD_REASON_INTERNALLYDISABLED          0x0000000D
#define SPSD_REASON_DISABLEDBYPOLICY            0x0000000E
#define SPSD_REASON_UPGRADEINPROGRESS           0x0000000F

__struct_bcount(sizeof(SYSTEM_POWER_STATE_DISABLE_REASON) + PowerReasonLength)
typedef struct _SYSTEM_POWER_STATE_DISABLE_REASON {
    BOOLEAN AffectedState[POWER_STATE_HANDLER_TYPE_MAX];
    ULONG PowerReasonCode;
    ULONG PowerReasonLength;
    //UCHAR PowerReasonInfo[ANYSIZE_ARRAY];
} SYSTEM_POWER_STATE_DISABLE_REASON, *PSYSTEM_POWER_STATE_DISABLE_REASON;

//
// valid flags for SYSTEM_POWER_LOGGING_ENTRY.LoggingType
//
#define LOGGING_TYPE_SPSD                       0x00000001
#define LOGGING_TYPE_POWERTRANSITION            0x00000002

typedef struct _SYSTEM_POWER_LOGGING_ENTRY {
        ULONG LoggingType;
        PVOID LoggingEntry;
} SYSTEM_POWER_LOGGING_ENTRY, *PSYSTEM_POWER_LOGGING_ENTRY;

#if (NTDDI_VERSION < NTDDI_WINXP) // win2k only

//
// Power structure in each processors PRCB
//
struct _PROCESSOR_POWER_STATE;      // forward ref

__drv_functionClass(PROCESSOR_IDLE_FUNCTION)
typedef
VOID
(FASTCALL PROCESSOR_IDLE_FUNCTION) (
    __inout struct _PROCESSOR_POWER_STATE   *PState
    );

typedef PROCESSOR_IDLE_FUNCTION *PPROCESSOR_IDLE_FUNCTION;

typedef struct _PROCESSOR_POWER_STATE {
    PPROCESSOR_IDLE_FUNCTION    IdleFunction;
    ULONG                       Idle0KernelTimeLimit;
    ULONG                       Idle0LastTime;

    PVOID                       IdleState;
    ULONGLONG                   LastCheck;
    PROCESSOR_IDLE_TIMES        IdleTimes;

    ULONG                       IdleTime1;
    ULONG                       PromotionCheck;
    ULONG                       IdleTime2;

    UCHAR                       CurrentThrottle;    // current throttle setting
    UCHAR                       ThrottleLimit;      // max available throttle setting
    UCHAR                       Spare1[2];

    ULONG                       SetMember;
    PVOID                       AbortThrottle;

// temp for debugging
    ULONGLONG                   DebugDelta;
    ULONG                       DebugCount;

    ULONG                       LastSysTime;
    ULONG                       Spare2[10];

    
} PROCESSOR_POWER_STATE, *PPROCESSOR_POWER_STATE;

#endif // (NTDDI_VERSION < NTDDI_WINXP)

typedef struct _PROCESSOR_POWER_INFORMATION {
    ULONG                   Number;
    ULONG                   MaxMhz;
    ULONG                   CurrentMhz;
    ULONG                   MhzLimit;
    ULONG                   MaxIdleState;
    ULONG                   CurrentIdleState;
} PROCESSOR_POWER_INFORMATION, *PPROCESSOR_POWER_INFORMATION;

typedef struct _SYSTEM_POWER_INFORMATION {
    ULONG                   MaxIdlenessAllowed;
    ULONG                   Idleness;
    ULONG                   TimeRemaining;
    UCHAR                   CoolingMode;
} SYSTEM_POWER_INFORMATION, *PSYSTEM_POWER_INFORMATION;

// end_nthal

typedef struct _SYSTEM_HIBERFILE_INFORMATION {
    ULONG NumberOfMcbPairs;
    LARGE_INTEGER Mcb[1];
} SYSTEM_HIBERFILE_INFORMATION, *PSYSTEM_HIBERFILE_INFORMATION;

#if (NTDDI_VERSION >= NTDDI_WIN7)

typedef enum _REQUESTER_TYPE {
    KernelRequester,
    UserProcessRequester,
    UserSharedServiceRequester
} REQUESTER_TYPE, *PREQUESTER_TYPE;

typedef struct _REASON_BUFFER {
    ULONG Flags;
    union {
        struct {
            SIZE_T ResourceFileNameOffset;
            USHORT ResourceReasonId;
            ULONG StringCount;
            SIZE_T SubstitutionStringsOffset;
        } DUMMYSTRUCTNAME;

        SIZE_T SimpleStringOffset;
    } DUMMYUNIONNAME;
} REASON_BUFFER, *PREASON_BUFFER;

typedef struct _DIAGNOSTIC_BUFFER {
    SIZE_T Size;
    REQUESTER_TYPE CallerType;
    union {
        struct {
            SIZE_T ProcessImageNameOffset;
            ULONG ProcessId;
            ULONG ServiceTag;
        } DUMMYSTRUCTNAME;

        struct {
            SIZE_T DeviceDescriptionOffset;
            SIZE_T DevicePathOffset;
        } DUMMYSTRUCTNAME;
    } DUMMYUNIONNAME;

    SIZE_T ReasonOffset;
} DIAGNOSTIC_BUFFER, *PDIAGNOSTIC_BUFFER;

typedef struct _WAKE_TIMER_INFO {
    SIZE_T OffsetToNext;
    ULARGE_INTEGER DueTime;
    ULONG Period;
    DIAGNOSTIC_BUFFER ReasonContext;
} WAKE_TIMER_INFO, *PWAKE_TIMER_INFO;

#endif // (NTDDI_VERSION >= NTDDI_WIN7)

//
// Wake source tracking
//

typedef enum {
    DeviceWakeSourceType,
    FixedWakeSourceType,
    TimerWakeSourceType
} PO_WAKE_SOURCE_TYPE, *PPO_WAKE_SOURCE_TYPE;

typedef enum {
    FixedWakeSourcePowerButton,
    FixedWakeSourceSleepButton,
    FixedWakeSourceRtc,
    FixedWakeSourceDozeToHibernate
} PO_FIXED_WAKE_SOURCE_TYPE, *PPO_FIXED_WAKE_SOURCE_TYPE;

typedef struct _PO_WAKE_SOURCE_HEADER {
    PO_WAKE_SOURCE_TYPE Type;
    ULONG Size;
} PO_WAKE_SOURCE_HEADER, *PPO_WAKE_SOURCE_HEADER;

typedef struct _PO_WAKE_SOURCE_DEVICE {
    PO_WAKE_SOURCE_HEADER Header;
    WCHAR InstancePath[ANYSIZE_ARRAY];
} PO_WAKE_SOURCE_DEVICE, *PPO_WAKE_SOURCE_DEVICE;

typedef struct _PO_WAKE_SOURCE_FIXED {
    PO_WAKE_SOURCE_HEADER Header;
    PO_FIXED_WAKE_SOURCE_TYPE FixedWakeSourceType;
} PO_WAKE_SOURCE_FIXED, *PPO_WAKE_SOURCE_FIXED;

#if (NTDDI_VERSION >= NTDDI_WIN7)

typedef struct _PO_WAKE_SOURCE_TIMER {
    PO_WAKE_SOURCE_HEADER Header;
    DIAGNOSTIC_BUFFER Reason;
} PO_WAKE_SOURCE_TIMER, *PPO_WAKE_SOURCE_TIMER;

#endif // (NTDDI_VERSION >= NTDDI_WIN7)

typedef struct _PO_WAKE_SOURCE_INFO {
    ULONG Count;
    ULONG Offsets[ANYSIZE_ARRAY];
} PO_WAKE_SOURCE_INFO, *PPO_WAKE_SOURCE_INFO;

typedef struct _PO_WAKE_SOURCE_HISTORY {
    ULONG Count;
    ULONG Offsets[ANYSIZE_ARRAY];
} PO_WAKE_SOURCE_HISTORY, *PPO_WAKE_SOURCE_HISTORY;

#ifdef __cplusplus
}
#endif

#if _MSC_VER >= 1200
#pragma warning(pop)
#else
#pragma warning(default:4201)
#pragma warning(default:4214)
#endif

#endif // _NTPOAPI_

