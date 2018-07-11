#include "mbed.h"
#include "TMP102.h"
#include "BLE.h"

#define NEED_CONSOLE_OUTPUT 1 /* Set this if you need debug messages on the console;
                               * it will have an impact on code-size and power consumption. */

#if NEED_CONSOLE_OUTPUT
Serial  pc(USBTX, USBRX);
#define DEBUG(...) { pc.printf(__VA_ARGS__); }
#else
#define DEBUG(...) /* nothing */
#endif /* #if NEED_CONSOLE_OUTPUT */

const static char  DEVICE_NAME[] = "HRM1017_HTM";
static volatile bool  triggerSensorPolling = false;

BLEDevice  ble;
TMP102      healthThemometer(I2C_SDA0, I2C_SCL0, 0x90);  /* The TMP102 connected to our board */
//TMP102      healthThemometer(I2C_SDA1, I2C_SCL1, 0x90);  /* The TMP102 connected to our board */

/* LEDs for indication: */
DigitalOut  oneSecondLed(LED1);        /* LED1 is toggled every second. */
DigitalOut  advertisingStateLed(LED2); /* LED2 is on when we are advertising, otherwise off. */


/* Health Thermometer Service */ 
uint8_t             thermTempPayload[5] = { 0, 0, 0, 0, 0 };

GattCharacteristic  tempChar (GattCharacteristic::UUID_TEMPERATURE_MEASUREMENT_CHAR,
                                thermTempPayload, 5, 5,
                                GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_INDICATE);
/* Battery Level Service */
uint8_t            batt = 100;     /* Battery level */
uint8_t            read_batt = 0;  /* Variable to hold battery level reads */
GattCharacteristic battLevel ( GattCharacteristic::UUID_BATTERY_LEVEL_CHAR, 
                                 (uint8_t *)batt, 1, 1,
                                 GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY);
GattCharacteristic *htmChars[] = {&tempChar, };
GattCharacteristic *battChars[] = {&battLevel, };
GattService        htmService(GattService::UUID_HEALTH_THERMOMETER_SERVICE, htmChars, 
                                sizeof(htmChars) / sizeof(GattCharacteristic *));
GattService        battService(GattService::UUID_BATTERY_SERVICE, battChars,
                                sizeof(battChars) / sizeof(GattCharacteristic *));

uint16_t             uuid16_list[] = {GattService::UUID_HEALTH_THERMOMETER_SERVICE,
                                      GattService::UUID_BATTERY_SERVICE};

uint32_t quick_ieee11073_from_float(float temperature);
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
    DEBUG("Init done\r\n");
    ble.gap().onDisconnection(disconnectionCallback);
    ble.gap().onConnection(onConnectionCallback);

    ble.gap().getPreferredConnectionParams(&connectionParams);

    /* setup advertising */
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::BREDR_NOT_SUPPORTED | GapAdvertisingData::LE_GENERAL_DISCOVERABLE);
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LIST_16BIT_SERVICE_IDS, (uint8_t*)uuid16_list, sizeof(uuid16_list));
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::GENERIC_THERMOMETER);
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LOCAL_NAME, (uint8_t *)DEVICE_NAME, sizeof(DEVICE_NAME));
    ble.gap().setAdvertisingType(GapAdvertisingParams::ADV_CONNECTABLE_UNDIRECTED);
    ble.gap().setAdvertisingInterval(160); /* 100ms; in multiples of 0.625ms. */
    ble.gap().startAdvertising();
    advertisingStateLed = 1;
    DEBUG("Start Advertising\r\n");

    ble.gattServer().addService(htmService);
    ble.gattServer().addService(battService);
    DEBUG("Add Service\r\n");

    while (true) {
        if (triggerSensorPolling) {
            triggerSensorPolling = false;
            updateServiceValues();
        } else {
            ble.waitForEvent();
        }
    }
}
n
/**************************************************************************/
/*!
    @brief  Ticker callback to switch advertisingStateLed state
*/
/**************************************************************************/
void updateServiceValues(void)
{
      /* Decrement the battery level. */
      batt <=50 ? batt=100 : batt--;
      
      /* Update the temperature. Note that we need to convert to an ieee11073 format float. */
      float temperature = healthThemometer.read();
      DEBUG("temp:%f\r\n", temperature);
      uint32_t temp_ieee11073 = quick_ieee11073_from_float(temperature);
      memcpy(thermTempPayload+1, &temp_ieee11073, 4);
      ble.gattServer().write(tempChar.getValueAttribute().getHandle(), thermTempPayload, sizeof(thermTempPayload));  //Mod
      ble.gattServer().write(battLevel.getValueAttribute().getHandle(), (uint8_t *)&batt, sizeof(batt));             //Mod
}

/**
 * @brief A very quick conversion between a float temperature and 11073-20601 FLOAT-Type.
 * @param temperature The temperature as a float.
 * @return The temperature in 11073-20601 FLOAT-Type format.
 */
uint32_t quick_ieee11073_from_float(float temperature)
{
    uint8_t  exponent = 0xFF; //exponent is -1
    uint32_t mantissa = (uint32_t)(temperature*10);
    
    return ( ((uint32_t)exponent) << 24) | mantissa;
}
