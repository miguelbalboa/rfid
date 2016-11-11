/*   
 *	MFRC522.h
 *
 *  Created on: 2016
 *      Author: Luis Fernando Guerreiro
 *
 * Released into the public domain. 
 *   Check UNLICENSE file in project root 
 *	 
 * LPCOpen compatible C library to be used with the NXP MFRC522 RFID module,
 *  see the datasheet at: http://www.nxp.com/documents/data_sheet/MFRC522.pdf
 *  
 * This is an adaptation of the C++ Arduino library , available at:
 * 	https://github.com/miguelbalboa/rfid.git
 * The original library is released to the public domain.
 *
 */

#ifndef MFRC522_h
#define MFRC522_h

// Adjustments to work with the NXP LPCXpresso4337 board
#include "board.h"
#include <stdint.h>
#include <string.h> //some functions need NULL to be defined
#include "delay.h"

/*******************************************************************************
 * Types/enumerations/variables
 ******************************************************************************/
// send only one byte per transfer, see WriteRegister functions
#define BUFFER_SIZE  1 
// Defined as 4MHz in the original library
#define MFRC522_BIT_RATE 4000000 
// Used for ADT object allocation
#define MFRC_MAX_INSTANCES 2	 

static const uint8_t FIFO_SIZE = 64; // Size of the MFRC522 FIFO

/**
 * MFRC522 registers. Described in chapter 9 of the datasheet.
 * When using SPI all addresses are shifted one bit left in the "SPI address
 * (section 8.1.2.3)
 */
enum PCD_Register {
	// Page 0: Command and status
	//						  0x00			// reserved for future use
	CommandReg = 0x01 << 1, // starts and stops command execution
	ComIEnReg = 0x02 << 1,  // enable and disable interrupt request control bits
	DivIEnReg = 0x03 << 1,  // enable and disable interrupt request control bits
	ComIrqReg = 0x04 << 1,  // interrupt request bits
	DivIrqReg = 0x05 << 1,  // interrupt request bits
	ErrorReg = 0x06 << 1,   // error bits showing the error status of the last
							// command executed
	Status1Reg = 0x07 << 1, // communication status bits
	Status2Reg = 0x08 << 1, // receiver and transmitter status bits
	FIFODataReg = 0x09 << 1,   // input and output of 64 byte FIFO buffer
	FIFOLevelReg = 0x0A << 1,  // number of bytes stored in the FIFO buffer
	WaterLevelReg = 0x0B << 1, // level for FIFO underflow and overflow warning
	ControlReg = 0x0C << 1,	// miscellaneous control registers
	BitFramingReg = 0x0D << 1, // adjustments for bit-oriented frames
	CollReg = 0x0E << 1, // bit position of the first bit-collision detected on
						 // the RF interface
	//						  0x0F			// reserved for future use

	// Page 1: Command
	// 						  0x10			// reserved for future use
	ModeReg = 0x11 << 1, // defines general modes for transmitting and receiving
	TxModeReg = 0x12 << 1,	// defines transmission data rate and framing
	RxModeReg = 0x13 << 1,	// defines reception data rate and framing
	TxControlReg = 0x14 << 1, // controls the logical behavior of the antenna
							  // driver pins TX1 and TX2
	TxASKReg = 0x15 << 1, // controls the setting of the transmission modulation
	TxSelReg = 0x16 << 1, // selects the internal sources for the antenna driver
	RxSelReg = 0x17 << 1, // selects internal receiver settings
	RxThresholdReg = 0x18 << 1, // selects thresholds for the bit decoder
	DemodReg = 0x19 << 1,		// defines demodulator settings
	// 						  0x1A			// reserved for future use
	// 						  0x1B			// reserved for future use
	MfTxReg =
		0x1C << 1, // controls some MIFARE communication transmit parameters
	MfRxReg =
		0x1D << 1, // controls some MIFARE communication receive parameters
	// 						  0x1E			// reserved for future use
	SerialSpeedReg =
		0x1F << 1, // selects the speed of the serial UART interface

