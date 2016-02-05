MFRC522
=======

.. image:: https://travis-ci.org/miguelbalboa/rfid.svg?branch=master
    :target: https://travis-ci.org/miguelbalboa/rfid

Arduino library for MFRC522 and other RFID RC522 based modules.

Read and write different types of Radio-Frequency IDentification (RFID) cards
on your Arduino using a RC522 based reader connected via the Serial Peripheral
Interface (SPI) interface.

Set the UID, write to sector 0, and unbrick Chinese UID changeable MIFARE cards.


.. _pin layout:

Pin Layout
----------

The following table shows the typical pin layout used:

+-----------+----------+-------------------------------------------------------------+--------------------------+
|           | PCD      | Arduino                                                     | Teensy                   |
|           +----------+-------------+---------+---------+---------------+-----------+--------+--------+--------+
|           | MFRC522  | Uno         | Mega    | Nano v3 |Leonardo/Micro | Pro Micro | 2.0    | ++ 2.0 | 3.1    |
+-----------+----------+-------------+---------+---------+---------------+-----------+--------+--------+--------+
| Signal    | Pin      | Pin         | Pin     | Pin     | Pin           | Pin       | Pin    | Pin    | Pin    |
+===========+==========+=============+=========+=========+===============+===========+========+========+========+
| RST/Reset | RST      | 9 [1]_      | 5 [1]_  | D9      | RESET/ICSP-5  | RST       | 7      | 4      | 9      |
+-----------+----------+-------------+---------+---------+---------------+-----------+--------+--------+--------+
| SPI SS    | SDA [3]_ | 10 [2]_     | 53 [2]_ | D10     | 10            | 10        | 0      | 20     | 10     |
+-----------+----------+-------------+---------+---------+---------------+-----------+--------+--------+--------+
| SPI MOSI  | MOSI     | 11 / ICSP-4 | 51      | D11     | ICSP-4        | 16        | 2      | 22     | 11     |
+-----------+----------+-------------+---------+---------+---------------+-----------+--------+--------+--------+
| SPI MISO  | MISO     | 12 / ICSP-1 | 50      | D12     | ICSP-1        | 14        | 3      | 23     | 12     |
+-----------+----------+-------------+---------+---------+---------------+-----------+--------+--------+--------+
| SPI SCK   | SCK      | 13 / ICSP-3 | 52      | D13     | ICSP-3        | 15        | 1      | 21     | 13     |
+-----------+----------+-------------+---------+---------+---------------+-----------+--------+--------+--------+

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


.. _protocol:
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
    
  * The reader does not support ISO/IEC 14443-3 Type B.


.. _security:
Security
-------
This library only supports crypto1-encrypted communication. Crypto1 has been known as `broken`_ for a few years, so it does NOT offer ANY security, it is virtually unencrypted communication. **Do not use it for any security related applications!**
This library does not offer 3DES or AES authentication used by cars like the Mifare DESFire, it may be possible to be implemented because the datasheet says there is support. We hope for pull requests :).


.. _troubleshooting:
Troubleshooting
-------

* **I don't get input from reader** or **WARNING: Communication failure, is the MFRC522 properly connected?**

  #. Check your connection, see `Pin Layout`_ .
  #. Check voltage. Most breakouts work with 3.3V.
  #. SPI only works with 3.3V, most breakouts seem 5V tollerant, but try a level shifter.
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
  #. Newer versions of Mifare cards like DESFire/Ultralight maybe not work according to missing authentification, see `security`_ or different `protocol`_.

* **My mobile phone doesn't recognize the MFRC522** or **my MFRC522 can't read data from other MFRC522**

  #. Card simmulation is not supported.
  #. Communication with mobile phones is not supported.
  #. Peer to peer communication is not supported.


* **I need more features.**

  #. If software: code it and make a pull request.
  #. If hardware: buy a more expensive like PN532 (supports NFC and many more, but costs about $15)


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


.. _arduino: https://arduino.cc/
.. _ebay: http://www.ebay.com/
.. _iso/iec 14443a: https://en.wikipedia.org/wiki/ISO/IEC_14443
.. _iso/iec 14443-3\:2011 part 3: 
.. _nxp mfrc522: http://www.nxp.com/documents/data_sheet/MFRC522.pdf
.. _broken: http://eprint.iacr.org/2008/166
