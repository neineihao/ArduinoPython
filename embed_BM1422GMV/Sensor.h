#ifndef SENSOR
#define SENSOR
#include "mbed.h"

/* i2c.frequency(400000);  // use fast (400 kHz) I2C    */

class Sensor{
 public:
  Sensor(PinName sda, PinName scl);
  
  uint8_t getRegValue(uint8_t addr, uint8_t reg, uint8_t msb, uint8_t lsb);
  
  uint8_t setRegValue(uint8_t addr, uint8_t reg, uint8_t value, uint8_t msb, uint8_t lsb);
  
  char readRegister(uint8_t addr, uint8_t reg);
  
  void writeRegister(uint8_t addr, uint8_t reg, uint8_t data);
  
  char read(uint8_t addr, uint8_t reg, uint8_t size, char *data);
  ~Sensor();
    
 private:
  I2C m_i2c;
};
  /* i2c.frequency(400000);  // use fast (400 kHz) I2C    */
#endif
