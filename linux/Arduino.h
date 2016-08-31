#ifndef ARDUINO_H
#define ARDUINO_H

#include <string.h>
#include <unistd.h>
#include <inttypes.h>

#define LOW 0
#define HIGH 1
#define OUTPUT 0
#define INPUT 2

#define HEX 16
#define OCT 8
#define DEC 10

#define byte uint8_t
#define word uint32_t
#define boolean bool

#define delay(ms) usleep((ms)*1000)

#define F(x) (x)
#define PROGMEM
#define pgm_read_byte(x) (*(x))
#define __FlashStringHelper char

class Serial {
  static int printc(unsigned char c, int radix);

  public:
  static inline void setTimeout(long){ /*FIXME*/}
  static void begin(int speed);
  static void print(unsigned char c, int radix = DEC);
  static void print(const char * str);
  static void println(unsigned char c, int radix = DEC);
  static void println(const char * str = "");
  static int readBytesUntil(char stop, char * buffer, int length);
  static byte read();
  static void write(byte);
};
inline bool operator !(class Serial arg) { return 0; }

extern Serial Serial;
static inline void printData(const char * data, int length, int base)
{
  for(int i=0; i<length; i++){
    if(i) Serial.print(" ");
    if(data[i]<base) Serial.print("0");
    Serial.print(data[i],base);
  }
  Serial.println();
}

void digitalWrite(int gpio, int state);
int digitalRead(int gpio);
void pinMode(int gpio, int mode);

#endif /*ARDUINO_H*/
