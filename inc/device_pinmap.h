#ifndef CODAL_PINNAMES_H
#define CODAL_PINNAMES_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    p0  = 0,
    p1  = 1,
    p2  = 2,
    p3  = 3,
    p4  = 4,
    p5  = 5,
    p6  = 6,
    p7  = 7,
    p8  = 8,
    p9  = 9,
    p10 = 10,
    p11 = 11,
    p12 = 12,
    p13 = 13,
    p14 = 14,
    p15 = 15,
    p16 = 16,
    p17 = 17,
    p18 = 18,
    p19 = 19,
    p20 = 20,
    p21 = 21,
    p22 = 22,
    p23 = 23,
    p24 = 24,
    p25 = 25,
    p26 = 26,
    p27 = 27,
    p28 = 28,
    p29 = 29,
    p30 = 30,

    //NORMAL PINS...
    P0_0  = p0,
    P0_1  = p1,
    P0_2  = p2,
    P0_3  = p3,
    P0_4  = p4,
    P0_5  = p5,
    P0_6  = p6,
    P0_7  = p7,

    P0_8  = p8,
    P0_9  = p9,
    P0_10 = p10,
    P0_11 = p11,
    P0_12 = p12,
    P0_13 = p13,
    P0_14 = p14,
    P0_15 = p15,

    P0_16 = p16,
    P0_17 = p17,
    P0_18 = p18,
    P0_19 = p19,
    P0_20 = p20,
    P0_21 = p21,
    P0_22 = p22,
    P0_23 = p23,

    P0_24 = p24,
    P0_25 = p25,
    P0_26 = p26,
    P0_27 = p27,
    P0_28 = p28,
    P0_29 = p29,
    P0_30 = p30,

    //PADS
    PAD3 = p1,
    PAD2 = p2,
    PAD1 = p3,


    //LED MATRIX COLS
    COL1 = p4,
    COL2 = p5,
    COL3 = p6,
    COL4 = p7,
    COL5 = p8,
    COL6 = p9,
    COL7 = p10,
    COL8 = p11,
    COL9 = p12,

    //LED MATRIX ROWS
    ROW1 = p13,
    ROW2 = p14,
    ROW3 = p15,

    //NORMAL PIN (NO SPECIFIED FUNCTIONALITY)
    //PIN_16

    // BUTTON A
    BUTTON_A = p17,


    //NORMAL PIN (NO SPECIFIED FUNCTIONALITY)
    //PIN_18

    //TARGET RESET
    TGT_NRESET = p19,

    //NORMAL PIN (NO SPECIFIED FUNCTIONALITY)
    //PIN_20

    //MASTER OUT SLAVE IN
    MOSI = p21,

    //MASTER IN SLAVE OUT
    MISO = p22,

    //SERIAL CLOCK
    SCK = p23,

    // RX AND TX PINS
    TGT_TX = p24,
    TGT_RX = p25,

    //BUTTON B
    BUTTON_B = p26,

    //ACCEL INTERRUPT PINS (MMA8653FC)
    ACCEL_INT2 = p27,
    ACCEL_INT1 = p28,

    //MAGENETOMETER INTERRUPT PIN (MAG3110)
    MAG_INT1 = p29,

    // Not connected
    NC = 0xFF,

    RX_PIN_NUMBER = TGT_RX,
    TX_PIN_NUMBER = TGT_TX,
    CTS_PIN_NUMBER = 31, //unused  ** REQUIRES A PROPER FIX **
    RTS_PIN_NUMBER = 31, //unused

    // mBed interface Pins
    USBTX = TX_PIN_NUMBER,
    USBRX = RX_PIN_NUMBER,

    LED1    = PAD1,
    LED2    = PAD2,
    LED3    = PAD3,
    LED4    = P0_16,

    //SDA (SERIAL DATA LINE)
    I2C_SDA0 = p30,

    //SCL (SERIAL CLOCK LINE)
    I2C_SCL0 = p0

} PinName;

#ifdef __cplusplus
}
#endif

#endif