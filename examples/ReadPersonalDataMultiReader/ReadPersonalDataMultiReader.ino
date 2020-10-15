/**
    --------------------------------------------------------------------------------------------------------------------
    Example sketch/program showing how to read data from more than one PICC to serial.
    --------------------------------------------------------------------------------------------------------------------
    This is a MFRC522 library example; for further details and other examples see: https://github.com/miguelbalboa/rfid

    Example sketch/program showing how to read UID and PERSONAL DATA from more than one PICC (that is: a RFID Tag or Card) using a
    MFRC522 based RFID Reader on the Arduino SPI interface.

    Warning: This has been tested on Wemos D1 mini and Arduino Uno for 2 readers only.
    Contribution: This was contributed by Wahaj Murtaza. https://github.com/wahajmurtaza

    @license Released into the public domain.

    Typical pin layout used:
    ----------------------------------------------------------------------------------------------------
               MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino     Wemos
               Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro   D1 mini
    Signal      Pin          Pin           Pin       Pin        Pin              Pin          Pin
    ---------------------------------------------------------------------------------------------------
    RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST        D4
    SPI SS 1    SDA(SS)      ** custom, take a unused pin, only HIGH/LOW required *         D8
    SPI SS 2    SDA(SS)      ** custom, take a unused pin, only HIGH/LOW required *         D0
    SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16         D7
    SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14         D6
    SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15         D5

*/

#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN         D4          // Configurable, see typical pin layout above
#define SS_1_PIN        D8         // Configurable, take a unused pin, only HIGH/LOW required, must be different to SS 2
#define SS_2_PIN        D0          // Configurable, take a unused pin, only HIGH/LOW required, must be different to SS 1

#define NR_OF_READERS   2

byte ssPins[] = {SS_1_PIN, SS_2_PIN};

MFRC522 mfrc522[NR_OF_READERS];   // Create MFRC522 instance.
MFRC522::MIFARE_Key key;
MFRC522::StatusCode status;
byte buffer1[18];

/**
    Initialize.
*/
void setup() {

  Serial.begin(9600); // Initialize serial communications with the PC
  while (!Serial);    // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)

  SPI.begin();        // Init SPI bus

  for (uint8_t reader = 0; reader < NR_OF_READERS; reader++) {
    mfrc522[reader].PCD_Init(ssPins[reader], RST_PIN); // Init each MFRC522 card
    Serial.print(F("Reader "));
    Serial.print(reader);
    Serial.print(F(": "));
    mfrc522[reader].PCD_DumpVersionToSerial();
  }
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;
}

/**
    Main loop.
*/
void loop() {

  for (uint8_t reader = 0; reader < NR_OF_READERS; reader++) {
    // Look for new cards

    if (mfrc522[reader].PICC_IsNewCardPresent() && mfrc522[reader].PICC_ReadCardSerial()) {
      Serial.print(F("Reader "));
      Serial.print(reader);
      // Show some details of the PICC (that is: the tag/card)
      Serial.print(F(": Card UID:"));
      dump_byte_array(mfrc522[reader].uid.uidByte, mfrc522[reader].uid.size);
      Serial.println();
      bool readed = try_reading(&key, reader);

    }
  }
}

/**
    Helper routine to dump a byte array as hex values to Serial.
*/
void dump_byte_array(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}

bool try_reading(MFRC522::MIFARE_Key *key, uint8_t reader)
{
  //---------WARNING---------------------------------------------------------------
  // DO NOT PUT THIS CODE IN MAIN LOOP, WHICH WILL NOT WORK FOR BLOCK OTHER THAN 0
  bool result = false;

  byte block = 0;

  // Serial.println(F("Authenticating using key A..."));
  status = mfrc522[reader].PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, key, &(mfrc522[reader].uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("PCD_Authenticate() failed: "));
    Serial.println(mfrc522[reader].GetStatusCodeName(status));
    return false;
  }

  // Read block
  byte byteCount = sizeof(buffer1);
  status = mfrc522[reader].MIFARE_Read(block, buffer1, &byteCount);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("MIFARE_Read() failed: "));
    Serial.println(mfrc522[reader].GetStatusCodeName(status));
    //  Soft Resetting the Sensor, Sensor gets hang.
    Serial.print("Resetting Reader: ");
    Serial.println(reader);
    mfrc522[reader].PCD_Reset();
    mfrc522[reader].PCD_Init();
    return false;
  }
  else {
    // Successful read
    result = true;
    Serial.print("  Authenticated using key: ");
    dump_byte_array((*key).keyByte, MFRC522::MF_KEY_SIZE);
    Serial.print("\n  Readed Data: ");
    dump_byte_array(buffer1, 18);
  }
  Serial.println();

  mfrc522[reader].PICC_HaltA();       // Halt PICC
  mfrc522[reader].PCD_StopCrypto1();  // Stop encryption on PCD
  return result;

}
