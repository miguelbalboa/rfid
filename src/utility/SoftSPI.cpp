
#include "Arduino.h"
#include "SoftSPI.h"


SoftSPI S_SPI;

uint8_t SoftSPI::_sckPin = 0;
uint8_t SoftSPI::_mosiPin = 0;
uint8_t SoftSPI::_misoPin = 0;

SoftSPI::SoftSPI() {
  
}

void SoftSPI::begin(uint8_t sck, uint8_t mosi, uint8_t miso)
{
  _sckPin = sck;
  _mosiPin = mosi;
  _misoPin = miso;
  pinMode(_sckPin, OUTPUT);
  pinMode(_mosiPin, OUTPUT);
  pinMode(_misoPin, INPUT);

}

void SoftSPI::end()
{
  pinMode(_sckPin, INPUT);
  pinMode(_mosiPin, INPUT);
  pinMode(_misoPin, INPUT);
}


uint8_t SoftSPI::readByte(void)
{
  uint8_t n,dat,bit_t;
  for(n = 0; n < 8; n ++)
  {
    digitalWrite(_sckPin, LOW);
	dat <<= 1;
	if(digitalRead(_misoPin))
	{
	  dat |= 0x01;
	}
	else
	{
	  dat &= 0xfe;
	}
	digitalWrite(_sckPin, HIGH);
  }
  digitalWrite(_sckPin, LOW);
  return dat;
}

uint8_t SoftSPI::transfer(uint8_t data)
{
  uint8_t i;

  for(i=0;i<8;i++)                      // output 8-bit
  {
    if(data&0x80)
    {
      digitalWrite(_mosiPin, 1);
    }
    else
    {
      digitalWrite(_mosiPin, 0);
    }
    digitalWrite(_sckPin, 1);
    data <<= 1;                         // shift next bit into MSB..
    if(digitalRead(_misoPin) == 1)
    {
      data |= 1;                         // capture current MISO bit
    }
    digitalWrite(_sckPin, 0);
  }
  return(data);                    // return read unsigned char
}

