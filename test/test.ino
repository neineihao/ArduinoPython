/*****************************************************************************
  BM1422AGMV.ino

 Copyright (c) 2017 ROHM Co.,Ltd.

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
******************************************************************************/
#include <Wire.h>
#include <BM1422AGMV.h>

BM1422AGMV bm1422agmv(BM1422AGMV_DEVICE_ADDRESS_0E);
void bm1422agmv_isr(void);

void setup() {
  byte rc;

  Serial.begin(19200);
  while (!Serial);

  Wire.begin();

  rc = bm1422agmv.init();
  if (rc != 0) {
    Serial.println(F("BM1422AGMV initialization failed"));
    Serial.flush();
  }
  bm1422agmv.isr_func(0, bm1422agmv_isr);
}

void loop() {
  byte rc;
  float mag[3];

  rc = bm1422agmv.get_val(mag);

  if (rc == 0) {
    Serial.print(F("XDATA="));
    Serial.print(mag[0], 3);
    Serial.println("[uT]");
    Serial.print(F("YDATA="));
    Serial.print(mag[1], 3);
    Serial.println("[uT]");
    Serial.print(F("ZDATA="));
    Serial.print(mag[2], 3);
    Serial.println("[uT]");
    Serial.println();
  }else
  {
    Serial.println("Ping Pong");
  }

  delay(200);
}

void bm1422agmv_isr(void)
{
  bm1422agmv.set_drdy_flg();
}

