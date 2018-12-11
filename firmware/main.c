
/*
 * Weller solder iron auto off
 * Copyright 2018 Jorgen Birkler
 * jorgen.birkler)a(gmail.com
 * License: MIT (see License.txt) or proprietary (contact author)
 */

#include <avr/io.h>

#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include <avr/sleep.h>


#define RELAY_COIL1_OFF() PORTB &= ~_BV(PB4)
#define RELAY_COIL1_ON() PORTB |= _BV(PB4)
#define RELAY_COIL1_INIT() DDRB |= _BV(PB4);RELAY_COIL1_OFF()

#define RELAY_COIL2_OFF() PORTB &= ~_BV(PB3)
#define RELAY_COIL2_ON() PORTB |= _BV(PB3)
#define RELAY_COIL2_INIT() DDRB |= _BV(PB3);RELAY_COIL2_OFF()

#define SWITCH_IS_PRESSED() bit_is_clear(PINB,1)
#define SWITCH_INIT() DDRB &=~_BV(PB1);PORTB|=_BV(PB1)

static void relay_on() {
    RELAY_COIL2_OFF();
    RELAY_COIL1_ON();
    _delay_ms(400);
    RELAY_COIL1_OFF();
}


static void relay_off() {
    RELAY_COIL1_OFF();
    RELAY_COIL2_ON();
    _delay_ms(400);
    RELAY_COIL2_OFF();
}

static void relay_init() {
    RELAY_COIL1_INIT();
    RELAY_COIL2_INIT();
}


int main(void)
{
    SWITCH_INIT();
    relay_init();
    wdt_enable(WDTO_2S);

    uint8_t led_on = 1;
    uint8_t led_on_prev = 1;
    uint8_t pressed_prev = 1;
    TCNT0 = 0;

    TCCR0B =  _BV(CS02);//_BV(CS00) |  256x 
    TIMSK |= _BV(TOIE0);

    uint32_t cnt = 0;

    relay_on();

    set_sleep_mode(SLEEP_MODE_IDLE);
    
    //sei();

    //0.5 hour
    const float timeout_hours = 0.5;
    const int timer0_prescaler = 256;
    const int timer0_overflow_count = 256;
    const uint32_t timer_count = 100000.0f * (float)timer0_prescaler * (float)timer0_overflow_count / (float)F_CPU;



    for (;;) {
        wdt_reset();
        //sleep_mode();
        
        if ((TIFR & _BV(TOV0))) {
            TIFR |= _BV(TOV0);
            cnt++;
            uint8_t pressed = SWITCH_IS_PRESSED();
            if (pressed_prev != pressed) {
                pressed_prev = pressed;
                if (pressed) {
                    //led_on ^= 0x1;
                    relay_off();
                    _delay_ms(1000);
                }
            }



            if (cnt >= timer_count) {
                cnt = 0;
                relay_off();
            }

            if (led_on_prev != led_on) {
                led_on_prev =  led_on;
                if (led_on & 1) {
                    relay_on();
                } else {
                    relay_off();
                }
            }
        }

    }
	return 0;
}

/* ------------------------------------------------------------------------- */

