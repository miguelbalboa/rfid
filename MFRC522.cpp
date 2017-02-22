/*
* MFRC522.cpp - Library to use ARDUINO RFID MODULE KIT 13.56 MHZ WITH TAGS SPI W AND R BY COOQROBOT.
* NOTE: Please also check the comments in MFRC522.h - they provide useful hints and background information.
* Released into the public domain.
*/

#include <Arduino.h>
#include <MFRC522.h>

/////////////////////////////////////////////////////////////////////////////////////
// Functions for setting up the Arduino
/////////////////////////////////////////////////////////////////////////////////////
/**
 * Constructor.
 */
MFRC522::MFRC522(): MFRC522(SS, UINT8_MAX) { // SS is defined in pins_arduino.h, UINT8_MAX means there is no connection from Arduino to MFRC522's reset and power down input
} // End constructor

/**
 * Constructor.
 * Prepares the output pins.
 */
MFRC522::MFRC522(	byte resetPowerDownPin	///< Arduino pin connected to MFRC522's reset and power down input (Pin 6, NRSTPD, active low). If there is no connection from the CPU to NRSTPD, set this to UINT8_MAX. In this case, only soft reset will be used in PCD_Init().
				): MFRC522(SS, resetPowerDownPin) { // SS is defined in pins_arduino.h
} // End constructor

/**
 * Constructor.
 * Prepares the output pins.
 */
MFRC522::MFRC522(	byte chipSelectPin,		///< Arduino pin connected to MFRC522's SPI slave select input (Pin 24, NSS, active low)
					byte resetPowerDownPin	///< Arduino pin connected to MFRC522's reset and power down input (Pin 6, NRSTPD, active low). If there is no connection from the CPU to NRSTPD, set this to UINT8_MAX. In this case, only soft reset will be used in PCD_Init().
				) {
	_chipSelectPin = chipSelectPin;
	_resetPowerDownPin = resetPowerDownPin;
} // End constructor

/////////////////////////////////////////////////////////////////////////////////////
// Basic interface functions for communicating with the MFRC522
/////////////////////////////////////////////////////////////////////////////////////

/**
 * Writes a byte to the specified register in the MFRC522 chip.
 * The interface is described in the datasheet section 8.1.2.
 */
void MFRC522::PCD_WriteRegister(	PCD_Register reg,	///< The register to write to. One of the PCD_Register enums.
									byte value			///< The value to write.
								) {
	SPI.beginTransaction(SPISettings(MFRC522_SPICLOCK, MSBFIRST, SPI_MODE0));	// Set the settings to work with SPI bus
	digitalWrite(_chipSelectPin, LOW);		// Select slave
	SPI.transfer(reg);						// MSB == 0 is for writing. LSB is not used in address. Datasheet section 8.1.2.3.
	SPI.transfer(value);
	digitalWrite(_chipSelectPin, HIGH);		// Release slave again
	SPI.endTransaction(); // Stop using the SPI bus
} // End PCD_WriteRegister()

/**
 * Writes a number of bytes to the specified register in the MFRC522 chip.
 * The interface is described in the datasheet section 8.1.2.
 */
void MFRC522::PCD_WriteRegister(	PCD_Register reg,	///< The register to write to. One of the PCD_Register enums.
									byte count,			///< The number of bytes to write to the register
									byte *values		///< The values to write. Byte array.
								) {
	SPI.beginTransaction(SPISettings(MFRC522_SPICLOCK, MSBFIRST, SPI_MODE0));	// Set the settings to work with SPI bus
	digitalWrite(_chipSelectPin, LOW);		// Select slave
	SPI.transfer(reg);						// MSB == 0 is for writing. LSB is not used in address. Datasheet section 8.1.2.3.
	for (byte index = 0; index < count; index++) {
		SPI.transfer(values[index]);
	}
	digitalWrite(_chipSelectPin, HIGH);		// Release slave again
	SPI.endTransaction(); // Stop using the SPI bus
} // End PCD_WriteRegister()

/**
 * Reads a byte from the specified register in the MFRC522 chip.
 * The interface is described in the datasheet section 8.1.2.
 */
byte MFRC522::PCD_ReadRegister(	PCD_Register reg	///< The register to read from. One of the PCD_Register enums.
								) {
	byte value;
	SPI.beginTransaction(SPISettings(MFRC522_SPICLOCK, MSBFIRST, SPI_MODE0));	// Set the settings to work with SPI bus
	digitalWrite(_chipSelectPin, LOW);			// Select slave
	SPI.transfer(0x80 | reg);					// MSB == 1 is for reading. LSB is not used in address. Datasheet section 8.1.2.3.
	value = SPI.transfer(0);					// Read the value back. Send 0 to stop reading.
	digitalWrite(_chipSelectPin, HIGH);			// Release slave again
	SPI.endTransaction(); // Stop using the SPI bus
	return value;
} // End PCD_ReadRegister()

/**
 * Reads a number of bytes from the specified register in the MFRC522 chip.
 * The interface is described in the datasheet section 8.1.2.
 */
void MFRC522::PCD_ReadRegister(	PCD_Register reg,	///< The register to read from. One of the PCD_Register enums.
								byte count,			///< The number of bytes to read
								byte *values,		///< Byte array to store the values in.
								byte rxAlign		///< Only bit positions rxAlign..7 in values[0] are updated.
								) {
	if (count == 0) {
		return;
	}
	//Serial.print(F("Reading ")); 	Serial.print(count); Serial.println(F(" bytes from register."));
	byte address = 0x80 | reg;				// MSB == 1 is for reading. LSB is not used in address. Datasheet section 8.1.2.3.
	byte index = 0;							// Index in values array.
	SPI.beginTransaction(SPISettings(MFRC522_SPICLOCK, MSBFIRST, SPI_MODE0));	// Set the settings to work with SPI bus
	digitalWrite(_chipSelectPin, LOW);		// Select slave
	count--;								// One read is performed outside of the loop
	SPI.transfer(address);					// Tell MFRC522 which address we want to read
	if (rxAlign) {		// Only update bit positions rxAlign..7 in values[0]
		// Create bit mask for bit positions rxAlign..7
		byte mask = (0xFF << rxAlign) & 0xFF;
		// Read value and tell that we want to read the same address again.
		byte value = SPI.transfer(address);
		// Apply mask to both current value of values[0] and the new data in value.
		values[0] = (values[0] & ~mask) | (value & mask);
		index++;
	}
	while (index < count) {
		values[index] = SPI.transfer(address);	// Read value and tell that we want to read the same address again.
		index++;
	}
	values[index] = SPI.transfer(0);			// Read the final byte. Send 0 to stop reading.
	digitalWrite(_chipSelectPin, HIGH);			// Release slave again
	SPI.endTransaction(); // Stop using the SPI bus
} // End PCD_ReadRegister()

/**
 * Sets the bits given in mask in register reg.
 */
void MFRC522::PCD_SetRegisterBitMask(	PCD_Register reg,	///< The register to update. One of the PCD_Register enums.
										byte mask			///< The bits to set.
									) { 
	byte tmp;
	tmp = PCD_ReadRegister(reg);
	PCD_WriteRegister(reg, tmp | mask);			// set bit mask
} // End PCD_SetRegisterBitMask()

/**
 * Clears the bits given in mask from register reg.
 */
void MFRC522::PCD_ClearRegisterBitMask(	PCD_Register reg,	///< The register to update. One of the PCD_Register enums.
										byte mask			///< The bits to clear.
									  ) {
	byte tmp;
	tmp = PCD_ReadRegister(reg);
	PCD_WriteRegister(reg, tmp & (~mask));		// clear bit mask
} // End PCD_ClearRegisterBitMask()


/**
 * Use the CRC coprocessor in the MFRC522 to calculate a CRC_A.
 * 
 * @return STATUS_OK on success, STATUS_??? otherwise.
 */
MFRC522::StatusCode MFRC522::PCD_CalculateCRC(	byte *data,		///< In: Pointer to the data to transfer to the FIFO for CRC calculation.
												byte length,	///< In: The number of bytes to transfer.
												byte *result	///< Out: Pointer to result buffer. Result is written to result[0..1], low byte first.
					 ) {
	PCD_WriteRegister(CommandReg, PCD_Idle);		// Stop any active command.
	PCD_WriteRegister(DivIrqReg, 0x04);				// Clear the CRCIRq interrupt request bit
	PCD_WriteRegister(FIFOLevelReg, 0x80);			// FlushBuffer = 1, FIFO initialization
	PCD_WriteRegister(FIFODataReg, length, data);	// Write data to the FIFO
	PCD_WriteRegister(CommandReg, PCD_CalcCRC);		// Start the calculation
	
	// Wait for the CRC calculation to complete. Each iteration of the while-loop takes 17.73μs.
	// TODO check/modify for other architectures than Arduino Uno 16bit

	// Wait for the CRC calculation to complete. Each iteration of the while-loop takes 17.73us.
	for (uint16_t i = 5000; i > 0; i--) {
		// DivIrqReg[7..0] bits are: Set2 reserved reserved MfinActIRq reserved CRCIRq reserved reserved
		byte n = PCD_ReadRegister(DivIrqReg);
		if (n & 0x04) {									// CRCIRq bit set - calculation done
			PCD_WriteRegister(CommandReg, PCD_Idle);	// Stop calculating CRC for new content in the FIFO.
			// Transfer the result from the registers to the result buffer
			result[0] = PCD_ReadRegister(CRCResultRegL);
			result[1] = PCD_ReadRegister(CRCResultRegH);
			return STATUS_OK;
		}
	}
	// 89ms passed and nothing happend. Communication with the MFRC522 might be down.
	return STATUS_TIMEOUT;
} // End PCD_CalculateCRC()


/////////////////////////////////////////////////////////////////////////////////////
// Functions for manipulating the MFRC522
/////////////////////////////////////////////////////////////////////////////////////

/**
 * Initializes the MFRC522 chip.
 */
void MFRC522::PCD_Init() {
	bool hardReset = false;

	// Set the chipSelectPin as digital output, do not select the slave yet
	pinMode(_chipSelectPin, OUTPUT);
	digitalWrite(_chipSelectPin, HIGH);
	
	// If a valid pin number has been set, pull device out of power down / reset state.
	if (_resetPowerDownPin != UINT8_MAX) {
		// Set the resetPowerDownPin as digital output, do not reset or power down.
		pinMode(_resetPowerDownPin, OUTPUT);
	
		if (digitalRead(_resetPowerDownPin) == LOW) {	// The MFRC522 chip is in power down mode.
			digitalWrite(_resetPowerDownPin, HIGH);		// Exit power down mode. This triggers a hard reset.
			// Section 8.8.2 in the datasheet says the oscillator start-up time is the start up time of the crystal + 37,74μs. Let us be generous: 50ms.
			delay(50);
			hardReset = true;
		}
	}

	if (!hardReset) { // Perform a soft reset if we haven't triggered a hard reset above.
		PCD_Reset();
	}
	
	// Reset baud rates
	PCD_WriteRegister(TxModeReg, 0x00);
	PCD_WriteRegister(RxModeReg, 0x00);
	// Reset ModWidthReg
	PCD_WriteRegister(ModWidthReg, 0x26);

	// When communicating with a PICC we need a timeout if something goes wrong.
	// f_timer = 13.56 MHz / (2*TPreScaler+1) where TPreScaler = [TPrescaler_Hi:TPrescaler_Lo].
	// TPrescaler_Hi are the four low bits in TModeReg. TPrescaler_Lo is TPrescalerReg.
	PCD_WriteRegister(TModeReg, 0x80);			// TAuto=1; timer starts automatically at the end of the transmission in all communication modes at all speeds
	PCD_WriteRegister(TPrescalerReg, 0xA9);		// TPreScaler = TModeReg[3..0]:TPrescalerReg, ie 0x0A9 = 169 => f_timer=40kHz, ie a timer period of 25μs.
	PCD_WriteRegister(TReloadRegH, 0x03);		// Reload timer with 0x3E8 = 1000, ie 25ms before timeout.
	PCD_WriteRegister(TReloadRegL, 0xE8);
	
	PCD_WriteRegister(TxASKReg, 0x40);		// Default 0x00. Force a 100 % ASK modulation independent of the ModGsPReg register setting
	PCD_WriteRegister(ModeReg, 0x3D);		// Default 0x3F. Set the preset value for the CRC coprocessor for the CalcCRC command to 0x6363 (ISO 14443-3 part 6.2.4)
	PCD_AntennaOn();						// Enable the antenna driver pins TX1 and TX2 (they were disabled by the reset)
} // End PCD_Init()

/**
 * Initializes the MFRC522 chip.
 */
void MFRC522::PCD_Init(	byte resetPowerDownPin	///< Arduino pin connected to MFRC522's reset and power down input (Pin 6, NRSTPD, active low)
					) {
	PCD_Init(SS, resetPowerDownPin); // SS is defined in pins_arduino.h
} // End PCD_Init()

/**
 * Initializes the MFRC522 chip.
 */
void MFRC522::PCD_Init(	byte chipSelectPin,		///< Arduino pin connected to MFRC522's SPI slave select input (Pin 24, NSS, active low)
						byte resetPowerDownPin	///< Arduino pin connected to MFRC522's reset and power down input (Pin 6, NRSTPD, active low)
					) {
	_chipSelectPin = chipSelectPin;
	_resetPowerDownPin = resetPowerDownPin; 
	// Set the chipSelectPin as digital output, do not select the slave yet
	PCD_Init();
} // End PCD_Init()

/**
 * Performs a soft reset on the MFRC522 chip and waits for it to be ready again.
 */
void MFRC522::PCD_Reset() {
	PCD_WriteRegister(CommandReg, PCD_SoftReset);	// Issue the SoftReset command.
	// The datasheet does not mention how long the SoftRest command takes to complete.
	// But the MFRC522 might have been in soft power-down mode (triggered by bit 4 of CommandReg) 
	// Section 8.8.2 in the datasheet says the oscillator start-up time is the start up time of the crystal + 37,74μs. Let us be generous: 50ms.
	delay(50);
	// Wait for the PowerDown bit in CommandReg to be cleared
	while (PCD_ReadRegister(CommandReg) & (1<<4)) {
		// PCD still restarting - unlikely after waiting 50ms, but better safe than sorry.
	}
} // End PCD_Reset()

/**
 * Turns the antenna on by enabling pins TX1 and TX2.
 * After a reset these pins are disabled.
 */
void MFRC522::PCD_AntennaOn() {
	byte value = PCD_ReadRegister(TxControlReg);
	if ((value & 0x03) != 0x03) {
		PCD_WriteRegister(TxControlReg, value | 0x03);
	}
} // End PCD_AntennaOn()

