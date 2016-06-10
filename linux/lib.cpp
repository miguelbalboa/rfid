#include <Arduino.h>
#include <SPI.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string>
#include <fstream>
#include <iostream>

extern void setup();
extern void loop();

int main() {
  setup();
  while (1)
    loop();
}

#define SPI_DEVICE "/dev/spidev2.0"

static int debug = 0;

class Serial Serial;

void Serial::begin(int speed)
{
/* do nothing */
}

int Serial::printc(unsigned char c, int radix){
  switch (radix) {
    case 16:
      return fprintf(stdout, "%x", c);
    case 10:
      return fprintf(stdout, "%u", c);
    case 8:
      return fprintf(stdout, "%o", c);
    default:
      return fputs("bogus", stdout);
  }
}

void Serial::print(unsigned char c, int radix){
  printc(c, radix);
}

void Serial::print(const char * str){
  fputs(str, stdout);
}

void Serial::println(unsigned char c, int radix){
  print(c, radix);
  fputc('\n', stdout);
}

void Serial::println(const char * str){
  fputs(str, stdout);
  fputc('\n', stdout);
}

int Serial::readBytesUntil(char stop, char * buffer, int length)
{
  std::string input;
  int c;
  int i;
  input.resize(length);
  for(i=0; ((c = fgetc(stdin)) != stop) && i < length; i++) {
    if(c == EOF)
      break;
    input[i] = c;
  }
  return i;
}

void Serial::write(byte c)
{
  fputc('\n', stdout);
}

byte Serial::read()
{
  return fgetc(stdin);
}

void export_gpio(int gpio)
{
  std::string file("/sys/class/gpio/gpio");
  file += std::to_string(gpio);
  struct stat sb;
  int res = stat(file.c_str(), &sb);
  if(res) {
    static int last_unexported = -1;
    if (gpio != last_unexported) {
      last_unexported = gpio;
      std::cerr << "GPIO " << gpio << " is not exported.\n" <<
        "If you think it's the correct number you can export it by 'echo " <<
        gpio << " > /sys/class/gpio/export'\n";
    }
    /*
    std::string ex("/sys/class/gpio/export");
    std::ofstream s(ex);
    if (debug) {
      Serial.print(file.c_str());
      Serial.print(" ");
      Serial.println(gpio);
    }
    s << gpio;
    */
  }
}

void digitalWrite(int gpio, int state)
{
  export_gpio(gpio);
  std::string file("/sys/class/gpio/gpio");
  file += std::to_string(gpio);
  file += "/value";
  if (debug) {
    Serial.print(file.c_str());
    Serial.print(" write ");
    Serial.println(state);
  }
  std::ofstream s(file);
  s << state;
}

int digitalRead(int gpio)
{
  export_gpio(gpio);
  std::string file("/sys/class/gpio/gpio");
  file += std::to_string(gpio);
  file += "/value";
  if (debug) {
    Serial.print(file.c_str());
    Serial.print(" read ");
  }
  std::ifstream s(file);
  int value;
  s >> value;
  if (debug)
    Serial.println(value);
  return value;
}

void pinMode(int gpio, int mode)
{
  export_gpio(gpio);
  const char * direction = "out";
  if (mode)
    direction = "in";
  std::string file("/sys/class/gpio/gpio");
  file += std::to_string(gpio);
  file += "/direction";
  if (debug) {
    Serial.print(file.c_str());
    Serial.print(" ");
    Serial.println(direction);
  }
  std::ofstream s(file);
  s << direction;
}

static int spi_speed = 35000, spi_byte_order, spi_mode;

SPISettings::SPISettings(int _speed, int _byte_order, int _mode)
{
  this->speed = _speed;
  this->byte_order = _byte_order;
  this->mode = _mode;
}

class SPI SPI;
static int spi_fd = 0;

void SPI::printbuf(const char * str, const byte * buffer, size_t size)
{
  unsigned i=0, same=0, prev;

  fprintf(stderr, "%s", str);
  if (! size)
    return;
  for(i=0,same=0; i <= size; i++){
    if((i > 0) && (i < size) && (buffer[i] == prev)) {
      same ++;
    } else {
      if (same > 10)
        fprintf(stderr, " %i*%02x", same, prev);
      else {
        for(;same > 0; same--)
          fprintf(stderr, " %02x", prev);
      }
      if(i<size) {
        prev = buffer[i];
        same = 1;
      }
    }
  }
  fprintf(stderr, "\n");
}

int SPI::spi_transfern(const byte * in, byte * out, size_t size)
{
  struct spi_ioc_transfer xfer;
  int status;
  memset(&xfer, 0, sizeof(xfer));
  xfer.tx_buf = (uintptr_t) in;
  xfer.rx_buf = (uintptr_t) out;
  xfer.len = size;
  if(debug) {
    printbuf("Sending", in, size);
  }
  status = ioctl(spi_fd, SPI_IOC_MESSAGE(1), &xfer);
  if(status < 0){
    status = -errno;
    perror("xfer");
  }

  if(debug && (status > 0))
    printbuf("Received", out, size);
  return status;
}

void SPI::deselect_cs()
{
  //MFRC522.cpp does this anyway
  //digitalWrite(SS_PIN, LOW);
}

void SPI::select_cs()
{
  //digitalWrite(SS_PIN, HIGH);
}

void SPI::endTransaction(){
  deselect_cs();
}

void SPI::begin()
{
  spi_fd = open(SPI_DEVICE, O_RDWR);
}

void SPI::set_params()
{
  int bpw = 0;

  if(debug)
    fprintf(stderr, "Setting up SPI speed %iHz, mode 0x%x, lsb 0x%x, bpw 0x%x\n",
        spi_speed, spi_mode, spi_byte_order, bpw);
  if(ioctl(spi_fd, SPI_IOC_WR_MODE32, &spi_mode) < 0)
    perror("Setting SPI mode");
  if(ioctl(spi_fd, SPI_IOC_WR_LSB_FIRST, &spi_byte_order) < 0)
    perror("Setting SPI MSB first");
  if(ioctl(spi_fd, SPI_IOC_WR_BITS_PER_WORD, &bpw) < 0)
    perror("Setting SPI bits per word");
  if(ioctl(spi_fd, SPI_IOC_WR_MAX_SPEED_HZ, &spi_speed) < 0)
    perror("Setting SPI speed");
}



void SPI::beginTransaction() {
  set_params();
  select_cs();
}

void SPI::beginTransaction(SPISettings settings) {
  spi_speed = settings.speed;
  spi_mode = settings.mode;
  spi_byte_order = settings.byte_order;
  beginTransaction();
}

char SPI::transfer(byte c){
  spi_transfern(&c, &c, 1);
  return c;
}
