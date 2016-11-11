MFRC522_PLCOpen
=======


LPCOpen compatible C library for MFRC522 and other RFID RC522 based modules.
It is useful when you have to do a plain C project. 
Based on the MFRC522 Arduino library


.. _development:
Development
----------
**This library was used for a project using NXP LPCXpresso 4337 board**. But it is compatible to any devices that uses LPCOpen CMSIS libraries. The user only have to configure the board GPIO pins and SPI interface. I will provide only one release, further development could be done by community. 


.. _what works and not:
What works and not?
----------

I ported all the functions to plain C code, and transformed the object initialization functions to C using ADT pointers. In my project I was able to:
 
  #. Communication (Crypto1) with MIFARE Classic (1k, 4k, Mini). The only type of card tested. 
  #. Read and write in the cards available memory space.
  #. Multiple readers instances, one per SPI interface.

* All the other functions should be working, but need further testing.

.. _compatible ide:
Compatible IDE
----------
This library works with the Eclipse based LPCXpresso IDE 8.2 and further versions.

Download at:
http://www.nxp.com/products/software-and-tools/software-development-tools/software-tools/lpc-microcontroller-utilities/lpcxpresso-ide-v8.2.2:LPCXPRESSO


.. _compatible boards:
Compatible boards
----------
In my project I used th LPCXpresso 4337 board. But any NXP board with SPI interface and LPCOpen support should work.

See LPCOPen examples at:
http://www.nxp.com/products/microcontrollers-and-processors/arm-processors/lpc-cortex-m-mcus/software-tools/lpcopen-libraries-and-examples:LPC-OPEN-LIBRARIES


.. _pin layout:
Pin Layout
----------

The following table shows the pin layout used in the project for the board LPCXpresso 4337:

+-----------+----------+-------------+---------------------------+
|           | PCD      | Arduino     | LPCXpresso 4337           |
|           +----------+-------------+--------------+------------+
|           | MFRC522  | Uno         | using SSP1   | using SSP0 |
+-----------+----------+-------------+--------------+------------+
| Signal    | Pin      | Pin         | Pin          | Pin        | 
+===========+==========+=============+==============+============+
| RST/Reset | RST      | 9           | P2_9 [1]_    | P2_13 [1]_ | 
+-----------+----------+-------------+--------------+------------+
| SPI SS    | SDA [2]_ | 10          | P2_12 [1]_   | P1_7 [1]_  | 
+-----------+----------+-------------+--------------+------------+
| SPI MOSI  | MOSI     | 11          | P1_4         | P1_2       |
+-----------+----------+-------------+--------------+------------+
| SPI MISO  | MISO     | 12          | P1_3         | P1_1       |
+-----------+----------+-------------+--------------+------------+
| SPI SCK   | SCK      | 13          | PF_4         | P3_0       |
+-----------+----------+-------------+--------------+------------+

.. [1] You can use any GPIO pin to drive the RST and SS signals.

.. [2] The SDA pin might be labeled SS on some/older MFRC522 boards. 

* Check schematics at : https://www.lpcware.com/system/files/LPCX4337_V3_Schematic_RevA3.pdf
* Also check two images that resume the board pin layout:
    https://developer.mbed.org/media/uploads/MACRUM/xlpcxpresso4337_arduino1_enabled.png.pagespeed.ic.dLhpKgSTBu.png

    https://developer.mbed.org/media/uploads/MACRUM/lpcxpresso4337_arduino2_enabled.png   

The images and schematics are also available in the docs folder.


.. _hardware:
Hardware
--------

There are three hardware components involved:

1. **Micro Controller**:

2. **Proximity Coupling Device (PCD)**:

* The PCD is the actual RFID **Reader** based on `NXP MFRC522`_ Contactless
  Reader Integrated Circuit).

* Readers can be found on `eBay`_ for around USD 5: search for *"rc522"*.

* You can also find them at several web stores, they are often included in
  *"starter kits"*; so check your favorite electronics provider as well.

3. **Proximity Integrated Circuit Card (PICC)**:

* The PICC is the RFID **Card** or **Tag** using the `ISO/IEC 14443A`_
  interface, for example Mifare or NTAG203.

