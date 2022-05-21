/*******************************************************************************
  System Interrupts File

  Company:
    Microchip Technology Inc.

  File Name:
    interrupt_a.S

  Summary:
    Raw ISR definitions.

  Description:
    This file maps all the interrupt vectors to their corresponding
    implementations. If a particular module interrupt is used, then its ISR
    definition can be found in corresponding PLIB source file. If a module
    interrupt is not used, then its ISR implementation is mapped to dummy
    handler.
 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2019 Microchip Technology Inc. and its subsidiaries.
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

#include <xc.h>
#include "ISR_Support.h"

    .extern  EXTERNAL_3_Handler

    .section   .vector_18,code, keep
    .equ     __vector_dispatch_18, IntVectorEXTERNAL_3_Handler
    .global  __vector_dispatch_18
    .set     nomicromips
    .set     noreorder
    .set     nomips16
    .set     noat
    .ent  IntVectorEXTERNAL_3_Handler

IntVectorEXTERNAL_3_Handler:
    portSAVE_CONTEXT
    la    s6,  EXTERNAL_3_Handler
    jalr  s6
    nop
    portRESTORE_CONTEXT
    .end   IntVectorEXTERNAL_3_Handler
    .extern  UART4_FAULT_Handler

    .section   .vector_65,code, keep
    .equ     __vector_dispatch_65, IntVectorUART4_FAULT_Handler
    .global  __vector_dispatch_65
    .set     nomicromips
    .set     noreorder
    .set     nomips16
    .set     noat
    .ent  IntVectorUART4_FAULT_Handler

IntVectorUART4_FAULT_Handler:
    portSAVE_CONTEXT
    la    s6,  UART4_FAULT_Handler
    jalr  s6
    nop
    portRESTORE_CONTEXT
    .end   IntVectorUART4_FAULT_Handler
    .extern  UART4_RX_Handler

    .section   .vector_66,code, keep
    .equ     __vector_dispatch_66, IntVectorUART4_RX_Handler
    .global  __vector_dispatch_66
    .set     nomicromips
    .set     noreorder
    .set     nomips16
    .set     noat
    .ent  IntVectorUART4_RX_Handler

IntVectorUART4_RX_Handler:
    portSAVE_CONTEXT
    la    s6,  UART4_RX_Handler
    jalr  s6
    nop
    portRESTORE_CONTEXT
    .end   IntVectorUART4_RX_Handler
    .extern  DMA0_Handler

    .section   .vector_72,code, keep
    .equ     __vector_dispatch_72, IntVectorDMA0_Handler
    .global  __vector_dispatch_72
    .set     nomicromips
    .set     noreorder
    .set     nomips16
    .set     noat
    .ent  IntVectorDMA0_Handler

IntVectorDMA0_Handler:
    portSAVE_CONTEXT
    la    s6,  DMA0_Handler
    jalr  s6
    nop
    portRESTORE_CONTEXT
    .end   IntVectorDMA0_Handler
    .extern  TIMER_8_Handler

    .section   .vector_84,code, keep
    .equ     __vector_dispatch_84, IntVectorTIMER_8_Handler
    .global  __vector_dispatch_84
    .set     nomicromips
    .set     noreorder
    .set     nomips16
    .set     noat
    .ent  IntVectorTIMER_8_Handler

IntVectorTIMER_8_Handler:
    portSAVE_CONTEXT
    la    s6,  TIMER_8_Handler
    jalr  s6
    nop
    portRESTORE_CONTEXT
    .end   IntVectorTIMER_8_Handler
    .extern  TIMER_9_Handler

    .section   .vector_88,code, keep
    .equ     __vector_dispatch_88, IntVectorTIMER_9_Handler
    .global  __vector_dispatch_88
    .set     nomicromips
    .set     noreorder
    .set     nomips16
    .set     noat
    .ent  IntVectorTIMER_9_Handler

IntVectorTIMER_9_Handler:
    portSAVE_CONTEXT
    la    s6,  TIMER_9_Handler
    jalr  s6
    nop
    portRESTORE_CONTEXT
    .end   IntVectorTIMER_9_Handler

