/**
 * Made by Steven Smethurst. 
 * Last updated: Dec 14th, 2015 
 * This is part of the FireFly installtion 
 */


#include "FastLED.h"

// How many leds are in the strip?
#define NUM_LEDS_PER_STRIP 60
#define NUM_STRIP           4
#define NUM_LEDS            NUM_LEDS_PER_STRIP*NUM_STRIP

// Data pin that led data will be written out over
#define DATA_PIN 4

#define SETTING_RISING_RATE             5 
#define SETTING_FALLING_RATE           25 
#define SETTING_MAX_BRIGHTNESS        255 
#define SETTING_STARTING_BRIGHTNESS    30 
#define SETTING_TIMER_CREATION         40 
#define SETTING_TIMER_FRAMES           40 
#define SETTING_SATURATION            255 // Set to 0 for white leds, or 255 for full color. 

#define LED_STATE_OFF      0
#define LED_STATE_RISING   1
#define LED_STATE_FALLING  2

class CFireFliesLEDs
{
  public: 
  
    unsigned char hue;
    unsigned char brightness;
    unsigned char state;
  
    CFireFliesLEDs() {
      state = LED_STATE_OFF ; 
    }
    
    void activate() {
      this->hue = random( 255 ) ; 
      this->brightness = SETTING_STARTING_BRIGHTNESS ; 
      this->state = LED_STATE_RISING ; 
    }
    void debug() {
      // Serial.println("HUE["+ String( this->hue, DEC ) + "], Brightness["+ String( this->brightness, DEC ) + "], state[" + String(this->state, DEC) + "]" );
    }
    CHSV tick() {
      switch( state ) {
        default : 
        case LED_STATE_OFF: {          
          brightness = 0 ; 
          break ; // Do nothing 
        }
        case LED_STATE_RISING: {
          if( brightness + SETTING_RISING_RATE >= SETTING_MAX_BRIGHTNESS ) {
            state = LED_STATE_FALLING ; 
            debug();
            break ; 
          }
          brightness += SETTING_RISING_RATE ;    
          debug();     
          break ; 
        }
        case LED_STATE_FALLING: {
          if( brightness - SETTING_FALLING_RATE <= 0 ) {
            state = LED_STATE_OFF ; 
            brightness = 0 ; 
            debug();
            break ; 
          }
          brightness -= SETTING_FALLING_RATE ;
          debug();
          break ; 
        }
      }
      return CHSV ( hue, SETTING_SATURATION, brightness ) ;
    }
}; 


// This is an array of leds.  One item for each led in your strip.
CRGB            leds[NUM_LEDS];
CFireFliesLEDs  ledFireFly[NUM_LEDS];

void setup() {
  Serial.begin(115200);
  
  // sanity check delay - allows reprogramming if accidently blowing power w/leds
    delay(2000);
    FastLED.addLeds<WS2812B, DATA_PIN+0, RGB>(leds, NUM_LEDS_PER_STRIP);
    FastLED.addLeds<WS2812B, DATA_PIN+1, RGB>(leds+NUM_LEDS_PER_STRIP*1, NUM_LEDS_PER_STRIP);
    FastLED.addLeds<WS2812B, DATA_PIN+2, RGB>(leds+NUM_LEDS_PER_STRIP*2, NUM_LEDS_PER_STRIP);
    FastLED.addLeds<WS2812B, DATA_PIN+3, RGB>(leds+NUM_LEDS_PER_STRIP*3, NUM_LEDS_PER_STRIP);

    
    Serial.println("Starting...");
}

void loop() {
  // Every n seconds start a new led
  static unsigned long timeout = 0 ; 
  if( timeout < millis() ) {
    timeout = millis() + SETTING_TIMER_CREATION ; 
    ledFireFly[ random( NUM_LEDS ) ].activate();
  }

  for( unsigned short offset = 0 ; offset < NUM_LEDS ; offset++ ) {
    leds[offset] = ledFireFly[ offset ].tick() ; 
  }  
  FastLED.show();
  
  // Wait a little bit
  delay(SETTING_TIMER_FRAMES);
  return ;
}
