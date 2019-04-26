#include "MFRC522_EX_SPI.h"

MFRC522_EX_SPI::MFRC522_EX_SPI()  :MFRC522_EX_SPI(SS, UINT8_MAX){
}

MFRC522_EX_SPI::MFRC522_EX_SPI(byte resetPowerDownPin)  :MFRC522_EX_SPI(SS, resetPowerDownPin){
}

MFRC522_EX_SPI::MFRC522_EX_SPI(byte chipSelectPin, byte resetPowerDownPin)
  {
  _chipSelectPin = chipSelectPin;
  _resetPowerDownPin = resetPowerDownPin;
}

void MFRC522_EX_SPI::InstalSPI(SPIClass *_InstalSPI){
  ExSPI = _InstalSPI;
}

/////////////////////////////////////////////////////////////////////////////////////
// Basic interface functions for communicating with the MFRC522
/////////////////////////////////////////////////////////////////////////////////////

/**
 * Writes a byte to the specified register in the MFRC522 chip.
 * The interface is described in the datasheet section 8.1.2.
 */
void MFRC522_EX_SPI::PCD_WriteRegister(  PCD_Register reg, ///< The register to write to. One of the PCD_Register enums.
                  byte value      ///< The value to write.
                ) {
  ExSPI->beginTransaction(SPISettings(MFRC522_SPICLOCK, MSBFIRST, SPI_MODE0));  // Set the settings to work with SPI bus
  digitalWrite(_chipSelectPin, LOW);    // Select slave
  ExSPI->transfer(reg);           // MSB == 0 is for writing. LSB is not used in address. Datasheet section 8.1.2.3.
  ExSPI->transfer(value);
  digitalWrite(_chipSelectPin, HIGH);   // Release slave again
  ExSPI->endTransaction(); // Stop using the SPI bus
} // End PCD_WriteRegister()

/**
 * Writes a number of bytes to the specified register in the MFRC522 chip.
 * The interface is described in the datasheet section 8.1.2.
 */
void MFRC522_EX_SPI::PCD_WriteRegister( PCD_Register reg, ///< The register to write to. One of the PCD_Register enums.
                  byte count,     ///< The number of bytes to write to the register
                  byte *values    ///< The values to write. Byte array.
                ) {
  ExSPI->beginTransaction(SPISettings(MFRC522_SPICLOCK, MSBFIRST, SPI_MODE0));  // Set the settings to work with SPI bus
  digitalWrite(_chipSelectPin, LOW);    // Select slave
  ExSPI->transfer(reg);           // MSB == 0 is for writing. LSB is not used in address. Datasheet section 8.1.2.3.
  for (byte index = 0; index < count; index++) {
    ExSPI->transfer(values[index]);
  }
  digitalWrite(_chipSelectPin, HIGH);   // Release slave again
  ExSPI->endTransaction(); // Stop using the SPI bus
} // End PCD_WriteRegister()

/**
 * Reads a byte from the specified register in the MFRC522 chip.
 * The interface is described in the datasheet section 8.1.2.
 */
byte MFRC522_EX_SPI::PCD_ReadRegister(  PCD_Register reg  ///< The register to read from. One of the PCD_Register enums.
                ) {
  byte value;
  ExSPI->beginTransaction(SPISettings(MFRC522_SPICLOCK, MSBFIRST, SPI_MODE0));  // Set the settings to work with SPI bus
  digitalWrite(_chipSelectPin, LOW);      // Select slave
  ExSPI->transfer(0x80 | reg);          // MSB == 1 is for reading. LSB is not used in address. Datasheet section 8.1.2.3.
  value = ExSPI->transfer(0);         // Read the value back. Send 0 to stop reading.
  digitalWrite(_chipSelectPin, HIGH);     // Release slave again
  ExSPI->endTransaction(); // Stop using the SPI bus
  return value;
} // End PCD_ReadRegister()

/**
 * Reads a number of bytes from the specified register in the MFRC522 chip.
 * The interface is described in the datasheet section 8.1.2.
 */
void MFRC522_EX_SPI::PCD_ReadRegister(  PCD_Register reg, ///< The register to read from. One of the PCD_Register enums.
                byte count,     ///< The number of bytes to read
                byte *values,   ///< Byte array to store the values in.
                byte rxAlign    ///< Only bit positions rxAlign..7 in values[0] are updated.
                ) {
  if (count == 0) {
    return;
  }
  //Serial.print(F("Reading "));  Serial.print(count); Serial.println(F(" bytes from register."));
  byte address = 0x80 | reg;        // MSB == 1 is for reading. LSB is not used in address. Datasheet section 8.1.2.3.
  byte index = 0;             // Index in values array.
  ExSPI->beginTransaction(SPISettings(MFRC522_SPICLOCK, MSBFIRST, SPI_MODE0));  // Set the settings to work with SPI bus
  digitalWrite(_chipSelectPin, LOW);    // Select slave
  count--;                // One read is performed outside of the loop
  ExSPI->transfer(address);         // Tell MFRC522 which address we want to read
  if (rxAlign) {    // Only update bit positions rxAlign..7 in values[0]
    // Create bit mask for bit positions rxAlign..7
    byte mask = (0xFF << rxAlign) & 0xFF;
    // Read value and tell that we want to read the same address again.
    byte value = ExSPI->transfer(address);
    // Apply mask to both current value of values[0] and the new data in value.
    values[0] = (values[0] & ~mask) | (value & mask);
    index++;
  }
  while (index < count) {
    values[index] = ExSPI->transfer(address); // Read value and tell that we want to read the same address again.
    index++;
  }
  values[index] = ExSPI->transfer(0);     // Read the final byte. Send 0 to stop reading.
  digitalWrite(_chipSelectPin, HIGH);     // Release slave again
  ExSPI->endTransaction(); // Stop using the SPI bus
} // End PCD_ReadRegister()
