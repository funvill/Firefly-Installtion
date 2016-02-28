/**
 * Made by Steven Smethurst. 
 * Last updated: Feb 27, 2016 
 * This is part of the FireFly installtion 
 */


#include "FastLED.h"


// Settings
// ------------------------------

// How many leds are in the strip?
#define NUM_LEDS            30*5

// Data pin that led data will be written out over
#define DATA_PIN              6

// How frequently the twinkles in ms 
// 50ms default
#define SETTING_MAX_TIMER_TWINKLE   50

// How much range there is in each twinkle. 
//   0 = No twinkle. 
//  30 = Small amount 
// 200 = Lots of twinkle, default 
#define SETTING_TWINKLE_RANGE      0

// This is the fade in and fade out. This setting depends on the SETTING_MAX_BRIGHTNESS 
// and SETTING_MIN_BRIGHTNESS. If these two settings are equal to each other then there 
// will be no fading. 
//  0 = No fade in and out. 
// 50 = good speed, default.
#define SETTING_STEP_BRIGHTNESS        0 // Default  50 
#define SETTING_MAX_BRIGHTNESS       200 // Default 200 
#define SETTING_MIN_BRIGHTNESS        20 // Default  20
#define SETTING_MAX_TIMER_FADE         3 // Default   3

// How frequently the snow flakes fall down. This is a range so that there can be some 
// randomness to it. You want a good size range between these values to have any visuable 
// difference.  This is in ms
// 2000-200 default 
#define SETTING_MAX_TIMER_FALLING   2000
#define SETTING_MIN_TIMER_FALLING    300

// The length or size of the snow flake. 
// 1 default. 
#define SETTING_LENGTH                2 

// How many different snowflakes are on the strip. 
// 25 default 
#define SETTING_NUM_SNOW_FLAKES       25 

#define SETTING_HUE                  180



// LED States 
// ------------------------------
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
    unsigned char state ; 
     
    unsigned short fallingRate ; 
    unsigned short twinkleRate ;
    unsigned short fadeRate ;
    unsigned long timerFalling ; 
    unsigned long timerTwinkle ;    
    unsigned long timerFade ; 

    unsigned char hue ; 
    unsigned char saturation ; 
    unsigned char brightness ; 
  
    CFallingSnowLEDs() {
      this->saturation = 0 ; 
      this->Reset(); 
      
    }
    void Reset() {
      this->SetOffset( 0 );
      this->state = STATE_TWINKLING ; 
      this->brightness = SETTING_MAX_BRIGHTNESS; 
      this->hue = SETTING_HUE; 
      // this->saturation = 32+random(128) ; 
      
      this->fallingRate  = random( SETTING_MAX_TIMER_FALLING - SETTING_MIN_TIMER_FALLING) + SETTING_MIN_TIMER_FALLING ; 
      this->twinkleRate  = random( SETTING_MAX_TIMER_TWINKLE );
      this->fadeRate     = random( SETTING_MAX_TIMER_FADE );
      
      this->timerFalling = random( 1000 );             
      this->timerTwinkle = random( 1000 ); 
      this->timerFade    = random( 1000 ); 
      
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
        if( SETTING_STEP_BRIGHTNESS == 0 || this->brightness > SETTING_MAX_BRIGHTNESS - (SETTING_MAX_BRIGHTNESS/SETTING_STEP_BRIGHTNESS) - 1 ) {
          // We have reached full brightness 
          this->state = STATE_TWINKLING ; 
          this->brightness = SETTING_MAX_BRIGHTNESS ; 
          return ; 
        }
        
        if( this->timerFade > millis() ) {
          return ; 
        }
        this->timerFade = millis() + SETTING_MAX_TIMER_FADE ;  
        
        // Brighten the leds
        brightness += ((SETTING_MAX_BRIGHTNESS-SETTING_MIN_BRIGHTNESS)/SETTING_STEP_BRIGHTNESS)  ;
        this->Twinkling();
    }

    void Falling() {
        if( SETTING_STEP_BRIGHTNESS == 0 || brightness <= SETTING_MIN_BRIGHTNESS ) {
          SetLEDs( ledOffset, 0, 0, 0 );// Turn the previouse LED off       
          SetOffset( ledOffset + 1 ) ; // Move the offset down one
          this->state = STATE_RISING ;
          return ; 
        }

        if( this->timerFade > millis() ) {
          return ; 
        }
        this->timerFade = millis() + SETTING_MAX_TIMER_FADE ;  
                
        // Dim the LEDS. 
        brightness -= ((SETTING_MAX_BRIGHTNESS-SETTING_MIN_BRIGHTNESS)/SETTING_STEP_BRIGHTNESS)  ;
        this->Twinkling();
    }
    
    void Twinkling()
    {
      if( SETTING_MAX_TIMER_TWINKLE != 0 ) {
        if( this->timerTwinkle > millis() ) {
          return ; 
        }
        this->timerTwinkle = millis() + this->twinkleRate ; 
      }
      
      // unsigned char s = this->saturation + (random( SETTING_TWINKLE_RANGE) - SETTING_TWINKLE_RANGE/2 )  ;
      unsigned char s = this->saturation ; 
      unsigned char b = this->brightness + (random( SETTING_TWINKLE_RANGE) - SETTING_TWINKLE_RANGE/2 ) ;
      // unsigned char b = this->brightness  ;
      
      for( int offset = 0 ; offset < SETTING_LENGTH ; offset++ ) {
        // Make them twinkel randomly 
        SetLEDs( ledOffset+offset, this->hue, s, (b/(SETTING_LENGTH+1) * (offset+1)) );
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
  delay(1);
  return ;
}
