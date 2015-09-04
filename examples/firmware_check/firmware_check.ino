/*
 * ----------------------------------------------------------------------------
 * Example sketch/program to test your firmware.
 * ----------------------------------------------------------------------------
 * This is a MFRC522 library example; see https://github.com/miguelbalboa/rfid
 * for further details and other examples.
 * 
 * NOTE: The library file MFRC522.h has a lot of useful info. The functions are
 * documented in MFRC522.cpp. Please read it.
 * 
 * Released into the public domain.
 * 
 * Typical pin layout used:
 * -----------------------------------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
 *             Reader/PCD   Uno           Mega      Nano v3    Leonardo/Micro   Pro Micro
 * Signal      Pin          Pin           Pin       Pin        Pin              Pin
 * -----------------------------------------------------------------------------------------
 * RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
 * SPI SS      SDA(SS)      10            53        D10        10               10
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
 * SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
 * SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
 *
 * @author Rotzbua
 */

#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN         9          // Configurable, see typical pin layout above
#define SS_PIN          10         // Configurable, see typical pin layout above

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance.

void setup() {
  Serial.begin(9600);   // Initialize serial communications with the PC
  while (!Serial);      // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
  SPI.begin();          // Init SPI bus
  mfrc522.PCD_Init();   // Init MFRC522 module
  
  Serial.println(F("*****************************"));
  Serial.println(F("MFRC522 Digital self test"));
  Serial.println(F("*****************************"));
  ShowReaderVersion();  // Show version of PCD - MFRC522 Card Reader
  Serial.println(F("Performing test..."));
  bool result = mfrc522.PCD_PerformSelfTest();
  Serial.println(F("-----------------------------"));
  Serial.print(F("Result: "));
  if (result)
    Serial.println(F("OK"));
  else
    Serial.println(F("DEFECT or UNKNOWN"));
  Serial.println();
}

void loop() {} // nothing to do

/**
* Helper to print MFRC522 module info
*/
void ShowReaderVersion() {
  // Get the MFRC522 firmware version
  byte v = mfrc522.PCD_ReadRegister(mfrc522.VersionReg);
  Serial.print(F("Firmware Version: 0x"));
  Serial.print(v, HEX);
  if (v == 0x88)
    Serial.print(F(" = (clone)"));
  else if (v == 0x90)
    Serial.print(F(" = v0.0"));
  else if (v == 0x91)
    Serial.print(F(" = v1.0"));
  else if (v == 0x92)
    Serial.print(F(" = v2.0"));
  else
    Serial.print(F(" = (unknown)"));
  Serial.println();
  // When 0x00 or 0xFF is returned, communication probably failed
  if ((v == 0x00) || (v == 0xFF))
    Serial.println(F("WARNING: Communication failure, is the MFRC522 properly connected?"));
}