/**
 * Turns the antenna off by disabling pins TX1 and TX2.
 */
void MFRC522::PCD_AntennaOff() {
	PCD_ClearRegisterBitMask(TxControlReg, 0x03);
} // End PCD_AntennaOff()

/**
 * Get the current MFRC522 Receiver Gain (RxGain[2:0]) value.
 * See 9.3.3.6 / table 98 in http://www.nxp.com/documents/data_sheet/MFRC522.pdf
 * NOTE: Return value scrubbed with (0x07<<4)=01110000b as RCFfgReg may use reserved bits.
 * 
 * @return Value of the RxGain, scrubbed to the 3 bits used.
 */
byte MFRC522::PCD_GetAntennaGain() {
	return PCD_ReadRegister(RFCfgReg) & (0x07<<4);
} // End PCD_GetAntennaGain()

/**
 * Set the MFRC522 Receiver Gain (RxGain) to value specified by given mask.
 * See 9.3.3.6 / table 98 in http://www.nxp.com/documents/data_sheet/MFRC522.pdf
 * NOTE: Given mask is scrubbed with (0x07<<4)=01110000b as RCFfgReg may use reserved bits.
 */
void MFRC522::PCD_SetAntennaGain(byte mask) {
	if (PCD_GetAntennaGain() != mask) {						// only bother if there is a change
		PCD_ClearRegisterBitMask(RFCfgReg, (0x07<<4));		// clear needed to allow 000 pattern
		PCD_SetRegisterBitMask(RFCfgReg, mask & (0x07<<4));	// only set RxGain[2:0] bits
	}
} // End PCD_SetAntennaGain()

/**
 * Performs a self-test of the MFRC522
 * See 16.1.1 in http://www.nxp.com/documents/data_sheet/MFRC522.pdf
 * 
 * @return Whether or not the test passed. Or false if no firmware reference is available.
 */
bool MFRC522::PCD_PerformSelfTest() {
	// This follows directly the steps outlined in 16.1.1
	// 1. Perform a soft reset.
	PCD_Reset();
	
	// 2. Clear the internal buffer by writing 25 bytes of 00h
	byte ZEROES[25] = {0x00};
	PCD_WriteRegister(FIFOLevelReg, 0x80);		// flush the FIFO buffer
	PCD_WriteRegister(FIFODataReg, 25, ZEROES);	// write 25 bytes of 00h to FIFO
	PCD_WriteRegister(CommandReg, PCD_Mem);		// transfer to internal buffer
	
	// 3. Enable self-test
	PCD_WriteRegister(AutoTestReg, 0x09);
	
	// 4. Write 00h to FIFO buffer
	PCD_WriteRegister(FIFODataReg, 0x00);
	
	// 5. Start self-test by issuing the CalcCRC command
	PCD_WriteRegister(CommandReg, PCD_CalcCRC);
	
	// 6. Wait for self-test to complete
	byte n;
	for (uint8_t i = 0; i < 0xFF; i++) {
		// The datasheet does not specify exact completion condition except
		// that FIFO buffer should contain 64 bytes.
		// While selftest is initiated by CalcCRC command
		// it behaves differently from normal CRC computation,
		// so one can't reliably use DivIrqReg to check for completion.
		// It is reported that some devices does not trigger CRCIRq flag
		// during selftest.
		n = PCD_ReadRegister(FIFOLevelReg);
		if (n >= 64) {
			break;
		}
	}
	PCD_WriteRegister(CommandReg, PCD_Idle);		// Stop calculating CRC for new content in the FIFO.
	
	// 7. Read out resulting 64 bytes from the FIFO buffer.
	byte result[64];
	PCD_ReadRegister(FIFODataReg, 64, result, 0);
	
	// Auto self-test done
	// Reset AutoTestReg register to be 0 again. Required for normal operation.
	PCD_WriteRegister(AutoTestReg, 0x00);
	
	// Determine firmware version (see section 9.3.4.8 in spec)
	byte version = PCD_ReadRegister(VersionReg);
	
	// Pick the appropriate reference values
	const byte *reference;
	switch (version) {
		case 0x88:	// Fudan Semiconductor FM17522 clone
			reference = FM17522_firmware_reference;
			break;
		case 0x90:	// Version 0.0
			reference = MFRC522_firmware_referenceV0_0;
			break;
		case 0x91:	// Version 1.0
			reference = MFRC522_firmware_referenceV1_0;
			break;
		case 0x92:	// Version 2.0
			reference = MFRC522_firmware_referenceV2_0;
			break;
		default:	// Unknown version
			return false; // abort test
	}
	
	// Verify that the results match up to our expectations
	for (uint8_t i = 0; i < 64; i++) {
		if (result[i] != pgm_read_byte(&(reference[i]))) {
			return false;
		}
	}
	
	// Test passed; all is good.
	return true;
} // End PCD_PerformSelfTest()

/////////////////////////////////////////////////////////////////////////////////////
// Functions for communicating with PICCs
/////////////////////////////////////////////////////////////////////////////////////

/**
 * Executes the Transceive command.
 * CRC validation can only be done if backData and backLen are specified.
 * 
 * @return STATUS_OK on success, STATUS_??? otherwise.
 */
MFRC522::StatusCode MFRC522::PCD_TransceiveData(	byte *sendData,		///< Pointer to the data to transfer to the FIFO.
													byte sendLen,		///< Number of bytes to transfer to the FIFO.
													byte *backData,		///< NULL or pointer to buffer if data should be read back after executing the command.
													byte *backLen,		///< In: Max number of bytes to write to *backData. Out: The number of bytes returned.
													byte *validBits,	///< In/Out: The number of valid bits in the last byte. 0 for 8 valid bits. Default NULL.
													byte rxAlign,		///< In: Defines the bit position in backData[0] for the first bit received. Default 0.
													bool checkCRC		///< In: True => The last two bytes of the response is assumed to be a CRC_A that must be validated.
								 ) {
	byte waitIRq = 0x30;		// RxIRq and IdleIRq
	return PCD_CommunicateWithPICC(PCD_Transceive, waitIRq, sendData, sendLen, backData, backLen, validBits, rxAlign, checkCRC);
} // End PCD_TransceiveData()

/**
 * Transfers data to the MFRC522 FIFO, executes a command, waits for completion and transfers data back from the FIFO.
 * CRC validation can only be done if backData and backLen are specified.
 *
 * @return STATUS_OK on success, STATUS_??? otherwise.
 */
MFRC522::StatusCode MFRC522::PCD_CommunicateWithPICC(	byte command,		///< The command to execute. One of the PCD_Command enums.
														byte waitIRq,		///< The bits in the ComIrqReg register that signals successful completion of the command.
														byte *sendData,		///< Pointer to the data to transfer to the FIFO.
														byte sendLen,		///< Number of bytes to transfer to the FIFO.
														byte *backData,		///< NULL or pointer to buffer if data should be read back after executing the command.
														byte *backLen,		///< In: Max number of bytes to write to *backData. Out: The number of bytes returned.
														byte *validBits,	///< In/Out: The number of valid bits in the last byte. 0 for 8 valid bits.
														byte rxAlign,		///< In: Defines the bit position in backData[0] for the first bit received. Default 0.
														bool checkCRC		///< In: True => The last two bytes of the response is assumed to be a CRC_A that must be validated.
									 ) {
	// Prepare values for BitFramingReg
	byte txLastBits = validBits ? *validBits : 0;
	byte bitFraming = (rxAlign << 4) + txLastBits;		// RxAlign = BitFramingReg[6..4]. TxLastBits = BitFramingReg[2..0]
	
	PCD_WriteRegister(CommandReg, PCD_Idle);			// Stop any active command.
	PCD_WriteRegister(ComIrqReg, 0x7F);					// Clear all seven interrupt request bits
	PCD_WriteRegister(FIFOLevelReg, 0x80);				// FlushBuffer = 1, FIFO initialization
	PCD_WriteRegister(FIFODataReg, sendLen, sendData);	// Write sendData to the FIFO
	PCD_WriteRegister(BitFramingReg, bitFraming);		// Bit adjustments
	PCD_WriteRegister(CommandReg, command);				// Execute the command
	if (command == PCD_Transceive) {
		PCD_SetRegisterBitMask(BitFramingReg, 0x80);	// StartSend=1, transmission of data starts
	}
	
	// Wait for the command to complete.
	// In PCD_Init() we set the TAuto flag in TModeReg. This means the timer automatically starts when the PCD stops transmitting.
	// Each iteration of the do-while-loop takes 17.86μs.
	// TODO check/modify for other architectures than Arduino Uno 16bit
	uint16_t i;
	for (i = 2000; i > 0; i--) {
		byte n = PCD_ReadRegister(ComIrqReg);	// ComIrqReg[7..0] bits are: Set1 TxIRq RxIRq IdleIRq HiAlertIRq LoAlertIRq ErrIRq TimerIRq
		if (n & waitIRq) {					// One of the interrupts that signal success has been set.
			break;
		}
		if (n & 0x01) {						// Timer interrupt - nothing received in 25ms
			return STATUS_TIMEOUT;
		}
	}
	// 35.7ms and nothing happend. Communication with the MFRC522 might be down.
	if (i == 0) {
		return STATUS_TIMEOUT;
	}
	
	// Stop now if any errors except collisions were detected.
	byte errorRegValue = PCD_ReadRegister(ErrorReg); // ErrorReg[7..0] bits are: WrErr TempErr reserved BufferOvfl CollErr CRCErr ParityErr ProtocolErr
	if (errorRegValue & 0x13) {	 // BufferOvfl ParityErr ProtocolErr
		return STATUS_ERROR;
	}
  
	byte _validBits = 0;
	
  // If the caller wants data back, get it from the MFRC522.
	if (backData && backLen) {
		byte n = PCD_ReadRegister(FIFOLevelReg);	// Number of bytes in the FIFO
		if (n > *backLen) {
			return STATUS_NO_ROOM;
		}
		*backLen = n;											// Number of bytes returned
		PCD_ReadRegister(FIFODataReg, n, backData, rxAlign);	// Get received data from FIFO
		_validBits = PCD_ReadRegister(ControlReg) & 0x07;		// RxLastBits[2:0] indicates the number of valid bits in the last received byte. If this value is 000b, the whole byte is valid.
		if (validBits) {
			*validBits = _validBits;
		}
	}
	
	// Tell about collisions
	if (errorRegValue & 0x08) {		// CollErr
		return STATUS_COLLISION;
	}
	
	// Perform CRC_A validation if requested.
	if (backData && backLen && checkCRC) {
		// In this case a MIFARE Classic NAK is not OK.
		if (*backLen == 1 && _validBits == 4) {
			return STATUS_MIFARE_NACK;
		}
		// We need at least the CRC_A value and all 8 bits of the last byte must be received.
		if (*backLen < 2 || _validBits != 0) {
			return STATUS_CRC_WRONG;
		}
		// Verify CRC_A - do our own calculation and store the control in controlBuffer.
		byte controlBuffer[2];
		MFRC522::StatusCode status = PCD_CalculateCRC(&backData[0], *backLen - 2, &controlBuffer[0]);
		if (status != STATUS_OK) {
			return status;
		}
		if ((backData[*backLen - 2] != controlBuffer[0]) || (backData[*backLen - 1] != controlBuffer[1])) {
			return STATUS_CRC_WRONG;
		}
	}
	
	return STATUS_OK;
} // End PCD_CommunicateWithPICC()

/**
 * Transmits a REQuest command, Type A. Invites PICCs in state IDLE to go to READY and prepare for anticollision or selection. 7 bit frame.
 * Beware: When two PICCs are in the field at the same time I often get STATUS_TIMEOUT - probably due do bad antenna design.
 * 
 * @return STATUS_OK on success, STATUS_??? otherwise.
 */
MFRC522::StatusCode MFRC522::PICC_RequestA(	byte *bufferATQA,	///< The buffer to store the ATQA (Answer to request) in
											byte *bufferSize	///< Buffer size, at least two bytes. Also number of bytes returned if STATUS_OK.
										) {
	return PICC_REQA_or_WUPA(PICC_CMD_REQA, bufferATQA, bufferSize);
} // End PICC_RequestA()

/**
 * Transmits a Wake-UP command, Type A. Invites PICCs in state IDLE and HALT to go to READY(*) and prepare for anticollision or selection. 7 bit frame.
 * Beware: When two PICCs are in the field at the same time I often get STATUS_TIMEOUT - probably due do bad antenna design.
 * 
 * @return STATUS_OK on success, STATUS_??? otherwise.
 */
MFRC522::StatusCode MFRC522::PICC_WakeupA(	byte *bufferATQA,	///< The buffer to store the ATQA (Answer to request) in
											byte *bufferSize	///< Buffer size, at least two bytes. Also number of bytes returned if STATUS_OK.
										) {
	return PICC_REQA_or_WUPA(PICC_CMD_WUPA, bufferATQA, bufferSize);
} // End PICC_WakeupA()

/**
 * Transmits REQA or WUPA commands.
 * Beware: When two PICCs are in the field at the same time I often get STATUS_TIMEOUT - probably due do bad antenna design.
 * 
 * @return STATUS_OK on success, STATUS_??? otherwise.
 */ 
MFRC522::StatusCode MFRC522::PICC_REQA_or_WUPA(	byte command, 		///< The command to send - PICC_CMD_REQA or PICC_CMD_WUPA
												byte *bufferATQA,	///< The buffer to store the ATQA (Answer to request) in
												byte *bufferSize	///< Buffer size, at least two bytes. Also number of bytes returned if STATUS_OK.
											) {
	byte validBits;
	MFRC522::StatusCode status;
	
	if (bufferATQA == NULL || *bufferSize < 2) {	// The ATQA response is 2 bytes long.
		return STATUS_NO_ROOM;
	}
	PCD_ClearRegisterBitMask(CollReg, 0x80);		// ValuesAfterColl=1 => Bits received after collision are cleared.
	validBits = 7;									// For REQA and WUPA we need the short frame format - transmit only 7 bits of the last (and only) byte. TxLastBits = BitFramingReg[2..0]
	status = PCD_TransceiveData(&command, 1, bufferATQA, bufferSize, &validBits);
	if (status != STATUS_OK) {
		return status;
	}
	if (*bufferSize != 2 || validBits != 0) {		// ATQA must be exactly 16 bits.
		return STATUS_ERROR;
	}
	return STATUS_OK;
} // End PICC_REQA_or_WUPA()

