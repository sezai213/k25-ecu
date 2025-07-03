#ifndef BleManager_H
#define BleManager_H

#include <Arduino.h>
#include <BLECharacteristic.h>
#include <BLEServer.h>

class BleManager {
    public:
        BleManager();
        void initialize();
        void send_throttle_level(float value);
        void send_temperature_value(float value);

    private:
        BLEServer *pServer = NULL;
        BLECharacteristic *pThrusterCharacteristic = NULL;
        BLECharacteristic *pTemperatureCharacteristic = NULL;
        void configure_characteristics();
        void start_advertising();
        void handle_connection_state();
    };

#endif