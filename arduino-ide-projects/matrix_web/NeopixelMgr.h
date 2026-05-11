/* 
Copyright 2026 Shobhit Aggarwal

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
 */

#include <Adafruit_NeoPixel.h>
// #include <Preferences.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

#define PIN       D0 // On Trinket or Gemma, suggest changing this to 1
#define NUMPIXELS 21 // Popular NeoPixel ring size

// Preferences prefs;

class NeopixelMgr {
  private:
    Adafruit_NeoPixel* pixels;

  public:
    NeopixelMgr(const NeopixelMgr&) = delete;
    NeopixelMgr& operator=(const NeopixelMgr&) = delete;

    NeopixelMgr() : pixels(new Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800)) {
      #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
        clock_prescale_set(clock_div_1);
      #endif
      pixels->begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
      pixels->clear();  

      // prefs.begin("rgb-led", false);  // Open namespace in read/write mode
      // int r = prefs.getUInt("red", 255);
      // int g = prefs.getUInt("green", 255);
      // int b = prefs.getUInt("blue", 255);
      // prefs.end();      

      setLEDColor(255, 255, 255);  // Restore last color
    }

    void setLEDColor(int r, int g, int b) {
      for(int i=0; i<NUMPIXELS; i++) { // For each pixel...
        pixels->setPixelColor(i, pixels->Color(r, g, b));
      }
      pixels->show();   // Send the updated pixel colors to the hardware.

      // prefs.begin("rgb-led", false);
      // prefs.putUInt("red", r);
      // prefs.putUInt("green", g);
      // prefs.putUInt("blue", b);
      // prefs.end();      
    }
};

extern NeopixelMgr neopixel; // Singleton instance