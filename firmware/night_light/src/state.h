#pragma once
// ============================================================
// Shared system state.
// Sensors write it, light control and the web server read it.
// ============================================================

struct LightState {

  int rawLight = 0;
  int rawPot = 0;

  float brightness = 0.0;
  float darkness = 0.0;

  float threshold = 50.0;

  bool nightMode = false;

  int ledBrightness = 0;

  unsigned long lastUpdate = 0;
};

extern LightState state;
