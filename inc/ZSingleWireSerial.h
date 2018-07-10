#ifndef ZSINGLE_WIRE_SERIAL_H
#define ZSINGLE_WIRE_SERIAL_H

#include "Pin.h"
#include "CodalComponent.h"
#include "CodalConfig.h"
#include "SingleWireSerial.h"
#include "PktSerial.h"
#include "MemberFunctionCallback.h"

namespace codal
{

    class ZSingleWireSerial : public DMASingleWireSerial
    {
        uint8_t* buf;
        uint16_t bufLen;

        protected:
        virtual void configureRxInterrupt(int enable);
        virtual void configureTxInterrupt(int enable);

        virtual int configureTx(int);
        virtual int configureRx(int);

        public:

        PktSerialPkt* currentBuffer;
        uint32_t currentBufferIndex;

        ZSingleWireSerial(Pin& p);

        virtual int putc(char c);
        virtual int getc();

        virtual int send(uint8_t* data, int len);
        virtual int receive(uint8_t* data, int len);

        virtual int sendDMA(uint8_t* data, int len);
        virtual int receiveDMA(uint8_t* data, int len);
        virtual int abortDMA();

        virtual int setBaud(uint32_t baud);

        virtual int sendBreak();
    };
}

#endif