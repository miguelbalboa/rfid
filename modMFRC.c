/*
Developer: Matteo Facchetti
 * This file is a further development of the basic libraries MFRC522 created by Miguel Balboa.
 *
 * Uses the library files MFRC522.h e MFRC522.cpp created by Miguel Balboa
 *
 ----------------------------------------------------------------------------- Nicola Coppola
 * Pin layout should be as follows:
 * Signal     Pin              Pin               Pin
 *            Arduino Uno      Arduino Mega      MFRC522 board
 * ------------------------------------------------------------
 * Reset      9                5                 RST
 * SPI SS     10               53                SDA
 * SPI MOSI   11               51                MOSI
 * SPI MISO   12               50                MISO
 * SPI SCK    13               52                SCK
 *
  ----------------------------------------------------------------------------- Matteo Facchetti
* It provides some useful functions, appropriately commented in the code:
 *   - connect
 *   - authentication
 *   - Format single block or trailer
 *   - Format card
 *   - Read and write single block or trailer
 *   - Initializing sectors with your own key
 *
 * The reader can be found on eBay for around 5 dollars. Search for "mf-rc522" on ebay.com.
 */

#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include "modMFRC.h"

#ifdef __AVR_ATmega2560__   // MEGA board
#define SS_PIN 53
#define RST_PIN 48
#else                       // UNO board
#define SS_PIN 9
#define RST_PIN 8
#endif

MFRC522 mfrc522(SS_PIN, RST_PIN);        // Create MFRC522 instance.

// define MILFARE Classic 1K
#define MFRC_1K_NUM_SECTOR                  16
#define MFRC_1K_NUM_BLK_FOR_SECTOR          4
#define MFRC_1K_MAX_BLOCKS                  (MFRC_1K_NUM_SECTOR * MFRC_1K_NUM_BLK_FOR_SECTOR)

// define MILFARE Classic 4K
#define MFRC_4K_NUM_SECTOR                  40
#define MFRC_4K_NUM_BLK_FOR_SECTOR_0_31     4    // per i settori 0-31
#define MFRC_4K_NUM_BLK_FOR_SECTOR_32_39    16   // per i settori 32-39
#define MFRC_4K_MAX_BLOCKS                  ((32 * MFRC_4K_NUM_BLK_FOR_SECTOR_0_31) + (8 * MFRC_4K_NUM_BLK_FOR_SECTOR_32_39))

// define MILFARE Classic MINI
#define MFRC_MINI_NUM_SECTOR                5
#define MFRC_MINI_NUM_BLK_FOR_SECTOR        4
#define MFRC_MINI_MAX_BLOCKS                (MFRC_MINI_NUM_SECTOR * MFRC_MINI_NUM_BLK_FOR_SECTOR)

#define TRAILER 3

static MFRC522::MIFARE_Key keyDefault = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

static MFRC522::MIFARE_Key keyA = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
static MFRC522::MIFARE_Key keyB = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

static byte g0 = 0;    // imposta il 1° blocco dati di un settore con accesso totale con KeaA e KeyB
static byte g1 = 0;    // imposta il 2° blocco dati di un settore con accesso totale con KeaA e KeyB
static byte g2 = 0;    // imposta il 3° blocco dati di un settore con accesso totale con KeaA e KeyB
static byte g3 = 1;    // imposta il TRAILER come default di fabbrica (accesso a tutto con KeyA tranne la lettura della KeyA stessa)

static byte tmp_buf[16];        // buffer temporaneo per la lettura dei blocchi
static byte trailerBuffer[16];  // buffer per la scrittura del trailer, riempito con keyA, KeyB e gli Access Bit (g0, g1, g2, g3)

static uint8_t idx_blk;             // block number to read/write - numero del blocco da leggere/scrivere
static uint8_t idx_blk_trailer;     // trailer number - numero del trailer