	// Page 2: Configuration
	// 						  0x20			// reserved for future use
	CRCResultRegH =
		0x21 << 1, // shows the MSB and LSB values of the CRC calculation
	CRCResultRegL = 0x22 << 1,
	// 						  0x23			// reserved for future use
	ModWidthReg = 0x24 << 1, // controls the ModWidth setting?
	// 						  0x25			// reserved for future use
	RFCfgReg = 0x26 << 1,  // configures the receiver gain
	GsNReg = 0x27 << 1,	// selects the conductance of the antenna driver pins
						   // TX1 and TX2 for modulation
	CWGsPReg = 0x28 << 1,  // defines the conductance of the p-driver output
						   // during periods of no modulation
	ModGsPReg = 0x29 << 1, // defines the conductance of the p-driver output
						   // during periods of modulation
	TModeReg = 0x2A << 1,  // defines settings for the internal timer
	TPrescalerReg = 0x2B << 1, // the lower 8 bits of the TPrescaler value. The
							   // 4 high bits are in TModeReg.
	TReloadRegH = 0x2C << 1,   // defines the 16-bit timer reload value
	TReloadRegL = 0x2D << 1,
	TCounterValueRegH = 0x2E << 1, // shows the 16-bit timer value
	TCounterValueRegL = 0x2F << 1,

	// Page 3: Test Registers
	// 						  0x30			// reserved for future use
	TestSel1Reg = 0x31 << 1,  // general test signal configuration
	TestSel2Reg = 0x32 << 1,  // general test signal configuration
	TestPinEnReg = 0x33 << 1, // enables pin output driver on pins D1 to D7
	TestPinValueReg =
		0x34
		<< 1, // defines the values for D1 to D7 when it is used as an I/O bus
	TestBusReg = 0x35 << 1,	// shows the status of the internal test bus
	AutoTestReg = 0x36 << 1,   // controls the digital self-test
	VersionReg = 0x37 << 1,	// shows the software version
	AnalogTestReg = 0x38 << 1, // controls the pins AUX1 and AUX2
	TestDAC1Reg = 0x39 << 1,   // defines the test value for TestDAC1
	TestDAC2Reg = 0x3A << 1,   // defines the test value for TestDAC2
	TestADCReg = 0x3B << 1	 // shows the value of ADC I and Q channels
	// 						  0x3C			// reserved for production tests
	// 						  0x3D			// reserved for production tests
	// 						  0x3E			// reserved for production tests
	// 						  0x3F			// reserved for production tests
};

/**
 * MFRC522 commands. Described in chapter 10 of the datasheet.
 */
typedef enum _PCD_Command {
	PCD_Idle = 0x00,			 // no action, cancels current command execution
	PCD_Mem = 0x01,				 // stores 25 bytes into the internal buffer
	PCD_GenerateRandomID = 0x02, // generates a 10-byte random ID number
	PCD_CalcCRC = 0x03, // activates the CRC co-processor or performs a self-test
	PCD_Transmit = 0x04,	// transmits data from the FIFO buffer
	PCD_NoCmdChange = 0x07, // no command change, can be used to modify the
							// CommandReg register bits without affecting the
							// command, for example, the PowerDown bit
	PCD_Receive = 0x08,		// activates the receiver circuits
	PCD_Transceive = 0x0C,  // transmits data from FIFO buffer to antenna and
							// automatically activates the receiver after
							// transmission
	PCD_MFAuthent =
		0x0E, // performs the MIFARE standard authentication as a reader
	PCD_SoftReset = 0x0F // resets the MFRC522
} PCD_Command;

/**
 * MFRC522 RxGain[2:0] masks, defines the receiver's signal voltage gain factor
 * (on the PCD).
 * Described in 9.3.3.6 / table 98 of the datasheet. 
 */
typedef enum _PCD_RxGain {
	RxGain_18dB = 0x00 << 4, // 000b - 18 dB, minimum
	RxGain_23dB = 0x01 << 4, // 001b - 23 dB
	RxGain_18dB_2 =
		0x02 << 4, // 010b - 18 dB, it seems 010b is a duplicate for 000b
	RxGain_23dB_2 =
		0x03 << 4, // 011b - 23 dB, it seems 011b is a duplicate for 001b
	RxGain_33dB = 0x04 << 4, // 100b - 33 dB, average, and typical default
	RxGain_38dB = 0x05 << 4, // 101b - 38 dB
	RxGain_43dB = 0x06 << 4, // 110b - 43 dB
	RxGain_48dB = 0x07 << 4, // 111b - 48 dB, maximum
	RxGain_min =
		0x00 << 4, // 000b - 18 dB, minimum, convenience for RxGain_18dB
	RxGain_avg =
		0x04 << 4,		   // 100b - 33 dB, average, convenience for RxGain_33dB
	RxGain_max = 0x07 << 4 // 111b - 48 dB, maximum, convenience for RxGain_48dB
} PCD_RxGain;

/**
 * Commands sent to the PICC.
 */
