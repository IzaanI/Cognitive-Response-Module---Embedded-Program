/*
 * File:   newmain.c
 * Author: Izaan Sunil, Zach, and Ben
 *
 * Created on November 5, 2025, 2:59 PM
 */

#define F_CPU 14745600UL 
#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3

#include "defines.h"
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <avr/io.h>
#include <stdlib.h>
#include <stdbool.h>
#include <util/delay.h>
#include "lcd.h"
#include <avr/interrupt.h>
//#include "uart.h"
/*
 *
 */
FILE lcd_str = FDEV_SETUP_STREAM(lcd_putchar, NULL, _FDEV_SETUP_WRITE);

void init_timer1() {
    TCCR1B |= (1 << WGM12); //CTC mode
    TCNT1 = 0; //initializing timer
    OCR1A = 230; //preloading timer so that it can count with 1 sec 
    TIMSK1 |= (1 << OCIE1A); //Timer compare A Match interrupt
    TCCR1B |= (1 << CS11) | (1 << CS10); //Starting timer and clock prescaler (64)
}

volatile uint16_t ms = 0;
volatile uint16_t s = 0;
volatile uint16_t score;
volatile uint16_t hiscore1 = 0;
volatile uint16_t hiscore2 = 0;

ISR(TIMER1_COMPA_vect) {
    ms++;
    if (ms >= 1000) {
        ms = 0;
        s++;
    }
}

int checkInput() {
    uint16_t x;
    uint16_t y;
    uint16_t addata;
    ADMUX = 0b01000100; //external reference, ADLAR right aligned, ADC4 channel
    ADCSRA |= (1 << ADIF); //clear the ADIF flag bit
    ADCSRA |= (1 << ADSC); //start to convert, ADSC defined in "avr/io.h"
    while (!(ADCSRA & (1 << ADIF))); //using query instead of interrupt, wait for conversion complete

    addata = ADCL;
    x = addata + ADCH * 256; //combine ADCL and ADCH values, 0b100000000=1*256
    ADCSRA |= (1 << ADIF); ////clear the ADIF flag bit
    //now switch to adc5  
    ADMUX = 0b01000101; //external reference, ADLAR right aligned, ADC5 channel
    ADCSRA |= (1 << ADIF); //clear the ADIF flag bit
    ADCSRA |= (1 << ADSC); //start to convert, ADSC defined in "avr/io.h"
    while (!(ADCSRA & (1 << ADIF))); //using query instead of interrupt, wait for conversion complete

    addata = ADCL;
    y = addata + ADCH * 256; //combine ADCL and ADCH values, 0b100000000=1*256
    ADCSRA |= (1 << ADIF); ////clear the ADIF flag bit

    if (x >= 900) {
        //up
        return UP;
    }
    if (x <= 100) {
        //down
        return DOWN;
    }
    if (y <= 100) {
        //left
        return LEFT;
    }
    if (y >= 900) {
        //right
        return RIGHT;
    }
    return -1;
}

void mode1() {
    uint16_t rtime [100];
    uint16_t totalTime = 0;
    uint16_t avgTime = 0;
    ms = 0;
    s = 0;
    fprintf(&lcd_str, "\x1b\x01");
    while (s < 3) {
        fprintf(&lcd_str, "%d", 3 - s);
        while (ms < 999);
        fprintf(&lcd_str, "\x1b\x01");
    }

    for (int i = 0; i < 100; i++) {
        rtime[i] = 0;
    }

    uint16_t limit = 10000;
    score = 0;
    int r = rand() % 4;
    int in = 0;
    fprintf(&lcd_str, "\x1b\x01");
    direction(r);
    fprintf(&lcd_str, "\x1b\xc0");
    s = 0;
    ms = 0;
    while (s * 1000 + ms < limit) {
        fprintf(&lcd_str, "Score:%d", score);
        fprintf(&lcd_str, " Time=%2d", (limit - s * 1000 - ms) / 1000);
        if (!((PINB & (1 << PB1)) / 2)) {
            break;
        }
        in = checkInput();
        if (in == r) {
            rtime[score] = s * 1000 + ms;
            score++;
            limit = 0.95 * limit;
            r = rand() % 4;
            while (checkInput() != -1) {
                fprintf(&lcd_str, "\x1b\x01");
                direction(-1);
                fprintf(&lcd_str, "\x1b\xc0");
                fprintf(&lcd_str, "%d", score); // score is supposed to be printed here
                ms = 0;
                s = 0;
            }

            fprintf(&lcd_str, "\x1b\xc0");
            fprintf(&lcd_str, "\x1b\x01");
            direction(r);
        } else if (in != r && in != -1) {
            break;
        }
        fprintf(&lcd_str, "\x1b\xc0");
    }
    if (score > 0) {
        fprintf(&lcd_str, "\x1b\xc0");
        fprintf(&lcd_str, "\x1b\x01");
        if (score > hiscore1) {
            hiscore1 = score;
        }
        // calculate avg reaction time
        for (int i = 0; i < score; i++) {
            totalTime += rtime[i];
        }

        avgTime = totalTime / score;
        fprintf(&lcd_str, "Avg Rxn T: %d", avgTime);
        fprintf(&lcd_str, "\x1b\xc0");
        fprintf(&lcd_str, "Score = %d", score);
    }
    _delay_ms(2000);
    return;
}

