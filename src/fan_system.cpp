#include <esp32-hal-gpio.h>
#include <global_configuration.h>
void fan_system_enable_fan()
{
    digitalWrite(FAN_RELAY_PIN, LOW);
}

void fan_system_disable_fan()
{
    digitalWrite(FAN_RELAY_PIN, HIGH);
}