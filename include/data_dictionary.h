#ifndef DATA_DICTIONARY_H
#define DATA_DICTIONARY_H

#include <stdbool.h>
#include <stdint.h>

// ---------- CALIDAD DEL DATO ----------
typedef enum {
  QUALITY_GOOD = 0,     // 0
  QUALITY_STALE,        // 1
  QUALITY_GOOD_TIMEOUT, // 2
  QUALITY_ERROR         // 3
} data_quality_t;
// ---------- ESTRUCTURA DE TAGs CANONICOS ----------
typedef struct {
  uint32_t tag_id;
  int32_t value;
  data_quality_t quality;
  uint32_t last_update_ms;
  bool updated;
} DataPoint_t;
// ---------- PROTOTIPOS ----------
void data_dict_init();
bool data_dict_get(uint32_t tag_id, DataPoint_t *out);
bool data_dict_set(uint32_t tag_id, int32_t value, data_quality_t quality);

#endif // DATA_DICTIONARY_H
