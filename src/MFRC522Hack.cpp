#include "MFRC522Hack.h"

/**
 * Performs the "magic sequence" needed to get Chinese UID changeable
 * Mifare cards to allow writing to sector 0, where the card UID is stored.
 *
 * Note that you do not need to have selected the card through REQA or WUPA,
 * this sequence works immediately when the card is in the reader vicinity.
 * This means you can use this method even on "bricked" cards that your reader does
 * not recognise anymore (see MFRC522Hack::MIFARE_UnbrickUidSector).
 * 
 * Of course with non-bricked devices, you're free to select them before calling this function.
 */
bool MFRC522Hack::MIFARE_OpenUidBackdoor(const bool logErrors) const {
	// Magic sequence:
	// > 50 00 57 CD (HALT + CRC)
	// > 40 (7 bits only)
	// < A (4 bits only)
	// > 43
	// < A (4 bits only)
	// Then you can write to sector 0 without authenticating
	
	_device->PICC_HaltA(); // 50 00 57 CD
	
	byte cmd = 0x40;
	byte validBits = 7; /* Our command is only 7 bits. After receiving card response,
						  this will contain amount of valid response bits. */
	byte response[32]; // Card's response is written here
	byte received;
	MFRC522::StatusCode status = _device->PCD_TransceiveData(&cmd, (byte) 1, response, &received, &validBits, (byte) 0,
														 false); // 40
	if (status != MFRC522::STATUS_OK) {
		if (logErrors) {
			Serial.println(
					F("Card did not respond to 0x40 after HALT command. Are you sure it is a UID changeable one?"));
			Serial.print(F("Error name: "));
			Serial.println(MFRC522Debug::GetStatusCodeName(status));
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
	status = _device->PCD_TransceiveData(&cmd, (byte) 1, response, &received, &validBits, (byte) 0, false); // 43
	if (status != MFRC522::STATUS_OK) {
		if (logErrors) {
			Serial.println(F("Error in communication at command 0x43, after successfully executing 0x40"));
			Serial.print(F("Error name: "));
			Serial.println(MFRC522Debug::GetStatusCodeName(status));
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
bool MFRC522Hack::MIFARE_SetUid(const byte *newUid, const byte uidSize, const bool logErrors) const {
	
	// UID + BCC byte can not be larger than 16 together
	if (!newUid || !uidSize || uidSize > 15) {
		if (logErrors) {
			Serial.println(F("New UID buffer empty, size 0, or size > 15 given"));
		}
		return false;
	}
	
	// Authenticate for reading
	MFRC522::MIFARE_Key key = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	MFRC522::StatusCode status = _device->PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, (byte) 1, &key, &(_device->uid));
	if (status != MFRC522::STATUS_OK) {
		
		if (status == MFRC522::STATUS_TIMEOUT) {
			// We get a read timeout if no card is selected yet, so let's select one
			
			// Wake the card up again if sleeping
//			  byte atqa_answer[2];
//			  byte atqa_size = 2;
//			  PICC_WakeupA(atqa_answer, &atqa_size);
			
			if (!_device->PICC_IsNewCardPresent() || !_device->PICC_ReadCardSerial()) {
				Serial.println(F("No card was previously selected, and none are available. Failed to set UID."));
				return false;
			}
			
			status = _device->PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, (byte) 1, &key, &(_device->uid));
			if (status != MFRC522::STATUS_OK) {
				// We tried, time to give up
				if (logErrors) {
					Serial.println(F("Failed to authenticate to card for reading, could not set UID: "));
					Serial.println(MFRC522Debug::GetStatusCodeName(status));
				}
				return false;
			}
		} else {
			if (logErrors) {
				Serial.print(F("PCD_Authenticate() failed: "));
				Serial.println(MFRC522Debug::GetStatusCodeName(status));
			}
			return false;
		}
	}
	
	// Read block 0
	byte block0_buffer[18];
	byte byteCount = sizeof(block0_buffer);
	status = _device->MIFARE_Read((byte) 0, block0_buffer, &byteCount);
	if (status != MFRC522::STATUS_OK) {
		if (logErrors) {
			Serial.print(F("MIFARE_Read() failed: "));
			Serial.println(MFRC522Debug::GetStatusCodeName(status));
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
	_device->PCD_StopCrypto1();
	
	// Activate UID backdoor
	if (!MIFARE_OpenUidBackdoor(logErrors)) {
		if (logErrors) {
			Serial.println(F("Activating the UID backdoor failed."));
		}
		return false;
	}
	
	// Write modified block 0 back to card
	status = _device->MIFARE_Write((byte) 0, block0_buffer, (byte) 16);
	if (status != MFRC522::STATUS_OK) {
		if (logErrors) {
			Serial.print(F("MIFARE_Write() failed: "));
			Serial.println(MFRC522Debug::GetStatusCodeName(status));
		}
		return false;
	}
	
	// Wake the card up again
	byte atqa_answer[2];
	byte atqa_size = 2;
	_device->PICC_WakeupA(atqa_answer, &atqa_size);
	
	return true;
}

/**
 * Resets entire sector 0 to zeroes, so the card can be read again by readers.
 */
bool MFRC522Hack::MIFARE_UnbrickUidSector(const bool logErrors) const {
	MIFARE_OpenUidBackdoor(logErrors);
	
	byte block0_buffer[] = {0x01, 0x02, 0x03, 0x04, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
							0x00};
	
	// Write modified block 0 back to card
	MFRC522::StatusCode status = _device->MIFARE_Write((byte) 0, block0_buffer, (byte) 16);
	if (status != MFRC522::STATUS_OK) {
		if (logErrors) {
			Serial.print(F("MIFARE_Write() failed: "));
			Serial.println(MFRC522Debug::GetStatusCodeName(status));
		}
		return false;
	}
	return true;
}
