/*
    base_modMFRC.ino  created by Matteo Facchetti

		- Uses the library files MFRC522.h e MFRC522.cpp created by Miguel Balboa
		- Uses the library files modMFRC.h e modMFRC.cpp created by Matteo Facchetti

    DESCRIPTION:
    Example of 4 operation on TAG:

    - Read data block
    - Write data block
    - write trailer (set owner KeyA)
    - format card

----------------------------------------------------------------------------------------------------------------
*/

#include <Arduino.h>
#include <SPI.h>
#include "modMFRC.h"


#define TAG_READ            1       // reads the tag with the keyA set
                                    // esegue la lettura del tag con la keyA impostata

//#define TAG_FORMAT          1     // performs the formatting of the TAG accessing sectors with the keyA
                                    // esegue la formattazione del TAG accedendo ai settori con la keyA

//#define TAG_WRITE_BLOCK     1     // performs the writing of a block of the TAG
                                    // esegue la scrittura di un blocco del TAG

//#define TAG_WRITE_TRAILER   1     // performs writing a trailer of the TAG
                                    // esegue la scrittura di un trailer del TAG


uint8_t rd_tag_buf[16];     // buffer for read TAG - buffer per la lettura del TAG
uint8_t wr_tag_buf[16] = {0, 1, 2, 3, 4 ,5 ,6, 7, 8, 9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf};     // buffer for write TAG - buffer per la scrittura del TAG

uint8_t tag_sts = 0;        // TAG status - stato del TAG

// set my own key if necessary
byte ownkeyA[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; //{0xAB, 0xCD, 0xEF, 0xAB, 0xCD, 0xEF}; //

void setup()
{
    Serial.begin(9600);             // Initialize serial communications with the PC
    while(!Serial) ;                // Wait serial initialization

    SPI.begin();                    // Init SPI bus
    initMFRC(ownkeyA, 0);           // Init MFRC - set the key for replace default keyA and keyB

    // Do this every time you want to change the block/trailer to use
    // only once at initialization in this case
    setIdxBlk(1);
    setIdxTrailer(3);
}

void loop()
{
    uint32_t time = millis();
    tag_sts = 0;

    // define the appropriate labels to perform the requested operations (one at a time)
    // definire le opportune etichette per eseguire le operazioni volute (una alla volta)

#ifdef TAG_READ
    if (tag_sts = check_rfid(AUTH_KEYA, MFRC_RD, rd_tag_buf, sizeof(rd_tag_buf))) // Controllo presenza di TAG da leggere
    {
        print_tag_sts(tag_sts);
    }
#elif defined(TAG_FORMAT)
    if (tag_sts = MFRC_formatCard(AUTH_KEYA))
    {
        print_tag_sts(tag_sts);
    }
#elif defined(TAG_WRITE_BLOCK)
    if (tag_sts = check_rfid(AUTH_KEYA, MFRC_WR, wr_tag_buf, sizeof(wr_tag_buf)))
    {
        print_tag_sts(tag_sts);
    }
#elif defined(TAG_WRITE_TRAILER)
    if (tag_sts = init_rfid(AUTH_DEFAULT))
    {
        print_tag_sts(tag_sts);
    }
#endif
}
