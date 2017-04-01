/*
 * Reads data written by a program such as "rfid_write_personal_data.ino" 
 * 
 * See: https://github.com/miguelbalboa/rfid/tree/master/examples/rfid_write_personal_data
 * 
 * Uses MIFARE RFID card using RFID-RC522 reader
 * Uses MFRC522 - Library  
 * -----------------------------------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
 *             Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
 * Signal      Pin          Pin           Pin       Pin        Pin              Pin
 * -----------------------------------------------------------------------------------------
 * RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
 * SPI SS      SDA(SS)      10            53        D10        10               10
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
 * SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
 * SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
 *
 */
 
#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN         9           //see pin layout
#define SS_PIN          10          //see pin layout

MFRC522 card(SS_PIN, RST_PIN);   // Create MFRC522 object 'card'

//*****************************************************************************************//
void setup() {
        Serial.begin(9600);                                           // Initialize serial communications with the PC
        SPI.begin();                                                  // Init SPI bus
        card.PCD_Init();                                              // Init MFRC522 card
        Serial.println(F("Read personal data on a MIFARE PICC:"));    //shows in serial that it is ready to read
}

//*****************************************************************************************//
void loop() {

        //create a default key
        //(all keys are set to FFFFFFFFFFFFh at chip delivery from the factory)
        MFRC522::MIFARE_Key key;
        for (byte i = 0; i < 6; i++) 
        {
          key.keyByte[i] = 0xFF;
        }

        //some variables we need
        byte block;
        byte len;
        MFRC522::StatusCode status;
  
        //-------------------------------------------
        
        //look for card
        if ( ! card.PICC_IsNewCardPresent()) {
                return;
        }

        //select a card
        if ( ! card.PICC_ReadCardSerial()) {
          return;
        }

        Serial.println("**Card Detected:**");

        //-------------------------------------------
        
        card.PICC_DumpDetailsToSerial(&(card.uid)); //dump some details about the card
        
        //card.PICC_DumpToSerial(&(card.uid));      //uncomment this to see all blocks in hex

        //-------------------------------------------
        
        Serial.print("Name: ");
        
        byte buffer1[18];
        
        block = 4;
        len = 18;

        //------------------------------------------- GET FIRST NAME
        status = card.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 4, &key, &(card.uid)); //line 834 of MFRC522.cpp file
        if (status != MFRC522::STATUS_OK) {
           Serial.print(F("Authentication failed: "));
           Serial.println(card.GetStatusCodeName(status));
           return;
        }
        
        status = card.MIFARE_Read(block, buffer1, &len); 
        if (status != MFRC522::STATUS_OK) {
           Serial.print(F("Reading failed: "));
           Serial.println(card.GetStatusCodeName(status));
           return;
        }

        //PRINT FIRST NAME
        for(int i = 0; i < 16; i++)
        {
          if(buffer1[i] != 32)
          {
            Serial.write(buffer1[i]);
          }
        }
        Serial.print(" ");

        //---------------------------------------- GET LAST NAME

        byte buffer2[18];
        block = 1;
        
        status = card.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 1, &key, &(card.uid)); //line 834
        if (status != MFRC522::STATUS_OK) {
           Serial.print(F("Authentication failed: "));
           Serial.println(card.GetStatusCodeName(status));
           return;
        }
        
        status = card.MIFARE_Read(block, buffer2, &len); 
        if (status != MFRC522::STATUS_OK) {
           Serial.print(F("Reading failed: "));
           Serial.println(card.GetStatusCodeName(status));
           return;
        }

        //PRINT LAST NAME
        for(int i = 0; i < 16; i++)
        {
          Serial.write(buffer2[i] );
        }


        //----------------------------------------
        
        Serial.println("\n**End Reading**\n");
        
        delay(1000); //change value if you want to read cards faster

        card.PICC_HaltA();
        card.PCD_StopCrypto1();
}
