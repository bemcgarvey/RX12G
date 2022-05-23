
#include "definitions.h"
#include "gyroTask.h"
#include "rtosHandles.h"
#include "output.h"

GyroMode currentMode;
TaskHandle_t gyroTaskHandle;

static uint16_t servoPositions[MAX_CHANNELS];

void gyroTask(void *pvParameters) {
    currentMode = GYRO_MODE_OFF;  //TODO use channel data to set this or settings
    int updateCount = 0;
    while (1) {
        BaseType_t result;
        result = xQueueReceive(rawServoQueue, servoPositions, 10);
        if (result) {
            if (!outputsActive) {
                ++updateCount;
                if (updateCount > 2) {
                    enableActiveOutputs();
                }
            }
            if (currentMode == GYRO_MODE_OFF) {
                for (int i = 0; i < MAX_CHANNELS; ++i) {
                    outputServos[i] = servoPositions[i];
                }
            }
        }
        //TODO check stack level
        /*int stack = uxTaskGetStackHighWaterMark(NULL);
        if (stack < 1) {
            while (1);
        }*/
    }
}
