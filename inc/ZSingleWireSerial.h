#ifndef ZSINGLE_WIRE_SERIAL_H
#define ZSINGLE_WIRE_SERIAL_H

#include "Pin.h"
#include "CodalComponent.h"
#include "CodalConfig.h"
#include "SingleWireSerial.h"
#include "PktSerial.h"
#include "MemberFunctionCallback.h"

#define SINGLE_WIRE_SERIAL_EVT_RX_FULL      1
#define SINGLE_WIRE_SERIAL_EVT_TX_EMPTY     2020        // using shared notify id, hopefully no one else uses this...

namespace codal
{

    class ZSingleWireSerial : public DMASingleWireSerial
    {
        uint8_t* buf;
        uint16_t bufLen;

        protected:
        virtual int configureTx(int);
        virtual int configureRx(int);

        public:

        virtual void configureRxInterrupt(int enable);
        virtual void configureTxInterrupt(int enable);

        static ZSingleWireSerial* instance;

        volatile uint8_t* txBuff;
        volatile uint16_t txBuffHead;
        volatile uint16_t txBuffTail;

        volatile uint8_t* rxBuff;
        volatile uint16_t rxBuffHead;
        volatile uint16_t rxBuffTail;

        uint8_t* userBuff;
        uint16_t targetLen;

        PktSerialPkt* currentBuffer;
        uint32_t currentBufferIndex;

        ZSingleWireSerial(Pin& p);

        void circularCopy(uint8_t *circularBuff, uint8_t circularBuffSize, uint8_t *linearBuff, uint16_t tailPosition, uint16_t headPosition);
        void idleCallback();

        virtual int putc(char c);
        virtual int getc();

        virtual int send(uint8_t* data, int len);
        virtual int receive(uint8_t* data, int len);

        virtual int sendDMA(uint8_t* data, int len);
        virtual int receiveDMA(uint8_t* data, int len);
        virtual int abortDMA();

        virtual int setBaud(uint32_t baud);
        virtual uint32_t getBaud();

        virtual int sendBreak();
        int rxBufferedSize();
    };
}

#endif