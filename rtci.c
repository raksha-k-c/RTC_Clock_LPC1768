#include <LPC17xx.h>
#include <stdio.h>  // Added for sprintf declaration

void RTC_Init(void);
void UART0_Init(void);
void UART0_SendChar(char c);
void UART0_SendString(char *str);
void display_time(void);

int main(void) {
    volatile int i;  // Moved declaration to beginning of block
    UART0_Init();    // Initialize UART for display
    RTC_Init();      // Initialize RTC

    while (1) {
        display_time();  // Continuously display time via UART
        for (i = 0; i < 1000000; i++);  // Simple delay
    }
}

void RTC_Init(void) {
    LPC_SC->PCONP |= (1 << 9);       // Power up RTC
    LPC_RTC->CCR = 0x02;             // Reset RTC
    LPC_RTC->CIIR = 0x01;            // Interrupt every second (optional)
    LPC_RTC->SEC = 0;
    LPC_RTC->MIN = 0;
    LPC_RTC->HOUR = 12;
    LPC_RTC->DOM = 7;
    LPC_RTC->MONTH = 5;
    LPC_RTC->YEAR = 2025;
    LPC_RTC->DOY = 127;              // Optional
    LPC_RTC->DOW = 3;                // Wednesday, for example
    LPC_RTC->CCR = 0x01;             // Enable RTC
}

void display_time(void) {
    char buffer[64];
    int hr = LPC_RTC->HOUR;
    int min = LPC_RTC->MIN;
    int sec = LPC_RTC->SEC;

    sprintf(buffer, "Time: %02d:%02d:%02d\r\n", hr, min, sec);
    UART0_SendString(buffer);
}

void UART0_Init(void) {
    LPC_SC->PCONP |= (1 << 3);      // Power UART0
    LPC_SC->PCLKSEL0 &= ~(3 << 6);  // CCLK/4
    LPC_PINCON->PINSEL0 |= (1 << 4) | (1 << 6);  // P0.2 = TXD0, P0.3 = RXD0

    LPC_UART0->LCR = 0x83;          // 8-bit, enable DLAB
    LPC_UART0->DLM = 0;
    LPC_UART0->DLL = 13;            // 9600 baud @ 25MHz PCLK
    LPC_UART0->LCR = 0x03;          // Disable DLAB
    LPC_UART0->FCR = 0x07;          // Enable FIFO
}

void UART0_SendChar(char c) {
    while (!(LPC_UART0->LSR & 0x20));  // Wait for empty THR
    LPC_UART0->THR = c;
}

void UART0_SendString(char *str) {
    while (*str) {
        UART0_SendChar(*str++);
    }
}