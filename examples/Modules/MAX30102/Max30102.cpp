/***************************************************
  This is a library written for the Maxim MAX30105 Optical Smoke Detector
  It should also work with the MAX30102. However, the MAX30102 does not have a Green LED.

  These sensors use I2C to communicate, as well as a single (optional)
  interrupt line that is not currently supported in this driver.

  Written by Peter Jansen and Nathan Seidle (SparkFun)
  BSD license, all text above must be included in any redistribution.
 *****************************************************/

#include "MAX30102.h"


// Status Registers
static const uint8_t MAX30105_INTSTAT1 =    0x00;
static const uint8_t MAX30105_INTSTAT2 =    0x01;
static const uint8_t MAX30105_INTENABLE1 =    0x02;
static const uint8_t MAX30105_INTENABLE2 =    0x03;

// FIFO Registers
static const uint8_t MAX30105_FIFOWRITEPTR =  0x04;
static const uint8_t MAX30105_FIFOOVERFLOW =  0x05;
static const uint8_t MAX30105_FIFOREADPTR =   0x06;
static const uint8_t MAX30105_FIFODATA =    0x07;

// Configuration Registers
static const uint8_t MAX30105_FIFOCONFIG =    0x08;
static const uint8_t MAX30105_MODECONFIG =    0x09;
static const uint8_t MAX30105_PARTICLECONFIG =  0x0A;    // Note, sometimes listed as "SPO2" config in datasheet (pg. 11)
static const uint8_t MAX30105_LED1_PULSEAMP =   0x0C;
static const uint8_t MAX30105_LED2_PULSEAMP =   0x0D;
static const uint8_t MAX30105_LED3_PULSEAMP =   0x0E;
static const uint8_t MAX30105_LED_PROX_AMP =  0x10;
static const uint8_t MAX30105_MULTILEDCONFIG1 = 0x11;
static const uint8_t MAX30105_MULTILEDCONFIG2 = 0x12;

// Die Temperature Registers
static const uint8_t MAX30105_DIETEMPINT =    0x1F;
static const uint8_t MAX30105_DIETEMPFRAC =   0x20;
static const uint8_t MAX30105_DIETEMPCONFIG =   0x21;

// Proximity Function Registers
static const uint8_t MAX30105_PROXINTTHRESH =   0x30;

// Part ID Registers
static const uint8_t MAX30105_REVISIONID =    0xFE;
static const uint8_t MAX30105_PARTID =      0xFF;    // Should always be 0x15. Identical to MAX30102.

// MAX30105 Commands
// Interrupt configuration (pg 13, 14)
static const uint8_t MAX30105_INT_A_FULL_MASK =   (byte)~0b10000000;
static const uint8_t MAX30105_INT_A_FULL_ENABLE =   0x80;
static const uint8_t MAX30105_INT_A_FULL_DISABLE =  0x00;

static const uint8_t MAX30105_INT_DATA_RDY_MASK = (byte)~0b01000000;
static const uint8_t MAX30105_INT_DATA_RDY_ENABLE = 0x40;
static const uint8_t MAX30105_INT_DATA_RDY_DISABLE = 0x00;

static const uint8_t MAX30105_INT_ALC_OVF_MASK = (byte)~0b00100000;
static const uint8_t MAX30105_INT_ALC_OVF_ENABLE =  0x20;
static const uint8_t MAX30105_INT_ALC_OVF_DISABLE = 0x00;

static const uint8_t MAX30105_INT_PROX_INT_MASK = (byte)~0b00010000;
static const uint8_t MAX30105_INT_PROX_INT_ENABLE = 0x10;
static const uint8_t MAX30105_INT_PROX_INT_DISABLE = 0x00;

static const uint8_t MAX30105_INT_DIE_TEMP_RDY_MASK = (byte)~0b00000010;
static const uint8_t MAX30105_INT_DIE_TEMP_RDY_ENABLE = 0x02;
static const uint8_t MAX30105_INT_DIE_TEMP_RDY_DISABLE = 0x00;

static const uint8_t MAX30105_SAMPLEAVG_MASK =  (byte)~0b11100000;
static const uint8_t MAX30105_SAMPLEAVG_1 =   0x00;
static const uint8_t MAX30105_SAMPLEAVG_2 =   0x20;
static const uint8_t MAX30105_SAMPLEAVG_4 =   0x40;
static const uint8_t MAX30105_SAMPLEAVG_8 =   0x60;
static const uint8_t MAX30105_SAMPLEAVG_16 =  0x80;
static const uint8_t MAX30105_SAMPLEAVG_32 =  0xA0;

static const uint8_t MAX30105_ROLLOVER_MASK =   0xEF;
static const uint8_t MAX30105_ROLLOVER_ENABLE = 0x10;
static const uint8_t MAX30105_ROLLOVER_DISABLE = 0x00;

