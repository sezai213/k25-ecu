#include <SPI.h>
#include <Arduino.h>
#include <mcp2515.h>
#include <X9C.h>
#include <com_manager.h>
#include <thermal_management.h>
#include <global_configuration.h>
#include <canbus_manager.h>

X9C digiPot(36, 12, 35); // CS, INC, U/D pinleri
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

  int potValue = (int)((throttlePercent / 100.0) * 30);

  if (potValue > 30)
    potValue = 30;
  if (potValue < 0)
    potValue = 0;
  // Sinyali üret
  digiPot.setValue(potValue); // Dijital potansiyometreye throttle değerini gönder
}


void setup()
{
  Serial.begin(115200); 
  digitalWrite(ARMING_SSR_RELAY_PIN, LOW); // Ensure all relays start off
  pinMode(ARMING_SSR_RELAY_PIN, OUTPUT);

  digiPot.begin(); // Dijital potansiyometre başlatılıyor
  digiPot.reset(); // Dijital potansiyometreye throttle değerini gönder

  thermalManagement.initialize();
  comManager.initializeBLE();

  pinMode(VIRTUAL_POTENTIOMETER_PIN, OUTPUT);
  analogWrite(VIRTUAL_POTENTIOMETER_PIN, 128);
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
