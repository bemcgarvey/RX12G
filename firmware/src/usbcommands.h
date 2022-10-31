/////////////////////////////////////////////////////
// Project: RX12G                                  //
// File: usbcommands.h                             //
// Target: PIC32MK1024GPK064                       // 
// Compiler: XC32                                  //
// Author: Brad McGarvey                           //
// License: GNU General Public License v3.0        //
// Description: USB command defines                //
/////////////////////////////////////////////////////

#ifndef USBCOMMANDS_H
#define	USBCOMMANDS_H

#ifdef	__cplusplus
extern "C" {
#endif

    enum {
        GET_VERSION = 0xf0,
        GET_SETTINGS = 0x01,
        SAVE_SETTINGS = 0x02,
        GET_CHANNELS = 0x03,
        SET_PRESETS = 0x04,
        GET_SENSORS = 0x05,
        GET_GAINS = 0x06,
        ENABLE_OFFSETS = 0x07,
        DISABLE_OFFSETS = 0x08,
        GET_ATTITUDE = 0x09,
        CMD_ACK = 0x80,
        CMD_NACK = 0x7f,
        REBOOT = 0x88,
        BIND = 0x89,
        BOOTLOAD = 0x90
    };


#ifdef	__cplusplus
}
#endif

#endif	/* USBCOMMANDS_H */

