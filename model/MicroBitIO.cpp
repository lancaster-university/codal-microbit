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

/**
  * Class definition for MicroBit NEXT IO.
  * Represents a collection of all I/O pins on the device.
  */

#include "CodalConfig.h"
#include "MicroBitIO.h"

using namespace codal;

/**
  * Constructor.
  *
  * Create a representation of all given I/O pins on the edge connector
  *
  * Accepts a sequence of unique ID's used to distinguish events raised
  * by MicroBitPin instances on the default EventModel.
  */
MicroBitIO::MicroBitIO() :
    P0 (ID_PIN_P0, MICROBIT_PIN_P0, PIN_CAPABILITY_AD),             //P0 is the left most pad (ANALOG/DIGITAL/TOUCH)
    P1 (ID_PIN_P1, MICROBIT_PIN_P1, PIN_CAPABILITY_AD),             //P1 is the middle pad (ANALOG/DIGITAL/TOUCH)
    P2 (ID_PIN_P2, MICROBIT_PIN_P2, PIN_CAPABILITY_AD),             //P2 is the right most pad (ANALOG/DIGITAL/TOUCH)
    P3 (ID_PIN_P3, MICROBIT_PIN_P3, PIN_CAPABILITY_AD),             //COL1 (ANALOG/DIGITAL)
    P4 (ID_PIN_P4, MICROBIT_PIN_P4, PIN_CAPABILITY_AD),             //COL2 (ANALOG/DIGITAL)
    P5 (ID_PIN_P5, MICROBIT_PIN_P5, PIN_CAPABILITY_DIGITAL),        //BTN_A
    P6 (ID_PIN_P6, MICROBIT_PIN_P6, PIN_CAPABILITY_DIGITAL),        //ROW2
    P7 (ID_PIN_P7, MICROBIT_PIN_P7, PIN_CAPABILITY_DIGITAL),        //ROW1
    P8 (ID_PIN_P8, MICROBIT_PIN_P8, PIN_CAPABILITY_DIGITAL),        //PIN 18
    P9 (ID_PIN_P9, MICROBIT_PIN_P9, PIN_CAPABILITY_DIGITAL),        //ROW3
    P10(ID_PIN_P10,MICROBIT_PIN_P10,PIN_CAPABILITY_AD),             //COL3 (ANALOG/DIGITAL)
    P11(ID_PIN_P11,MICROBIT_PIN_P11,PIN_CAPABILITY_DIGITAL),        //BTN_B
    P12(ID_PIN_P12,MICROBIT_PIN_P12,PIN_CAPABILITY_DIGITAL),        //PIN 20
    P13(ID_PIN_P13,MICROBIT_PIN_P13,PIN_CAPABILITY_DIGITAL),        //SCK
    P14(ID_PIN_P14,MICROBIT_PIN_P14,PIN_CAPABILITY_DIGITAL),        //MISO
    P15(ID_PIN_P15,MICROBIT_PIN_P15,PIN_CAPABILITY_DIGITAL),        //MOSI
    P16(ID_PIN_P16,MICROBIT_PIN_P16,PIN_CAPABILITY_DIGITAL),        //PIN 16
    P19(ID_PIN_P19,MICROBIT_PIN_P19,PIN_CAPABILITY_DIGITAL),        //SCL
    P20(ID_PIN_P20,MICROBIT_PIN_P20,PIN_CAPABILITY_DIGITAL),         //SDA
    buttonA(P5),
    buttonB(P11),
    scl(P19),
    sda(P20),

    col1(P3),
    col2(P4),
    col3(P10),
    col4(ID_PIN_P21, P0_7, PIN_CAPABILITY_DIGITAL),
    col5(ID_PIN_P22, P0_8, PIN_CAPABILITY_DIGITAL),
    col6(ID_PIN_P23, P0_9, PIN_CAPABILITY_DIGITAL),
    col7(P9),
    col8(P7),
    col9(P6),
    row1(ID_PIN_P24, P0_13, PIN_CAPABILITY_DIGITAL),
    row2(ID_PIN_P25, P0_14, PIN_CAPABILITY_DIGITAL),
    row3(ID_PIN_P26, P0_15, PIN_CAPABILITY_DIGITAL)
{
}
