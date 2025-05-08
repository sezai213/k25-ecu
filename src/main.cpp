#include <mcp_can.h>
#include <SPI.h>

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

#define CAN_CS_PIN 5 // MCP2515 CS pini
#define VIRTUAL_POTENTIOMETER_PIN   15
MCP_CAN CAN(CAN_CS_PIN); // MCP2515 nesnesi oluşturuluyor



BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;
bool deviceConnected = false;
float throttleValue = 0.0;
bool oldDeviceConnected = false;

class ServerCallbacks: public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    deviceConnected = true;
  };

  void onDisconnect(BLEServer* pServer) {
    deviceConnected = false;
  }
};

void setupBLE() {
  BLEDevice::init("ThrottleController");
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new ServerCallbacks());
  
  BLEService *pService = pServer->createService(SERVICE_UUID);
  
  pCharacteristic = pService->createCharacteristic(
            CHARACTERISTIC_UUID,
            BLECharacteristic::PROPERTY_READ   |
            BLECharacteristic::PROPERTY_WRITE  |
            BLECharacteristic::PROPERTY_NOTIFY
          );

  pCharacteristic->addDescriptor(new BLE2902());
  pCharacteristic->setValue("0.0");
  
  pService->start();
  
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  
  Serial.println("BLE server ready, waiting for connections...");
}

void updateThrottleLevel(float value) {
  throttleValue = value;
  
  if (deviceConnected) {
  char txString[8];
  dtostrf(throttleValue, 2, 1, txString);
  pCharacteristic->setValue(txString);
  pCharacteristic->notify();
  }
  
  // Handle connection state changes
  if (!deviceConnected && oldDeviceConnected) {
  delay(500); // Give the Bluetooth stack time to get ready
  pServer->startAdvertising(); // Restart advertising
  Serial.println("BLE advertising restarted");
  oldDeviceConnected = deviceConnected;
  }
  
  if (deviceConnected && !oldDeviceConnected) {
  oldDeviceConnected = deviceConnected;
  }
}


void setup()
{
  setupBLE(); // BLE ayarları yapılıyor
  Serial.begin(115200); // Seri iletişim başlatılıyor
  Serial.println("MCP2515 CAN Bus Test");
  pinMode(VIRTUAL_POTENTIOMETER_PIN, OUTPUT);
  // CAN Bus başlatılıyor
  if (CAN.begin(MCP_ANY, CAN_500KBPS, MCP_8MHZ) == CAN_OK)
  {
    Serial.println("MCP2515 başarıyla başlatıldı!");
  }
  else
  {
    Serial.println("MCP2515 başlatılamadı!");
  }

  CAN.setMode(MCP_NORMAL); // Normal moda geçiş yapılıyor
}
static unsigned long lastUpdateTime = 0;
void loop()
{

  // Generate random throttle value between 0-100
  float randomThrottle = random(0, 101);

  if (CAN.checkReceive() == CAN_MSGAVAIL)
  { // Yeni bir mesaj var mı?
    unsigned long rxId;
    byte len = 0;
    byte rxBuf[8];

    CAN.readMsgBuf(&rxId, &len, rxBuf); // Mesajı oku



    if (rxId == 0x0B4 && len == 8)
    {                                               // CAN ID: 0x100 ve veri uzunluğu: 1
      int throttleRaw = (rxBuf[0] << 8) | rxBuf[1]; // Big-endian

      // 0 - 65535 arası değeri yüzdeye çevir
      float throttlePercent = (throttleRaw / 65535.0) * 100.0;
    
      if (millis() - lastUpdateTime >= 1000) { // update once per second
        updateThrottleLevel(throttlePercent);
        lastUpdateTime = millis();
      }
      Serial.print(" → Yüzde: ");
      Serial.print(throttlePercent);
      Serial.println(" %");

      int pwmValue = (int)(57 + (throttlePercent / 100.0) * 133);

      // Sinyali üret
      analogWrite(VIRTUAL_POTENTIOMETER_PIN, pwmValue);

    }
  }
  else
  {
    //Serial.println("Yeni mesaj yok");
  }
  //delay(2); // 100 ms bekle
  // MCP2515'e mesaj gönder
}