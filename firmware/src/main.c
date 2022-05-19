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

void externalInt3Callback(EXTERNAL_INT_PIN pin, uintptr_t context);

int main ( void )
{
    /* Initialize all modules */
    SYS_Initialize ( NULL );
    EVIC_ExternalInterruptCallbackRegister(EXTERNAL_INT_3, &externalInt3Callback, 0);
    EVIC_ExternalInterruptEnable(EXTERNAL_INT_3);
    if (OSCCONbits.COSC == 0b001) {
        SAT1_LED_Set();
    }
    uint8_t reg = 0x0f;
    uint8_t data;
    CORETIMER_DelayMs(100);
    I2C2_WriteRead(0X6a, &reg, 1, &data, 1);
    while (I2C2_IsBusy());
    SAT2_LED_Set();
    if (data == 0x6c) {
        SAT3_LED_Set();
    }
    while ( true )
    {
        /* Maintain state machines of all polled MPLAB Harmony modules. */
        SYS_Tasks ( );
    }

    /* Execution should not come here during normal operation */

    return ( EXIT_FAILURE );
}

void externalInt3Callback(EXTERNAL_INT_PIN pin, uintptr_t context) {
    LED_B_Toggle();
}

/*******************************************************************************
 End of File
*/

