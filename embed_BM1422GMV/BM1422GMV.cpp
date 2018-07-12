#include "BM1422GMV.h"


BM1422GMV::BM1422GMV(PinName sda, PinName scl, uint8_t addr = BM1422GMV_DEVICE_ADDRESS_L) {
  _address = addr;
  mi2c(sda, scl);
}

//Initialization function
BM1422GMV::init(void func(void), uint8_t mode = BM1422GMV_MODE_SINGLE, uint8_t rate = BM1422GMV_OUTPUT_RATE_20_HZ, uint8_t output = BM1422GMV_OUTPUT_14_BIT, uint8_t avg = BM1422GMV_AVERAGE_4) {
  //check manufacturer ID
  if(getRegValue(_address, BM1422GMV_REG_WHO_AM_I) != BM1422GMV_WHO_AM_I) {
    //if the manufacturer ID does not match cancel initialization
    return(1);
  }
      
  //interrupt setup
  attachInterrupt(_intNum, func, RISING);
  _flagDrdy = false;
      
  //set output sensitivity
  if(output == BM1422GMV_OUTPUT_14_BIT) {
    _outputSens = 24;
  } else if(output == BM1422GMV_OUTPUT_12_BIT) {
    _outputSens = 6;
  }
      
  if(mode == BM1422GMV_MODE_SINGLE) {
    //single mode: the measurements will be only taken when the measurement function is called, rate is ignored
    //set control registers according to datasheet and user settings
    setRegValue(_address, BM1422GMV_REG_CNTL_1, BM1422GMV_ACTIVE | output | mode);
    setRegValue(_address, BM1422GMV_REG_CNTL_4_MSB, 0x00);
    setRegValue(_address, BM1422GMV_REG_CNTL_4_LSB, 0x00);
    setRegValue(_address, BM1422GMV_REG_CNTL_2, BM1422GMV_DRDY_ON | BM1422GMV_DRDY_ACTIVE_HIGH, 3, 2);
    setRegValue(_address, BM1422GMV_REG_AVE_A, BM1422GMV_AVERAGE_4, 4, 2);
  } else if(mode == BM1422GMV_MODE_CONTINUOUS) {
    //TODO: implement continuous mode
  }
      
  return(0);
}
    
//Measurement function
//TODO: implement single-axis measurement
BM1422GMV::measure(void) {
  //if interrupts are used, check DRDY flag
  if(_intNum != INT_NONE) {
    //start new measurement
    setRegValue(_address, BM1422GMV_REG_CNTL_3, BM1422GMV_FORCE_MEASUREMENT, 6, 6);
    //if the flag is present, read measured values and calculate magnetic induction in uT
    if(_flagDrdy) {
      magX = (float)(((int16_t)getRegValue(_address, BM1422GMV_REG_DATA_X_MSB) << 8) | getRegValue(_address, BM1422GMV_REG_DATA_X_LSB)) / _outputSens;
      magY = (float)(((int16_t)getRegValue(_address, BM1422GMV_REG_DATA_Y_MSB) << 8) | getRegValue(_address, BM1422GMV_REG_DATA_Y_LSB)) / _outputSens;
      magZ = (float)(((int16_t)getRegValue(_address, BM1422GMV_REG_DATA_Z_MSB) << 8) | getRegValue(_address, BM1422GMV_REG_DATA_Z_LSB)) / _outputSens;
      //reset flag
      _flagDrdy = false;
      //acceleration values were successfully updated, return 0
      return(0);
    }
  } else {
    //BM1422GMV does not work without interrupts
  }
  return(1);
}
    
//Function to be called inside ISR, this will set data ready flag
BM1422GMV::setFlagDrdy(void) {
  _flagDrdy = true;
}

