#include "Sensor.h"

Sensor::Sensor(PinName sda, PinName scl) : m_i2c(sda,scl){

}

Sensor::~Sensor(){

}

Sensor::getRegValue(uint8_t addr, uint8_t reg, uint8_t msb = 7, uint8_t lsb = 0){
  if((msb > 7) || (lsb > 7) || (lsb > msb)) {
    return 0xFF;
  }
      
  uint8_t rawValue = readRegister(addr, reg);
  uint8_t maskedValue = rawValue & ((0b11111111 << lsb) | (0b11111111 >> (7 - msb)));
      
  return(maskedValue);
}

Sensor::setRegValue(uint8_t addr, uint8_t reg, uint8_t value, uint8_t msb = 7, uint8_t lsb = 0){
  if((msb > 7) || (lsb > 7) || (lsb > msb)) {
    return 0xFF;
  }
      
  uint8_t currentValue = readRegister(deviceAddress, reg);
  uint8_t newValue = currentValue & ((0b11111111 << (msb + 1)) | (0b11111111 >> (8 - lsb)));
      
  writeRegister(deviceAddress, reg, newValue | value);
}

Sensor::readRegister(uint8_t addr, uint8_t reg){
  // Wire.write(reg);
  uint8_t inByte;
  m_i2c.write(addr, reg, 1);
  m_i2c.read(addr | 1, inByte, 1);
  return(inByte);
}

Sensor::writeRegister(uint8_t addr,uint8_t reg, uint8_t data){
  cmd[0] = reg;            // pointer to command register
  cmd[1] = data;
  m_i2c.write(addr, cmd, 2);
}
