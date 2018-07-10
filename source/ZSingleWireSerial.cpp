#include "ZSingleWireSerial.h"
#include "nrf51.h"
#include "nrf51_bitfields.h"

using namespace codal;

#define TX_CONFIGURED       0x02
#define RX_CONFIGURED       0x04

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
        NRF_UART0->PSELTXD = 0xFFFFFFFF;
        NRF_UART0->TASKS_STOPTX = 1;
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
        NRF_UART0->TASKS_STOPRX = 1;
        NRF_UART0->PSELRXD = 0xFFFFFFFF;
        status &= ~RX_CONFIGURED;
    }
}

ZSingleWireSerial::ZSingleWireSerial(Pin& p) : DMASingleWireSerial(p)
{
    status = 0;

    // these lines are disabled
    NRF_UART0->PSELCTS = 0xFFFFFFFF;
    NRF_UART0->PSELRTS = 0xFFFFFFFF;

    // this will be set to pin name depending on configure TX/RX
    NRF_UART0->PSELTXD = 0xFFFFFFFF;
    NRF_UART0->PSELRXD = 0xFFFFFFFF;

    NRF_UART0->ENABLE = 4;
    while(!(NRF_UART0->ENABLE));

    NRF_UART0->EVENTS_RXDRDY = 0;
    // dummy write needed or TXDRDY trails write rather than leads write.
    //  pins are disconnected so nothing is physically transmitted on the wire
    NRF_UART0->TXD = 0;

    NVIC_SetPriority(UART0_IRQn, 3);
    NVIC_EnableIRQ(UART0_IRQn);
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

}

int ZSingleWireSerial::receive(uint8_t* data, int len)
{

}

int ZSingleWireSerial::sendDMA(uint8_t* data, int len)
{

}
int ZSingleWireSerial::receiveDMA(uint8_t* data, int len)
{

}
int ZSingleWireSerial::abortDMA()
{

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

int ZSingleWireSerial::sendBreak()
{
    return DEVICE_NOT_IMPLEMENTED;
}