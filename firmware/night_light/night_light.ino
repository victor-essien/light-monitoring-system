// ============================================================
// ESP32 Night Light
//
// Reads a light sensor and a potentiometer, switches a warm RGB
// night light on when the room gets darker than the threshold,
// and serves a live dashboard over Wi-Fi.
//
// Setup: copy secrets.example.h to secrets.h and add your Wi-Fi
// details. See the README for the full guide.
// ============================================================

#include "secrets.h"

#include "src/config.h"
#include "src/state.h"
#include "src/sensors.h"
#include "src/light_control.h"
#include "src/wifi_manager.h"
#include "src/web_server.h"


// Read the sensors, decide day/night, drive the LED.
static void updateSystem() {

  sensorsRead();

  lightUpdateMode();

  lightUpdateOutput();

  state.lastUpdate = millis();
}


void setup() {

  Serial.begin(Timing::SERIAL_BAUD);

  delay(500);

  sensorsBegin();
  lightBegin();

  // First reading before the network starts
  updateSystem();

  wifiConnect(WIFI_SSID, WIFI_PASSWORD);

  webBegin();
}


void loop() {

  updateSystem();

  webHandle();

  delay(Timing::LOOP_DELAY_MS);
}
