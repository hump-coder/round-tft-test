# round-tft-test
Test project to experiment with a round 1.28" TFT 240x240 GC9A01 display.

## Wiring

Connect the display to the ESP32 using the VSPI bus for best performance.

| Display Pin | ESP32 Pin |
|-------------|-----------|
| `SCL`       | `GPIO18`  |
| `SDA`       | `GPIO23`  |
| `CS`        | `GPIO5`   |
| `DC`        | `GPIO16`  |
| `RST`       | `GPIO17`  |
| `BL` (backlight, if available) | `GPIO21` |

## Building

This project uses [PlatformIO](https://platformio.org/). To build the
firmware run:

```bash
pio run
```

The display will show a clipping test for a few seconds and then animate a
simple star field.
