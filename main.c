#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/wdt.h>

#include <util/delay.h>

// Pinout reminder:

// ATtiny85 (DIP-8) physical pin mapping used by this firmware:
//   Pin 1 -> PB5/RESET (not used here)
//   Pin 2 -> PB3 = WATCHDOG_STATUS_DBG_PIN
//   Pin 3 -> PB4 = FX_STATUS_DBG_PIN
//   Pin 4 -> GND
//   Pin 5 -> PB0 = FX_ACTIVE_OUT
//   Pin 6 -> PB1 (not used here)
//   Pin 7 -> PB2 = MOMENTARY_BUTTON_IN (active-low, pull-up enabled)
//   Pin 8 -> VCC

// Pin Definitions
#define FX_ACTIVE_OUT PB0
#define MOMENTARY_BUTTON_IN PB2

// Debug Pins (Optional)
// These pins can be used to monitor the watchdog check-ins and fx active state
// via an LED or oscilloscope.
#define WATCHDOG_STATUS_DBG_PIN PB3
#define FX_STATUS_DBG_PIN PB4

// State Tracking
volatile uint8_t wdt_checkin_flag = 0;
volatile uint8_t btn_pressed_flag = 0;
uint8_t effect_is_active = 0;

// Store last known fx active state in EEPROM
uint8_t EEMEM stored_fx_active_state;

// Do not call this from an ISR.
static void apply_effect_state(uint8_t new_state) {
  effect_is_active = new_state;
  if (effect_is_active > 1) {
    effect_is_active = 0;
  }

  if (effect_is_active) {
    PORTB |= (1 << FX_ACTIVE_OUT);
    PORTB |= (1 << FX_STATUS_DBG_PIN);
  } else {
    PORTB &= ~(1 << FX_ACTIVE_OUT);
    PORTB &= ~(1 << FX_STATUS_DBG_PIN);
  }
  eeprom_update_byte(&stored_fx_active_state, effect_is_active);
}

// Timer0 Compare A Interrupt - Fires every 10ms
ISR(TIMER0_COMPA_vect) {
  static uint8_t wdt_counter = 0;
  static uint8_t btn_history = 0xFF; // 8-bit history for debouncing
  static uint8_t btn_state = 1;      // 1 = released (pull-up high), 0 = pressed

  // 1. Handle the Quarter-Second Watchdog Tick
  wdt_counter++;
  if (wdt_counter >= 25) { // 25 ticks * 10ms = 250ms
    wdt_counter = 0;
    wdt_checkin_flag = 1; // Flag main loop to pat the dog
  }

  // 2. Handle Button Debouncing (Shift Register Method)
  // Read PB2, isolate the bit, and shift it into our history byte
  uint8_t current_read = (PINB & (1 << MOMENTARY_BUTTON_IN)) ? 1 : 0;
  btn_history = (btn_history << 1) | current_read;

  // Evaluate debounced state
  if (btn_history == 0x00 && btn_state == 1) {
    // Switch has been solidly CLOSED (low) for 80ms
    btn_state = 0;
    btn_pressed_flag = 1; // Trigger fx active transition on CLOSE
  } else if (btn_history == 0xFF && btn_state == 0) {
    // Switch has been solidly OPENED (high) for 80ms
    btn_state = 1; // State updated, but no action flagged
  }
}

int main(void) {
  // --- I/O Initialization ---
  DDRB = (1 << FX_ACTIVE_OUT) | (1 << WATCHDOG_STATUS_DBG_PIN) |
         (1 << FX_STATUS_DBG_PIN);    // Enable as output
  PORTB = (1 << MOMENTARY_BUTTON_IN); // Enable pull-up on PB2 input

  // --- Watchdog Setup ---
  // Enable watchdog with a 1-second timeout
  wdt_enable(WDTO_1S);

  // --- Timer0 Setup (10ms tick) ---
  // Assuming default 1MHz internal clock:
  // Prescaler 64 -> 1,000,000 / 64 = 15,625 Hz
  // Target: 100 Hz (10ms). 15,625 / 100 = 156.25 cycles.
  // Set OCR0A to 155 (0-indexed).
  TCCR0A = (1 << WGM01);              // CTC Mode
  TCCR0B = (1 << CS01) | (1 << CS00); // Prescaler = 64
  OCR0A = 155;                        // Compare match value
  TIMSK = (1 << OCIE0A);              // Enable Timer0 Compare Match A Interrupt

  // Read last known state from EEPROM
  effect_is_active = eeprom_read_byte(&stored_fx_active_state);

  // Sanity check (EEPROM defaults to 0xFF on fresh chips)
  if (effect_is_active > 1) {
    effect_is_active = 0; // Default to 0 if invalid
  }

  // Re-apply saved state on boot.
  apply_effect_state(effect_is_active);

  // --- Sleep Setup ---
  set_sleep_mode(SLEEP_MODE_IDLE);
  sleep_enable();
  sei(); // Enable global interrupts

  // --- Main Loop ---
  while (1) {
    // CPU enters IDLE mode. It will wake up every 10ms when Timer0 fires.
    sleep_cpu();

    // After waking up, check our flags:
    if (wdt_checkin_flag) {
      wdt_checkin_flag = 0;
      wdt_reset();                             // Quarter-second check-in
      PORTB ^= (1 << WATCHDOG_STATUS_DBG_PIN); // Toggle the status LED
    }

    if (btn_pressed_flag) {
      btn_pressed_flag = 0;
      apply_effect_state(!effect_is_active);
    }
  }

  return 0;
}
