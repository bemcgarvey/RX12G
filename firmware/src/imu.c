
#include "definitions.h"
#include "imu.h"
#include "settings.h"
#include "attitude.h"
#include "tasks.h"

#define IMU_DEVICE_ADDRESS  0x6a

//Registers
#define PIN_CTRL            0x02
#define FIFO_CTRL4          0x0a
#define COUNTER_BDR_REG1    0x0b
#define INT1_CTRL           0x0d
#define INT2CTRL            0x0e
#define WHO_AM_I            0x0f
#define CTRL1_XL            0x10
#define CTRL2_G             0x11
#define CTRL3_C             0x12
#define CTRL4_C             0x13
#define CTRL5_C             0x14
#define CTRL6_C             0x15
#define CTRL7_G             0x16
#define CTRL8_XL            0x17
#define CTRL9_XL            0x18
#define CTRL10_C            0x19
#define ALL_INT_SRC         0x1a
#define STATUS_REG          0x1e
#define OUTX_L_G            0x22
#define OUTX_H_G            0x23
#define OUTY_L_G            0x24
#define OUTY_H_G            0x25
#define OUTZ_L_G            0x26
#define OUTZ_H_G            0x27
#define OUTX_L_A            0x28
#define OUTX_H_A            0x29
#define OUTY_L_A            0x2a
#define OUTY_H_A            0x2b
#define OUTZ_L_A            0x2c
#define OUTZ_H_A            0x2d
#define INTERNAL_FREQ_FINE  0x63
#define X_OFS_USR           0x73
#define Y_OFS_USR           0x74
#define Z_OFS_USR           0x75

#define SW_RESET            0x01             

TaskHandle_t imuTaskHandle;
QueueHandle_t imuQueue;

bool imuReady = false;
bool imuInitialized = false;

int16_t rawImuData[6];

void imuIntHandler(EXTERNAL_INT_PIN pin, uintptr_t context);
bool isUpright(void);

bool initIMU(void) {
    uint8_t wValue[4];
    uint8_t rValue;
    imuReady = false;
    CORETIMER_DelayMs(70);
    wValue[0] = WHO_AM_I;
    I2C2_WriteRead(IMU_DEVICE_ADDRESS, wValue, 1, &rValue, 1);
    while (I2C2_IsBusy());
    if (rValue != 0x6c) {
        return false;
    }
    //Do a software reset to reset register values to defaults
    wValue[0] = CTRL3_C;
    wValue[1] = SW_RESET;
    I2C2_Write(IMU_DEVICE_ADDRESS, wValue, 2);
    while (I2C2_IsBusy());
    CORETIMER_DelayUs(50);
    //Device settings
    wValue[0] = INT1_CTRL;
    wValue[1] = 1; //INT1_DRDY_XL
    I2C2_Write(IMU_DEVICE_ADDRESS, wValue, 2);
    while (I2C2_IsBusy());
    wValue[0] = CTRL9_XL;
    wValue[1] = 0b00000010; //No DEN stamping, I3C disabled
    I2C2_Write(IMU_DEVICE_ADDRESS, wValue, 2);
    while (I2C2_IsBusy());
    wValue[0] = CTRL4_C;
    wValue[1] = 0b00001000; //DRDY_MASK
    I2C2_Write(IMU_DEVICE_ADDRESS, wValue, 2);
    while (I2C2_IsBusy());
    //Configure accelerometer
    wValue[0] = CTRL1_XL;
    wValue[1] = 0b01100000; //416Hz, 4g range, LPF2_XL_EN off
    I2C2_Write(IMU_DEVICE_ADDRESS, wValue, 2);
    while (I2C2_IsBusy());
    wValue[0] = CTRL8_XL; //TODO may want to remove this - see below
    wValue[1] = 0b00000000; //LPF2 at ODR/4 = 104Hz Only if LPF2_XL_EN is on
    I2C2_Write(IMU_DEVICE_ADDRESS, wValue, 2);
    while (I2C2_IsBusy());
    wValue[0] = CTRL7_G;
    wValue[1] = 0b00000010; //USER_OFF_ON_OUT enable user offsets for accel.
    I2C2_Write(IMU_DEVICE_ADDRESS, wValue, 2);
    while (I2C2_IsBusy());
    wValue[0] = X_OFS_USR; //Write user offsets
    wValue[1] = settings.levelOffsets[0];
    wValue[2] = settings.levelOffsets[1];
    wValue[3] = settings.levelOffsets[2];
    I2C2_Write(IMU_DEVICE_ADDRESS, wValue, 4);
    while (I2C2_IsBusy());
    //Configure gyroscope
    wValue[0] = CTRL2_G;
    wValue[1] = 0b01101100; //416Hz, 2000dps
    I2C2_Write(IMU_DEVICE_ADDRESS, wValue, 2);
    while (I2C2_IsBusy());
    wValue[0] = CTRL6_C;
    wValue[1] = 0b00000000; //Gyro LPF1 
    I2C2_Write(IMU_DEVICE_ADDRESS, wValue, 2);
    while (I2C2_IsBusy());
    //Configure IMU_INT1 interrupt
    INTCONbits.INT2EP = 1; //Rising edge
    EVIC_ExternalInterruptCallbackRegister(EXTERNAL_INT_2, imuIntHandler, 0);
    EVIC_ExternalInterruptEnable(EXTERNAL_INT_2);
    return true;
}

