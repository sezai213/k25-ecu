#include <esp32-hal-gpio.h>
#include <global_configuration.h>
#include <siren_system.h>

void siren_system_startup_melody()
{
    digitalWrite(ALARM_RELAY_PIN, LOW);
    delay(SIREN_SYSTEM_STARTUP_DELAY);
    digitalWrite(ALARM_RELAY_PIN, HIGH);
    delay(SIREN_SYSTEM_STARTUP_DELAY * 2);
    digitalWrite(ALARM_RELAY_PIN, LOW);
    delay(SIREN_SYSTEM_STARTUP_DELAY);
    digitalWrite(ALARM_RELAY_PIN, HIGH);
    delay(SIREN_SYSTEM_STARTUP_DELAY * 2);
    digitalWrite(ALARM_RELAY_PIN, LOW);
    delay(SIREN_SYSTEM_STARTUP_DELAY);
    digitalWrite(ALARM_RELAY_PIN, HIGH);
}
void siren_system_panic_melody()
{
    digitalWrite(ALARM_RELAY_PIN, LOW);
    delay(SIREN_SYSTEM_PANIC_DELAY);
    digitalWrite(ALARM_RELAY_PIN, HIGH);
}