typedef enum _PICC_Command {
	// The commands used by the PCD to manage communication with several PICCs
	// (ISO 14443-3, Type A, section 6.4)
	PICC_CMD_REQA = 0x26, // REQuest command, Type A. Invites PICCs in state
						  // IDLE to go to READY and prepare for anti collision
						  // or selection. 7 bit frame.
	PICC_CMD_WUPA = 0x52, // Wake-UP command, Type A. Invites PICCs in state
						  // IDLE and HALT to go to READY(*) and prepare for
						  // anti collision or selection. 7 bit frame.
	PICC_CMD_CT = 0x88,   // Cascade Tag. Not really a command, but used during
						  // anti collision.
	PICC_CMD_SEL_CL1 = 0x93, // Anti collision/Select, Cascade Level 1
	PICC_CMD_SEL_CL2 = 0x95, // Anti collision/Select, Cascade Level 2
	PICC_CMD_SEL_CL3 = 0x97, // Anti collision/Select, Cascade Level 3
	PICC_CMD_HLTA = 0x50, // HaLT command, Type A. Instructs an ACTIVE PICC to
						  // go to state HALT.
	// The commands used for MIFARE Classic (from
	// http://www.mouser.com/ds/2/302/MF1S503x-89574.pdf, Section 9)
	// Use PCD_MFAuthent to authenticate access to a sector, then use these
	// commands to read/write/modify the blocks on the sector.
	// The read/write commands can also be used for MIFARE Ultralight.
	PICC_CMD_MF_AUTH_KEY_A = 0x60, // Perform authentication with Key A
	PICC_CMD_MF_AUTH_KEY_B = 0x61, // Perform authentication with Key B
	PICC_CMD_MF_READ = 0x30,	   // Reads one 16 byte block from the
	// authenticated sector of the PICC. Also used for
	// MIFARE Ultralight.
	PICC_CMD_MF_WRITE = 0xA0, // Writes one 16 byte block to the
							  // authenticated sector of the PICC. Called
							  // "COMPATIBILITY WRITE" for MIFARE Ultralight.
	PICC_CMD_MF_DECREMENT = 0xC0, // Decrements the contents of a block and
								  // stores the result in the internal data
								  // register.
	PICC_CMD_MF_INCREMENT = 0xC1, // Increments the contents of a block and
								  // stores the result in the internal data
								  // register.
	PICC_CMD_MF_RESTORE =
		0xC2, // Reads the contents of a block into the internal data register.
	PICC_CMD_MF_TRANSFER =
		0xB0, // Writes the contents of the internal data register to a block.
	// The commands used for MIFARE Ultralight (from
	// http://www.nxp.com/documents/data_sheet/MF0ICU1.pdf, Section 8.6)
	// The PICC_CMD_MF_READ and PICC_CMD_MF_WRITE can also be used for MIFARE
	// Ultralight.
	PICC_CMD_UL_WRITE = 0xA2 // Writes one 4 byte page to the PICC.
} PICC_Command;

// MIFARE constants that does not fit anywhere else
typedef enum _MIFARE_Misc {
	MF_ACK = 0xA, // The MIFARE Classic uses a 4 bit ACK/NAK. Any other value
	// than 0xA is NAK.
	MF_KEY_SIZE = 6 // A Mifare Crypto1 key is 6 bytes.
} MIFARE_Misc;

// PICC types we can detect. Remember to update PICC_GetTypeName() if you add
// more.
// last value set to 0xff, then compiler uses less ram, it seems some
// optimizations are triggered
typedef enum _PICC_Type {
	PICC_TYPE_UNKNOWN,
	PICC_TYPE_ISO_14443_4,		  // PICC compliant with ISO/IEC 14443-4
	PICC_TYPE_ISO_18092,		  // PICC compliant with ISO/IEC 18092 (NFC)
	PICC_TYPE_MIFARE_MINI,		  // MIFARE Classic protocol, 320 bytes
	PICC_TYPE_MIFARE_1K,		  // MIFARE Classic protocol, 1KB
	PICC_TYPE_MIFARE_4K,		  // MIFARE Classic protocol, 4KB
	PICC_TYPE_MIFARE_UL,		  // MIFARE Ultralight or Ultralight C
	PICC_TYPE_MIFARE_PLUS,		  // MIFARE Plus
	PICC_TYPE_TNP3XXX,			  // Only mentioned in NXP AN 10833 MIFARE Type
								  // Identification Procedure
	PICC_TYPE_NOT_COMPLETE = 0xff // SAK indicates UID is not complete.
} PICC_Type;

