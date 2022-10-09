#include "Arduino.h"
#include "Button2.h"
#include "MFRC522.h"
#include "Helpers.h"
#include "SPI.h"

#include "state.cpp"

#define BUTTON_PIN PA0
#define SS_PIN PA5
#define RST_PIN 7

MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class

MFRC522::MIFARE_Key key;

byte nuidPICC[4];

Button2 button;

void longpress(Button2 &btn)
{
  unsigned int time = btn.wasPressedFor();
  Serial.print("You clicked ");
  if (time > 1500)
  {
    Serial.print("a really really long time.");
  }
  else if (time > 1000)
  {
    Serial.print("a really long time.");
  }
  else if (time > 500)
  {
    Serial.print("a long time.");
  }
  else
  {
    Serial.print("long.");
  }
  Serial.print(" (");
  Serial.print(time);
  Serial.println(" ms)");
}

void shortPress(Button2 &btn)
{
  Serial.println("Button has been pressed");
}


void setup()
{
  Serial.begin(9600);
  while (!Serial)
  {
    delay(20);
  }
  Serial.println("\n\nLongpress Handler Demo");
  Serial.println("Button Pin: " + String(BUTTON_PIN));

  button.begin(BUTTON_PIN);
  button.setLongClickHandler(longpress);
  button.setClickHandler(shortPress);

  // nfc read initialization
  SPI.begin();     // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522

  for (byte i = 0; i < 6; i++)
  {
    key.keyByte[i] = 0xFF;
  }

  Serial.println(F("This code scan the MIFARE Classsic NUID."));
  Serial.print(F("Using the following key:"));
  printHex(key.keyByte, MFRC522::MF_KEY_SIZE);
  Serial.println();
}

unsigned long prevTime = millis();

void loop()
{
  button.loop();

  unsigned long currentTime = millis();
  if (currentTime - prevTime > 1000)
  {
    prevTime = currentTime;
    Serial.println("Uptime: " + String(currentTime / 1000) + " seconds");
  }










// Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  if ( ! rfid.PICC_IsNewCardPresent())
    return;

  // Verify if the NUID has been readed
  if ( ! rfid.PICC_ReadCardSerial())
    return;

  Serial.print(F("PICC type: "));
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  Serial.println(rfid.PICC_GetTypeName(piccType));

  // Check is the PICC of Classic MIFARE type
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&  
    piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
    piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
    Serial.println(F("Your tag is not of type MIFARE Classic."));
    return;
  }

  if (rfid.uid.uidByte[0] != nuidPICC[0] || 
    rfid.uid.uidByte[1] != nuidPICC[1] || 
    rfid.uid.uidByte[2] != nuidPICC[2] || 
    rfid.uid.uidByte[3] != nuidPICC[3] ) {
    Serial.println(F("A new card has been detected."));

    // Store NUID into nuidPICC array
    for (byte i = 0; i < 4; i++) {
      nuidPICC[i] = rfid.uid.uidByte[i];
    }
   
    Serial.println(F("The NUID tag is:"));
    Serial.print(F("In hex: "));
    printHex(rfid.uid.uidByte, rfid.uid.size);
    Serial.println();
    Serial.print(F("In dec: "));
    printDec(rfid.uid.uidByte, rfid.uid.size);
    Serial.println();
  }
  else Serial.println(F("Card read previously."));

  // Halt PICC
  rfid.PICC_HaltA();

  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();







}
