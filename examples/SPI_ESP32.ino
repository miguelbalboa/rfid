#include <SPI.h>
#include <MFRC522_EX_SPI.h>

// HSPI
#define SS_PIN 15
#define RST_PIN 27

// VSPI
#define SS_PIN1 5
#define RST_PIN1 21
 
MFRC522_EX_SPI rfid(SS_PIN, RST_PIN); // Instance of the class
MFRC522_EX_SPI rfid1(SS_PIN1, RST_PIN1); // Instance of the class

SPIClass * vspi = NULL;
SPIClass * hspi = NULL;

void setup() {
  vspi = new SPIClass(VSPI);
  hspi = new SPIClass(HSPI);

   //clock miso mosi ss

  //initialise vspi with default pins
  //SCLK = 18, MISO = 19, MOSI = 23, SS = 5
  vspi->begin();
  //alternatively route through GPIO pins of your choice
  //hspi->begin(0, 2, 4, 33); //SCLK, MISO, MOSI, SS
  
  //initialise hspi with default pins
  //SCLK = 14, MISO = 12, MOSI = 13, SS = 15
  hspi->begin(); 
  //alternatively route through GPIO pins
  //hspi->begin(25, 26, 27, 32); //SCLK, MISO, MOSI, SS

  //set up slave select pins as outputs as the Arduino API
  //doesn't handle automatically pulling SS low
  pinMode(SS_PIN1, OUTPUT); //VSPI SS
  pinMode(SS_PIN, OUTPUT); //HSPI SS

  rfid.InstalSPI(hspi);
  rfid1.InstalSPI(vspi);
}

void loop() {
  // put your main code here, to run repeatedly:

}
