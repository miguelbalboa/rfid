/**
* Read a card using a mfrc522 reader on your SPI interface
* Pin layout should be as follows (on Arduino Uno):
* MOSI: Pin 11 / ICSP-4
* MISO: Pin 12 / ICSP-1
* SCK: Pin 13 / ISCP-3
* SS: Pin 10
* RST: Pin 9
*
* Script is based on the script of Miguel Balboa. 
* Serial number is shown on a HD44780 compatible display
*
* The circuit:
* LCD RS pin to digital pin (7)
* LCD Enable pin to digital pin (6)
* LCD D4 pin to digital pin 5
* LCD D5 pin to digital pin 4
* LCD D6 pin to digital pin 3
* LCD D7 pin to digital pin 2
* LCD R/W pin to ground
* 10K resistor:
* ends to +5V and ground
* wiper to LCD VO pin (pin 3)
*
* @version 0.1
* @author Henri de Jong
* @since 27-01-2013
*/

#include <SPI.h>
#include <RFID.h>
#include <LiquidCrystal.h>

#define SS_PIN 10
#define RST_PIN 9

RFID rfid(SS_PIN, RST_PIN); 

LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

// Setup variables:
    int serNum0;
    int serNum1;
    int serNum2;
    int serNum3;
    int serNum4;

void setup()
{ 
  Serial.begin(9600);
  lcd.begin(16, 2);
  SPI.begin(); 
  rfid.init();
  
}

void loop()
{
    
    if (rfid.isCard()) {
        if (rfid.readCardSerial()) {
            if (rfid.serNum[0] != serNum0
                && rfid.serNum[1] != serNum1
                && rfid.serNum[2] != serNum2
                && rfid.serNum[3] != serNum3
                && rfid.serNum[4] != serNum4
            ) {
                /* With a new cardnumber, show it. */
                Serial.println(" ");
                Serial.println("Card found");
                serNum0 = rfid.serNum[0];
                serNum1 = rfid.serNum[1];
                serNum2 = rfid.serNum[2];
                serNum3 = rfid.serNum[3];
                serNum4 = rfid.serNum[4];
               
                //Serial.println(" ");
                Serial.println("Cardnumber:");
                Serial.print("Dec: ");
		Serial.print(rfid.serNum[0],DEC);
                Serial.print(", ");
		Serial.print(rfid.serNum[1],DEC);
                Serial.print(", ");
		Serial.print(rfid.serNum[2],DEC);
                Serial.print(", ");
		Serial.print(rfid.serNum[3],DEC);
                Serial.print(", ");
		Serial.print(rfid.serNum[4],DEC);
                Serial.println(" ");
                        
                Serial.print("Hex: ");
		Serial.print(rfid.serNum[0],HEX);
                Serial.print(", ");
		Serial.print(rfid.serNum[1],HEX);
                Serial.print(", ");
		Serial.print(rfid.serNum[2],HEX);
                Serial.print(", ");
		Serial.print(rfid.serNum[3],HEX);
                Serial.print(", ");
		Serial.print(rfid.serNum[4],HEX);
                Serial.println(" ");

                /* Write the HEX code to the display */
                lcd.clear();                
                lcd.setCursor(0, 0);
                lcd.print("Cardno (hex):");
                lcd.setCursor(0,1);
                lcd.print(rfid.serNum[0], HEX);
                lcd.print(',');
                lcd.print(rfid.serNum[1], HEX);
                lcd.print(',');
                lcd.print(rfid.serNum[2], HEX);
                lcd.print(',');
                lcd.print(rfid.serNum[3], HEX);
                lcd.print(',');
                lcd.print(rfid.serNum[4], HEX);
             } else {
               /* If we have the same ID, just write a dot. */
               Serial.print(".");
             }
          }
    }
    
    rfid.halt();
}

