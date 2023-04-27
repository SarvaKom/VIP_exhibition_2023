#include <Arduino.h>

#include "pitches.h"

#include <ADCTouch.h> // Include the pitches library

#include <FastLED.h>

FASTLED_USING_NAMESPACE

int statetime = 0;
bool state = false; 

//for led strip
#define DATA_PIN    7
#define DATA_PIN2    8
//#define CLK_PIN   9
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
#define NUM_LEDS    30
CRGB leds[NUM_LEDS];

#define BRIGHTNESS          96
#define FRAMES_PER_SECOND  120

// piezo buzzer
int speakerPin1 = 5;
int speakerPin2 = 6;

// timer
unsigned long startTime;
unsigned long currentTime;

// set intervals to trigger different light / music patterns
int intervalLength = 500;
int intervalStagger = 5;
int interval1 = (intervalLength * (intervalStagger * 1));
int interval2 = (intervalLength * (intervalStagger * 2));
int interval3 = (intervalLength * (intervalStagger * 3));
int interval4 = (intervalLength * (intervalStagger * 4));
int interval5 = (intervalLength * (intervalStagger * 5));
int interval6 = (intervalLength * (intervalStagger * 6));
int interval7 = (intervalLength * (intervalStagger * 7));
int interval8 = (intervalLength * (intervalStagger * 8));

// Define the notes of the song as an array
int melody[] = {
  NOTE_C4,
  NOTE_G3,
  NOTE_G3,
  NOTE_A3,
  NOTE_G3,
  0,
  NOTE_B3,
  NOTE_C4
};

// Define the timing of the song as an array
int noteDurations[] = {
  4,
  8,
  8,
  4,
  4,
  4,
  4,
  4
};

int tune1[] = {
  NOTE_C5,
  NOTE_D5,
  NOTE_G5,
  NOTE_D5,
  NOTE_C5
};

int tune2[] = {
  NOTE_D5,
  NOTE_F5,
  NOTE_A5,
  NOTE_F5,
  NOTE_D5
};

int tune3[] = {
  NOTE_E5,
  NOTE_G5,
  NOTE_B5,
  NOTE_G5,
  NOTE_E5
};

int tuneDurations[] = {
  4,
  4,
  4,
  4,
  4
};

int notes = 8;
int notesTunes = 5;

int currNote = 0;
int currNote1 = 0;
int currNote2 = 0;
int currNote3 = 0;

// references values to remove offset for the flower sensors
int ref0;
int ref1;
int ref2;

uint8_t gHue = 0; // rotating "base color" used by many of the patterns

//animation functions based on fast led library
void rainbow() 
{
  // FastLED's built-in rainbow generator
  fill_rainbow( leds, NUM_LEDS, gHue, 7);
}
void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, NUM_LEDS, 20);
  byte dothue = 0;
  for( int i = 0; i < 8; i++) {
    leds[beatsin16( i+7, 0, NUM_LEDS-1 )] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}

void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
  
}

void setup() {
  // start the timer
  startTime = millis();

  // reference values for the flowers
  ref0 = ADCTouch.read(A0, 500);
  ref1 = ADCTouch.read(A1, 500);
  ref2 = ADCTouch.read(A2, 500);
  //  ref3 = ADCTouch.read(A4, 500);

//setting up LED strip - fast led library:
  // tell FastLED about the LED strip configuration
  FastLED.addLeds<NEOPIXEL, 7>(leds, 30); 
  // FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  // //FastLED.addLeds<LED_TYPE,DATA_PIN,CLK_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  // FastLED.addLeds<LED_TYPE,DATA_PIN2,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);

  Serial.begin(9600);

}

