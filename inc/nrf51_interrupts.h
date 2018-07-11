#ifndef NRF51_INTERRUPT_H
#define NRF51_INTERRUPT_H

extern "C"
{
void POWER_CLOCK_IRQHandler() __attribute__((weak));
void RADIO_IRQHandler() __attribute__((weak));
void UART0_IRQHandler() __attribute__((weak));
void SPI0_TWI0_IRQHandler() __attribute__((weak));
void SPI1_TWI1_IRQHandler() __attribute__((weak));
void GPIOTE_IRQHandler() __attribute__((weak));
void ADC_IRQHandler() __attribute__((weak));
void TIMER0_IRQHandler() __attribute__((weak));
void TIMER1_IRQHandler() __attribute__((weak));
void TIMER2_IRQHandler() __attribute__((weak));
void RTC0_IRQHandler() __attribute__((weak));
void TEMP_IRQHandler() __attribute__((weak));
void RNG_IRQHandler() __attribute__((weak));
void ECB_IRQHandler() __attribute__((weak));
void CCM_AAR_IRQHandler() __attribute__((weak));
void WDT_IRQHandler() __attribute__((weak));
void RTC1_IRQHandler() __attribute__((weak));
void QDEC_IRQHandler() __attribute__((weak));
void LPCOMP_IRQHandler() __attribute__((weak));
void SWI0_IRQHandler() __attribute__((weak));
void SWI1_IRQHandler() __attribute__((weak));
void SWI2_IRQHandler() __attribute__((weak));
void SWI3_IRQHandler() __attribute__((weak));
void SWI4_IRQHandler() __attribute__((weak));
void SWI5_IRQHandler() __attribute__((weak));
}
#endif