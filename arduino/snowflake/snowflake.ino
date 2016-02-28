/**
 * Made by Steven Smethurst. 
 * Last updated: Feb 27, 2016 
 * This is part of the FireFly installtion 
 */


#include "FastLED.h"

// How many leds are in the strip?
#define NUM_LEDS            30*5


// Data pin that led data will be written out over
#define DATA_PIN 6

// How frequently the twinkles in ms 
// 50ms default
#define SETTING_TIMER_TWINKLE         50 

// How much range there is in each twinkle. This is mesured in saturation. 
// 0 = No twinkle. 
// 200 = lots of twinkle, default 
#define SETTING_TWINKLE_RANGE         20

// This is the fade in and fade out. This setting depends on the SETTING_MAX_BRIGHTNESS 
// and SETTING_MIN_BRIGHTNESS. If these two settings are equal to each other then there 
// will be no fading. 
//  1 No fade in and out. 
// 10 good speed, default.
#define SETTING_STEP_BRIGHTNESS        3
#define SETTING_MAX_BRIGHTNESS       200 
#define SETTING_MIN_BRIGHTNESS        20

// How frequently the snow flakes fall down. This is a range so that there can be some 
// randomness to it. You want a good size range between these values to have any visuable 
// difference.  This is in ms
// 2000-200 default 
#define SETTING_MAX_TIMER_FALLING   2000
#define SETTING_MIN_TIMER_FALLING    500

// The length or size of the snow flake. 
// 1 default. 
#define SETTING_LENGTH                2 

// How many different snowflakes are on the strip. 
// 25 default 
#define SETTING_NUM_SNOW_FLAKES       20 

#define SETTING_HUE                  204


#define STATE_TWINKLING            0
#define STATE_FALLING              1 
#define STATE_RISING               2


CRGB leds[NUM_LEDS];
void SetLEDs( short offset, unsigned char hue, unsigned char saturation, unsigned char brightness ) {
  // Prevent overflows 
  if( offset < 0 || offset > NUM_LEDS ) { 
    return ; 
  }
  // Set the LEDs 
  leds[ offset ] = CHSV ( hue, saturation, brightness ); 
}





class CFallingSnowLEDs
{
  public: 
    unsigned short ledOffset ; 
    unsigned long timerFalling ; 
    unsigned short fallingRate ; 
    unsigned char state ; 

    unsigned char hue ; 
    unsigned char saturation ; 
    unsigned char brightness ; 
  
    CFallingSnowLEDs() {
      this->Reset(); 
    }
    void Reset() {
      this->SetOffset( 0 );
      this->timerFalling = random( SETTING_MAX_TIMER_FALLING ); 
      this->state = STATE_TWINKLING ; 
      this->hue = SETTING_HUE; 
      this->brightness = SETTING_MAX_BRIGHTNESS; 
      this->saturation = random(128) ; 
      this->fallingRate = random (SETTING_MAX_TIMER_FALLING-SETTING_MIN_TIMER_FALLING) + SETTING_MIN_TIMER_FALLING ; 
    }
    void SetOffset( unsigned short offset ) {
      this->ledOffset = offset ; 
    }
        
    void Tick() {
      if( this->ledOffset > NUM_LEDS ) {
        this->Reset(); 
      }

      switch( this->state ) {
        default:
        case STATE_TWINKLING: {
          this->Twinkling(); 
          // Make the LEDs fall down the strip. 
          if( this->timerFalling < millis() ) {
            this->state = STATE_FALLING ; 
            this->timerFalling = millis() + fallingRate ;  // Start the timer for the next fall. 
          }
          break ; 
        }
        case STATE_FALLING: {
          this->Falling(); 
          break ; 
        }        
        case STATE_RISING: {
          this->Rising(); 
          break ; 
        }
      }

      
    }

    void Rising() {
        brightness += ((SETTING_MAX_BRIGHTNESS-SETTING_MIN_BRIGHTNESS)/ SETTING_STEP_BRIGHTNESS)  ;
        this->Twinkling();      

        // Brighten the leds
        if( this->brightness > SETTING_MAX_BRIGHTNESS - (SETTING_MAX_BRIGHTNESS/SETTING_STEP_BRIGHTNESS) - 1 ) {
          // We have reached full brightness 
          this->state = STATE_TWINKLING ; 
          this->brightness = SETTING_MAX_BRIGHTNESS ; 
          
          
          return ; 
        }        
    }

    void Falling() {

        // Dim the LEDS. 
        brightness -= ((SETTING_MAX_BRIGHTNESS-SETTING_MIN_BRIGHTNESS)/SETTING_STEP_BRIGHTNESS)  ;
        this->Twinkling();

        if( brightness <= SETTING_MIN_BRIGHTNESS ) {
          SetLEDs( ledOffset, 0, 0, 0 );// Turn the previouse LED off       
          SetOffset( ledOffset + 1 ) ; // Move the offset down one
          this->state = STATE_RISING ;
        }
    }
    
    void Twinkling()
    {      
      unsigned char s = this->saturation + (random( SETTING_TWINKLE_RANGE) - SETTING_TWINKLE_RANGE/2 )  ;
      unsigned char b = this->brightness + (random( SETTING_TWINKLE_RANGE) - SETTING_TWINKLE_RANGE/2 ) ;
      // unsigned char b = this->brightness  ;
      
      for( int offset = 0 ; offset < SETTING_LENGTH ; offset++ ) {
        // Make them twinkel randomly 
        SetLEDs( ledOffset+offset, this->hue, s, (b/SETTING_LENGTH * (offset+1)) );
      }     
    }
}; 


// This is an array of leds.  One item for each led in your strip.
CFallingSnowLEDs  ledSnowFlake[SETTING_NUM_SNOW_FLAKES];

void setup() {
  Serial.begin(115200);
  
  // sanity check delay - allows reprogramming if accidently blowing power w/leds
  delay(2000);
  FastLED.addLeds<WS2812B, DATA_PIN, RGB>(leds, NUM_LEDS);

  Serial.println("Starting...");
  for( unsigned short offset = 0 ; offset < SETTING_NUM_SNOW_FLAKES ; offset++ ) {
    // Add a little bit of randomness to the starting location of the flake. 
    unsigned short start = ( NUM_LEDS / SETTING_NUM_SNOW_FLAKES ) * offset + random( NUM_LEDS / SETTING_NUM_SNOW_FLAKES ) ; 
    ledSnowFlake[ offset ].SetOffset( start ) ; 
  }
}

void loop() {
  for( unsigned short offset = 0 ; offset < SETTING_NUM_SNOW_FLAKES ; offset++ ) {
    ledSnowFlake[ offset ].Tick() ; 
  }
  FastLED.show();
  
  // Wait a little bit
  delay(SETTING_TIMER_TWINKLE);
  return ;
}
