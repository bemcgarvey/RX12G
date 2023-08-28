/////////////////////////////////////////////////////
// Project: RX12G                                  //
// File: imu.h                                     //
// Target: PIC32MK1024GPK064                       // 
// Compiler: XC32                                  //
// Author: Brad McGarvey                           //
// License: GNU General Public License v3.0        //
// Description: imu functions                      //
/////////////////////////////////////////////////////

#ifndef IMU_H
#define	IMU_H

#ifdef	__cplusplus
extern "C" {
#endif

#define GYRO_ODR                    416     //Ouput rate in Hz
#define CALIBRATION_SAMPLE_COUNT    (2 * GYRO_ODR)     //2 seconds
#define GYRO_SAMPLE_PERIOD          (1.0 / GYRO_ODR)   //Period in sec.
#define MAX_MOTION_ALLOWED          75  //Maximum gyro count difference to qualify as not moving
#define UPRIGHT_MIN_VALUE           6000 //minimum raw accelerometer value to qualify as upright
    
    extern bool imuReady;
    extern bool imuInitialized;
    bool initIMU(void);
    void imuTask(void *pvParameters);
    void disableAccelOffsets(void);
    void enableAccelOffsets(void);

#ifdef	__cplusplus
}
#endif

#endif	/* IMU_H */

