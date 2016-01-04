/*
Kiara Navarro
sophiekovalevsky arroba fedoraproject.org
www.panamahitek.com
*/

/* 
 Pinaje (MFRC522 hacia Arduino)
 MFRC522 pin SDA hacia el pin 10
 MFRC522 pin SCK hacia el pin 13
 MFRC522 pin MOSI hacia el pin 11
 MFRC522 pin MISO hacia el pin 12
 MFRC522 pin GND a tierra
 MFRC522 pin RST hacia el pin 9
 MFRC522 pin 3.3V A 3.3. V
*/

// Librerías necesarias
#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10 // pin SDA hacia el pin 10
#define RST_PIN 9 // pin RST hacia el pin 9
 
MFRC522 rfid(SS_PIN, RST_PIN); // Creo la instancia de la clase MFRC522

MFRC522::MIFARE_Key key; // Crea una estructura de tipo llave

// Inicializo vector que almacenará el NUID del PICC
byte nuidPICC[3];

void setup() { 
	Serial.begin(9600);
	SPI.begin(); // Inicia el bus de SPI
	rfid.PCD_Init(); // Inicia el lector 

	// Carga los valores que vienen por defecto para las llaves A y B
	for (byte i = 0; i < 6; i++) {
	    key.keyByte[i] = 0xFF;
	}

	Serial.println(F("Este código escanea el número de identificación de una tarjeta MIFARE Classic."));
	Serial.print(F("Utilizando la siguiente llave:"));
	printHex(key.keyByte, MFRC522::MF_KEY_SIZE);
}
 
void loop() {

    // Buscando nuevas tarjetas
    if ( ! rfid.PICC_IsNewCardPresent())
        return;

    // Lee tarjeta
    if ( ! rfid.PICC_ReadCardSerial())
        return;

    Serial.print(F("Tipo de PICC: "));
    byte piccType = rfid.PICC_GetType(rfid.uid.sak);
    Serial.println(rfid.PICC_GetTypeName(piccType));

    // Verifica si la tarjeta es de tipo MIFARE Classic
    if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&  
    	piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
    	piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
        Serial.println(F("Tu tarjeta no es del tipo MIFARE Classic"));
        return;
    }

    if (rfid.uid.uidByte[0] != nuidPICC[0] || 
	    rfid.uid.uidByte[1] != nuidPICC[1] || 
	    rfid.uid.uidByte[2] != nuidPICC[2] || 
	    rfid.uid.uidByte[3] != nuidPICC[3] ) {
		Serial.println(F("Se ha detectado nueva tarjeta"));

		// Almacena el NUID en el arreglo nuidPICC
		for (byte i = 0; i < 4; i++) {
			nuidPICC[i] = rfid.uid.uidByte[i];
		}
	 
	    Serial.println(F("Su número no único de identificación es:"));
	    Serial.print(F("En hexadecimal: "));
	    printHex(rfid.uid.uidByte, rfid.uid.size);
	    Serial.println();
	    Serial.print(F("En decimal: "));
	    printDec(rfid.uid.uidByte, rfid.uid.size);
	    Serial.println();
    }
    else Serial.println("Tarjeta leída anteriormente, ingrese otra.");

    // Halt PICC
    rfid.PICC_HaltA();

    // Detiene el cifrado en el PCD
    rfid.PCD_StopCrypto1();
}


/**
 * Función que ayuda a representar valores hex en el monitor serial
 */
void printHex(byte *buffer, byte bufferSize) {
    for (byte i = 0; i < bufferSize; i++) {
        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], HEX);
    }
}

/**
 * Función que ayuda a representar valores en dec en el monitor serial
 */
void printDec(byte *buffer, byte bufferSize) {
    for (byte i = 0; i < bufferSize; i++) {
        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], DEC);
    }
}