static const uint8_t MAX30105_A_FULL_MASK =   0xF0;

// Mode configuration commands (page 19)
static const uint8_t MAX30105_SHUTDOWN_MASK =   0x7F;
static const uint8_t MAX30105_SHUTDOWN =    0x80;
static const uint8_t MAX30105_WAKEUP =      0x00;

static const uint8_t MAX30105_RESET_MASK =    0xBF;
static const uint8_t MAX30105_RESET =       0x40;

static const uint8_t MAX30105_MODE_MASK =     0xF8;
static const uint8_t MAX30105_MODE_REDONLY =  0x02;
static const uint8_t MAX30105_MODE_REDIRONLY =  0x03;
static const uint8_t MAX30105_MODE_MULTILED =   0x07;

// Particle sensing configuration commands (pgs 19-20)
static const uint8_t MAX30105_ADCRANGE_MASK =   0x9F;
static const uint8_t MAX30105_ADCRANGE_2048 =   0x00;
static const uint8_t MAX30105_ADCRANGE_4096 =   0x20;
static const uint8_t MAX30105_ADCRANGE_8192 =   0x40;
static const uint8_t MAX30105_ADCRANGE_16384 =  0x60;

static const uint8_t MAX30105_SAMPLERATE_MASK = 0xE3;
static const uint8_t MAX30105_SAMPLERATE_50 =   0x00;
static const uint8_t MAX30105_SAMPLERATE_100 =  0x04;
static const uint8_t MAX30105_SAMPLERATE_200 =  0x08;
static const uint8_t MAX30105_SAMPLERATE_400 =  0x0C;
static const uint8_t MAX30105_SAMPLERATE_800 =  0x10;
static const uint8_t MAX30105_SAMPLERATE_1000 = 0x14;
static const uint8_t MAX30105_SAMPLERATE_1600 = 0x18;
static const uint8_t MAX30105_SAMPLERATE_3200 = 0x1C;

static const uint8_t MAX30105_PULSEWIDTH_MASK = 0xFC;
static const uint8_t MAX30105_PULSEWIDTH_69 =   0x00;
static const uint8_t MAX30105_PULSEWIDTH_118 =  0x01;
static const uint8_t MAX30105_PULSEWIDTH_215 =  0x02;
static const uint8_t MAX30105_PULSEWIDTH_411 =  0x03;

//Multi-LED Mode configuration (pg 22)
static const uint8_t MAX30105_SLOT1_MASK =    0xF8;
static const uint8_t MAX30105_SLOT2_MASK =    0x8F;
static const uint8_t MAX30105_SLOT3_MASK =    0xF8;
static const uint8_t MAX30105_SLOT4_MASK =    0x8F;

static const uint8_t SLOT_NONE =        0x00;
static const uint8_t SLOT_RED_LED =       0x01;
static const uint8_t SLOT_IR_LED =        0x02;
static const uint8_t SLOT_GREEN_LED =       0x03;
static const uint8_t SLOT_NONE_PILOT =      0x04;
static const uint8_t SLOT_RED_PILOT =     0x05;
static const uint8_t SLOT_IR_PILOT =      0x06;
static const uint8_t SLOT_GREEN_PILOT =     0x07;

static const uint8_t MAX_30105_EXPECTEDPARTID = 0x15;

/**
  * @brief  初始化MAX30102
  * @parameter i2cSpeed: IIC时速
  * @parameter i2caddr: MAX30102地址
  * @retval 成功返回true，失败返回false
  */
boolean MAX30102::begin(TwoWire &wirePort, uint32_t i2cSpeed, uint8_t i2caddr) {

  _i2cPort = &wirePort; //Grab which port the user wants us to use

  _i2cPort->begin();
  _i2cPort->setClock(i2cSpeed);

  _i2caddr = i2caddr;

  // Step 1: Initial Communication and Verification
  // Check that a MAX30105 is connected
  if (readPartID() != MAX_30105_EXPECTEDPARTID) {
    // Error -- Part ID read from MAX30105 does not match expected part ID.
    // This may mean there is a physical connectivity problem (broken wire, unpowered, etc).
    return false;
  }

  // Populate revision ID
  readRevisionID();
  
  return true;
}


/**
  * @brief  重置MAX30102
  * @parameter void
  * @retval void
  */
void MAX30102::softReset(void) {
  bitMask(MAX30105_MODECONFIG, MAX30105_RESET_MASK, MAX30105_RESET);

  // Poll for bit to clear, reset is then complete
  // Timeout after 100ms
  unsigned long startTime = millis();
  while (millis() - startTime < 100)
  {
    uint8_t response = readRegister8(_i2caddr, MAX30105_MODECONFIG);
    if ((response & MAX30105_RESET) == 0) break; //We're done!
    delay(1); //Let's not over burden the I2C bus
  }
}

