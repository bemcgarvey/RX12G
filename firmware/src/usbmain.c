#include "configuration.h"
#include "definitions.h"
#include "usbmain.h"
#include "usbapp.h"

void _USB_DEVICE_Tasks(void *pvParameters) {
    while (1) {
        /* USB Device layer tasks routine */
        USB_DEVICE_Tasks(sysObj.usbDevObject0);
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

TaskHandle_t xAPP_Tasks;

void _APP_Tasks(void *pvParameters) {
    while (1) {
        APP_Tasks();
        vTaskDelay(10);
    }
}

void USBMain(void) {
    xTaskCreate(_USB_DEVICE_Tasks,
            "USB_DEVICE_TASKS",
            1024,
            (void*) NULL,
            1,
            (TaskHandle_t*) NULL
            );

    xTaskCreate((TaskFunction_t) _APP_Tasks,
            "APP_Tasks",
            1024,
            NULL,
            1,
            &xAPP_Tasks);
    vTaskStartScheduler();
    while (1);
}
