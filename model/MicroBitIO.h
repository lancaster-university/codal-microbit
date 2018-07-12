/*
The MIT License (MIT)

Copyright (c) 2017 Lancaster University.

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
*/

#ifndef MICROBIT_IO_H
#define MICROBIT_IO_H

#include "CodalConfig.h"
#include "NRF51Pin.h"
#include "PinMap.h"

//
// Unique Pin number for each pin (synonymous with mbedos PinName)
//

//Edge Connector
#define MICROBIT_PIN_P0                     P0_3        //P0 is the left most pad (ANALOG/DIGITAL) used to be P0_3 on green board
#define MICROBIT_PIN_P1                     P0_2        //P1 is the middle pad (ANALOG/DIGITAL)
#define MICROBIT_PIN_P2                     P0_1        //P2 is the right most pad (ANALOG/DIGITAL) used to be P0_1 on green board
#define MICROBIT_PIN_P3                     P0_4        //COL1 (ANALOG/DIGITAL)
#define MICROBIT_PIN_P4                     P0_5        //COL2 (ANALOG/DIGITAL)
#define MICROBIT_PIN_P5                     P0_17       //BTN_A
#define MICROBIT_PIN_P6                     P0_12       //COL9
#define MICROBIT_PIN_P7                     P0_11       //COL8
#define MICROBIT_PIN_P8                     P0_18       //PIN 18
#define MICROBIT_PIN_P9                     P0_10       //COL7
#define MICROBIT_PIN_P10                    P0_6        //COL3 (ANALOG/DIGITAL)
#define MICROBIT_PIN_P11                    P0_26       //BTN_B
#define MICROBIT_PIN_P12                    P0_20       //PIN 20
#define MICROBIT_PIN_P13                    P0_23       //SCK
#define MICROBIT_PIN_P14                    P0_22       //MISO
#define MICROBIT_PIN_P15                    P0_21       //MOSI
#define MICROBIT_PIN_P16                    P0_16       //PIN 16
#define MICROBIT_PIN_P19                    P0_0        //SCL
#define MICROBIT_PIN_P20                    P0_30       //SDA

// Internal connections
#define MICROBIT_PIN_ACCEL_DATA_READY               P0_25
#define MICROBIT_PIN_COMPASS_DATA_READY             P0_25
#define MICROBIT_PIN_BUTTON_A                       P0_14
#define MICROBIT_PIN_BUTTON_B                       P0_15
#define MICROBIT_PIN_BUTTON_RESET                   -1

//
// Component IDs for each pin.
// The can be user defined, but uniquely identify a pin when using the eventing APIs/
//
#define ID_PIN_P0        (DEVICE_ID_IO_P0 + 0)
#define ID_PIN_P1        (DEVICE_ID_IO_P0 + 1)
#define ID_PIN_P2        (DEVICE_ID_IO_P0 + 2)
#define ID_PIN_P3        (DEVICE_ID_IO_P0 + 3)
#define ID_PIN_P4        (DEVICE_ID_IO_P0 + 4)
#define ID_PIN_P5        (DEVICE_ID_IO_P0 + 5)
#define ID_PIN_P6        (DEVICE_ID_IO_P0 + 6)
#define ID_PIN_P7        (DEVICE_ID_IO_P0 + 7)
#define ID_PIN_P8        (DEVICE_ID_IO_P0 + 8)
#define ID_PIN_P9        (DEVICE_ID_IO_P0 + 9)
#define ID_PIN_P10       (DEVICE_ID_IO_P0 + 10)
#define ID_PIN_P11       (DEVICE_ID_IO_P0 + 11)
#define ID_PIN_P12       (DEVICE_ID_IO_P0 + 12)
#define ID_PIN_P13       (DEVICE_ID_IO_P0 + 13)
#define ID_PIN_P14       (DEVICE_ID_IO_P0 + 14)
#define ID_PIN_P15       (DEVICE_ID_IO_P0 + 15)
#define ID_PIN_P16       (DEVICE_ID_IO_P0 + 16)
#define ID_PIN_P17       (DEVICE_ID_IO_P0 + 17)
#define ID_PIN_P18       (DEVICE_ID_IO_P0 + 18)
#define ID_PIN_P19       (DEVICE_ID_IO_P0 + 19)
#define ID_PIN_P20       (DEVICE_ID_IO_P0 + 20)
#define ID_PIN_P21       (DEVICE_ID_IO_P0 + 21)
#define ID_PIN_P22       (DEVICE_ID_IO_P0 + 22)
#define ID_PIN_P23       (DEVICE_ID_IO_P0 + 23)
#define ID_PIN_P24       (DEVICE_ID_IO_P0 + 24)
#define ID_PIN_P25       (DEVICE_ID_IO_P0 + 25)
#define ID_PIN_P26       (DEVICE_ID_IO_P0 + 26)
#define ID_PIN_P27       (DEVICE_ID_IO_P0 + 27)
#define ID_PIN_P28       (DEVICE_ID_IO_P0 + 28)
#define ID_PIN_P29       (DEVICE_ID_IO_P0 + 29)
#define ID_PIN_P30       (DEVICE_ID_IO_P0 + 30)
#define ID_PIN_P31       (DEVICE_ID_IO_P0 + 31)

#define ID_NRF51_PIN_HI  (DEVICE_ID_IO_P0 + 35)
#define ID_NRF51_PIN_LO  (DEVICE_ID_IO_P0 + 36)

namespace codal
{
    /**
     * Represents a collection of all I/O pins exposed by the device.
     */
    class MicroBitIO
    {
        public:
            NRF51Pin          pin[0];
            NRF51Pin          P0;
            NRF51Pin          P1;
            NRF51Pin          P2;
            NRF51Pin          P3;
            NRF51Pin          P4;
            NRF51Pin          P5;
            NRF51Pin          P6;
            NRF51Pin          P7;
            NRF51Pin          P8;
            NRF51Pin          P9;
            NRF51Pin          P10;
            NRF51Pin          P11;
            NRF51Pin          P12;
            NRF51Pin          P13;
            NRF51Pin          P14;
            NRF51Pin          P15;
            NRF51Pin          P16;
            NRF51Pin          P19;
            NRF51Pin          P20;

            NRF51Pin&         buttonA;
            NRF51Pin&         buttonB;

            NRF51Pin&         scl;
            NRF51Pin&         sda;

            NRF51Pin&         col1;
            NRF51Pin&         col2;
            NRF51Pin&         col3;
            NRF51Pin          col4;
            NRF51Pin          col5;
            NRF51Pin          col6;
            NRF51Pin&         col7;
            NRF51Pin&         col8;
            NRF51Pin&         col9;

            NRF51Pin          row1;
            NRF51Pin          row2;
            NRF51Pin          row3;

            NRF51Pin          compassInt;
            NRF51Pin          accelerometerInt;

            /**
             * Constructor.
             *
             * Create a representation of all given I/O pins on the edge connector
             *
             * Accepts a sequence of unique ID's used to distinguish events raised
             * by _mbed::Pin instances on the default EventModel.
             */
            MicroBitIO();
    };
}

#endif