/**
  * @brief  设定MAX30102中的LED工作模式
  * @parameter mode:工作模式参数
  * @retval void
  */
void MAX30102::setLEDMode(uint8_t mode) {
  // Set which LEDs are used for sampling -- Red only, RED+IR only, or custom.
  // See datasheet, page 19
  bitMask(MAX30105_MODECONFIG, MAX30105_MODE_MASK, mode);
}

/**
  * @brief  设定MAX30102中的ADC采集范围
  * @parameter adcRange:采集范围参数
  * @retval void
  */
void MAX30102::setADCRange(uint8_t adcRange) {
  // adcRange: one of MAX30105_ADCRANGE_2048, _4096, _8192, _16384
  bitMask(MAX30105_PARTICLECONFIG, MAX30105_ADCRANGE_MASK, adcRange);
}

/**
  * @brief  设定MAX30102中的抽样率
  * @parameter sampleRate:抽样率参数
  * @retval void
  */
void MAX30102::setSampleRate(uint8_t sampleRate) {
  // sampleRate: one of MAX30105_SAMPLERATE_50, _100, _200, _400, _800, _1000, _1600, _3200
  bitMask(MAX30105_PARTICLECONFIG, MAX30105_SAMPLERATE_MASK, sampleRate);
}

/**
  * @brief  设定MAX30102中的脉冲宽度
  * @parameter pulseWidth:脉冲宽度参数
  * @retval void
  */
void MAX30102::setPulseWidth(uint8_t pulseWidth) {
  // pulseWidth: one of MAX30105_PULSEWIDTH_69, _188, _215, _411
  bitMask(MAX30105_PARTICLECONFIG, MAX30105_PULSEWIDTH_MASK, pulseWidth);
}

/**
  * @brief  设定MAX30102中的红色LED的脉冲幅度
  * @parameter amplitude:红色LED的脉冲幅度参数
  * @retval void
  */
void MAX30102::setPulseAmplitudeRed(uint8_t amplitude) {
  writeRegister8(_i2caddr, MAX30105_LED1_PULSEAMP, amplitude);
  // NOTE: Amplitude values: 0x00 = 0mA, 0x7F = 25.4mA, 0xFF = 50mA (typical)
}

/**
  * @brief  设定MAX30102中的红外线的脉冲幅度
  * @parameter amplitude:红外线的脉冲幅度参数
  * @retval void
  */
void MAX30102::setPulseAmplitudeIR(uint8_t amplitude) {
  writeRegister8(_i2caddr, MAX30105_LED2_PULSEAMP, amplitude);
}

/**
  * @brief  设定MAX30105中的绿色LED灯的脉冲幅度
  * @parameter amplitude:绿色LED灯的脉冲幅度参数
  * @retval void
  */
void MAX30102::setPulseAmplitudeGreen(uint8_t amplitude) {
  writeRegister8(_i2caddr, MAX30105_LED3_PULSEAMP, amplitude);
}

/**
  * @brief  设定MAX30105中的肺动脉高压
  * @parameter amplitude:肺动脉高压脉冲幅度参数
  * @retval void
  */
void MAX30102::setPulseAmplitudeProximity(uint8_t amplitude) {
  writeRegister8(_i2caddr, MAX30105_LED_PROX_AMP, amplitude);
}

/**
  * @brief  设定MAX30102将触发粒子感应模式开始的IR ADC计数
  * @parameter threshMSB:ADC计数的8个最高有效位。
  * @retval void
  */
void MAX30102::setProximityThreshold(uint8_t threshMSB) {
  // Set the IR ADC count that will trigger the beginning of particle-sensing mode.
  // The threshMSB signifies only the 8 most significant-bits of the ADC count.
  writeRegister8(_i2caddr, MAX30105_PROXINTTHRESH, threshMSB);
}

//Given a slot number assign a thing to it
//Devices are SLOT_RED_LED or SLOT_RED_PILOT (proximity)
//Assigning a SLOT_RED_LED will pulse LED
//Assigning a SLOT_RED_PILOT will ??
/**
  * @brief  给一个槽号分配一个东西给它
            设备为插槽红色指示灯或插槽红色指示灯（接近）
            分配一个槽\u红色\u发光二极管将脉冲发光二极管
  * @parameter slotNumber:  槽号
  * @parameter device:  设备
  * @retval void
  */
