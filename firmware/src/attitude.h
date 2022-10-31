/////////////////////////////////////////////////////
// Project: RX12G                                  //
// File: attitude.h                                //
// Target: PIC32MK1024GPK064                       // 
// Compiler: XC32                                  //
// Author: Brad McGarvey                           //
// License: GNU General Public License v3.0        //
// Description: attitude functions                 //
/////////////////////////////////////////////////////

#ifndef ATTITUDE_H
#define	ATTITUDE_H

#include <math.h>

#ifdef	__cplusplus
extern "C" {
#endif

    typedef union {

        struct {
            float x;
            float y;
            float z;
        };

        struct {
            float roll;
            float pitch;
            float yaw;
        };

        struct {
            float rollRate;
            float pitchRate;
            float yawRate;
        };
    } Vector;

    typedef struct {
        Vector ypr;
        Vector gyroRatesDeg;
    } AttitudeData;

    extern AttitudeData attitude;

#define RAD_TO_DEGREES      (180.0 / M_PI)
#define DEGREES_TO_RAD      (M_PI / 180.0)

    void initAttitude(void);
    void updateAttitude(void);

#ifdef	__cplusplus
}
#endif

#endif	/* ATTITUDE_H */

