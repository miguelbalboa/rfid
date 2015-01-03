/*
 * MFRC522 - Library to use ARDUINO RFID MODULE KIT 13.56 MHZ WITH TAGS SPI W AND R BY COOQROBOT.
 * The library file MFRC522.h has a wealth of useful info. Please read it.
 * The functions are documented in MFRC522.cpp.
 *
 * Based on code Dr.Leong   ( WWW.B2CQSHOP.COM )
 * Created by Miguel Balboa (circuitito.com), Jan, 2012.
 * Rewritten by Søren Thing Andersen (access.thing.dk), fall of 2013 (Translation to English, refactored, comments, anti collision, cascade levels.)
 * Extended by Tom Clement with functionality to write to sector 0 of UID changeable Mifare cards.
 * 
 * Released into the public domain.
 *
 * This sample shows how to set the UID on a UID changeable MIFARE card.
 * 
 ----------------------------------------------------------------------------- empty_skull 
 
 - Aggiunti pin per arduino Mega
 - Scritto semplice codice per la scrittura e lettura 
 
 - add pin configuration for arduino mega
 - write simple read/write Code for new entry user
 
 http://mac86project.altervista.org/
 
 ----------------------------------------------------------------------------- Nicola Coppola
 * Pin layout should be as follows:
 * Signal     Pin              Pin               Pin
 *            Arduino Uno      Arduino Mega      MFRC522 board
 * ------------------------------------------------------------
 * Reset      9                5                 RST
 * SPI SS     10               53                SDA
 * SPI MOSI   11               51                MOSI
 * SPI MISO   12               50                MISO
 * SPI SCK    13               52                SCK
 *
 * The reader can be found on eBay for around 5 dollars. Search for "mf-rc522" on ebay.com. 
 */

#include <SPI.h>
#include <MFRC522.h>

/* Set your new UID here! */
#define NEW_UID {0xDE, 0xAD, 0xBE, 0xEF}
#define SS_PIN 10
#define RST_PIN 9

MFRC522 mfrc522(SS_PIN, RST_PIN);        // Create MFRC522 instance.
MFRC522::MIFARE_Key key;

void setup() {
        Serial.begin(9600);        // Initialize serial communications with the PC
        SPI.begin();                // Init SPI bus
        mfrc522.PCD_Init();        // Init MFRC522 card
        Serial.println("Warning: this example overwrites the UID of your UID changeable card, use with care!");
        
        // Prepare key - all keys are set to FFFFFFFFFFFFh at chip delivery from the factory.
        for (byte i = 0; i < 6; i++) {
                key.keyByte[i] = 0xFF;
        }
}

// Setting the UID can be as simple as this:
//void loop() {
//    byte newUid[] = NEW_UID;
//    if ( mfrc522.MIFARE_SetUid(newUid, (byte)4, true) ) {
//      Serial.println("Wrote new UID to card.");
//    }
//    delay(1000);
//}

// But of course this is a more proper approach
void loop() {
  
        // Look for new cards, and select one if present
        if ( ! mfrc522.PICC_IsNewCardPresent() || ! mfrc522.PICC_ReadCardSerial() ) {
            delay(50);
            return;
        }
        
        // Now a card is selected. The UID and SAK is in mfrc522.uid.
        
        // Dump UID
        Serial.print("Card UID:");
        for (byte i = 0; i < mfrc522.uid.size; i++) {
                Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
                Serial.print(mfrc522.uid.uidByte[i], HEX);
        } 
        Serial.println();

        // Dump PICC type
//        byte piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
//        Serial.print("PICC type: ");
//        Serial.print(mfrc522.PICC_GetTypeName(piccType));
//        Serial.print(" (SAK ");
//        Serial.print(mfrc522.uid.sak);
//        Serial.print(")\r\n");
//        if (        piccType != MFRC522::PICC_TYPE_MIFARE_MINI 
//                &&        piccType != MFRC522::PICC_TYPE_MIFARE_1K
//                &&        piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
//                Serial.println("This sample only works with MIFARE Classic cards.");
//                return;
//        }
        
        // Set new UID
        byte newUid[] = NEW_UID;
        if ( mfrc522.MIFARE_SetUid(newUid, (byte)4, true) ) {
            Serial.println("Wrote new UID to card.");
        }
        
        // Halt PICC and re-select it so DumpToSerial doesn't get confused
        mfrc522.PICC_HaltA();
        if ( ! mfrc522.PICC_IsNewCardPresent() || ! mfrc522.PICC_ReadCardSerial() ) {
                return;
        }
        
        // Dump the new memory contents
        Serial.println("New UID and contents:");
        mfrc522.PICC_DumpToSerial(&(mfrc522.uid));
        
        delay(2000);
}
