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

    .extern  TIMER_2_Handler

    .section   .vector_9,code, keep
    .equ     __vector_dispatch_9, IntVectorTIMER_2_Handler
    .global  __vector_dispatch_9
    .set     nomicromips
    .set     noreorder
    .set     nomips16
    .set     noat
    .ent  IntVectorTIMER_2_Handler

IntVectorTIMER_2_Handler:
    portSAVE_CONTEXT
    la    s6,  TIMER_2_Handler
    jalr  s6
    nop
    portRESTORE_CONTEXT
    .end   IntVectorTIMER_2_Handler
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
    .extern  TIMER_5_Handler

    .section   .vector_24,code, keep
    .equ     __vector_dispatch_24, IntVectorTIMER_5_Handler
    .global  __vector_dispatch_24
    .set     nomicromips
    .set     noreorder
    .set     nomips16
    .set     noat
    .ent  IntVectorTIMER_5_Handler

IntVectorTIMER_5_Handler:
    portSAVE_CONTEXT
    la    s6,  TIMER_5_Handler
    jalr  s6
    nop
    portRESTORE_CONTEXT
    .end   IntVectorTIMER_5_Handler
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
    .extern  UART5_FAULT_Handler

    .section   .vector_68,code, keep
    .equ     __vector_dispatch_68, IntVectorUART5_FAULT_Handler
    .global  __vector_dispatch_68
    .set     nomicromips
    .set     noreorder
    .set     nomips16
    .set     noat
    .ent  IntVectorUART5_FAULT_Handler

IntVectorUART5_FAULT_Handler:
    portSAVE_CONTEXT
    la    s6,  UART5_FAULT_Handler
    jalr  s6
    nop
    portRESTORE_CONTEXT
    .end   IntVectorUART5_FAULT_Handler
    .extern  UART5_RX_Handler

    .section   .vector_69,code, keep
    .equ     __vector_dispatch_69, IntVectorUART5_RX_Handler
    .global  __vector_dispatch_69
    .set     nomicromips
    .set     noreorder
    .set     nomips16
    .set     noat
    .ent  IntVectorUART5_RX_Handler

IntVectorUART5_RX_Handler:
    portSAVE_CONTEXT
    la    s6,  UART5_RX_Handler
    jalr  s6
    nop
    portRESTORE_CONTEXT
    .end   IntVectorUART5_RX_Handler
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
    .extern  DMA1_Handler

    .section   .vector_73,code, keep
    .equ     __vector_dispatch_73, IntVectorDMA1_Handler
    .global  __vector_dispatch_73
    .set     nomicromips
    .set     noreorder
    .set     nomips16
    .set     noat
    .ent  IntVectorDMA1_Handler

IntVectorDMA1_Handler:
    portSAVE_CONTEXT
    la    s6,  DMA1_Handler
    jalr  s6
    nop
    portRESTORE_CONTEXT
    .end   IntVectorDMA1_Handler
    .extern  DMA2_Handler

    .section   .vector_74,code, keep
    .equ     __vector_dispatch_74, IntVectorDMA2_Handler
    .global  __vector_dispatch_74
    .set     nomicromips
    .set     noreorder
    .set     nomips16
    .set     noat
    .ent  IntVectorDMA2_Handler

IntVectorDMA2_Handler:
    portSAVE_CONTEXT
    la    s6,  DMA2_Handler
    jalr  s6
    nop
    portRESTORE_CONTEXT
    .end   IntVectorDMA2_Handler
    .extern  TIMER_6_Handler

    .section   .vector_76,code, keep
    .equ     __vector_dispatch_76, IntVectorTIMER_6_Handler
    .global  __vector_dispatch_76
    .set     nomicromips
    .set     noreorder
    .set     nomips16
    .set     noat
    .ent  IntVectorTIMER_6_Handler

IntVectorTIMER_6_Handler:
    portSAVE_CONTEXT
    la    s6,  TIMER_6_Handler
    jalr  s6
    nop
    portRESTORE_CONTEXT
    .end   IntVectorTIMER_6_Handler
    .extern  TIMER_7_Handler

    .section   .vector_80,code, keep
    .equ     __vector_dispatch_80, IntVectorTIMER_7_Handler
    .global  __vector_dispatch_80
    .set     nomicromips
    .set     noreorder
    .set     nomips16
    .set     noat
    .ent  IntVectorTIMER_7_Handler

IntVectorTIMER_7_Handler:
    portSAVE_CONTEXT
    la    s6,  TIMER_7_Handler
    jalr  s6
    nop
    portRESTORE_CONTEXT
    .end   IntVectorTIMER_7_Handler
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
    .extern  UART6_FAULT_Handler

    .section   .vector_164,code, keep
    .equ     __vector_dispatch_164, IntVectorUART6_FAULT_Handler
    .global  __vector_dispatch_164
    .set     nomicromips
    .set     noreorder
    .set     nomips16
    .set     noat
    .ent  IntVectorUART6_FAULT_Handler

IntVectorUART6_FAULT_Handler:
    portSAVE_CONTEXT
    la    s6,  UART6_FAULT_Handler
    jalr  s6
    nop
    portRESTORE_CONTEXT
    .end   IntVectorUART6_FAULT_Handler
    .extern  UART6_RX_Handler

    .section   .vector_165,code, keep
    .equ     __vector_dispatch_165, IntVectorUART6_RX_Handler
    .global  __vector_dispatch_165
    .set     nomicromips
    .set     noreorder
    .set     nomips16
    .set     noat
    .ent  IntVectorUART6_RX_Handler

IntVectorUART6_RX_Handler:
    portSAVE_CONTEXT
    la    s6,  UART6_RX_Handler
    jalr  s6
    nop
    portRESTORE_CONTEXT
    .end   IntVectorUART6_RX_Handler

