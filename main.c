#include <msp430g2553.h>

#define RS BIT0
#define EN BIT1
#define D4 BIT2
#define D5 BIT3
#define D6 BIT4
#define D7 BIT5

#define HYSTERESIS 1

volatile unsigned int adc_result = 0;
volatile int ref = 25;
unsigned int temp = 0;

void delay_ms(unsigned int ms);
void lcd_cmd(unsigned char cmd);
void lcd_data(unsigned char data);
void lcd_init(void);
void lcd_send_nibble(unsigned char nibble);
void lcd_puts(const char *str);
void lcd_clear_line_2(void);
void adc_init(void);

void main(void) {
    WDTCTL = WDTPW | WDTHOLD;
    DCOCTL = CALDCO_1MHZ;
    BCSCTL1 = CALBC1_1MHZ;

    P2DIR |= RS + EN + D4 + D5 + D6 + D7;
    P2OUT &= ~(RS + EN + D4 + D5 + D6 + D7);

    P1DIR |= BIT0 + BIT2 + BIT3;
    P1OUT &= ~(BIT0 + BIT2 + BIT3);

    P1DIR &= ~(BIT4 + BIT5);
    P1REN |= BIT4 + BIT5;
    P1OUT |= BIT4 + BIT5;
    P1IE  |= BIT4 + BIT5;
    P1IES |= BIT4 + BIT5;
    P1IFG &= ~(BIT4 + BIT5);

    adc_init();
    lcd_init();
    __enable_interrupt();

    static unsigned int blink_counter = 0;

    while (1) {
        P1IE &= ~(BIT4 + BIT5);
        ADC10CTL0 |= ENC + ADC10SC;
        while (ADC10CTL1 & ADC10BUSY);
        adc_result = ADC10MEM;
        P1IE |= BIT4 + BIT5;

        temp = (unsigned int)(adc_result * 0.146484375);

        if (temp < (ref - HYSTERESIS)) {
            P1OUT |= BIT2;
            P1OUT &= ~BIT3;
        } else if (temp > (ref + HYSTERESIS)) {
            P1OUT |= BIT3;
            P1OUT &= ~BIT2;
        }

        lcd_cmd(0x80);
        lcd_puts("Temp: ");
        lcd_data((temp / 10) + '0');
        lcd_data((temp % 10) + '0');
        lcd_data('C');

        lcd_clear_line_2();
        lcd_cmd(0xC0);
        lcd_puts("Ref: ");
        if (ref < 0) {
            lcd_data('-');
            lcd_data((-ref / 10) + '0');
            lcd_data((-ref % 10) + '0');
        } else {
            lcd_data((ref / 10) + '0');
            lcd_data((ref % 10) + '0');
        }

        if (++blink_counter >= 20) {
            P1OUT |= BIT0;
            __delay_cycles(100000);
            P1OUT &= ~BIT0;
            blink_counter = 0;
        }

        delay_ms(500);
    }
}

void adc_init(void) {
    ADC10CTL1 = INCH_6;
    ADC10AE0 = BIT6;
    ADC10CTL0 = SREF_1 + REFON + ADC10SHT_3 + ADC10ON;
}

#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void) {
    if (P1IFG & BIT4) {
        ref++;
        P1IFG &= ~BIT4;
    }
    if (P1IFG & BIT5) {
        ref--;
        P1IFG &= ~BIT5;
    }
    __delay_cycles(50000);
}

void delay_ms(unsigned int ms) {
    while (ms--) {
        __delay_cycles(1000);
    }
}

void lcd_send_nibble(unsigned char nibble) {
    P2OUT = (P2OUT & ~(D4 + D5 + D6 + D7)) | ((nibble & 0x0F) << 2);
    P2OUT |= EN;
    delay_ms(1);
    P2OUT &= ~EN;
}

void lcd_cmd(unsigned char cmd) {
    P2OUT &= ~RS;
    lcd_send_nibble(cmd >> 4);
    lcd_send_nibble(cmd & 0x0F);
    delay_ms(2);
}

void lcd_data(unsigned char data) {
    P2OUT |= RS;
    lcd_send_nibble(data >> 4);
    lcd_send_nibble(data & 0x0F);
    delay_ms(2);
}

void lcd_init(void) {
    delay_ms(20);
    lcd_cmd(0x02);
    lcd_cmd(0x28);
    lcd_cmd(0x0C);
    lcd_cmd(0x06);
    lcd_cmd(0x01);
    delay_ms(2);
}

void lcd_puts(const char *str) {
    while (*str) {
        lcd_data(*str++);
    }
}

void lcd_clear_line_2(void) {
    lcd_cmd(0xC0);
    int i;
    for (i = 0; i < 16; i++) {
        lcd_data(' ');
    }
}
