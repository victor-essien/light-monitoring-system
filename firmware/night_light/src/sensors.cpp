#include <Arduino.h>

#include "sensors.h"
#include "config.h"
#include "state.h"

void sensorsBegin() {

  pinMode(Pins::LIGHT_SENSOR, INPUT);
  pinMode(Pins::POTENTIOMETER, INPUT);

  analogReadResolution(12);
}


void sensorsRead() {

  state.rawLight = analogRead(Pins::LIGHT_SENSOR);
  state.rawPot = analogRead(Pins::POTENTIOMETER);


  // ----------------------------------------------------------
  // Photosensor reading -> brightness percentage
  //
  //   SENSOR_BRIGHT -> 100% brightness
  //   SENSOR_DARK   ->   0% brightness
  // ----------------------------------------------------------

  state.brightness = map(
    state.rawLight,
    LightConfig::SENSOR_BRIGHT,
    LightConfig::SENSOR_DARK,
    100,
    0
  );

  state.brightness = constrain(state.brightness, 0, 100);

  // Darkness is the inverse
  state.darkness = 100.0 - state.brightness;


  // ----------------------------------------------------------
  // Potentiometer -> darkness threshold (%)
  //
  //   0    -> THRESHOLD_MIN
  //   4095 -> THRESHOLD_MAX
  // ----------------------------------------------------------

  state.threshold = map(
    state.rawPot,
    0,
    4095,
    LightConfig::THRESHOLD_MIN,
    LightConfig::THRESHOLD_MAX
  );

  state.threshold = constrain(
    state.threshold,
    LightConfig::THRESHOLD_MIN,
    LightConfig::THRESHOLD_MAX
  );
}
