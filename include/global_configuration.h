
#define THERMAL_SENSOR_PIN          4
#define THERMAL_SENSOR_PERIOD_MS    10000
#define THERMAL_FAN_THRESHOLD_TEMP  40
//#define VIRTUAL_POTENTIOMETER_PIN 46

////////////////////// VIRTUAL POTENTIOMETER //////////////////////
#define VIRTUAL_POTENTIOMETER_CS_PIN            36
#define VIRTUAL_POTENTIOMETER_INC_PIN           12
#define VIRTUAL_POTENTIOMETER_UD_PIN            35
///////////////////////////////////////////////////////////////////


////////////////////// BLE CONFIGURATION //////////////////////
#define SERVICE_UUID            "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID     "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define DEVICE_DISPLAY_NAME     "K25-ECU"
//////////////////////////////////////////////////////////////


////////////////////// CAN SPI PINS //////////////////////
#define CAN_CS_PIN      18
#define CAN_MISO        16   
#define CAN_MOSI        15  
#define CAN_SCK         17    
//////////////////////////////////////////////////////////


////////////////////// RELAY PINS //////////////////////
#define ALARM_RELAY_PIN             37
#define FAN_RELAY_PIN               38
#define RELAY_PIN_3                 39
#define RELAY_PIN_4                 40
#define IGNITION_SSR_RELAY_PIN      8
////////////////////////////////////////////////////////
