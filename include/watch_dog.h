
#include <esp32-hal-timer.h>
#ifndef WATCH_DOG_H
#define WATCH_DOG_H

void IRAM_ATTR  watch_dog_onTimer();
void watch_dog_initialize();
void watch_dog_update_throttle_data_received();


#endif