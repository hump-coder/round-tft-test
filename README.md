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

After a short clipping test the firmware will run one of several demos:

- **Star field** – the original 3D star animation.
- **Clock** – a simple analogue watch face.
- **Plasma** – a colourful swirling plasma effect.

Edit the `DEMO_MODE` constant in `src/main.cpp` to choose which demo is
displayed.