/**
 * Transmits SELECT/ANTICOLLISION commands to select a single PICC.
 * Before calling this function the PICCs must be placed in the READY(*) state by calling PICC_RequestA() or PICC_WakeupA().
 * On success:
 * 		- The chosen PICC is in state ACTIVE(*) and all other PICCs have returned to state IDLE/HALT. (Figure 7 of the ISO/IEC 14443-3 draft.)
 * 		- The UID size and value of the chosen PICC is returned in *uid along with the SAK.
 * 
 * A PICC UID consists of 4, 7 or 10 bytes.
 * Only 4 bytes can be specified in a SELECT command, so for the longer UIDs two or three iterations are used:
 * 		UID size	Number of UID bytes		Cascade levels		Example of PICC
 * 		========	===================		==============		===============
 * 		single				 4						1				MIFARE Classic
 * 		double				 7						2				MIFARE Ultralight
 * 		triple				10						3				Not currently in use?
 * 
 * @return STATUS_OK on success, STATUS_??? otherwise.
 */
MFRC522::StatusCode MFRC522::PICC_Select(	Uid *uid,			///< Pointer to Uid struct. Normally output, but can also be used to supply a known UID.
											byte validBits		///< The number of known UID bits supplied in *uid. Normally 0. If set you must also supply uid->size.
										 ) {
	bool uidComplete;
	bool selectDone;
	bool useCascadeTag;
	byte cascadeLevel = 1;
	MFRC522::StatusCode result;
	byte count;
	byte index;
	byte uidIndex;					// The first index in uid->uidByte[] that is used in the current Cascade Level.
	int8_t currentLevelKnownBits;		// The number of known UID bits in the current Cascade Level.
	byte buffer[9];					// The SELECT/ANTICOLLISION commands uses a 7 byte standard frame + 2 bytes CRC_A
	byte bufferUsed;				// The number of bytes used in the buffer, ie the number of bytes to transfer to the FIFO.
	byte rxAlign;					// Used in BitFramingReg. Defines the bit position for the first bit received.
	byte txLastBits;				// Used in BitFramingReg. The number of valid bits in the last transmitted byte. 
	byte *responseBuffer;
	byte responseLength;
	
	// Description of buffer structure:
	//		Byte 0: SEL 				Indicates the Cascade Level: PICC_CMD_SEL_CL1, PICC_CMD_SEL_CL2 or PICC_CMD_SEL_CL3
	//		Byte 1: NVB					Number of Valid Bits (in complete command, not just the UID): High nibble: complete bytes, Low nibble: Extra bits. 
	//		Byte 2: UID-data or CT		See explanation below. CT means Cascade Tag.
	//		Byte 3: UID-data
	//		Byte 4: UID-data
	//		Byte 5: UID-data
	//		Byte 6: BCC					Block Check Character - XOR of bytes 2-5
	//		Byte 7: CRC_A
	//		Byte 8: CRC_A
	// The BCC and CRC_A are only transmitted if we know all the UID bits of the current Cascade Level.
	//
	// Description of bytes 2-5: (Section 6.5.4 of the ISO/IEC 14443-3 draft: UID contents and cascade levels)
	//		UID size	Cascade level	Byte2	Byte3	Byte4	Byte5
	//		========	=============	=====	=====	=====	=====
	//		 4 bytes		1			uid0	uid1	uid2	uid3
	//		 7 bytes		1			CT		uid0	uid1	uid2
	//						2			uid3	uid4	uid5	uid6
	//		10 bytes		1			CT		uid0	uid1	uid2
	//						2			CT		uid3	uid4	uid5
	//						3			uid6	uid7	uid8	uid9
	
	// Sanity checks
	if (validBits > 80) {
		return STATUS_INVALID;
	}
	
	// Prepare MFRC522
	PCD_ClearRegisterBitMask(CollReg, 0x80);		// ValuesAfterColl=1 => Bits received after collision are cleared.
	
	// Repeat Cascade Level loop until we have a complete UID.
	uidComplete = false;
	while (!uidComplete) {
		// Set the Cascade Level in the SEL byte, find out if we need to use the Cascade Tag in byte 2.
		switch (cascadeLevel) {
			case 1:
				buffer[0] = PICC_CMD_SEL_CL1;
				uidIndex = 0;
				useCascadeTag = validBits && uid->size > 4;	// When we know that the UID has more than 4 bytes
				break;
			
			case 2:
				buffer[0] = PICC_CMD_SEL_CL2;
				uidIndex = 3;
				useCascadeTag = validBits && uid->size > 7;	// When we know that the UID has more than 7 bytes
				break;
			
			case 3:
				buffer[0] = PICC_CMD_SEL_CL3;
				uidIndex = 6;
				useCascadeTag = false;						// Never used in CL3.
				break;
			
			default:
				return STATUS_INTERNAL_ERROR;
				break;
		}
		
		// How many UID bits are known in this Cascade Level?
		currentLevelKnownBits = validBits - (8 * uidIndex);
		if (currentLevelKnownBits < 0) {
			currentLevelKnownBits = 0;
		}
		// Copy the known bits from uid->uidByte[] to buffer[]
		index = 2; // destination index in buffer[]
		if (useCascadeTag) {
			buffer[index++] = PICC_CMD_CT;
		}
		byte bytesToCopy = currentLevelKnownBits / 8 + (currentLevelKnownBits % 8 ? 1 : 0); // The number of bytes needed to represent the known bits for this level.
		if (bytesToCopy) {
			byte maxBytes = useCascadeTag ? 3 : 4; // Max 4 bytes in each Cascade Level. Only 3 left if we use the Cascade Tag
			if (bytesToCopy > maxBytes) {
				bytesToCopy = maxBytes;
			}
			for (count = 0; count < bytesToCopy; count++) {
				buffer[index++] = uid->uidByte[uidIndex + count];
			}
		}
		// Now that the data has been copied we need to include the 8 bits in CT in currentLevelKnownBits
		if (useCascadeTag) {
			currentLevelKnownBits += 8;
		}
		
		// Repeat anti collision loop until we can transmit all UID bits + BCC and receive a SAK - max 32 iterations.
		selectDone = false;
		while (!selectDone) {
			// Find out how many bits and bytes to send and receive.
			if (currentLevelKnownBits >= 32) { // All UID bits in this Cascade Level are known. This is a SELECT.
				//Serial.print(F("SELECT: currentLevelKnownBits=")); Serial.println(currentLevelKnownBits, DEC);
				buffer[1] = 0x70; // NVB - Number of Valid Bits: Seven whole bytes
				// Calculate BCC - Block Check Character
				buffer[6] = buffer[2] ^ buffer[3] ^ buffer[4] ^ buffer[5];
				// Calculate CRC_A
				result = PCD_CalculateCRC(buffer, 7, &buffer[7]);
				if (result != STATUS_OK) {
					return result;
				}
				txLastBits		= 0; // 0 => All 8 bits are valid.
				bufferUsed		= 9;
				// Store response in the last 3 bytes of buffer (BCC and CRC_A - not needed after tx)
				responseBuffer	= &buffer[6];
				responseLength	= 3;
			}
			else { // This is an ANTICOLLISION.
				//Serial.print(F("ANTICOLLISION: currentLevelKnownBits=")); Serial.println(currentLevelKnownBits, DEC);
				txLastBits		= currentLevelKnownBits % 8;
				count			= currentLevelKnownBits / 8;	// Number of whole bytes in the UID part.
				index			= 2 + count;					// Number of whole bytes: SEL + NVB + UIDs
				buffer[1]		= (index << 4) + txLastBits;	// NVB - Number of Valid Bits
				bufferUsed		= index + (txLastBits ? 1 : 0);
				// Store response in the unused part of buffer
				responseBuffer	= &buffer[index];
				responseLength	= sizeof(buffer) - index;
			}
			
			// Set bit adjustments
			rxAlign = txLastBits;											// Having a separate variable is overkill. But it makes the next line easier to read.
			PCD_WriteRegister(BitFramingReg, (rxAlign << 4) + txLastBits);	// RxAlign = BitFramingReg[6..4]. TxLastBits = BitFramingReg[2..0]
			
			// Transmit the buffer and receive the response.
			result = PCD_TransceiveData(buffer, bufferUsed, responseBuffer, &responseLength, &txLastBits, rxAlign);
			if (result == STATUS_COLLISION) { // More than one PICC in the field => collision.
				byte valueOfCollReg = PCD_ReadRegister(CollReg); // CollReg[7..0] bits are: ValuesAfterColl reserved CollPosNotValid CollPos[4:0]
				if (valueOfCollReg & 0x20) { // CollPosNotValid
					return STATUS_COLLISION; // Without a valid collision position we cannot continue
				}
				byte collisionPos = valueOfCollReg & 0x1F; // Values 0-31, 0 means bit 32.
				if (collisionPos == 0) {
					collisionPos = 32;
				}
				if (collisionPos <= currentLevelKnownBits) { // No progress - should not happen 
					return STATUS_INTERNAL_ERROR;
				}
				// Choose the PICC with the bit set.
				currentLevelKnownBits = collisionPos;
				count			= (currentLevelKnownBits - 1) % 8; // The bit to modify
				index			= 1 + (currentLevelKnownBits / 8) + (count ? 1 : 0); // First byte is index 0.
				buffer[index]	|= (1 << count);
			}
			else if (result != STATUS_OK) {
				return result;
			}
			else { // STATUS_OK
				if (currentLevelKnownBits >= 32) { // This was a SELECT.
					selectDone = true; // No more anticollision 
					// We continue below outside the while.
				}
				else { // This was an ANTICOLLISION.
					// We now have all 32 bits of the UID in this Cascade Level
					currentLevelKnownBits = 32;
					// Run loop again to do the SELECT.
				}
			}
		} // End of while (!selectDone)
		
		// We do not check the CBB - it was constructed by us above.
		
		// Copy the found UID bytes from buffer[] to uid->uidByte[]
		index			= (buffer[2] == PICC_CMD_CT) ? 3 : 2; // source index in buffer[]
		bytesToCopy		= (buffer[2] == PICC_CMD_CT) ? 3 : 4;
		for (count = 0; count < bytesToCopy; count++) {
			uid->uidByte[uidIndex + count] = buffer[index++];
		}
		
		// Check response SAK (Select Acknowledge)
		if (responseLength != 3 || txLastBits != 0) { // SAK must be exactly 24 bits (1 byte + CRC_A).
			return STATUS_ERROR;
		}
		// Verify CRC_A - do our own calculation and store the control in buffer[2..3] - those bytes are not needed anymore.
		result = PCD_CalculateCRC(responseBuffer, 1, &buffer[2]);
		if (result != STATUS_OK) {
			return result;
		}
		if ((buffer[2] != responseBuffer[1]) || (buffer[3] != responseBuffer[2])) {
			return STATUS_CRC_WRONG;
		}
		if (responseBuffer[0] & 0x04) { // Cascade bit set - UID not complete yes
			cascadeLevel++;
		}
		else {
			uidComplete = true;
			uid->sak = responseBuffer[0];
		}
	} // End of while (!uidComplete)
	
	// Set correct uid->size
	uid->size = 3 * cascadeLevel + 1;
	
	// IF SAK bit 6 = 1 then it is ISO/IEC 14443-4 (T=CL)
	// A Request ATS command should be sent
	// We also check SAK bit 3 is cero, as it stands for UID complete (1 would tell us it is incomplete)
	if ((uid->sak & 0x24) == 0x20) {
		Ats ats;
		result = PICC_RequestATS(&ats);
		if (result == STATUS_OK) {
			// Check the ATS
			if (ats.size > 0)
			{
				// TA1 has been transmitted?
				// PPS must be supported...
				if (ats.ta1.transmitted)
				{
					// TA1
					//  8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | Description
					// ---+---+---+---+---+---+---+---+------------------------------------------
					//  0 | - | - | - | 0 | - | - | - | Different D for each direction supported
					//  1 | - | - | - | 0 | - | - | - | Only same D for both direction supported
					//  - | x | x | x | 0 | - | - | - | DS (Send D)
					//  - | - | - | - | 0 | x | x | x | DR (Receive D)
					//
					// D to bitrate table
					//  3 | 2 | 1 | Value
					// ---+---+---+-----------------------------
					//  1 | - | - | 848 kBaud is supported
					//  - | 1 | - | 424 kBaud is supported
					//  - | - | 1 | 212 kBaud is supported
					//  0 | 0 | 0 | Only 106 kBaud is supported
					//
					// Note: 106 kBaud is always supported
					//
					// I have almost constant timeouts when changing speeds :(
					TagBitRates ds = BITRATE_106KBITS; 
					TagBitRates dr = BITRATE_106KBITS;

					//// Not working at 848 or 424
					//if (ats.ta1.ds & 0x04) 
					//{
					//	ds = BITRATE_848KBITS;
					//} 
					//else if (ats.ta1.ds & 0x02)
					//{
					//	ds = BITRATE_424KBITS;
					//}
					//else if (ats.ta1.ds & 0x01)
					//{
					//	ds = BITRATE_212KBITS;
					//}
					//else 
					//{
					//	ds = BITRATE_106KBITS;
					//}

					if (ats.ta1.ds & 0x01)
					{
						ds = BITRATE_212KBITS;
					}
					else 
					{
						ds = BITRATE_106KBITS;
					}

					//// Not working at 848 or 424
					//if (ats.ta1.dr & 0x04) 
					//{
					//	dr = BITRATE_848KBITS;
					//} 
					//else if (ats.ta1.dr & 0x02)
					//{
					//	dr = BITRATE_424KBITS;
					//}
					//else if (ats.ta1.dr & 0x01)
					//{
					//	dr = BITRATE_212KBITS;
					//}
					//else 
					//{
					//	dr = BITRATE_106KBITS;
					//}

					if (ats.ta1.dr & 0x01)
					{
						dr = BITRATE_212KBITS;
					}
					else
					{
						dr = BITRATE_106KBITS;
					}

					PICC_PPS(ds, dr);
				}
			}
		}
	}

	return STATUS_OK;
} // End PICC_Select()

/**
 * Instructs a PICC in state ACTIVE(*) to go to state HALT.
 *
 * @return STATUS_OK on success, STATUS_??? otherwise.
 */ 
MFRC522::StatusCode MFRC522::PICC_HaltA() {
	MFRC522::StatusCode result;
	byte buffer[4];
	
	// Build command buffer
	buffer[0] = PICC_CMD_HLTA;
	buffer[1] = 0;
	// Calculate CRC_A
	result = PCD_CalculateCRC(buffer, 2, &buffer[2]);
	if (result != STATUS_OK) {
		return result;
	}
	
	// Send the command.
	// The standard says:
	//		If the PICC responds with any modulation during a period of 1 ms after the end of the frame containing the
	//		HLTA command, this response shall be interpreted as 'not acknowledge'.
	// We interpret that this way: Only STATUS_TIMEOUT is a success.
	result = PCD_TransceiveData(buffer, sizeof(buffer), NULL, 0);
	if (result == STATUS_TIMEOUT) {
		return STATUS_OK;
	}
	if (result == STATUS_OK) { // That is ironically NOT ok in this case ;-)
		return STATUS_ERROR;
	}
	return result;
} // End PICC_HaltA()