* One or two might be included with the Reader or *"starter kit"* already.


.. _protocol:
Protocols
---------

1. The micro controller and the reader use SPI for communication.

* The protocol is described in the `NXP MFRC522`_ datasheet.

* See the `Pin Layout`_ section for details on connecting the pins.

2. The reader and the tags communicate using a 13.56 MHz electromagnetic field.

* The protocol is defined in ISO/IEC 14443-3:2011 Part 3 Type A.

  * Details are found in chapter 6 *"Type A – Initialization and anti-collision"*.
  
  * See http://wg8.de/wg8n1496_17n3613_Ballot_FCD14443-3.pdf for a free version
    of the final draft (which might be outdated in some areas).
    
  * The reader does not support ISO/IEC 14443-3 Type B.


.. _security:
Security
-------
This library only supports crypto1-encrypted communication. Crypto1 has been known as `broken`_ for a few years, so it does NOT offer ANY security, it is virtually unencrypted communication. **Do not use it for any security related applications!**

This library does not offer 3DES or AES authentication used by cards like the Mifare DESFire, it may be possible to be implemented because the datasheet says there is support. We hope for pull requests :).


.. _troubleshooting:
Troubleshooting
-------

* **I don't get input from reader** or **WARNING: Communication failure, is the MFRC522 properly connected?**

  #. Check your connection, see `Pin Layout`_ .
  #. Check voltage. Most breakouts work with 3.3V.
  #. SPI only works with 3.3V, most breakouts seem 5V tolerant, but try a level shifter.
  #. According to reports #101, #126 and #131, there may be a problem with the soldering on the MFRC522 breakout. You could fix this on your own.


* **Sometimes I get timeouts** or **sometimes tag/card does not work.**

  #. Try other side of the antenna.
  #. Try to decrease distance between MFRC522.
  #. Increase antenna gain per firmware: ``mfrc522.PCD_SetAntennaGain(mfrc522.RxGain_max);``
  #. Use better power supply.
  #. Hardware may be corrupted, most products are from china and sometimes the quality is really poor. Contact your seller.


* **My tag/card doesn't work.**
  
  #. Distance between antenna and token too large (>1cm).
  #. You got the wrong type PICC. Is it really 13.56 MHz? Is it really a Mifare Type A?
  #. NFC tokens are not supported. Some may work.
  #. Animal RFID tags are not supported. They use a different frequency (125 kHz).
  #. Hardware may be corrupted, most products are from china and sometimes the quality is really poor. Contact your seller.
  #. Newer versions of Mifare cards like DESFire/Ultralight maybe not work according to missing authentication, see `security`_ or different `protocol`_.
  #. Some boards bought from chinese manufactures do not use the best components and this can affect the detection of different types of tag/card. In some of these boards, the L1 and L2 inductors do not have a high enough current so the signal generated is not enough to get Ultralight C and NTAG203 tags to work, replacing those with same inductance (2.2uH) but higher operating current inductors should make things work smoothly. Also, in some of those boards the  harmonic and matching circuit needs to be tuned, for this replace C4 and C5 with 33pf capacitors and you are all set. (Source: `Mikro Elektronika`_) 

* **My mobile phone doesn't recognize the MFRC522** or **my MFRC522 can't read data from other MFRC522**

  #. Card simulation is not supported.
  #. Communication with mobile phones is not supported.
  #. Peer to peer communication is not supported.


* **I need more features.**

  #. If software: code it and make a pull request.
  #. If hardware: buy a more expensive like PN532 (supports NFC and many more, but costs about $15)


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


History
-------

The MFRC522 library was first created in Jan 2012 by Miguel Balboa (from
http://circuitito.com) based on code by Dr. Leong (from http://B2CQSHOP.com)
for *"Arduino RFID module Kit 13.56 Mhz with Tags SPI W and R By COOQRobot"*.

It was translated into English and rewritten/refactored in the fall of 2013
by Søren Thing Andersen (from http://access.thing.dk).

It has been extended with functionality to alter sector 0 on Chinese UID changeable MIFARE card in Oct 2014 by Tom Clement (from http://tomclement.nl).


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
