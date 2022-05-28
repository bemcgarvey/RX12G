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
    APP_STATE_INIT,
    APP_STATE_WAIT_FOR_CONFIGURATION,
    APP_STATE_MAIN_TASK,
    APP_STATE_ERROR
} APP_STATES;


void APP_Initialize ( void );

void APP_Tasks ( void );

#ifdef	__cplusplus
}
#endif

#endif /* USBAPP_H */
/*******************************************************************************
 End of File
 */