/**
 * Transmits a Request command for Answer To Select (ATS).
 *
 * @return STATUS_OK on success, STATUS_??? otherwise.
 */
MFRC522::StatusCode MFRC522::PICC_RequestATS(Ats *ats) 
{
	byte count;
	MFRC522::StatusCode result;

	byte bufferATS[FIFO_SIZE];
	byte bufferSize = FIFO_SIZE;

	memset(bufferATS, 0, FIFO_SIZE);

	// Build command buffer
	bufferATS[0] = PICC_CMD_RATS;
	
	// The CID defines the logical number of the addressed card and has a range of 0 
	// through 14; 15 is reserved for future use (RFU).
	//
	// FSDI codes the maximum frame size (FSD) that the terminal can receive.
	//
	// FSDI        |  0  |  1  |  2  |  3  |  4  |  5  |  6  |  7  |  8  |  9-F
	// ------------+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----------
	// FSD (bytes) |  16 |  24 |  32 |  40 |  48 |  64 |  96 | 128 | 256 | RFU > 256
	//
	bufferATS[1] = 0x50; // FSD=64, CID=0

	// Calculate CRC_A
	result = PCD_CalculateCRC(bufferATS, 2, &bufferATS[2]);
	if (result != STATUS_OK) {
		return result;
	}

	// Transmit the buffer and receive the response, validate CRC_A.
	result = PCD_TransceiveData(bufferATS, 4, bufferATS, &bufferSize, NULL, 0, true);
	if (result != STATUS_OK) {
		PICC_HaltA();
	}

	// Set the ats structure data
	ats->size = bufferATS[0];

	// T0 byte:
	//
	// b8 | b7 | b6 | b5 | b4 | b3 | b2 | b1 | Meaning
	//----+----+----+----+----+----+----+----+---------------------------
	//  0 | ...| ...| ...| ...|... | ...| ...| Set to 0 (RFU)
	//  0 |  1 | x  |  x | ...|... | ...| ...| TC1 transmitted
	//  0 |  x | 1  |  x | ...|... | ...| ...| TB1 transmitted
	//  0 |  x | x  |  1 | ...|... | ...| ...| TA1 transmitted
	//  0 | ...| ...| ...|  x |  x |  x | x  | Maximum frame size (FSCI)
	//
	// FSCI        |  0  |  1  |  2  |  3  |  4  |  5  |  6  |  7  |  8  |  9-F
	// ------------+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----------
	// FSC (bytes) |  16 |  24 |  32 |  40 |  48 |  64 |  96 | 128 | 256 | RFU > 256
	//
	// Default FSCI is 2 (32 bytes)
	if (ats->size > 0x01)
	{
		// TC1, TB1 and TA1 where NOT transmitted
		ats->ta1.transmitted = (bool)(bufferATS[1] & 0x40);
		ats->tb1.transmitted = (bool)(bufferATS[1] & 0x20);
		ats->tc1.transmitted = (bool)(bufferATS[1] & 0x10);

		// Decode FSCI
		switch (bufferATS[1] & 0x0F)
		{
			case 0x00:
				ats->fsc = 16;
				break;
			case 0x01:
				ats->fsc = 24;
				break;
			case 0x02:
				ats->fsc = 32;
				break;
			case 0x03:
				ats->fsc = 40;
				break;
			case 0x04:
				ats->fsc = 48;
				break;
			case 0x05:
				ats->fsc = 64;
				break;
			case 0x06:
				ats->fsc = 96;
				break;
			case 0x07:
				ats->fsc = 128;
				break;
			case 0x08:
				// This value cannot be hold by a byte
				// The reason I ignore it is that MFRC255 FIFO is 64 bytes so this is not a possible value (or atleast it shouldn't)
				//ats->fsc = 256;
				break;
				// TODO: What to do with RFU (Reserved for future use)?
		}

		// TA1
		if (ats->ta1.transmitted)
		{
			ats->ta1.sameD = (bool)(bufferATS[2] & 0x80);
			ats->ta1.ds = (TagBitRates)((bufferATS[2] & 0x70) >> 4);
			ats->ta1.dr = (TagBitRates)(bufferATS[2] & 0x07);
		}
		else
		{
			// Default TA1
			ats->ta1.ds = BITRATE_106KBITS;
			ats->ta1.dr = BITRATE_106KBITS;
		}

		// TB1
		if (ats->tb1.transmitted)
		{
			uint8_t tb1Index = 2;

			if (ats->ta1.transmitted)
				tb1Index++;
			
			ats->tb1.fwi = (bufferATS[tb1Index] & 0xF0) >> 4;
			ats->tb1.sfgi = bufferATS[tb1Index] & 0x0F;
		}
		else
		{
			// Defaults for TB1
			ats->tb1.fwi = 0;	// TODO: Don't know the default for this!
			ats->tb1.sfgi = 0;	// The default value of SFGI is 0 (meaning that the card does not need any particular SFGT)
		}

		// TC1
		if (ats->tc1.transmitted)
		{
			uint8_t tc1Index = 2;

			if (ats->ta1.transmitted)
				tc1Index++;
			if (ats->tb1.transmitted)
				tc1Index++;

			ats->tc1.supportsCID = (bool)(bufferATS[tc1Index] & 0x02);
			ats->tc1.supportsNAD = (bool)(bufferATS[tc1Index] & 0x01);
		}
		else
		{
			// Defaults for TC1
			ats->tc1.supportsCID = true;
			ats->tc1.supportsNAD = false;
		}
	}
	else
	{
		// TC1, TB1 and TA1 where NOT transmitted
		ats->ta1.transmitted = false;
		ats->tb1.transmitted = false;
		ats->tc1.transmitted = false;

		// Default FSCI
		ats->fsc = 32;	// Defaults to FSCI 2 (32 bytes)

		// Default TA1
		ats->ta1.sameD = false;
		ats->ta1.ds = BITRATE_106KBITS;
		ats->ta1.dr = BITRATE_106KBITS;

		// Defaults for TB1
		ats->tb1.transmitted = false;
		ats->tb1.fwi = 0;	// TODO: Don't know the default for this!
		ats->tb1.sfgi = 0;	// The default value of SFGI is 0 (meaning that the card does not need any particular SFGT)

		// Defaults for TC1
		ats->tc1.transmitted = false;
		ats->tc1.supportsCID = true;
		ats->tc1.supportsNAD = false;
	}

	memcpy(ats->data, bufferATS, bufferSize - 2);

	return result;
} // End PICC_RequestATS()

/**
 * Transmits Protocol and Parameter Selection Request (PPS) without parameter 1 
 *
 * @return STATUS_OK on success, STATUS_??? otherwise.
 */
MFRC522::StatusCode MFRC522::PICC_PPS()
{
	StatusCode result;

	byte ppsBuffer[4];
	byte ppsBufferSize = 4;
	// Start byte: The start byte (PPS) consists of two parts:
	//  –The upper nibble(b8–b5) is set to’D'to identify the PPS. All other values are RFU.
	//  -The lower nibble(b4–b1), which is called the ‘card identifier’ (CID), defines the logical number of the addressed card.
	ppsBuffer[0] = 0xD0;	// CID is hardcoded as 0 in RATS
	ppsBuffer[1] = 0x00;	// PPS0 indicates whether PPS1 is present

	// Calculate CRC_A
	result = PCD_CalculateCRC(ppsBuffer, 2, &ppsBuffer[2]);
	if (result != STATUS_OK) {
		return result;
	}

	// Transmit the buffer and receive the response, validate CRC_A.
	result = PCD_TransceiveData(ppsBuffer, 4, ppsBuffer, &ppsBufferSize, NULL, 0, true);
	if (result == STATUS_OK)
	{
		// Enable CRC for T=CL
		byte txReg = PCD_ReadRegister(TxModeReg) | 0x80;
		byte rxReg = PCD_ReadRegister(RxModeReg) | 0x80;

		PCD_WriteRegister(TxModeReg, txReg);
		PCD_WriteRegister(RxModeReg, rxReg);
	}

	return result;
} // End PICC_PPS()

/**
 * Transmits Protocol and Parameter Selection Request (PPS)
 *
 * @return STATUS_OK on success, STATUS_??? otherwise.
 */
MFRC522::StatusCode MFRC522::PICC_PPS(TagBitRates sendBitRate,	          ///< DS
                                      TagBitRates receiveBitRate		  ///< DR
) {
	StatusCode result;

	byte txReg = PCD_ReadRegister(TxModeReg) & 0x8F;
	byte rxReg = PCD_ReadRegister(RxModeReg) & 0x8F;

	byte ppsBuffer[5];
	byte ppsBufferSize = 5;
	// Start byte: The start byte (PPS) consists of two parts:
	//  –The upper nibble(b8–b5) is set to’D'to identify the PPS. All other values are RFU.
	//  -The lower nibble(b4–b1), which is called the ‘card identifier’ (CID), defines the logical number of the addressed card.
	ppsBuffer[0] = 0xD0;	// CID is hardcoded as 0 in RATS
	ppsBuffer[1] = 0x11;	// PPS0 indicates whether PPS1 is present

	// Bit 8 - Set to '0' as MFRC522 allows different bit rates for send and receive
	// Bit 4 - Set to '0' as it is Reserved for future use.
	//ppsBuffer[2] = (((sendBitRate & 0x03) << 4) | (receiveBitRate & 0x03)) & 0xE7;
	ppsBuffer[2] = (((sendBitRate & 0x03) << 2) | (receiveBitRate & 0x03)) & 0xE7;

	// Calculate CRC_A
	result = PCD_CalculateCRC(ppsBuffer, 3, &ppsBuffer[3]);
	if (result != STATUS_OK) {
		return result;
	}
	
	// Transmit the buffer and receive the response, validate CRC_A.
	result = PCD_TransceiveData(ppsBuffer, 5, ppsBuffer, &ppsBufferSize, NULL, 0, true);
	if (result == STATUS_OK)
	{
		// Make sure it is an answer to our PPS
		// We should receive our PPS byte and 2 CRC bytes
		if ((ppsBufferSize == 3) && (ppsBuffer[0] == 0xD0)) {
			byte txReg = PCD_ReadRegister(TxModeReg) & 0x8F;
			byte rxReg = PCD_ReadRegister(RxModeReg) & 0x8F;

			// Set bit rate and enable CRC for T=CL
			txReg = (txReg & 0x8F) | ((receiveBitRate & 0x03) << 4) | 0x80;
			rxReg = (rxReg & 0x8F) | ((sendBitRate & 0x03) << 4) | 0x80;
			rxReg &= 0xF0; //Enforce although this should be set already

			// From ConfigIsoType
			//rxReg |= 0x06;

			PCD_WriteRegister(TxModeReg, txReg);
			PCD_WriteRegister(RxModeReg, rxReg);

			// At 212kBps
			switch (sendBitRate) {
				case BITRATE_212KBITS:
					{
						//PCD_WriteRegister(ModWidthReg, 0x13);
						PCD_WriteRegister(ModWidthReg, 0x15);
					}
					break;
				case BITRATE_424KBITS:
					{
						PCD_WriteRegister(ModWidthReg, 0x0A);
					}
					break;
				case BITRATE_848KBITS:
					{
						PCD_WriteRegister(ModWidthReg, 0x05);
					}
					break;
				default:
					{
						PCD_WriteRegister(ModWidthReg, 0x26); // Default value
					}
					break;
			}

			//PCD_WriteRegister(RxThresholdReg, 0x84); // ISO-14443.4 Type A (default)
			//PCD_WriteRegister(ControlReg, 0x10);
			
			delayMicroseconds(10);
		}
		else 
		{
			return STATUS_ERROR;
		}
	}

	return result;
} // End PICC_PPS()


/////////////////////////////////////////////////////////////////////////////////////
// Functions for communicating with ISO/IEC 14433-4 cards
/////////////////////////////////////////////////////////////////////////////////////

MFRC522::StatusCode MFRC522::TCL_Transceive(PcbBlock *send, PcbBlock *back)
{
	MFRC522::StatusCode result;
	byte inBuffer[FIFO_SIZE];
	byte inBufferSize = FIFO_SIZE;
	byte outBuffer[send->inf.size + 5]; // PCB + CID + NAD + INF + EPILOGUE (CRC)
	byte outBufferOffset = 1;
	byte inBufferOffset = 1;

	// Set the PCB byte
	outBuffer[0] = send->prologue.pcb;

	// Set the CID byte if available
	if (send->prologue.pcb & 0x08) {
		outBuffer[outBufferOffset] = send->prologue.cid;
		outBufferOffset++;
	}

	// Set the NAD byte if available
	if (send->prologue.pcb & 0x04) {
		outBuffer[outBufferOffset] = send->prologue.nad;
		outBufferOffset++;
	}

	// Copy the INF field if available
	if (send->inf.size > 0) {
		memcpy(&outBuffer[outBufferOffset], send->inf.data, send->inf.size);
		outBufferOffset += send->inf.size;
	}

	// Is the CRC enabled for transmission?
	byte txModeReg = PCD_ReadRegister(TxModeReg);
	if ((txModeReg & 0x80) != 0x80) {
		// Calculate CRC_A
		result = PCD_CalculateCRC(outBuffer, outBufferOffset, &outBuffer[outBufferOffset]);
		if (result != STATUS_OK) {
			return result;
		}

		outBufferOffset += 2;
	}

	// Transceive the block
	result = PCD_TransceiveData(outBuffer, outBufferOffset, inBuffer, &inBufferSize);
	if (result != STATUS_OK) {
		return result;
	}

	// We want to turn the received array back to a PcbBlock
	back->prologue.pcb = inBuffer[0];

	// CID byte is present?
	if (send->prologue.pcb & 0x08) {
		back->prologue.cid = inBuffer[inBufferOffset];
		inBufferOffset++;
	}

	// NAD byte is present?
	if (send->prologue.pcb & 0x04) {
		back->prologue.nad = inBuffer[inBufferOffset];
		inBufferOffset++;
	}

	// Check if CRC is taken care of by MFRC522
	byte rxModeReg = PCD_ReadRegister(TxModeReg);
	if ((rxModeReg & 0x80) != 0x80) {
		Serial.print("CRC is not taken care of by MFRC522: ");
		Serial.println(rxModeReg, HEX);

		// Check the CRC
		// We need at least the CRC_A value.
		if ((int)(inBufferSize - inBufferOffset) < 2) {
			return STATUS_CRC_WRONG;
		}

		// Verify CRC_A - do our own calculation and store the control in controlBuffer.
		byte controlBuffer[2];
		MFRC522::StatusCode status = PCD_CalculateCRC(inBuffer, inBufferSize - 2, controlBuffer);
		if (status != STATUS_OK) {
			return status;
		}

		if ((inBuffer[inBufferSize - 2] != controlBuffer[0]) || (inBuffer[inBufferSize - 1] != controlBuffer[1])) {
			return STATUS_CRC_WRONG;
		}

		// Take away the CRC bytes
		inBufferSize -= 2;
	}

	// Got more data?
	if (inBufferSize > inBufferOffset) {
		if ((inBufferSize - inBufferOffset) > back->inf.size) {
			return STATUS_NO_ROOM;
		}

		memcpy(back->inf.data, &inBuffer[inBufferOffset], inBufferSize - inBufferOffset);
		back->inf.size = inBufferSize - inBufferOffset;
	} else {
		back->inf.size = 0;
	}

	// If the response is a R-Block check NACK
	if (((inBuffer[0] & 0xC0) == 0x80) && (inBuffer[0] & 0x20)) {
		return STATUS_MIFARE_NACK;
	}
	
	return result;
}
/**
 * Send an I-Block (Application)
 */
