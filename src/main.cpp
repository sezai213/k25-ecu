
#include <SPI.h>
#include <Arduino.h>

#include <mcp2515.h>
#include <X9C.h>
#include <com_manager.h>


#define VIRTUAL_POTENTIOMETER_PIN 46

// #define CAN_CS_PIN 7
// #define CAN_MISO  13
// #define CAN_MOSI  11
// #define CAN_SCK   12

#define CAN_CS_PIN 18 // 5
#define CAN_MISO 16   //
#define CAN_MOSI 15   //
#define CAN_SCK 17    //
struct can_frame canMsg;
struct can_frame response;
MCP2515 *canController;

// Relay pin definitions

#define ALARM_RELAY_PIN 37
#define FAN_RELAY_PIN 38
#define HEADLIGHT_RELAY_PIN 39
#define RELAY_PIN_6 40

#define ARMING_SSR_RELAY_PIN 8

#include "DHT.h"

#define DHTPIN 4      // Sensörün bağlı olduğu GPIO pini
#define DHTTYPE DHT22 // Sensör tipi (DHT11 yerine DHT22 kullanıyoruz)

DHT dht(DHTPIN, DHTTYPE);


// RELAY_PIN_1, RELAY_PIN_2,
const int relayPins[] = {ALARM_RELAY_PIN, FAN_RELAY_PIN, HEADLIGHT_RELAY_PIN, RELAY_PIN_6};
const int relayCount = sizeof(relayPins) / sizeof(relayPins[0]);

// Variables for relay control
unsigned long relayStartTime = 0;
int currentRelay = -1;
const unsigned long relayActivationTime = 200; // 2 seconds in milliseconds