/*
    Function: print_tag_sts(-)
    Description: Print the status of the tag
    Descrizione: stampa lo stato del tag
*/
void print_tag_sts(uint8_t sts)
{
    //Serial.print("Status TAG: ");
    switch(sts)
    {
    case MFRC_TYPE_ERROR:       // type TAG error - tipo TAG errato
        Serial.println("MFRC_TYPE_ERROR");
        break;
    case MFRC_AUTH_ERROR:       // authentication fail - autenticazione fallita
        Serial.println("MFRC_AUTH_ERROR");
        break;
    case MFRC_RD_OK:            // read ok - lettura ok
        Serial.println("MFRC_RD_OK");
        break;
    case MFRC_WR_OK:            // write ok - scrittura ok
        Serial.println("MFRC_WR_OK");
        break;
    case MFRC_RD_ERROR:         // read error - errore in lettura
        Serial.println("MFRC_RD_ERROR");
        break;
    case MFRC_WR_ERROR:         // write error - errore in scrittura
        Serial.println("MFRC_WR_ERROR");
        break;
    case MFRC_COMM_ERROR:       // error tag in the approach phase - errore in fase di avvicinamento
        Serial.println("MFRC_COMM_ERROR");
        break;
    case MFRC_NO_CARD:          // TAG not present - nessun TAG presente
    case MFRC_NO_ERROR:         // nothing to do - niente da fare
    default:
        break;
    }
}

/*
    Function: initMFRC(-)
    Description: Init MFRC522 card
    Descrizione: Inizializza lettore MFRC522
*/
void initMFRC(byte *kA, byte *kB)
{

    if (kA) memcpy((void *)&keyA, kA, sizeof(keyA));  // set keyA or default
    if (kB) memcpy((void *)&keyB, kB, sizeof(keyB));  // set keyB or default

    mfrc522.PCD_Init();        // Init MFRC522 card
}

/*
    Function: setIdxBlk(-)
    Description: set index block to use
    Descrizione: imposta l'indice del blocco da usare
*/
void setIdxBlk(uint8_t idx)
{
    idx_blk = idx;
}

/*
    Function: setIdxTrailer(-)
    Description: set index trailer to use
    Descrizione: imposta l'indice del trailer da usare
*/
void setIdxTrailer(uint8_t idx)
{
    idx_blk_trailer = idx;
}

/*
    Function: MFRC_close(-)
    Description: close communication
    Descrizione: chiude la comunicazione
*/
void MFRC_close()
{
    // Halt PICC
    mfrc522.PICC_HaltA();
    // Stop encryption on PCD
    mfrc522.PCD_StopCrypto1();
}

/*
    Function: MFRC_connect(-)
    Description: if TAG present, retrieves the type and the UID
    Descrizione: se TAG presente, recupera il tipo ed l'UID
*/
uint8_t MFRC_connect()
{
    // Look for new cards
    if ( ! mfrc522.PICC_IsNewCardPresent())
        return (MFRC_NO_CARD);

    // Select one of the cards
    if ( ! mfrc522.PICC_ReadCardSerial())
    {
        Serial.println("Error: PICC_ReadCardSerial");
        return(MFRC_COMM_ERROR);
    }
    // Now a card is selected. The UID and SAK is in mfrc522.uid.

    // Dump UID
    Serial.print("Card UID:");
    for (byte i = 0; i < mfrc522.uid.size; i++)
    {
        Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
        Serial.print(mfrc522.uid.uidByte[i], HEX);
    }
    Serial.println();

    // Dump PICC type
    byte piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
    Serial.print("PICC type: ");
    Serial.println(mfrc522.PICC_GetTypeName(piccType));
    if ((piccType != MFRC522::PICC_TYPE_MIFARE_MINI) &&
        (piccType != MFRC522::PICC_TYPE_MIFARE_1K) &&
        (piccType != MFRC522::PICC_TYPE_MIFARE_4K))
    {
        Serial.println("This sample only works with MIFARE Classic cards.");
        return(MFRC_TYPE_ERROR);
    }

    return(MFRC_NO_ERROR);
}

/*
    Function: MFRC_auth(-)
    Description: authenticates to the block with the specified key
    Descrizione: esegue l'autenticazione al blocco e con la chiave indicati
*/
uint8_t MFRC_auth(uint8_t blk, uint8_t type_key)
{
    byte status;
    switch (type_key)
    {
    case AUTH_KEYA:
        status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blk, &keyA, &(mfrc522.uid));
        break;
    case AUTH_KEYB:
        status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_B, blk, &keyB, &(mfrc522.uid));
        break;
    case AUTH_DEFAULT:
        status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blk, &keyDefault, &(mfrc522.uid));
        break;
    default:
        Serial.print("Auth TYPE KEY ERROR->");
        Serial.println(type_key);
        return (MFRC_AUTH_ERROR);
    }

    if (status != MFRC522::STATUS_OK)
    {
        Serial.print("Auth failed: ");
        Serial.println(mfrc522.GetStatusCodeName(status));
        return(MFRC_AUTH_ERROR);
    }
    return(MFRC_NO_ERROR);
}

