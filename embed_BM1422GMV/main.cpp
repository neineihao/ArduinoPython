#include "mbed.h"
#include "BLE.h"
#include "BM1422GMV.h"

/*
#define DBG 1
#define NEED_CONSOLE_OUTPUT 1 // Set this if you need debug messages on the console;
                              // it will have an impact on code-size and power consumption.
 
#if NEED_CONSOLE_OUTPUT
Serial  pc(USBTX, USBRX);
#define DEBUG(...) { pc.printf(__VA_ARGS__); }
#else
#define DEBUG(...) // nothing //
#endif // #if NEED_CONSOLE_OUTPUT //
*/
 
//#define MIN_CONN_INTERVAL 250  /**< Minimum connection interval */
//#define MAX_CONN_INTERVAL 350  /**< Maximum connection interval */
#define CONN_INTERVAL 25  /**< connection interval 250ms; in multiples of 0.125ms. (durationInMillis * 1000) / UNIT_0_625_MS; */
#define CONN_SUP_TIMEOUT  8000 /**< Connection supervisory timeout (6 seconds); in multiples of 0.125ms. */
#define SLAVE_LATENCY     0
#define TICKER_INTERVAL   2.0f
 
BLE   ble;
BM1422GMV Magsensor;
// TODO: BM1422GMV seneorMag(I2C_SDA0, I2C_SCL0);

// MPU6050 mpu(I2C_SDA0, I2C_SCL0);
 
static const char DEVICENAME[] = "BLE-MagSensor";
static volatile bool  triggerSensorPolling = false;
 
//const uint8_t MPU6050_adv_service_uuid[] = {
//    0x9F,0xDF,0x32,0x83,
//    0x90,0x49,
//    0xCF,0x8D,
//    0x5C,0x4D,    
//    0x98,0xE7,0xE2,0x00,0x27,0x31
//};
 
const uint8_t MPU6050_service_uuid[] = {
    0x45,0x35,0x56,0x80,0x0F,0xD8,0x5F,0xB5,0x51,0x48,0x30,0x27,0x06,0x9B,0x3F,0xD9
};

const uint8_t BM1422GMV_service_uuid[] = {
 // TODO: Decide the uuid 
}


const uint8_t MPU6050_Accel_Characteristic_uuid[] = {
    0x45,0x35,0x56,0x81,0x0F,0xD8,0x5F,0xB5,0x51,0x48,0x30,0x27,0x06,0x9B,0x3F,0xD9
};

const uint8_t BM1422GMV_Accel_Characteristic_uuid[] = {
  // TODO
}


const uint8_t MPU6050_Write_Characteristic_uuid[] =
{
    0x45,0x35,0x56,0x83,0x0F,0xD8,0x5F,0xB5,0x51,0x48,0x30,0x27,0x06,0x9B,0x3F,0xD9
};

const uint8_t BM1422GMV_Write_Charateristic_uuid[] = {
  // TODO
}



uint8_t accelPayload[sizeof(float)*10] = {0,};
 
uint8_t defaultWriteValue = 3;
uint8_t writePayload[2] = {defaultWriteValue, defaultWriteValue,};
 
 
GattCharacteristic  accelChar (MPU6050_Accel_Characteristic_uuid,
                                        accelPayload, (sizeof(float) * 10), (sizeof(float) * 10),
                                        GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ);
 
GattCharacteristic  writeChar (MPU6050_Write_Characteristic_uuid,
                                        writePayload, 2, 2,
                                        GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE_WITHOUT_RESPONSE);
 
GattCharacteristic *ControllerChars[] = { &accelChar, &writeChar, };
GattService         MPU6050Service(MPU6050_service_uuid, ControllerChars, sizeof(ControllerChars) / sizeof(GattCharacteristic *));
 
 
 
