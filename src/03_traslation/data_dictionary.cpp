#include "data_dictionary.h"
#include "config_global.h"
#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

static DataPoint_t data_points[MAX_TAGS];
static SemaphoreHandle_t data_mutex = NULL;

void data_dict_init(void) {
  data_mutex = xSemaphoreCreateMutex();
  for (int i = 0; i < MAX_TAGS; i++) {
    data_points[i].tag_id = 0xFFFFFFFF; // Invalido
    data_points[i].value = 0;
    data_points[i].quality = QUALITY_STALE;
    data_points[i].last_update_ms = 0;
    data_points[i].updated = false;
  }
}

bool data_dict_get(uint32_t tag_id, DataPoint_t *out) {
  if (data_mutex == NULL || out == NULL)
    return false; // Si la estructura es nula o no hay mutex devuelve falso
  bool found = false;
  if (xSemaphoreTake(data_mutex, portMAX_DELAY) ==
      pdTRUE) { // Si esta libre el semaforo:
    for (int i = 0; i < MAX_TAGS;
         i++) { // Buscar si hay un id como el ingresado
      if (data_points[i].tag_id == tag_id) {
        *out = data_points[i]; // Si hay el id buscado devolver el la estructura
                               // al out
        found = true;
        break;
      }
    }
    xSemaphoreGive(data_mutex); // Devolvemos el semaforo
  }
  return found;
}

bool data_dict_set(uint32_t tag_id, int32_t value, data_quality_t quality) {
  if (data_mutex == NULL)
    return false; // Si no se creo el mutex devolvemos null
  bool success = false;
  if (xSemaphoreTake(data_mutex, portMAX_DELAY) ==
      pdTRUE) { // Si se agarro el semaforo

    int idx = -1;

    for (int i = 0; i < MAX_TAGS; i++) { // Si existe el tag idx = indice
      if (data_points[i].tag_id == tag_id) {
        idx = i;
        break;
      }
    }

    if (idx == -1) {
      for (int i = 0; i < MAX_TAGS;
           i++) { // Si no existe el tag se busca un indice libre
        if (data_points[i].tag_id == 0xFFFFFFFF) {
          idx = i;
          data_points[i].tag_id = tag_id;
          break;
        }
      }
    }

    if (idx != -1) {
      data_points[idx].value = value;
      data_points[idx].quality = quality;
      data_points[idx].last_update_ms = millis();
      data_points[idx].updated = true;
      success = true;
    }
    xSemaphoreGive(data_mutex);
  }
  return success;
}
