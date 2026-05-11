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
#include <WiFi.h>
#include "builtinfiles.h" 
#include "secrets.h" // Ensure this file exists with your SSID/Pass

// --- Server Configuration ---
#define HOSTNAME "esp32-rgb"
WebServer server(80);

// --- Request Handlers ---

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

    server.send(200, "text/plain", "OK");
  } else {
    server.send(400, "text/plain", "Missing RGB arguments");
  }
}

void handleNotFound() {
  server.send(404, "text/plain", "Not Found");
}

// --- Setup and Loop ---

void setup(void) {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\nStarting RGB WebServer...");

  // 2. Connect to WiFi
  WiFi.setHostname(HOSTNAME);
  WiFi.mode(WIFI_STA);
  
  // Use credentials from secrets.h or hardcode here
  // if (strlen(ssid) == 0) WiFi.begin(); else WiFi.begin(ssid, passPhrase);
  WiFi.begin(ssid, passPhrase); // Assuming secrets.h defines these

  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("\nConnected. IP Address: ");
  Serial.println(WiFi.localIP());

  // 3. Register Server Handlers
  server.on("/", HTTP_GET, handleRoot);
  server.on("/api/set_rgb", HTTP_GET, handleSetRGB);
  server.onNotFound(handleNotFound);

  server.enableCORS(true);

  // 4. Start Server
  server.begin();
  Serial.printf("Open http://%s.local or http://%s in your browser\n", HOSTNAME, WiFi.localIP().toString().c_str());
}

void loop(void) {
  server.handleClient();
}