void MAX30102::enableSlot(uint8_t slotNumber, uint8_t device) {

  uint8_t originalContents;

  switch (slotNumber) {
    case (1):
      bitMask(MAX30105_MULTILEDCONFIG1, MAX30105_SLOT1_MASK, device);
      break;
    case (2):
      bitMask(MAX30105_MULTILEDCONFIG1, MAX30105_SLOT2_MASK, device << 4);
      break;
    case (3):
      bitMask(MAX30105_MULTILEDCONFIG2, MAX30105_SLOT3_MASK, device);
      break;
    case (4):
      bitMask(MAX30105_MULTILEDCONFIG2, MAX30105_SLOT4_MASK, device << 4);
      break;
    default:
      //Shouldn't be here!
      break;
  }
}

/**
  * @brief  清除所有插槽分配
  * @parameter void
  * @retval void
  */
void MAX30102::disableSlots(void) {
  writeRegister8(_i2caddr, MAX30105_MULTILEDCONFIG1, 0);
  writeRegister8(_i2caddr, MAX30105_MULTILEDCONFIG2, 0);
}

//
// FIFO Configuration
//

//Set sample average 
/**
  * @brief  设定样本平均值
  * @parameter numberOfSamples:  样本平均值参数
  * @retval void
  */
void MAX30102::setFIFOAverage(uint8_t numberOfSamples) {
  bitMask(MAX30105_FIFOCONFIG, MAX30105_SAMPLEAVG_MASK, numberOfSamples);
}

/**
  * @brief  将所有点重置为以已知状态开始（最好在开始读取FIFO数据前执行）
  * @parameter void
  * @retval void
  */
void MAX30102::clearFIFO(void) {
  writeRegister8(_i2caddr, MAX30105_FIFOWRITEPTR, 0);
  writeRegister8(_i2caddr, MAX30105_FIFOOVERFLOW, 0);
  writeRegister8(_i2caddr, MAX30105_FIFOREADPTR, 0);
}

/**
  * @brief  如果FIFO溢出，启用翻滚
  * @parameter void
  * @retval void
  */
void MAX30102::enableFIFORollover(void) {
  bitMask(MAX30105_FIFOCONFIG, MAX30105_ROLLOVER_MASK, MAX30105_ROLLOVER_ENABLE);
}

/**
  * @brief  如果FIFO溢出，则禁用回滚
  * @parameter void
  * @retval void
  */
void MAX30102::disableFIFORollover(void) {
  bitMask(MAX30105_FIFOCONFIG, MAX30105_ROLLOVER_MASK, MAX30105_ROLLOVER_DISABLE);
}

/**
  * @brief  设置触发几乎完全中断的样本数
  * @parameter void
  * @retval void
  */
void MAX30102::setFIFOAlmostFull(uint8_t numberOfSamples) {
  bitMask(MAX30105_FIFOCONFIG, MAX30105_A_FULL_MASK, numberOfSamples);
  //Power on default is 32 samples
  //Note it is reverse: 0x00 is 32 samples, 0x0F is 17 samples
}

/**
  * @brief  读取FIFO写入指针
  * @parameter void
  * @retval void
  */
uint8_t MAX30102::getWritePointer(void) {
  return (readRegister8(_i2caddr, MAX30105_FIFOWRITEPTR));
}

/**
  * @brief  读取FIFO读取指针
  * @parameter void
  * @retval void
  */
uint8_t MAX30102::getReadPointer(void) {
  return (readRegister8(_i2caddr, MAX30105_FIFOREADPTR));
}

/**
  * @brief  读取模具温度
  * @parameter void
  * @retval 温度值
  */
float MAX30102::readTemperature() {
  
  //DIE_TEMP_RDY interrupt must be enabled
  //See issue 19: https://github.com/sparkfun/SparkFun_MAX3010x_Sensor_Library/issues/19
  
  // Step 1: Config die temperature register to take 1 temperature sample
  writeRegister8(_i2caddr, MAX30105_DIETEMPCONFIG, 0x01);

  // Poll for bit to clear, reading is then complete
  // Timeout after 100ms
  unsigned long startTime = millis();
  while (millis() - startTime < 100)
  {
    //uint8_t response = readRegister8(_i2caddr, MAX30105_DIETEMPCONFIG); //Original way
    //if ((response & 0x01) == 0) break; //We're done!
    
  //Check to see if DIE_TEMP_RDY interrupt is set
  uint8_t response = readRegister8(_i2caddr, MAX30105_INTSTAT2);
    if ((response & MAX30105_INT_DIE_TEMP_RDY_ENABLE) > 0) break; //We're done!
    delay(1); //Let's not over burden the I2C bus
  }
  //TODO How do we want to fail? With what type of error?
  //? if(millis() - startTime >= 100) return(-999.0);

  // Step 2: Read die temperature register (integer)
  int8_t tempInt = readRegister8(_i2caddr, MAX30105_DIETEMPINT);
  uint8_t tempFrac = readRegister8(_i2caddr, MAX30105_DIETEMPFRAC); //Causes the clearing of the DIE_TEMP_RDY interrupt

  // Step 3: Calculate temperature (datasheet pg. 23)
  return (float)tempInt + ((float)tempFrac * 0.0625);
}

