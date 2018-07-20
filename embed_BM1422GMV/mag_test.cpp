#include "mbed.h"
#include "BM1422GMV.h"
Serial  pc(P0_9, P0_11);
#define DEBUG(...) { pc.printf(__VA_ARGS__); }
#define ADDR 0x1C;
DigitalOut myled(LED1);
//DigitalInOut pin(P0_7);
BM1422GMV magSensor(I2C_SDA0, I2C_SCL0);
I2C m_i2c(I2C_SDA0, I2C_SCL0);
InterruptIn DRDY(P0_7);

char read(uint8_t addr, uint8_t reg){
  char inByte[1];
  char c_reg[1];
  c_reg[0] = (char)reg;
  m_i2c.write(addr, c_reg, 1);
  m_i2c.read(addr | 1, inByte, 1);
  return(inByte[0]);
}

void write(uint8_t addr,uint8_t reg, uint8_t data){
  char cmd[2];
  cmd[0] = reg;            // pointer to command register
  cmd[1] = data;
  m_i2c.write(addr, cmd, 2);
    }


void DoSomething(void){
    myled = 1;
    DEBUG("Start to measure: \n");
    magSensor.measure();
    DEBUG(" MagX: %f \n", magSensor.magX);
    DEBUG(" MagY: %f \n", magSensor.magY);
    DEBUG(" MagZ: %f \n", magSensor.magZ);
    } 

void testSingleMode(char *cmd){
    //char cmd[2];
    write(0x1C, 0x1B, 0xC2);
    cmd [0] = read(0x1C, 0x1B);
    DEBUG(" CNTL1 : %x \n", cmd[0]);
    write(0x1C, 0x5C, 0x00);
    write(0x1C, 0x5D, 0x00);
    cmd[0] = read(0x1C, 0x5C);
    DEBUG(" CNTL4(1) : %x\n", cmd[0]);
    cmd[0] = read(0x1C, 0x5D);
    DEBUG(" CNTL4(2) : %x\n", cmd[0]);
    write(0x1C, 0x1C, 0x0C);
    cmd [0] = read(0x1C, 0x1C);
    DEBUG(" CNT2 : %x\n", cmd[0]);
    write(0x1C, 0x1D, 0x40);
    cmd [0] = read(0x1C, 0x1D);
    }
void get_val(char *val)
{ 
  
  float data[1];
  signed short mag[1];
  mag[0] = ((signed short)val[1] << 8) | (val[0]);
  data[0] = (float)mag[0] / 24;
  DEBUG("The mag amplitude: %f \n", data[0]);
}

int main() {
    char cmd[2];
    magSensor.init(BM1422GMV_MODE_SINGLE, BM1422GMV_OUTPUT_RATE_20_HZ, BM1422GMV_OUTPUT_14_BIT, BM1422GMV_AVERAGE_4);
    DEBUG("The initial is finished \n");
    //testSingleMode();
    //DRDY.rise(&DoSomething);
    while(1) {
        write(0x1C, 0x1D, 0x40);
        wait(0.05);
        cmd[0] = read(0x1C, 0x10);
        cmd[1] = read(0x1C, 0x11);
        get_val(cmd);
        DoSomething();
    }
}
