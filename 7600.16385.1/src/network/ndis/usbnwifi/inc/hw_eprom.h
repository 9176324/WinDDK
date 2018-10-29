#ifndef __INC_EEPROM_H
#define __INC_EEPROM_H

#define EEPROM_MAX_SIZE         256
#define CLOCK_RATE                  50          //100us     

//- EEPROM opcodes
#define EEPROM_READ_OPCODE      06
#define EEPROM_WRITE_OPCODE     05
#define EEPROM_ERASE_OPCODE     07
#define EEPROM_EWEN_OPCODE      19      // Erase/write enable
#define EEPROM_EWDS_OPCODE      16      // Erase/write disable
typedef struct _EEPROM_OFFSET{
    USHORT CmdRegister;
    USHORT BIT_EEDO;
    USHORT BIT_EEDI;
    USHORT BIT_EESK;
    USHORT BIT_EECS;
    USHORT BIT_EEM0;
    USHORT BIT_EEM1;
}EEPROM_OFFSET,*PEEPROM_OFFSET;

extern EEPROM_OFFSET EEPROMOffset8187;

#define CSR_EEPROM_CONTROL_REG  (EEPROMOffset8187.CmdRegister)
#define EEDO                            (EEPROMOffset8187.BIT_EEDO)  // EEPROM data out
#define EEDI                            (EEPROMOffset8187.BIT_EEDI)      // EEPROM data in (set for writing data)
#define EECS                            (EEPROMOffset8187.BIT_EECS)  // EEPROM chip select (1=high, 0=low)
#define EESK                            (EEPROMOffset8187.BIT_EESK)  // EEPROM shift clock (1=high, 0=low)
#define EEM0                            (EEPROMOffset8187.BIT_EEM0)
#define EEM1                            (EEPROMOffset8187.BIT_EEM1)


USHORT
GetEEpromAddressSize(
    USHORT  Size
    );

USHORT
GetEEpromSize(
    PNIC    Nic
    );

USHORT 
ReadEEprom(
    PNIC    Nic,
    USHORT      Reg
    );

VOID 
ShiftOutBits(
    PNIC    Nic,
    USHORT      data,
    USHORT      count
    );

USHORT 
ShiftInBits(
    PNIC    Nic
    );

VOID 
RaiseClock(
    PNIC    Nic,
    USHORT      *x
    );

VOID 
LowerClock(
    PNIC    Nic,
    USHORT      *x
    );

VOID 
EEpromCleanup(
    PNIC    Nic
    );

VOID
UpdateChecksum(
    PNIC    Nic
    );

VOID
WriteEEprom(
    PNIC    Nic,
    USHORT      reg,
    USHORT      data
    );

USHORT
WaitEEPROMCmdDone(
    PNIC    Nic
    );

VOID
StandBy(
    PNIC    Nic
    );

#endif // #ifndef __INC_EEPROM_H

