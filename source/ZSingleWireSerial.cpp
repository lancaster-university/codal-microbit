#include "ZSingleWireSerial.h"
#include "nrf51.h"
#include "nrf51_bitfields.h"
#include "CodalDmesg.h"

using namespace codal;

#define TX_CONFIGURED       0x02
#define RX_CONFIGURED       0x04
#define FIRST_BREAK         0x08

#define SWS_BUFFER_SIZE     ((int)256)

ZSingleWireSerial* ZSingleWireSerial::instance = NULL;

#ifdef __cplusplus
extern "C" {
#endif

void UART0_IRQHandler()
{
    // codal_dmesg("IRQ %d", NRF_UART0->EVENTS_ERROR);
    if (ZSingleWireSerial::instance == NULL)
    {
        NRF_UART0->EVENTS_TXDRDY = 0;
        NRF_UART0->EVENTS_RXDRDY = 0;
        return;
    }

    if (NRF_UART0->EVENTS_ERROR)
    {
        codal_dmesg("ERR %d", NRF_UART0->ERRORSRC);
    }

    if((NRF_UART0->INTENSET & 0x80) && NRF_UART0->EVENTS_TXDRDY)
    {
        // codal_dmesg("TX");
        NRF_UART0->EVENTS_TXDRDY = 0;
        // no buffer
        if (ZSingleWireSerial::instance->txBuff== NULL || ZSingleWireSerial::instance->txBuffTail == ZSingleWireSerial::instance->txBuffHead)
            return;

        //send our current chars
        NRF_UART0->TXD = ZSingleWireSerial::instance->txBuff[ZSingleWireSerial::instance->txBuffTail];

        uint16_t nextTail = (ZSingleWireSerial::instance->txBuffTail + 1) % SWS_BUFFER_SIZE;

        //unblock any waiting fibers that are waiting for transmission to finish.
        if(nextTail == ZSingleWireSerial::instance->txBuffHead)
        {
            ZSingleWireSerial::instance->configureTxInterrupt(0);
            Event evt(0, SWS_EVT_DATA_SENT, 0, CREATE_ONLY);

            if (ZSingleWireSerial::instance->cb)
                ZSingleWireSerial::instance->cb->fire(evt);
        }

        //update our tail!
        ZSingleWireSerial::instance->txBuffTail = nextTail;
    }

    // keep processing minimal
    // any processing will result in lost bytes, or radio timing irregularities.
    if((NRF_UART0->INTENSET & 0x04) && NRF_UART0->EVENTS_RXDRDY)
    {
        // codal_dmesg("RX");
        NRF_UART0->EVENTS_RXDRDY = 0;

        if (NRF_UART0->EVENTS_ERROR)
        {
            codal_dmesg("ERR");
            NRF_UART0->EVENTS_ERROR = 0;
            uint32_t set = NRF_UART0->ERRORSRC & (UART_ERRORSRC_FRAMING_Msk | UART_ERRORSRC_BREAK_Msk);

            if (NRF_UART0->ERRORSRC & (UART_ERRORSRC_FRAMING_Msk | UART_ERRORSRC_BREAK_Msk)&& !(ZSingleWireSerial::instance->status & FIRST_BREAK))
            {
                NRF_UART0->ERRORSRC &= ~(UART_ERRORSRC_FRAMING_Msk | UART_ERRORSRC_BREAK_Msk);
                char c = NRF_UART0->RXD;
                codal_dmesg("F[%d]", c);
                // ZSingleWireSerial::instance->status |= FIRST_BREAK;
                return;
            }

            // if (NRF_UART0->ERRORSRC & (UART_ERRORSRC_BREAK_Msk) && (ZSingleWireSerial::instance->status & FIRST_BREAK))
            // {
            //     NRF_UART0->ERRORSRC &= ~(UART_ERRORSRC_BREAK_Msk);
            //     codal_dmesg("B");
            //     ZSingleWireSerial::instance->status &= ~FIRST_BREAK;
            // }
        }

        if (ZSingleWireSerial::instance->status & FIRST_BREAK)
        {
            char c = NRF_UART0->RXD;
            codal_dmesg("D[%d]", c);
            return;
        }

        // no buffer
        if (ZSingleWireSerial::instance->rxBuff == NULL)
            return;

        char c = NRF_UART0->RXD;
        codal_dmesg("R[%d]",c);

        uint16_t newHead = (ZSingleWireSerial::instance->rxBuffHead + 1) % SWS_BUFFER_SIZE;

        //look ahead to our newHead value to see if we are about to collide with the tail
        if(newHead != ZSingleWireSerial::instance->rxBuffTail)
        {

            //if we are not, store the character, and update our actual head.
            ZSingleWireSerial::instance->rxBuff[ZSingleWireSerial::instance->rxBuffHead] = c;
            ZSingleWireSerial::instance->rxBuffHead = newHead;
        }
        else
            //otherwise, our buffer is full, send an event to the user...
            Event(ZSingleWireSerial::instance->id, SINGLE_WIRE_SERIAL_EVT_RX_FULL);
    }
}

#ifdef __cplusplus
}
#endif


