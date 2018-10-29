Bluetooth Inquiry Record Verifier (sdpverify.exe)

The Bluetooth Inquiry Record Verifier displays a Bluetooth Device’s Inquiry Record as Microsoft Windows interprets it.  This record is displayed in a tree format, showing both the Service Discovery Protocol’s Record and any Extended Inquiry Response data obtained.

The Application has three menus: File, Radio and View.

File

Use the file menu to:
- Save the current Inquiry record by selecting "Export."
- Open a saved Inquiry record by selecting "Import."
- “Load a Profile Description.”  By default Microsoft supplies and loads profile descriptions for 12 common profiles.
- “Manage Profile Descriptions.”  Choose which profile descriptions to load.

Radio

Use the radio menu to:
- “Inquire and Select” will display all Discoverable and Paired Devices.  Select a device to view its Inquiry record.
- “Enter Address” will Inquiry the Bluetooth radio at the specified Bluetooth MAC address
- “Requery Current Radio” will refresh the SDP record currently displayed

View

- “Raw results…” will open a dialog displaying the results in HEX
- “Attribute Values as siblings” will display the Attribute Values in line with their Attribute IDs
- Jump from error to error using the “Previous Error” and “Next Error” options

This tool is not a replacement for any portion of the Bluetooth SIG’s Qualification process or for the Microsoft Windows Logo Program.  It is provided as is to troubleshoot interactions between a device and Microsoft Windows.

Known Issues on Windows 7
-On searching for remote devices, everything found is listed during the initial inquiry.  Once the inquiry is complete, only devices for which we have a friendly name persist.
-Extended Inquiry Response record parsing does not work
-LanguageBaseAttributeIDList may be marked incorrectly as invalid
