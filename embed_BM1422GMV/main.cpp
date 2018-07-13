#include "mbed.h"
#include "TMP102.h"
#include "BLE.h"
#include "BM1422GMV.h"

#define NEED_CONSOLE_OUTPUT 0 /* Set this if you need debug messages on the console;
                               * it will have an impact on code-size and power consumption. */
#define UUID_MAGNETIC_FLUX_DENSITY 0x2AA1
#define UUID_ENV_SENSING_SERVICE   0x181A



#if NEED_CONSOLE_OUTPUT
Serial  pc(USBTX, USBRX);
#define DEBUG(...) { pc.printf(__VA_ARGS__); }
#else
#define DEBUG(...) /* nothing */
#endif /* #if NEED_CONSOLE_OUTPUT */

const static char  DEVICE_NAME[] = "HRM1017_Mag";
static volatile bool  triggerSensorPolling = false;

BLEDevice  ble;

BM1422GMV magSensor(I2C_SDA1, I2C_SCL1);
//TMP102      healthThemometer(I2C_SDA1, I2C_SCL1, 0x90);  /* The TMP102 connected to our board */

/* LEDs for indication: */
lDigitalOut  oneSecondLed(LED1);        /* LED1 is toggled every second. */
DigitalOut  advertisingStateLed(LED2); /* LED2 is on when we are advertising, otherwise off. */

// Trigger signal pin
InterruptIn DRDY(D??);

DigitalInOut ADDR(D??);



uint8_t magTempPaylad[sizeof(float)*3] = {0,};

// MORE information @ https://www.bluetooth.com/specifications/gatt/characteristics
GattCharacteristic  magChar (UUID_MAGNETIC_FLUX_DENSITY,
			     magTempPayload, sizeof(float)*3, sizeof(float)*3,
			     GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_INDICATE);
/* Battery Level Service */

GattCharacteristic *geoMagChars = {&magChar, };
GattService magService(UUID_ENV_SENSING_SERVICE, geoMagChars, sizeof(geoMagChars) / sizeof(GattCharacteristic *));

uint16_t uuid16_list[] = {UUID_ENV_SENSING_SERVICE};

void updateServiceValues(void);

static Gap::ConnectionParams_t connectionParams;

void disconnectionCallback(const Gap::DisconnectionCallbackParams_t *params)    // Mod
{
  advertisingStateLed = 1;
    
  DEBUG("Disconnected handle %u, reason %u\r\n", params->handle, params->reason);
  DEBUG("Restarting the advertising process\r\n");
  ble.gap().startAdvertising();
}

void onConnectionCallback(const Gap::ConnectionCallbackParams_t *params)   //Mod
{
  advertisingStateLed = 0;

  DEBUG("connected. Got handle %u\r\n", params->handle);

  connectionParams.slaveLatency = 1;
  if (ble.gap().updateConnectionParams(params->handle, &connectionParams) != BLE_ERROR_NONE) {
    DEBUG("failed to update connection paramter\r\n");
  }
}

void periodicCallback(void)
{
  oneSecondLed = !oneSecondLed; /* Do blinky on LED1 while we're waiting for BLE events */
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
    
  /* Setup blinky led */
  oneSecondLed = 1;
  Ticker ticker;
  ticker.attach(periodicCallback, 1);
       
  DEBUG("Initialising the nRF51822\r\n");
  ble.init();
  ADDR.write(1);
  magSensor.init();
  DEBUG("Init done\r\n");
  ble.gap().onDisconnection(disconnectionCallback);
  ble.gap().onConnection(onConnectionCallback);

  ble.gap().getPreferredConnectionParams(&connectionParams);

  /* setup advertising */
  ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::BREDR_NOT_SUPPORTED | GapAdvertisingData::LE_GENERAL_DISCOVERABLE);
  ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LIST_128BIT_SERVICE_IDS, (uint8_t*)uuid16_list, sizeof(uuid16_list));
 // TODO:Check for COMPLETE_LIST_128BIT_SERVICE_IDS
 // TODO: Check for the GENERIC_THERMOMETER
  // ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::GENERIC_THERMOMETER);
  
  ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LOCAL_NAME, (uint8_t *)DEVICE_NAME, sizeof(DEVICE_NAME));
  ble.gap().setAdvertisingType(GapAdvertisingParams::ADV_CONNECTABLE_UNDIRECTED);
  ble.gap().setAdvertisingInterval(160); /* 100ms; in multiples of 0.625ms. */
  ble.gap().startAdvertising();
  advertisingStateLed = 1;
  // DEBUG("Start Advertising\r\n");

  ble.gattServer().addService(magService);
  // DEBUG("Add Service\r\n");

  DRDY.rise(&updateServiceValues);
  
  while (true) {
    ble.waitForEvent();
  }

}

/**************************************************************************/
/*!
  @brief  Ticker callback to switch advertisingStateLed state
*/
/**************************************************************************/
void updateServiceValues(void)
{
  /* Decrement the battery level. */
  magSensor.measure();
  float magData[3] = {magSensor.magX, magSensor.magY, magSensor.magZ};
  /* Update the temperature. Note that we need to convert to an ieee11073 format float. */
  // DEBUG("temp:%f\r\n", temperature);
  // uint32_t temp_ieee11073 = quick_ieee11073_from_float(temperature);
  memcpy(magTempPaylad+sizeof(float)*0, &magData[0], sizeof(magData[0]));
  memcpy(magTempPaylad+sizeof(float)*1, &magData[1], sizeof(magData[1]));
  memcpy(magTempPaylad+sizeof(float)*2, &magData[2], sizeof(magData[2]));

  ble.gattServer().write(magChar.getValueAttribute().getHandle(), magTempPaylad, sizeof(magTempPaylad))

}

