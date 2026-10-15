#include <Arduino.h>

#include "light_control.h"
#include "config.h"
#include "state.h"


static void setRGB(int red, int green, int blue) {

  analogWrite(Pins::RED, red);
  analogWrite(Pins::GREEN, green);
  analogWrite(Pins::BLUE, blue);
}


void lightBegin() {

  pinMode(Pins::RED, OUTPUT);
  pinMode(Pins::GREEN, OUTPUT);
  pinMode(Pins::BLUE, OUTPUT);

  lightTurnOff();
}


void lightTurnOff() {

  setRGB(0, 0, 0);

  state.ledBrightness = 0;
}


// ------------------------------------------------------------
// Night / day decision
//
// HYSTERESIS: suppose threshold = 50%.
//
//   The light turns ON  when darkness >= 50%
//   Once ON, it turns OFF only when darkness < 47%
//
// This prevents flickering around the threshold.
// ------------------------------------------------------------

void lightUpdateMode() {

  if (!state.nightMode) {

    // Currently DAY
    if (state.darkness >= state.threshold) {
      state.nightMode = true;
    }

  } else {

    // Currently NIGHT
    if (state.darkness < (state.threshold - LightConfig::HYSTERESIS)) {
      state.nightMode = false;
    }
  }
}


// ------------------------------------------------------------
// Warm night-light color. The darker the room, the brighter
// the light, but never fully invisible.
// ------------------------------------------------------------

void lightUpdateOutput() {

  if (!state.nightMode) {

    lightTurnOff();

    return;
  }

  float intensity = state.darkness / 100.0;

  int brightness =
    LedConfig::MIN_BRIGHTNESS +
    (int)(intensity * LedConfig::BRIGHTNESS_RANGE);

  brightness = constrain(brightness, LedConfig::MIN_BRIGHTNESS, 255);

  state.ledBrightness = brightness;

  int red   = brightness;
  int green = brightness * LedConfig::GREEN_RATIO;
  int blue  = brightness * LedConfig::BLUE_RATIO;

  setRGB(red, green, blue);
}
