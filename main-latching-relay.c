#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/wdt.h>

#include <util/delay.h>

// Pin Definitions
#define RELAY_SET PB0
#define RELAY_RESET PB1
#define BTN_PIN PB2

// Debug Pins (Optional)
// These pins can be used to monitor the watchdog check-ins and relay state via
// an LED or oscilloscope.
#define WATCHDOG_STATUS_DBG_PIN PB3
#define RELAY_STATUS_DBG_PIN PB4

// State Tracking
volatile uint8_t wdt_checkin_flag = 0;
volatile uint8_t btn_pressed_flag = 0;
uint8_t relay_is_set = 0;

// Store last known relay state in EEPROM
uint8_t EEMEM stored_relay_state;

// Do not call this function directly from the ISR.
static void pulse_relay_pin(uint8_t pin) {
  PORTB |= (1 << pin);
  _delay_ms(10);
  PORTB &= ~(1 << pin);
}

static void apply_relay_state(uint8_t new_state) {
  relay_is_set = new_state;
  if (relay_is_set > 1) {
    relay_is_set = 0;
  }

  if (relay_is_set) {
    pulse_relay_pin(RELAY_SET);
    PORTB |= (1 << RELAY_STATUS_DBG_PIN);
  } else {
    pulse_relay_pin(RELAY_RESET);
    PORTB &= ~(1 << RELAY_STATUS_DBG_PIN);
  }
  eeprom_update_byte(&stored_relay_state, relay_is_set);
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
  uint8_t current_read = (PINB & (1 << BTN_PIN)) ? 1 : 0;
  btn_history = (btn_history << 1) | current_read;

  // Evaluate debounced state
  if (btn_history == 0x00 && btn_state == 1) {
    // Switch has been solidly CLOSED (low) for 80ms
    btn_state = 0;
    btn_pressed_flag = 1; // Trigger relay transition on CLOSE
  } else if (btn_history == 0xFF && btn_state == 0) {
    // Switch has been solidly OPENED (high) for 80ms
    btn_state = 1; // State updated, but no action flagged
  }
}

int main(void) {
  // --- I/O Initialization ---
  DDRB =
      (1 << RELAY_SET) | (1 << RELAY_RESET) | (1 << WATCHDOG_STATUS_DBG_PIN) |
      (1 << RELAY_STATUS_DBG_PIN); // Enable status LED and relay pins as output
  PORTB = (1 << BTN_PIN);          // Enable pull-up on PB2 input

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
  relay_is_set = eeprom_read_byte(&stored_relay_state);

  // Sanity check (EEPROM defaults to 0xFF on fresh chips)
  if (relay_is_set > 1) {
    relay_is_set = 0; // Default to 0 if invalid
  }

  // Re-apply saved state on boot.
  apply_relay_state(relay_is_set);

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
      apply_relay_state(!relay_is_set);
    }
  }

  return 0;
}