void updateValue(void){
    float   acData[3];
    float   gyData[3];
    float   tempData = 0.0f;
    float   at = 0.0f;
    float   gt = 0.0f;
    float   tickerInterval = 0.0f;
    
    //加速度を取得
    Timer acTimer;
    acTimer.start();
    mpu.getAccelero(acData);
    acTimer.stop();
    at = acTimer.read_ms();
    acTimer.reset();
 
    memcpy(accelPayload+sizeof(float)*0, &acData[0], sizeof(acData[0]));
    memcpy(accelPayload+sizeof(float)*1, &acData[1], sizeof(acData[1]));
    memcpy(accelPayload+sizeof(float)*2, &acData[2], sizeof(acData[2]));
 
    //ジャイロを取得
    Timer gyTimer;
    gyTimer.start();
    mpu.getGyro(gyData);
    gyTimer.stop();
    gt = gyTimer.read_ms();
    gyTimer.reset();
    
    memcpy(accelPayload+sizeof(float)*3, &gyData[0], sizeof(gyData[0]));
    memcpy(accelPayload+sizeof(float)*4, &gyData[1], sizeof(gyData[1]));
    memcpy(accelPayload+sizeof(float)*5, &gyData[2], sizeof(gyData[2]));
 
    //温度を取得
    tempData = mpu.getTemp();
 
    memcpy(accelPayload+sizeof(float)*6, &tempData, sizeof(tempData));
    /*
    pc.printf("Accel: %.3lf,%.3lf,%.3lf\r\n",
                        *(float*)&accelPayload[sizeof(float)*0],
                        *(float*)&accelPayload[sizeof(float)*1],
                        *(float*)&accelPayload[sizeof(float)*2]);
 
    pc.printf("Gyro: %.3lf,%.3lf,%.3lf\r\n",
                        *(float*)&accelPayload[sizeof(float)*3],
                        *(float*)&accelPayload[sizeof(float)*4],
                        *(float*)&accelPayload[sizeof(float)*5]);
 
    pc.printf("Temp: %.3lf\r\n", *(float*)&accelPayload[sizeof(float)*6]);
    */
    memcpy(accelPayload+sizeof(float)*7, &at, sizeof(at));
    memcpy(accelPayload+sizeof(float)*8, &gt, sizeof(gt));
 
    tickerInterval = TICKER_INTERVAL;
    memcpy(accelPayload+sizeof(float)*9, &tickerInterval, sizeof(tickerInterval));
 
    ble.updateCharacteristicValue(accelChar.getValueAttribute().getHandle(), accelPayload, sizeof(accelPayload));    //Mod
    ble.updateCharacteristicValue(writeChar.getValueAttribute().getHandle(), writePayload, sizeof(writePayload));    //Mod
}
 
void disconnectionCallback(Gap::Handle_t handle, Gap::DisconnectionReason_t reason)    // Mod
{
    
    //DEBUG("Disconnected handle %u, reason %u\n", handle, reason);
    //DEBUG("Restarting the advertising process\n\r");
 
    ble.startAdvertising();
}
 
void connectionCallback(const Gap::ConnectionCallbackParams_t *params)
{
 
    //DEBUG("connected. Got handle %u\r\n", handle);
 
    /*******************************************************************************/
    /*  CentralがMacOS X の時 connection intervalを設定する場合は                      */
    /*  nRF51822 -> projectconfig.h -> GAP ->                                      */
    /*  CFG_GAP_CONNECTION_MIN_INTERVAL_MS / CFG_GAP_CONNECTION_MAX_INTERVAL_MSを  */
    /*  直接編集すること                                                             */
    /******************************************************************************/
        //Gap::Handle_t handle;
        Gap::ConnectionParams_t gap_conn_params;
        gap_conn_params.minConnectionInterval        = Gap::MSEC_TO_GAP_DURATION_UNITS(CONN_INTERVAL);
        gap_conn_params.maxConnectionInterval        = Gap::MSEC_TO_GAP_DURATION_UNITS(CONN_INTERVAL);
        gap_conn_params.connectionSupervisionTimeout = Gap::MSEC_TO_GAP_DURATION_UNITS(CONN_SUP_TIMEOUT);
        gap_conn_params.slaveLatency                 = SLAVE_LATENCY;
        
    if (ble.updateConnectionParams(params->handle, &gap_conn_params) != BLE_ERROR_NONE) {
        //DEBUG("failed to update connection paramter\r\n");
    }
}
 