MFRC522::StatusCode MFRC522::TCL_Transceive(TagInfo *tag, byte *sendData, byte sendLen, byte *backData, byte *backLen)
{
	MFRC522::StatusCode result;

	PcbBlock out;
	PcbBlock in;
	byte outBuffer[FIFO_SIZE];
	byte outBufferSize = FIFO_SIZE;
	byte totalBackLen = *backLen;

	// This command sends an I-Block
	out.prologue.pcb = 0x02;

	if (tag->ats.tc1.supportsCID) {
		out.prologue.pcb |= 0x08;
		out.prologue.cid = 0x00;	// CID is curentlly hardcoded as 0x00
	}

	// This command doe not support NAD
	out.prologue.pcb &= 0xFB;
	out.prologue.nad = 0x00;

	// Set the block number
	if (tag->blockNumber) {
		out.prologue.pcb |= 0x01;
	}

	// Do we have data to send?
	if (sendData && (sendLen > 0)) {
		out.inf.size = sendLen;
		out.inf.data = sendData;
	} else {
		out.inf.size = 0;
		out.inf.data = NULL;
	}

	// Initialize the receiving data
	in.inf.data = outBuffer;
	in.inf.size = outBufferSize;

	result = TCL_Transceive(&out, &in);
	if (result != STATUS_OK) {
		return result;
	}

	// Swap block number on success
	if (tag->blockNumber)
		tag->blockNumber = false;
	else
		tag->blockNumber = true;

	if (backData && (backLen > 0)) {
		if (*backLen < in.inf.size)
			return STATUS_NO_ROOM;

		*backLen = in.inf.size;
		memcpy(backData, in.inf.data, in.inf.size);
	}

	// Check chaining
	if (in.prologue.pcb & 0x10 == 0x00)
		return result;

	// Result is chained
	// Send an ACK to receive more data
	// TODO: Should be checked I've never needed to send an ACK
	while (in.prologue.pcb & 0x10) {
		byte ackData[FIFO_SIZE];
		byte ackDataSize = FIFO_SIZE;

		result = TCL_TransceiveRBlock(tag, true, ackData, &ackDataSize);
		if (result != STATUS_OK)
			return result;

		if (backData && (backLen > 0)) {
			if ((*backLen + ackDataSize) > totalBackLen)
				return STATUS_NO_ROOM;

			memcpy(&(backData[*backLen]), ackData, ackDataSize);
			*backLen += ackDataSize;
		}
	}
	
	return result;
} // End TCL_Transceive()

/**
 * Send R-Block to the PICC.
 */
MFRC522::StatusCode MFRC522::TCL_TransceiveRBlock(TagInfo *tag, bool ack, byte *backData, byte *backLen)
{
	MFRC522::StatusCode result;

	PcbBlock out;
	PcbBlock in;
	byte outBuffer[FIFO_SIZE];
	byte outBufferSize = FIFO_SIZE;

	// This command sends an R-Block
	if (ack)
		out.prologue.pcb = 0xA2;	// ACK
	else 
		out.prologue.pcb = 0xB2;	// NAK
	

	if (tag->ats.tc1.supportsCID) {
		out.prologue.pcb |= 0x08;
		out.prologue.cid = 0x00;	// CID is curentlly hardcoded as 0x00
	}

	// This command doe not support NAD
	out.prologue.pcb &= 0xFB;
	out.prologue.nad = 0x00;

	// Set the block number
	if (tag->blockNumber) {
		out.prologue.pcb |= 0x01;
	}

	// No INF data for R-Block
	out.inf.size = 0;
	out.inf.data = NULL;

	// Initialize the receiving data
	in.inf.data = outBuffer;
	in.inf.size = outBufferSize;

	result = TCL_Transceive(&out, &in);
	if (result != STATUS_OK) {
		return result;
	}

	// Swap block number on success
	if (tag->blockNumber)
		tag->blockNumber = false;
	else
		tag->blockNumber = true;

	if (backData && backLen) {
		if (*backLen < in.inf.size)
			return STATUS_NO_ROOM;

		*backLen = in.inf.size;
		memcpy(backData, in.inf.data, in.inf.size);
	}
	
	return result;
} // End TCL_TransceiveRBlock()

/**
 * Send an S-Block to deselect the card.
 */
MFRC522::StatusCode MFRC522::TCL_Deselect(TagInfo *tag)
{
	MFRC522::StatusCode result;
	byte outBuffer[4];
	byte outBufferSize = 1;
	byte inBuffer[FIFO_SIZE];
	byte inBufferSize = FIFO_SIZE;

	outBuffer[0] = 0xC2;
	if (tag->ats.tc1.supportsCID)
	{
		outBuffer[0] |= 0x08;
		outBuffer[1] = 0x00;  // CID is hardcoded
		outBufferSize = 2;
	}

	result = PCD_TransceiveData(outBuffer, outBufferSize, inBuffer, &inBufferSize);
	if (result != STATUS_OK) {
		return result;
	}

	// TODO:Maybe do some checks? In my test it returns: CA 00 (Same data as I sent to my card)

	return result;
} // End TCL_Deselect()

/////////////////////////////////////////////////////////////////////////////////////
// Functions for communicating with MIFARE PICCs
/////////////////////////////////////////////////////////////////////////////////////

/**
 * Executes the MFRC522 MFAuthent command.
 * This command manages MIFARE authentication to enable a secure communication to any MIFARE Mini, MIFARE 1K and MIFARE 4K card.
 * The authentication is described in the MFRC522 datasheet section 10.3.1.9 and http://www.nxp.com/documents/data_sheet/MF1S503x.pdf section 10.1.
 * For use with MIFARE Classic PICCs.
 * The PICC must be selected - ie in state ACTIVE(*) - before calling this function.
 * Remember to call PCD_StopCrypto1() after communicating with the authenticated PICC - otherwise no new communications can start.
 * 
 * All keys are set to FFFFFFFFFFFFh at chip delivery.
 * 
 * @return STATUS_OK on success, STATUS_??? otherwise. Probably STATUS_TIMEOUT if you supply the wrong key.
 */
MFRC522::StatusCode MFRC522::PCD_Authenticate(byte command,		///< PICC_CMD_MF_AUTH_KEY_A or PICC_CMD_MF_AUTH_KEY_B
											byte blockAddr, 	///< The block number. See numbering in the comments in the .h file.
											MIFARE_Key *key,	///< Pointer to the Crypteo1 key to use (6 bytes)
											Uid *uid			///< Pointer to Uid struct. The first 4 bytes of the UID is used.
											) {
	byte waitIRq = 0x10;		// IdleIRq
	
	// Build command buffer
	byte sendData[12];
	sendData[0] = command;
	sendData[1] = blockAddr;
	for (byte i = 0; i < MF_KEY_SIZE; i++) {	// 6 key bytes
		sendData[2+i] = key->keyByte[i];
	}
	// Use the last uid bytes as specified in http://cache.nxp.com/documents/application_note/AN10927.pdf
	// section 3.2.5 "MIFARE Classic Authentication".
	// The only missed case is the MF1Sxxxx shortcut activation,
	// but it requires cascade tag (CT) byte, that is not part of uid.
	for (byte i = 0; i < 4; i++) {				// The last 4 bytes of the UID
		sendData[8+i] = uid->uidByte[i+uid->size-4];
	}
	
	// Start the authentication.
	return PCD_CommunicateWithPICC(PCD_MFAuthent, waitIRq, &sendData[0], sizeof(sendData));
} // End PCD_Authenticate()

/**
 * Used to exit the PCD from its authenticated state.
 * Remember to call this function after communicating with an authenticated PICC - otherwise no new communications can start.
 */
void MFRC522::PCD_StopCrypto1() {
	// Clear MFCrypto1On bit
	PCD_ClearRegisterBitMask(Status2Reg, 0x08); // Status2Reg[7..0] bits are: TempSensClear I2CForceHS reserved reserved MFCrypto1On ModemState[2:0]
} // End PCD_StopCrypto1()

/**
 * Reads 16 bytes (+ 2 bytes CRC_A) from the active PICC.
 * 
 * For MIFARE Classic the sector containing the block must be authenticated before calling this function.
 * 
 * For MIFARE Ultralight only addresses 00h to 0Fh are decoded.
 * The MF0ICU1 returns a NAK for higher addresses.
 * The MF0ICU1 responds to the READ command by sending 16 bytes starting from the page address defined by the command argument.
 * For example; if blockAddr is 03h then pages 03h, 04h, 05h, 06h are returned.
 * A roll-back is implemented: If blockAddr is 0Eh, then the contents of pages 0Eh, 0Fh, 00h and 01h are returned.
 * 
 * The buffer must be at least 18 bytes because a CRC_A is also returned.
 * Checks the CRC_A before returning STATUS_OK.
 * 
 * @return STATUS_OK on success, STATUS_??? otherwise.
 */
MFRC522::StatusCode MFRC522::MIFARE_Read(	byte blockAddr, 	///< MIFARE Classic: The block (0-0xff) number. MIFARE Ultralight: The first page to return data from.
											byte *buffer,		///< The buffer to store the data in
											byte *bufferSize	///< Buffer size, at least 18 bytes. Also number of bytes returned if STATUS_OK.
										) {
	MFRC522::StatusCode result;
	
	// Sanity check
	if (buffer == NULL || *bufferSize < 18) {
		return STATUS_NO_ROOM;
	}
	
	// Build command buffer
	buffer[0] = PICC_CMD_MF_READ;
	buffer[1] = blockAddr;
	// Calculate CRC_A
	result = PCD_CalculateCRC(buffer, 2, &buffer[2]);
	if (result != STATUS_OK) {
		return result;
	}
	
	// Transmit the buffer and receive the response, validate CRC_A.
	return PCD_TransceiveData(buffer, 4, buffer, bufferSize, NULL, 0, true);
} // End MIFARE_Read()

/**
 * Writes 16 bytes to the active PICC.
 * 
 * For MIFARE Classic the sector containing the block must be authenticated before calling this function.
 * 
 * For MIFARE Ultralight the operation is called "COMPATIBILITY WRITE".
 * Even though 16 bytes are transferred to the Ultralight PICC, only the least significant 4 bytes (bytes 0 to 3)
 * are written to the specified address. It is recommended to set the remaining bytes 04h to 0Fh to all logic 0.
 * * 
 * @return STATUS_OK on success, STATUS_??? otherwise.
 */
MFRC522::StatusCode MFRC522::MIFARE_Write(	byte blockAddr, ///< MIFARE Classic: The block (0-0xff) number. MIFARE Ultralight: The page (2-15) to write to.
											byte *buffer,	///< The 16 bytes to write to the PICC
											byte bufferSize	///< Buffer size, must be at least 16 bytes. Exactly 16 bytes are written.
										) {
	MFRC522::StatusCode result;
	
	// Sanity check
	if (buffer == NULL || bufferSize < 16) {
		return STATUS_INVALID;
	}
	
	// Mifare Classic protocol requires two communications to perform a write.
	// Step 1: Tell the PICC we want to write to block blockAddr.
	byte cmdBuffer[2];
	cmdBuffer[0] = PICC_CMD_MF_WRITE;
	cmdBuffer[1] = blockAddr;
	result = PCD_MIFARE_Transceive(cmdBuffer, 2); // Adds CRC_A and checks that the response is MF_ACK.
	if (result != STATUS_OK) {
		return result;
	}
	
	// Step 2: Transfer the data
	result = PCD_MIFARE_Transceive(buffer, bufferSize); // Adds CRC_A and checks that the response is MF_ACK.
	if (result != STATUS_OK) {
		return result;
	}
	
	return STATUS_OK;
} // End MIFARE_Write()

/**
 * Writes a 4 byte page to the active MIFARE Ultralight PICC.
 * 
 * @return STATUS_OK on success, STATUS_??? otherwise.
 */
MFRC522::StatusCode MFRC522::MIFARE_Ultralight_Write(	byte page, 		///< The page (2-15) to write to.
														byte *buffer,	///< The 4 bytes to write to the PICC
														byte bufferSize	///< Buffer size, must be at least 4 bytes. Exactly 4 bytes are written.
													) {
	MFRC522::StatusCode result;
	
	// Sanity check
	if (buffer == NULL || bufferSize < 4) {
		return STATUS_INVALID;
	}
	
	// Build commmand buffer
	byte cmdBuffer[6];
	cmdBuffer[0] = PICC_CMD_UL_WRITE;
	cmdBuffer[1] = page;
	memcpy(&cmdBuffer[2], buffer, 4);
	
	// Perform the write
	result = PCD_MIFARE_Transceive(cmdBuffer, 6); // Adds CRC_A and checks that the response is MF_ACK.
	if (result != STATUS_OK) {
		return result;
	}
	return STATUS_OK;
} // End MIFARE_Ultralight_Write()

/**
 * MIFARE Decrement subtracts the delta from the value of the addressed block, and stores the result in a volatile memory.
 * For MIFARE Classic only. The sector containing the block must be authenticated before calling this function.
 * Only for blocks in "value block" mode, ie with access bits [C1 C2 C3] = [110] or [001].
 * Use MIFARE_Transfer() to store the result in a block.
 * 
 * @return STATUS_OK on success, STATUS_??? otherwise.
 */
MFRC522::StatusCode MFRC522::MIFARE_Decrement(	byte blockAddr, ///< The block (0-0xff) number.
												int32_t delta		///< This number is subtracted from the value of block blockAddr.
											) {
	return MIFARE_TwoStepHelper(PICC_CMD_MF_DECREMENT, blockAddr, delta);
} // End MIFARE_Decrement()

