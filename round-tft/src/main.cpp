#include <Arduino.h>
#include <LovyanGFX.hpp>
#include <math.h>

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
LGFX_Sprite canvas(&tft);  // off-screen buffer to reduce flicker

enum DemoMode {
  DEMO_STARFIELD,
  DEMO_CLOCK,
  DEMO_PLASMA
};

// Change this constant to pick which demo runs
static const DemoMode DEMO_MODE = DEMO_PLASMA;

static uint16_t color565(uint8_t r, uint8_t g, uint8_t b) {
  return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

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

static void runStarfield() {
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

static void drawHand(float angleDeg, int length, uint16_t color) {
  float rad = angleDeg * DEG_TO_RAD;
  int x = CENTER + (int)(sinf(rad) * length);
  int y = CENTER - (int)(cosf(rad) * length);
  canvas.drawLine(CENTER, CENTER, x, y, color);
}

static void runClock() {
  canvas.fillScreen(TFT_BLACK);
  canvas.drawCircle(CENTER, CENTER, RADIUS - 1, TFT_WHITE);
  for (int i = 0; i < 12; ++i) {
    float a = i * 30 * DEG_TO_RAD;
    int x1 = CENTER + (int)((RADIUS - 10) * sinf(a));
    int y1 = CENTER - (int)((RADIUS - 10) * cosf(a));
    int x2 = CENTER + (int)((RADIUS - 2) * sinf(a));
    int y2 = CENTER - (int)((RADIUS - 2) * cosf(a));
    canvas.drawLine(x1, y1, x2, y2, TFT_WHITE);
  }

  uint32_t seconds = millis() / 1000;
  int s = seconds % 60;
  int m = (seconds / 60) % 60;
  int h = (seconds / 3600) % 12;

  drawHand(h * 30 + m * 0.5f, RADIUS - 50, TFT_WHITE);
  drawHand(m * 6 + s * 0.1f, RADIUS - 30, TFT_WHITE);
  drawHand(s * 6, RADIUS - 20, TFT_RED);
  canvas.fillCircle(CENTER, CENTER, 3, TFT_WHITE);
  canvas.pushSprite(0, 0);
}

static void runPlasma() {
  static float t = 0.0f;
  t += 0.05f;
  canvas.startWrite();
  for (int y = 0; y < 240; ++y) {
    for (int x = 0; x < 240; ++x) {
      int dx = x - CENTER;
      int dy = y - CENTER;
      if (dx * dx + dy * dy <= RADIUS * RADIUS) {
        float v = sinf(dx * 0.05f + t) + sinf(dy * 0.05f + t) + sinf((dx + dy) * 0.05f + t);
        uint8_t r = sinf(v + t) * 127 + 128;
        uint8_t g = sinf(v + t + 2.1f) * 127 + 128;
        uint8_t b = sinf(v + t + 4.2f) * 127 + 128;
        canvas.drawPixel(x, y, color565(r, g, b));
      } else {
        canvas.drawPixel(x, y, TFT_BLACK);
      }
    }
  }
  canvas.endWrite();
  canvas.pushSprite(0, 0);
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
  canvas.setColorDepth(16);
  if (!canvas.createSprite(240, 240)) {
    Serial.println("Failed to allocate sprite buffer");
  }
  canvas.setSwapBytes(true);
  canvas.fillScreen(TFT_BLACK);

  drawClippingTest();
  Serial.println("Clipping test displayed");
  tft.clearClipRect();
  delay(3000);  // view clipping test for 3 seconds
  if (DEMO_MODE == DEMO_STARFIELD) {
    initStars();
    Serial.println("Stars initialized");
  }
}

void loop() {
  static unsigned long lastPrint = 0;
  if (millis() - lastPrint > 1000) {
    Serial.println("Loop running");
    lastPrint = millis();
  }
  switch (DEMO_MODE) {
    case DEMO_STARFIELD:
      runStarfield();
      break;
    case DEMO_CLOCK:
      runClock();
      break;
    case DEMO_PLASMA:
      runPlasma();
      break;
  }
}
