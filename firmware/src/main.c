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
#include "output.h"
#include "tasks.h"

int main ( void )
{
    /* Initialize all modules */
    SYS_Initialize ( NULL );
    startSystemTime();
    initSatellites();
    initOutputs();
    enableActiveOutputs();
    
    initTasks();  //Starts rtos tasks - Does not return
    while ( true )
    {
        /* Execution should not come here during normal operation */
    }
    return ( EXIT_FAILURE );
}

/*******************************************************************************
 End of File
*/

