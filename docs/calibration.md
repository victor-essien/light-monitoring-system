# Calibrating the light sensor

Different LDRs and resistors give different readings, so the brightness percentage is only accurate once the firmware knows your sensor's range. The values live in `firmware/night_light/src/config.h`:

```cpp
constexpr int SENSOR_BRIGHT = 87;    // reading in bright light
constexpr int SENSOR_DARK   = 4095;  // reading in the dark
```

## Measure your range

You don't need to change any code to do this. The `/api` route already reports the raw reading.

1. Upload the firmware and open `http://<esp32-ip>/api` in a browser.
2. **Bright reading.** Put the sensor in the brightest light you expect (for example, near a window or under a lamp). Refresh and note `rawLight`.
3. **Dark reading.** Cover the sensor completely. Refresh and note `rawLight`.
4. Put those two numbers into `SENSOR_BRIGHT` and `SENSOR_DARK`, then upload again.

## Signs your calibration is off

- Brightness is stuck at 0% or 100% in normal rooms
- The light never turns on, or turns on in a lit room
- The dashboard ring barely moves when you cover the sensor

## Notes

- Higher raw numbers mean **darker**. The firmware assumes this, which is why `SENSOR_BRIGHT` is the smaller number. If your readings go the other way, your sensor divider is wired the opposite way round (see the [hardware notes](../hardware/README.md)).
- Readings drift a little with temperature and supply voltage. Take a few readings and use typical values rather than a single spike.
