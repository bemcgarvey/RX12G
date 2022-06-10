/* 
 * File:   attitude.h
 * Author: bemcg
 *
 * Created on June 6, 2022, 11:17 AM
 */

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
        int zSign; //sign of z axis acceleration: 1 = upright, -1 = inverted
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

