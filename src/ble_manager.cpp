#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>
#include <ble_manager.h>
#include <global_configuration.h>

BleManager::BleManager()
{
}

bool deviceConnected = false;
float throttleValue = 0.0;
bool oldDeviceConnected = false;

class ServerCallbacks : public BLEServerCallbacks
{
    void onConnect(BLEServer *pServer)
    {
        deviceConnected = true;
    };

    void onDisconnect(BLEServer *pServer)
    {
        deviceConnected = false;
    }
};

void BleManager::initialize()

{
    BLEDevice::init(DEVICE_DISPLAY_NAME);
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new ServerCallbacks());
    configure_characteristics();

    start_advertising();

    Serial.println("BLE server ready, waiting for connections...");
}

void BleManager::start_advertising()
{
    BLEDevice::setMTU(128);                             // Set the MTU size to 128 bytes
    BLEDevice::getAdvertising()->setMinPreferred(0x30); // 60 ms
    BLEDevice::getAdvertising()->setMaxPreferred(0x50); // 100 ms
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    BLEDevice::startAdvertising();
}

void BleManager::configure_characteristics()
{
    float initialFloat = 0.0f;

    BLEService *pService = pServer->createService(SERVICE_UUID);
    pThrusterCharacteristic = pService->createCharacteristic(
        BLE_CHARACTERISTIC_THROTTLE_UUID,
        BLECharacteristic::PROPERTY_READ |
            BLECharacteristic::PROPERTY_WRITE |
            BLECharacteristic::PROPERTY_NOTIFY);
    pThrusterCharacteristic->addDescriptor(new BLE2902());
    pThrusterCharacteristic->setValue((uint8_t *)&initialFloat, sizeof(float));

    pTemperatureCharacteristic = pService->createCharacteristic(
        BLE_CHARACTERISTIC_TEMPERATURE_UUID,
        BLECharacteristic::PROPERTY_READ |
            BLECharacteristic::PROPERTY_WRITE |
            BLECharacteristic::PROPERTY_NOTIFY);
    pTemperatureCharacteristic->addDescriptor(new BLE2902());
    pTemperatureCharacteristic->setValue((uint8_t *)&initialFloat, sizeof(float));

    pService->start();
}


void BleManager::send_throttle_level(float value)
{

    if (deviceConnected)
    {
        char txString[8];
        dtostrf(value, 2, 1, txString);
        pThrusterCharacteristic->setValue(txString);
        pThrusterCharacteristic->notify();
    }
    handle_connection_state();
}

void BleManager::send_temperature_value(float value)
{
    if (deviceConnected)
    {
        char txString[8];
        dtostrf(value, 2, 1, txString);
        pTemperatureCharacteristic->setValue(txString);
        pTemperatureCharacteristic->notify();
    }
    handle_connection_state();
}

void BleManager::handle_connection_state()
{
    // Handle connection state changes
    if (!deviceConnected && oldDeviceConnected)
    {
        delay(500);                  // Give the Bluetooth stack time to get ready
        pServer->startAdvertising(); // Restart advertising
        Serial.println("BLE advertising restarted");
        oldDeviceConnected = deviceConnected;
    }

    if (deviceConnected && !oldDeviceConnected)
    {
        oldDeviceConnected = deviceConnected;
    }
}
