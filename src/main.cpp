#include <SPI.h>
#include <Arduino.h>
#include <mcp2515.h>
#include <X9C.h>
#include <ble_manager.h>
#include <thermal_management.h>
#include <global_configuration.h>
#include <canbus_manager.h>
#include <watch_dog.h>
#include <fan_system.h>
#include <relays.h>
#include <siren_system.h>
#include <ignition_system.h>

X9C throttlePotentiometer(VIRTUAL_POTENTIOMETER_CS_PIN, VIRTUAL_POTENTIOMETER_INC_PIN, VIRTUAL_POTENTIOMETER_UD_PIN);
ThermalManagement thermalManagement(THERMAL_SENSOR_PIN);
BleManager bleManager;
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
    bleManager.send_throttle_level(throttlePercent);
    lastUpdateTime = millis();
  }
  throttlePotentiometer.setPercentageValue(throttlePercent);
}

void check_temp()
{
  if (millis() - lastTempDataTime >= THERMAL_SENSOR_PERIOD_MS)
  {
    float temperature = thermalManagement.readTemperature();
    bleManager.send_temperature_value(temperature);
    lastTempDataTime = millis();
    if (temperature > THERMAL_FAN_THRESHOLD_TEMP)
    {
      fan_system_enable_fan();
    }
    else
    {
      fan_system_disable_fan();
    }
  }
}
void setup()
{
  Serial.begin(115200);
  relays_initialize();
  canBusManager.initialize();
  throttlePotentiometer.initialize();
  thermalManagement.initialize();
  bleManager.initialize();
  watch_dog_initialize();
  siren_system_startup_melody();
  ignition_system_on();

}

void loop()
{

  check_temp();

  simple_can_package simplePackage = canBusManager.tick();
  if (simplePackage.package_type == CANBUS_PACKAGE_TYPE_THROTTLE_VALUE)
  {
    processThrottleMessage(simplePackage);
    watch_dog_update_throttle_data_received();
  }

  delayMicroseconds(100);
}
