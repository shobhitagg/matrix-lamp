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

#include <Arduino.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <WiFi.h>
#include "builtinfiles.h"
#include "secrets.h"
#include <Adafruit_NeoPixel.h>
#include <Preferences.h>
#ifdef __AVR__
#include <avr/power.h>  // Required for 16 MHz Adafruit Trinket
#endif

#define PIN D10        // On Trinket or Gemma, suggest changing this to 1
#define NUMPIXELS 25  // Popular NeoPixel ring size

WebServer server(80);
Preferences prefs;
bool connected;
IPAddress localIP;

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

void setLEDColor(int r, int g, int b) {
  for (int i = 0; i < NUMPIXELS; i++) {  // For each pixel...
    pixels.setPixelColor(i, pixels.Color(r, g, b));
  }
  pixels.show();  // Send the updated pixel colors to the hardware.

  prefs.begin("rgb-led", false);
  prefs.putUInt("red", r);
  prefs.putUInt("green", g);
  prefs.putUInt("blue", b);
  prefs.end();
}

// Serve the HTML page
void handleRoot() {
  server.send(200, "text/html", FPSTR(indexContent));
}

// API to set the color: /api/set_rgb?r=255&g=0&b=0
void handleSetRGB() {
  if (server.hasArg("r") && server.hasArg("g") && server.hasArg("b")) {
    int r = server.arg("r").toInt();
    int g = server.arg("g").toInt();
    int b = server.arg("b").toInt();

    // Serial.println("RGB color request received.");

    setLEDColor(r, g, b);  // Your function to apply RGB to LED
    server.send(200, "text/plain", "OK");
  } else {
    server.send(400, "text/plain", "Missing RGB arguments");
  }
}

void handleNotFound() {
  server.send(404, "text/plain", "Not Found");
}

void connect() {
  if (connected) return;
  // Connect to WiFi
  WiFi.setHostname(HOSTNAME);
  WiFi.mode(WIFI_STA);

  // Use credentials from secrets.h or hardcode here
  // if (strlen(ssid) == 0) WiFi.begin(); else WiFi.begin(ssid, passPhrase);
  WiFi.begin(ssid, passPhrase);  // Assuming secrets.h defines these

  unsigned long timeout = 10*60*1000; // 1 min
  unsigned long time_start = millis();
  unsigned long time_elapsed = millis() - time_start;
  while (WiFi.status() != WL_CONNECTED && time_elapsed < timeout) {
    delay(500);
    time_elapsed = millis() - time_start;
  }

  if (WiFi.status() != WL_CONNECTED) {
    connected = false;
    return;
  }

  localIP = WiFi.localIP();

  if(!MDNS.begin(mdnsName)) {
    while(1){
      delay(1000);
    }
  }

  MDNS.addService("_http", "_tcp", 80);

  // 3. Register Server Handlers
  server.on("/", HTTP_GET, handleRoot);
  server.on("/api/set_rgb", HTTP_GET, handleSetRGB);
  server.onNotFound(handleNotFound);

  server.enableCORS(true);

  // 4. Start Server
  server.begin();
  connected = true;
}

void setup(void) {
  Serial.begin(115200);
  delay(1200);

#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
    clock_prescale_set(clock_div_1);
#endif
  pixels.begin();  // INITIALIZE NeoPixel strip object (REQUIRED)
  pixels.clear();
  pixels.setBrightness(255);
  for (int i = 0; i < NUMPIXELS; i++) {  // For each pixel...
    pixels.setPixelColor(i, pixels.Color(255, 255, 255));
  }
  pixels.show();  // Send the updated pixel colors to the hardware.
  delay(500);
  pixels.show();  // Send the updated pixel colors to the hardware.
  
  prefs.begin("rgb-led", false);  // Open namespace in read/write mode
  int r = prefs.getUInt("red", 255);
  int g = prefs.getUInt("green", 255);
  int b = prefs.getUInt("blue", 255);
  prefs.end();

  setLEDColor(r, g, b);  // Restore last color  
  delay(300);
  setLEDColor(r, g, b);  // Restore last color  
  Serial.println("\nStarting RGB WebServer...");

  connect();

  if (connected){
    Serial.print("Connected! IP Address: ");
    Serial.println(localIP);
  } else {
    Serial.print("Timeout. Not Connected.");
  }

}

void loop(void) {
  server.handleClient();
}