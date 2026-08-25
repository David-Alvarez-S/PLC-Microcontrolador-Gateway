#ifndef HMI_BUTTONS_H
#define HMI_BUTTONS_H

#include <stdbool.h>
#include <stdint.h>

// Identificadores de botones
typedef enum {
  BUTTON_LEFT_ID,
  BUTTON_RIGHT_ID,
  BUTTON_ENTER_ID,
  BUTTON_COUNT
} hmi_button_id_t;

// Estados de pulsación
typedef enum {
  BUTTON_NONE = 0,
  BUTTON_PRESSED,     // flanco descendente (recién presionado)
  BUTTON_RELEASED,    // flanco ascendente (recién soltado)
  BUTTON_HELD,        // se mantiene presionado (después de un tiempo)
  BUTTON_LONG_PRESSED // pulsación larga detectada (una sola vez)
} hmi_button_state_t;

// Inicializa los pines de los botones
void hmi_buttons_init(void);

// Escanea los botones y actualiza estados. Debe llamarse periódicamente.
void hmi_buttons_scan(void);

// Obtiene el estado actual de un botón (y limpia el evento)
hmi_button_state_t hmi_button_get_state(hmi_button_id_t button);

// Devuelve true si el botón está físicamente presionado
bool hmi_button_is_pressed(hmi_button_id_t button);

#endif