/**
  * @brief  读取模具温度（单位:F）
  * @parameter void
  * @retval 温度值
  */
float MAX30102::readTemperatureF() {
  float temp = readTemperature();

  if (temp != -999.0) temp = temp * 1.8 + 32.0;

  return (temp);
}

/**
  * @brief  设置近程推力保持
  * @parameter void
  * @retval void
  */
void MAX30102::setPROXINTTHRESH(uint8_t val) {
  writeRegister8(_i2caddr, MAX30105_PROXINTTHRESH, val);
}

/**
  * @brief  读取设备版本
  * @parameter void
  * @retval void
  */
uint8_t MAX30102::readPartID() {
  return readRegister8(_i2caddr, MAX30105_PARTID);
}

/**
  * @brief  读取设备ID
  * @parameter void
  * @retval void
  */
void MAX30102::readRevisionID() {
  revisionID = readRegister8(_i2caddr, MAX30105_REVISIONID);
}

/**
  * @brief  返回设备ID
  * @parameter void
  * @retval void
  */
uint8_t MAX30102::getRevisionID() {
  return revisionID;
}

/**
  * @brief  设置传感器
            MAX30105有许多设置。默认情况下，我们选择：
            样本平均值=4
            模式=多LED
            ADC范围=16384（每LSB 62.5pA）
            采样率=50
  * @parameter void
  * @retval void
  */
void MAX30102::setup() 
{
  softReset(); //Reset all configuration, threshold, and data registers to POR values
  byte ledMode = 2;
  byte powerLevel = 0x7f;
  //FIFO Configuration
  //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
  //The chip will average multiple samples of same type together if you wish
  /*if (sampleAverage == 1) setFIFOAverage(MAX30105_SAMPLEAVG_1); //No averaging per FIFO record
  else if (sampleAverage == 2) setFIFOAverage(MAX30105_SAMPLEAVG_2);
  else if (sampleAverage == 4) */setFIFOAverage(MAX30105_SAMPLEAVG_4);
  /*else if (sampleAverage == 8) setFIFOAverage(MAX30105_SAMPLEAVG_8);
  else if (sampleAverage == 16) setFIFOAverage(MAX30105_SAMPLEAVG_16);
  else if (sampleAverage == 32) setFIFOAverage(MAX30105_SAMPLEAVG_32);
  else setFIFOAverage(MAX30105_SAMPLEAVG_4);*/

  //setFIFOAlmostFull(2); //Set to 30 samples to trigger an 'Almost Full' interrupt
  enableFIFORollover(); //Allow FIFO to wrap/roll over
  //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

  //Mode Configuration
  //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
  /*if (ledMode == 3) setLEDMode(MAX30105_MODE_MULTILED); //Watch all three LED channels
  else if (ledMode == 2)*/ setLEDMode(MAX30105_MODE_REDIRONLY); //Red and IR
  //else setLEDMode(MAX30105_MODE_REDONLY); //Red only
  activeLEDs = ledMode; //Used to control how many bytes to read from FIFO buffer
  //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

  //Particle Sensing Configuration
  //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
  /*if(adcRange < 4096) setADCRange(MAX30105_ADCRANGE_2048); //7.81pA per LSB
  else if(adcRange < 8192) setADCRange(MAX30105_ADCRANGE_4096); //15.63pA per LSB
  else if(adcRange < 16384) setADCRange(MAX30105_ADCRANGE_8192); //31.25pA per LSB
  else if(adcRange == 16384)*/ setADCRange(MAX30105_ADCRANGE_16384); //62.5pA per LSB
  //else setADCRange(MAX30105_ADCRANGE_2048);

  /*if (sampleRate < 100) setSampleRate(MAX30105_SAMPLERATE_50); //Take 50 samples per second
  else if (sampleRate < 200) setSampleRate(MAX30105_SAMPLERATE_100);
  else if (sampleRate < 400)*/ setSampleRate(MAX30105_SAMPLERATE_200);
  /*else if (sampleRate < 800) setSampleRate(MAX30105_SAMPLERATE_400);
  else if (sampleRate < 1000) setSampleRate(MAX30105_SAMPLERATE_800);
  else if (sampleRate < 1600) setSampleRate(MAX30105_SAMPLERATE_1000);
  else if (sampleRate < 3200) setSampleRate(MAX30105_SAMPLERATE_1600);
  else if (sampleRate == 3200) setSampleRate(MAX30105_SAMPLERATE_3200);
  else setSampleRate(MAX30105_SAMPLERATE_50);*/

  //The longer the pulse width the longer range of detection you'll have
  //At 69us and 0.4mA it's about 2 inches
  //At 411us and 0.4mA it's about 6 inches
  /*if (pulseWidth < 118) setPulseWidth(MAX30105_PULSEWIDTH_69); //Page 26, Gets us 15 bit resolution
  else if (pulseWidth < 215) setPulseWidth(MAX30105_PULSEWIDTH_118); //16 bit resolution
  else if (pulseWidth < 411) setPulseWidth(MAX30105_PULSEWIDTH_215); //17 bit resolution
  else if (pulseWidth == 411)*/ setPulseWidth(MAX30105_PULSEWIDTH_411); //18 bit resolution
  //else setPulseWidth(MAX30105_PULSEWIDTH_69);
  //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

  //LED Pulse Amplitude Configuration
  //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
  //Default is 0x1F which gets us 6.4mA
  //powerLevel = 0x02, 0.4mA - Presence detection of ~4 inch
  //powerLevel = 0x1F, 6.4mA - Presence detection of ~8 inch
  //powerLevel = 0x7F, 25.4mA - Presence detection of ~8 inch
  //powerLevel = 0xFF, 50.0mA - Presence detection of ~12 inch

  setPulseAmplitudeRed(powerLevel);
  setPulseAmplitudeIR(powerLevel);
  setPulseAmplitudeGreen(powerLevel);
  setPulseAmplitudeProximity(powerLevel);
  //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

  //Multi-LED Mode Configuration, Enable the reading of the three LEDs
  //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
  enableSlot(1, SLOT_RED_LED);
  /*if (ledMode > 1)*/ enableSlot(2, SLOT_IR_LED);
  //if (ledMode > 2) enableSlot(3, SLOT_GREEN_LED);
  //enableSlot(1, SLOT_RED_PILOT);
  //enableSlot(2, SLOT_IR_PILOT);
  //enableSlot(3, SLOT_GREEN_PILOT);
  //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

  clearFIFO(); //Reset the FIFO before we begin checking the sensor
}

