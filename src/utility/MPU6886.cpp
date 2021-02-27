#include "MPU6886.h"

/**
  * @brief  Read a byte of data
  * @param  address：Register address
  * @retval data value
  */
uint8_t I2C_MPU6886::readByte(uint8_t address) {
  Wire.beginTransmission(ADDR);
  Wire.write(address);
  Wire.endTransmission();
  Wire.requestFrom(ADDR, 1);
  uint8_t val = Wire.read();

  ESP_LOGD("MPU6886", "readByte(%02X) = %02X", address, val);
  return val;
}

/**
  * @brief  Write a byte of data
  * @param  address：Register address
  * @param  data:data value
  * @retval none
  */
void I2C_MPU6886::writeByte(uint8_t address, uint8_t data) {
  Wire.beginTransmission(ADDR);
  Wire.write(address);
  Wire.write(data);
  Wire.endTransmission();
  ESP_LOGD("MPU6886", "writeByte(%02X) = %02X", address, data);
}

/**
  * @brief  Initialize mpu6886
  * @param  void
  * @retval success:return 0; failed:return 1
  */
int I2C_MPU6886::begin(void) {
  // WHO_AM_I : IMU Check
  uint8_t temp;
  temp=readByte(MPU6886_WHOAMI);
  
  if(temp != 0x19)
    return -1;
  delay(1);

  // PWR_MGMT_1(0x6b)
  writeByte(MPU6886_PWR_MGMT_1, 0x00);
  delay(10);

  // PWR_MGMT_1(0x6b)
  writeByte(MPU6886_PWR_MGMT_1, 1 << 7);
  delay(10);

  // PWR_MGMT_1(0x6b)
  writeByte(MPU6886_PWR_MGMT_1, 1 << 0);
  delay(10);

  // ACCEL_CONFIG(0x1c) : +-8G
  writeByte(MPU6886_ACCEL_CONFIG, 0x10);
  delay(1);

  // GYRO_CONFIG(0x1b) : +-2000dps
  writeByte(MPU6886_GYRO_CONFIG, 0x18);
  delay(1);

  // CONFIG(0x1a)
  writeByte(MPU6886_CONFIG, 0x01);
  delay(1);

  // SMPLRT_DIV(0x19)
  writeByte(MPU6886_SMPLRT_DIV, 0x05);
  delay(1);

  // INT_ENABLE(0x38)
  writeByte(MPU6886_INT_ENABLE, 0x00);
  delay(1);

  // ACCEL_CONFIG 2(0x1d)
  writeByte(MPU6886_ACCEL_CONFIG2, 0x00);
  delay(1);

  // USER_CTRL(0x6a)
  writeByte(MPU6886_USER_CTRL, 0x00);
  delay(1);

  // FIFO_EN(0x23)
  writeByte( MPU6886_FIFO_EN, 0x00);
  delay(1);

  // INT_PIN_CFG(0x37)
  writeByte(MPU6886_INT_PIN_CFG, 0x22);
  delay(1);

  // INT_ENABLE(0x38)
  writeByte(MPU6886_INT_ENABLE, 0x01);
  delay(100);

  return 0;
}

/**
  * @brief  Obtain the acceleration data of x-axis, Y-axis and z-axis 
  * @param  *ax:Acceleration data pointer to X-axis
  * @param  *ay:Acceleration data pointer to y-axis
  * @param  *az:Acceleration data pointer to z-axis 
  * @retval none
  */
void I2C_MPU6886::getAccel(float* ax, float* ay, float* az) {
  float aRes = 8.0 / 32768.0;
  *ax = (int16_t)((readByte(MPU6886_ACCEL_XOUT_H) << 8) | readByte(MPU6886_ACCEL_XOUT_L)) * aRes;
  *ay = (int16_t)((readByte(MPU6886_ACCEL_YOUT_H) << 8) | readByte(MPU6886_ACCEL_YOUT_L)) * aRes;
  *az = (int16_t)((readByte(MPU6886_ACCEL_ZOUT_H) << 8) | readByte(MPU6886_ACCEL_ZOUT_L)) * aRes;
}

/**
  * @brief  Obtain the angular velocity data of x-axis, Y-axis and z-axis
  * @param  *gx:Pointer to X-axis angle data
  * @param  *gy:Pointer to Y-axis angle data
  * @param  *gz:Pointer to Z-axis angle data
  * @retval none
  */
void I2C_MPU6886::getGyro(float* gx, float* gy, float* gz) {
  float gRes = 2000.0 / 32768.0;
  *gx = (int16_t)((readByte(MPU6886_GYRO_XOUT_H) << 8) | readByte(MPU6886_GYRO_XOUT_L)) * gRes;
  *gy = (int16_t)((readByte(MPU6886_GYRO_YOUT_H) << 8) | readByte(MPU6886_GYRO_YOUT_L)) * gRes;
  *gz = (int16_t)((readByte(MPU6886_GYRO_ZOUT_H) << 8) | readByte(MPU6886_GYRO_ZOUT_L)) * gRes;
}

/**
  * @brief  get the temperature data
  * @param  *t：Pointer to temperature data
  * @retval none
  */
void I2C_MPU6886::getTemp(float *t) {
  *t = 25.0 + ((readByte(MPU6886_TEMP_OUT_H) << 8) | readByte(MPU6886_TEMP_OUT_L)) / 326.8;
}