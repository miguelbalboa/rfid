#include <I2CRC522.h>

/////////////////////////////////////////////////////////////////////////////////////
// Basic interface functions for communicating with the MFRC522
/////////////////////////////////////////////////////////////////////////////////////

/**
 * Writes a byte to the specified register in the MFRC522 chip.
 * The interface is described in the datasheet section 8.1.4. 
 */
void I2CRC522::PCD_WriteRegister(PCD_Register reg,	///< The register to write to. One of the PCD_Register enums.
				byte value		///< The value to write.
				) {
	//Serial.println("I2C write reg"); // for some reason it used the SPI one, from the parent class... Why?
	Wire.beginTransmission(_SlaveAdr);
	Wire.write(reg>>1); // shift reg back right, because using I2C and not SPI. (Using constants from base class, meant for SPI)

	Wire.write(value); 
	Wire.endTransmission();

} // End PCD_WriteRegister().

/**
 * Writes a number of bytes to the specified register in the MFRC522 chip.
 * The interface is described in the datasheet section 8.1.4.
 */
void I2CRC522::PCD_WriteRegister(PCD_Register reg,	///< The register to write to. One of the PCD_Register enums.
				byte count,		///< The number of bytes to write to the register
				byte *values		///< The values to write. Byte array.
				) {
	//Serial.println("I2C write reg multibyte"); // for some reason it used the SPI one, from the parent class... Why?
	
	Wire.beginTransmission(_SlaveAdr);
	Wire.write(reg>>1); // shift reg back right, because using I2C and not SPI. TODO: find better solution
	Wire.write(values,count);
	Wire.endTransmission();

} // End PCD_WriteRegister()

/**
 * Reads a byte from the specified register in the MFRC522 chip.
 * The interface is described in the datasheet section 8.1.4.
 */
byte I2CRC522::PCD_ReadRegister(	PCD_Register reg	///< The register to read from. One of the PCD_Register enums.
			      ) {
	//Serial.println("I2C read reg"); // for some reason it used the SPI one, from the parent class... Why?	
	byte value;
	Wire.beginTransmission(_SlaveAdr);
	Wire.write(reg>>1); // shift reg back right, because using I2C and not SPI. TODO: find better solution
	Wire.endTransmission(); 	// Bus restart	
	Wire.requestFrom(_SlaveAdr,1);
 		while(!Wire.available()); // Dangerous! Might block! Wait for byte to be available.
	value = Wire.read();
	Wire.endTransmission();

	return value;
} // End PCD_ReadRegister()

/**
 * Reads a number of bytes from the specified register in the MFRC522 chip.
 * The interface is described in the datasheet section 8.1.2.
 */
void I2CRC522::PCD_ReadRegister(PCD_Register reg,	///< The register to read from. One of the PCD_Register enums.
				byte count,		///< The number of bytes to read
				byte *values,		///< Byte array to store the values in.
				byte rxAlign		///< Only bit positions rxAlign..7 in values[0] are updated.
				) {
	//Serial.println("I2C read reg multibyte");	

	byte index = 0;
	
	if (count == 0) {
		return;
	}

	Wire.beginTransmission(_SlaveAdr);
	Wire.write(reg>>1); 		// shift reg back right, because using I2C and not SPI.
					// Tell MFRC522 which address we want to read
	Wire.endTransmission(); 	// Bus restart
	
	Wire.requestFrom(_SlaveAdr,count);

	// with thanks to arozcan (https://github.com/arozcan/MFRC522-I2C-Library),  but slightly modified: {
 	while (Wire.available() && index<count) {
		if (index == 0 && rxAlign) {		// Only update bit positions rxAlign..7 in values[0]
			// Create bit mask for bit positions rxAlign..7
			byte mask = 0;
			for (byte i = rxAlign; i <= 7; i++) {
				mask |= (1 << i);
			}
			// Read value and tell that we want to read the same address again.
			byte value = Wire.read();
			// Apply mask to both current value of values[0] and the new data in value.
			values[0] = (values[index] & ~mask) | (value & mask);
		}
		else { // Normal case
			values[index] = Wire.read();
		}
		index++;
	} // }
} // End PCD_ReadRegister()

/*
#define TEHMAGICVALUE 0x80
void I2CRC522::SwitchToHighSpeedI2C(){
	Wire.begintransmission(TEHMAGICVALUE); // no, should not send slave addr, just TEHMAGICVALUE. 
	//Wire.write(TEHMAGICVALUE);
	Wire.endTransmission();
	Wire.setClock(3400000);
	// TODO: set the bit that switches RC522 to HS without it needing TEHMAGICVALUE again, because otherwise this won't work at all.
};
*/
