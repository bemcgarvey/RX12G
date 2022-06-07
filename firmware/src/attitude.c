#include "definitions.h"
#include "attitude.h"
#include "gyroTask.h"
#include "imu.h"


Quaternion MulQuaternion(Quaternion lhs, Quaternion rhs);
Quaternion ScalerMulQuaternion(float a, Quaternion q);
Quaternion SumQuaternion(Quaternion lhs, Quaternion rhs);
Quaternion QuaternionInt(Vector v, float t);
Vector ScalerMulVector(float a, Vector v);
Vector VecSum(Vector lhs, Vector rhs);
Vector VecCrossProduct(Vector lhs, Vector rhs);
Vector RotateR(Vector v, Quaternion q);
Vector RotateL(Quaternion q, Vector v);
void NormalizeQuat(Quaternion *q);
Quaternion toQuaternion(Vector v);
Vector toYPR(Quaternion q);
float AccelPitch(void);
float AccelRoll(void);
float AccelYaw(void);
Vector AccelYPR(void);

AttitudeData attitude;
static Vector VERTICAL = {
    {0, 0, 1}};

void initAttitude(void) {
    Vector v = AccelYPR();
    v.yaw = 0;
    attitude.qAttitude = toQuaternion(v);
    attitude.ypr = toYPR(attitude.qAttitude);
    attitude.gyroRatesDeg.rollRate = 0;
    attitude.gyroRatesRads.rollRate = 0;
    attitude.gyroRatesDeg.pitchRate = 0;
    attitude.gyroRatesRads.pitchRate = 0;
    attitude.gyroRatesDeg.yawRate = 0;
    attitude.gyroRatesRads.yawRate = 0;
    attitude.zSign = 1;
}

void updateAttitude(void) {
    Vector correction_Body, correction_World;
    Vector Accel_Body, Accel_World;
    Quaternion incrementalRotation;
    Vector gyroVec;

    attitude.gyroRatesRads.rollRate = imuData[IMU_GYRO_X] * (70.0 / 1000.0 * DEGREES_TO_RAD);
    attitude.gyroRatesRads.pitchRate = imuData[IMU_GYRO_Y] * (70.0 / 1000.0 * DEGREES_TO_RAD);
    attitude.gyroRatesRads.yawRate = imuData[IMU_GYRO_Z] * (70.0 / 1000.0 * DEGREES_TO_RAD);
    attitude.gyroRatesDeg.rollRate = imuData[IMU_GYRO_X] * (70.0 / 1000.0);
    attitude.gyroRatesDeg.pitchRate = imuData[IMU_GYRO_Y] * (70.0 / 1000.0);
    attitude.gyroRatesDeg.yawRate = imuData[IMU_GYRO_Z] * (70.0 / 1000.0);
    Accel_Body.x = imuData[IMU_ACCEL_X] * (0.122 / 1000.0);
    Accel_Body.y = imuData[IMU_ACCEL_Y] * (0.122 / 1000.0);
    Accel_Body.z = imuData[IMU_ACCEL_Z] * (0.122 / 1000.0);
    if (imuData[IMU_ACCEL_Z] < 0) {
        attitude.zSign = -1;
    } else {
        attitude.zSign = 1;
    }
    Accel_World = RotateL(attitude.qAttitude, Accel_Body); // rotate accel from body frame to world frame
    correction_World = VecCrossProduct(Accel_World, VERTICAL); // cross product to determine error
    correction_Body = RotateR(correction_World, attitude.qAttitude); // rotate correction vector to body frame
    gyroVec = VecSum(attitude.gyroRatesRads, correction_Body); // add correction vector to gyro data
    incrementalRotation = QuaternionInt(gyroVec, GYRO_SAMPLE_PERIOD); // create incremental rotation quaternion
    attitude.qAttitude = MulQuaternion(incrementalRotation, attitude.qAttitude); // quaternion integration
    attitude.ypr = toYPR(attitude.qAttitude);
}


//TODO are these needed?
//float AccelPitch(void) {
//    return atan2(-imuData[IMU_ACCEL_X], sqrt(imuData[IMU_ACCEL_Y] 
//            * imuData[IMU_ACCEL_Y] + imuData[IMU_ACCEL_Z] * imuData[IMU_ACCEL_Z]));
//}
//
//float AccelRoll(void) {
//    return atan2(imuData[IMU_ACCEL_Y], imuData[IMU_ACCEL_Z]);
//}
//
//float AccelYaw(void) {
//    //Only works while nose up
//    return atan2(imuData[IMU_ACCEL_Y], -imuData[IMU_ACCEL_X]);
//}

