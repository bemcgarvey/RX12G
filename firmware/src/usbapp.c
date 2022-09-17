
#include "configuration.h"
#include "definitions.h"
#include "usb/usb_chapter_9.h"
#include "usb/usb_device.h"
#include "usbapp.h"
#include "usbcommands.h"
#include "version.h"
#include "settings.h"
#include "gyroTask.h"
#include "imu.h"
#include "output.h"
#include "attitude.h"

#define BTL_TRIGGER_RAM_START   KVA0_TO_KVA1(0x80000000)
#define BTL_PATTERN1    0x52583132
#define BTL_PATTERN2    0x32315852
static uint32_t *ramStart = (uint32_t *)BTL_TRIGGER_RAM_START;

typedef enum {
    APP_STATE_INIT,
    APP_STATE_WAIT_FOR_CONFIGURATION,
    APP_STATE_MAIN_TASK,
    APP_STATE_ERROR
} USBAppStates;

TaskHandle_t usbAppTaskHandle;

static uint8_t receiveDataBuffer[64] CACHE_ALIGN;
static uint8_t transmitDataBuffer[64] CACHE_ALIGN;
static USBAppStates state;
static USB_DEVICE_HANDLE usbDevHandle;
static bool deviceConfigured;
static USB_DEVICE_HID_TRANSFER_HANDLE txTransferHandle;
static USB_DEVICE_HID_TRANSFER_HANDLE rxTransferHandle;
static uint8_t configurationValue;
static bool hidDataReceived;
static bool hidDataTransmitted;
static uint8_t idleRate;

USB_DEVICE_HID_EVENT_RESPONSE APP_USBDeviceHIDEventHandler
(
        USB_DEVICE_HID_INDEX iHID,
        USB_DEVICE_HID_EVENT event,
        void * eventData,
        uintptr_t userData
        ) {
    USB_DEVICE_HID_EVENT_DATA_REPORT_SENT * reportSent;
    USB_DEVICE_HID_EVENT_DATA_REPORT_RECEIVED * reportReceived;

    switch (event) {
        case USB_DEVICE_HID_EVENT_REPORT_SENT:
            reportSent = (USB_DEVICE_HID_EVENT_DATA_REPORT_SENT *) eventData;
            if (reportSent->handle == txTransferHandle) {
                hidDataTransmitted = true;
            }
            break;
        case USB_DEVICE_HID_EVENT_REPORT_RECEIVED:
            reportReceived = (USB_DEVICE_HID_EVENT_DATA_REPORT_RECEIVED *) eventData;
            if (reportReceived->handle == rxTransferHandle) {
                hidDataReceived = true;
            }
            break;
        case USB_DEVICE_HID_EVENT_SET_IDLE:
            /* For now we just accept this request as is. We acknowledge
             * this request using the USB_DEVICE_HID_ControlStatus()
             * function with a USB_DEVICE_CONTROL_STATUS_OK flag */
            USB_DEVICE_ControlStatus(usbDevHandle, USB_DEVICE_CONTROL_STATUS_OK);
            /* Save Idle rate received from Host */
            idleRate = ((USB_DEVICE_HID_EVENT_DATA_SET_IDLE*) eventData)->duration;
            break;
        case USB_DEVICE_HID_EVENT_GET_IDLE:
            /* Host is requesting for Idle rate. Now send the Idle rate */
            USB_DEVICE_ControlSend(usbDevHandle, & (idleRate), 1);
            /* On successfully receiving Idle rate, the Host would acknowledge back with a
               Zero Length packet. The HID function driver returns an event
               USB_DEVICE_HID_EVENT_CONTROL_TRANSFER_DATA_SENT to the application upon
               receiving this Zero Length packet from Host.
               USB_DEVICE_HID_EVENT_CONTROL_TRANSFER_DATA_SENT event indicates this control transfer
               event is complete */
            break;
        default:
            // Nothing to do.
            break;
    }
    return USB_DEVICE_HID_EVENT_RESPONSE_NONE;
}

void APP_USBDeviceEventHandler(USB_DEVICE_EVENT event, void * eventData, uintptr_t context) {
    switch (event) {
        case USB_DEVICE_EVENT_RESET:
        case USB_DEVICE_EVENT_DECONFIGURED:
            /* Host has de configured the device or a bus reset has happened.
             * Device layer is going to de-initialize all function drivers.
             * Hence close handles to all function drivers (Only if they are
             * opened previously. */
            deviceConfigured = false;
            state = APP_STATE_WAIT_FOR_CONFIGURATION;
            break;

        case USB_DEVICE_EVENT_CONFIGURED:
            /* Set the flag indicating device is configured. */
            deviceConfigured = true;
            /* Save the other details for later use. */
            configurationValue = ((USB_DEVICE_EVENT_DATA_CONFIGURED*) eventData)->configurationValue;
            /* Register application HID event handler */
            USB_DEVICE_HID_EventHandlerSet(USB_DEVICE_HID_INDEX_0, APP_USBDeviceHIDEventHandler, (uintptr_t) 0);
            break;
        case USB_DEVICE_EVENT_SUSPENDED:
            break;
        case USB_DEVICE_EVENT_POWER_DETECTED:
            /* VBUS was detected. We can attach the device */
            USB_DEVICE_Attach(usbDevHandle);
            break;
        case USB_DEVICE_EVENT_POWER_REMOVED:
            /* VBUS is not available */
            USB_DEVICE_Detach(usbDevHandle);
            break;
            /* These events are not used*/
        case USB_DEVICE_EVENT_RESUMED:
        case USB_DEVICE_EVENT_ERROR:
        default:
            break;
    }
}

