#ifndef ComManager_H
#define ComManager_H

#include <Arduino.h>
#include <BLECharacteristic.h>
#include <BLEServer.h>

class ComManager {
    public:
        ComManager();
        void initializeBLE();
        void send_throttle_level(float value);

    private:
        BLEServer *pServer = NULL;
        BLECharacteristic *pCharacteristic = NULL;
        void configure_characteristics();
        void start_advertising();
    };

#endif