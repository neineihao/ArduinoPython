#include "BM1422GMV.h"


BM1422GMV::BM1422GMV(PinName sda, PinName scl) : Sensor(sda, scl){
  _address = BM1422GMV_DEVICE_ADDRESS_L;
  _outputSens = 24;
  magX = 0 ;
  magY = 0 ;
  magZ = 0 ;
}

//Initialization function
uint8_t BM1422GMV::init(uint8_t mode, uint8_t rate, uint8_t output,uint8_t avg) {
  //check manufacturer ID
  /*
  uint8_t mode, rate, output, avg;
  mode = BM1422GMV_MODE_SINGLE;
  rate = BM1422GMV_OUTPUT_RATE_20_HZ;
  output = BM1422GMV_OUTPUT_14_BIT;
  avg = BM1422GMV_AVERAGE_4;
  */
  
  if(getRegValue(_address, BM1422GMV_REG_WHO_AM_I, 7, 0) != BM1422GMV_WHO_AM_I) {
    //if the manufacturer ID does not match cancel initialization
    return(0);
  }
      
  //interrupt setup
  // attachInterrupt(_intNum, func, RISING);
  // TODO: Solve the Drdy signal problem
  // _flagDrdy = false;
      
  //set output sensitivity
  if(output == BM1422GMV_OUTPUT_14_BIT) {
    _outputSens = 24;
  } else if(output == BM1422GMV_OUTPUT_12_BIT) {
    _outputSens = 6;
  }
      
  if(mode == BM1422GMV_MODE_SINGLE) {
    //single mode: the measurements will be only taken when the measurement function is called, rate is ignored
    //set control registers according to datasheet and user settings
    setRegValue(_address, BM1422GMV_REG_CNTL_1, BM1422GMV_ACTIVE | output | mode, 7, 0);
    setRegValue(_address, BM1422GMV_REG_CNTL_4_MSB, 0x00, 7, 0);
    setRegValue(_address, BM1422GMV_REG_CNTL_4_LSB, 0x00, 7, 0);
    setRegValue(_address, BM1422GMV_REG_CNTL_2, BM1422GMV_DRDY_ON | BM1422GMV_DRDY_ACTIVE_HIGH, 3, 2);
    setRegValue(_address, BM1422GMV_REG_AVE_A, avg, 4, 2);
  } else if(mode == BM1422GMV_MODE_CONTINUOUS) {
    //TODO: implement continuous mode
  }
      
  return(1);
}

void BM1422GMV::get_val(char *val)
{ 
  signed short mag[3];
  mag[0] = ((signed short)val[1] << 8) | (val[0]);
  magX = (float)mag[0] / 24;
  mag[1] = ((signed short)val[3] << 8) | (val[2]);
  magY = (float)mag[1] / 24;
  mag[2] = ((signed short)val[5] << 8) | (val[4]);
  magZ = (float)mag[2] / 24;
}

//Measurement function
//TODO: implement single-axis measurement
uint8_t BM1422GMV::measure(void){
    char cmd[6];
    cmd[0] = readRegister(0x1C, 0x10);
    cmd[1] = readRegister(0x1C, 0x11);
    cmd[2] = readRegister(0x1C, 0x12);
    cmd[3] = readRegister(0x1C, 0x13);
    cmd[4] = readRegister(0x1C, 0x14);
    cmd[5] = readRegister(0x1C, 0x15);
    get_val(cmd);
//  magX = (float)(((signed short)getRegValue(_address, 0x10, 7, 0) << 8) | (getRegValue(_address, 0x11, 7, 0))) / _outputSens;
//  magX = (float)(((signed short)readRegister(_address, 0x10) << 8) | (readRegister(_address, 0x11))) / _outputSens;
//  magY = (float)(((signed short)getRegValue(_address, 0x12, 7, 0) << 8) | (getRegValue(_address, 0x13, 7, 0))) / _outputSens;
//  magY = (float)(((signed short)readRegister(_address, 0x12) << 8) | (readRegister(_address, 0x13))) / _outputSens;
//  magZ = (float)(((signed short)getRegValue(_address, 0x14, 7, 0) << 8) | (getRegValue(_address, 0x15, 7, 0))) / _outputSens;
//  magZ = (float)(((signed short)readRegister(_address, 0x14) << 8) | (readRegister(_address, 0x15))) / _outputSens;
  return(0);
}
    
// //Function to be called inside ISR, this will set data ready flag
// BM1422GMV::setFlagDrdy(void) {
//   _flagDrdy = true;
// }

