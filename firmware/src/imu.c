
#include "definitions.h"
#include "imu.h"

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

int16_t xlData[3];

void imuIntHandler(EXTERNAL_INT_PIN pin, uintptr_t context);

bool initIMU(void) {
    uint8_t wValue[2];
    uint8_t rValue;
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
    //Accel data ready of INT1
    wValue[0] = INT1_CTRL;
    wValue[1] = 1;
    I2C2_Write(IMU_DEVICE_ADDRESS, wValue, 2);
    while (I2C2_IsBusy());
    //Configure accelerometer
    wValue[0] = CTRL1_XL;
    wValue[1] = 0b01000000;  //104Hz, 4g range
    I2C2_Write(IMU_DEVICE_ADDRESS, wValue, 2);
    while (I2C2_IsBusy());
    INTCONbits.INT2EP = 1;  //Rising edge
    EVIC_ExternalInterruptCallbackRegister(EXTERNAL_INT_2, imuIntHandler, 0);
    EVIC_ExternalInterruptEnable(EXTERNAL_INT_2);
    return true;
}


void imuTask(void *pvParameters) {
    static uint8_t reg = OUTX_L_A;
    while (1) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        I2C2_WriteRead(IMU_DEVICE_ADDRESS, &reg, 1, (uint8_t *)xlData, 6);
        while (I2C2_IsBusy());
    }
}

void imuIntHandler(EXTERNAL_INT_PIN pin, uintptr_t context) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    vTaskNotifyGiveFromISR(imuTaskHandle, &xHigherPriorityTaskWoken);
    portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
}