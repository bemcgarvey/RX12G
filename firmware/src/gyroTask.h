
#ifndef GYROTASK_H
#define	GYROTASK_H

#ifdef	__cplusplus
extern "C" {
#endif

    typedef enum {
        GYRO_MODE_OFF = 0, GYRO_MODE_NORMAL, GYRO_MODE_LEVEL,
        GYRO_MODE_ATTITUDE_LOCK, GYRO_MODE_LAUNCH_ASSIST
    } GyroMode;
    
    void gyroTask(void *pvParameters);

#ifdef	__cplusplus
}
#endif

#endif	/* GYROTASK_H */

