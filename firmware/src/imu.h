/* 
 * File:   imu.h
 * Author: bemcg
 *
 * Created on May 29, 2022, 9:08 PM
 */

#ifndef IMU_H
#define	IMU_H

#ifdef	__cplusplus
extern "C" {
#endif

#define GYRO_ODR                    208     //Ouput rate in Hz
#define CALIBRATION_SAMPLE_COUNT    (3 * GYRO_ODR)     //about 3 seconds at current ODR
#define GYRO_SAMPLE_PERIOD          (1.0 / GYRO_ODR)   //Period in sec.
    
    extern bool imuReady;
    bool initIMU(void);
    void imuTask(void *pvParameters);
    void disableAccelOffsets(void);
    void enableAccelOffsets(void);

#ifdef	__cplusplus
}
#endif

#endif	/* IMU_H */

