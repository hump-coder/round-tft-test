# round-tft-test
Test project to experiment with a round 1.28" TFT 240x240 GC9A01 display.
It targets the Wemos **LOLIN S2 mini** (ESP32‑S2) board with PSRAM enabled.

## Wiring

Connect the display to the S2 mini using the general purpose **SPI2** bus for best performance.

| Display Pin | ESP32 Pin |
|-------------|-----------|
| `SCL`       | `GPIO7`   |
| `SDA`       | `GPIO11`  |
| `CS`        | `GPIO12`  |
| `DC`        | `GPIO6`   |
| `RST`       | `GPIO13`  |
| `BL` (backlight, if available) | `GPIO5`  |

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
