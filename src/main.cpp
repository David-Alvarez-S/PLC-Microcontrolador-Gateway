#include <Arduino.h>
#include <esp_task_wdt.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "config_global.h"
#include "data_dictionary.h"
#include "esp32-hal-gpio.h"
#include "freertos/projdefs.h"

TaskHandle_t writer_task_handle = NULL;
TaskHandle_t reader_task_handle = NULL;
TaskHandle_t led_task_handle = NULL;

void writer_task(void *param) {
  int32_t counter = 0;
  while (1) {
    counter++;
    data_dict_set(0, counter, QUALITY_GOOD);
    Serial.printf("writer: Valor = %d\n", counter);
    vTaskDelay(pdMS_TO_TICKS(1000));
    esp_task_wdt_reset();
  }
}

void reader_task(void *param) {
  DataPoint_t dp;
  int32_t last_value = -1;
  while (1) {
    if (data_dict_get(0, &dp)) {
      if (last_value != dp.value) {
        Serial.printf("Reader: tag 0 = %d, calidad = %d\n", dp.value,
                      dp.quality);
        last_value = dp.value;
      }
    }
    vTaskDelay(pdMS_TO_TICKS(500));
    esp_task_wdt_reset();
  }
}

void led_task(void *param) {
  pinMode(PIN_LED, OUTPUT);
  while (1) {
    digitalWrite(PIN_LED, !digitalRead(PIN_LED));
    vTaskDelay(pdMS_TO_TICKS(500));
    esp_task_wdt_reset();
  }
}

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("Iniciando comunicacion");
  data_dict_init();
  esp_task_wdt_init(WDT_TIMEOUT_SEC, false);
  esp_task_wdt_add(NULL);

  // Crear tareas
  xTaskCreatePinnedToCore(writer_task,          // Función
                          "writer",             // Nombre
                          4096,                 // Stack
                          NULL,                 // Parámetro
                          PRIORITY_TEST_WRITER, // Prioridad
                          &writer_task_handle,  // Handle
                          0                     // Core 0
  );

  xTaskCreatePinnedToCore(reader_task, "reader", 4096, NULL,
                          PRIORITY_TEST_READER, &reader_task_handle,
                          1 // Core 1
  );

  xTaskCreatePinnedToCore(led_task, "led", 2048, NULL, PRIORITY_TEST_LED,
                          &led_task_handle,
                          0 // Core 0
  );

  // Agregar tareas al watchdog
  esp_task_wdt_add(writer_task_handle);
  esp_task_wdt_add(reader_task_handle);
  esp_task_wdt_add(led_task_handle);

  Serial.println("Tareas creadas. Núcleo funcionando.");
}

void loop() {
  // No hacemos nada aquí, las tareas hacen el trabajo
  vTaskDelay(pdMS_TO_TICKS(1000));
  esp_task_wdt_reset(); // reset del watchdog para loopTask
}
