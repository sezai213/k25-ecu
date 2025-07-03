#include <global_configuration.h>
#include <esp32-hal-gpio.h>

void ignition_system_on()
{
    digitalWrite(IGNITION_SSR_RELAY_PIN, HIGH);
}
void ignition_system_off()
{
    digitalWrite(IGNITION_SSR_RELAY_PIN, LOW);
}
