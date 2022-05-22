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


void TIMER_2_InterruptHandler( void );
void EXTERNAL_3_InterruptHandler( void );
void DMA0_InterruptHandler( void );
void DMA1_InterruptHandler( void );
void DMA2_InterruptHandler( void );
void TIMER_6_InterruptHandler( void );
void TIMER_7_InterruptHandler( void );
void TIMER_8_InterruptHandler( void );
void TIMER_9_InterruptHandler( void );



/* All the handlers are defined here.  Each will call its PLIB-specific function. */


void TIMER_2_Handler (void)
{
    TIMER_2_InterruptHandler();
}

void EXTERNAL_3_Handler (void)
{
    EXTERNAL_3_InterruptHandler();
}

void UART4_FAULT_Handler (void)
{
    if (U4STAbits.OERR == 1) {
        U4STAbits.OERR = 0;
    }
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

void UART5_FAULT_Handler (void)
{
    if (U5STAbits.OERR == 1) {
        U5STAbits.OERR = 0;
    }
    TMR7 = 0;
    IFS2bits.U5EIF = 0;
}

void UART5_RX_Handler (void)
{
    TMR7 = 0;
    uint8_t rx = U5RXREG;
    (void) rx;
    IFS2bits.U5RXIF = 0; 
}

void DMA0_Handler (void)
{
    DMA0_InterruptHandler();
}

void DMA1_Handler (void)
{
    DMA1_InterruptHandler();
}

void DMA2_Handler (void)
{
    DMA2_InterruptHandler();
}

void TIMER_6_Handler (void)
{
    TIMER_6_InterruptHandler();
}

void TIMER_7_Handler (void)
{
    TIMER_7_InterruptHandler();
}

void TIMER_8_Handler (void)
{
    TIMER_8_InterruptHandler();
}

void TIMER_9_Handler (void)
{
    TIMER_9_InterruptHandler();
}

void UART6_FAULT_Handler (void)
{
    if (U6STAbits.OERR == 1) {
        U6STAbits.OERR = 0;
    }
    TMR6 = 0;
    IFS5bits.U6EIF = 0;
}

void UART6_RX_Handler (void)
{
    TMR6 = 0;
    uint8_t rx = U6RXREG;
    (void) rx;
    IFS5bits.U6RXIF = 0;
}




/*******************************************************************************
 End of File
*/
