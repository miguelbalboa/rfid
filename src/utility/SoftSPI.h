#ifndef __SOFTSPI_H
#define __SOFTSPI_H

#include <SPI.h>

class SoftSPI
{
  public:
    SoftSPI();
	static void begin(uint8_t mosi, uint8_t miso, uint8_t sck);
    inline static void beginTransaction(SPISettings settings){}; // do nothing (may be disable iterrupts ?)
    inline static void endTransaction(void) {}; // do nothing (may be enable iterrupts ?)
    static void end();
	static uint8_t transfer(uint8_t dat);
	static uint8_t readByte(void);

  private:
	static uint8_t _sckPin;
	static uint8_t _mosiPin;
	static uint8_t _misoPin;
};

extern SoftSPI S_SPI;

#endif