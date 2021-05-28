#include <SPI.h>              // include libraries
#include <LoRa.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
const long frequency = 915E6;  // LoRa Frequency
const int SSR1 = 4;
const int SSR2 = 5;
const int csPin = 10;          // LoRa radio chip select
const int resetPin = 9;        // LoRa radio reset
const int irqPin = 2;          // change for your board; must be a hardware interrupt pin
const float RL = 1;
const float clean_air_ratio = 10.0;

float RS, Ro, RsRo, ppm, v;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);                   // initialize serial
  while (!Serial);

  LoRa.setPins(csPin, resetPin, irqPin);

  if (!LoRa.begin(frequency)) {
    Serial.println("LoRa init failed. Check your connections.");
    while (true);                       // if failed, do nothing
  }

  Serial.println("LoRa init succeeded.");
  Serial.println();
  Serial.println("LoRa Simple Node");
  Serial.println("Only receive messages from gateways");
  Serial.println("Tx: invertIQ disable");
  Serial.println("Rx: invertIQ enable");
  Serial.println();
  LoRa.onReceive(onReceive);
  LoRa_rxMode();
  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("kalibrasi...");
  delay(60000);
  int adc = analogRead(A0);
  v = adc * 5 / 1023.00;
  //RS = ((5 * RL / v) - RL);
  RS = 5 - v;
  RS = (RS / v) * RL;
  //RS = ( (float) RL * (1023 - adc) / adc );
  Ro = RS / clean_air_ratio;
  // Ro = 6.29;
  String tampilrl = "Ro = ";
  tampilrl += Ro;
  lcd.setCursor(0, 1);
  lcd.print(tampilrl);
  delay(3000);
}

void loop() {
  if (runEvery(900)) { //ganti detiknya
    lcd.clear();
    String message1 = "";

    while (LoRa.available()) {
      message1 += (char)LoRa.read();
    }
    int adc = analogRead(A0);
    // float v = adc * 5 / 1023.00;
    // RS = ((5 * RL / v) - RL);
    v = adc * 5 / 1023.00;
    RS = 5 - v;
    RS = (RS / v) * RL;
    // RS = ((5 * RL / v) - RL);
    //RS = ( (float) RL * (1023 - adc) / adc );
    //Ro = RS / CLEAN_AIR_RATIO;    // Do only once while measuring clean air.
    RsRo = RS / Ro;
    // ppm = 1099.6 * pow((RsRo), -2.31);
    ppm = pow((RsRo / 18.446), -2.375);
    // ppm = 2127.2 * pow((RsRo), -2.526);
    // double ppm_log = (log10(RsRo) - 1.21) / -0.411;
    // ppm = pow(10, ppm_log);
    //  float linear = 9.58*adc+200;
    String tampil = "PPM:";
    tampil += ppm;
    lcd.setCursor(0, 0);
    lcd.print(tampil);

   /* if (ppm >= 200 && ppm < 2000) {
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
    } */
    if (message1 == "node?") {
      Serial.print("Node Receive: ");
      Serial.println(message1);
      // String message = "I'm node! ";
      String message = "111,";
      //message += "I'm a Node! ";
      // message += counter;
      // message += ",";
      message += ppm;
      message += "#";
      //message += millis();

      LoRa_sendMessage(message); // send a message

      Serial.println("Send Message!");
      // lcd.setCursor(0, 0);
      // lcd.print("gas: " + message);


      // counter++;
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
  /*
    String message1 = "";

    while (LoRa.available()) {
      message1 += (char)LoRa.read();
    }

    Serial.print("Node Receive: ");
    Serial.println(message1); */

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
