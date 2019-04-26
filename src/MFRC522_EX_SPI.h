#ifndef MFRC522_EX_SPI_h
#define MFRC522_EX_SPI_h

#include <stdint.h>
#include <Arduino.h>
#include <SPI.h>

#include <MFRC522.h>

class MFRC522_EX_SPI :public MFRC522 {
  public: 
    MFRC522_EX_SPI();
    MFRC522_EX_SPI(byte resetPowerDownPin);
    MFRC522_EX_SPI(byte chipSelectPin, byte resetPowerDownPin);
    
    void InstalSPI(SPIClass * _InstalSPI);
    
    void PCD_WriteRegister(PCD_Register reg, byte value);
    void PCD_WriteRegister(PCD_Register reg, byte count, byte *values);
    byte PCD_ReadRegister(PCD_Register reg);
    void PCD_ReadRegister(PCD_Register reg, byte count, byte *values, byte rxAlign = 0);
  private:
    SPIClass * ExSPI;
};


#endif
