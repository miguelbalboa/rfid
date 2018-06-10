/*
 * --------------------------------------------------------------------------------------------------------------------
 * Example sketch/program showing how to read data from a PICC to serial. Modified to use I2C instead of SPI.
 * --------------------------------------------------------------------------------------------------------------------
 * This is a I2CRC522 library example; for further details and other examples see: https://github.com/miguelbalboa/rfid 
*									 	and https://github.com/lukelectro/rfid
 * 
 * Example sketch/program showing how to read data from a PICC (that is: a RFID Tag or Card) using a MFRC522 based RFID
 * Reader on the Arduino I2C interface.
 * 
 * When the Arduino and the MFRC522 module are connected (see the pin layout below), load this sketch into Arduino IDE
 * then verify/compile and upload it. To see the output: use Tools, Serial Monitor of the IDE (hit Ctrl+Shft+M). When
 * you present a PICC (that is: a RFID Tag or Card) at reading distance of the MFRC522 Reader/PCD, the serial output
 * will show the ID/UID, type and any data blocks it can read. Note: you may see "Timeout in communication" messages
 * when removing the PICC from reading distance too early.
 * 
 * If your reader supports it, this sketch/program will read all the PICCs presented (that is: multiple tag reading).
 * So if you stack two or more PICCs on top of each other and present them to the reader, it will first output all
 * details of the first and then the next PICC. Note that this may take some time as all data blocks are dumped, so
 * keep the PICCs at reading distance until complete.
 * 
 * @license Released into the public domain.
 * 
 * Typical pin layout used:
 * ------------------------------------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino      
 *             Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro    
 * Signal      Pin          Pin           Pin       Pin        Pin              Pin          
 * ------------------------------------------------------------------------------------------
 * RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST             
 * I2C SCL     SCL          A5(SCL)      21(SCL)    A5(SCL)    3(SCL)           A5(SCL)         
 * I2C SDA     SDA          A4(SDA)      20(SDA)    A4(SDA)    2(SDA)           A4(SDA)         
 *                                                                                              
 * The other example sketches can be modified in the same way.
 */

#include <Wire.h>              // To use I2C and not spi, replace SPI class with Wire class
#include <I2CRC522.h>         //  and replace MFRC522 with I2CRC522 


constexpr uint8_t RST_PIN = 9; // Configurable, see typical pin layout above

I2CRC522 mfrc522(RST_PIN);  // Create MFRC522 instance

void setup() {
	Serial.begin(9600);		// Initialize serial communications with the PC
	while (!Serial);		  // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
	//SPI.begin();			  // Init SPI bus (Replace this with I2C bus initialisation)
  Wire.setClock(400000);
  Wire.begin();         // Init I2C bus
	mfrc522.PCD_Init();		// Init MFRC522
	mfrc522.PCD_DumpVersionToSerial();	// Show details of PCD - MFRC522 Card Reader details
	Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));
}

void loop() {
	// Look for new cards
	if ( ! mfrc522.PICC_IsNewCardPresent()) {
		return;
	}

	// Select one of the cards
	if ( ! mfrc522.PICC_ReadCardSerial()) {
		return;
	}

	// Dump debug info about the card; PICC_HaltA() is automatically called
	mfrc522.PICC_DumpToSerial(&(mfrc522.uid));
}
