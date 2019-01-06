/*
 * In this program I took the FastLED ColorPaletet example
 * Set the pins for ATTiny85
 * Moved the interrupt to Pin 3
 * the WS2812B is on pin 0  there is a 470ohm resitor to the 
 * and made it so hitting a button will change the pallet.
 * I thought about adding antibounce but decided its just not required.
 * This uses less than 70% of the program space on an ATTINY85.
 */
#include <FastLED.h>
#define LED_PIN     0  //WS2812B data line. (two wires for redundancy).
#define NUM_LEDS    28
#define BRIGHTNESS  80
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];

#define UPDATES_PER_SECOND 100


CRGBPalette16 currentPalette;
TBlendType    currentBlending;

extern CRGBPalette16 myRedWhiteBluePalette;
extern const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM;
const int buttonPin=3;  // for interrupt.
volatile int value=0;   // the button press count.

void setup() {
    delay( 3000 ); // power-up safety delay
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
    FastLED.setBrightness(  BRIGHTNESS );

    //Start with rainbow palette
    currentPalette = RainbowColors_p;
    currentBlending = LINEARBLEND;

    //Set interrupt. 
    pinMode(buttonPin, INPUT);
    GIMSK = 0b00100000;    // turns on pin change interrupts
    // are from right to left pin 0,1,2,3...
    PCMSK = 0b00001000;    // turn on interrupts on pins PB3
    sei();                 // enables interrupts
 
}


void loop()
{
   //The cool part about this: 
   //The palette can be changed in the interrupt.
   //because they use a pointer startIndex rather than a for loop.
    
    static uint8_t startIndex = 0;
    startIndex = startIndex + 1; /* motion speed */
    
    FillLEDsFromPaletteColors( startIndex);
    
    FastLED.show();
    FastLED.delay(1000 / UPDATES_PER_SECOND);
}

void FillLEDsFromPaletteColors( uint8_t colorIndex)
{
    uint8_t brightness = 255;
    
    for( int i = 0; i < NUM_LEDS; i++) {
        leds[i] = ColorFromPalette( currentPalette, colorIndex, brightness, currentBlending);
        colorIndex += 3;
    }
}



// This function fills the palette with totally random colors.
void SetupTotallyRandomPalette()
{
    for( int i = 0; i < 16; i++) {
        currentPalette[i] = CHSV( random8(), 255, random8());
    }
}

// This function sets up a palette of black and white stripes,
// using code.  Since the palette is effectively an array of
// sixteen CRGB colors, the various fill_* functions can be used
// to set them up.
void SetupBlackAndWhiteStripedPalette()
{
    // 'black out' all 16 palette entries...
    fill_solid( currentPalette, 16, CRGB::Black);
    // and set every fourth one to white.
    currentPalette[0] = CRGB::White;
    currentPalette[4] = CRGB::White;
    currentPalette[8] = CRGB::White;
    currentPalette[12] = CRGB::White;
    
}

// This function sets up a palette of purple and green stripes.
void SetupPurpleAndGreenPalette()
{
    CRGB purple = CHSV( HUE_PURPLE, 255, 255);
    CRGB green  = CHSV( HUE_GREEN, 255, 255);
    CRGB black  = CRGB::Black;
    
    currentPalette = CRGBPalette16(
                                   green,  green,  black,  black,
                                   purple, purple, black,  black,
                                   green,  green,  black,  black,
                                   purple, purple, black,  black );
}


// This example shows how to set up a static color palette
// which is stored in PROGMEM (flash), which is almost always more
// plentiful than RAM.  A static PROGMEM palette like this
// takes up 64 bytes of flash.
const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM =
{
    CRGB::Red,
    CRGB::Gray, // 'white' is too bright compared to red and blue
    CRGB::Blue,
    CRGB::Black,
    
    CRGB::Red,
    CRGB::Gray,
    CRGB::Blue,
    CRGB::Black,
    
    CRGB::Red,
    CRGB::Red,
    CRGB::Gray,
    CRGB::Gray,
    CRGB::Blue,
    CRGB::Blue,
    CRGB::Black,
    CRGB::Black
};
/*
 * Setup interrupt.  I know there is no debounce but the main things
 * is that the color pattern changes when you hit the button.
 * 
 * There are several different palettes of colors demonstrated here.
 * FastLED provides several 'preset' palettes: RainbowColors_p, RainbowStripeColors_p,
 * OceanColors_p, CloudColors_p, LavaColors_p, ForestColors_p, and PartyColors_p.
 */

ISR(PCINT0_vect)
{
    value ++;             // Increment volatile variable
    switch(value){
       case 1:  currentPalette = RainbowColors_p;         currentBlending = LINEARBLEND; break;
       case 2:  currentPalette = RainbowStripeColors_p;   currentBlending = NOBLEND;  break;
       case 3:  currentPalette = RainbowStripeColors_p;   currentBlending = LINEARBLEND; break;
       case 4:  SetupPurpleAndGreenPalette();             currentBlending = LINEARBLEND; break;
       case 5:  SetupTotallyRandomPalette();              currentBlending = LINEARBLEND; break;
       case 6:  SetupBlackAndWhiteStripedPalette();       currentBlending = NOBLEND; break;
       case 7:  SetupBlackAndWhiteStripedPalette();       currentBlending = LINEARBLEND; break;
       case 8:  currentPalette = CloudColors_p;           currentBlending = LINEARBLEND; break;
       case 9:  currentPalette = PartyColors_p;           currentBlending = LINEARBLEND; break;
       case 10: currentPalette = myRedWhiteBluePalette_p; currentBlending = NOBLEND; break;
       case 11: currentPalette = myRedWhiteBluePalette_p; currentBlending = LINEARBLEND;  break;
       default: value=0; 
    }
}

