
#include "definitions.h"
#include "gyroTask.h"
#include "rtosHandles.h"
#include "output.h"

GyroMode currentMode;
TaskHandle_t gyroTaskHandle;

volatile uint16_t rawServoPositions[MAX_CHANNELS];

void gyroTask(void *pvParameters) {
    currentMode = GYRO_MODE_OFF; //TODO use channel data to set this or settings
    while (1) {
        if (currentMode == GYRO_MODE_OFF) {
            for (int i = 0; i < MAX_CHANNELS; ++i) {
                outputServos[i] = rawServoPositions[i];
            }
        }
        vTaskDelay(5);
        //TODO check stack level
        /*int stack = uxTaskGetStackHighWaterMark(NULL);
        if (stack < 1) {
            while (1);
        }*/
    }
}
