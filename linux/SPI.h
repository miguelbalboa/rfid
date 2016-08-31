#ifndef SPI_H
#define SPI_H

#include <Arduino.h>

#include <linux/spi/spidev.h>

#define SPI_CLOCK_DIV4 1000000 /*some arbitrary value*/
#define MSBFIRST 0
#define LSBFIRST SPI_LSB_FIRST
#define SPI_MODE0 SPI_MODE_0
#define SPI_MODE1 SPI_MODE_1
#define SPI_MODE2 SPI_MODE_2
#define SPI_MODE3 SPI_MODE_3

class SPISettings {
  public:
    int speed;
    int byte_order;
    int mode;
    SPISettings(int speed, int byte_order, int mode);
};

class SPI {
  static void printbuf(const char * str, const byte * buffer, size_t size);
  static int spi_transfern(const byte * in, byte * out, size_t size);
  static void deselect_cs();
  static void select_cs();
  static void set_params();

  public:
  static void begin();
  static void endTransaction();
  static void beginTransaction();
  static void beginTransaction(SPISettings settings);
  static char transfer(byte c);

} extern SPI;

#endif /*SPI_H*/