X9C digiPot(36, 12, 35); // CS, INC, U/D pinleri
ComManager comManager;
void setup()
{
  
  digitalWrite(ARMING_SSR_RELAY_PIN, LOW); // Ensure all relays start off
  pinMode(ARMING_SSR_RELAY_PIN, OUTPUT);

  digiPot.begin(); // Dijital potansiyometre başlatılıyor
  digiPot.reset(); // Dijital potansiyometreye throttle değerini gönder
  dht.begin();
  // Initialize relay pins
  for (int i = 0; i < relayCount; i++)
  {
    pinMode(relayPins[i], OUTPUT);
    digitalWrite(relayPins[i], HIGH); // Ensure all relays start off
  }

  Serial.begin(115200); // Seri iletişim başlatılıyor
  while (!Serial)
  {
    delay(10); // USB bağlantısı kurulana kadar bekle
  }
  SPI.begin(CAN_SCK, CAN_MISO, CAN_MOSI, CAN_CS_PIN); // SPI pinlerini başlat
  //setupBLE();                                         // BLE ayarları yapılıyor
  comManager.initializeBLE();
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
uint8_t didWork = 0;

// uint32_t rxId;
// uint8_t len = 0;
// uint8_t rxBuf[64];
uint8_t testVal = 0;
static bool increasing = true;
 float throttlePercent = 0.0; // Throttle yüzdesi
void loop()
{

  if (millis() - lastUpdateTime >= 100)
  { // update once per second
    int throttlePercentR = std::rand() % 101; // 0–100 arası
    comManager.send_throttle_level(throttlePercentR);
    lastUpdateTime = millis();
  }


  // digitalWrite(ARMING_SSR_RELAY_PIN, HIGH); // Ensure all relays start off
  // delay(1000);
  // digitalWrite(ARMING_SSR_RELAY_PIN, LOW); // Ensure all relays start off
  // delay(1000);

  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  // Hata kontrolü
  if (isnan(humidity) || isnan(temperature))
  {
    Serial.println(F("DHT sensörü okunamadı!"));
    return;
  }

  if (didWork == 0)
  {
    didWork = 1;

    digitalWrite(relayPins[1], LOW);
    delay(3000);
    digitalWrite(relayPins[1], HIGH);

    digitalWrite(relayPins[0], LOW);
    delay(relayActivationTime);
    digitalWrite(relayPins[0], HIGH);
    delay(relayActivationTime * 2);
    digitalWrite(relayPins[0], LOW);
    delay(relayActivationTime);
    digitalWrite(relayPins[0], HIGH);
    delay(relayActivationTime * 2);
    digitalWrite(relayPins[0], LOW);
    delay(relayActivationTime);
    digitalWrite(relayPins[0], HIGH);

    digitalWrite(HEADLIGHT_RELAY_PIN, LOW);
    delay(3000);
    digitalWrite(HEADLIGHT_RELAY_PIN, HIGH);
  }

  

  if (canController->readMessage(&canMsg) == MCP2515::ERROR_OK)
  {



if (canMsg.can_id == 0x7DF && canMsg.data[0] == 0x02 && canMsg.data[1] == 0x01 && canMsg.data[2] == 0x00) {
  Serial.println("PID 0x00 destek sorgusu alındı");

  // 0x00 PID için desteklenen PID'leri bildir
  // Cevap: 06 41 00 BE 1F A8 13 00 00
  // Bu örnekte:
  // - BE 1F A8 13: PID 0x01 - 0x20 arası için destek bitmask'ı (örnek)
  // - 0x41: mode 1 response
  // - 0x00: PID 0x00

response.can_id = 0x7E8;
response.can_dlc = 8;
response.data[0] = 0x06;  // 6 byte data
response.data[1] = 0x41;  // Response to Mode 01
response.data[2] = 0x00;  // Response to PID 0x00 (which PIDs are supported)
response.data[3] = 0xBE;  // Bits 1–8  (0x01–0x08)
response.data[4] = 0x1F;  // Bits 9–16 (0x09–0x10)
response.data[5] = 0xA8;  // Bits 17–24 (0x11–0x18): PID 0x0C and 0x0D (bits 4, 3)
response.data[6] = 0x13;  // Bits 25–32 (0x19–0x20): PID 0x05 (bit 4)
response.data[7] = 0x00;


  MCP2515::ERROR status = canController->sendMessage(&response);
  if (status != MCP2515::ERROR_OK) {
    Serial.print("CAN gönderme hatası: ");
    switch (status) {
      case MCP2515::ERROR_FAIL:
        Serial.println("FAIL - Genel hata");
        break;
      case MCP2515::ERROR_ALLTXBUSY:
        Serial.println("ALLTXBUSY - Tüm TX tamponları dolu");
        break;
      case MCP2515::ERROR_FAILINIT:
        Serial.println("FAILINIT - Başlatma hatası");
        break;
      default:
        Serial.println("Bilinmeyen hata");
        break;
    }
  } else {

    Serial.println("0x7DF destek listesi gönderildi");
  }

}




if (canMsg.can_id == 0x7DF && canMsg.data[0] == 0x02 && canMsg.data[1] == 0x01 && canMsg.data[2] == 0x10) {


response.can_id  = 0x7E8;       // ECU yanıt ID'si
response.can_dlc = 8;
response.data[0] = 0x04;        // 4 byte veri var
response.data[1] = 0x41;        // Mode 01 yanıtı
response.data[2] = 0x10;        // PID 0x10 (MAF)
response.data[3] = 0x07;        // A = upper byte (2000 = 0x07D0)
response.data[4] = 0xD0;        // B = lower byte
response.data[5] = 0x00;
response.data[6] = 0x00;
response.data[7] = 0x00;

canController->sendMessage(&response);
Serial.println("MAF yanıtı gönderildi (20.00 g/s)");
}

if (canMsg.can_id == 0x7DF && canMsg.data[0] == 0x02 && canMsg.data[1] == 0x01 && canMsg.data[2] == 0x0D) {


uint8_t speedKmh = 88;  // Göndermek istediğin hız


response.can_id = 0x7E8;
response.can_dlc = 8;
response.data[0] = 0x03;    // 3 byte veri
response.data[1] = 0x41;    // Mode 01 yanıtı
response.data[2] = 0x0D;    // PID 0x0D → Hız
response.data[3] = speedKmh;
response.data[4] = 0x00;
response.data[5] = 0x00;
response.data[6] = 0x00;
response.data[7] = 0x00;

canController->sendMessage(&response);
Serial.print("Hız yanıtı gönderildi: ");
Serial.print(speedKmh);
Serial.println(" km/h");
}





  Serial.print("CAN ID: 0x");
    Serial.print(canMsg.can_id, HEX);
    Serial.print(" DLC: ");
    Serial.print(canMsg.can_dlc);
    Serial.print(" Data: ");

    for (int i = 0; i < canMsg.can_dlc; i++) {
      Serial.print(canMsg.data[i], HEX);
      Serial.print(" ");
    }

    Serial.println();

 if (canMsg.can_id == 0x7DF && canMsg.data[1] == 0x01 && canMsg.data[2] == 0x0C) {
      Serial.println("RPM isteği alındı");


      // Yanıt oluştur: 04 41 0C A B 00 00 00

      // Yanıtı 0x7E8 ID’si ile gönder

      // Scale throttlePercent (0-100) to RPM range (0-8000)
      uint16_t rpm = (uint16_t)(throttlePercent * 60); // 100% throttle = 8000 RPM
      uint16_t val = rpm * 4;
      uint8_t A = (val >> 8) & 0xFF;
      uint8_t B = val & 0xFF;

  
      response.can_id = 0x7E8;
      response.can_dlc = 8;
      response.data[0] = 0x04;
      response.data[1] = 0x41;
      response.data[2] = 0x0C;
      response.data[3] = A;
      response.data[4] = B;
      response.data[5] = 0x00;
      response.data[6] = 0x00;
      response.data[7] = 0x00;

      canController->sendMessage(&response);
      Serial.println("RPM yanıtı gönderildi");
    }

 if (canMsg.can_id == 0x7DF && canMsg.data[1] == 0x01 && canMsg.data[2] == 0x05) {
      Serial.println("Sıcaklık isteği alındı");


      // Yanıt oluştur: 04 41 0C A B 00 00 00

      // Yanıtı 0x7E8 ID’si ile gönder

uint8_t tempByte = (uint8_t)(temperature + 40);  // OBD-II’ye uygun format
  

      response.can_id = 0x7E8;
response.can_dlc = 8;
response.data[0] = 0x03;  // 3 byte veri
response.data[1] = 0x41;  // Mode 01 yanıtı
response.data[2] = 0x05;  // PID 0x05 → Motor sıcaklığı
response.data[3] = tempByte;   // 90°C + 40 = 130 → 0x82
response.data[4] = 0x00;
response.data[5] = 0x00;
response.data[6] = 0x00;
response.data[7] = 0x00;

      canController->sendMessage(&response);
      Serial.println("Sicaklik yanıtı gönderildi");
    }

    if (canMsg.can_id == 0x0B4 && canMsg.can_dlc == 8)
    {
      int throttleRaw = (canMsg.data[0] << 8) | canMsg.data[1]; // Big-endian
      // throttleRaw=1000;
      //  0 - 65535 arası değeri yüzdeye çevir
     throttlePercent = (throttleRaw / 65535.0) * 100.0;
      Serial.print("Throttle Raw: ");
      Serial.print(throttleRaw);
      if (throttlePercent > 100.0)
        throttlePercent = 100.0; // Yüzdeyi sınırla
      if (throttlePercent < 0.0)
        throttlePercent = 0.0; // Yüzdeyi sınırla

      if (millis() - lastUpdateTime >= 100)
      { // update once per second
        comManager.send_throttle_level(throttlePercent);
        lastUpdateTime = millis();
      }

      int potValue = (int)((throttlePercent / 100.0) * 30);

      if (potValue > 30)
        potValue = 30;
      if (potValue < 0)
        potValue = 0;
      // Sinyali üret
      digiPot.setValue(potValue); // Dijital potansiyometreye throttle değerini gönder
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

  // Serial.println("PWM Write: 128");
  delay(5); // 100 ms bekle
}