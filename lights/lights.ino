#include <FastLED.h>
#include <Wire.h>

#define NUM_LEDS 27
#define DATA_PIN 2
#define I2C_PIN 4
#define I2C_ADDRESS 8

#define FORWARD 0
#define BACKWARD 1
#define SLOW 250
#define MEDIUM 50
#define FAST 5

boolean direction = FORWARD;

#define DISABLED_LIGHTS 1
#define ENABLED_LIGHTS 2
#define ERROR_LIGHTS 3
#define AUTO_LIGHTS 4
#define ARM_LIGHTS 5
#define INTAKE_LIGHTS 6
#define CLIMB_LIGHTS 7

#define MAX_BRIGHTNESS 255

int signal;
CRGB leds[NUM_LEDS];

void setup() {
  signal = 0;
  Wire.begin(I2C_ADDRESS);
  Wire.onReceive(receiveEvent); // register event
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);

  // put your setup code here, to run once:

}

void loop() {
  switch (signal) {
    case DISABLED_LIGHTS:
      disabledLights();
      break;
    case ENABLED_LIGHTS:
      enabledLights();
      break;
    case ERROR_LIGHTS:
      errorLights();
      break;
    case AUTO_LIGHTS:
      autoLights();
      break;
    case ARM_LIGHTS:
      armLights();
      break;
    case INTAKE_LIGHTS:
      intakeLights();
      break;
    case CLIMB_LIGHTS:
      climbLights();
      break;
    default:
      disconnectedLights();
      break;

  }
  // put your main code here, to run repeatedly:

}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent() {

  signal = Wire.read();    // receive byte as an integer
  Serial.println(signal);         // print the integer
}

/***ANDYMARK Example Light Functions***/

//These are the functions we have defined to do chase patterns.  They are actually called inside the  loop() above
//They are meant to demonstrate things such as setting LED colors, controlling brightness
void color_chase(uint32_t color, uint8_t wait)
{
  //clear() turns all LEDs off
  FastLED.clear();
  //The brightness ranges from 0-255
  //Sets brightness for all LEDS at once
  FastLED.setBrightness(MAX_BRIGHTNESS);
  // Move a single led
  for (int led_number = 0; led_number < NUM_LEDS; led_number++)
  {
    // Turn our current led ON, then show the leds
    leds[led_number] = color;

    // Show the leds (only one of which is has a color set, from above
    // Show turns actually turns on the LEDs
    FastLED.show();

    // Wait a little bit
    delay(wait);

    // Turn our current led back to black for the next loop around
    leds[led_number] = CRGB::Black;
  }
  return;
}

//Move an "empty" dot down the strip
void missing_dot_chase(uint32_t color, uint8_t wait)
{
  //Step thru some brightness levels from max to 10.  led_brightness/=2 is a cryptic shorthand way of saying led_brightness = led_brightness/2
  for (int led_brightness = MAX_BRIGHTNESS; led_brightness > 10; led_brightness /= 2)
  {
    FastLED.setBrightness(led_brightness);

    // Start by turning all pixels on:
    //for(int led_number = 0; led_number < NUM_LEDS; led_number++) leds[led_number] = color;
    //https://github.com/FastLED/FastLED/wiki/Controlling-leds
    fill_solid(leds, NUM_LEDS, color);

    // Then display one pixel at a time:
    for (int led_number = 0; led_number < NUM_LEDS; led_number++)
    {
      leds[led_number] =  CRGB::Black; // Set new pixel 'off'
      if ( led_number > 0 && led_number < NUM_LEDS)
      {
        leds[led_number - 1] = color; // Set previous pixel 'on'
      }
      FastLED.show();
      delay(wait);
    }
  }
  return;
}

//Cylon - LED sweeps back and forth, with the color, delay and number of cycles of your choice
void cylon(CRGB color, uint16_t wait, uint8_t number_of_cycles)
{
  FastLED.setBrightness(255);
  for (uint8_t times = 0; times <= number_of_cycles; times++)
  {
    // Make it look like one LED is moving in one direction
    for (int led_number = 0; led_number < NUM_LEDS; led_number++)
    {
      //Apply the color that was passed into the function
      leds[led_number] = color;
      //Actually turn on the LED we just set
      FastLED.show();
      // Turn it back off
      leds[led_number] = CRGB::Black;
      // Pause before "going" to next LED
      delay(wait);
    }

    // Now "move" the LED the other direction
    for (int led_number = NUM_LEDS - 1; led_number >= 0; led_number--)
    {
      //Apply the color that was passed into the function
      leds[led_number] = color;
      //Actually turn on the LED we just set
      FastLED.show();
      // Turn it back off
      leds[led_number] = CRGB::Black;
      // Pause before "going" to next LED
      delay(wait);
    }
  }
  return;
}

void rainbow(uint8_t wait)
{

  uint16_t hue;
  FastLED.clear();

  for (hue = 10; hue < 255 * 3; hue++)
  {

    fill_rainbow( &(leds[0]), NUM_LEDS /*led count*/, hue /*starting hue*/);
    FastLED.show();
    delay(wait);
  }
  return;
}

/**** Using Resseguie FastLED pattererns ****/

// Changes all LEDS to given color
void allColor(CRGB c) {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = c;
  }
  FastLED.show();
}

void allRandom() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = randomColor();
  }
  FastLED.show();
}

