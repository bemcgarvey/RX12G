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

    extern int16_t imuData[6];
    extern bool imuReady;
    
    bool initIMU(void);
    void imuTask(void *pvParameters);

#ifdef	__cplusplus
}
#endif

#endif	/* IMU_H */

