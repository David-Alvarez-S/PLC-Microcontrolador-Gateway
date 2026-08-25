#include "config_global.h"
#include "data_dictionary.h"
#include "logger.h"
#include "protocols.h"
#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

static char rx_buffer[UART_MAX_FRAME_LEN];
static uint16_t rx_index = 0;

// Función para parsear y aplicar la trama
void process_uart_frame(const char *frame, uint16_t len) {
  // Formato: R:<addr>=<value>;R:<addr>=<value>;...
  char *token = strtok((char *)frame, ";");
  while (token != NULL) {
    if (token[0] == 'R' && token[1] == ':') {
      uint16_t addr = atoi(token + 2);
      char *eq = strchr(token, '=');
      if (eq) {
        int32_t value = atoi(eq + 1);
        // Mapear a tag: si addr es 40001 -> índice 0
        uint16_t tag_index = addr - 40001;
        if (tag_index < MAX_TAGS) {
          data_dict_set(tag_index, value, QUALITY_GOOD);
          log("UART: tag actualizado");
        }
      }
    }
    token = strtok(NULL, ";");
  }
}

void uart_gateway_task(void *param) {
  while (1) {
    while (Serial.available()) {
      char c = Serial.read();
      if (c == UART_FRAME_END) {
        rx_buffer[rx_index] = '\0';
        process_uart_frame(rx_buffer, rx_index);
        rx_index = 0;
      } else if (rx_index < UART_MAX_FRAME_LEN - 1) {
        rx_buffer[rx_index++] = c;
      } else {
        rx_index = 0; // overflow, descartar trama
      }
    }
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

void uart_gateway_init(void) {
  Serial.begin(GATEWAY_UART_BAUD, SERIAL_8N1, GATEWAY_UART_RX, GATEWAY_UART_TX);
  xTaskCreatePinnedToCore(uart_gateway_task, "uart_gw", 4096, NULL, 3, NULL, 0);
  log("Gateway UART iniciado");
}
