#include "MFRC522.h"

#ifndef MFRC522Debug_h
#define MFRC522Debug_h

class MFRC522Debug {
private:
	
public:
	// Get human readable code and type
	static const __FlashStringHelper *PICC_GetTypeName(MFRC522::PICC_Type type);
	static const __FlashStringHelper *GetStatusCodeName(MFRC522::StatusCode code);
};
#endif // MFRC522Debug_h
