// define CPU speed - actual speed is set using CLKPSR in main()
#define F_CPU 8000000UL
#include <stdint.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>

const uint8_t loop_ddrb[] PROGMEM = {
  0b011, // d1
  0b011, // d2
  0b101, // d3
  0b101, // d4
  0b110, // d5
  0b110, // d6
};

const uint8_t loop_portb[] PROGMEM = {
  0b001,
  0b010,
  0b100,
  0b001,
  0b010,
  0b100,
};

// len(sin_vals)-1
#define SIN_FRAMES 4
// num(leds)
#define NUM_LEDS 6
// len(d_status)
#define NUM_SINE 2
const uint8_t sin_vals[] PROGMEM = {
  0, 49,141,212,255
};
int8_t d_status[] = {
  4,
  4,
};

volatile uint8_t d_pwm[] = {
  0,
  0,
  255,
  0,
  255,
  255,
};
volatile uint8_t  currLed = 0;
volatile uint8_t  pwmCount = 0;
volatile uint8_t  loop_portb_mem = 0;
volatile uint16_t counter0 = 0;
volatile uint8_t mode_count = 0;
volatile uint8_t mode = 0;
const uint16_t colorInterval = 2048;

int main() {
  // CCP register must first be written with the correct signature - 0xD8
  CCP = 0xD8;
  //  CLKPS[3:0] sets the clock division factor
  CLKPSR = 0;

  // set initial values
  for(uint8_t i = 0; i < NUM_SINE; i++) {
    int8_t state = d_status[i];
    d_pwm[i] = pgm_read_byte(&sin_vals[state<0?-state:state]);
  }

  // main loop
  while (1) {
    for(uint8_t currLed = 0; currLed < NUM_LEDS; currLed++) {
      PORTB = 0;
      DDRB = pgm_read_byte(&loop_ddrb[currLed]);
      loop_portb_mem = pgm_read_byte(&loop_portb[currLed]);
      if(pwmCount < d_pwm[currLed]) {
        PORTB = loop_portb_mem;
      } else {
        PORTB = 0;
      }
    }
    // increment PWM count
    pwmCount++;
    // increment interval counter
    counter0++;
    if(counter0 % colorInterval == 0) {
      for(uint8_t i = 0; i < NUM_SINE; i++) {
        int8_t state = d_status[i];
        state = state == -1? 0 : state == SIN_FRAMES? -(state-1) : state+1;
        d_status[i] = state;
        d_pwm[i] = pgm_read_byte(&sin_vals[state<0?-state:state]);
      }
      mode++;
      switch(mode) {
      case 1:
        d_pwm[2] = 0;
        d_pwm[3] = 255;
        break;
      default:
        mode = 0;
        d_pwm[2] = 255;
        d_pwm[3] = 0;
      }
    }
  }
  return 0;
}
