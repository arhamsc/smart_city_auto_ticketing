#include <Arduino.h>
#include <SPI.h>
#include <MFRC522v2.h>
#include <MFRC522DriverSPI.h>
#include <MFRC522DriverPinSimple.h>
#include <MFRC522Debug.h>
#include <SoftwareSerial.h>
#include "rfidCardCRUD.h"
#include <ArduinoJson.h>

SoftwareSerial EspArduino(3, 2);

MFRC522DriverPinSimple ss_pin(10);

MFRC522DriverSPI driver{ss_pin};

MFRC522 mfrc522{driver};

const int BLOCK_TO_WRITE = 2; // block 14 is read first, hence we write the data to this card

RFIDCRUD rfid(mfrc522);

byte blockData[WRITE_BUFFER_SIZE] = {"MyUserId3"};

byte readBlockData[READ_BUFFER_SIZE];
int writing_flag = 0;

const int LED_YELLOW = 4;

void sendReadDetailsToESP()
{
  if (writing_flag == 1)
  {
    return;
  }
  EspArduino.println("##readCard##");
  rfid.readDataFromBlock(BLOCK_TO_WRITE, readBlockData);

  rfid.printReadBlockData(readBlockData, BLOCK_TO_WRITE);
  String str((char *)readBlockData);
  EspArduino.println(str);
}

void setup()
{
  Serial.begin(9600);
  EspArduino.begin(9600);
  while (!Serial)
    ;
  pinMode(LED_YELLOW, OUTPUT);
  SPI.begin();

  mfrc522.PCD_Init();
  delay(10);
  MFRC522Debug::PCD_DumpVersionToSerial(mfrc522, Serial);

  Serial.println(F("Scan PICC to see UID, SAK, type etc...."));
  Serial.println(F("Setup Ended."));
}

void loop()
{

  rfid.initializeKey();

  if (!mfrc522.PICC_IsNewCardPresent())
  {
    return;
  }

  if (!mfrc522.PICC_ReadCardSerial())
  {
    return;
  }

  sendReadDetailsToESP();

  if (EspArduino.available() && EspArduino.read() != -1)
  {
    while (EspArduino.available() > 0)
    {
      String command = EspArduino.readStringUntil('\n');
      command.replace('#', ' ');
      command.trim();
      Serial.println(command);
      if (command.equalsIgnoreCase("ledon"))
      {
        digitalWrite(LED_YELLOW, HIGH);
      }
      else if (command.equals("ledOff"))
      {
        digitalWrite(LED_YELLOW, LOW);
      }
      else if (command.equals("writeUserID"))
      {
        writing_flag = 1;
        digitalWrite(LED_YELLOW, HIGH);

        String body = EspArduino.readStringUntil('\n');
        DynamicJsonDocument data(512);
        deserializeJson(data, body);
        rfid.printUid();
        byte blockDataToWrite[WRITE_BUFFER_SIZE];
        body = data["email"].as<String>();
        body.getBytes(blockDataToWrite, WRITE_BUFFER_SIZE);
        rfid.writeDataToBlock(BLOCK_TO_WRITE, blockDataToWrite);
        rfid.readDataFromBlock(BLOCK_TO_WRITE, readBlockData);

        rfid.printReadBlockData(readBlockData, BLOCK_TO_WRITE);
        Serial.println("\n");
        EspArduino.println("##done##");
        digitalWrite(LED_YELLOW, LOW);
        writing_flag = 0;
      }
      else if (command.equals("getUid"))
      {
        char uidH[32];
        // DynamicJsonDocument data(512);
        rfid.copyUid(uidH);
        EspArduino.println(uidH);
        EspArduino.println("##done##");
        // data["uid"] = uidH;
        // serializeJson(data, EspArduino);
      }
    }
  }

  timeNow = millis();
  while (millis() - timeNow < 1500)
  {
    if (Serial.available())
    {
      Serial.read();
    }
  }
  rfid.reselect_Card();
}