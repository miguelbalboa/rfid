#ifndef modMFRC_h
#define modMFRC_h

#define MFRC_RD    0
#define MFRC_WR    1

// Errori
#define MFRC_NO_CARD    0xff
#define MFRC_COMM_ERROR 0xfe
#define MFRC_TYPE_ERROR 0xfd
#define MFRC_AUTH_ERROR 0xfc
#define MFRC_RD_ERROR   0xfb
#define MFRC_WR_ERROR   0xfa

// Segnalazioni
#define MFRC_NO_ERROR   0x0
#define MFRC_RD_OK      1
#define MFRC_WR_OK      2

extern void print_tag_sts(uint8_t sts);
extern void initMFRC();
extern uint8_t check_rfid(byte write, uint8_t *buf, uint8_t size);
extern uint8_t MFRC_formatCard();
extern uint8_t init_rfid();

#endif // modMFRC_h
