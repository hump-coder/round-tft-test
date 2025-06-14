#include <Arduino.h>
#include <LovyanGFX.hpp>

class LGFX : public lgfx::LGFX_Device {
  lgfx::Panel_GC9A01 _panel;  // display driver class
  lgfx::Bus_SPI _bus;         // SPI bus
  lgfx::Light_PWM _light;     // backlight control

public:
  LGFX(void) {
    auto cfg = _bus.config();
    cfg.spi_host = VSPI_HOST;
    cfg.spi_mode = 0;
    cfg.freq_write = 40000000; // 40MHz SPI write
    cfg.freq_read  = 16000000;
    cfg.dma_channel = 1;       // use DMA channel 1
    cfg.pin_sclk = 18;         // SCL
    cfg.pin_mosi = 23;         // SDA/MOSI
    cfg.pin_miso = -1;         // not used
    cfg.pin_dc   = 16;         // D/C
    _bus.config(cfg);
    _panel.setBus(&_bus);

    auto p_cfg = _panel.config();
    p_cfg.invert = true;       // typical for GC9A01
    p_cfg.panel_width = 240;
    p_cfg.panel_height = 240;
    p_cfg.offset_x = 0;
    p_cfg.offset_y = 0;
    p_cfg.pin_cs   = 5;
    p_cfg.pin_rst  = 17;
    _panel.config(p_cfg);

    auto l_cfg = _light.config();
    l_cfg.pin_bl = 21;         // backlight pin
    l_cfg.invert = false;
    l_cfg.freq   = 44100;
    l_cfg.pwm_channel = 7;
    _light.config(l_cfg);
    _panel.setLight(&_light);

    setPanel(&_panel);
  }
};

LGFX tft;  // create display object

struct Star {
  float x;
  float y;
  float z;
};

static const int NUM_STARS = 80;
static Star stars[NUM_STARS];
static const float STAR_SPEED = 0.6f;
static const int CENTER = 120;
static const int RADIUS = 120;

static void resetStar(Star &s) {
  s.x = random(-RADIUS, RADIUS);
  s.y = random(-RADIUS, RADIUS);
  s.z = random(1, RADIUS);
}

static void initStars() {
  for (int i = 0; i < NUM_STARS; ++i) {
    resetStar(stars[i]);
  }
}

static void drawClippingTest() {
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.setTextDatum(MC_DATUM);
  tft.drawString("Clipping test", CENTER, CENTER);

  // draw diagonals
  tft.drawLine(0, 0, 239, 239, TFT_RED);
  tft.drawLine(0, 239, 239, 0, TFT_RED);

  // draw nested rectangles to visualize corners
  for (int r = 0; r < CENTER; r += 20) {
    tft.drawRect(r, r, 240 - 2 * r, 240 - 2 * r, TFT_BLUE);
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("Starting setup...");
  tft.begin();
  Serial.println("Display initialized");
  tft.setRotation(0);
  Serial.println("Rotation set");
  tft.setBrightness(200);
  Serial.println("Brightness set (if supported)");

  drawClippingTest();
  Serial.println("Clipping test displayed");
  delay(3000);  // view clipping test for 3 seconds
  initStars();
  Serial.println("Stars initialized");
}

void loop() {
  static unsigned long lastPrint = 0;
  if (millis() - lastPrint > 1000) {
    Serial.println("Loop running");
    lastPrint = millis();
  }
  tft.fillScreen(TFT_BLACK);
  tft.startWrite();
  for (int i = 0; i < NUM_STARS; ++i) {
    Star &s = stars[i];
    s.z -= STAR_SPEED;
    if (s.z <= 1) {
      resetStar(s);
      continue;
    }
    int sx = CENTER + (int)(s.x / s.z * CENTER);
    int sy = CENTER + (int)(s.y / s.z * CENTER);
    if (sx >= 0 && sx < 240 && sy >= 0 && sy < 240) {
      int dx = sx - CENTER;
      int dy = sy - CENTER;
      if (dx * dx + dy * dy <= RADIUS * RADIUS) {
        tft.drawPixel(sx, sy, TFT_WHITE);
      }
    }
  }
  tft.endWrite();
}