/**
 * MIFARE Increment adds the delta to the value of the addressed block, and stores the result in a volatile memory.
 * For MIFARE Classic only. The sector containing the block must be authenticated before calling this function.
 * Only for blocks in "value block" mode, ie with access bits [C1 C2 C3] = [110] or [001].
 * Use MIFARE_Transfer() to store the result in a block.
 * 
 * @return STATUS_OK on success, STATUS_??? otherwise.
 */
MFRC522::StatusCode MFRC522::MIFARE_Increment(	byte blockAddr, ///< The block (0-0xff) number.
												int32_t delta		///< This number is added to the value of block blockAddr.
											) {
	return MIFARE_TwoStepHelper(PICC_CMD_MF_INCREMENT, blockAddr, delta);
} // End MIFARE_Increment()

/**
 * MIFARE Restore copies the value of the addressed block into a volatile memory.
 * For MIFARE Classic only. The sector containing the block must be authenticated before calling this function.
 * Only for blocks in "value block" mode, ie with access bits [C1 C2 C3] = [110] or [001].
 * Use MIFARE_Transfer() to store the result in a block.
 * 
 * @return STATUS_OK on success, STATUS_??? otherwise.
 */
MFRC522::StatusCode MFRC522::MIFARE_Restore(	byte blockAddr ///< The block (0-0xff) number.
											) {
	// The datasheet describes Restore as a two step operation, but does not explain what data to transfer in step 2.
	// Doing only a single step does not work, so I chose to transfer 0L in step two.
	return MIFARE_TwoStepHelper(PICC_CMD_MF_RESTORE, blockAddr, 0L);
} // End MIFARE_Restore()

/**
 * Helper function for the two-step MIFARE Classic protocol operations Decrement, Increment and Restore.
 * 
 * @return STATUS_OK on success, STATUS_??? otherwise.
 */
MFRC522::StatusCode MFRC522::MIFARE_TwoStepHelper(	byte command,	///< The command to use
													byte blockAddr,	///< The block (0-0xff) number.
													int32_t data		///< The data to transfer in step 2
													) {
	MFRC522::StatusCode result;
	byte cmdBuffer[2]; // We only need room for 2 bytes.
	
	// Step 1: Tell the PICC the command and block address
	cmdBuffer[0] = command;
	cmdBuffer[1] = blockAddr;
	result = PCD_MIFARE_Transceive(	cmdBuffer, 2); // Adds CRC_A and checks that the response is MF_ACK.
	if (result != STATUS_OK) {
		return result;
	}
	
	// Step 2: Transfer the data
	result = PCD_MIFARE_Transceive(	(byte *)&data, 4, true); // Adds CRC_A and accept timeout as success.
	if (result != STATUS_OK) {
		return result;
	}
	
	return STATUS_OK;
} // End MIFARE_TwoStepHelper()

/**
 * MIFARE Transfer writes the value stored in the volatile memory into one MIFARE Classic block.
 * For MIFARE Classic only. The sector containing the block must be authenticated before calling this function.
 * Only for blocks in "value block" mode, ie with access bits [C1 C2 C3] = [110] or [001].
 * 
 * @return STATUS_OK on success, STATUS_??? otherwise.
 */
MFRC522::StatusCode MFRC522::MIFARE_Transfer(	byte blockAddr ///< The block (0-0xff) number.
											) {
	MFRC522::StatusCode result;
	byte cmdBuffer[2]; // We only need room for 2 bytes.
	
	// Tell the PICC we want to transfer the result into block blockAddr.
	cmdBuffer[0] = PICC_CMD_MF_TRANSFER;
	cmdBuffer[1] = blockAddr;
	result = PCD_MIFARE_Transceive(	cmdBuffer, 2); // Adds CRC_A and checks that the response is MF_ACK.
	if (result != STATUS_OK) {
		return result;
	}
	return STATUS_OK;
} // End MIFARE_Transfer()

/**
 * Helper routine to read the current value from a Value Block.
 * 
 * Only for MIFARE Classic and only for blocks in "value block" mode, that
 * is: with access bits [C1 C2 C3] = [110] or [001]. The sector containing
 * the block must be authenticated before calling this function. 
 * 
 * @param[in]   blockAddr   The block (0x00-0xff) number.
 * @param[out]  value       Current value of the Value Block.
 * @return STATUS_OK on success, STATUS_??? otherwise.
  */
MFRC522::StatusCode MFRC522::MIFARE_GetValue(byte blockAddr, int32_t *value) {
	MFRC522::StatusCode status;
	byte buffer[18];
	byte size = sizeof(buffer);
	
	// Read the block
	status = MIFARE_Read(blockAddr, buffer, &size);
	if (status == STATUS_OK) {
		// Extract the value
		*value = (int32_t(buffer[3])<<24) | (int32_t(buffer[2])<<16) | (int32_t(buffer[1])<<8) | int32_t(buffer[0]);
	}
	return status;
} // End MIFARE_GetValue()

/**
 * Helper routine to write a specific value into a Value Block.
 * 
 * Only for MIFARE Classic and only for blocks in "value block" mode, that
 * is: with access bits [C1 C2 C3] = [110] or [001]. The sector containing
 * the block must be authenticated before calling this function. 
 * 
 * @param[in]   blockAddr   The block (0x00-0xff) number.
 * @param[in]   value       New value of the Value Block.
 * @return STATUS_OK on success, STATUS_??? otherwise.
 */
MFRC522::StatusCode MFRC522::MIFARE_SetValue(byte blockAddr, int32_t value) {
	byte buffer[18];
	
	// Translate the int32_t into 4 bytes; repeated 2x in value block
	buffer[0] = buffer[ 8] = (value & 0xFF);
	buffer[1] = buffer[ 9] = (value & 0xFF00) >> 8;
	buffer[2] = buffer[10] = (value & 0xFF0000) >> 16;
	buffer[3] = buffer[11] = (value & 0xFF000000) >> 24;
	// Inverse 4 bytes also found in value block
	buffer[4] = ~buffer[0];
	buffer[5] = ~buffer[1];
	buffer[6] = ~buffer[2];
	buffer[7] = ~buffer[3];
	// Address 2x with inverse address 2x
	buffer[12] = buffer[14] = blockAddr;
	buffer[13] = buffer[15] = ~blockAddr;
	
	// Write the whole data block
	return MIFARE_Write(blockAddr, buffer, 16);
} // End MIFARE_SetValue()

/**
 * Authenticate with a NTAG216.
 * 
 * Only for NTAG216. First implemented by Gargantuanman.
 * 
 * @param[in]   passWord   password.
 * @param[in]   pACK       result success???.
 * @return STATUS_OK on success, STATUS_??? otherwise.
 */
MFRC522::StatusCode MFRC522::PCD_NTAG216_AUTH(byte* passWord, byte pACK[]) //Authenticate with 32bit password
{
	// TODO: Fix cmdBuffer length and rxlength. They really should match.
	//       (Better still, rxlength should not even be necessary.)

	MFRC522::StatusCode result;
	byte				cmdBuffer[18]; // We need room for 16 bytes data and 2 bytes CRC_A.
	
	cmdBuffer[0] = 0x1B; //Comando de autentificacion
	
	for (byte i = 0; i<4; i++)
		cmdBuffer[i+1] = passWord[i];
	
	result = PCD_CalculateCRC(cmdBuffer, 5, &cmdBuffer[5]);
	
	if (result!=STATUS_OK) {
		return result;
	}
	
	// Transceive the data, store the reply in cmdBuffer[]
	byte waitIRq		= 0x30;	// RxIRq and IdleIRq
//	byte cmdBufferSize	= sizeof(cmdBuffer);
	byte validBits		= 0;
	byte rxlength		= 5;
	result = PCD_CommunicateWithPICC(PCD_Transceive, waitIRq, cmdBuffer, 7, cmdBuffer, &rxlength, &validBits);
	
	pACK[0] = cmdBuffer[0];
	pACK[1] = cmdBuffer[1];
	
	if (result!=STATUS_OK) {
		return result;
	}
	
	return STATUS_OK;
} // End PCD_NTAG216_AUTH()


/////////////////////////////////////////////////////////////////////////////////////
// Support functions
/////////////////////////////////////////////////////////////////////////////////////

/**
 * Wrapper for MIFARE protocol communication.
 * Adds CRC_A, executes the Transceive command and checks that the response is MF_ACK or a timeout.
 * 
 * @return STATUS_OK on success, STATUS_??? otherwise.
 */
MFRC522::StatusCode MFRC522::PCD_MIFARE_Transceive(	byte *sendData,		///< Pointer to the data to transfer to the FIFO. Do NOT include the CRC_A.
													byte sendLen,		///< Number of bytes in sendData.
													bool acceptTimeout	///< True => A timeout is also success
												) {
	MFRC522::StatusCode result;
	byte cmdBuffer[18]; // We need room for 16 bytes data and 2 bytes CRC_A.
	
	// Sanity check
	if (sendData == NULL || sendLen > 16) {
		return STATUS_INVALID;
	}
	
	// Copy sendData[] to cmdBuffer[] and add CRC_A
	memcpy(cmdBuffer, sendData, sendLen);
	result = PCD_CalculateCRC(cmdBuffer, sendLen, &cmdBuffer[sendLen]);
	if (result != STATUS_OK) { 
		return result;
	}
	sendLen += 2;
	
	// Transceive the data, store the reply in cmdBuffer[]
	byte waitIRq = 0x30;		// RxIRq and IdleIRq
	byte cmdBufferSize = sizeof(cmdBuffer);
	byte validBits = 0;
	result = PCD_CommunicateWithPICC(PCD_Transceive, waitIRq, cmdBuffer, sendLen, cmdBuffer, &cmdBufferSize, &validBits);
	if (acceptTimeout && result == STATUS_TIMEOUT) {
		return STATUS_OK;
	}
	if (result != STATUS_OK) {
		return result;
	}
	// The PICC must reply with a 4 bit ACK
	if (cmdBufferSize != 1 || validBits != 4) {
		return STATUS_ERROR;
	}
	if (cmdBuffer[0] != MF_ACK) {
		return STATUS_MIFARE_NACK;
	}
	return STATUS_OK;
} // End PCD_MIFARE_Transceive()

/**
 * Returns a __FlashStringHelper pointer to a status code name.
 * 
 * @return const __FlashStringHelper *
 */
const __FlashStringHelper *MFRC522::GetStatusCodeName(MFRC522::StatusCode code	///< One of the StatusCode enums.
										) {
	switch (code) {
		case STATUS_OK:				return F("Success.");
		case STATUS_ERROR:			return F("Error in communication.");
		case STATUS_COLLISION:		return F("Collission detected.");
		case STATUS_TIMEOUT:		return F("Timeout in communication.");
		case STATUS_NO_ROOM:		return F("A buffer is not big enough.");
		case STATUS_INTERNAL_ERROR:	return F("Internal error in the code. Should not happen.");
		case STATUS_INVALID:		return F("Invalid argument.");
		case STATUS_CRC_WRONG:		return F("The CRC_A does not match.");
		case STATUS_MIFARE_NACK:	return F("A MIFARE PICC responded with NAK.");
		default:					return F("Unknown error");
	}
} // End GetStatusCodeName()

/**
 * Get the PICC type.
 *
 * @return PICC_Type
 */
MFRC522::PICC_Type MFRC522::PICC_GetType(TagInfo *tag		///< The TagInfo returned from PICC_Select().
) {
	// http://www.nxp.com/documents/application_note/AN10833.pdf 
	// 3.2 Coding of Select Acknowledge (SAK)
	// ignore 8-bit (iso14443 starts with LSBit = bit 1)
	// fixes wrong type for manufacturer Infineon (http://nfc-tools.org/index.php?title=ISO14443A)
	byte sak = tag->uid.sak & 0x7F;
	switch (sak) {
	case 0x04:	return PICC_TYPE_NOT_COMPLETE;	// UID not complete
	case 0x09:	return PICC_TYPE_MIFARE_MINI;
	case 0x08:	return PICC_TYPE_MIFARE_1K;
	case 0x18:	return PICC_TYPE_MIFARE_4K;
	case 0x00:	return PICC_TYPE_MIFARE_UL;
	case 0x10:
	case 0x11:	return PICC_TYPE_MIFARE_PLUS;
	case 0x01:	return PICC_TYPE_TNP3XXX;
	case 0x20:
		if (tag->atqa == 0x0344)
			return PICC_TYPE_MIFARE_DESFIRE;
		return PICC_TYPE_ISO_14443_4;
	case 0x40:	return PICC_TYPE_ISO_18092;
	default:	return PICC_TYPE_UNKNOWN;
	}
} // End PICC_GetType()

/**
 * Translates the SAK (Select Acknowledge) to a PICC type.
 * 
 * @return PICC_Type
 */
MFRC522::PICC_Type MFRC522::PICC_GetType(byte sak		///< The SAK byte returned from PICC_Select().
										) {
	// http://www.nxp.com/documents/application_note/AN10833.pdf 
	// 3.2 Coding of Select Acknowledge (SAK)
	// ignore 8-bit (iso14443 starts with LSBit = bit 1)
	// fixes wrong type for manufacturer Infineon (http://nfc-tools.org/index.php?title=ISO14443A)
	sak &= 0x7F;
	switch (sak) {
		case 0x04:	return PICC_TYPE_NOT_COMPLETE;	// UID not complete
		case 0x09:	return PICC_TYPE_MIFARE_MINI;
		case 0x08:	return PICC_TYPE_MIFARE_1K;
		case 0x18:	return PICC_TYPE_MIFARE_4K;
		case 0x00:	return PICC_TYPE_MIFARE_UL;
		case 0x10:
		case 0x11:	return PICC_TYPE_MIFARE_PLUS;
		case 0x01:	return PICC_TYPE_TNP3XXX;
		case 0x20:	return PICC_TYPE_ISO_14443_4;
		case 0x40:	return PICC_TYPE_ISO_18092;
		default:	return PICC_TYPE_UNKNOWN;
	}
} // End PICC_GetType()

/**
 * Returns a __FlashStringHelper pointer to the PICC type name.
 * 
 * @return const __FlashStringHelper *
 */
