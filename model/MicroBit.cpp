/*
The MIT License (MIT)

Copyright (c) 2016 Lancaster University, UK.

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


#include "MicroBit.h"
#include "Timer.h"

using namespace codal;

//
// Matrix layout model for LED Matrix
//
// static const MatrixPoint ledMatrixPositions[5*5] =
// {
//     {0,0},{0,1},{0,2},{0,3},{0,4},
//     {1,0},{1,1},{1,2},{1,3},{1,4},
//     {2,0},{2,1},{2,2},{2,3},{2,4},
//     {3,0},{3,1},{3,2},{3,3},{3,4},
//     {4,0},{4,1},{4,2},{4,3},{4,4}
// };

// static const MatrixPoint ledMatrixPositions[5*5] =
// {
//     {0,0},{1,3},{0,1},{1,4},{0,2},
//     {2,3},{2,4},{2,5},{2,6},{2,7},
//     {1,1},{0,8},{1,2},{2,8},{1,0},
//     {0,7},{0,6},{0,5},{0,4},{0,3},
//     {2,2},{1,6},{2,0},{1,5},{2,1}
// };


// static const MatrixPoint ledMatrixPositions[5*5] =
// {
//     {0,0},{3,1},{1,0},{4,1},{2,0},
//     {3,2},{4,2},{5,2},{6,2},{7,2},
//     {1,1},{8,0},{2,1},{8,2},{0,1},
//     {7,0},{6,0},{5,0},{4,0},{3,0},
//     {2,2},{6,1},{0,2},{5,1},{1,2}
// };


static const MatrixPoint ledMatrixPositions[3*9] =
{
    {0,0},{4,2},{2,4},
    {2,0},{0,2},{4,4},
    {4,0},{2,2},{0,4},
    {4,3},{1,0},{0,1},
    {3,3},{3,0},{1,1},
    {2,3},{3,4},{2,1},
    {1,3},{1,4},{3,1},
    {0,3},{NO_CONN,NO_CONN},{4,1},
    {1,2},{NO_CONN,NO_CONN},{3,2}
};



/**
  * Constructor.
  *
  * Create a representation of a GenuinoZero device, which includes member variables
  * that represent various device drivers used to control aspects of the micro:bit.
  */
MicroBit::MicroBit() :

    serial(USBTX, USBRX),
    messageBus(),
    timer(),
    io(),
    i2c(io.sda, io.scl),

    ledRowPins{&io.row1, &io.row2, &io.row3},
    ledColPins{&io.col1, &io.col2, &io.col3, &io.col4, &io.col5, &io.col6, &io.col7, &io.col8, &io.col9},

    ledMatrixMap{ 5, 5, 3, 9, (Pin**)ledRowPins, (Pin**)ledColPins, ledMatrixPositions},
    display(ledMatrixMap),
    buttonA(io.buttonA, DEVICE_ID_BUTTON_A, DEVICE_BUTTON_ALL_EVENTS, ACTIVE_LOW),
    buttonB(io.buttonB, DEVICE_ID_BUTTON_B, DEVICE_BUTTON_ALL_EVENTS, ACTIVE_LOW),
    buttonAB(DEVICE_ID_BUTTON_A, DEVICE_ID_BUTTON_B, DEVICE_ID_BUTTON_AB),
    radio(),
    thermometer(),
    coordinateSpace(SIMPLE_CARTESIAN, true)
    // compassCalibrator(compass, accelerometer, display)
{
    // Clear our status
    status = 0;

    serial.baud(115200);
    i2c.setFrequency(400000);
}

/**
  * Post constructor initialisation method.
  *
  * This call will initialised the scheduler, memory allocator and Bluetooth stack.
  *
  * This is required as the Bluetooth stack can't be brought up in a
  * static context i.e. in a constructor.
  *
  * @code
  * uBit.init();
  * @endcode
  *
  * @note This method must be called before user code utilises any functionality
  *       contained within the GenuinoZero class.
  */