void USBAppInitialize(void) {
    state = APP_STATE_INIT;
    usbDevHandle = USB_DEVICE_HANDLE_INVALID;
    deviceConfigured = false;
    txTransferHandle = USB_DEVICE_HID_TRANSFER_HANDLE_INVALID;
    rxTransferHandle = USB_DEVICE_HID_TRANSFER_HANDLE_INVALID;
    hidDataReceived = false;
    hidDataTransmitted = true;
}

void USBAppTasks(void *pvParameters) {
    uint32_t crc;
    USBAppInitialize();
    while (1) {
        switch (state) {
            case APP_STATE_INIT:
                usbDevHandle = USB_DEVICE_Open(USB_DEVICE_INDEX_0, DRV_IO_INTENT_READWRITE);
                if (usbDevHandle != USB_DEVICE_HANDLE_INVALID) {
                    USB_DEVICE_EventHandlerSet(usbDevHandle, APP_USBDeviceEventHandler, 0);
                    state = APP_STATE_WAIT_FOR_CONFIGURATION;
                } else {
                    /* The Device Layer is not ready to be opened. We should try
                     * again later. */
                }
                break;
            case APP_STATE_WAIT_FOR_CONFIGURATION:
                if (deviceConfigured == true) {
                    hidDataReceived = false;
                    hidDataTransmitted = true;
                    state = APP_STATE_MAIN_TASK;
                    USB_DEVICE_HID_ReportReceive(USB_DEVICE_HID_INDEX_0,
                            &rxTransferHandle, receiveDataBuffer, 64);
                }
                break;
            case APP_STATE_MAIN_TASK:
                if (!deviceConfigured) {
                    state = APP_STATE_WAIT_FOR_CONFIGURATION;
                } else if (hidDataReceived) {
                    switch (receiveDataBuffer[0]) {
                        case GET_VERSION:
                            while (!hidDataTransmitted)
                                taskYIELD();
                            *(uint16_t *) transmitDataBuffer = firmwareVersion;
                            hidDataTransmitted = false;
                            USB_DEVICE_HID_ReportSend(USB_DEVICE_HID_INDEX_0,
                                    &txTransferHandle, transmitDataBuffer, 64);
                            break;
                        case GET_SETTINGS:
                        {
                            int bytesRemaining = sizeof (Settings);
                            uint8_t *p = (uint8_t *) & settings;
                            do {
                                while (!hidDataTransmitted)
                                    taskYIELD();
                                if (bytesRemaining >= 64) {
                                    memcpy(transmitDataBuffer, p, 64);
                                    p += 64;
                                    bytesRemaining -= 64;
                                } else {
                                    memcpy(transmitDataBuffer, p, bytesRemaining);
                                    bytesRemaining = 0;
                                }
                                hidDataTransmitted = false;
                                USB_DEVICE_HID_ReportSend(USB_DEVICE_HID_INDEX_0,
                                        &txTransferHandle, transmitDataBuffer, 64);
                            } while (bytesRemaining > 0);
                            while (!hidDataTransmitted)
                                taskYIELD();
                            crc = calculateCRC(&settings, sizeof (settings));
                            memcpy(transmitDataBuffer, &crc, sizeof (uint32_t));
                            hidDataTransmitted = false;
                            USB_DEVICE_HID_ReportSend(USB_DEVICE_HID_INDEX_0,
                                    &txTransferHandle, transmitDataBuffer, 64);
                        }
                            break;
                        case GET_CHANNELS:
                            while (!hidDataTransmitted)
                                taskYIELD();
                            memcpy(transmitDataBuffer, (void *) outputServos, sizeof (uint16_t) * MAX_CHANNELS);
                            hidDataTransmitted = false;
                            USB_DEVICE_HID_ReportSend(USB_DEVICE_HID_INDEX_0,
                                    &txTransferHandle, transmitDataBuffer, 64);
                            break;
                        case SAVE_SETTINGS:
                        {
                            int bytesRemaining = sizeof (Settings);
                            Settings temp;
                            uint8_t *p = (uint8_t *) & temp;
                            do {
                                hidDataReceived = false;
                                USB_DEVICE_HID_ReportReceive(USB_DEVICE_HID_INDEX_0,
                                        &rxTransferHandle, receiveDataBuffer, 64);
                                while (!hidDataReceived)
                                    taskYIELD();
                                if (bytesRemaining >= 64) {
                                    memcpy(p, receiveDataBuffer, 64);
                                    bytesRemaining -= 64;
                                    p += 64;
                                } else {
                                    memcpy(p, receiveDataBuffer, bytesRemaining);
                                    bytesRemaining = 0;
                                }
                            } while (bytesRemaining > 0);
                            hidDataReceived = false;
                            USB_DEVICE_HID_ReportReceive(USB_DEVICE_HID_INDEX_0,
                                    &rxTransferHandle, receiveDataBuffer, 64);
                            while (!hidDataReceived)
                                taskYIELD();
                            crc = calculateCRC(&temp, sizeof (Settings));
                            if (crc == *(uint32_t *) receiveDataBuffer) {
                                memcpy(&settings, &temp, sizeof (Settings));
                                while (!hidDataTransmitted)
                                    taskYIELD();
                                if (saveSettings()) {
                                    transmitDataBuffer[0] = CMD_ACK;
                                } else {
                                    transmitDataBuffer[0] = CMD_NACK;
                                }
                            } else {
                                transmitDataBuffer[0] = CMD_NACK;
                            }
                            hidDataTransmitted = false;
                            USB_DEVICE_HID_ReportSend(USB_DEVICE_HID_INDEX_0,
                                    &txTransferHandle, transmitDataBuffer, 64);
                        }
                            break;
                        case SET_PRESETS:
                            for (int i = 0; i < MAX_CHANNELS; ++i) {
                                channelPresets[i] = outputServos[i];
                            }
                            while (!hidDataTransmitted)
                                taskYIELD();
                            if (savePresets()) {
                                transmitDataBuffer[0] = CMD_ACK;
                            } else {
                                transmitDataBuffer[0] = CMD_NACK;
                            }
                            hidDataTransmitted = false;
                            USB_DEVICE_HID_ReportSend(USB_DEVICE_HID_INDEX_0,
                                    &txTransferHandle, transmitDataBuffer, 64);
                            break;
                        case GET_SENSORS:
                            while (!hidDataTransmitted)
                                taskYIELD();
                            memcpy(transmitDataBuffer, (void *) imuData, sizeof (uint16_t) * 6);
                            hidDataTransmitted = false;
                            USB_DEVICE_HID_ReportSend(USB_DEVICE_HID_INDEX_0,
                                    &txTransferHandle, transmitDataBuffer, 64);
                            break;
                        case REBOOT:
                            disableOutputs(DISABLE_KEY);
                            while (!outputsDisabled) {
                                taskYIELD();
                            }
                            SYS_RESET_SoftwareReset();
                            break;
                        case BOOTLOAD:
                            disableOutputs(DISABLE_KEY);
                            while (!outputsDisabled) {
                                taskYIELD();
                            }
                            ramStart[0] = BTL_PATTERN1;
                            ramStart[1] = BTL_PATTERN2;
                            SYS_RESET_SoftwareReset();
                            break;
                        case GET_GAINS:
                            while (!hidDataTransmitted)
                                taskYIELD();
                            float *fp = (float *) transmitDataBuffer;
                            for (int i = 0; i < 3; ++i) {
                                *fp = rollGains[i];
                                ++fp;
                                *fp = pitchGains[i];
                                ++fp;
                                *fp = yawGains[i];
                                ++fp;
                            }
                            hidDataTransmitted = false;
                            USB_DEVICE_HID_ReportSend(USB_DEVICE_HID_INDEX_0,
                                    &txTransferHandle, transmitDataBuffer, 64);
                            break;
                        case ENABLE_OFFSETS:
                            enableAccelOffsets();
                            break;
                        case DISABLE_OFFSETS:
                            disableAccelOffsets();
                            break;
                        case GET_ATTITUDE:
                            while (!hidDataTransmitted)
                                taskYIELD();
                            memcpy(transmitDataBuffer, (void *) &attitude, sizeof(attitude));
                            hidDataTransmitted = false;
                            USB_DEVICE_HID_ReportSend(USB_DEVICE_HID_INDEX_0,
                                    &txTransferHandle, transmitDataBuffer, 64);
                            break;
                    }
                    hidDataReceived = false;
                    USB_DEVICE_HID_ReportReceive(USB_DEVICE_HID_INDEX_0,
                            &rxTransferHandle, receiveDataBuffer, 64);
                }
            case APP_STATE_ERROR:
                break;
            default:
                break;
        }
        vTaskDelay(10);
    }
}

