/* Try the most used default keys in 
 * https://code.google.com/p/mfcuk/wiki/MifareClassicDefaultKeys
 * to dump block 0 of a MIFARE RFID card using a RFID-RC522 reader
 * Uses MFRC522 - Library to use ARDUINO RFID MODULE KIT 13.56 MHZ WITH TAGS SPI W AND R BY COOQROBOT. 
 ----------------------------------------------------------------------------- 
 * Pin layout should be as follows:
 * Signal     Pin              Pin               Pin
 *            Arduino Uno      Arduino Mega      MFRC522 board
 * ------------------------------------------------------------
 * Reset      9                5                 RST
 * SPI SS     10               53                SDA
 * SPI MOSI   11               52                MOSI
 * SPI MISO   12               51                MISO
 * SPI SCK    13               50                SCK
 *
 * Hardware required:
 * Arduino
 * PCD (Proximity Coupling Device): NXP MFRC522 Contactless Reader IC
 * PICC (Proximity Integrated Circuit Card): A card or tag using the ISO 14443A interface, eg Mifare or NTAG203.
 * The reader can be found on eBay for around 5 dollars. Search for "mf-rc522" on ebay.com. 
 */

#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10    //Arduino Uno
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN);        // Create MFRC522 instance.

void setup() {
        Serial.begin(9600);        // Initialize serial communications with the PC
        SPI.begin();                // Init SPI bus
        mfrc522.PCD_Init();        // Init MFRC522 card
        Serial.println("Try the most used default keys to print block 0 of a MIFARE PICC ");
}


void try_key(MFRC522::MIFARE_Key *key)
{
        // try with the supplied key
        byte buffer[18];  
        byte block  = 0;
        byte status;
        //Serial.println("Authenticating using key A...");
        status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, key, &(mfrc522.uid));
        if (status != MFRC522::STATUS_OK) {
           // Serial.print("PCD_Authenticate() failed: ");
           // Serial.println(mfrc522.GetStatusCodeName(status));
           return;
        }
        
        // Read block
	byte byteCount = sizeof(buffer);
	status = mfrc522.MIFARE_Read(block, buffer, &byteCount);
	if (status != MFRC522::STATUS_OK) {
	    // Serial.print("MIFARE_Read() failed: ");
	    // Serial.println(mfrc522.GetStatusCodeName(status));
	}
        else  // Dump data
        {
          Serial.print("Success: key ");
          for (byte i = 0; i < 6; i++) Serial.print((*key).keyByte[i], HEX);
          Serial.print(" Block 0 : ");
	  for (byte index = 0; index < 16; index++) {
	    Serial.print(buffer[index] < 0x10 ? " 0" : " ");
	    Serial.print(buffer[index], HEX);
	    if ((index % 4) == 3) Serial.print(" ");
	  }
        }
        Serial.println(" ");
        mfrc522.PICC_HaltA(); // Halt PICC
        mfrc522.PCD_StopCrypto1();  // Stop encryption on PCD
       
}

void loop() {
        // Prepare key - all keys are set to FFFFFFFFFFFFh at chip delivery from the factory.
        MFRC522::MIFARE_Key k;
        // Look for new cards
        if ( ! mfrc522.PICC_IsNewCardPresent()) return;
      
        // Select one of the cards
        if ( ! mfrc522.PICC_ReadCardSerial())    return;
        
        Serial.print("Card UID:");    //Dump UID
        for (byte i = 0; i < mfrc522.uid.size; i++) {
          Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
          Serial.print(mfrc522.uid.uidByte[i], HEX);
        } 
        Serial.print(" PICC type: ");   // Dump PICC type
        byte piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
        Serial.println(mfrc522.PICC_GetTypeName(piccType));
         
        // Trying FFFFFFFFFFFF
        for (byte i = 0; i < 6; i++) k.keyByte[i] = 0xFF;
        try_key(&k); 
        // Trying A0A1A2A3A4A5
        k.keyByte[0] = 0xA0; k.keyByte[1] = 0xA1; k.keyByte[2] = 0xA2;  k.keyByte[3] = 0xA3; k.keyByte[4] = 0xA4; k.keyByte[5] = 0xA5;
        try_key(&k); 
        // Trying B0B1B2B3B4B5
        k.keyByte[0] = 0xB0; k.keyByte[1] = 0xB1; k.keyByte[2] = 0xB2;  k.keyByte[3] = 0xB3; k.keyByte[4] = 0xB4; k.keyByte[5] = 0xB5;
        try_key(&k); 
         // Trying 000000000000
        k.keyByte[0] = 0x00; k.keyByte[1] = 0x00; k.keyByte[2] = 0x00;  k.keyByte[3] = 0x00; k.keyByte[4] = 0x00; k.keyByte[5] = 0x00;
        try_key(&k); 
        // Trying 4d3a99c351dd
        k.keyByte[0] = 0x04d; k.keyByte[1] = 0x3a; k.keyByte[2] = 0x99;  k.keyByte[3] = 0xc3; k.keyByte[4] = 0x51; k.keyByte[5] = 0xdd;
        try_key(&k); 
        // Trying 1a982c7e459a
        k.keyByte[0] = 0x1a; k.keyByte[1] = 0x98; k.keyByte[2] = 0x2c;  k.keyByte[3] = 0x7e; k.keyByte[4] = 0x45; k.keyByte[5] = 0x9a;
        try_key(&k); 
        // Trying d3f7d3f7d3f7
        k.keyByte[0] = 0xd3; k.keyByte[1] = 0xf7; k.keyByte[2] = 0xd3;  k.keyByte[3] = 0xf7; k.keyByte[4] = 0xd3; k.keyByte[5] = 0xf7;
        try_key(&k); 
        // Trying aabbccddeeff
        k.keyByte[0] = 0xaa; k.keyByte[1] = 0xbb; k.keyByte[2] = 0xcc;  k.keyByte[3] = 0xdd; k.keyByte[4] = 0xee; k.keyByte[5] = 0xff;
        try_key(&k); 
}