void ZSingleWireSerial::configureRxInterrupt(int enable)
{
    if (enable)
        NRF_UART0->INTENSET |= UART_INTENSET_RXDRDY_Msk;
    else
        NRF_UART0->INTENCLR |= UART_INTENCLR_RXDRDY_Msk;
}

void ZSingleWireSerial::configureTxInterrupt(int enable)
{
    if (enable)
        NRF_UART0->INTENSET |= UART_INTENSET_TXDRDY_Msk;
    else
        NRF_UART0->INTENCLR |= UART_INTENCLR_TXDRDY_Msk;
}

int ZSingleWireSerial::configureTx(int enable)
{
    if (enable && !(status & TX_CONFIGURED))
    {
        NRF_GPIO->DIR |= (1 << p.name);
        NRF_GPIO->PIN_CNF[p.name] =  3 << 2;
        NRF_UART0->PSELTXD = p.name;
        NRF_UART0->TASKS_STARTTX = 1;
        status |= TX_CONFIGURED;
    }
    else if (status & TX_CONFIGURED)
    {
        NRF_UART0->TASKS_SUSPEND = 1;
        while(NRF_UART0->TASKS_SUSPEND);

        NRF_UART0->PSELTXD = 0xFFFFFFFF;
        status &= ~TX_CONFIGURED;
    }
}

int ZSingleWireSerial::configureRx(int enable)
{
    if (enable && !(status & RX_CONFIGURED))
    {
        NRF_GPIO->DIR &= ~(1 << p.name);
        NRF_GPIO->PIN_CNF[p.name] =  3 << 2;
        NRF_UART0->PSELRXD = p.name;
        NRF_UART0->TASKS_STARTRX = 1;
        status |= RX_CONFIGURED;
    }
    else if (status & RX_CONFIGURED)
    {
        NRF_UART0->TASKS_SUSPEND = 1;
        while(NRF_UART0->TASKS_SUSPEND);

        NRF_UART0->PSELRXD = 0xFFFFFFFF;
        status &= ~RX_CONFIGURED;
    }
}

ZSingleWireSerial::ZSingleWireSerial(Pin& p) : DMASingleWireSerial(p)
{
    if (instance == NULL)
        instance = this;

    status = 0;

    rxBuff = (uint8_t*) malloc(SWS_BUFFER_SIZE);
    rxBuffHead = 0;
    rxBuffTail = 0;

    txBuff = (uint8_t*) malloc(SWS_BUFFER_SIZE);
    txBuffHead = 0;
    txBuffTail = 0;

    userBuff = NULL;
    targetLen = 0;

    // dummy write needed or TXDRDY trails write rather than leads write.
    //  pins are disconnected so nothing is physically transmitted on the wire
    NRF_UART0->TXD = 0;

    // these lines are disabled
    NRF_UART0->PSELCTS = 0xFFFFFFFF;
    NRF_UART0->PSELRTS = 0xFFFFFFFF;

    // this will be set to pin name depending on configure TX/RX
    NRF_UART0->PSELTXD = 0xFFFFFFFF;
    NRF_UART0->PSELRXD = 0xFFFFFFFF;

    setBaud(115200);

    NRF_UART0->ENABLE = 4;
    while(!(NRF_UART0->ENABLE));

    NRF_UART0->EVENTS_RXDRDY = 0;

    NVIC_SetPriority(UART0_IRQn, 3);
    NVIC_EnableIRQ(UART0_IRQn);

    status |= (DEVICE_COMPONENT_STATUS_IDLE_TICK | DEVICE_COMPONENT_RUNNING);
}

