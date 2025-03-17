#include <Adafruit_NeoPixel.h>

#define CLK_PIN 2
#define DT_PIN 3
#define SW_PIN 4
#define LED_PIN 8
#define NUM_LEDS 10
#define DEBOUNCE_DELAY 5 // Adjust this value as needed

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

volatile int last_position = 0;
volatile int n = 0;
volatile bool direction = true; // true for forward, false for backward
int led_position = 0;
bool taster = LOW;
bool last_taster = LOW;
unsigned long lastDebounceTime = 0;

void setup() {
  pinMode(CLK_PIN, INPUT_PULLUP);
  pinMode(DT_PIN, INPUT_PULLUP);
  pinMode(SW_PIN, INPUT_PULLUP);
  Serial.begin(9600);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

  attachInterrupt(digitalPinToInterrupt(CLK_PIN), readEncoder, CHANGE);
}

void loop() {
  taster = !digitalRead(SW_PIN);
  if (taster != last_taster) {
    if (taster) {
      // Send click command
      Serial.println("CLICK");
    }
    delay(10);
    last_taster = taster;
  }
}

void readEncoder() {
  unsigned long currentTime = millis();
  if ((currentTime - lastDebounceTime) > DEBOUNCE_DELAY) {
    n = digitalRead(CLK_PIN);
    if ((last_position == 0) && (n == HIGH)) {
      if (digitalRead(DT_PIN) == LOW) {
        // Send up command
        Serial.println("UP");
        direction = false; // Invert direction for UP
      } else {
        // Send down command
        Serial.println("DOWN");
        direction = true; // Invert direction for DOWN
      }
      updateLEDs();
    }
    last_position = n;
    lastDebounceTime = currentTime;
  }
}

void updateLEDs() {
  // Turn off all LEDs
  for (int i = 0; i < NUM_LEDS; i++) {
    strip.setPixelColor(i, strip.Color(0, 0, 0)); // Off
  }
  // Update LED position
  if (direction) {
    led_position = (led_position + 1) % NUM_LEDS;
  } else {
    led_position = (led_position - 1 + NUM_LEDS) % NUM_LEDS;
  }
  // Turn on the current LED with orange color
  strip.setPixelColor(led_position, strip.Color(255, 165, 0)); // Orange
  strip.show();
}