/*
    Function: read_rfid(-)
    Description: reads the block and copies the data to the buffer
    Descrizione: esegue la lettura del blocco e copia i dati in buffer
*/
uint8_t read_rfid(uint8_t blk, uint8_t* buffer, uint8_t size)
{
    Serial.print("Read block ");
    Serial.println(blk);
    uint8_t rd_buf[18];
    uint8_t sz = sizeof(rd_buf);
    memset(rd_buf, 0, sz);
    // change this: valueBlockA , for read anather block
    // cambiate valueBlockA per leggere un altro blocco
    byte status = mfrc522.MIFARE_Read(blk, rd_buf, &sz);
    if (status != MFRC522::STATUS_OK)
    {
        Serial.print("MIFARE_Read() failed: ");
        Serial.println(mfrc522.GetStatusCodeName(status));
        return (MFRC_RD_ERROR); // Errore!
    }

    memset(buffer, 0, size);
    memcpy(buffer, rd_buf, size);

    int i = 0;
    Serial.print("Byte[");
    while (i < size)
    {
        Serial.print(buffer[i++], HEX);
        if (i < (size)) Serial.print(" ");
    }
    Serial.println("]");

    return (MFRC_NO_ERROR); // Tutto ok!
}

/*
    Function: read_rfid(-)
    Description: writes to the block with the datas in buffer with eventual rereading
    Descrizione: esegue la scrittura del blocco dei dati contenuti in buffer con eventuale rilettura
*/
uint8_t write_rfid(uint8_t blk, byte *data, uint8_t size, uint8_t chk)
{
    byte status;

    Serial.print("Writing new value block ");
    Serial.println(blk);
    status = mfrc522.MIFARE_Write(blk, data, size);

    if (status != MFRC522::STATUS_OK)
    {
        Serial.print("MIFARE_Write() failed: ");
        Serial.println(mfrc522.GetStatusCodeName(status));
        return (MFRC_WR_ERROR); // Error!
    }

    if (chk)
    {
       // control the data just written - controllo i dati appena scritti
        if (read_rfid(blk, tmp_buf, sizeof(tmp_buf)))
            return (MFRC_RD_ERROR); // Errore!

        if (memcmp(tmp_buf, data, size))
        {
            //  scrittura Fallita
            Serial.print("Write/Read block ");
            Serial.print(blk);
            Serial.println(": error!");
            return (MFRC_WR_ERROR); // Error!
        }
    }

    Serial.print("Write block ");
    Serial.print(blk);
    Serial.println(": success");
    return (MFRC_NO_ERROR); // Tutto ok senza rilettura
}

