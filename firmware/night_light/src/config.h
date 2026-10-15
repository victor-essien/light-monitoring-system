#pragma once
// ============================================================
// Project settings: pins, calibration and timing.
// Change values here, not in the other files.
// ============================================================

// ------------------------------------------------------------
// Pins
// ------------------------------------------------------------
namespace Pins {

  // Analog inputs (ADC1 pins, so they keep working while Wi-Fi is on)
  constexpr int LIGHT_SENSOR  = 34;
  constexpr int POTENTIOMETER = 35;

  // RGB LED
  constexpr int RED   = 4;
  constexpr int GREEN = 5;
  constexpr int BLUE  = 18;
}

// ------------------------------------------------------------
// Light sensor calibration and night-mode behavior
// ------------------------------------------------------------
// Measured sensor range (12-bit ADC, 0-4095):
//   Bright ~ 87
//   Dark   ~ 4095
// See docs/calibration.md to measure your own values.

namespace LightConfig {

  constexpr int SENSOR_BRIGHT = 87;
  constexpr int SENSOR_DARK   = 4095;

  // Potentiometer range mapped to the darkness threshold (%)
  constexpr int THRESHOLD_MIN = 5;
  constexpr int THRESHOLD_MAX = 95;

  // Prevents rapid ON/OFF switching around the threshold.
  constexpr float HYSTERESIS = 3.0;
}

// ------------------------------------------------------------
// Night-light output
// ------------------------------------------------------------
namespace LedConfig {

  // Never fully dim at low darkness, and scale up to 255 at full darkness.
  constexpr int MIN_BRIGHTNESS   = 35;
  constexpr int BRIGHTNESS_RANGE = 220;

  // Warm color: red strong, green medium, blue low.
  constexpr double GREEN_RATIO = 0.45;
  constexpr double BLUE_RATIO  = 0.08;
}

// ------------------------------------------------------------
// Timing and serial
// ------------------------------------------------------------
namespace Timing {

  constexpr unsigned long SERIAL_BAUD         = 115200;
  constexpr unsigned long LOOP_DELAY_MS       = 100;
  constexpr unsigned long WIFI_RETRY_DELAY_MS = 500;
  constexpr int           WIFI_MAX_ATTEMPTS   = 30;
}
