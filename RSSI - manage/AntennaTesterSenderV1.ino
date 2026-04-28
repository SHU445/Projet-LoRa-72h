// Display suppr and GPS

#include "SSD1306.h" // alias for `#include "SSD1306Wire.h"'
#include <TinyGPS++.h>
#include <LoRa.h>
#include <ArduinoJson.h>

static const uint32_t GPSBaud = 9600;

// WIFI_LoRa_32 ports
// GPIO5  -- SX1278's SCK
// GPIO19 -- SX1278's MISO
// GPIO27 -- SX1278's MOSI
// GPIO18 -- SX1278's CS
// GPIO14 -- SX1278's RESET
// GPIO26 -- SX1278's IRQ(Interrupt Request)

//GPIO16 OLED RESET


#define SS      18
#define RST     14
#define DI0     26

#define STARTPIN 25
#define BUZZERPIN 13
#define ZEROPIN 2


#define spreadingFactor 9
#define SignalBandwidth 125E3
#define preambleLength 8
#define codingRateDenominator 8

#define FREQUENCY 868.9E6

#define TIMEOUT 2000
#define DELAY_MS 400


struct locStruct
{
  bool isValid;
  bool isUpdated;
  uint32_t age;
  double lat;
  double lng;
  double altitude;
  double hdop;
  int power;
  int rssi;
} sendLoc, recLoc;

bool sendMessage() {
  bool success = false;
  char json[500];
  StaticJsonBuffer<500> jsonBuffer;
  JsonObject& sendMessage = jsonBuffer.createObject();
  sendMessage["freq"] = FREQUENCY;
  sendMessage["sf"] = spreadingFactor;
  sendMessage["power"] = power;
  sendMessage["rLat"] = "";
  sendMessage["rLng"] = "";
  sendMessage["rAlt"] = "";
  sendMessage["rHdop"] = "";
  sendMessage["RSSI"] = "";

  // Send Packet
  String sMessage;
  Serial.println("Send");
  sendMessage.printTo(sMessage);
  LoRa.setTxPower(power);
  LoRa.beginPacket();
  LoRa.print(sMessage);
  LoRa.endPacket();

  // Receive Message
  unsigned long entry = millis();
  Serial.println("wait for receive");
  while (LoRa.parsePacket() == 0 && (millis() < entry + TIMEOUT));
  Serial.println("receive");
  if (millis() < entry + TIMEOUT - 1) {
    // received a packet
    json[0] = '\0';
    while (LoRa.available()) {
      byte hi = strlen(json);
      json[hi] = (char)LoRa.read();
      json[hi + 1] = '\0';
    }
    JsonObject& recMessage = jsonBuffer.parse(json);
    if (recMessage.success()) {
      recLoc.lat = recMessage["rLat"];
      recLoc.lng = recMessage["rLng"];
      recLoc.altitude = recMessage["rAlt"];
      recLoc.hdop = recMessage["rHdop"];
      power = recMessage["power"];
      rssi = recMessage["RSSI"];
      Serial.print("Power: ");
      Serial.print(power);
      Serial.print(" RSSI: ");
      Serial.print(rssi);
      Serial.print(" Performance: ");
      performance = (rssi - power);
      Serial.println(performance);
      Serial.print(" Relative Performance: ");
      Serial.println(performance - zero);

      success = true;
    } else  Serial.println("parseObject() failed");
  } else Serial.println("Timeout");
  return success;
}

void initLoRa() {

  SPI.begin(5, 19, 27, 18);
  LoRa.setPins(SS, RST, DI0);
  if (!LoRa.begin(FREQUENCY)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  Serial.print("FREQUENCY ");
  Serial.println(FREQUENCY);
  LoRa.setTxPower(1);
  Serial.print("LoRa Spreading Factor: ");
  Serial.println(spreadingFactor);
  LoRa.setSpreadingFactor(spreadingFactor);
  Serial.print("LoRa Signal Bandwidth: ");
  Serial.println(SignalBandwidth);
  LoRa.setSignalBandwidth(SignalBandwidth);
  Serial.println("LoRa Initial OK!");
}

void buzz(int duration, int number) {
  for (int i = 0; i < number; i++) {
    digitalWrite(BUZZERPIN, 1);
    delay(duration);
    digitalWrite(BUZZERPIN, 0);
  }
}

void setup()
{
  Serial.begin(115200);
  pinMode(16, OUTPUT);
  digitalWrite(16, HIGH);
  pinMode(STARTPIN, INPUT_PULLUP);
  pinMode(ZEROPIN, INPUT_PULLUP);
  pinMode(BUZZERPIN, OUTPUT);
  digitalWrite(BUZZERPIN, 0);
  initLoRa();
}

void loop()
{
  Serial.println("Press button");
  while (digitalRead(STARTPIN) > 0) { // Wait for start
  }
  Serial.println("waiting");
  delay(4000);
  buzz(100, 1);
  Serial.println("Start ");
  float sum = 0;
  for (int i = 0; i < 5; i++) {
    while (!sendMessage()) {
      Serial.println("Failure");
      delay(DELAY_MS);
      // buzz(500, 1);
    }
    sum = sum + performance;
    delay(DELAY_MS);
  }
  buzz(100, 3);
  average = (sum / 5.0);
  Serial.print("Average RSSI ");
  Serial.println(average);

  char line3[15];
  dtostrf(average-zero, 7, 1, line3);

  while ((digitalRead(STARTPIN) == 1) &&  (digitalRead(ZEROPIN) == 1)) {
    // Serial.print(digitalRead(ZEROPIN));
    delay(50);
    if (digitalRead(ZEROPIN) == 0) {
      zero = average;
      Serial.print("Zero ");
      Serial.println(zero);
      if (digitalRead(ZEROPIN) == 0);
    }
  }
}
