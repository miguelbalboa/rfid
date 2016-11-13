/*
 * rfid_utils.h
 *
 *  Created on: 2016
 *      Author: Luis Fernando Guerreiro 
 *
 *
 * Auxiliary library to use MFRC522 library in the main.c file.
 * Released into the public domain. 
 *   Check UNLICENSE file in project root folder. 
 */

#ifndef INC_RFID_UTILS_H
#define INC_RFID_UTILS_H

#include "board.h"
#include "MFRC522.h"

/**
 * Setup an MFRC522_T instance and pin configurations. 
 * Tailored to LPCXpresso4337, to be used in other boards check pin configuration and SPI settings. 
 * Refer to MFRC522.h for detailed information
 * @param mfrc522 ADT pointer to an MFRC522_T instance
 */
void setupRFID(MFRC522Ptr_t* mfrc522);

/**
 * Example function to read the card  balance, the balance is stored in the 
 * block 4 (sector 1), the first 4 bytes
 * @param  mfrc522 mfrc522 MFRC522 ADT pointer
 * @return         the balance is stored in the PICC, -999 if reading errors
 */
int readCardBalance(MFRC522Ptr_t mfrc522);

/**
 * Example function to write the card balance, the balance is stored in the 
 * block 4 (sector 1), the first 4 bytes
 * @param  mfrc522    mfrc522 MFRC522 ADT pointer
 * @param  newBalance Desired balance to write to card.
 * @return            0 if no errors
 */
int writeCardBalance(MFRC522Ptr_t mfrc522, int newBalance);

#endif
