#include <Arduino.h>
#include <esp_task_wdt.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

// #include "HardwareSerial.h"
#include "config_global.h"
#include "data_dictionary.h"
#include "logger.h"
#include "protocols.h"
// #include "esp32-hal-gpio.h"
#include "freertos/projdefs.h"

TaskHandle_t modbus_tcp_task_handle = NULL;
TaskHandle_t modbus_rtu_rs232_task_handle = NULL;
TaskHandle_t modbus_rtu_rs485_task_handle = NULL;

void setup() {
#ifdef DEBUG_MODE
  Serial.begin(115200);
  delay(500);
  Serial.println("Iniciando comunicacion");
#endif // DEBUG_MODE
  data_dict_init();
  log("Diccionario Creado.");

  rs232_driver_init();
  rs485_driver_init();
  log("Servidor RTU rs232 creado");

  log("Tareas creadas. Núcleo funcionando.");
}

void loop() { vTaskDelay(pdMS_TO_TICKS(1000)); }
