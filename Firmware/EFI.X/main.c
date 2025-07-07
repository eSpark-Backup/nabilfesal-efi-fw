//#include <xc.h>
//#include <stdio.h>
//
//#define _XTAL_FREQ 4000000
//
//// CONFIGURATION BITS
//#pragma config FOSC = INTRC_NOCLKOUT
//#pragma config WDTE = OFF, PWRTE = OFF, MCLRE = OFF
//#pragma config CP = OFF, CPD = OFF, BOREN = OFF, IESO = OFF
//#pragma config FCMEN = OFF, LVP = OFF
//#pragma config BOR4V = BOR40V
//#pragma config WRT = OFF
//
////#define BAUD 9600
////#define SPBRG_VAL ((_XTAL_FREQ / (16UL * BAUD)) - 1)
////
//void putch(char data) {
//    while (!TXIF);
//    TXREG = data;
//}
////
////void USART_Init() {
////    TRISC6 = 1; // TX
////    TRISC7 = 1; // RX
////
////    SPBRG = (unsigned char)SPBRG_VAL;
////    BRGH = 1;
////    SYNC = 0;
////    SPEN = 1;
////
////    TXEN = 1;
////    CREN = 1;
////}
//
//// Function to initialize ADC
//void ADC_Init(void) {
//    ANSEL = 0x00;         // Clear analog selects on PORTA
//    ANSELH = 0x40;        // Set AN6 (RE1) as analog input
//    TRISE1 = 1;           // Set RE1 as input
//
//    ADCON0 = 0b00011001;  // Channel AN6 (RE1), ADC ON
//    // bit 5-2: 0110 for AN6
//    // bit 1: GO/DONE (auto cleared)
//    // bit 0: ADC ON
//
//    ADCON1 = 0b10000000;  // Right justified, Vref+ = VDD, Vref- = VSS
//}
//
//// Function to read ADC value
//unsigned int ADC_Read(void) {
//    ADCON0bits.GO = 1;              // Start conversion
//    while (ADCON0bits.GO);          // Wait for conversion to complete
//    return ((ADRESH << 8) + ADRESL); // Combine 10-bit result
//}
//
//void main() {
////    USART_Init();
//    ADC_Init();
////    printf("Starting...\r\n");
//    
//    unsigned int adc_value;
//    
//    while (1) {
//        adc_value = ADC_Read();
//        // Now adc_value is from 0?1023 corresponding to 0?5V
//        float voltage = adc_value * 5.0 / 1023.0;
//        printf("ADC VALUE: %.2f\r\n", voltage);
//        __delay_ms(500);
//    }
//}

#include <xc.h>
#include "components/LED/led.h"
#include "components/Relay/relay.h"
#include "components/Sleep/sleep.h"
#include "components/Battery/battery.h"
#include "components/Declarations/declaration.h"

// CONFIG BITS
#pragma config FOSC = INTRC_NOCLKOUT
#pragma config WDTE = OFF, PWRTE = OFF, MCLRE = OFF
#pragma config CP = OFF, CPD = OFF, BOREN = OFF, IESO = OFF
#pragma config FCMEN = OFF, LVP = OFF
#pragma config BOR4V = BOR40V
#pragma config WRT = OFF

void main(void)
{
    ANSEL = 0;
    ANSELH = 0;
    
    led_init();
    relay_init();
    sleep_init();
    battery_init();
    
    // Clear outputs
    PORTA = 0;
    PORTD = 0;
    
    while (1)
    {
        if(!BATT_SW)
        {
            battery_test();
            sleep_func();
        }
    }
}
