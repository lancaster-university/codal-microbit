#include "nrf51_interrupts.h"
#include "codal_target_hal.h"

// Override the default interrupt handlers.
extern "C"
{
void POWER_CLOCK_IRQHandler(){target_panic(1);}
void RADIO_IRQHandler(){target_panic(2);}
void UART0_IRQHandler(){target_panic(3);}
void SPI0_TWI0_IRQHandler(){target_panic(4);}
void SPI1_TWI1_IRQHandler(){target_panic(5);}
void GPIOTE_IRQHandler(){target_panic(5);}
void ADC_IRQHandler(){target_panic(6);}
void TIMER0_IRQHandler(){target_panic(7);}
void TIMER1_IRQHandler(){target_panic(8);}
void TIMER2_IRQHandler(){target_panic(9);}
void RTC0_IRQHandler(){target_panic(10);}
void TEMP_IRQHandler(){target_panic(11);}
void RNG_IRQHandler(){target_panic(12);}
void ECB_IRQHandler(){target_panic(13);}
void CCM_AAR_IRQHandler(){target_panic(14);}
void WDT_IRQHandler(){target_panic(15);}
void RTC1_IRQHandler(){target_panic(16);}
void QDEC_IRQHandler(){target_panic(17);}
void LPCOMP_IRQHandler(){target_panic(18);}
void SWI0_IRQHandler(){target_panic(19);}
void SWI1_IRQHandler(){target_panic(20);}
void SWI2_IRQHandler(){target_panic(21);}
void SWI3_IRQHandler(){target_panic(22);}
void SWI4_IRQHandler(){target_panic(23);}
void SWI5_IRQHandler(){target_panic(24);}
}