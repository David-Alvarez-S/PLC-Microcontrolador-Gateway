#ifndef HMI_MENU_H
#define HMI_MENU_H

#include <stdint.h>

// Inicializa el estado del menú
void hmi_menu_init(void);

// Actualiza la máquina de estados según los botones (debe llamarse
// periódicamente)
void hmi_menu_update(void);

// Dibuja la pantalla según el estado actual
void hmi_menu_render(void);

#endif