// Return codes from the functions in this class. Remember to update
// GetStatusCodeName() if you add more.
// last value set to 0xff, then compiler uses less ram, it seems some
// optimizations are triggered
typedef enum _StatusCode {
	STATUS_OK,			   // Success
	STATUS_ERROR,		   // Error in communication
	STATUS_COLLISION,	  // Collision detected
	STATUS_TIMEOUT,		   // Timeout in communication.
	STATUS_NO_ROOM,		   // A buffer is not big enough.
	STATUS_INTERNAL_ERROR, // Internal error in the code. Should not happen ;-)
	STATUS_INVALID,		   // Invalid argument.
	STATUS_CRC_WRONG,	  // The CRC_A does not match
	STATUS_MIFARE_NACK = 0xff // A MIFARE PICC responded with NAK.
} StatusCode;

// A struct used for passing the UID of a PICC.
typedef struct {
	uint8_t size; // Number of bytes in the UID. 4, 7 or 10.
	uint8_t uidByte[10];
	uint8_t sak; // The SAK (Select acknowledge) byte returned from the PICC
				 // after successful selection.
} Uid;

// A struct used for passing a MIFARE Crypto1 key
typedef struct { uint8_t keybyte[MF_KEY_SIZE]; } MIFARE_Key;

// A struct used to set GPIO pins of LPCXpresso4337 
typedef struct {
	uint8_t port;
	uint8_t pin;
} io_port_t;

// A struct used to define a MFRC522 ADT object, useful when using more than one
// PCD reader
// To understand the pin map refer to the LPC4337 schematic
// https://www.lpcware.com/system/files/LPCX4337_V3_Schematic_RevA3.pdf
// Refer to table 190: Pin multiplexing, page 405 of the manual
// http://www.nxp.com/documents/user_manual/UM10503.pdf
struct MFRC522_T {
	Uid uid; // Used by PICC_ReadCardSerial().
	// Variables used in the SSP(SPI) peripheral of the board
	LPC_SSP_T *pSSP; // Select SSP0 or SSP1
	io_port_t
		_chipSelectPin; // = {1, 8}; // As default example use GPIO1[8]= P1_5
	io_port_t
		_resetPowerDownPin; // = {3, 4}; //As default example use GPIO3[4]= P6_5
	Chip_SSP_DATA_SETUP_T data_Setup;
	uint8_t Tx_Buf[BUFFER_SIZE];
	uint8_t Rx_Buf[BUFFER_SIZE];
};

// Pointer to a MFRC5222 ADT object
typedef struct MFRC522_T *MFRC522Ptr_t;

/**
 * Function to setup a MFRC522 ADT object
 * @return an initialized  ADT object
 */
MFRC522Ptr_t MFRC522_Init();

/*******************************************************************************
* Basic interface functions for communicating with the MFRC522
*******************************************************************************/
void PCD_WriteRegister(MFRC522Ptr_t mfrc, uint8_t reg, uint8_t value);
void PCD_WriteNRegister(MFRC522Ptr_t mfrc, uint8_t reg, uint8_t count,
						uint8_t *values);
uint8_t PCD_ReadRegister(MFRC522Ptr_t mfrc, uint8_t reg);
void PCD_ReadNRegister(MFRC522Ptr_t mfrc, uint8_t reg, uint8_t count,
					   uint8_t *values, uint8_t rxAlign);
void setBitMask(unsigned char reg, unsigned char mask);
void PCD_SetRegisterBitMask(MFRC522Ptr_t mfrc, uint8_t reg, uint8_t mask);
void PCD_ClearRegisterBitMask(MFRC522Ptr_t mfrc, uint8_t reg, uint8_t mask);
StatusCode PCD_CalculateCRC(MFRC522Ptr_t mfrc, uint8_t *data, uint8_t length,
							uint8_t *result);

/*******************************************************************************
* Functions for manipulating the MFRC522
*******************************************************************************/
void PCD_Init(MFRC522Ptr_t mfrc, LPC_SSP_T *pSSP);
void PCD_Reset(MFRC522Ptr_t mfrc);
void PCD_AntennaOn(MFRC522Ptr_t mfrc);
void PCD_AntennaOff(MFRC522Ptr_t mfrc);
uint8_t PCD_GetAntennaGain(MFRC522Ptr_t mfrc);
void PCD_SetAntennaGain(MFRC522Ptr_t mfrc, uint8_t mask);

/*******************************************************************************
* Functions for communicating with PICCs
*******************************************************************************/
StatusCode PCD_TransceiveData(MFRC522Ptr_t mfrc, uint8_t *sendData,
							  uint8_t sendLen, uint8_t *backData,
							  uint8_t *backLen, uint8_t *validBits,
							  uint8_t rxAlign, bool checkCRC);
