/////////////////////////////////////////////////////
// Project: RX12G                                  //
// File: version.c                                 //
// Target: PIC32MK1024GPK064                       // 
// Compiler: XC32                                  //
// Author: Brad McGarvey                           //
// License: GNU General Public License v3.0        //
// Description: firmware version                   //
/////////////////////////////////////////////////////

#include "version.h"


const unsigned int firmwareVersion = (MAJOR_VERSION << 8) | MINOR_VERSION;