void imuTask(void *pvParameters) {
    const uint8_t reg = OUTX_L_G;
    int16_t gyroMins[3];
    int16_t gyroMaxs[3];
    int gyroSums[3] = {0, 0, 0};
    int samples = CALIBRATION_SAMPLE_COUNT;
    int16_t xGyroOffset = 0;
    int16_t yGyroOffset = 0;
    int16_t zGyroOffset = 0;
    bool moving;
    if (startMode == START_WDTO) {
        imuReady = true;
    } else {
        imuReady = false;
    }
    while (1) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        I2C2_WriteRead(IMU_DEVICE_ADDRESS, (uint8_t *) & reg, 1, (uint8_t *) rawImuData, 12);
        while (I2C2_IsBusy());
        if (!imuReady) {
            if (isUpright()) {
                moving = false;
                for (int i = 0; i < 3; ++i) {
                    if (samples == CALIBRATION_SAMPLE_COUNT) { //First sample
                        gyroMins[i] = gyroMaxs[i] = rawImuData[i];
                    } else if (rawImuData[i] < gyroMins[i]) {
                        gyroMins[i] = rawImuData[i];
                    } else if (rawImuData[i] > gyroMaxs[i]) {
                        gyroMaxs[i] = rawImuData[i];
                    }
                    if (gyroMaxs[i] - gyroMins[i] <= MAX_MOTION_ALLOWED) {
                        gyroSums[i] += rawImuData[i];
                    } else { //It's moving so start over
                        moving = true;
                    }
                }
                if (moving) {
                    samples = CALIBRATION_SAMPLE_COUNT;
                    for (int i = 0; i < 3; ++i) {
                        gyroSums[i] = 0;
                    }
                } else {
                    --samples;
                    if (samples == 0) {
                        xGyroOffset = -gyroSums[0] / CALIBRATION_SAMPLE_COUNT;
                        yGyroOffset = -gyroSums[1] / CALIBRATION_SAMPLE_COUNT;
                        zGyroOffset = -gyroSums[2] / CALIBRATION_SAMPLE_COUNT;
                        rawImuData[0] += xGyroOffset;
                        rawImuData[1] += yGyroOffset;
                        rawImuData[2] += zGyroOffset;
                        imuReady = true;
                        xQueueOverwrite(imuQueue, rawImuData);
                    }
                }
            } else {
                //Not upright so do nothing
            }
        } else {
            rawImuData[0] += xGyroOffset;
            rawImuData[1] += yGyroOffset;
            rawImuData[2] += zGyroOffset;
            xQueueOverwrite(imuQueue, rawImuData);
        }
    }
}

void imuIntHandler(EXTERNAL_INT_PIN pin, uintptr_t context) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    vTaskNotifyGiveFromISR(imuTaskHandle, &xHigherPriorityTaskWoken);
    portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
}

void disableAccelOffsets(void) {
    uint8_t wValue[2];
    wValue[0] = CTRL7_G;
    wValue[1] = 0b00000000; //disable user offsets for accel.
    I2C2_Write(IMU_DEVICE_ADDRESS, wValue, 2);
    while (I2C2_IsBusy());
}

void enableAccelOffsets(void) {
    uint8_t wValue[2];
    wValue[0] = CTRL7_G;
    wValue[1] = 0b00000010; //USER_OFF_ON_OUT enable user offsets for accel.
    I2C2_Write(IMU_DEVICE_ADDRESS, wValue, 2);
    while (I2C2_IsBusy());
}

bool isUpright(void) {
    switch (settings.gyroOrientation) {
        case FLAT_ORIENTATION:
            if (rawImuData[5] > UPRIGHT_MIN_VALUE) {
                return true;
            }
            break;
        case INVERTED_ORIENTATION:
            if (rawImuData[5] < -UPRIGHT_MIN_VALUE) {
                return true;
            }
            break;
        case LEFT_DOWN_ORIENTATION:
            if (rawImuData[4] < -UPRIGHT_MIN_VALUE) {
                return true;
            }
            break;
        case RIGHT_DOWN_ORIENTATION:
            if (rawImuData[4] > UPRIGHT_MIN_VALUE) {
                return true;
            }
            break;
    }
    return false;
}