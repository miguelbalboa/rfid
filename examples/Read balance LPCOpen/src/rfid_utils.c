/*
* rfid_utils.c 
* NOTE: Please also check the comments in rfid_utils.h 
* they provide useful hints and background information.
*/
#include "rfid_utils.h"

/****************************************
 * Private variables
 ****************************************/

// auxBuffer used to store the read end write data, each block have 16 bytes,
// auxBuffer must have 18 slots, see MIFARE_Read()
static uint8_t auxBuffer[18];
static uint8_t size = sizeof(auxBuffer);

// return status from MFRC522 functions
static StatusCode status;


/****************************************
 * Private Functions
 ****************************************/

/**
 * Read 16 bytes from a block inside a sector
 * @param  mfrc522   MFRC522 ADT pointer
 * @param  sector    card sector, 0 to 15
 * @param  blockAddr card block address, 0 to 63
 * @return           0 is no error, -1 is authentication error -2 is read error
 */
static int readCardBlock(MFRC522Ptr_t mfrc522, uint8_t sector,
						 uint8_t blockAddr) {

	MIFARE_Key key;
	int i;
	// using FFFFFFFFFFFFh which is the default at chip delivery from the
	// factory
	for (i = 0; i < 6; i++) {
		key.keybyte[i] = 0xFF;
	}

	// Authenticate using key A
	status = (StatusCode)PCD_Authenticate(mfrc522, PICC_CMD_MF_AUTH_KEY_A,
										  blockAddr, &key, &(mfrc522->uid));
	if (status != STATUS_OK) {
		DEBUGOUT("PCD_Authenticate() failed: ");
		DEBUGOUT(GetStatusCodeName(status));
		return -1;
	}

	// Read data from the block
	status = (StatusCode)MIFARE_Read(mfrc522, blockAddr, auxBuffer, &size);
	if (status != STATUS_OK) {
		DEBUGOUT("MIFARE_Read() failed: ");
		DEBUGOUT(GetStatusCodeName(status));
		return -2;
	}

	// Halt PICC
    PICC_HaltA(mfrc522);
    // Stop encryption on PCD
    PCD_StopCrypto1(mfrc522);

	return 0;
}

/**
 * Write 16 bytes to a block inside a sector
 * @param  mfrc522   MFRC522 ADT pointer
 * @param  sector    card sector, 0 to 15
 * @param  blockAddr card block address, 0 to 63
 * @return           0 is no error, -1 is authentication error -2 is write error
 */
static int writeCardBlock(MFRC522Ptr_t mfrc522, uint8_t sector,
						  uint8_t blockAddr) {

	MIFARE_Key key;
	int i;
	// using FFFFFFFFFFFF which is the default at chip delivery from the factory
	for (i = 0; i < 6; i++) {
		key.keybyte[i] = 0xFF;
	}

	// Authenticate using key A
	status = (StatusCode)PCD_Authenticate(mfrc522, PICC_CMD_MF_AUTH_KEY_A,
										  blockAddr, &key, &(mfrc522->uid));
	if (status != STATUS_OK) {
		DEBUGOUT("PCD_Authenticate() failed: ");
		DEBUGOUT(GetStatusCodeName(status));
		return -1;
	}

	// Write data from the block, always write 16 bytes
	status = (StatusCode)MIFARE_Write(mfrc522, blockAddr, auxBuffer, 16);
	if (status != STATUS_OK) {
		DEBUGOUT("MIFARE_Write() failed: ");
		DEBUGOUT(GetStatusCodeName(status));
		return -2;
	}

	// Halt PICC
    PICC_HaltA(mfrc522);
    // Stop encryption on PCD
    PCD_StopCrypto1(mfrc522);

	return 0;
}

/****************************************
 * Public Functions
 ****************************************/


void setupRFID(MFRC522Ptr_t* mfrc522) {
	*mfrc522 = MFRC522_Init();
	// Define the pins to use as CS(SS or SSEL) and RST
	// // Set pins as// GPIO
	Chip_SCU_PinMuxSet(0x1, 0, (SCU_PINIO_FAST | SCU_MODE_FUNC0)); 
	Chip_SCU_PinMuxSet(0x5, 2,(SCU_PINIO_FAST | SCU_MODE_FUNC0));

	// GPIO1[0]= P1_07
	(*mfrc522)->_chipSelectPin.port = 1;
	(*mfrc522)->_chipSelectPin.pin = 0;

	// GPIO5[2]
	(*mfrc522)->_resetPowerDownPin.port = 5;
	(*mfrc522)->_resetPowerDownPin.pin = 2;

	PCD_Init(*mfrc522, LPC_SSP0);
	DEBUGOUT("\nReader 2 ");
	
	// Show details of PCD - MFRC522 Card Reader details
	PCD_DumpVersionToSerial(*mfrc522); 
}

/**
 * Function to read the balance, the balance is stored in the block 4 (sector
 * 1), the first 4 bytes
 * @param  mfrc522 MFRC522 ADT pointer
 * @return         the balance is stored in the PICC, -999 for reading errors
 */
int readCardBalance(MFRC522Ptr_t mfrc522) {

	int balance;

	int readStatus = readCardBlock(mfrc522, 1, 4);

	if (readStatus == 0) {
		// convert the balance bytes to an integer, byte[0] is the MSB
		balance = (int)auxBuffer[3] | (int)(auxBuffer[2] << 8) |
				  (int)(auxBuffer[1] << 16) | (int)(auxBuffer[0] << 24);
	} else {
		balance = -999;
	}

	return balance;
}

/**
 * Function to rwrite the balance, the balance is stored in the block 4 (sector
 * 1), the first 4 bytes
 * @param  mfrc522    MFRC522 ADT pointer
 * @param  newBalance the balance to be write  i  the card
 * @return            0 is no errors
 */
int writeCardBalance(MFRC522Ptr_t mfrc522, int newBalance) {

	// set the 16 bytes auxBuffer, auxBuffer[0] is the MSB
	auxBuffer[0] = newBalance >> 24;
	auxBuffer[1] = newBalance >> 16;
	auxBuffer[2] = newBalance >> 8;
	auxBuffer[3] = newBalance & 0x000000FF;
	int i;
	for (i = 4; i < size; i++) {
		auxBuffer[i] = 0xBB;
	}

	int writeStatus = writeCardBlock(mfrc522, 1, 4);

	return writeStatus;
}