Vector AccelYPR(void) {
    Vector ypr;
    ypr.yaw = atan2(imuData[IMU_ACCEL_Y], -imuData[IMU_ACCEL_X]);
    ypr.pitch = atan2(-imuData[IMU_ACCEL_X], sqrt(imuData[IMU_ACCEL_Y]
            * imuData[IMU_ACCEL_Y] + imuData[IMU_ACCEL_Z] * imuData[IMU_ACCEL_Z]));
    ypr.roll = atan2(imuData[IMU_ACCEL_Y], imuData[IMU_ACCEL_Z]);
    return ypr;
}

Quaternion MulQuaternion(Quaternion lhs, Quaternion rhs) {
    Quaternion prod;
    prod.x = lhs.w * rhs.x + lhs.z * rhs.y - lhs.y * rhs.z + lhs.x * rhs.w;
    prod.y = lhs.w * rhs.y + lhs.x * rhs.z + lhs.y * rhs.w - lhs.z * rhs.x;
    prod.z = lhs.y * rhs.x - lhs.x * rhs.y + lhs.w * rhs.z + lhs.z * rhs.w;
    prod.w = lhs.w * rhs.w - lhs.x * rhs.x - lhs.y * rhs.y - lhs.z * rhs.z;
    return prod;
}

Quaternion ScalerMulQuaternion(float a, Quaternion q) {
    q.w *= a;
    q.x *= a;
    q.y *= a;
    q.z *= a;
    return q;
}

Quaternion QuaternionInt(Vector v, float t) {
    float dt2;
    Quaternion a;
    dt2 = t * 0.5f;
    a.x = v.x * dt2;
    a.y = v.y * dt2;
    a.z = v.z * dt2;
    a.w = 1.0 - 0.5 * (a.x * a.x + a.y * a.y + a.z * a.z);
    return a;
}

Vector ScalerMulVector(float a, Vector v) {
    v.x *= a;
    v.y *= a;
    v.z *= a;
    return v;
}

Vector VecSum(Vector lhs, Vector rhs) {
    lhs.x += rhs.x;
    lhs.y += rhs.y;
    lhs.z += rhs.z;
    return lhs;
}

Vector VecCrossProduct(Vector lhs, Vector rhs) {
    Vector cross;
    cross.x = lhs.y * rhs.z - lhs.z * rhs.y;
    cross.y = lhs.z * rhs.x - lhs.x * rhs.z;
    cross.z = lhs.x * rhs.y - lhs.y * rhs.x;
    return cross;
}

Vector RotateR(Vector v, Quaternion q) {
    Vector r = {
        {-q.x, -q.y, -q.z}};
    return VecSum(v, VecCrossProduct(VecSum(r, r), VecSum(VecCrossProduct(r, v), ScalerMulVector(q.w, v))));

}

Vector RotateL(Quaternion q, Vector v) {
    Vector r = {
        {q.x, q.y, q.z}};
    return VecSum(v, VecCrossProduct(VecSum(r, r), VecSum(VecCrossProduct(r, v), ScalerMulVector(q.w, v))));
}

void NormalizeQuat(Quaternion *q) {
    float d = 1.0 / sqrt(q->w * q->w + q->x * q->x + q->y * q->y + q->z * q->z);
    q->w *= d;
    q->x *= d;
    q->y *= d;
    q->z *= d;
}

Quaternion toQuaternion(Vector v) {
    Quaternion q;
    float cy = cos(v.z * 0.5);
    float sy = sin(v.z * 0.5);
    float cr = cos(v.x * 0.5);
    float sr = sin(v.x * 0.5);
    float cp = cos(v.y * 0.5);
    float sp = sin(v.y * 0.5);

    q.w = cy * cr * cp + sy * sr * sp;
    q.x = cy * sr * cp - sy * cr * sp;
    q.y = cy * cr * sp + sy * sr * cp;
    q.z = sy * cr * cp - cy * sr * sp;
    return q;
}

Vector toYPR(Quaternion q) {
    Vector ypr;
    ypr.roll = atan2(q.y * q.z + q.w * q.x, 0.5f - (q.x * q.x + q.y * q.y));
    ypr.roll *= RAD_TO_DEGREES;
    ypr.pitch = asin(-2.0f * (q.x * q.z - q.w * q.y));
    ypr.pitch *= RAD_TO_DEGREES;
    ypr.yaw = 0.0;
    return ypr;
}