// Random disolve colors
void disolve(int simultaneous, int cycles, int speed) {
  for (int i = 0; i < cycles; i++) {
    for (int j = 0; j < simultaneous; j++) {
      int idx = random(NUM_LEDS);
      leds[idx] = CRGB::Black;
    }
    FastLED.show();
    delay(speed);
  }

  allColor(CRGB::Black);
}

// Flashes given color
// If c==NULL, random color flash
void flash(CRGB c, int count, int speed) {
  for (int i = 0; i < count; i++) {
    if (c) {
      allColor(c);
    }
    else {
      allColor(randomColor());
    }
    delay(speed);
    allColor(CRGB::Black);
    delay(speed);
  }
}

// Wipes color from end to end
void colorWipe(CRGB c, int speed, int direction) {
  for (int i = 0; i < NUM_LEDS; i++) {
    if (direction == FORWARD) {
      leds[i] = c;
    }
    else {
      leds[NUM_LEDS - 1 - i] = c;
    }
    FastLED.show();
    delay(speed);
  }
}

// Rainbow colors that slowly cycle across LEDs
void rainbow(int cycles, int speed) { // TODO direction
  if (cycles == 0) {
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = Wheel(((i * 256 / NUM_LEDS)) & 255);
    }
    FastLED.show();
  }
  else {
    for (int j = 0; j < 256 * cycles; j++) {
      for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = Wheel(((i * 256 / NUM_LEDS) + j) & 255);
      }
      FastLED.show();
      delay(speed);
    }
  }
}

// Theater-style crawling lights
void theaterChase(CRGB c, int cycles, int speed) { // TODO direction

  for (int j = 0; j < cycles; j++) {
    for (int q = 0; q < 3; q++) {
      for (int i = 0; i < NUM_LEDS; i = i + 3) {
        int pos = i + q;
        leds[pos] = c;    //turn every third pixel on
      }
      FastLED.show();

      delay(speed);

      for (int i = 0; i < NUM_LEDS; i = i + 3) {
        leds[i + q] = CRGB::Black;      //turn every third pixel off
      }
    }
  }
}

// Theater-style crawling lights with rainbow effect
void theaterChaseRainbow(int cycles, int speed) { // TODO direction, duration
  for (int j = 0; j < 256 * cycles; j++) {   // cycle all 256 colors in the wheel
    for (int q = 0; q < 3; q++) {
      for (int i = 0; i < NUM_LEDS; i = i + 3) {
        int pos = i + q;
        leds[pos] = Wheel( (i + j) % 255);  //turn every third pixel on
      }
      FastLED.show();

      delay(speed);

      for (int i = 0; i < NUM_LEDS; i = i + 3) {
        leds[i + q] = CRGB::Black; //turn every third pixel off
      }
    }
  }
}

// Random flashes of lightning
void lightning(CRGB c, int simultaneous, int cycles, int speed) {
  int flashes[simultaneous];

  for (int i = 0; i < cycles; i++) {
    for (int j = 0; j < simultaneous; j++) {
      int idx = random(NUM_LEDS);
      flashes[j] = idx;
      leds[idx] = c ? c : randomColor();
    }
    FastLED.show();
    delay(speed);
    for (int s = 0; s < simultaneous; s++) {
      leds[flashes[s]] = CRGB::Black;
    }
    delay(speed);
  }
}

// Sliding bar across LEDs
void cylon(CRGB c, int width, int speed) {
  // First slide the leds in one direction
  for (int i = 0; i <= NUM_LEDS - width; i++) {
    for (int j = 0; j < width; j++) {
      leds[i + j] = c;
    }

    FastLED.show();

    // now that we've shown the leds, reset to black for next loop
    for (int j = 0; j < 5; j++) {
      leds[i + j] = CRGB::Black;
    }
    delay(speed);
  }

  // Now go in the other direction.
  for (int i = NUM_LEDS - width; i >= 0; i--) {
    for (int j = 0; j < width; j++) {
      leds[i + j] = c;
    }
    FastLED.show();
    for (int j = 0; j < width; j++) {
      leds[i + j] = CRGB::Black;
    }

    delay(speed);
  }
}

// Display alternating stripes
void stripes(CRGB c1, CRGB c2, int width) {
  for (int i = 0; i < NUM_LEDS; i++) {
    if (i % (width * 2) < width) {
      leds[i] = c1;
    }
    else {
      leds[i] = c2;
    }
  }
  FastLED.show();
}


// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
CRGB Wheel(byte WheelPos) {
  if (WheelPos < 85) {
    return CRGB(WheelPos * 3, 255 - WheelPos * 3, 0);
  }
  else if (WheelPos < 170) {
    WheelPos -= 85;
    return CRGB(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  else {
    WheelPos -= 170;
    return CRGB(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

CRGB randomColor() {
  return Wheel(random(256));
}

/*** Team 3997 Light Routines ***/

void disabledLights() {
  cylon(CRGB::Red, 1, 10);  
}

void enabledLights() {
  rainbow(1);
}

void errorLights() {
  flash(CRGB::Red, 10, 5);
}

void autoLights() {
  stripes(CRGB::Green, CRGB::Blue, 1);  
}

void armLights() {
  theaterChase(CRGB::Purple, 10, 10);
}

void intakeLights() {
  color_chase(CRGB::Yellow, 10);
}

void climbLights() {
  theaterChaseRainbow(10, 10);  
}

void disconnectedLights() {
  lightning(CRGB::Red, 10, 10, 10);
}
