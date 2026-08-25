#include "hmi_buttons.h"
#include "config_global.h"
#include <Arduino.h>

// Tiempos de debounce y pulsación larga (ms)
#define DEBOUNCE_MS 20
#define LONG_PRESS_MS 1000

// Estructura interna por botón
typedef struct {
  uint8_t pin;
  bool current_stable;     // estado estable actual (false = presionado, true =
                           // suelto)
  bool last_raw;           // última lectura cruda
  uint32_t last_change_ms; // momento del último cambio
  bool pressed_event;      // evento de flanco descendente
  bool released_event;     // evento de flanco ascendente
  bool long_press_event;   // evento de pulsación larga (una vez)
  bool long_press_triggered; // para no repetir el evento
} button_t;

static button_t buttons[BUTTON_COUNT];

// Inicializar botones
void hmi_buttons_init(void) {
  buttons[BUTTON_LEFT_ID].pin = BUTTON_LEFT;
  buttons[BUTTON_RIGHT_ID].pin = BUTTON_RIGHT;
  buttons[BUTTON_ENTER_ID].pin = BUTTON_ENTER;

  for (int i = 0; i < BUTTON_COUNT; i++) {
    pinMode(buttons[i].pin, INPUT);   // espera pull-up externo
    buttons[i].current_stable = true; // asumimos no presionado
    buttons[i].last_raw = digitalRead(buttons[i].pin);
    buttons[i].last_change_ms = millis();
    buttons[i].pressed_event = false;
    buttons[i].released_event = false;
    buttons[i].long_press_event = false;
    buttons[i].long_press_triggered = false;
  }
}

// Escanear botones con debounce
void hmi_buttons_scan(void) {
  uint32_t now = millis();

  for (int i = 0; i < BUTTON_COUNT; i++) {
    bool raw = digitalRead(buttons[i].pin);
    // Si hay cambio crudo, actualizar temporizador
    if (raw != buttons[i].last_raw) {
      buttons[i].last_raw = raw;
      buttons[i].last_change_ms = now;
    }

    // Si ha pasado el tiempo de debounce, actualizar estado estable
    if ((now - buttons[i].last_change_ms) >= DEBOUNCE_MS) {
      bool new_stable = raw; // asumimos lógica activa baja: presionado = LOW
      if (new_stable != buttons[i].current_stable) {
        // Detección de flanco
        if (!new_stable) { // transición a presionado
          buttons[i].pressed_event = true;
          buttons[i].released_event = false;
          buttons[i].long_press_triggered = false;
        } else { // transición a suelto
          buttons[i].released_event = true;
          buttons[i].pressed_event = false;
        }
        buttons[i].current_stable = new_stable;
      }
    }

    // Detección de pulsación larga
    if (!buttons[i].current_stable) { // si está presionado
      if ((now - buttons[i].last_change_ms) >= LONG_PRESS_MS &&
          !buttons[i].long_press_triggered) {
        buttons[i].long_press_event = true;
        buttons[i].long_press_triggered = true;
      }
    }
  }
}

// Obtener estado y limpiar evento
hmi_button_state_t hmi_button_get_state(hmi_button_id_t button) {
  if (button >= BUTTON_COUNT)
    return BUTTON_NONE;
  hmi_button_state_t state = BUTTON_NONE;

  if (buttons[button].pressed_event) {
    buttons[button].pressed_event = false;
    state = BUTTON_PRESSED;
  } else if (buttons[button].released_event) {
    buttons[button].released_event = false;
    state = BUTTON_RELEASED;
  } else if (buttons[button].long_press_event) {
    buttons[button].long_press_event = false;
    state = BUTTON_LONG_PRESSED;
  } else if (!buttons[button].current_stable) {
    state = BUTTON_HELD; // sigue presionado
  }
  return state;
}

// Estado físico del botón (true = presionado)
bool hmi_button_is_pressed(hmi_button_id_t button) {
  if (button >= BUTTON_COUNT)
    return false;
  return !buttons[button].current_stable;
}
