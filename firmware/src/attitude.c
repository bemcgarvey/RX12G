#include "definitions.h"
#include "attitude.h"
#include "gyroTask.h"
#include "imu.h"
#include "fusion.h"

Vector AccelYPR(void);

AttitudeData attitude;
FusionAhrs ahrs;
FusionOffset offset;
FusionVector gyroscope;
FusionVector accelerometer;

void initAttitude(void) {
    Vector v = AccelYPR();
    v.yaw = 0;
    attitude.ypr = v;
    attitude.gyroRatesDeg.rollRate = 0;
    attitude.gyroRatesDeg.pitchRate = 0;
    attitude.gyroRatesDeg.yawRate = 0;
    attitude.zSign = 1;
    FusionOffsetInitialise(&offset, GYRO_ODR);
//    const FusionAhrsSettings settings = {
//            .gain = 0.5f,
//            .accelerationRejection = 0.0f,
//            .magneticRejection = 0.0f,
//            .rejectionTimeout = 5 * GYRO_ODR
//    };
//    FusionAhrsSetSettings(&ahrs, &settings);
//    FusionAhrsReset(&ahrs);
    FusionAhrsInitialise(&ahrs);
}

void updateAttitude(void) {
    CORETIMER_Start();
    attitude.gyroRatesDeg.rollRate = imuData[IMU_GYRO_X] * (70.0 / 1000.0);
    attitude.gyroRatesDeg.pitchRate = imuData[IMU_GYRO_Y] * (70.0 / 1000.0);
    attitude.gyroRatesDeg.yawRate = imuData[IMU_GYRO_Z] * (70.0 / 1000.0);
    gyroscope.axis.x = attitude.gyroRatesDeg.rollRate;
    gyroscope.axis.y = attitude.gyroRatesDeg.pitchRate;
    gyroscope.axis.z = attitude.gyroRatesDeg.yawRate;
    accelerometer.axis.x = imuData[IMU_ACCEL_X] * (0.122 / 1000.0);
    accelerometer.axis.y = imuData[IMU_ACCEL_Y] * (0.122 / 1000.0);
    accelerometer.axis.z = imuData[IMU_ACCEL_Z] * (0.122 / 1000.0);
    if (imuData[IMU_ACCEL_Z] < 0) {
        attitude.zSign = -1;
    } else {
        attitude.zSign = 1;
    }
    gyroscope = FusionOffsetUpdate(&offset, gyroscope);
    FusionAhrsUpdateNoMagnetometer(&ahrs, gyroscope, accelerometer, GYRO_SAMPLE_PERIOD);
    FusionEuler euler = FusionQuaternionToEuler(FusionAhrsGetQuaternion(&ahrs));
    attitude.ypr.roll = euler.angle.roll;
    attitude.ypr.pitch = euler.angle.pitch;
    attitude.ypr.yaw = 0;
    attitude.zSign = CORETIMER_CounterGet();
}

Vector AccelYPR(void) {
    Vector ypr;
    //TODO do we need yaw? or should it be 0
    ypr.yaw = atan2(imuData[IMU_ACCEL_Y], -imuData[IMU_ACCEL_X]);
    ypr.pitch = atan2(-imuData[IMU_ACCEL_X], sqrt(imuData[IMU_ACCEL_Y]
            * imuData[IMU_ACCEL_Y] + imuData[IMU_ACCEL_Z] * imuData[IMU_ACCEL_Z]));
    ypr.roll = atan2(imuData[IMU_ACCEL_Y], imuData[IMU_ACCEL_Z]);
    return ypr;
}