StatusCode PCD_CommunicateWithPICC(MFRC522Ptr_t mfrc, uint8_t command,
								   uint8_t waitIRq, uint8_t *sendData,
								   uint8_t sendLen, uint8_t *backData,
								   uint8_t *backLen, uint8_t *validBits,
								   uint8_t rxAlign, bool checkCRC);
StatusCode PICC_RequestA(MFRC522Ptr_t mfrc, uint8_t *bufferATQA,
						 uint8_t *bufferSize);
StatusCode PICC_WakeupA(MFRC522Ptr_t mfrc, uint8_t *bufferATQA,
						uint8_t *bufferSize);
StatusCode PICC_REQA_or_WUPA(MFRC522Ptr_t mfrc, uint8_t command,
							 uint8_t *bufferATQA, uint8_t *bufferSize);
StatusCode PICC_Select(MFRC522Ptr_t mfrc, Uid *uid, uint8_t validBits);
StatusCode PICC_HaltA(MFRC522Ptr_t mfrc);

/*******************************************************************************
*Functions for communicating with MIFARE PICCs
*******************************************************************************/
StatusCode PCD_Authenticate(MFRC522Ptr_t mfrc, uint8_t command,
							uint8_t blockAddr, MIFARE_Key *key, Uid *uid);
void PCD_StopCrypto1(MFRC522Ptr_t mfrc);
StatusCode MIFARE_Read(MFRC522Ptr_t mfrc, uint8_t blockAddr, uint8_t *buffer,
					   uint8_t *bufferSize);
StatusCode MIFARE_Write(MFRC522Ptr_t mfrc, uint8_t blockAddr, uint8_t *buffer,
						uint8_t bufferSize);
StatusCode MIFARE_Ultralight_Write(MFRC522Ptr_t mfrc, uint8_t page,
								   uint8_t *buffer, uint8_t bufferSize);
StatusCode MIFARE_Decrement(MFRC522Ptr_t mfrc, uint8_t blockAddr, long delta);
StatusCode MIFARE_Increment(MFRC522Ptr_t mfrc, uint8_t blockAddr, long delta);
StatusCode MIFARE_Restore(MFRC522Ptr_t mfrc, uint8_t blockAddr);
StatusCode MIFARE_Transfer(MFRC522Ptr_t mfrc, uint8_t blockAddr);
StatusCode MIFARE_GetValue(MFRC522Ptr_t mfrc, uint8_t blockAddr, long *value);
StatusCode MIFARE_SetValue(MFRC522Ptr_t mfrc, uint8_t blockAddr, long value);
StatusCode PCD_NTAG216_AUTH(MFRC522Ptr_t mfrc, uint8_t *passWord,
							uint8_t pACK[]);

/*******************************************************************************
* Support functions
*******************************************************************************/
StatusCode PCD_MIFARE_Transceive(MFRC522Ptr_t mfrc, uint8_t *sendData,
								 uint8_t sendLen, bool acceptTimeout);
const char *GetStatusCodeName(StatusCode code);
const char *PICC_GetTypeName(PICC_Type type);
StatusCode MIFARE_TwoStepHelper(MFRC522Ptr_t mfrc, uint8_t command,
								uint8_t blockAddr, long data);

// Support functions for debugging
void PCD_DumpVersionToSerial(MFRC522Ptr_t mfrc);
void PICC_DumpToSerial(MFRC522Ptr_t mfrc, Uid *uid);
void PICC_DumpDetailsToSerial(Uid *uid);
void PICC_DumpMifareClassicToSerial(MFRC522Ptr_t mfrc, Uid *uid,
									PICC_Type piccType, MIFARE_Key *key);
void PICC_DumpMifareClassicSectorToSerial(MFRC522Ptr_t mfrc, Uid *uid,
										  MIFARE_Key *key, uint8_t sector);
void PICC_DumpMifareUltralightToSerial(MFRC522Ptr_t mfrc);

// Advanced functions for MIFARE
void MIFARE_SetAccessBits(uint8_t *accessBitBuffer, uint8_t g0, uint8_t g1,
						  uint8_t g2, uint8_t g3);
bool MIFARE_OpenUidBackdoor(MFRC522Ptr_t mfrc, bool logErrors);
bool MIFARE_SetUid(MFRC522Ptr_t mfrc, uint8_t *newUid, uint8_t uidSize,
				   bool logErrors);
bool MIFARE_UnbrickUidSector(MFRC522Ptr_t mfrc, bool logErrors);

/*******************************************************************************
* Convenience functions - does not add extra functionality
*******************************************************************************/
bool PICC_IsNewCardPresent(MFRC522Ptr_t mfrc);
bool PICC_ReadCardSerial(MFRC522Ptr_t mfrc);

#endif
