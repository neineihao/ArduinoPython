#ifndef _ROHM_BM1422GMV_CPP
#define _ROHM_BM1422GMV_CPP

//BM1422GMV register map
#define BM1422GMV_REG_INFO_LSB                        0x0D
#define BM1422GMV_REG_INFO_MSB                        0x0E
#define BM1422GMV_REG_WHO_AM_I                        0x0F
#define BM1422GMV_REG_DATA_X_LSB                      0x10
#define BM1422GMV_REG_DATA_X_MSB                      0x11
#define BM1422GMV_REG_DATA_Y_LSB                      0x12
#define BM1422GMV_REG_DATA_Y_MSB                      0x13
#define BM1422GMV_REG_DATA_Z_LSB                      0x14
#define BM1422GMV_REG_DATA_Z_MSB                      0x15
#define BM1422GMV_REG_STA_1                           0x18
#define BM1422GMV_REG_CNTL_1                          0x1B
#define BM1422GMV_REG_CNTL_2                          0x1C
#define BM1422GMV_REG_CNTL_3                          0x1D
#define BM1422GMV_REG_PRET                            0x30
#define BM1422GMV_REG_AVE_A                           0x40
#define BM1422GMV_REG_CNTL_4_LSB                      0x5C
#define BM1422GMV_REG_CNTL_4_MSB                      0x5D
#define BM1422GMV_REG_TEMP_LSB                        0x60
#define BM1422GMV_REG_TEMP_MSB                        0x61
#define BM1422GMV_REG_OFF_X                           0x6C
#define BM1422GMV_REG_OFF_Y                           0x72
#define BM1422GMV_REG_OFF_Z                           0x78
#define BM1422GMV_REG_FINEOUTPUT_X_LSB                0x90
#define BM1422GMV_REG_FINEOUTPUT_X_MSB                0x91
#define BM1422GMV_REG_FINEOUTPUT_Y_LSB                0x92
#define BM1422GMV_REG_FINEOUTPUT_Y_MSB                0x93
#define BM1422GMV_REG_FINEOUTPUT_Z_LSB                0x94
#define BM1422GMV_REG_FINEOUTPUT_Z_MSB                0x95
#define BM1422GMV_REG_GAIN_PARA_X_TO_Z                0x9C
#define BM1422GMV_REG_GAIN_PARA_X_TO_Y                0x9D
#define BM1422GMV_REG_GAIN_PARA_Y_TO_Z                0x9E
#define BM1422GMV_REG_GAIN_PARA_Y_TO_X                0x9F
#define BM1422GMV_REG_GAIN_PARA_Z_TO_Y                0xA0
#define BM1422GMV_REG_GAIN_PARA_Z_TO_X                0xA1

//BM1422GMV default values
#define BM1422GMV_DEVICE_ADDRESS_L                    0x0E
#define BM1422GMV_DEVICE_ADDRESS_H                    0x0F
#define BM1422GMV_WHO_AM_I                            0x41
#define INT_0                                         0x00
#define INT_1                                         0x01
#define INT_NONE                                      0xFF

//BM1422GMV settings
//BM1422GMV_REG_STA_1                                                 MSB   LSB   DESCRIPTION
#define BM1422GMV_DRDY_NOT_READY                        0b00000000 // 6     6     measured data not ready
#define BM1422GMV_DRDY_READY                            0b01000000 // 6     6     measured data ready
//BM1422GMV_REG_CNTL_1
#define BM1422GMV_POWER_DOWN                            0b00000000 // 7     7     power control:  power down
#define BM1422GMV_ACTIVE                                0b10000000 // 7     7                     active
#define BM1422GMV_OUTPUT_12_BIT                         0b00000000 // 6     6     output: 12-bit
#define BM1422GMV_OUTPUT_14_BIT                         0b01000000 // 6     6             14-bit
#define BM1422GMV_RESET_RELEASE                         0b00000000 // 5     5     reset logic:  reset release
#define BM1422GMV_RESET                                 0b00100000 // 5     5                   reset
#define BM1422GMV_OUTPUT_RATE_10_HZ                     0b00000000 // 4     3     data output rate: 10 Hz
#define BM1422GMV_OUTPUT_RATE_20_HZ                     0b00001000 // 4     3                       20 Hz
#define BM1422GMV_OUTPUT_RATE_100_HZ                    0b00010000 // 4     3                       100 Hz
#define BM1422GMV_OUTPUT_RATE_1_KHZ                     0b00011000 // 4     3                       1 kHz
#define BM1422GMV_MODE_CONTINUOUS                       0b00000000 // 1     1     continuous measurement mode
#define BM1422GMV_MODE_SINGLE                           0b00000010 // 1     1     single measurement mode
//BM1422GMV_REG_CNTL_2
#define BM1422GMV_DRDY_OFF                              0b00000000 // 3     3     DRDY output disabled
#define BM1422GMV_DRDY_ON                               0b00001000 // 3     3     DRDY output enabled
#define BM1422GMV_DRDY_ACTIVE_LOW                       0b00000000 // 2     2     DRDY active low
#define BM1422GMV_DRDY_ACTIVE_HIGH                      0b00000100 // 2     2     DRDY active high
//BM1422GMV_REG_CNTL_3
#define BM1422GMV_FORCE_MEASUREMENT                     0b01000000 // 6     6     force start new measurement or restart running
//BM1422GMV_REG_AVE_A
#define BM1422GMV_AVERAGE_1                             0b00000100 // 4     2     number of measurements to average:  1
#define BM1422GMV_AVERAGE_2                             0b00001000 // 4     2                                         2
#define BM1422GMV_AVERAGE_4                             0b00000000 // 4     2                                         4
#define BM1422GMV_AVERAGE_8                             0b00001100 // 4     2                                         8
#define BM1422GMV_AVERAGE_16                            0b00010000 // 4     2                                         16

#include "Sensor.h"
#include "mbed.h"

class BM1422GMV : public Sensor{
 pubic:
  float magX = 0;
  float magY = 0;
  float magZ = 0;

  BM1422GMV(PinName sda, PinName scl, uint8_t address = BM1422GMV_DEVICE_ADDRESS_L);
  /* I2C mi2c(sda, scl) */
  
  /* uint8_t init(void func(void), uint8_t mode = BM1422GMV_MODE_SINGLE, uint8_t rate = BM1422GMV_OUTPUT_RATE_20_HZ, uint8_t output = BM1422GMV_OUTPUT_14_BIT, uint8_t avg = BM1422GMV_AVERAGE_4); */
  uint8_t init(uint8_t mode = BM1422GMV_MODE_SINGLE, uint8_t rate = BM1422GMV_OUTPUT_RATE_20_HZ, uint8_t output = BM1422GMV_OUTPUT_14_BIT, uint8_t avg = BM1422GMV_AVERAGE_4);
  /* Why the func(void) */
  
  uint8_t measure(void);
  
  volatile void setFlagDry(void);
  
 private:
  uint8_t _address;
  uint8_t _outputSens = 24;
  /* volatile bool _flagDrdy = false; */
}
#endif