/*
    Function: MFRC_formatBlock(-)
    Description: writes to the block with the datas in buffer with eventual rereading
    Descrizione: esegue la formattazione di un blocco
*/
uint8_t MFRC_formatBlock(byte blk)
{
	Serial.print("Reading block "); Serial.println(blk);
	uint8_t err = 0;
#if 0
    // setup a block on a MIFARE Classic PICC to be in "Value Block" mode.
    // In Value Block mode the operations Increment/Decrement/Restore and Transfer can be used.
	if(err = read_rfid(blk, tmp_buf, sizeof(tmp_buf)))
        return(err);

	if ((tmp_buf[0] == (byte)~tmp_buf[4])   &&
		(tmp_buf[1] == (byte)~tmp_buf[5])   &&
		(tmp_buf[2] == (byte)~tmp_buf[6])   &&
		(tmp_buf[3] == (byte)~tmp_buf[7])
		                                  &&
		(tmp_buf[0] == tmp_buf[8])          &&
		(tmp_buf[1] == tmp_buf[9])          &&
		(tmp_buf[2] == tmp_buf[10])         &&
		(tmp_buf[3] == tmp_buf[11])
		                                  &&
		(tmp_buf[12] == (byte)~tmp_buf[13]) &&
		(tmp_buf[12] ==        tmp_buf[14]) &&
		(tmp_buf[12] == (byte)~tmp_buf[15]))
    {
		Serial.println("Block has correct Block Value format.");
	}
	else
    {
		byte valueBlock[] = {0,0,0,0,  255,255,255,255,  0,0,0,0, blk,~blk,blk,~blk};
#else
    {
        // I want to reset the block - io voglio azzerare il blocco
		byte valueBlock[] = {0,0,0,0,  0,0,0,0,  0,0,0,0, 0,0,0,0};
#endif
		if (err = write_rfid(blk, valueBlock, sizeof(valueBlock), 1))
			return(err);
	}
	return (MFRC_NO_ERROR);
} // End formatBlock()


/*
    Function: MFRC_setTrailer(-)
    Description: writes the trailer indicated
    Descrizione: scrive il trailer indicato
*/
uint8_t MFRC_setTrailer(byte blk)
{
    uint8_t err = 0;
    Serial.print("Set sector trailer: ");
    Serial.println(blk);

    // I copy the new keyA to be written in the trailer - copio la nuova chiave A da scrivere nel trailer
    memcpy(&trailerBuffer[0], (void *)&keyA, sizeof(keyA));
    // I set the access with the only keyA in read/write -
    // Accesso con la sola KeyA in lettura/scrittura
    mfrc522.MIFARE_SetAccessBits(&trailerBuffer[6], g0, g1, g2, g3);
    memcpy(&trailerBuffer[10], (void *)&keyB, sizeof(keyB));

    // Write TRAILER - Scrive il TRAILER
    if (err = write_rfid(blk, trailerBuffer, sizeof(trailerBuffer), 0))
            return(err);
}

/*
    Function: MFRC_formatTrailer(-)
    Description: writes the trailer indicated
    Descrizione: scrive il trailer indicato
*/
uint8_t MFRC_formatTrailer(byte blk)
{
    // Access bit già calcolati di default su MILFARE Classic 1K:
    /*
    (0xff, 0x07, 0x80, 0x0) ovvero g0=0,g1=0,g2=0,g3=1 (Access trailer with only KEYA)
    (0x7f, 0x07, 0x88, 0x0) ovvero g0=0,g1=0,g2=0,g3=3 (Access trailer with KEYA-KEYB)
    */
    uint8_t err = 0;
    Serial.print("Format sector trailer: ");
    Serial.println(blk);

    memcpy(&trailerBuffer[0], (void *)&keyDefault, sizeof(keyDefault));
    // I set the access with the only keyA in read/write -
    // Accesso con la sola KeyA in lettura/scrittura
    mfrc522.MIFARE_SetAccessBits(&trailerBuffer[6], g0, g1, g2, g3);
    memcpy(&trailerBuffer[10], (void *)&keyDefault, sizeof(keyDefault));

    /*
    void MFRC522::MIFARE_SetAccessBits(	byte *accessBitBuffer,	///< Pointer to byte 6, 7 and 8 in the sector trailer. Bytes [0..2] will be set.
                                    byte g0,				///< Access bits [C1 C2 C3] for block 0 (for sectors 0-31) or blocks 0-4 (for sectors 32-39)
                                    byte g1,				///< Access bits C1 C2 C3] for block 1 (for sectors 0-31) or blocks 5-9 (for sectors 32-39)
                                    byte g2,				///< Access bits C1 C2 C3] for block 2 (for sectors 0-31) or blocks 10-14 (for sectors 32-39)
                                    byte g3					///< Access bits C1 C2 C3] for the sector trailer, block 3 (for sectors 0-31) or block 15 (for sectors 32-39)
                                  )
    */
    if(err = read_rfid(blk, tmp_buf, sizeof(tmp_buf)))
        return(err);

    if (memcmp(&tmp_buf[6], &trailerBuffer[6], 4)) // Controllo corretto valore degli Access Bit
    {
        if (err = write_rfid(blk, trailerBuffer, sizeof(trailerBuffer), 0))
            return(err);
    }
    return (MFRC_NO_ERROR);
}

/*
    Function: MFRC_formatCard(-)
    Description: Format card
    Descrizione: Formatta la card (TAG)
*/
uint8_t MFRC_formatCard(uint8_t auth)
{
    uint8_t blk, max_blk, nblk_for_sect, err = 0;
    byte piccType;

    // connect to TAG - connessione al TAG
    if (err = MFRC_connect())
    {
        MFRC_close();
        return (err); // Error
    }

    Serial.println("Format CARD");

    // get max number of blocks and number of blocks for sector -
    // ottiene il numero massimo di blocchi e il numero di blocchi per settore
    piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
    switch(piccType)
    {
    case MFRC522::PICC_TYPE_MIFARE_MINI:
        max_blk = MFRC_MINI_MAX_BLOCKS;
        nblk_for_sect = MFRC_MINI_NUM_BLK_FOR_SECTOR;
        break;
    case MFRC522::PICC_TYPE_MIFARE_1K:
        max_blk = MFRC_1K_MAX_BLOCKS;
        nblk_for_sect = MFRC_1K_NUM_BLK_FOR_SECTOR;
        break;
    case MFRC522::PICC_TYPE_MIFARE_4K:
        max_blk = MFRC_4K_MAX_BLOCKS;
        nblk_for_sect = MFRC_4K_NUM_BLK_FOR_SECTOR_0_31;
        break;
    default:
        Serial.print("Tipo Card non riconosciuta:");
        Serial.println(piccType);
        MFRC_close();
        return(MFRC_TYPE_ERROR);
    }

    // card formatting - formattazione CARD
    for (blk = 1; blk < max_blk; blk++) // first block is protected
    {
        if(!(err = MFRC_auth(blk, auth))) // need to know key for access
        {
            // specialization for MIFARE 4K - specializzazione per MIFARE 4K
            if((piccType == MFRC522::PICC_TYPE_MIFARE_4K) && (blk > 31)) //
                nblk_for_sect = MFRC_4K_NUM_BLK_FOR_SECTOR_32_39;

            if (((blk + 1) % nblk_for_sect) == 0) // trailer or block?
                err = MFRC_formatTrailer(blk);  // tariler format - formattazione trailer
            else
                err = MFRC_formatBlock(blk);    // block format - formattazione blocco
        }
        if (err) break; // Stop if error - si ferma se c'è un errore
    }
    MFRC_close();
    return (err ? err : MFRC_WR_OK);
}

/*
    Function: check_rfid(-)
    Description: Check the presence of a TAG and performs a read or write
    Descrizione: Controlla la presenza di un TAG ed esegue lettura o scrittura
*/
uint8_t check_rfid(uint8_t auth, byte write, uint8_t *buf, uint8_t size)
{
    uint8_t err = 0;

    if ((err = MFRC_connect()) ||       // 1 connect - Break if error
        (err = MFRC_auth(idx_blk, auth)) ||    // 2 auth - Break if error
        ((write) && (err = write_rfid(idx_blk, buf, size, 1))) ||   // 3 write or read
        ((!write) && (err = read_rfid(idx_blk, buf, size))))        //
    {
        MFRC_close();
        return (err); // Errore
    }

    MFRC_close();
    return (write ? MFRC_WR_OK : MFRC_RD_OK); // Tutto ok! Write o Read eseguito correttamente
}

/*
    Function: init_rfid(-)
    Description: Initializes a sector by setting the trailer (and the key bit access)
                 Set owner keyA with default keyA access
    Descrizione: Inizializza un settore impostando il trailer (chiave e bit di accesso)
*/
uint8_t init_rfid(uint8_t auth)
{
    uint8_t err = 0;

    if ((err = MFRC_connect()) ||       // 1 connect - Break if error
        //(err = MFRC_auth(idx_blk, AUTH_DEFAULT)) ||
        //(err = MFRC_formatBlock(idx_blk))||
        (err = MFRC_auth(idx_blk_trailer, auth)) ||    // 2 auth - Break if error
        (err = MFRC_setTrailer(idx_blk_trailer)))        // 3 - Set trailer with owner keyA
    {
        MFRC_close();
        return (err); // Errore
    }
    MFRC_close();
    return (MFRC_WR_OK); // Tutto ok! Write o Read eseguito correttamente
}
