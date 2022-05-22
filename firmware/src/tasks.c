
#include "definitions.h"
#include "tasks.h"
#include "buttonTask.h"
#include "rtosHandles.h"

void initTasks(void) {
    xTaskCreate(buttonTask, "button", 128, NULL, 1, &buttonTaskHandle);
    
    vTaskStartScheduler();
}

