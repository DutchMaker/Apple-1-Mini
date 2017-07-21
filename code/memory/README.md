`8k-Apple-1-EEPROM.hex` contains the memory data for the 8-Kbit EEPROM.  
The ROM contains only the BASIC, Krusader and WozMon code.  
Address decoding makes sure the correct location of ROM memory is being accessed.  
For example: when the 6502 addresses FF00 to run the WozMon, it actually addresses 1F00 on the ROM.