//void writtenCallback(const GattCharacteristicWriteCBParams *params){
void writtenCallback(const GattWriteCallbackParams *params){
    char acceleroRange = 0xFF;
    char gyroRange = 0xFF;
    
    if (params->handle == writeChar.getValueAttribute().getHandle()) {
        uint16_t len = params->len;
        
        if (len == 2) {
            uint8_t controller[2] = {0};
 
            ble.readCharacteristicValue(writeChar.getValueAttribute().getHandle(), writePayload, &len);
            memcpy(controller, writePayload, sizeof(controller));
            
            //pc.printf("write: %u, %u, %u\r\n", controller[0],controller[1],controller[2]);   
            
            switch(controller[0]){
                case 1:
                    acceleroRange = MPU6050_ACCELERO_RANGE_2G;
                    break;
                case 2:
                    acceleroRange = MPU6050_ACCELERO_RANGE_4G;
                    break;
                case 3:
                    acceleroRange = MPU6050_ACCELERO_RANGE_8G;
                    break;
                case 4:
                    acceleroRange = MPU6050_ACCELERO_RANGE_16G;
                    
                    break;
                default:
                    break;  
            }
            
            switch(controller[1]){
                case 1:
                    gyroRange = MPU6050_GYRO_RANGE_250;
                    break;
                case 2:
                    gyroRange = MPU6050_GYRO_RANGE_500; 
                    break;
                case 3:
                    gyroRange = MPU6050_GYRO_RANGE_1000;
                    break;
                case 4:
                    gyroRange = MPU6050_GYRO_RANGE_2000;
                    break;
                default:
                    break;
            }
            mpu.mpu_set_accel_fsr(acceleroRange);
            mpu.mpu_set_gyro_fsr(gyroRange);
            
        }
    }
}
 
void timeoutCallback(const Gap::TimeoutSource_t source)
{
    //DEBUG("TimeOut\n\r");
    //DEBUG("Restarting the advertising process\n\r");    
 
    ble.startAdvertising();
}
 
void periodicCallback(void)
{
    //oneSecondLed = !oneSecondLed; /* Do blinky on LED1 while we're waiting for BLE events */
 
    /* Note that the periodicCallback() executes in interrupt context, so it is safer to do
     * heavy-weight sensor polling from the main thread. */
    triggerSensorPolling = true;
}
 
/**************************************************************************/
/*!
    @brief  Program entry point
*/
/**************************************************************************/
int main(void)
{
    
    //#if DBG
    //    pc.printf("Start\n\r");
    //#endif
    /*
        #define MPU6050_ACCELERO_RANGE_2G   1
        #define MPU6050_ACCELERO_RANGE_4G   2
        #define MPU6050_ACCELERO_RANGE_8G   3
        #define MPU6050_ACCELERO_RANGE_16G  4
        
        #define MPU6050_GYRO_RANGE_250      1
        #define MPU6050_GYRO_RANGE_500      2
        #define MPU6050_GYRO_RANGE_1000     3
        #define MPU6050_GYRO_RANGE_2000     4
    */
 
    mpu.initialize();
    mpu.setAcceleroRange(MPU6050_ACCELERO_RANGE_8G);
    mpu.setGyroRange(MPU6050_GYRO_RANGE_1000);
    
    if( mpu.testConnection() ){
        //pc.printf("mpu test:OK\n\r");
    }else{
        //pc.printf("mpu test:NG\n\r");
    }
 
    float ticker_ms = (TICKER_INTERVAL / 100.0f);
    Ticker ticker;
    ticker.attach(periodicCallback, ticker_ms);//0.02f //.2f-sec
    
    ble.init();
    ble.onDisconnection(disconnectionCallback);
    ble.onConnection(connectionCallback);
    ble.onDataWritten(writtenCallback);
    ble.onTimeout(timeoutCallback);
 
    /* setup device name */
    ble.setDeviceName((const uint8_t *)DEVICENAME);
    
    /* setup advertising */
    ble.accumulateAdvertisingPayload(GapAdvertisingData::BREDR_NOT_SUPPORTED  | GapAdvertisingData::LE_GENERAL_DISCOVERABLE);
    ble.setAdvertisingType(GapAdvertisingParams::ADV_CONNECTABLE_UNDIRECTED);
    ble.accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LOCAL_NAME, (const uint8_t *)DEVICENAME, sizeof(DEVICENAME));
    ble.accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LIST_128BIT_SERVICE_IDS,
                                    (const uint8_t *)MPU6050_service_uuid, sizeof(MPU6050_service_uuid));
                                    //(const uint8_t *)MPU6050_adv_service_uuid, sizeof(MPU6050_adv_service_uuid));
 
    ble.setAdvertisingInterval(160); /* 100ms; in multiples of 0.625ms. */
    ble.startAdvertising();
 
    ble.addService(MPU6050Service);
 
    while(true) {
        if (triggerSensorPolling && ble.getGapState().connected) {
            triggerSensorPolling = false;
            updateValue();
        } else {
            ble.waitForEvent();
        }
    }
}
 
 
            
