# TempHygroTX868

Arduino library to control the ELV TX868 rf transmitter module to send temperature and humidity values over the air at 868.35 MHz.

The communication protocol is compatible to ELV sensors like the S 300 and ASH 2200, therefore the data may be received by weather stations like USB-WDE 1, WS 200/300, and IPWE 1 manufactured by [ELV](http://www.elv.de).

## Usage

    #include <TempHygroTX868.h>

    TempHygroTX868 tx;

    void setup()
    {
      tx.setup(5); // TX868 is at data pin 5
    }

    void loop()
    {
      byte address = 3;
      float humidity = 50;
      float temperature = 21.4;

      tx.setAddress(address);
      tx.send(temperature, humidity);

      delay((unsigned long)tx.getPause() * 1000UL);
    }

For all options, see [TempHygroTX868.h][header]. The [examples][example] directory contains several examples including full production code to build a weather sensor/transmitter similar to the S300:

![Wireless temperature and humidity sensor](https://www.kompf.de/tech/images/rftemp_comp_annot.png)

## Extension for old sensor protocol

The library has been extended to implement the old ELV sensor transmission protocol V1.1 as well. This should support Thermo/Hygro sensors like the AS2000 and ASH2000 which are using the 433 MHz transmitter HFS-300.

To enable the old protocol, pass the protocol version to the setup() method:

    void setup()
    {
      // TX868 is at data pin 5
      // Use the old protocol V1.1
      tx.setup(5, TempHygroTX868::PROT_V11);
    }

## Installation

* Download TempHygroTX868.zip from [releases page][release].
* Follow the steps described at [Installing Additional Arduino Libraries](https://www.arduino.cc/en/guide/libraries).

## References

* [Protokollversion V1.2][prot_12]
* [Protokollversion V1.1][prot_11]

## License

Copyright 2015, 2020 Martin Kompf

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.
 
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

[header]: https://github.com/skaringa/TempHygroTX868/blob/master/TempHygroTX868.h "Header file"
[example]: https://github.com/skaringa/TempHygroTX868/blob/master/examples "Examples"
[release]: https://github.com/skaringa/TempHygroTX868/releases/latest
[prot_11]: http://www.dc3yc.homepage.t-online.de/protocol_alt.htm
[prot_12]: http://www.dc3yc.homepage.t-online.de/protocol.htm

