# API reference

The ESP32 serves two routes on port 80.

| Route | Method | Returns |
|---|---|---|
| `/` | GET | The dashboard page (HTML) |
| `/api` | GET | Live readings as JSON |

## `GET /api`

Example response:

```json
{
  "rawLight": 2048,
  "rawPot": 1900,
  "brightness": 50.0,
  "darkness": 50.0,
  "threshold": 46.0,
  "nightMode": true,
  "ledBrightness": 145,
  "uptime": 183422
}
```

| Field | Type | Range | Meaning |
|---|---|---|---|
| `rawLight` | integer | 0–4095 | Raw ADC reading from the light sensor |
| `rawPot` | integer | 0–4095 | Raw ADC reading from the potentiometer |
| `brightness` | number | 0–100 | Room brightness in percent |
| `darkness` | number | 0–100 | Room darkness in percent (`100 - brightness`) |
| `threshold` | number | 5–95 | Darkness level that switches the light on, in percent |
| `nightMode` | boolean | true / false | Whether the night light is active |
| `ledBrightness` | integer | 0–255 | LED output level (0 when off) |
| `uptime` | integer | 0 and up | Milliseconds since the ESP32 started |

The dashboard uses `brightness`, `darkness`, `threshold`, `ledBrightness` and `nightMode`. The other fields are there for debugging and calibration.

## Try it

```bash
curl http://<esp32-ip>/api
```

You can also open `http://<esp32-ip>/api` in a browser. Reading `rawLight` here is how you calibrate the sensor (see [calibration.md](calibration.md)).

## Notes

- There is no authentication. Keep the ESP32 on a trusted network.
- The API is read-only. Settings are changed with the physical potentiometer.
- Each request is handled one at a time, so a few open dashboards are fine, but dozens are not.
