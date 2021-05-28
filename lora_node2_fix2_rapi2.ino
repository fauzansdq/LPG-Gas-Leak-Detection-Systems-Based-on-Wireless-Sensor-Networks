#include <SPI.h>              // include libraries
#include <LoRa.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
const int SSR1 = 4;
const int SSR2 = 5;
const long frequency = 915E6;  // LoRa Frequency
const int csPin = 10;          // LoRa radio chip select
const int resetPin = 9;        // LoRa radio reset
const int irqPin = 2;          // change for your board; must be a hardware interrupt pin
const float RL = 1;
const float clean_air_ratio = 10.0;
float RS, Ro, RsRo, ppm, v;

void setup() {
  Serial.begin(9600);
  while (!Serial);
  LoRa.setPins(csPin, resetPin, irqPin);
  if (!LoRa.begin(frequency)) {
    Serial.println("inisialisasi LoRa gagal. cek koneksi.");
    while (true); // jika gagal, do nothing
  }
  Serial.println("inisialisasi LoRa sukses.");
  Serial.println("LoRa Node 2");
  LoRa.onReceive(onReceive);
  LoRa_rxMode();
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("kalibrasi...");
  delay(60000);
  int adc = analogRead(A0);
  v = adc * 5 / 1023.00;
  //  RS = ((5 * RL / v) - RL);
  RS = 5 - v;
  RS = (RS / v) * RL;
  Ro = RS / clean_air_ratio;
  //Ro = 1.83;
  String tampilrl = "Ro = ";
  tampilrl += Ro;
  lcd.setCursor(0, 1);
  lcd.print(tampilrl);
  delay(3000);
}

void loop() {

  if (runEvery(800)) {
    lcd.clear();
    String message1 = "";

    while (LoRa.available()) {
      message1 += (char)LoRa.read();
    }
    int adc = analogRead(A0);
    v = adc * 5 / 1023.00;
    // RS = ((5 * RL / v) - RL);
    RS = 5 - v;
    RS = (RS / v) * RL;
    //RsRo=1;
    RsRo = RS / Ro;
    //    ppm = 1099.6 * pow((RsRo), -2.31);
    ppm = pow((RsRo / 18.446), -2.375);
    String tampil = "PPM:";
    tampil += ppm;
    lcd.setCursor(0, 0);
    lcd.print(tampil);

    if (ppm >= 200 && ppm < 2000) {
      digitalWrite(SSR1, HIGH);
      digitalWrite(SSR2, LOW);
      lcd.setCursor(0, 1);
      lcd.print("WASPADA");
      Serial.println("WASPADA");
    }
    else if (ppm >= 2000) {
      digitalWrite(SSR1, HIGH);
      digitalWrite(SSR2, HIGH);
      lcd.setCursor(0, 1);
      lcd.print("BAHAYA");
      Serial.println("BAHAYA");
    }
    else {
      digitalWrite(SSR1, LOW);
      digitalWrite(SSR2, LOW);
      Serial.println("aman");
      lcd.setCursor(0, 1);
      lcd.print("AMAN");
    }
    if (message1 == "node?") {

      Serial.print("Node Receive: ");
      Serial.println(message1);

      String message = "112,";

      message += ppm;
      message += "#";

      LoRa_sendMessage(message);

      Serial.println("Send Message!");
    }
    Serial.print("ADC= ");
    Serial.print(adc);
    Serial.print(" |V= ");
    Serial.print(v);
    Serial.print(" |RS= ");
    Serial.print(RS);
    Serial.print(" |RO= ");
    Serial.print(Ro);
    Serial.print(" |RS/RO= ");
    Serial.print(RsRo);
    Serial.print(" |ppm= ");
    Serial.print(ppm);
    Serial.println(" ppm");
  }
}
void LoRa_txMode() {
  LoRa.idle();                          // set standby mode
  LoRa.disableInvertIQ();               // normal mode
}
void LoRa_rxMode() {
  LoRa.enableInvertIQ();                // active invert I and Q signals
  LoRa.receive();                       // set receive mode
}

void LoRa_sendMessage(String message) {
  LoRa_txMode();                        // set tx mode
  LoRa.beginPacket();                   // start packet
  LoRa.print(message);                  // add payload
  LoRa.endPacket();                     // finish packet and send it
  LoRa_rxMode();                        // set rx mode
}
// put your main code here, to run repeatedly:
void onReceive(int packetSize) {
}
boolean runEvery(unsigned long interval)
{
  static unsigned long previousMillis = 0;
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;
    return true;
  }
  return false;
}
