#include <esp32-hal-timer.h>
#include <X9C.h>

#define THROTTLE_TIMEOUT_MS 2000 // 2 saniye zaman aşımı
#define TIMER_INTERVAL_US 100000 // 100ms = 100000us

volatile unsigned long lastThrottleUpdateTime = 0;
extern X9C throttlePotentiometer;

hw_timer_t *timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

void IRAM_ATTR watch_dog_onTimer()
{
    unsigned long now = millis();
    portENTER_CRITICAL_ISR(&timerMux);
    if ((now - lastThrottleUpdateTime) > THROTTLE_TIMEOUT_MS)
    {
        throttlePotentiometer.reset();
    }
    portEXIT_CRITICAL_ISR(&timerMux);
}

void watch_dog_initialize()
{
    lastThrottleUpdateTime = millis() + 1000;
    timer = timerBegin(0, 80, true); // 80 MHz / 80 = 1 MHz => 1 tick = 1 microsecond
    timerAttachInterrupt(timer, &watch_dog_onTimer, true);
    timerAlarmWrite(timer, TIMER_INTERVAL_US, true); // 100ms interrupt
    timerAlarmEnable(timer);                         // Başlat
}

void watch_dog_update_throttle_data_received()
{
    portENTER_CRITICAL(&timerMux);
    lastThrottleUpdateTime = millis();
    portEXIT_CRITICAL(&timerMux);
}
