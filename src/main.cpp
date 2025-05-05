#include <mcp_can.h>
#include <SPI.h>

#define CAN_CS_PIN 5 // MCP2515 CS pini
#define VIRTUAL_POTENTIOMETER_PIN   15
MCP_CAN CAN(CAN_CS_PIN); // MCP2515 nesnesi oluşturuluyor

void setup()
{
  Serial.begin(9600); // Seri iletişim başlatılıyor
  pinMode(VIRTUAL_POTENTIOMETER_PIN, OUTPUT);
  // CAN Bus başlatılıyor
  if (CAN.begin(MCP_ANY, CAN_500KBPS, MCP_8MHZ) == CAN_OK)
  {
    Serial.println("MCP2515 başarıyla başlatıldı!");
  }
  else
  {
    Serial.println("MCP2515 başlatılamadı!");
    while (1)
      ; // Başlatma başarısız olursa programı durdur
  }

  CAN.setMode(MCP_NORMAL); // Normal moda geçiş yapılıyor
}

void loop()
{

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
      Serial.print(" → Yüzde: ");
      Serial.print(throttlePercent);
      Serial.println(" %");

      int pwmValue = (int)(57 + (throttlePercent / 100.0) * 133);

      // Sinyali üret
      analogWrite(VIRTUAL_POTENTIOMETER_PIN, pwmValue);

    }
  }
}