#include "definitions.h"
#include "attitude.h"
#include "gyroTask.h"
#include "imu.h"
#include "dcm2.h"

Vector AccelYPR(void);

AttitudeData attitude;

void initAttitude(void) {
    Vector v = AccelYPR();
    v.yaw = 0;
    attitude.ypr = v;
    attitude.gyroRatesDeg.rollRate = 0;
    attitude.gyroRatesDeg.pitchRate = 0;
    attitude.gyroRatesDeg.yawRate = 0;
    float state[6] = DEFAULT_state;
    initDCM_IMU_uC(state);
}

void updateAttitude(void) {
    attitude.gyroRatesDeg.rollRate = imuData[IMU_GYRO_X] * (70.0 / 1000.0);
    attitude.gyroRatesDeg.pitchRate = imuData[IMU_GYRO_Y] * (70.0 / 1000.0);
    attitude.gyroRatesDeg.yawRate = imuData[IMU_GYRO_Z] * (70.0 / 1000.0);
    float gyro[3];
    float accel[3];
    gyro[0] = attitude.gyroRatesDeg.rollRate * DEGREES_TO_RAD;
    gyro[1] = attitude.gyroRatesDeg.pitchRate * DEGREES_TO_RAD;
    gyro[2] = attitude.gyroRatesDeg.yawRate * DEGREES_TO_RAD;
    accel[0] = imuData[IMU_ACCEL_X] * (0.122 / 1000.0);
    accel[1] = imuData[IMU_ACCEL_Y] * (0.122 / 1000.0);
    accel[2]= imuData[IMU_ACCEL_Z] * (0.122 / 1000.0);
    updateIMU(gyro, accel, GYRO_SAMPLE_PERIOD);
    attitude.ypr.roll = getRoll() * RAD_TO_DEGREES;
    attitude.ypr.pitch = getPitch() * RAD_TO_DEGREES;
    attitude.ypr.yaw = getYaw() * RAD_TO_DEGREES;
}

Vector AccelYPR(void) {
    Vector ypr;
    ypr.yaw = atan2(imuData[IMU_ACCEL_Y], -imuData[IMU_ACCEL_X]);
    ypr.pitch = atan2(-imuData[IMU_ACCEL_X], sqrt(imuData[IMU_ACCEL_Y]
            * imuData[IMU_ACCEL_Y] + imuData[IMU_ACCEL_Z] * imuData[IMU_ACCEL_Z]));
    ypr.roll = atan2(imuData[IMU_ACCEL_Y], imuData[IMU_ACCEL_Z]);
    return ypr;
}
