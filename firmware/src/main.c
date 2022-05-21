/////////////////////////////////////////////////////
// Project: RX12G                                  //
// File: main.c                                    //
// Target: PIC32MK1024GPK064                       // 
// Compiler: XC32                                  //
// Author: Brad McGarvey                           //
// License: GNU General Public License v3.0        //
// Description: main                               //
/////////////////////////////////////////////////////

#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include "definitions.h"                // SYS function prototypes
#include "timers.h"
#include "satellites.h"

int main ( void )
{
    /* Initialize all modules */
    SYS_Initialize ( NULL );
    startSystemTime();
    initSatellites();
    while ( true )
    {
        if (getSystemTime() % 1000 == 0) {
            LED_A_Set();
        } 
        else if (getSystemTime() % 500 == 0) {
            LED_A_Clear();
        }
        if (BIND_BUTTON_Get() == 0) {
            CORETIMER_DelayMs(2000);
            if (BIND_BUTTON_Get() == 0) {
                LED_B_Toggle();
                bindSats();
            }
        }
        SYS_Tasks ( );
    }

    /* Execution should not come here during normal operation */

    return ( EXIT_FAILURE );
}

/*******************************************************************************
 End of File
*/

