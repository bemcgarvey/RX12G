
#ifndef GYROTASK_H
#define	GYROTASK_H

#include "output.h"

#ifdef	__cplusplus
extern "C" {
#endif
    
    typedef enum {
        GYRO_MODE_OFF = 0, GYRO_MODE_NORMAL, GYRO_MODE_LEVEL,
        GYRO_MODE_ATTITUDE_LOCK, GYRO_MODE_LAUNCH_ASSIST
    } GyroMode;
    
    extern volatile uint16_t rawServoPositions[MAX_CHANNELS];
    
    void gyroTask(void *pvParameters);

#ifdef	__cplusplus
}
#endif

#endif	/* GYROTASK_H */

