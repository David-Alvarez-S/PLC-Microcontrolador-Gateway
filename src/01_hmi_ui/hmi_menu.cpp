#include "hmi_menu.h"
#include "config_global.h"
#include "data_dictionary.h"
// #include "hmi_buttons.h"
#include "hmi.h"
#include "logger.h"
#include <Arduino.h>

// Para vista de registros
static int active_tag_ids[MAX_TAGS]; // lista de índices de tags activos
static uint8_t active_tag_count = 0; // número de tags activos
static uint8_t view_start_index =
    0; // índice del primer tag visible en pantalla
//
// Estados del menú
typedef enum {
  STATE_IDLE,
  STATE_MAIN_MENU,
  STATE_VIEW_REGISTERS,
  STATE_EDIT_CONFIG,
  STATE_EDIT_UART,
  STATE_SCREEN_OFF,
  STATE_CONFIRM_SAVE
} menu_state_t;

// Opciones del menú principal
static const char *main_menu_items[] = {"Ver registros", "Configuracion",
                                        "UART", "Pantalla OFF"};
#define MAIN_MENU_COUNT 4

// Campos de configuración de red
static const char *config_fields[] = {"IP", "Mascara", "Gateway", "DNS"};
#define CONFIG_FIELD_COUNT 4

// Variables de estado
static menu_state_t current_state = STATE_IDLE;
static uint8_t main_menu_index = 0;
static uint8_t config_field_index = 0;
static uint8_t digit_index = 0;
static bool editing = false;

// Prototipo de funciones externas de NVS
extern void nvs_load_config(void);
extern void nvs_save_config(void);

// Función para dibujar texto centrado
static void draw_centered(uint8_t y, const char *text) {
  int16_t x = (128 - strlen(text) * 6) / 2;
  hmi_oled_set_cursor(x, y);
  hmi_oled_print(text);
}

static void update_active_tags(void) {
  active_tag_count = 0;
  for (uint16_t i = 0; i < MAX_TAGS; i++) {
    DataPoint_t dp;
    if (data_dict_get(i, &dp)) {
      active_tag_ids[active_tag_count++] = i;
    }
  }
  view_start_index = 0;
}
// Inicializar menú
void hmi_menu_init(void) {
  // Cargar configuración desde NVS (si existe)
  nvs_load_config();
  current_state = STATE_IDLE;
  main_menu_index = 0;
  config_field_index = 0;
  digit_index = 0;
  editing = false;
}

// Obtener el número de tags activos
static uint8_t count_active_tags(void) {
  uint8_t count = 0;
  DataPoint_t dp;
  for (uint16_t i = 0; i < MAX_TAGS; i++) {
    if (data_dict_get(i, &dp)) {
      count++;
    }
  }
  return count;
}

// Renderizar pantalla según estado
void hmi_menu_render(void) {
  hmi_oled_clear();

  switch (current_state) {
  case STATE_IDLE:
    draw_centered(0, "Gateway Industrial");
    hmi_oled_set_cursor(0, 10);
    hmi_oled_printf("IP: %s", eth_ip_str);
    hmi_oled_set_cursor(0, 20);
    hmi_oled_printf("Tags: %d", count_active_tags());
    break;

  case STATE_MAIN_MENU:
    draw_centered(0, "Menu Principal");
    for (uint8_t i = 0; i < MAIN_MENU_COUNT; i++) {
      uint8_t y = 10 + i * 8;
      if (i == main_menu_index) {
        hmi_oled_set_cursor(0, y);
        hmi_oled_print(">");
      }
      hmi_oled_set_cursor(12, y);
      hmi_oled_print(main_menu_items[i]);
    }
    break;

  case STATE_VIEW_REGISTERS:
    draw_centered(0, "Registros activos");
    // Mostrar hasta 3 tags a partir de view_start_index
    for (int row = 0; row < 3; row++) {
      int idx = view_start_index + row;
      if (idx >= active_tag_count)
        break; // no hay más
      uint16_t tag_id = active_tag_ids[idx];
      DataPoint_t dp;
      if (data_dict_get(tag_id, &dp)) {
        uint8_t y = 10 + row * 8;
        hmi_oled_set_cursor(0, y);
        // Formato: "40001: 1234"
        hmi_oled_printf("%5d: %5d", tag_id + 40001, (int)dp.value);
      }
    }
    break;

  case STATE_EDIT_CONFIG:
    draw_centered(0, "Editar Config");
    hmi_oled_set_cursor(0, 10);
    hmi_oled_printf("%s:", config_fields[config_field_index]);
    hmi_oled_set_cursor(0, 20);
    if (editing) {
      // Mostrar cursor sobre el dígito actual (simple)
      hmi_oled_printf("%s", (config_field_index == 0)   ? eth_ip_str
                            : (config_field_index == 1) ? eth_mask_str
                            : (config_field_index == 2) ? eth_gw_str
                                                        : eth_dns_str);
      // No implementamos cursor visual por simplicidad
    } else {
      hmi_oled_printf("%s", (config_field_index == 0)   ? eth_ip_str
                            : (config_field_index == 1) ? eth_mask_str
                            : (config_field_index == 2) ? eth_gw_str
                                                        : eth_dns_str);
    }
    break;

  case STATE_EDIT_UART:
    draw_centered(0, "Editar UART");
    hmi_oled_set_cursor(0, 10);
    hmi_oled_printf("Baudrate: %u", uart_baud);
    break;

  case STATE_SCREEN_OFF:
    hmi_oled_off(); // apagar pantalla
    break;

  case STATE_CONFIRM_SAVE:
    draw_centered(0, "Guardar cambios?");
    draw_centered(12, "ENTER: Si");
    draw_centered(22, "LARGO: No");
    break;
  }

  hmi_oled_display();
}

