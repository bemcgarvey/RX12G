#include "definitions.h"
#include "attitude.h"
#include "gyroTask.h"
#include "imu.h"
#include "dcm.h"

Vector AccelYPR(void);

AttitudeData attitude;

void initAttitude(void) {
    Vector v = AccelYPR();
    v.yaw = 0;
    attitude.ypr.pitch = v.pitch * RAD_TO_DEGREES;
    attitude.ypr.roll = v.roll * RAD_TO_DEGREES;
    attitude.ypr.yaw = 0;
    attitude.gyroRatesDeg.rollRate = 0;
    attitude.gyroRatesDeg.pitchRate = 0;
    attitude.gyroRatesDeg.yawRate = 0;
    dcmInit(v.pitch, v.roll, v.yaw);
}

void updateAttitude(void) {
    attitude.gyroRatesDeg.rollRate = imuData[IMU_GYRO_X] * (70.0 / 1000.0);
    attitude.gyroRatesDeg.pitchRate = imuData[IMU_GYRO_Y] * (70.0 / 1000.0);
    attitude.gyroRatesDeg.yawRate = imuData[IMU_GYRO_Z] * (70.0 / 1000.0);
    
    float accelX = imuData[IMU_ACCEL_X] * (0.122 / 1000.0);
    float accelY = imuData[IMU_ACCEL_Y] * (0.122 / 1000.0);
    float accelZ = imuData[IMU_ACCEL_Z] * (0.122 / 1000.0);
    dcmUpdate(GYRO_SAMPLE_PERIOD, attitude.gyroRatesDeg.rollRate, attitude.gyroRatesDeg.pitchRate,
            attitude.gyroRatesDeg.yawRate, accelX, accelY, accelZ);
    attitude.ypr.roll = dcmGetRoll();
    attitude.ypr.pitch = dcmGetPitch();
    attitude.ypr.yaw = dcmGetYaw();
}

Vector AccelYPR(void) {
    Vector ypr;
    ypr.yaw = atan2(imuData[IMU_ACCEL_Y], -imuData[IMU_ACCEL_X]);
    ypr.pitch = atan2(-imuData[IMU_ACCEL_X], sqrt(imuData[IMU_ACCEL_Y]
            * imuData[IMU_ACCEL_Y] + imuData[IMU_ACCEL_Z] * imuData[IMU_ACCEL_Z]));
    ypr.roll = atan2(imuData[IMU_ACCEL_Y], imuData[IMU_ACCEL_Z]);
    return ypr;
}
