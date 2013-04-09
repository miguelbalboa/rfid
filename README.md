Arduino RFID Library for MF522-AN
=================================
> Name: MF522-AN  
> Design: C_CL  
> Date: 10-06-10  

Pin order, starting from the bottom left hand pin
(in case your MF522-AN doesn't have pin markings like the B2CQSHOP one):

| Pins | SPI      | UNO  | Mega2560    |
| ---- |:--------:| ----:| --------:   |
| 1    | SS       |  10  |  53         |
| 2    | SCK      |  13  |  52         |
| 3    | MOSI     |  11  |  51         |
| 4    | MISO     |  12  |  50         |
| 5    |          | None               |
| 6    |          | GND                |
| 7    |          | RST (Digital Pin 5)|
| 8    |          | +3.3V              |
