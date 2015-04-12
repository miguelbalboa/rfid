MFRC522
=======

Arduino library for MFRC522 and other RFID RC522 based modules.

Read and write different types of Radio-Frequency IDentification (RFID) cards
on your Arduino using a RC522 based reader connected via the Serial Peripheral
Interface (SPI) interface.

Set the UID, write to sector 0, and unbrick Chinese UID changeable MIFARE cards.


.. _pin layout:

Pin Layout
----------

The following table shows the typical pin layout used:

+-----------+----------+-------------------------------------------------------------+--------+
|           | PCD      | Arduino                                                     | Teensy |
|           +----------+-------------+---------+---------+---------------+-----------+--------+
|           | MFRC522  | Uno         | Mega    | Nano v3 |Leonardo/Micro | Pro Micro | 3.1    |
+-----------+----------+-------------+---------+---------+---------------+-----------+--------+
| Signal    | Pin      | Pin         | Pin     | Pin     | Pin           | Pin       | Pin    |
+===========+==========+=============+=========+=========+===============+===========+========+
| RST/Reset | RST      | 9 [1]_      | 5 [1]_  | D9      | RESET/ICSP-5  | RST       | 9      |
+-----------+----------+-------------+---------+---------+---------------+-----------+--------+
| SPI SS    | SDA [3]_ | 10 [2]_     | 53 [2]_ | D10     | 10            | 10        | 10     |
+-----------+----------+-------------+---------+---------+---------------+-----------+--------+
| SPI MOSI  | MOSI     | 11 / ICSP-4 | 52      | D11     | ICSP-4        | 16        | 11     |
+-----------+----------+-------------+---------+---------+---------------+-----------+--------+
| SPI MISO  | MISO     | 12 / ICSP-1 | 51      | D12     | ICSP-1        | 14        | 12     |
+-----------+----------+-------------+---------+---------+---------------+-----------+--------+
| SPI SCK   | SCK      | 13 / ICSP-3 | 50      | D13     | ICSP-3        | 15        | 13     |
+-----------+----------+-------------+---------+---------+---------------+-----------+--------+

.. [1] Configurable, typically defined as RST_PIN in sketch/program.
.. [2] Configurable, typically defined as SS_PIN in sketch/program.
.. [3] The SDA pin might be labeled SS on some/older MFRC522 boards. 


Hardware
--------

There are three hardware components involved:

1. **Micro Controller**:

* An `Arduino`_ or compatible executing the Sketch using this library.

* Prices vary from USD 7 for clones, to USD 75 for "starter kits" (which
  might be a good choice if this is your first exposure to Arduino;
  check if such kit already includes the Arduino, Reader, and some Tags).

2. **Proximity Coupling Device (PCD)**:

* The PCD is the actual RFID **Reader** based on `NXP MFRC522`_ Contactless
  Reader Integrated Circuit).

* Readers can be found on `eBay`_ for around USD 5: search for *"rc522"*.

* You can also find them at several web stores, they are often included in
  *"starter kits"*; so check your favourite electronics provider as well.

3. **Proximity Integrated Circuit Card (PICC)**:

* The PICC is the RFID **Card** or **Tag** using the `ISO/IEC 14443A`_
  interface, for example Mifare or NTAG203.

* One or two might be included with the Reader or *"starter kit"* already.


Protocols
---------

1. The micro controller and the reader use SPI for communication.

* The protocol is described in the `NXP MFRC522`_ datasheet.

* See the `Pin Layout`_ section for details on connecting the pins.

2. The reader and the tags communicate using a 13.56 MHz electromagnetic field.

* The protocol is defined in ISO/IEC 14443-3:2011 Part 3 Type A.

  * Details are found in chapter 6 *"Type A – Initialization and anticollision"*.

  * See http://wg8.de/wg8n1496_17n3613_Ballot_FCD14443-3.pdf for a free version
    of the final draft (which might be outdated in some areas).

  * The reader do not support ISO/IEC 14443-3 Type B.

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

For more information, please refer to http://unlicense.org/


History
-------

The MFRC522 library was first created in Jan 2012 by Miguel Balboa (from
http://circuitito.com) based on code by Dr. Leong (from http://B2CQSHOP.com)
for *"Arduino RFID module Kit 13.56 Mhz with Tags SPI W and R By COOQRobot"*.

It was translated into English and rewritten/refactored in the fall of 2013
by Søren Thing Andersen (from http://access.thing.dk).

It has been extended with functionality to alter sector 0 on Chinese UID changeable MIFARE card in Oct 2014 by Tom Clement (from http://tomclement.nl).


.. _arduino: http://arduino.cc/
.. _ebay: http://www.ebay.com/
.. _iso/iec 14443a: http://en.wikipedia.org/wiki/ISO/IEC_14443
.. _iso/iec 14443-3\:2011 part 3: 
.. _nxp mfrc522: http://www.nxp.com/documents/data_sheet/MFRC522.pdf