// Manejar eventos de botones y actualizar estado
void hmi_menu_update(void) {
  hmi_button_state_t left = hmi_button_get_state(BUTTON_LEFT_ID);
  hmi_button_state_t right = hmi_button_get_state(BUTTON_RIGHT_ID);
  hmi_button_state_t enter = hmi_button_get_state(BUTTON_ENTER_ID);

  // Si la pantalla está apagada, cualquier botón la enciende
  if (current_state == STATE_SCREEN_OFF) {
    if (left == BUTTON_PRESSED || right == BUTTON_PRESSED ||
        enter == BUTTON_PRESSED) {
      hmi_oled_on();
      current_state = STATE_IDLE;
    }
    return;
  }

  switch (current_state) {
  case STATE_IDLE:
    if (enter == BUTTON_PRESSED) {
      current_state = STATE_MAIN_MENU;
      main_menu_index = 0;
    }
    break;

  case STATE_MAIN_MENU:
    if (left == BUTTON_PRESSED) {
      if (main_menu_index > 0)
        main_menu_index--;
    } else if (right == BUTTON_PRESSED) {
      if (main_menu_index < MAIN_MENU_COUNT - 1)
        main_menu_index++;
    } else if (enter == BUTTON_PRESSED) {
      switch (main_menu_index) {
      case 0:
        current_state = STATE_VIEW_REGISTERS;
        update_active_tags();
        break;
      case 1:
        current_state = STATE_EDIT_CONFIG;
        config_field_index = 0;
        editing = false;
        break;
      case 2:
        current_state = STATE_EDIT_UART;
        break;
      case 3:
        current_state = STATE_SCREEN_OFF;
        break;
      }
    } else if (enter == BUTTON_LONG_PRESSED) {
      current_state = STATE_IDLE;
    }
    break;

  case STATE_VIEW_REGISTERS:
    if (left == BUTTON_PRESSED) {
      if (view_start_index > 0)
        view_start_index--;
    } else if (right == BUTTON_PRESSED) {
      if (view_start_index < active_tag_count - 1 &&
          view_start_index < active_tag_count - 3) {
        view_start_index++;
      }
    } else if (enter == BUTTON_LONG_PRESSED) {
      current_state = STATE_MAIN_MENU;
    }
    break;

  case STATE_EDIT_CONFIG:
    if (editing) {
      // Modificar dígito actual (solo para IP como ejemplo)
      if (left == BUTTON_PRESSED) {
        // decrementar dígito
      } else if (right == BUTTON_PRESSED) {
        // incrementar dígito
      } else if (enter == BUTTON_PRESSED) {
        // siguiente dígito o terminar
      }
      // Simplificación: no implementamos edición real ahora
    } else {
      if (left == BUTTON_PRESSED) {
        if (config_field_index > 0)
          config_field_index--;
      } else if (right == BUTTON_PRESSED) {
        if (config_field_index < CONFIG_FIELD_COUNT - 1)
          config_field_index++;
      } else if (enter == BUTTON_PRESSED) {
        editing = true;
        digit_index = 0;
      } else if (enter == BUTTON_LONG_PRESSED) {
        current_state = STATE_MAIN_MENU;
      }
    }
    break;

  case STATE_EDIT_UART:
    if (left == BUTTON_PRESSED) {
      uart_baud -= 1200;
    } else if (right == BUTTON_PRESSED) {
      uart_baud += 1200;
    } else if (enter == BUTTON_PRESSED) {
      current_state = STATE_CONFIRM_SAVE;
    } else if (enter == BUTTON_LONG_PRESSED) {
      current_state = STATE_MAIN_MENU;
    }
    break;

  case STATE_CONFIRM_SAVE:
    if (enter == BUTTON_PRESSED) {
      nvs_save_config();
      ESP.restart();
    } else if (enter == BUTTON_LONG_PRESSED) {
      current_state = STATE_MAIN_MENU;
    }
    break;

  default:
    break;
  }
}
