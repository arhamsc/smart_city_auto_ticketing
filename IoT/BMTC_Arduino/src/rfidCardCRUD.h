#pragma once

#include <Arduino.h>
#include <SPI.h>
#include <MFRC522v2.h>
#include <MFRC522DriverSPI.h>
#include <MFRC522DriverPinSimple.h>
#include <MFRC522Debug.h>
#include <MFRC522Constants.h>
#include <Wire.h>

const int RFID_RST = 9;

// Writing is only from 0th to 15th byte
const int WRITE_BUFFER_SIZE = 16;

// Reading has 2 extra bytes on the trailing which shouldn't be changed.
const int READ_BUFFER_SIZE = 18;

byte bufferLen = READ_BUFFER_SIZE; // blockData size + 2

unsigned long timeNow = 0;

char uid[32];

class RFIDCRUD
{
private:
  MFRC522 *mfrc522;
  // Key is present at every sector 1st block to depict  authorization, it can be customized.
  // After Key A there is access permissions so they can also be set
  // Additionally there is a Key B.
  MFRC522::MIFARE_Key key;

  MFRC522::StatusCode statusCode;

protected:
  // Method to check status of the process
  void checkStatus(MFRC522::StatusCode status, String checkFor = "Authentication")
  {
    Serial.print(checkFor);
    if (status != MFRC522Constants::STATUS_OK)
    {
      Serial.print(" failed.");
      Serial.println(MFRC522Debug::GetStatusCodeName(status));
      Serial.println("Note: Check for key Init.");
      return;
    }
    else
    {
      Serial.println(" success");
    }
  }

public:
  RFIDCRUD() = delete;
  RFIDCRUD(MFRC522 &mfrc522Obj)
  {
    this->mfrc522 = &mfrc522Obj;
  }
  // Initializing the key for the manufacturer/default key to authorize later.
  //* This has to be called before read or write methods.
  void initializeKey()
  {
    for (byte i = 0; i < 6; i++)
    {
      key.keyByte[i] = 0xFF;
    }
  }

  void sendResetPulse()
  {
    digitalWrite(RFID_RST, LOW);
    delay(300);
    digitalWrite(RFID_RST, HIGH);
  }

  void array_to_string(byte array[], unsigned int len, char buffer[])
  {
    for (unsigned int i = 0; i < len; i++)
    {
      byte nib1 = (array[i] >> 4) & 0x0F;
      byte nib2 = (array[i] >> 0) & 0x0F;
      buffer[i * 2 + 0] = nib1 < 0xA ? '0' + nib1 : 'A' + nib1 - 0xA;
      buffer[i * 2 + 1] = nib2 < 0xA ? '0' + nib2 : 'A' + nib2 - 0xA;
    }
    buffer[len * 2] = '\0';
  }

  void copyUid(char *uidRe)
  {
    char str[32] = "";
    array_to_string((*mfrc522).uid.uidByte, 4, str);
    for (int i = 0; i < 32; i++)
    {
      uidRe[i] = str[i];
    }
  }

  // Method to print Uid of the RFID Card
  void printUid()
  {
    char str[32] = "";
    array_to_string((*mfrc522).uid.uidByte, 4, str);
    Serial.print("Card Uid: ");
    Serial.println(str);
    for (int i = 0; i < 32; i++)
    {
      uid[i] = str[i];
    }
    // for (byte i = 0; i < (*mfrc522).uid.size; i++)
    // {
    //   // uid[i] = (*mfrc522).uid.uidByte[i] < 0x10 ? " 0" : " ";
    //   Serial.print((*mfrc522).uid.uidByte[i] < 0x10 ? " 0" : " ");
    //   Serial.print((*mfrc522).uid.uidByte[i], HEX);
    // }
  }

  // Method to print the PICC Type
  void printPICCType()
  {
    Serial.print("PICC Type: ");
    MFRC522::PICC_Type piccType = (*mfrc522).PICC_GetType((*mfrc522).uid.sak);
    Serial.println(MFRC522Debug::PICC_GetTypeName(piccType));
  }

  // Method to read block data
  void printReadBlockData(byte readBlockData[], int blockToWrite)
  {
    Serial.print("Data in Block:");
    Serial.print(blockToWrite);
    Serial.print(" --> ");
    for (int j = 0; j < WRITE_BUFFER_SIZE; j++)
    {
      Serial.write(readBlockData[j]);
    }
  }

  // Method to write data to card in particular block
  void writeDataToBlock(int blockNum, byte blockData[])
  {
    /*Authenticate the block if its read or write or both using the KEY A (1st Block of all Sectors)*/
    statusCode = (*mfrc522).PCD_Authenticate(MFRC522Constants::PICC_CMD_MF_AUTH_KEY_A, blockNum, &key, &((*mfrc522).uid));

    checkStatus(statusCode);

    statusCode = (*mfrc522).MIFARE_Write(blockNum, 0, WRITE_BUFFER_SIZE);
    statusCode = (*mfrc522).MIFARE_Write(blockNum, blockData, WRITE_BUFFER_SIZE);
    checkStatus(statusCode, "Writing");
    sendResetPulse();
  }

  // Method to read from a particular block from the card.
  void readDataFromBlock(int blockNum, byte readBlock[])
  {
    /*Authenticate the block if its read or write or both using the KEY A (1st Block of all Sectors)*/
    statusCode = (*mfrc522).PCD_Authenticate(MFRC522Constants::PICC_CMD_MF_AUTH_KEY_A, blockNum, &key, &((*mfrc522).uid));

    checkStatus(statusCode);

    statusCode = (*mfrc522).MIFARE_Read(blockNum, readBlock, &bufferLen);
    checkStatus(statusCode, "Reading");
    sendResetPulse();
  }

  bool reselect_Card()
  {

    byte s;
    byte req_buff[2];
    byte req_buff_size = 2;
    (*mfrc522).PCD_StopCrypto1();
    s = (*mfrc522).PICC_HaltA();
    delay(100);
    s = (*mfrc522).PICC_WakeupA(req_buff, &req_buff_size);
    delay(100);
    s = (*mfrc522).PICC_Select(&((*mfrc522).uid), 0);
    if (MFRC522Debug::GetStatusCodeName((MFRC522::StatusCode)s) == F("Timeout in communication."))
    {
      return false;
    }
    return true;
  }
};