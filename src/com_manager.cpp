#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>
#include <com_manager.h>
#include <global_configuration.h>

ComManager::ComManager()
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

void ComManager::initialize()

{
    BLEDevice::init(DEVICE_DISPLAY_NAME);
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new ServerCallbacks());
    configure_characteristics();

    start_advertising();

    Serial.println("BLE server ready, waiting for connections...");
}

void ComManager::start_advertising()
{
    BLEDevice::setMTU(128);                             // Set the MTU size to 128 bytes
    BLEDevice::getAdvertising()->setMinPreferred(0x30); // 60 ms
    BLEDevice::getAdvertising()->setMaxPreferred(0x50); // 100 ms
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    BLEDevice::startAdvertising();
}

void ComManager::configure_characteristics()
{
    BLEService *pService = pServer->createService(SERVICE_UUID);
    pCharacteristic = pService->createCharacteristic(
                                                        CHARACTERISTIC_UUID,
                                                        BLECharacteristic::PROPERTY_READ |
                                                        BLECharacteristic::PROPERTY_WRITE |
                                                        BLECharacteristic::PROPERTY_NOTIFY);
    pCharacteristic->addDescriptor(new BLE2902());
    pCharacteristic->setValue("0.0");
    pService->start();
}
void ComManager::send_throttle_level(float value)
{
    throttleValue = value;

    if (deviceConnected)
    {
        char txString[8];
        dtostrf(throttleValue, 2, 1, txString);
        pCharacteristic->setValue(txString);
        pCharacteristic->notify();
    }

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