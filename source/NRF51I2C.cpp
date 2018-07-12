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
#include "CodalConfig.h"
#include "ErrorNo.h"
#include "NRF51I2C.h"
#include "nrf51.h"
#include "nrf51_bitfields.h"

using namespace codal;

void NRF51I2C::init()
{
    NRF_TWI0->EVENTS_ERROR = 0;
    NRF_TWI0->ENABLE       = TWI_ENABLE_ENABLE_Disabled << TWI_ENABLE_ENABLE_Pos;
    NRF_TWI0->POWER        = 0;
    for (int i = 0; i<100; i++) {
    }

    NRF_TWI0->POWER = 1;

    NRF_GPIO->PIN_CNF[scl.name] = ((GPIO_PIN_CNF_DIR_Input << GPIO_PIN_CNF_DIR_Pos) |
                              (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos) |
                              (GPIO_PIN_CNF_PULL_Disabled << GPIO_PIN_CNF_PULL_Pos) |
                              (GPIO_PIN_CNF_DRIVE_S0D1 << GPIO_PIN_CNF_DRIVE_Pos) |
                              (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos));

    NRF_GPIO->PIN_CNF[sda.name] = ((GPIO_PIN_CNF_DIR_Input << GPIO_PIN_CNF_DIR_Pos) |
                              (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos) |
                              (GPIO_PIN_CNF_PULL_Disabled << GPIO_PIN_CNF_PULL_Pos) |
                              (GPIO_PIN_CNF_DRIVE_S0D1 << GPIO_PIN_CNF_DRIVE_Pos) |
                              (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos));

    NRF_TWI0->PSELSCL = scl.name;
    NRF_TWI0->PSELSDA = sda.name;

    NRF_TWI0->ENABLE = (TWI_ENABLE_ENABLE_Enabled << TWI_ENABLE_ENABLE_Pos);
}

NRF51I2C::NRF51I2C(Pin &sda, Pin &scl) : I2C(sda,scl), sda(sda), scl(scl)
{
    init();
}

/** Set the frequency of the I2C interface
 *
 * @param frequency The bus frequency in hertz
 */
int NRF51I2C::setFrequency(uint32_t frequency)
{
    if (frequency == 100000)
        NRF_TWI0->FREQUENCY = 0x01980000;
    else if (frequency == 250000)
        NRF_TWI0->FREQUENCY = 0x04000000;
    else
        // default to 400 000
        NRF_TWI0->FREQUENCY = 0x06680000;
    return DEVICE_OK;
}

/**
 * Issues a START condition on the I2C bus
 * @return DEVICE_OK on success, or an error code
 */
int NRF51I2C::start()
{
    int status = 0;
    init();
    return status;
}

/**
 * Issues a STOP condition on the I2C bus
 * @return DEVICE_OK on success, or an error code
 */
int NRF51I2C::stop()
{
    int timeOut = 100000;
    NRF_TWI0->EVENTS_STOPPED = 0;

    // write the stop bit
    NRF_TWI0->TASKS_STOP = 1;
    while (NRF_TWI0->EVENTS_STOPPED == 0)
    {
        timeOut--;

        if (timeOut < 0)
            return DEVICE_I2C_ERROR;
    }

    init();

    return DEVICE_OK;
}

/**
 * Writes the given byte to the I2C bus.
 *
 * The CPU will busy wait until the transmission is complete.
 *
 * @param data The byte to write.
 * @return DEVICE_OK on success, DEVICE_I2C_ERROR if the the write request failed.
 */
int NRF51I2C::write(uint8_t data)
{
    int timeOut = 100000;
    NRF_TWI0->TXD = data;
    NRF_TWI0->TASKS_STARTTX = 1;
    while (NRF_TWI0->EVENTS_TXDSENT == 0)
    {
        timeOut--;

        if (timeOut < 0)
            return DEVICE_I2C_ERROR;
    }

    NRF_TWI0->EVENTS_TXDSENT = 0;
    return DEVICE_OK;
}

/**
* Reads a single byte from the I2C bus.
* The CPU will busy wait until the transmission is complete.
*
* @return the byte read from the I2C bus, or DEVICE_I2C_ERROR if the the write request failed.
*/
int NRF51I2C::read(AcknowledgeType ack)
{
    int timeOut = 100000;

    // To trigger stop task when a byte is received,
    // must be set before resume task.
    // if (ack == NACK)
    //      NRF_TWI0->SHORTS |= 1 << 1;

    NRF_TWI0->TASKS_STARTRX = 1;
    NRF_TWI0->TASKS_RESUME = 1;

    while (NRF_TWI0->EVENTS_RXDREADY == 0)
    {
        timeOut--;

        if (timeOut < 0)
        {

            return DEVICE_I2C_ERROR;
        }
    }

    NRF_TWI0->SHORTS &= ~(1 << 1);
    return NRF_TWI0->RXD;
}

