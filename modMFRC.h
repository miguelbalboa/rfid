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

#define AUTH_KEYA       1
#define AUTH_KEYB       2
#define AUTH_DEFAULT    3


extern void setIdxTrailer(uint8_t idx);
extern void setIdxBlk(uint8_t idx);
extern void print_tag_sts(uint8_t sts);
extern void initMFRC(byte *kA, byte *kB);
extern uint8_t check_rfid(uint8_t auth, byte write, uint8_t *buf, uint8_t size);
extern uint8_t MFRC_formatCard(uint8_t auth);
extern uint8_t init_rfid(uint8_t auth);

#endif // modMFRC_h