const __FlashStringHelper *MFRC522::PICC_GetTypeName(PICC_Type piccType	///< One of the PICC_Type enums.
													) {
	switch (piccType) {
		case PICC_TYPE_ISO_14443_4:		return F("PICC compliant with ISO/IEC 14443-4");
		case PICC_TYPE_ISO_18092:		return F("PICC compliant with ISO/IEC 18092 (NFC)");
		case PICC_TYPE_MIFARE_MINI:		return F("MIFARE Mini, 320 bytes");
		case PICC_TYPE_MIFARE_1K:		return F("MIFARE 1KB");
		case PICC_TYPE_MIFARE_4K:		return F("MIFARE 4KB");
		case PICC_TYPE_MIFARE_UL:		return F("MIFARE Ultralight or Ultralight C");
		case PICC_TYPE_MIFARE_PLUS:		return F("MIFARE Plus");
		case PICC_TYPE_MIFARE_DESFIRE:	return F("MIFARE DESFire");
		case PICC_TYPE_TNP3XXX:			return F("MIFARE TNP3XXX");
		case PICC_TYPE_NOT_COMPLETE:	return F("SAK indicates UID is not complete.");
		case PICC_TYPE_UNKNOWN:
		default:						return F("Unknown type");
	}
} // End PICC_GetTypeName()

/**
 * Dumps debug info about the connected PCD to Serial.
 * Shows all known firmware versions
 */
void MFRC522::PCD_DumpVersionToSerial() {
	// Get the MFRC522 firmware version
	byte v = PCD_ReadRegister(VersionReg);
	Serial.print(F("Firmware Version: 0x"));
	Serial.print(v, HEX);
	// Lookup which version
	switch(v) {
		case 0x88: Serial.println(F(" = (clone)"));  break;
		case 0x90: Serial.println(F(" = v0.0"));     break;
		case 0x91: Serial.println(F(" = v1.0"));     break;
		case 0x92: Serial.println(F(" = v2.0"));     break;
		default:   Serial.println(F(" = (unknown)"));
	}
	// When 0x00 or 0xFF is returned, communication probably failed
	if ((v == 0x00) || (v == 0xFF))
		Serial.println(F("WARNING: Communication failure, is the MFRC522 properly connected?"));
} // End PCD_DumpVersionToSerial()

/**
 * Dumps debug info about the selected PICC to Serial.
 * On success the PICC is halted after dumping the data.
 * For MIFARE Classic the factory default key of 0xFFFFFFFFFFFF is tried.
 */
void MFRC522::PICC_DumpToSerial(TagInfo *tag)
{
	MIFARE_Key key;

	// Dump UID, SAK and Type
	PICC_DumpDetailsToSerial(tag);

	// Dump contents
	PICC_Type piccType = PICC_GetType(tag->uid.sak);
	switch (piccType) {
	case PICC_TYPE_MIFARE_MINI:
	case PICC_TYPE_MIFARE_1K:
	case PICC_TYPE_MIFARE_4K:
		// All keys are set to FFFFFFFFFFFFh at chip delivery from the factory.
		for (byte i = 0; i < 6; i++) {
			key.keyByte[i] = 0xFF;
		}
		PICC_DumpMifareClassicToSerial(&tag->uid, piccType, &key);
		break;

	case PICC_TYPE_MIFARE_UL:
		PICC_DumpMifareUltralightToSerial();
		break;

	case PICC_TYPE_ISO_14443_4:
	case PICC_TYPE_MIFARE_DESFIRE:
		PICC_DumpISO14443_4(tag);
		Serial.println(F("Dumping memory contents not implemented for that PICC type."));
		break;
	case PICC_TYPE_ISO_18092:
	case PICC_TYPE_MIFARE_PLUS:
	case PICC_TYPE_TNP3XXX:
		Serial.println(F("Dumping memory contents not implemented for that PICC type."));
		break;

	case PICC_TYPE_UNKNOWN:
	case PICC_TYPE_NOT_COMPLETE:
	default:
		break; // No memory dump here
	}

	Serial.println();
	PICC_HaltA(); // Already done if it was a MIFARE Classic PICC.
}

/**
 * Dumps debug info about the selected PICC to Serial.
 * On success the PICC is halted after dumping the data.
 * For MIFARE Classic the factory default key of 0xFFFFFFFFFFFF is tried.  
 *
 * @DEPRECATED Kept for bakward compatibility
 */
void MFRC522::PICC_DumpToSerial(Uid *uid	///< Pointer to Uid struct returned from a successful PICC_Select().
								) {
	MIFARE_Key key;
	
	// Dump UID, SAK and Type
	PICC_DumpDetailsToSerial(uid);
	
	// Dump contents
	PICC_Type piccType = PICC_GetType(uid->sak);
	switch (piccType) {
		case PICC_TYPE_MIFARE_MINI:
		case PICC_TYPE_MIFARE_1K:
		case PICC_TYPE_MIFARE_4K:
			// All keys are set to FFFFFFFFFFFFh at chip delivery from the factory.
			for (byte i = 0; i < 6; i++) {
				key.keyByte[i] = 0xFF;
			}
			PICC_DumpMifareClassicToSerial(uid, piccType, &key);
			break;
			
		case PICC_TYPE_MIFARE_UL:
			PICC_DumpMifareUltralightToSerial();
			break;
			
		case PICC_TYPE_ISO_14443_4:
		case PICC_TYPE_MIFARE_DESFIRE:
		case PICC_TYPE_ISO_18092:
		case PICC_TYPE_MIFARE_PLUS:
		case PICC_TYPE_TNP3XXX:
			Serial.println(F("Dumping memory contents not implemented for that PICC type."));
			break;
			
		case PICC_TYPE_UNKNOWN:
		case PICC_TYPE_NOT_COMPLETE:
		default:
			break; // No memory dump here
	}
	
	Serial.println();
	PICC_HaltA(); // Already done if it was a MIFARE Classic PICC.
} // End PICC_DumpToSerial()

/**
 * Dumps card info (UID,SAK,Type) about the selected PICC to Serial.
 */
void MFRC522::PICC_DumpDetailsToSerial(TagInfo *tag	///< Pointer to TagInfo struct returned from a successful PICC_Select().
) {
	// ATQA
	Serial.print(F("Card ATQA:"));
	if (((tag->atqa & 0xFF00u) >> 8) < 0x10)
		Serial.print(F(" 0"));
	Serial.print((tag->atqa & 0xFF00u) >> 8, HEX);
	if ((tag->atqa & 0x00FFu) < 0x10)
		Serial.print(F("0"));
	else
		Serial.print(F(" "));
	Serial.println(tag->atqa & 0x00FFu, HEX);

	// UID
	Serial.print(F("Card UID:"));
	for (byte i = 0; i < tag->uid.size; i++) {
		if (tag->uid.uidByte[i] < 0x10)
			Serial.print(F(" 0"));
		else
			Serial.print(F(" "));
		Serial.print(tag->uid.uidByte[i], HEX);
	}
	Serial.println();

	// SAK
	Serial.print(F("Card SAK: "));
	if (tag->uid.sak < 0x10)
		Serial.print(F("0"));
	Serial.println(tag->uid.sak, HEX);

	// (suggested) PICC type
	PICC_Type piccType = PICC_GetType(tag);
	Serial.print(F("PICC type: "));
	Serial.println(PICC_GetTypeName(piccType));
} // End PICC_DumpDetailsToSerial()

/**
 * Dumps card info (UID,SAK,Type) about the selected PICC to Serial.
 *
 * @DEPRECATED kept for backward compatibility
 */
void MFRC522::PICC_DumpDetailsToSerial(Uid *uid	///< Pointer to Uid struct returned from a successful PICC_Select().
									) {
	// UID
	Serial.print(F("Card UID:"));
	for (byte i = 0; i < uid->size; i++) {
		if(uid->uidByte[i] < 0x10)
			Serial.print(F(" 0"));
		else
			Serial.print(F(" "));
		Serial.print(uid->uidByte[i], HEX);
	} 
	Serial.println();
	
	// SAK
	Serial.print(F("Card SAK: "));
	if(uid->sak < 0x10)
		Serial.print(F("0"));
	Serial.println(uid->sak, HEX);
	
	// (suggested) PICC type
	PICC_Type piccType = PICC_GetType(uid->sak);
	Serial.print(F("PICC type: "));
	Serial.println(PICC_GetTypeName(piccType));
} // End PICC_DumpDetailsToSerial()

/**
 * Dumps memory contents of a MIFARE Classic PICC.
 * On success the PICC is halted after dumping the data.
 */
void MFRC522::PICC_DumpMifareClassicToSerial(	Uid *uid,			///< Pointer to Uid struct returned from a successful PICC_Select().
												PICC_Type piccType,	///< One of the PICC_Type enums.
												MIFARE_Key *key		///< Key A used for all sectors.
											) {
	byte no_of_sectors = 0;
	switch (piccType) {
		case PICC_TYPE_MIFARE_MINI:
			// Has 5 sectors * 4 blocks/sector * 16 bytes/block = 320 bytes.
			no_of_sectors = 5;
			break;
			
		case PICC_TYPE_MIFARE_1K:
			// Has 16 sectors * 4 blocks/sector * 16 bytes/block = 1024 bytes.
			no_of_sectors = 16;
			break;
			
		case PICC_TYPE_MIFARE_4K:
			// Has (32 sectors * 4 blocks/sector + 8 sectors * 16 blocks/sector) * 16 bytes/block = 4096 bytes.
			no_of_sectors = 40;
			break;
			
		default: // Should not happen. Ignore.
			break;
	}
	
	// Dump sectors, highest address first.
	if (no_of_sectors) {
		Serial.println(F("Sector Block   0  1  2  3   4  5  6  7   8  9 10 11  12 13 14 15  AccessBits"));
		for (int8_t i = no_of_sectors - 1; i >= 0; i--) {
			PICC_DumpMifareClassicSectorToSerial(uid, key, i);
		}
	}
	PICC_HaltA(); // Halt the PICC before stopping the encrypted session.
	PCD_StopCrypto1();
} // End PICC_DumpMifareClassicToSerial()

/**
 * Dumps memory contents of a sector of a MIFARE Classic PICC.
 * Uses PCD_Authenticate(), MIFARE_Read() and PCD_StopCrypto1.
 * Always uses PICC_CMD_MF_AUTH_KEY_A because only Key A can always read the sector trailer access bits.
 */
void MFRC522::PICC_DumpMifareClassicSectorToSerial(Uid *uid,			///< Pointer to Uid struct returned from a successful PICC_Select().
													MIFARE_Key *key,	///< Key A for the sector.
													byte sector			///< The sector to dump, 0..39.
													) {
	MFRC522::StatusCode status;
	byte firstBlock;		// Address of lowest address to dump actually last block dumped)
	byte no_of_blocks;		// Number of blocks in sector
	bool isSectorTrailer;	// Set to true while handling the "last" (ie highest address) in the sector.
	
	// The access bits are stored in a peculiar fashion.
	// There are four groups:
	//		g[3]	Access bits for the sector trailer, block 3 (for sectors 0-31) or block 15 (for sectors 32-39)
	//		g[2]	Access bits for block 2 (for sectors 0-31) or blocks 10-14 (for sectors 32-39)
	//		g[1]	Access bits for block 1 (for sectors 0-31) or blocks 5-9 (for sectors 32-39)
	//		g[0]	Access bits for block 0 (for sectors 0-31) or blocks 0-4 (for sectors 32-39)
	// Each group has access bits [C1 C2 C3]. In this code C1 is MSB and C3 is LSB.
	// The four CX bits are stored together in a nible cx and an inverted nible cx_.
	byte c1, c2, c3;		// Nibbles
	byte c1_, c2_, c3_;		// Inverted nibbles
	bool invertedError;		// True if one of the inverted nibbles did not match
	byte g[4];				// Access bits for each of the four groups.
	byte group;				// 0-3 - active group for access bits
	bool firstInGroup;		// True for the first block dumped in the group
	
	// Determine position and size of sector.
	if (sector < 32) { // Sectors 0..31 has 4 blocks each
		no_of_blocks = 4;
		firstBlock = sector * no_of_blocks;
	}
	else if (sector < 40) { // Sectors 32-39 has 16 blocks each
		no_of_blocks = 16;
		firstBlock = 128 + (sector - 32) * no_of_blocks;
	}
	else { // Illegal input, no MIFARE Classic PICC has more than 40 sectors.
		return;
	}
		
	// Dump blocks, highest address first.
	byte byteCount;
	byte buffer[18];
	byte blockAddr;
	isSectorTrailer = true;
	invertedError = false;	// Avoid "unused variable" warning.
	for (int8_t blockOffset = no_of_blocks - 1; blockOffset >= 0; blockOffset--) {
		blockAddr = firstBlock + blockOffset;
		// Sector number - only on first line
		if (isSectorTrailer) {
			if(sector < 10)
				Serial.print(F("   ")); // Pad with spaces
			else
				Serial.print(F("  ")); // Pad with spaces
			Serial.print(sector);
			Serial.print(F("   "));
		}
		else {
			Serial.print(F("       "));
		}
		// Block number
		if(blockAddr < 10)
			Serial.print(F("   ")); // Pad with spaces
		else {
			if(blockAddr < 100)
				Serial.print(F("  ")); // Pad with spaces
			else
				Serial.print(F(" ")); // Pad with spaces
		}
		Serial.print(blockAddr);
		Serial.print(F("  "));
		// Establish encrypted communications before reading the first block
		if (isSectorTrailer) {
			status = PCD_Authenticate(PICC_CMD_MF_AUTH_KEY_A, firstBlock, key, uid);
			if (status != STATUS_OK) {
				Serial.print(F("PCD_Authenticate() failed: "));
				Serial.println(GetStatusCodeName(status));
				return;
			}
		}
		// Read block
		byteCount = sizeof(buffer);
		status = MIFARE_Read(blockAddr, buffer, &byteCount);
		if (status != STATUS_OK) {
			Serial.print(F("MIFARE_Read() failed: "));
			Serial.println(GetStatusCodeName(status));
			continue;
		}
		// Dump data
		for (byte index = 0; index < 16; index++) {
			if(buffer[index] < 0x10)
				Serial.print(F(" 0"));
			else
				Serial.print(F(" "));
			Serial.print(buffer[index], HEX);
			if ((index % 4) == 3) {
				Serial.print(F(" "));
			}
		}
		// Parse sector trailer data
		if (isSectorTrailer) {
			c1  = buffer[7] >> 4;
			c2  = buffer[8] & 0xF;
			c3  = buffer[8] >> 4;
			c1_ = buffer[6] & 0xF;
			c2_ = buffer[6] >> 4;
			c3_ = buffer[7] & 0xF;
			invertedError = (c1 != (~c1_ & 0xF)) || (c2 != (~c2_ & 0xF)) || (c3 != (~c3_ & 0xF));
			g[0] = ((c1 & 1) << 2) | ((c2 & 1) << 1) | ((c3 & 1) << 0);
			g[1] = ((c1 & 2) << 1) | ((c2 & 2) << 0) | ((c3 & 2) >> 1);
			g[2] = ((c1 & 4) << 0) | ((c2 & 4) >> 1) | ((c3 & 4) >> 2);
			g[3] = ((c1 & 8) >> 1) | ((c2 & 8) >> 2) | ((c3 & 8) >> 3);
			isSectorTrailer = false;
		}
		
		// Which access group is this block in?
		if (no_of_blocks == 4) {
			group = blockOffset;
			firstInGroup = true;
		}
		else {
			group = blockOffset / 5;
			firstInGroup = (group == 3) || (group != (blockOffset + 1) / 5);
		}
		
		if (firstInGroup) {
			// Print access bits
			Serial.print(F(" [ "));
			Serial.print((g[group] >> 2) & 1, DEC); Serial.print(F(" "));
			Serial.print((g[group] >> 1) & 1, DEC); Serial.print(F(" "));
			Serial.print((g[group] >> 0) & 1, DEC);
			Serial.print(F(" ] "));
			if (invertedError) {
				Serial.print(F(" Inverted access bits did not match! "));
			}
		}
		
		if (group != 3 && (g[group] == 1 || g[group] == 6)) { // Not a sector trailer, a value block
			int32_t value = (int32_t(buffer[3])<<24) | (int32_t(buffer[2])<<16) | (int32_t(buffer[1])<<8) | int32_t(buffer[0]);
			Serial.print(F(" Value=0x")); Serial.print(value, HEX);
			Serial.print(F(" Adr=0x")); Serial.print(buffer[12], HEX);
		}
		Serial.println();
	}
	
	return;
} // End PICC_DumpMifareClassicSectorToSerial()

