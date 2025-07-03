#include <global_configuration.h>
#include <esp32-hal-gpio.h>

void relays_initialize(){
    pinMode(ALARM_RELAY_PIN, OUTPUT);
    pinMode(FAN_RELAY_PIN, OUTPUT);
    pinMode(RELAY_PIN_3, OUTPUT);
    pinMode(RELAY_PIN_4, OUTPUT);
    pinMode(IGNITION_SSR_RELAY_PIN, OUTPUT);
    
    // Ensure all relays start off
    digitalWrite(ALARM_RELAY_PIN, HIGH); 
    digitalWrite(FAN_RELAY_PIN, HIGH); 
    digitalWrite(RELAY_PIN_3, HIGH); 
    digitalWrite(RELAY_PIN_4, HIGH); 
    digitalWrite(IGNITION_SSR_RELAY_PIN, HIGH); 
  
  }