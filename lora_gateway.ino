#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
SoftwareSerial mySerial(4, 5);
LiquidCrystal_I2C lcd(0x27, 20, 4);
const long frequency = 915E6;
const int csPin = 10;
const int resetPin = 9;
const int irqPin = 2;
const int buzzer = 7;
String id, Data, gas1, gas1simpan, gas2simpan, gas2;
int len_ID, len_DT;


void setup() {
  pinMode(buzzer, OUTPUT);
  Serial.begin(9600);
  mySerial.begin(9600);
  while (!Serial);
  LoRa.setPins(csPin, resetPin, irqPin);
  if (!LoRa.begin(frequency)) {
    Serial.println("LoRa init failed. Check your connections.");
    while (true);
  }
  Serial.println("LoRa init succeeded.");
  Serial.println("LoRa Gateway");
  LoRa.onReceive(onReceive);
  LoRa_rxMode();
  lcd.init();
  lcd.backlight();
}

void loop() {

  if (runEvery(1000)) { // repeat every 5000 millis
    lcd.clear();
    lcd.setCursor(5, 0);
    lcd.print("GAS GATEWAY");
    digitalWrite(buzzer, LOW);
    String message = "node?";
    LoRa_sendMessage(message);
    Serial.println("Send Message Request!");
    lcd.setCursor(0, 2);
    lcd.print("01: ");
    lcd.setCursor(0, 3);
    lcd.print("02: ");

    if (len_DT < 0 ) {
      lcd.setCursor(4, 2);
      lcd.print(gas1simpan);

      lcd.setCursor(4, 3);
      lcd.print(gas2simpan);

    }
    else {
      gas1simpan = gas1;
      gas2simpan = gas2;


      lcd.setCursor(4, 2);
      lcd.print(gas1);
      lcd.setCursor(4, 3);
      lcd.print(gas2);

    }

    if (gas1.toInt() > 200 && gas1.toInt() < 1999) {
      lcd.setCursor(4, 1);
      lcd.print("WASPADA GAS 1");
      digitalWrite(buzzer, HIGH);
      Serial.println("waspada gas 1 ");
    }
   /* else if (gas1.toInt() > 2000) {
      lcd.setCursor(4, 1);
      lcd.print("BAHAYA GAS 1");
      digitalWrite(buzzer, HIGH);
      Serial.println("bahaya gas 1 ");
      // lcd.setCursor(12, 2);
      // lcd.print("SMS");
      mySerial.println("AT");
      updateSerial();
      mySerial.println("AT+CMGF=1"); // Configuring TEXT mode
      updateSerial();
      mySerial.println("AT+CMGS=\"085155\"");
      updateSerial();
      mySerial.print("GAS DI DAPUR 1 BOCOR"); //text content
      updateSerial();
      mySerial.write(26);
    } */
    else {
      digitalWrite(buzzer, LOW);
      lcd.setCursor(4, 1);
      lcd.print("             ");
      lcd.setCursor(11, 2);
      lcd.print("          ");
    }

    if (gas2.toInt() >= 200 && gas2.toInt() < 2000) {
      lcd.setCursor(4, 1);
      lcd.print("WASPADA GAS 2");
      digitalWrite(buzzer, HIGH);
      Serial.println("waspada gas 2 ");
    }
    else if (gas2.toInt() >= 2000) {
      lcd.setCursor(4, 1);
      lcd.print("BAHAYA GAS 2");
      digitalWrite(buzzer, HIGH);
      Serial.println("bahaya gas 2 ");
      //  lcd.setCursor(12, 3);
      // lcd.print("SMS");

      mySerial.println("AT");
      updateSerial();

      mySerial.println("AT+CMGF=1"); // Configuring TEXT mode
      updateSerial();
      mySerial.println("AT+CMGS=\"08515\"");
      updateSerial();
      //lcd.setCursor(0, 2);
      //lcd.print("SMS dikirim");
      mySerial.print("GAS DI DAPUR 2 BOCOR"); //text content
      updateSerial();
      mySerial.write(26);
    }

    else {
      digitalWrite(buzzer, LOW);
      lcd.setCursor(4, 1);
      lcd.print("             ");
      lcd.setCursor(11, 3);
      lcd.print("          ");
    }
  /*  if (gas2.toInt() > 200 && gas2.toInt() < 2000 && gas1.toInt() > 200 && gas1.toInt() < 2000) {
      lcd.setCursor(4, 1);
      lcd.print("WASPADA GAS 1 DAN 2");
    }
    else if (gas2.toInt() > 2000 && gas1.toInt() > 2000 ) {
      lcd.setCursor(4, 1);
      lcd.print("BAHAYA GAS 1 DAN 2");
      digitalWrite(buzzer, HIGH);
      // lcd.setCursor(12, 3);
      // lcd.print("SMS");

      mySerial.println("AT");
      updateSerial();

      mySerial.println("AT+CMGF=1");
      updateSerial();
      mySerial.println("AT+CMGS=\"0899\"");
      updateSerial();
      mySerial.print("GAS DI DAPUR 1 DAN 2 BOCOR");
      updateSerial();
      mySerial.write(26);

    }
    else {
      digitalWrite(buzzer, LOW);
      lcd.setCursor(4, 1);
      lcd.print("             ");
      lcd.setCursor(11, 3);
      lcd.print("          ");
    }
*/
  }
}



void LoRa_rxMode() {
  LoRa.disableInvertIQ();               // normal mode
  LoRa.receive();                       // set receive mode
}

void LoRa_txMode() {
  LoRa.idle();                          // set standby mode
  LoRa.enableInvertIQ();                // active invert I and Q signals
}

void LoRa_sendMessage(String message) {
  LoRa_txMode();                        // set tx mode
  LoRa.beginPacket();                   // start packet
  LoRa.print(message);                  // add payload
  LoRa.endPacket();                     // finish packet and send it
  LoRa_rxMode();                        // set rx mode
}

void onReceive(int packetSize) {

  String message = "";
  while (LoRa.available()) {
    message += (char)LoRa.read();
  }
  Serial.println(message);
  len_ID = message.indexOf(",");
  len_DT = message.indexOf("#");
  id = message.substring(0, len_ID); // parsing id
  Data = message.substring(len_ID + 1, len_DT); // parsing data

  if (len_ID > 2 && len_DT > 7) {

    if (id == "111") {
      gas1 = Data;
      Serial.print("001, ");
      Serial.println(" " + gas1);
    }
    if (id == "112") {

      gas2 = Data;
      Serial.print("002, ");
      Serial.println(" " + gas2);
    }
  }
}


// Data, Data1, Data2 = "";


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

void updateSerial()
{
  delay(500);
  while (Serial.available())
  {
    mySerial.write(Serial.read());//Forward what Serial received to Software Serial Port
  }
  while (mySerial.available())
  {
    Serial.write(mySerial.read());//Forward what Software Serial received to Serial Port
  }
}
