#include "ZSingleWireSerial.h"
#include "nrf51.h"
#include "nrf51_bitfields.h"
#include "CodalDmesg.h"

using namespace codal;

#define TX_CONFIGURED       ((uint16_t)0x02)
#define RX_CONFIGURED       ((uint16_t)0x04)
#define FIRST_BREAK         ((uint16_t)0x08)

#define SWS_BUFFER_SIZE     ((int)256)

ZSingleWireSerial* ZSingleWireSerial::instance = NULL;

#ifdef __cplusplus
extern "C" {
#endif

void UART0_IRQHandler()
{
    if (ZSingleWireSerial::instance == NULL)
    {
        NRF_UART0->EVENTS_TXDRDY = 0;
        NRF_UART0->EVENTS_RXDRDY = 0;
        return;
    }

    // keep processing minimal
    // any processing will result in lost bytes, or radio timing irregularities.
    while(NRF_UART0->EVENTS_RXDRDY)
    {
        NRF_UART0->EVENTS_RXDRDY = 0;

        // always drain the FIFO
        char c = NRF_UART0->RXD;

        // no buffer
        if (ZSingleWireSerial::instance->buffer == NULL)
            return;

        ZSingleWireSerial::instance->buffer[ZSingleWireSerial::instance->bufferIdx++] = c;

        if(ZSingleWireSerial::instance->bufferIdx == ZSingleWireSerial::instance->bufferLength)
        {
            ZSingleWireSerial::instance->configureRxInterrupt(0);

            ZSingleWireSerial::instance->buffer = NULL;
            ZSingleWireSerial::instance->bufferIdx = 0;
            ZSingleWireSerial::instance->bufferLength = 0;

            Event evt(0, SWS_EVT_DATA_RECEIVED, 0, CREATE_ONLY);
            if (ZSingleWireSerial::instance->cb)
                ZSingleWireSerial::instance->cb->fire(evt);

            return;
        }
    }

    if (NRF_UART0->EVENTS_TXDRDY)
    {
        NRF_UART0->EVENTS_TXDRDY = 0;

        // no buffer
        if (ZSingleWireSerial::instance->buffer == NULL || !(NRF_UART0->INTENSET & UART_INTENSET_TXDRDY_Msk))
            return;

        char c = ZSingleWireSerial::instance->buffer[ZSingleWireSerial::instance->bufferIdx++];

        //send our current chars
        NRF_UART0->TXD = c;

        //unblock any waiting fibers that are waiting for transmission to finish.
        if(ZSingleWireSerial::instance->bufferIdx == ZSingleWireSerial::instance->bufferLength)
        {
            ZSingleWireSerial::instance->configureTxInterrupt(0);

            ZSingleWireSerial::instance->buffer = NULL;
            ZSingleWireSerial::instance->bufferIdx = 0;
            ZSingleWireSerial::instance->bufferLength = 0;

            Event evt(0, (uint16_t)SWS_EVT_DATA_SENT, 0, CREATE_ONLY);

            if (ZSingleWireSerial::instance->cb)
                ZSingleWireSerial::instance->cb->fire(evt);
        }
    }

    if (NRF_UART0->EVENTS_ERROR)
    {
        NRF_UART0->EVENTS_ERROR = 0;

        if (NRF_UART0->ERRORSRC & (UART_ERRORSRC_BREAK_Msk | UART_ERRORSRC_FRAMING_Msk))
        {
            NRF_UART0->ERRORSRC |= UART_ERRORSRC_BREAK_Clear;
            NRF_UART0->ERRORSRC |= UART_ERRORSRC_FRAMING_Clear;
        }

        if (NRF_UART0->ERRORSRC & (UART_ERRORSRC_OVERRUN_Msk))
        {
            ZSingleWireSerial::instance->buffer = NULL;
            ZSingleWireSerial::instance->bufferIdx = 0;
            ZSingleWireSerial::instance->bufferLength = 0;

            Event evt(0, (uint16_t)SWS_EVT_ERROR, 0, CREATE_ONLY);
            if (ZSingleWireSerial::instance->cb)
                ZSingleWireSerial::instance->cb->fire(evt);

            NRF_UART0->ERRORSRC |= UART_ERRORSRC_OVERRUN_Clear;
        }
    }
}

#ifdef __cplusplus
}
#endif