/**
  * @brief  得到样品数
  * @parameter void
  * @retval void
  */
uint8_t MAX30102::available(void)
{
  int8_t numberOfSamples = sense.head - sense.tail;
  if (numberOfSamples < 0) numberOfSamples += STORAGE_SIZE;

  return (numberOfSamples);
}

/**
  * @brief  得到最近一次测量的红色值
  * @parameter void
  * @retval 测量的红色值
  */
uint32_t MAX30102::getRed(void)
{
  //Check the sensor for new data for 250ms
  if(safeCheck(250))
    return (sense.red[sense.head]);
  else
    return(0); //Sensor failed to find new data
}

/**
  * @brief  得到最近一次测量的红外值
  * @parameter void
  * @retval 测量的红外值
  */
uint32_t MAX30102::getIR(void)
{
  //Check the sensor for new data for 250ms
  if(safeCheck(250))
    return (sense.IR[sense.head]);
  else
    return(0); //Sensor failed to find new data
}

/**
  * @brief  得到在FIFO中下一个红色值
  * @parameter void
  * @retval void
  */
uint32_t MAX30102::getFIFORed(void)
{
  return (sense.red[sense.tail]);
}

/**
  * @brief  得到在FIFO中下一个红外值
  * @parameter void
  * @retval void
  */
uint32_t MAX30102::getFIFOIR(void)
{
  return (sense.IR[sense.tail]);
}

/**
  * @brief  得到在FIFO中下一个绿色值
  * @parameter void
  * @retval void
  */
uint32_t MAX30102::getFIFOGreen(void)
{
  return (sense.green[sense.tail]);
}

/**
  * @brief  推进尾部
  * @parameter void
  * @retval void
  */
void MAX30102::nextSample(void)
{
  if(available()) //Only advance the tail if new data is available
  {
    sense.tail++;
    sense.tail %= STORAGE_SIZE; //Wrap condition
  }
}

/**
  * @brief  轮询传感器以获取新数据
            如果有新数据可用，它将更新主结构中的head和tail
  * @parameter void
  * @retval numberOfSamples: 返回获得的新样本数 
  */
