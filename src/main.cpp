#include <SPI.h>
#include <Arduino.h>
#include <mcp2515.h>
#include <X9C.h>
#include <com_manager.h>
#include <thermal_management.h>
#include <global_configuration.h>
#include <canbus_manager.h>

X9C throttlePotentiometer(VIRTUAL_POTENTIOMETER_CS_PIN, VIRTUAL_POTENTIOMETER_INC_PIN, VIRTUAL_POTENTIOMETER_UD_PIN); 
ThermalManagement thermalManagement(THERMAL_SENSOR_PIN);
ComManager comManager;
CanBusManager canBusManager;

static unsigned long lastUpdateTime = 0;
static unsigned long lastTempDataTime = 0;
void processThrottleMessage(simple_can_package &simplePackage)
{
  float throttlePercent = (simplePackage.package_value / 65535.0) * 100.0;
  if (throttlePercent > 100.0)
    throttlePercent = 100.0; // Yüzdeyi sınırla
  if (throttlePercent < 0.0)
    throttlePercent = 0.0; // Yüzdeyi sınırla

  if (millis() - lastUpdateTime >= 100)
  { // update once per second
    comManager.send_throttle_level(throttlePercent);
    lastUpdateTime = millis();
  }
  throttlePotentiometer.setPercentageValue(throttlePercent);
}


void setup()
{
  Serial.begin(115200); 
  digitalWrite(ARMING_SSR_RELAY_PIN, LOW); // Ensure all relays start off
  pinMode(ARMING_SSR_RELAY_PIN, OUTPUT);

  throttlePotentiometer.begin(); // Dijital potansiyometre başlatılıyor
  thermalManagement.initialize();
  comManager.initializeBLE();

}


void loop()
{

  if (millis() - lastTempDataTime >= 10000)
  { 
    float temperature = thermalManagement.readTemperature();
    lastTempDataTime = millis();
  }


  
  simple_can_package simplePackage = canBusManager.tick();
  if (simplePackage.package_type == CANBUS_PACKAGE_TYPE_THROTTLE_VALUE)
  {
    processThrottleMessage(simplePackage);
  }


  delay(5); // 5 ms bekle
}