void ZSingleWireSerial::configureRxInterrupt(int enable)
{
    if (enable)
        NRF_UART0->INTENSET |= (UART_INTENSET_RXDRDY_Msk | UART_INTENSET_ERROR_Msk);
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
        NRF_UART0->ENABLE = 4;
        while(!(NRF_UART0->ENABLE));
        NRF_UART0->TASKS_STARTTX = 1;
        status |= TX_CONFIGURED;
    }
    else if (status & TX_CONFIGURED)
    {
        NRF_UART0->TASKS_STOPTX = 1;
        while(NRF_UART0->TASKS_STOPTX);
        NRF_UART0->ENABLE = 0;
        while((NRF_UART0->ENABLE));

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
        NRF_UART0->EVENTS_RXDRDY = 0;
        NRF_UART0->ERRORSRC = 0xFF;
        NRF_UART0->EVENTS_ERROR = 0;
        NRF_UART0->ENABLE = 4;
        while(!(NRF_UART0->ENABLE));
        NRF_UART0->TASKS_STARTRX = 1;
        status |= RX_CONFIGURED;
    }
    else if (enable == 0 && status & RX_CONFIGURED)
    {
        NRF_UART0->TASKS_STOPRX = 1;
        while(NRF_UART0->TASKS_STOPRX);
        NRF_UART0->ENABLE = 0;
        while((NRF_UART0->ENABLE));
        NRF_UART0->PSELRXD = 0xFFFFFFFF;
        status &= ~RX_CONFIGURED;
    }
}

ZSingleWireSerial::ZSingleWireSerial(Pin& p) : DMASingleWireSerial(p)
{
    if (instance == NULL)
        instance = this;

    status = 0;

    buffer = NULL;
    bufferIdx = 0;
    bufferLength = 0;

    NRF_GPIO->DIR |= (1 << DBG_PIN);
    NRF_GPIO->OUTCLR |= (1 << DBG_PIN);

    NRF_GPIO->DIR |= (1 << DBG_PIN2);
    NRF_GPIO->OUTCLR |= (1 << DBG_PIN2);

    NRF_UART0->CONFIG = 0;

    // these lines are disabled
    NRF_UART0->PSELCTS = 0xFFFFFFFF;
    NRF_UART0->PSELRTS = 0xFFFFFFFF;

    // this will be set to pin name depending on configure TX/RX
    NRF_UART0->PSELTXD = 0xFFFFFFFF;
    NRF_UART0->PSELRXD = 0xFFFFFFFF;

    setBaud(115200);

    NVIC_SetPriority(UART0_IRQn, 1);
    NVIC_EnableIRQ(UART0_IRQn);

    status |= DEVICE_COMPONENT_RUNNING;
}

int ZSingleWireSerial::putc(char c)
{
    // synchronous write
    if (!(status & TX_CONFIGURED))
        setMode(SingleWireTx);

    while(!NRF_UART0->EVENTS_TXDRDY);
    NRF_UART0->EVENTS_TXDRDY = 0;
    NRF_UART0->TXD = c;
}
int ZSingleWireSerial::getc()
{
    // synchronous read
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

// asynchronous write (WHY NO DMA NORDIC?)
int ZSingleWireSerial::sendDMA(uint8_t* data, int len)
{
    if (!(status & TX_CONFIGURED))
        setMode(SingleWireTx);

    // bug here if len == 1! TODO:
    buffer = data;
    bufferLength = len;
    bufferIdx = 1;

    NRF_UART0->EVENTS_TXDRDY = 0;

    if (len > 1)
        configureTxInterrupt(1);

    NRF_UART0->TXD = data[0];
}

// asynchronous read (WHY NO DMA NORDIC?)
int ZSingleWireSerial::receiveDMA(uint8_t* data, int len)
{
    if (!(status & RX_CONFIGURED))
        setMode(SingleWireRx);

    buffer = data;
    bufferLength = len;
    bufferIdx = 0;

    configureRxInterrupt(1);
}

int ZSingleWireSerial::abortDMA()
{
    configureTxInterrupt(0);
    configureRxInterrupt(0);

    buffer = NULL;
    bufferLength = 0;
    bufferIdx = 0;
}

int ZSingleWireSerial::setBaud(uint32_t baud)
{
    if (baud == 1000000)
        NRF_UART0->BAUDRATE = 0x10000000;
    else if (baud == 38400)
        NRF_UART0->BAUDRATE = 0x009D5000;
    else if (baud == 9600)
        NRF_UART0->BAUDRATE = 0x00275000;
    else
        // 115200
        NRF_UART0->BAUDRATE = 0x01D7E000;

    return DEVICE_OK;
}

uint32_t ZSingleWireSerial::getBaud()
{
    if (NRF_UART0->BAUDRATE == 0x10000000)
        return 1000000;

    if (NRF_UART0->BAUDRATE == 0x009D5000)
        return 38400;

    if (NRF_UART0->BAUDRATE == 0x00275000)
        return 9600;

    if (NRF_UART0->BAUDRATE == 0x01D7E000)
        return 115200;

    return 0;
}

int ZSingleWireSerial::sendBreak()
{
    return DEVICE_NOT_IMPLEMENTED;
}