int MicroBit::init()
{
    if (status & DEVICE_INITIALIZED)
        return DEVICE_NOT_SUPPORTED;

    status |= DEVICE_INITIALIZED;

    // Bring up fiber scheduler.
    scheduler_init(messageBus);

    for(int i = 0; i < DEVICE_COMPONENT_COUNT; i++)
    {
        if(CodalComponent::components[i])
            CodalComponent::components[i]->init();
    }

    // Seed our random number generator
    //seedRandom();

    // Create an event handler to trap any handlers being created for I2C services.
    // We do this to enable initialisation of those services only when they're used,
    // which saves processor time, memeory and battery life.
    messageBus.listen(DEVICE_ID_MESSAGE_BUS_LISTENER, DEVICE_EVT_ANY, this, &MicroBit::onListenerRegisteredEvent);

#if CONFIG_ENABLED(DMESG_SERIAL_DEBUG)
#if DEVICE_DMESG_BUFFER_SIZE > 0
    codal_dmesg_set_flush_fn(microbit_dmesg_flush);
#endif
#endif
    status |= DEVICE_COMPONENT_STATUS_IDLE_TICK;

    return DEVICE_OK;
}

/**
  * A listener to perform actions as a result of Message Bus reflection.
  *
  * In some cases we want to perform lazy instantiation of components, such as
  * the compass and the accelerometer, where we only want to add them to the idle
  * fiber when someone has the intention of using these components.
  */
void MicroBit::onListenerRegisteredEvent(Event evt)
{
    switch(evt.value)
    {
        case DEVICE_ID_BUTTON_AB:
            // A user has registered to receive events from the buttonAB multibutton.
            // Disable click events from being generated by ButtonA and ButtonB, and defer the
            // control of this to the multibutton handler.
            //
            // This way, buttons look independent unless a buttonAB is requested, at which
            // point button A+B clicks can be correclty handled without breaking
            // causal ordering.
            buttonA.setEventConfiguration(DEVICE_BUTTON_SIMPLE_EVENTS);
            buttonB.setEventConfiguration(DEVICE_BUTTON_SIMPLE_EVENTS);
            buttonAB.setEventConfiguration(DEVICE_BUTTON_ALL_EVENTS);
            break;

        case DEVICE_ID_ACCELEROMETER:
        case DEVICE_ID_GESTURE:
            // A listener has been registered for the accelerometer.
            // The accelerometer uses lazy instantiation, we just need to read the data once to start it running.
            //accelerometer.updateSample();
            break;

        case DEVICE_ID_THERMOMETER:
            // A listener has been registered for the thermometer.
            // The thermometer uses lazy instantiation, we just need to read the data once to start it running.
            //thermometer.updateSample();
            break;

        case DEVICE_ID_LIGHT_SENSOR:
            // A listener has been registered for the light sensor.
            // The light sensor uses lazy instantiation, we just need to read the data once to start it running.
            //lightSensor.updateSample();
            break;
    }
}

/**
  * A periodic callback invoked by the fiber scheduler idle thread.
  * We use this for any low priority, backgrounf housekeeping.
  *
  */
void MicroBit::idleCallback()
{
#if CONFIG_ENABLED(DMESG_SERIAL_DEBUG)
#if DEVICE_DMESG_BUFFER_SIZE > 0
    codal_dmesg_flush();
#endif
#endif
}

void microbit_dmesg_flush()
{
#if CONFIG_ENABLED(DMESG_SERIAL_DEBUG)
#if DEVICE_DMESG_BUFFER_SIZE > 0
    if (codalLogStore.ptr > 0 && microbit_device_instance)
    {
        for (uint32_t i=0; i<codalLogStore.ptr; i++)
            ((MicroBit *)microbit_device_instance)->serial.putc(codalLogStore.buffer[i]);

        codalLogStore.ptr = 0;
    }
#endif
#endif
}

