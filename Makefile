# Makefile for MFRC522 library
#

all:	package

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
