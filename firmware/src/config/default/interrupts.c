/*******************************************************************************
 System Interrupts File

  Company:
    Microchip Technology Inc.

  File Name:
    interrupt.c

  Summary:
    Interrupt vectors mapping

  Description:
    This file maps all the interrupt vectors to their corresponding
    implementations. If a particular module interrupt is used, then its ISR
    definition can be found in corresponding PLIB source file. If a module
    interrupt is not used, then its ISR implementation is mapped to dummy
    handler.
 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2018 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *******************************************************************************/
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "interrupts.h"
#include "definitions.h"


// *****************************************************************************
// *****************************************************************************
// Section: System Interrupt Vector Functions
// *****************************************************************************
// *****************************************************************************


void EXTERNAL_3_InterruptHandler( void );
void DMA0_InterruptHandler( void );
void TIMER_8_InterruptHandler( void );
void TIMER_9_InterruptHandler( void );



/* All the handlers are defined here.  Each will call its PLIB-specific function. */


void EXTERNAL_3_Handler (void)
{
    EXTERNAL_3_InterruptHandler();
}

void UART4_FAULT_Handler (void)
{
    if (U4STAbits.OERR == 1) {
        U4STAbits.OERR = 0;
    }
    SAT1_LED_Set();  //TODO remove
    TMR8 = 0;
    IFS2bits.U4EIF = 0;
}

void UART4_RX_Handler (void)
{
    TMR8 = 0;
    uint8_t rx = U4RXREG;
    (void) rx;
    IFS2bits.U4RXIF = 0;
}

void DMA0_Handler (void)
{
    DMA0_InterruptHandler();
}

void TIMER_8_Handler (void)
{
    TIMER_8_InterruptHandler();
}

void TIMER_9_Handler (void)
{
    TIMER_9_InterruptHandler();
}




/*******************************************************************************
 End of File
*/
