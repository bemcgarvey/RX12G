#ifndef USBAPP_H
#define USBAPP_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "configuration.h"
#include "definitions.h"
#include "usb/usb_chapter_9.h"
#include "usb/usb_device.h"

#ifdef	__cplusplus
extern "C" {
#endif

typedef enum
{
    /* Application is initializing */
    APP_STATE_INIT,

    /* Application is waiting for configuration */
    APP_STATE_WAIT_FOR_CONFIGURATION,

    /* Application is running the main tasks */
    APP_STATE_MAIN_TASK,

    /* Application is in an error state */
    APP_STATE_ERROR
            
} APP_STATES;


typedef struct
{
    /* The application's current state */
    APP_STATES state;

      /* Device layer handle returned by device layer open function */
    USB_DEVICE_HANDLE  usbDevHandle;

    /* Device configured */
    bool deviceConfigured;

    /* Send report transfer handle*/
    USB_DEVICE_HID_TRANSFER_HANDLE txTransferHandle;

    /* Receive report transfer handle */
    USB_DEVICE_HID_TRANSFER_HANDLE rxTransferHandle;

    /* Configuration value selected by the host*/
    uint8_t configurationValue;

    /* HID data received flag*/
    bool hidDataReceived;

    /* HID data transmitted flag */
    bool hidDataTransmitted;

     /* USB HID current Idle */
    uint8_t idleRate;

} APP_DATA;


void APP_Initialize ( void );

void APP_Tasks ( void );



extern const USB_DEVICE_FUNCTION_REGISTRATION_TABLE funcRegistrationTable[1];
extern const USB_DEVICE_MASTER_DESCRIPTOR usbMasterDescriptor;

#ifdef	__cplusplus
}
#endif

#endif /* USBAPP_H */
/*******************************************************************************
 End of File
 */

