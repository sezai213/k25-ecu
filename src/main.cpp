
#include <SPI.h>
#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>
#include <mcp2515.h>
#include <X9C.h>

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"


#define VIRTUAL_POTENTIOMETER_PIN   46



// #define CAN_CS_PIN 7
// #define CAN_MISO  13
// #define CAN_MOSI  11
// #define CAN_SCK   12

#define CAN_CS_PIN 18 //5
#define CAN_MISO   16 //
#define CAN_MOSI   15 //
#define CAN_SCK    17 //
struct can_frame canMsg;
MCP2515* canController;



  // Relay pin definitions

  #define ALARM_RELAY_PIN 37
  #define FAN_RELAY_PIN 38
  #define RELAY_PIN_5 39
  #define RELAY_PIN_6 40




#include "DHT.h"

#define DHTPIN 4          // Sensörün bağlı olduğu GPIO pini
#define DHTTYPE DHT22     // Sensör tipi (DHT11 yerine DHT22 kullanıyoruz)

DHT dht(DHTPIN, DHTTYPE);




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
//RELAY_PIN_1, RELAY_PIN_2, 
  const int relayPins[] = {ALARM_RELAY_PIN, FAN_RELAY_PIN, RELAY_PIN_5, RELAY_PIN_6};
  const int relayCount = sizeof(relayPins) / sizeof(relayPins[0]);

  // Variables for relay control
  unsigned long relayStartTime = 0;
  int currentRelay = -1;
  const unsigned long relayActivationTime = 200; // 2 seconds in milliseconds


X9C digiPot(36, 46,35 ); // CS, INC, U/D pinleri




void setup()
{

  dht.begin();
  // Initialize relay pins
  for (int i = 0; i < relayCount; i++) {
    pinMode(relayPins[i], OUTPUT);
    digitalWrite(relayPins[i], HIGH); // Ensure all relays start off
  }

  Serial.begin(115200); // Seri iletişim başlatılıyor
    while (!Serial) {
    delay(10); // USB bağlantısı kurulana kadar bekle
  }
  SPI.begin(CAN_SCK, CAN_MISO, CAN_MOSI, CAN_CS_PIN);  // SPI pinlerini başlat
  setupBLE(); // BLE ayarları yapılıyor

  Serial.println("MCP2515 CAN Bus Test");
  pinMode(VIRTUAL_POTENTIOMETER_PIN, OUTPUT);
  // SPI pinlerini belirterek başlat (ESP32-S3 destekler)
 
  canController = new MCP2515(CAN_CS_PIN);
  canController->reset();
  canController->setBitrate(CAN_500KBPS, MCP_8MHZ); 
  canController->setNormalMode();
  
  
  Serial.println("------- CAN Read ----------");
  Serial.println("ID  DLC   DATA");

 Serial.println("CAN Başlatıldı!");
  analogWrite(VIRTUAL_POTENTIOMETER_PIN, 128);



//  digiPot.begin();

//   digiPot.setValue(20);  // 0 → 20 arası arttır
//   delay(1000);

//   digiPot.setValue(50);  // 20 → 50 arası arttır
//   delay(1000);

//   digiPot.setValue(30);  // 50 → 30 arası azalt
//   delay(1000);

//   digiPot.setValue(0);   // Tam reset

}
static unsigned long lastUpdateTime = 0;
uint8_t didWork= 0;

  //uint32_t rxId;
    //uint8_t len = 0;
   // uint8_t rxBuf[64];
void loop()
{



  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  // Hata kontrolü
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println(F("DHT sensörü okunamadı!"));
    return;
  }

  Serial.print(F("Nem: "));
  Serial.print(humidity);
  Serial.print(F(" % | Sıcaklık: "));
  Serial.print(temperature);
  Serial.println(F(" °C"));


if(didWork == 0) {
  didWork = 1;

  digitalWrite(relayPins[1], LOW);
delay(3000);
  digitalWrite(relayPins[1], HIGH);

  digitalWrite(relayPins[0], LOW);
      delay(relayActivationTime);
        digitalWrite(relayPins[0], HIGH);
   delay(relayActivationTime*2);
  digitalWrite(relayPins[0], LOW);
   delay(relayActivationTime);
      digitalWrite(relayPins[0], HIGH);
   delay(relayActivationTime*2);
  digitalWrite(relayPins[0], LOW);
   delay(relayActivationTime);
      digitalWrite(relayPins[0], HIGH);
    
}



 if (canController->readMessage(&canMsg) == MCP2515::ERROR_OK) {
 
    if (canMsg.can_id == 0x0B4 && canMsg.can_dlc == 8){
     int throttleRaw = (canMsg.data[0] << 8) | canMsg.data[1]; // Big-endian
      //throttleRaw=1000;
      // 0 - 65535 arası değeri yüzdeye çevir
      float throttlePercent = (throttleRaw / 65535.0) * 100.0;
    if(throttlePercent>100.0) throttlePercent = 100.0; // Yüzdeyi sınırla
      if(throttlePercent<0.0) throttlePercent = 0.0; // Yüzdeyi sınırla

      if (millis() - lastUpdateTime >= 1000) { // update once per second
        updateThrottleLevel(throttlePercent);
        lastUpdateTime = millis();
      }

      int pwmValue = (int)( (throttlePercent / 100.0) * 255);
      Serial.print("Raw: ");
      Serial.print(throttleRaw);
      Serial.print(" / PWM: ");
      Serial.print(pwmValue);
      Serial.print(" / Yüzde: ");
      Serial.print(throttlePercent);
      Serial.println("%");

    
if (pwmValue > 255) pwmValue = 255;
if (pwmValue < 0) pwmValue = 0;
      // Sinyali üret
      analogWrite(VIRTUAL_POTENTIOMETER_PIN, pwmValue);
      delay(100);

    }
//
  }

  
//   if ( CAN.checkReceive() == CAN_MSGAVAIL)
//   { // Yeni bir mesaj var mı?
  

//     CAN.readMsgBuf(&rxId, &len, rxBuf); // Mesajı oku

//     if (rxId == 0x0B4 && len == 8)
//     {                                               // CAN ID: 0x100 ve veri uzunluğu: 1
//       int throttleRaw = (rxBuf[0] << 8) | rxBuf[1]; // Big-endian
//       //throttleRaw=1000;
//       // 0 - 65535 arası değeri yüzdeye çevir
//       float throttlePercent = (throttleRaw / 65535.0) * 100.0;
//     if(throttlePercent>100.0) throttlePercent = 100.0; // Yüzdeyi sınırla
//       if(throttlePercent<0.0) throttlePercent = 0.0; // Yüzdeyi sınırla

//       if (millis() - lastUpdateTime >= 1000) { // update once per second
//         //updateThrottleLevel(throttlePercent);
//         lastUpdateTime = millis();
//       }
//       Serial.print(" → Yüzde: ");
//       Serial.print(throttlePercent);
//       Serial.println(" %");

    
//       int pwmValue = (int)( (throttlePercent / 100.0) * 255);
//       Serial.print("PWM Değeri: ");
//       Serial.println(pwmValue);
// if (pwmValue > 255) pwmValue = 255;
// if (pwmValue < 0) pwmValue = 0;
//       // Sinyali üret
//       analogWrite(VIRTUAL_POTENTIOMETER_PIN, pwmValue);
//       Serial.print("PWM Send:");
//       delay(100);
//     }
//   }
//   else
//   {
//     //Serial.println("Yeni mesaj yok");
//   }
  
  //Serial.println("PWM Write: 128");
  delay(100); // 100 ms bekle
 
}