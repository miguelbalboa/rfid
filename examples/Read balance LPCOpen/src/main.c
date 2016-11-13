/**
 * 	main.c
 *  Created on: 2016
 *  Author: Luis Fernando Guerreiro
 *    
 * 	Example main.c file to be used with MFRC522 C library compatible with 
 * 	LPCOpen CMSIS  C libraries.
 *	Released into the public domain. 
 *	 Check UNLICENSE file in project root folder.
 */
/*******************************************************************************
 * LPCXpresso4337 project includes
 ******************************************************************************/
#include "board.h"
#include "chip.h"
#include <cr_section_macros.h>
/*******************************************************************************
 * Custom files and libraries includes
 ******************************************************************************/
#include "delay.h"
#include "MFRC522.h"
#include "rfid_utils.h"
#include "MFRC522.h"

#define MIN_BALANCE 300
/*******************************************************************************
 *  Extra functions defined in the main.c file
 ******************************************************************************/
void userTapIn();

/*******************************************************************************
 * Private types/enumerations/variables
 ******************************************************************************/
int last_balance = 0;
unsigned int last_user_ID;

/*******************************************************************************
 * Public types/enumerations/variables
 ******************************************************************************/

// RFID structs
MFRC522Ptr_t mfrcInstance;


/**
 * @brief	Main program body
 * @return	Does not return
 */
int main(void) {
	// Read clock settings and update SystemCoreClock variable
	SystemCoreClockUpdate();

	// To use the delay library and programCounter
	SysTick_Init();
	resetProgramCounter();

	/* Board_Init calls Chip_GPIO_Init and enables GPIO clock if needed,
	   Chip_GPIO_Init is not called again */
	Board_Init();
	Board_LED_Set(0, true);

	setupRFID(&mfrcInstance);

	while (1) {

		// Look for new cards in RFID2
		if (PICC_IsNewCardPresent(mfrcInstance)) {
			// Select one of the cards
			if (PICC_ReadCardSerial(mfrcInstance)) {
//				int status = writeCardBalance(mfrcInstance, 100000); // used to recharge the card
				 userTapIn();
			}
		}

		__WFI();
	}
}

/*******************************************************************************
 *  System routine functions
 ******************************************************************************/

/**
 * Executed every time the card reader detects a user in
 */
void userTapIn() {

//	show card UID
	DEBUGOUT("\nCard uid bytes: ");
	for (uint8_t i = 0; i < mfrcInstance->uid.size; i++) {
		DEBUGOUT(" %X", mfrcInstance->uid.uidByte[i]);
	}
	DEBUGOUT("\n\r");


	// Convert the uid bytes to an integer, byte[0] is the MSB
	last_user_ID =
		(int)mfrcInstance->uid.uidByte[3] |
		(int)mfrcInstance->uid.uidByte[2] << 8 |
		(int)mfrcInstance->uid.uidByte[1] << 16 |
		(int)mfrcInstance->uid.uidByte[0] << 24;

	DEBUGOUT("\nCard Read user ID: %u ",last_user_ID);


		// Read the user balance
		last_balance = readCardBalance(mfrcInstance);

		if (last_balance == (-999)) {
			// Error handling, the card does not have proper balance data inside
		} else {
			// Check for minimum balance
			if (last_balance < MIN_BALANCE) {
				DEBUGOUT(", Insufficient balance\n");
			} else {
				DEBUGOUT(", Balance OK\n");
			}
		}
}