/**
 * Dumps memory contents of a MIFARE Ultralight PICC.
 */
void MFRC522::PICC_DumpMifareUltralightToSerial() {
	MFRC522::StatusCode status;
	byte byteCount;
	byte buffer[18];
	byte i;
	
	Serial.println(F("Page  0  1  2  3"));
	// Try the mpages of the original Ultralight. Ultralight C has more pages.
	for (byte page = 0; page < 16; page +=4) { // Read returns data for 4 pages at a time.
		// Read pages
		byteCount = sizeof(buffer);
		status = MIFARE_Read(page, buffer, &byteCount);
		if (status != STATUS_OK) {
			Serial.print(F("MIFARE_Read() failed: "));
			Serial.println(GetStatusCodeName(status));
			break;
		}
		// Dump data
		for (byte offset = 0; offset < 4; offset++) {
			i = page + offset;
			if(i < 10)
				Serial.print(F("  ")); // Pad with spaces
			else
				Serial.print(F(" ")); // Pad with spaces
			Serial.print(i);
			Serial.print(F("  "));
			for (byte index = 0; index < 4; index++) {
				i = 4 * offset + index;
				if(buffer[i] < 0x10)
					Serial.print(F(" 0"));
				else
					Serial.print(F(" "));
				Serial.print(buffer[i], HEX);
			}
			Serial.println();
		}
	}
} // End PICC_DumpMifareUltralightToSerial()

/**
 * Dumps memory contents of a ISO-14443-4 PICC.
 */
void MFRC522::PICC_DumpISO14443_4(TagInfo *tag)
{
	// ATS
	if (tag->ats.size > 0x00) {	// The first byte is the ATS length including the length byte
		Serial.print(F("Card ATS:"));
		for (byte offset = 0; offset < tag->ats.size; offset++) {
			if (tag->ats.data[offset] < 0x10)
				Serial.print(F(" 0"));
			else
				Serial.print(F(" "));
			Serial.print(tag->ats.data[offset], HEX);
		}
		Serial.println();
	}
	
} // End PICC_DumpISO14443_4

/**
 * Calculates the bit pattern needed for the specified access bits. In the [C1 C2 C3] tuples C1 is MSB (=4) and C3 is LSB (=1).
 */
void MFRC522::MIFARE_SetAccessBits(	byte *accessBitBuffer,	///< Pointer to byte 6, 7 and 8 in the sector trailer. Bytes [0..2] will be set.
									byte g0,				///< Access bits [C1 C2 C3] for block 0 (for sectors 0-31) or blocks 0-4 (for sectors 32-39)
									byte g1,				///< Access bits C1 C2 C3] for block 1 (for sectors 0-31) or blocks 5-9 (for sectors 32-39)
									byte g2,				///< Access bits C1 C2 C3] for block 2 (for sectors 0-31) or blocks 10-14 (for sectors 32-39)
									byte g3					///< Access bits C1 C2 C3] for the sector trailer, block 3 (for sectors 0-31) or block 15 (for sectors 32-39)
								) {
	byte c1 = ((g3 & 4) << 1) | ((g2 & 4) << 0) | ((g1 & 4) >> 1) | ((g0 & 4) >> 2);
	byte c2 = ((g3 & 2) << 2) | ((g2 & 2) << 1) | ((g1 & 2) << 0) | ((g0 & 2) >> 1);
	byte c3 = ((g3 & 1) << 3) | ((g2 & 1) << 2) | ((g1 & 1) << 1) | ((g0 & 1) << 0);
	
	accessBitBuffer[0] = (~c2 & 0xF) << 4 | (~c1 & 0xF);
	accessBitBuffer[1] =          c1 << 4 | (~c3 & 0xF);
	accessBitBuffer[2] =          c3 << 4 | c2;
} // End MIFARE_SetAccessBits()


/**
 * Performs the "magic sequence" needed to get Chinese UID changeable
 * Mifare cards to allow writing to sector 0, where the card UID is stored.
 *
 * Note that you do not need to have selected the card through REQA or WUPA,
 * this sequence works immediately when the card is in the reader vicinity.
 * This means you can use this method even on "bricked" cards that your reader does
 * not recognise anymore (see MFRC522::MIFARE_UnbrickUidSector).
 * 
 * Of course with non-bricked devices, you're free to select them before calling this function.
 */
bool MFRC522::MIFARE_OpenUidBackdoor(bool logErrors) {
	// Magic sequence:
	// > 50 00 57 CD (HALT + CRC)
	// > 40 (7 bits only)
	// < A (4 bits only)
	// > 43
	// < A (4 bits only)
	// Then you can write to sector 0 without authenticating
	
	PICC_HaltA(); // 50 00 57 CD
	
	byte cmd = 0x40;
	byte validBits = 7; /* Our command is only 7 bits. After receiving card response,
						  this will contain amount of valid response bits. */
	byte response[32]; // Card's response is written here
	byte received;
	MFRC522::StatusCode status = PCD_TransceiveData(&cmd, (byte)1, response, &received, &validBits, (byte)0, false); // 40
	if(status != STATUS_OK) {
		if(logErrors) {
			Serial.println(F("Card did not respond to 0x40 after HALT command. Are you sure it is a UID changeable one?"));
			Serial.print(F("Error name: "));
			Serial.println(GetStatusCodeName(status));
		}
		return false;
	}
	if (received != 1 || response[0] != 0x0A) {
		if (logErrors) {
			Serial.print(F("Got bad response on backdoor 0x40 command: "));
			Serial.print(response[0], HEX);
			Serial.print(F(" ("));
			Serial.print(validBits);
			Serial.print(F(" valid bits)\r\n"));
		}
		return false;
	}
	
	cmd = 0x43;
	validBits = 8;
	status = PCD_TransceiveData(&cmd, (byte)1, response, &received, &validBits, (byte)0, false); // 43
	if(status != STATUS_OK) {
		if(logErrors) {
			Serial.println(F("Error in communication at command 0x43, after successfully executing 0x40"));
			Serial.print(F("Error name: "));
			Serial.println(GetStatusCodeName(status));
		}
		return false;
	}
	if (received != 1 || response[0] != 0x0A) {
		if (logErrors) {
			Serial.print(F("Got bad response on backdoor 0x43 command: "));
			Serial.print(response[0], HEX);
			Serial.print(F(" ("));
			Serial.print(validBits);
			Serial.print(F(" valid bits)\r\n"));
		}
		return false;
	}
	
	// You can now write to sector 0 without authenticating!
	return true;
} // End MIFARE_OpenUidBackdoor()

/**
 * Reads entire block 0, including all manufacturer data, and overwrites
 * that block with the new UID, a freshly calculated BCC, and the original
 * manufacturer data.
 *
 * It assumes a default KEY A of 0xFFFFFFFFFFFF.
 * Make sure to have selected the card before this function is called.
 */
bool MFRC522::MIFARE_SetUid(byte *newUid, byte uidSize, bool logErrors) {
	
	// UID + BCC byte can not be larger than 16 together
	if (!newUid || !uidSize || uidSize > 15) {
		if (logErrors) {
			Serial.println(F("New UID buffer empty, size 0, or size > 15 given"));
		}
		return false;
	}
	
	// Authenticate for reading
	MIFARE_Key key = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	MFRC522::StatusCode status = PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, (byte)1, &key, &uid);
	if (status != STATUS_OK) {
		
		if (status == STATUS_TIMEOUT) {
			// We get a read timeout if no card is selected yet, so let's select one
			
			// Wake the card up again if sleeping
//			  byte atqa_answer[2];
//			  byte atqa_size = 2;
//			  PICC_WakeupA(atqa_answer, &atqa_size);
			
			if (!PICC_IsNewCardPresent() || !PICC_ReadCardSerial()) {
				Serial.println(F("No card was previously selected, and none are available. Failed to set UID."));
				return false;
			}
			
			status = PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, (byte)1, &key, &uid);
			if (status != STATUS_OK) {
				// We tried, time to give up
				if (logErrors) {
					Serial.println(F("Failed to authenticate to card for reading, could not set UID: "));
					Serial.println(GetStatusCodeName(status));
				}
				return false;
			}
		}
		else {
			if (logErrors) {
				Serial.print(F("PCD_Authenticate() failed: "));
				Serial.println(GetStatusCodeName(status));
			}
			return false;
		}
	}
	
	// Read block 0
	byte block0_buffer[18];
	byte byteCount = sizeof(block0_buffer);
	status = MIFARE_Read((byte)0, block0_buffer, &byteCount);
	if (status != STATUS_OK) {
		if (logErrors) {
			Serial.print(F("MIFARE_Read() failed: "));
			Serial.println(GetStatusCodeName(status));
			Serial.println(F("Are you sure your KEY A for sector 0 is 0xFFFFFFFFFFFF?"));
		}
		return false;
	}
	
	// Write new UID to the data we just read, and calculate BCC byte
	byte bcc = 0;
	for (uint8_t i = 0; i < uidSize; i++) {
		block0_buffer[i] = newUid[i];
		bcc ^= newUid[i];
	}
	
	// Write BCC byte to buffer
	block0_buffer[uidSize] = bcc;
	
	// Stop encrypted traffic so we can send raw bytes
	PCD_StopCrypto1();
	
	// Activate UID backdoor
	if (!MIFARE_OpenUidBackdoor(logErrors)) {
		if (logErrors) {
			Serial.println(F("Activating the UID backdoor failed."));
		}
		return false;
	}
	
	// Write modified block 0 back to card
	status = MIFARE_Write((byte)0, block0_buffer, (byte)16);
	if (status != STATUS_OK) {
		if (logErrors) {
			Serial.print(F("MIFARE_Write() failed: "));
			Serial.println(GetStatusCodeName(status));
		}
		return false;
	}
	
	// Wake the card up again
	byte atqa_answer[2];
	byte atqa_size = 2;
	PICC_WakeupA(atqa_answer, &atqa_size);
	
	return true;
}

/**
 * Resets entire sector 0 to zeroes, so the card can be read again by readers.
 */
bool MFRC522::MIFARE_UnbrickUidSector(bool logErrors) {
	MIFARE_OpenUidBackdoor(logErrors);
	
	byte block0_buffer[] = {0x01, 0x02, 0x03, 0x04, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	
	// Write modified block 0 back to card
	MFRC522::StatusCode status = MIFARE_Write((byte)0, block0_buffer, (byte)16);
	if (status != STATUS_OK) {
		if (logErrors) {
			Serial.print(F("MIFARE_Write() failed: "));
			Serial.println(GetStatusCodeName(status));
		}
		return false;
	}
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////
// Convenience functions - does not add extra functionality
/////////////////////////////////////////////////////////////////////////////////////

/**
 * Returns true if a PICC responds to PICC_CMD_REQA.
 * Only "new" cards in state IDLE are invited. Sleeping cards in state HALT are ignored.
 * 
 * @return bool
 */
bool MFRC522::PICC_IsNewCardPresent() {
	byte bufferATQA[2];
	byte bufferSize = sizeof(bufferATQA);

	// Reset baud rates
	PCD_WriteRegister(TxModeReg, 0x00);
	PCD_WriteRegister(RxModeReg, 0x00);
	// Reset ModWidthReg
	PCD_WriteRegister(ModWidthReg, 0x26);

	MFRC522::StatusCode result = PICC_RequestA(bufferATQA, &bufferSize);

	if (result == STATUS_OK || result == STATUS_COLLISION) {
		tag.atqa = ((uint16_t)bufferATQA[1] << 8) | bufferATQA[0];
		tag.ats.size = 0;
		tag.ats.fsc = 32;	// default FSC value

		// Defaults for TA1
		tag.ats.ta1.transmitted = false;
		tag.ats.ta1.sameD = false;
		tag.ats.ta1.ds = BITRATE_106KBITS;
		tag.ats.ta1.dr = BITRATE_106KBITS;

		// Defaults for TB1
		tag.ats.tb1.transmitted = false;
		tag.ats.tb1.fwi = 0;	// TODO: Don't know the default for this!
		tag.ats.tb1.sfgi = 0;	// The default value of SFGI is 0 (meaning that the card does not need any particular SFGT)

		// Defaults for TC1
		tag.ats.tc1.transmitted = false;
		tag.ats.tc1.supportsCID = true;
		tag.ats.tc1.supportsNAD = false;

		memset(tag.ats.data, 0, FIFO_SIZE - 2);

		tag.blockNumber = false;
		return true;
	}
	return false;
} // End PICC_IsNewCardPresent()

/**
 * Simple wrapper around PICC_Select.
 * Returns true if a UID could be read.
 * Remember to call PICC_IsNewCardPresent(), PICC_RequestA() or PICC_WakeupA() first.
 * The read UID is available in the class variable uid.
 * 
 * @return bool
 */
bool MFRC522::PICC_ReadCardSerial() {
	MFRC522::StatusCode result = PICC_Select(&tag.uid);

	// Backward compatibility
	uid.size = tag.uid.size;
	uid.sak = tag.uid.sak;
	memcpy(uid.uidByte, tag.uid.uidByte, sizeof(tag.uid.uidByte));
	
	if (result != STATUS_OK)
		return false;

	return true;
} // End 
