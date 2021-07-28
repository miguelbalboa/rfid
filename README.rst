rc522-ultralight-c
=======

RC522 with ESP32 and support for MiFare Ultralight C.

Warning
-------

This library has only been tested with ESP32. It might not work on other boards (or at least there will be adaptions to be made).

The 3DES library used to authenticate agains the Ultralight C PICC is Mbed TLS (see https://github.com/ARMmbed/mbedtls). Setup for other platforms than ESP32 might be different. ESP-SDK comes packaged with some Mbed TLS functionality, but DES is disabled for security reasons. The easiest way to provide it is just packaging it with the repository.

Original Readme
---------------

Original Readme see: https://github.com/miguelbalboa/rfid/blob/master/README.rst

Arduino library for MFRC522 and other RFID RC522 based modules.

Read and write different types of Radio-Frequency IDentification (RFID) cards
on your Arduino using a RC522 based reader connected via the Serial Peripheral
Interface (SPI) interface.


.. _license:
License
-------
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to https://unlicense.org/

License Mbed TLS
----------------
The following applies for the files src/des.h and src/des.c. They are unmodified.

Unless specifically indicated otherwise in a file, Mbed TLS files are provided under the Apache-2.0 license. See the LICENSE file at https://github.com/ARMmbed/mbedtls/blob/development/LICENSE for the full text of this license. Contributors must accept that their contributions are made under both the Apache-2.0 AND GPL-2.0-or-later licenses. This enables LTS (Long Term Support) branches of the software to be provided under either the Apache-2.0 OR GPL-2.0-or-later licenses.

History
-------

The MFRC522 library was first created in Jan 2012 by Miguel Balboa (from
http://circuitito.com) based on code by Dr. Leong (from http://B2CQSHOP.com)
for *"Arduino RFID module Kit 13.56 Mhz with Tags SPI W and R By COOQRobot"*.

It was translated into English and rewritten/refactored in the fall of 2013
by Søren Thing Andersen (from http://access.thing.dk).

It has been extended with functionality to alter sector 0 on Chinese UID changeable MIFARE card in Oct 2014 by Tom Clement (from http://tomclement.nl).

Maintained by miguelbalboa until 2016.
Maintained by Rotzbua from 2016 until 2018.


.. _arduino: https://arduino.cc/
.. _ebay: https://www.ebay.com/
.. _iso/iec 14443a: https://en.wikipedia.org/wiki/ISO/IEC_14443
.. _iso/iec 14443-3\:2011 part 3: 
.. _nxp mfrc522: https://www.nxp.com/documents/data_sheet/MFRC522.pdf
.. _broken: https://eprint.iacr.org/2008/166
.. _supported by hardware: https://web.archive.org/web/20151210045625/http://www.nxp.com/documents/leaflet/939775017564.pdf
.. _Arduino forum: https://forum.arduino.cc
.. _stdint.h: https://en.wikibooks.org/wiki/C_Programming/C_Reference/stdint.h
.. _Mikro Elektronika: https://forum.mikroe.com/viewtopic.php?f=147&t=64203
