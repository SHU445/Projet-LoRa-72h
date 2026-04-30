// Display suppr and GPS

#include <TinyGPS++.h>
#include <ArduinoJson.h>
#include <LoRa.h>

static const uint32_t GPSBaud = 9600;

/*WeMos D1          RFM9x Module
  GPIO12 (D6) <----> MISO
  GPIO13 (D7) <----> MOSI
  GPIO14 (D5) <----> CLK
  GPIO15 (D8) <----> DIO0/D2 OR DIO1/D3 OR DIO2/D4
  GPIO16 (D0) <----> SEL Chip Select (depending on bottom solder PAD position)

   WeMos D1         Shield Feature
  GPIO5  (D1) <----> I2C SCL
  GPIO4  (D2) <----> I2C SDA
  GPIO0  (D3) <----> WS2812 LEDS
  GPIO2  (D4) <----> Push Button
  GPIO16 (D0) <----> RESET (depending on bottom solder PAD position)

*/

#define SS      D0
#define RST     D0
#define DI0     D8


#define spreadingFactor 9
#define SignalBandwidth 125E3
#define preambleLength 8
#define codingRateDenominator 8

#define FREQUENCY 868.9E6

#define LEDPIN 5


double retFrequency;
int retSF;

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

void receivingMessage () {
  unsigned long eentry = millis();
  StaticJsonBuffer<500> jsonBuffer;
  char json[500];
  Serial.println("HIGH");
  digitalWrite(LEDPIN, LOW);

  // Receive Message

  //  Serial.print("Start receiving ");
  //  Serial.println(millis());
  while (LoRa.parsePacket() == 0) yield();
  // received a packet
  // Serial.println("Received packet");

  // read packet
  json[0] = '\0';
  while (LoRa.available()) {
    byte hi = strlen(json);
    json[hi] = (char)LoRa.read();
    json[hi + 1] = '\0';
  }
  //  Serial.println(json);
  JsonObject& recMessage = jsonBuffer.parse(json);

  if (recMessage.success()) {

    recMessage["RSSI"] = LoRa.packetRssi();

    recMessage["rLat"] = sendLoc.lat;
    recMessage["rLng"] = sendLoc.lng;
    recMessage["rAlt"] = sendLoc.altitude;
    recMessage["rHdop"] = sendLoc.hdop;

    //    Serial.println("-------");
    //    Serial.print("Received  Data / Start sending ");
    //    Serial.println(millis());
    recMessage.printTo(Serial);
    //    Serial.println();
    Serial.println("LOW");
    digitalWrite(LEDPIN, HIGH);
    delay(200);

    // Send packet back
    String sendMessage;
    recMessage.printTo(sendMessage);
    LoRa.setTxPower(17);
    LoRa.beginPacket();
    LoRa.print(sendMessage);
    LoRa.endPacket();
    Serial.println("Package sent");
  } else Serial.println("parseObject() failed");
  Serial.println(millis() - eentry);
}

void initLoRa() {
  SPI.begin();
  LoRa.setPins(D0, D0, D8);
  if (!LoRa.begin(FREQUENCY)) {
    Serial.println("Starting LoRa failed!");
    while (1) {
      digitalWrite(LEDPIN, 1);
      delay(200);
      digitalWrite(LEDPIN, 0);
      delay(200);
    }
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

void setup() {
  Serial.begin(115200);
  pinMode(LEDPIN, OUTPUT);
  digitalWrite(LEDPIN, 1);
  delay(1000);
  digitalWrite(LEDPIN, 0);
  initLoRa();
}

void loop() {
  receivingMessage();
  yield();
}