void ZSingleWireSerial::circularCopy(uint8_t *circularBuff, uint8_t circularBuffSize, uint8_t *linearBuff, uint16_t tailPosition, uint16_t headPosition)
{
    int toBuffIndex = 0;

    // codal_dmesg("%d, %d: ", tailPosition, headPosition);

    __disable_irq();
    while(tailPosition != headPosition)
    {
        // codal_dmesg("[%d] ", circularBuff[tailPosition]);
        linearBuff[toBuffIndex++] = circularBuff[tailPosition];
        tailPosition = (tailPosition + 1) % SWS_BUFFER_SIZE;
    }
    __enable_irq();
}

// it's be
void ZSingleWireSerial::idleCallback()
{
    // codal_dmesg("rxbs: %d", rxBufferedSize());
    if (userBuff && rxBufferedSize() >= targetLen)
    {
        codal_dmesg("RX DONE");
        configureRxInterrupt(0);
        // copy it into the user buffer
        circularCopy((uint8_t*)rxBuff, SWS_BUFFER_SIZE, userBuff, rxBuffTail, ((rxBuffTail + targetLen) % SWS_BUFFER_SIZE));
        rxBuffTail += targetLen;

        // signal driver code.
        Event evt(0, SWS_EVT_DATA_RECEIVED, CREATE_ONLY);
        if (ZSingleWireSerial::instance->cb)
        {
            codal_dmesg("FIRE");
            ZSingleWireSerial::instance->cb->fire(evt);
        }

        userBuff = NULL;
        targetLen = 0;
    }
}

int ZSingleWireSerial::putc(char c)
{
    if (!(status & TX_CONFIGURED))
        setMode(SingleWireTx);

    while(!NRF_UART0->EVENTS_TXDRDY);
    NRF_UART0->EVENTS_TXDRDY = 0;
    NRF_UART0->TXD = c;
}
int ZSingleWireSerial::getc()
{
    if (!(status & RX_CONFIGURED))
        setMode(SingleWireRx);

    while(!NRF_UART0->EVENTS_RXDRDY);
    NRF_UART0->EVENTS_RXDRDY = 0;
    return (uint8_t)NRF_UART0->RXD;
}

int ZSingleWireSerial::send(uint8_t* data, int len)
{
    for (int i = 0; i < len; i++)
        putc((char)data[i]);

    return DEVICE_OK;;
}

int ZSingleWireSerial::receive(uint8_t* data, int len)
{
    for (int i = 0; i < len; i++)
    {
        char c = getc();
        data[i] = c;
    }

    return DEVICE_OK;
}

int ZSingleWireSerial::sendDMA(uint8_t* data, int len)
{
    if (!(status & TX_CONFIGURED))
        setMode(SingleWireTx);

    int copiedBytes = 0;

    for(copiedBytes = 0; copiedBytes < len; copiedBytes++)
    {
        uint16_t nextHead = (txBuffHead + 1) % SWS_BUFFER_SIZE;
        if(nextHead != txBuffTail)
        {
            this->txBuff[txBuffHead] = data[copiedBytes];
            txBuffHead = nextHead;
        }
        else
            break;
    }

    configureTxInterrupt(1);
}
int ZSingleWireSerial::receiveDMA(uint8_t* data, int len)
{
    if (!(status & RX_CONFIGURED))
        setMode(SingleWireRx);

    userBuff = data;
    targetLen = len;

    configureRxInterrupt(1);
}

int ZSingleWireSerial::abortDMA()
{
    configureTxInterrupt(0);
    // configureRxInterrupt(0);
    txBuffHead = 0;
    txBuffTail = 0;
    userBuff = NULL;
    targetLen = 0;
}

int ZSingleWireSerial::setBaud(uint32_t baud)
{
    if (baud == 1000000)
        // 1m
        NRF_UART0->BAUDRATE = 0x10000000;
    else
        // 115200
        NRF_UART0->BAUDRATE = 0x01D7E000;

    return DEVICE_OK;
}

uint32_t ZSingleWireSerial::getBaud()
{
    if (NRF_UART0->BAUDRATE == 0x10000000)
        return 1000000;

    if (NRF_UART0->BAUDRATE == 0x01D7E000)
        return 115200;

    return 0;
}

int ZSingleWireSerial::sendBreak()
{
    return DEVICE_NOT_IMPLEMENTED;
}

/**
  * The number of bytes currently stored in our rx buffer waiting to be digested,
  * by the user.
  *
  * @return The currently buffered number of bytes in our rxBuff.
  */
int ZSingleWireSerial::rxBufferedSize()
{
    if(rxBuffTail > rxBuffHead)
        return (SWS_BUFFER_SIZE - rxBuffTail) + rxBuffHead;

    return rxBuffHead - rxBuffTail;
}