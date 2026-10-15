# Troubleshooting

## Upload stops partway ("The chip stopped responding")

The code compiled, but the ESP32 reset during flashing. Try these in order:

1. **Lower the upload speed.** In the Arduino IDE, choose *Tools → Upload Speed → 460800* (or 115200).
2. **Change the cable and port.** Use a data-capable USB cable, plugged straight into the computer rather than a hub.
3. **Disconnect the circuit.** Unplug the LED, sensor and potentiometer wires, upload, then reconnect them. Some GPIOs affect the ESP32 at boot, and this project uses **GPIO 5**, which is one of them.
4. **Hold BOOT.** If the IDE shows `Connecting.....` for a long time, hold the BOOT button until writing starts.
5. **Erase and retry.** Enable *Tools → Erase All Flash Before Sketch Upload* for one upload.

## Brownout Detector (BOD) reset
- Switch USB ports: Plug it directly into a dedicated 5V wall adapter capable of delivering at least 1A to 2A.
- Or plug into a USB charger adapter

## No IP address in the Serial Monitor

- Set the Serial Monitor to **115200 baud**.
- Press the ESP32's EN (reset) button after opening the monitor.
- If it prints "Wi-Fi connection failed.", check `secrets.h`. The network must be **2.4 GHz**, and the name and password are case sensitive.

## The page won't load

- Use `http://`, not `https://`.
- The phone or computer must be on the same Wi-Fi network as the ESP32.
- Some guest networks block devices from talking to each other (client isolation). Try your main network.

## Dashboard says "Reconnecting" or "Disconnected"

- Check the ESP32 still has power and Wi-Fi.
- Browsers can pause background tabs, so reopen the tab.
- Many open dashboards slow the ESP32 down. Close extra tabs.

## Brightness is stuck at 0% or 100%

Recalibrate the sensor. See [calibration.md](calibration.md).

## The light flickers around the threshold

Increase `HYSTERESIS` in `src/config.h` (default 3.0).

## LED colors look wrong, or the light is on when it should be off

- **Inverted behavior:** if the LED is brightest when the pin output is lowest, it is probably a common-anode RGB LED. This firmware assumes a common-cathode LED.
- **Wrong colors:** check which leg is red, green and blue on your LED. The pins are set in `src/config.h`.

## Compile error: `analogWrite` was not declared

Update the ESP32 board package in the Arduino IDE to version 3.0 or newer (*Tools → Board → Boards Manager → esp32*).
