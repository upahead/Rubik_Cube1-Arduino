#include <MPU6886.h>

I2C_MPU6886 imu;                    

void setup() {
  Serial.begin(115200);
  delay(1000);
  Wire.begin();
  imu.begin();
}

void loop() {
  float ax;
  float ay;
  float az;
  float gx;
  float gy;
  float gz;
  float t;

  imu.getAccel(&ax, &ay, &az);
  imu.getGyro(&gx, &gy, &gz);
  imu.getTemp(&t);

  Serial.printf("%f,%f,%f,%f,%f,%f,%f\n", ax, ay, az, gx, gy, gz, t);

  delay(1000);
}
