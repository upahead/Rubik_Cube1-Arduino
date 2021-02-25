#ifndef MPU6886_H__
#define MPU6886_H__

#include <Wire.h>

#define MPU6886_WHOAMI            0x75
#define MPU6886_ACCEL_INTEL_CTRL  0x69
#define MPU6886_SMPLRT_DIV        0x19          
#define MPU6886_INT_PIN_CFG       0x37
#define MPU6886_INT_ENABLE        0x38
#define MPU6886_ACCEL_XOUT_H      0x3B          
#define MPU6886_ACCEL_XOUT_L      0x3C          
#define MPU6886_ACCEL_YOUT_H      0x3D          
#define MPU6886_ACCEL_YOUT_L      0x3E          
#define MPU6886_ACCEL_ZOUT_H      0x3F          
#define MPU6886_ACCEL_ZOUT_L      0x40          

#define MPU6886_TEMP_OUT_H        0x41          
#define MPU6886_TEMP_OUT_L        0x42          

#define MPU6886_GYRO_XOUT_H       0x43          
#define MPU6886_GYRO_XOUT_L       0x44          
#define MPU6886_GYRO_YOUT_H       0x45          
#define MPU6886_GYRO_YOUT_L       0x46          
#define MPU6886_GYRO_ZOUT_H       0x47          
#define MPU6886_GYRO_ZOUT_L       0x48          

#define MPU6886_USER_CTRL         0x6A
#define MPU6886_PWR_MGMT_1        0x6B          
#define MPU6886_PWR_MGMT_2        0x6C
#define MPU6886_CONFIG            0x1A          
#define MPU6886_GYRO_CONFIG       0x1B         
#define MPU6886_ACCEL_CONFIG      0x1C          
#define MPU6886_ACCEL_CONFIG2     0x1D
#define MPU6886_FIFO_EN           0x23

#define MPU6886_FIFO_ENABLE       0x23          
#define MPU6886_FIFO_CONUTH       0x72         
#define MPU6886_FIFO_CONUTL       0x73          
#define MPU6886_FIFO_R_W          0x74          


#define ADDR 0x68                               //IIC correspondence address of MPU6886

class I2C_MPU6886 {
  public:
    I2C_MPU6886(){}

    int begin(void);

    void getAccel(float* ax, float* ay, float* az);
    void getGyro(float* gx, float* gy, float* gz);
    void getTemp(float *t);

  private:
    uint8_t readByte(uint8_t address);
    void writeByte(uint8_t address, uint8_t data);

    float aRes, gRes; 
};

#endif