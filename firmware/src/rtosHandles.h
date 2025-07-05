/////////////////////////////////////////////////////
// Project: RX12G                                  //
// File: rtosHandles.h                             //
// Target: PIC32MK1024GPK064                       // 
// Compiler: XC32                                  //
// Author: Brad McGarvey                           //
// License: GNU General Public License v3.0        //
// Description: handles for rtos objects           //
/////////////////////////////////////////////////////

#ifndef RTOSHANDLES_H
#define	RTOSHANDLES_H

#ifdef	__cplusplus
extern "C" {
#endif
    //Task priorities
#define BUTTON_TASK_PRIORITY        4
#define RX_TASK_PRIORITY            3   
#define RX_ONLY_TASK_PRIORITY       2
#define GYRO_TASK_PRIORITY          2
#define IMU_TASK_PRIORITY           4
#define STATUS_LED_TASK_PRIORITY    1
#define USB_BIND_TASK_PRIORITY      1
#define DETECT_USB_TASK_PRIORITY    1
#define USB_DEVICE_TASK_PRIORITY    1
#define USB_APP_TASK_PRIORITY       1
#define CRSF_TELEMETRY_TASK_PRIORITY    1
    
    //Queues
    extern QueueHandle_t rxQueue;
    extern QueueHandle_t imuQueue;

    //Tasks
    extern TaskHandle_t buttonTaskHandle;
    extern TaskHandle_t rxTaskHandle;
    extern TaskHandle_t gyroTaskHandle;
    extern TaskHandle_t statusLedTaskHandle;
    extern TaskHandle_t usbAppTaskHandle;
    extern TaskHandle_t imuTaskHandle;
    extern TaskHandle_t rxOnlyTaskHandle;
    extern TaskHandle_t detectUSBTaskHandle;
    extern TaskHandle_t usbBindTaskHandle;
    extern TaskHandle_t crsfTelemetryTaskHandle;

#ifdef	__cplusplus
}
#endif

#endif	/* RTOSHANDLES_H */

