/*******************************************************************************
  Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This file contains the "main" function for a project.

  Description:
    This file contains the "main" function for a project.  The
    "main" function calls the "SYS_Initialize" function to initialize the state
    machines of all modules in the system
 *******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include "definitions.h"                // SYS function prototypes


// *****************************************************************************
// *****************************************************************************
// Section: Main Entry Point
// *****************************************************************************
// *****************************************************************************

#define BTL_TRIGGER_RAM_START   KVA0_TO_KVA1(0x80000000)
#define BTL_PATTERN1    0x52583132
#define BTL_PATTERN2    0x32315852

bool bootloader_Trigger(void)
{
    if (BUTTON_Get() == 0) {
        CORETIMER_DelayMs(2000);
        if (BUTTON_Get() == 0) {
            return true;
        }
    }
    uint32_t *ramStart = (uint32_t *)BTL_TRIGGER_RAM_START;
    if (ramStart[0] == BTL_PATTERN1 && ramStart[1] == BTL_PATTERN2) {
        ramStart[0] = 0;
        ramStart[1] = 0;
        return true;
    }
    return false;
}


int main ( void )
{
    /* Initialize all modules */
    SYS_Initialize ( NULL );

    while ( true )
    {
        /* Maintain state machines of all polled MPLAB Harmony modules. */
        SYS_Tasks ( );
    }

    /* Execution should not come here during normal operation */

    return ( EXIT_FAILURE );
}


/*******************************************************************************
 End of File
*/

