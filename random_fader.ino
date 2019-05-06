#include <FastLED.h>
#define NUM_LEDS 6
CRGB leds[NUM_LEDS];
#define PIN 7

#define MIN_TICKS 20
#define MAX_TICKS 50
#define BRIGHTNESS 255

struct FRGB {
  float r;
  float g;
  float b;
};

#define COLOR_COUNT 6
const CRGB colors_set[COLOR_COUNT] = {
  CRGB(124, 252, 0),
  CRGB(50, 205, 50),
  CRGB(0, 255, 0),
  CRGB(34, 180, 34),
  CRGB(0, 200, 0),
  CRGB(0, 100, 0)
};

CRGB target_colors[NUM_LEDS];
FRGB current_colors_F[NUM_LEDS];
void setup()
{
  FastLED.addLeds<WS2811, PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);

  // setting timer1 to 0.1 second interval
  cli();
  // configuring timer1 to CTC and 1/256 prescaler
  TCCR1A = 0;
  TCCR1B = (1 << WGM12) | (1 << CS12);

  // 0.1 seconds
  OCR1A = 6250;
  TIMSK1 |= (1 << OCIE1A);
  sei();
  
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = colors_set[5];
    target_colors[i] = leds[i];
    current_colors_F[i] = {leds[i].r, leds[i].g, leds[i].b};
  }

  randomSeed(analogRead(0));
}

FRGB color_increments[NUM_LEDS];
int current_ticks_to_target_arr[NUM_LEDS];
int ticks_to_target_arr[NUM_LEDS];
volatile bool timer_tick = false;
void loop() {
  if (timer_tick) {
    for (int i = 0; i < NUM_LEDS; i++) {
      // choose new target color when reached
      if (current_ticks_to_target_arr[i] >= ticks_to_target_arr[i]) {
        target_colors[i] = get_new_color();
        current_colors_F[i] = {leds[i].r, leds[i].g, leds[i].b};
        int ticks_to_target = random(MIN_TICKS, MAX_TICKS + 1);
        ticks_to_target_arr[i] = ticks_to_target;
        current_ticks_to_target_arr[i] = 0;
        color_increments[i] = {((float)target_colors[i].r - (float)leds[i].r) / (float)ticks_to_target, ((float)target_colors[i].g - (float)leds[i].g) / (float)ticks_to_target, ((float)target_colors[i].b - (float)leds[i].b) / (float)ticks_to_target};
      }
      
      current_colors_F[i].r = current_colors_F[i].r + color_increments[i].r;
      current_colors_F[i].g = current_colors_F[i].g + color_increments[i].g;
      current_colors_F[i].b = current_colors_F[i].b + color_increments[i].b;
      leds[i].r = (int)current_colors_F[i].r;
      leds[i].g = (int)current_colors_F[i].g;
      leds[i].b = (int)current_colors_F[i].b;

      current_ticks_to_target_arr[i]++;
    }

    FastLED.show();

    timer_tick = false;
  }
}

CRGB get_new_color() {
  return colors_set[random(COLOR_COUNT)];
}

ISR(TIMER1_COMPA_vect) {
  timer_tick = true;
}


