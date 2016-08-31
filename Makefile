# Makefile for MFRC522 library
#

CXXFLAGS=-g -Wall -std=gnu++11
CPPFLAGS=-Ilinux -I.
LDFLAGS=-lstdc++

BROKEN=examples/MinimalInterrupt/MinimalInterrupt.ino examples/servo_motor/servo_motor.ino

SOURCES=$(filter-out $(BROKEN), $(wildcard examples/*/*.ino))

all:	package examples

help:
	@echo "Please use \`make <target>' where <target> is one of"
	@echo "  clean      to clean the project (e.g. remove process files)"
	@echo "  package    to package the library (into a zip file)"

clean:
	rm ./MFRC522.zip
	@echo
	@echo "Clean finished."

package:
	zip -o ./MFRC522.zip ./MFRC522.h ./MFRC522.cpp

$(patsubst %.ino,%.o,$(SOURCES)) MFRC522.o linux/lib.o:linux/Arduino.h linux/SPI.h

%.o: %.ino
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) -x c++ $< -o $@

$(patsubst %.ino,%,$(SOURCES)): linux/lib.o MFRC522.o


examples: $(patsubst %.ino,%,$(SOURCES))