uint16_t MAX30102::check(void)
{
  //Read register FIDO_DATA in (3-byte * number of active LED) chunks
  //Until FIFO_RD_PTR = FIFO_WR_PTR

  byte readPointer = getReadPointer();
  byte writePointer = getWritePointer();

  int numberOfSamples = 0;

  //Do we have new data?
  if (readPointer != writePointer)
  {
    //Calculate the number of readings we need to get from sensor
    numberOfSamples = writePointer - readPointer;
    if (numberOfSamples < 0) numberOfSamples += 32; //Wrap condition

    //We now have the number of readings, now calc bytes to read
    //For this example we are just doing Red and IR (3 bytes each)
    int bytesLeftToRead = numberOfSamples * activeLEDs * 3;

    //Get ready to read a burst of data from the FIFO register
    _i2cPort->beginTransmission(MAX30105_ADDRESS);
    _i2cPort->write(MAX30105_FIFODATA);
    _i2cPort->endTransmission();

    //We may need to read as many as 288 bytes so we read in blocks no larger than I2C_BUFFER_LENGTH
    //I2C_BUFFER_LENGTH changes based on the platform. 64 bytes for SAMD21, 32 bytes for Uno.
    //Wire.requestFrom() is limited to BUFFER_LENGTH which is 32 on the Uno
    while (bytesLeftToRead > 0)
    {
      int toGet = bytesLeftToRead;
      if (toGet > I2C_BUFFER_LENGTH)
      {
        //If toGet is 32 this is bad because we read 6 bytes (Red+IR * 3 = 6) at a time
        //32 % 6 = 2 left over. We don't want to request 32 bytes, we want to request 30.
        //32 % 9 (Red+IR+GREEN) = 5 left over. We want to request 27.

        toGet = I2C_BUFFER_LENGTH - (I2C_BUFFER_LENGTH % (activeLEDs * 3)); //Trim toGet to be a multiple of the samples we need to read
      }

      bytesLeftToRead -= toGet;

      //Request toGet number of bytes from sensor
      _i2cPort->requestFrom(MAX30105_ADDRESS, toGet);
      
      while (toGet > 0)
      {
        sense.head++; //Advance the head of the storage struct
        sense.head %= STORAGE_SIZE; //Wrap condition

        byte temp[sizeof(uint32_t)]; //Array of 4 bytes that we will convert into long
        uint32_t tempLong;

        //Burst read three bytes - RED
        temp[3] = 0;
        temp[2] = _i2cPort->read();
        temp[1] = _i2cPort->read();
        temp[0] = _i2cPort->read();

        //Convert array to long
        memcpy(&tempLong, temp, sizeof(tempLong));
    
    tempLong &= 0x3FFFF; //Zero out all but 18 bits

        sense.red[sense.head] = tempLong; //Store this reading into the sense array

        if (activeLEDs > 1)
        {
          //Burst read three more bytes - IR
          temp[3] = 0;
          temp[2] = _i2cPort->read();
          temp[1] = _i2cPort->read();
          temp[0] = _i2cPort->read();

          //Convert array to long
          memcpy(&tempLong, temp, sizeof(tempLong));

      tempLong &= 0x3FFFF; //Zero out all but 18 bits
          
      sense.IR[sense.head] = tempLong;
        }

        if (activeLEDs > 2)
        {
          //Burst read three more bytes - Green
          temp[3] = 0;
          temp[2] = _i2cPort->read();
          temp[1] = _i2cPort->read();
          temp[0] = _i2cPort->read();

          //Convert array to long
          memcpy(&tempLong, temp, sizeof(tempLong));

      tempLong &= 0x3FFFF; //Zero out all but 18 bits

          sense.green[sense.head] = tempLong;
        }

        toGet -= activeLEDs * 3;
      }

    } //End while (bytesLeftToRead > 0)

  } //End readPtr != writePtr

  return (numberOfSamples); //Let the world know how much new data we found
}

/**
  * @brief  检查新数据，但在一段时间后放弃
  * @parameter void
  * @retval 成功返回true, 失败返回false
  */
bool MAX30102::safeCheck(uint8_t maxTimeToCheck)
{
  uint32_t markTime = millis();
  
  while(1)
  {
  if(millis() - markTime > maxTimeToCheck) return(false);

  if(check() == true) //We found new data!
    return(true);

  delay(1);
  }
}

/**
  * @brief  读取，屏蔽，设置寄存器
  * @parameter reg:  寄存器地址
  * @parameter mask:  屏蔽参数
  * @parameter mask:  设置参数
  * @retval  void
  */
void MAX30102::bitMask(uint8_t reg, uint8_t mask, uint8_t thing)
{
  // Grab current register context
  uint8_t originalContents = readRegister8(_i2caddr, reg);

  // Zero-out the portions of the register we're interested in
  originalContents = originalContents & mask;

  // Change contents
  writeRegister8(_i2caddr, reg, originalContents | thing);
}

//
// Low-level I2C Communication
//

/**
  * @brief  读取寄存器数据
  * @parameter reg:  寄存器地址
  * @parameter address:  器件地址
  * @retval  成功返回读取的数据，失败返回0
  */
