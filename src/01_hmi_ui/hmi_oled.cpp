#include "hmi_oled.h"
#include "config_global.h"
#include "logger.h"
#include <Adafruit_SSD1306.h>
#include <Arduino.h>
#include <Wire.h>
#include <stdarg.h>

// Definir dimensiones de la pantalla
#define OLED_WIDTH 128
#define OLED_HEIGHT 32
#define OLED_ADDR 0x3C

// Objeto display
static Adafruit_SSD1306 display(OLED_WIDTH, OLED_HEIGHT, &Wire, -1);

void hmi_oled_init(void) {
  // Inicializar I2C0 con los pines definidos
  Wire.begin(I2C0_SDA, I2C0_SCL, 400000); // 400 kHz

  // Inicializar el display
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    log("Error al iniciar OLED");

    while (1)
      ; // Detener si falla (o manejar de otra forma)
  }

  display.clearDisplay();
  display.setTextSize(1); // Tamaño de fuente normal 1:1
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.display();
}

void hmi_oled_clear(void) { display.clearDisplay(); }

void hmi_oled_set_cursor(uint8_t x, uint8_t y) { display.setCursor(x, y); }

void hmi_oled_print(const char *text) { display.print(text); }

void hmi_oled_printf(const char *format, ...) {
  char buffer[64];
  va_list args;
  va_start(args, format);
  vsnprintf(buffer, sizeof(buffer), format, args);
  va_end(args);
  display.print(buffer);
}

void hmi_oled_display(void) { display.display(); }

void hmi_oled_off(void) { display.ssd1306_command(SSD1306_DISPLAYOFF); }

void hmi_oled_on(void) { display.ssd1306_command(SSD1306_DISPLAYON); }