void loop() {
  leds[0] = CRGB::White; FastLED.show(); delay(30); 
  leds[0] = CRGB::Black; FastLED.show(); delay(30);
  // get the touch readings for each capacitor
  int value0 = ADCTouch.read(A0);
  int value1 = ADCTouch.read(A1);
  int value2 = ADCTouch.read(A2);
  // int value3 = ADCTouch.read(A4);

  // remove the offset
  value0 -= ref0;
  value1 -= ref1;
  value2 -= ref2;
  //  value3 -= ref3;

  // use  value of capacitor as a boolean
  int buttonState1 = (value0 > 40);
  int buttonState2 = (value1 > 40);
  int buttonState3 = (value2 > 40);
  // int buttonState4 = (value3 > 40) ;

  // get the current time
  currentTime = millis();
  // Check if all buttons are held down
  if (buttonState1 == 1 && buttonState2 == 1 && buttonState3 == 1) {

    // Play the song, while loop that iterates through the melody 
    // while (true) {
    //   // As soon as any of the buttons is released, stop playing
    //   Serial.println("All buttons pressed.");
    //   if (buttonState1 != 1 || buttonState2 != 1 || buttonState3 != 1) {
    //     break;
    //   }

      
      // Calculate the note duration for each note
      int noteDuration = 1000 / noteDurations[currNote];
      // Play the note
      tone(speakerPin1, melody[currNote], noteDuration);
      // Calculate the pause between each note
      int pauseBetweenNotes = noteDuration * 1.30;
      // Have a longer pause if the last note of the melody is played
      if (currNote == notes - 1) {
        pauseBetweenNotes *= 4;
      }
      // Actual pause here
      delay(pauseBetweenNotes);
      // Increment note 
      currNote = (currNote + 1) % notes;
    
  }
  // If a single button is pressed
  else if (buttonState1 == 1 && buttonState2 != 1 && buttonState3 != 1) {
    Serial.println("Button 1 pressed.");
    tone(speakerPin1, NOTE_C4, 200);
  } else if (buttonState1 != 1 && buttonState2 == 1 && buttonState3 != 1) {
    Serial.println("Button 2 pressed.");
    tone(speakerPin2, NOTE_D4, 200);
  } else if (buttonState1 != 1 && buttonState2 != 1 && buttonState3 == 1) {
    Serial.println("Button 3 pressed.");
    tone(speakerPin2, NOTE_G4, 200);
  }

  // If a pair of two buttons are pressed
  else if (buttonState1 == 1 && buttonState2 == 1 && buttonState3 != 1) {
    Serial.println("Button 1 and 2 pressed.");
    int tune1Duration = 1000 / tuneDurations[currNote1];
    tone(speakerPin1, tune1[currNote1], tune1Duration);
    int pauseBetweenNotes = tune1Duration * 1.30;
    if (currNote1 == notesTunes - 1) {
        pauseBetweenNotes *= 4;
    }
    delay(pauseBetweenNotes);
    currNote1 = (currNote1 + 1) % notesTunes;
  } else if (buttonState1 != 1 && buttonState2 == 1 && buttonState3 == 1) {
    Serial.println("Button 2 and 3 pressed.");
    int tune2Duration = 1000 / tuneDurations[currNote2];
    tone(speakerPin1, tune2[currNote2], tune2Duration);
    int pauseBetweenNotes = tune2Duration * 1.30;
    if (currNote2 == notesTunes - 1) {
      pauseBetweenNotes *= 4;
    }
    delay(pauseBetweenNotes);
    currNote2 = (currNote2 + 1) % notesTunes;
  } else if (buttonState1 == 1 && buttonState2 != 1 && buttonState3 == 1) {
    Serial.println("Button 3 and 1 pressed.");
    int tune3Duration = 1000 / tuneDurations[currNote3];
    tone(speakerPin1, tune3[currNote3], tune3Duration);
    int pauseBetweenNotes = tune3Duration * 1.30;
    if (currNote3 == notesTunes - 1) {
      pauseBetweenNotes *= 4;
    }
    delay(pauseBetweenNotes);
    currNote3 = (currNote3 + 1) % notesTunes;
  }

  // If no combination of buttons are pressed, stop both speakers
  else {
    // Stop playing the song
    noTone(speakerPin1);
    noTone(speakerPin2);
  }
}