uint8_t MAX30102::readRegister8(uint8_t address, uint8_t reg) {
  _i2cPort->beginTransmission(address);
  _i2cPort->write(reg);
  _i2cPort->endTransmission(false);

  _i2cPort->requestFrom((uint8_t)address, (uint8_t)1); // Request 1 byte
  if (_i2cPort->available())
  {
    return(_i2cPort->read());
  }

  return (0); //Fail

}

/**
  * @brief  得到计算血氧饱和度需要的数据
  * @parameter avered:  指向数据avered的指针
  * @parameter aveir:  指向数据aveir的指针
  * @parameter sumirrms:  指向数据sumirrms的指针
  * @parameter sumredrms:  指向数据sumredrms的指针
  * @parameter red:  红色值
  * @parameter ir:  红外值
  * @retval  void
  */
void MAX30102::caldata(double *avered,double *aveir,double *sumirrms,double *sumredrms,int32_t red, int32_t ir)
{
  double frate = 0.95; //low pass filter for IR/red LED value to eliminate AC component
  double fred, fir;
  fred = (double)red;
  fir = (double)ir;
  (*avered) = (*avered) * frate + (double)red * (1.0 - frate);//average red level by low pass filter
  (*aveir) = (*aveir) * frate + (double)ir * (1.0 - frate); //average IR level by low pass filter
  (*sumredrms) += (fred - (*avered)) * (fred - (*avered)); //square sum of alternate component of red level
  (*sumirrms) += (fir - (*aveir)) * (fir - (*aveir));//square sum of alternate component of IR level
}

/**
  * @brief  得到计算血氧饱度
  * @parameter avered:  指向数据avered的指针
  * @parameter aveir:  指向数据aveir的指针
  * @parameter sumirrms:  指向数据sumirrms的指针
  * @parameter sumredrms:  指向数据sumredrms的指针
  * @parameter ir:  红外值
  * @parameter SpO2: 指向血氧饱和度数据的指针
  * @parameter ESpO2:  指向平均血氧饱和度数据的指针
  * @retval  void
  */
void MAX30102::get_ESPO2(int32_t ir, double *avered,double *aveir,double *sumirrms,double *sumredrms, double *SpO2, double *ESpO2)
{
  static int i=0;
  double FSpO2 = 0.7; //filter factor for estimated SpO2
  i++;
   if(i%100==0){
      double R = (sqrt((*sumredrms)) / (*avered)) / (sqrt((*sumirrms)) / (*aveir));
      
      (*SpO2) = -23.3 * (R - 0.4) + 100; //http://ww1.microchip.com/downloads/jp/AppNotes/00001525B_JP.pdf
      (*ESpO2) = FSpO2 * (*ESpO2) + (1.0 - FSpO2) * (*SpO2);//low pass filter
      
      (*sumredrms) = 0.0; 
      (*sumirrms) = 0.0; 
      i=0;
    }  
}

/**
  * @brief  得到心率和平均心率
  * @parameter irValue:  指向红色值的指针
  * @parameter beatAvg:  指向平均心率的指针
  * @parameter beatsPerMinute:  指向心率的指针
  * @retval  成功返回true，失败返回false
  */
bool MAX30102::get_avgBPM(int32_t *irValue,  int *beatAvg, float *beatsPerMinute)
{
    static byte rateSpot;
   static byte rates[4];                  //Array of heart rates
   static long lastBeat;                  //Time at which the last beat occurred 
  if((*irValue)<500)
  {
    Serial.println("No finger");
    return false;
  }
  
  if (checkForBeat((*irValue)) == true)
  {
    //We sensed a beat!
    long delta = millis() - lastBeat;
    lastBeat = millis();

    *beatsPerMinute = 60 / (delta / 1000.0);

    if ((*beatsPerMinute) < 255 && (*beatsPerMinute) > 20)
    {
      rates[rateSpot++] = (byte)(*beatsPerMinute); //Store this reading in the array
      //(*rateSpot) %= RATE_SIZE; //Wrap variable
      rateSpot %= 4; //Wrap variable
      //Take average of readings
      *beatAvg = 0;
      for (byte x = 0 ; x < 4 ; x++)
        *beatAvg += rates[x];
      //*beatAvg /= RATE_SIZE;
      *beatAvg /= 4;
    }
    
    return true;
  }
}

/**
  * @brief  向寄存器写入数据
  * @parameter address:  器件地址
  * @parameter reg:  寄存器地址
  * @parameter value:  要写入的数据
  * @retval  void
  */
void MAX30102::writeRegister8(uint8_t address, uint8_t reg, uint8_t value) {
  _i2cPort->beginTransmission(address);
  _i2cPort->write(reg);
  _i2cPort->write(value);
  _i2cPort->endTransmission();
}
