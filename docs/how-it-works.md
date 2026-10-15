# How it works

Every loop (about every 100 ms) the firmware does four things, in this order:

1. **Read sensors**: `sensorsRead()` in `src/sensors.cpp`
2. **Decide day or night**: `lightUpdateMode()` in `src/light_control.cpp`
3. **Drive the LED**: `lightUpdateOutput()` in `src/light_control.cpp`
4. **Serve web requests**: `webHandle()` in `src/web_server.cpp`

## 1. Reading the sensors

The ESP32 reads both inputs with its 12-bit ADC, so values run from 0 to 4095.

- **Light sensor** (GPIO 34). The raw value is mapped linearly to a brightness percentage using two calibration points from `config.h`:
  - `SENSOR_BRIGHT` (87) maps to 100% brightness
  - `SENSOR_DARK` (4095) maps to 0% brightness

  Darkness is simply `100 - brightness`.
- **Potentiometer** (GPIO 35). The raw value is mapped to a threshold between 5% and 95%, so the knob can never be set to "always on" or "never on".

Both pins are on ADC1, which keeps working while Wi-Fi is active.

## 2. Day or night, with hysteresis

The decision compares **darkness** with the **threshold**:

| Current mode | Switches when |
|---|---|
| Day | darkness is **at or above** the threshold, then night starts |
| Night | darkness drops **below threshold minus 3**, then day starts |

The 3-point gap (`HYSTERESIS` in `config.h`) stops the light flickering when the room sits right at the threshold. For example, with a threshold of 50%, the light turns on at 50% darkness and only turns off below 47%.

## 3. LED output

In day mode the LED is off. In night mode:

- **Brightness** = `35 + darkness × 2.2`, limited to 35–255. The light is never fully invisible, and it gets brighter as the room gets darker.
- **Color** is a warm amber: red at full brightness, green at 45% of it, blue at 8%.

All of these numbers live in the `LedConfig` namespace in `config.h`.

## 4. The dashboard

The ESP32 serves one self-contained page (`src/web/dashboard.h`) at `/`. The page then asks `/api` for fresh readings about twice a second.

- The **lamp** glows in proportion to the real LED output (`ledBrightness`), not just the mode.
- The **ring** shows brightness, with darkness and the threshold beside it.
- The **history chart** keeps the last two minutes in the browser's memory. It draws brightness, shaded stretches where the light was on, and a dashed line showing where the light turns on. That line is drawn at `100 - threshold`, because darkness is `100 - brightness`.
- The **connection pill** shows Connecting, Connected, Reconnecting or Disconnected, and how old the last reading is. The age uses the browser's clock, not an ESP32 timestamp.

History is not stored on the ESP32. Refreshing the page starts a new chart.

## Shared state

`src/state.h` holds one `LightState` struct. Sensors write to it, light control and the web server read from it. Keeping it in one place means each module has a single job and the JSON API always matches what the LED is doing.