void mode2() {
    ms = 0;
    s = 0;
    fprintf(&lcd_str, "\x1b\x01");
    while (s < 3) {
        fprintf(&lcd_str, "%d", 3 - s);
        while (ms < 999);
        fprintf(&lcd_str, "\x1b\x01");
    }
    uint16_t cdir [100];
    score = 0;
    cdir[score] = rand() % 4;
    int in = 0;
    int j = 0;
    fprintf(&lcd_str, "\x1b\x01");
    while (1) {
        while (j <= score) {
            direction(cdir[j]);
            j++;
            _delay_ms(500);
            fprintf(&lcd_str, "\x1b\x01");
            _delay_ms(100);
        }

        fprintf(&lcd_str, "Go!");
        _delay_ms(500);
        fprintf(&lcd_str, "\x1b\x01");

        j = 0;
        while (j <= score) {
            in = checkInput();
            if (!((PINB & (1 << PB1)) / 2)) {
                return;
            }
            if (in == cdir[j]) {
                j++;
                while (checkInput() != -1);
            } else if (in != cdir[j] && in != -1) {
                fprintf(&lcd_str, "\x1b\xc0");
                fprintf(&lcd_str, "Score = %d", score);
                _delay_ms(2000);
                return;
            }
        }
        score++;
        if (score > hiscore2) {
            hiscore2 = score;
        }
        j = 0;
        cdir[score] = rand() % 4;
    }
    return;
}

void direction(int d) {
    switch (d) {
        case UP:
            fprintf(&lcd_str, "^");
            break;
        case DOWN:
            fprintf(&lcd_str, "v");
            break;
        case LEFT:
            fprintf(&lcd_str, "<");
            break;
        case RIGHT:
            fprintf(&lcd_str, ">");
            break;
        default:
            fprintf(&lcd_str, " ");
            break;
    }
}

int menu() {
    fprintf(&lcd_str, "\x1b\x01");
    fprintf(&lcd_str, "Mode 1 <-- HS:%2d", hiscore1);
    fprintf(&lcd_str, "\x1b\xc0");
    fprintf(&lcd_str, "Mode 2 --> HS:%2d", hiscore2);
    while (1) {
        if (checkInput() == LEFT) {
            return 1;
        }
        if (checkInput() == RIGHT) {
            return 2;
        }
    }
    return 0;
}

int main(int argc, char** argv) {

    sei();

    lcd_init(); // this should be called once before using lcd (read lcd.c for details)

    DDRC &= ~(1 << PC4); //input without pull-up resistor
    PORTC &= ~(1 << PC4);

    DDRC &= ~(1 << PC5); //input without pull-up resistor
    PORTC &= ~(1 << PC5);

    //switch on
    DDRB &= ~(1 << PB1);
    PORTB |= (1 << PB1);

    unsigned char xchar[5];
    unsigned char ychar[5];
    unsigned char switchStatus[1];

    ADCSRA = 0x87; //ADC enable, no auto trigger, no interrupt, start no conversion, prescaler 128
    DIDR0 = 0x00;

    init_timer1();
    while (1) {
        switch (menu()) {
            case 1:
                mode1();
                break;
            case 2:
                mode2();
                break;
        }
    }
    return (EXIT_SUCCESS);
}