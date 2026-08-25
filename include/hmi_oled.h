#ifndef HMI_OLED_H
#define HMI_OLED_H

#include <stddef.h>
#include <stdint.h>

// Inicializa la pantalla OLED (I2C0, dirección 0x3C)
void hmi_oled_init(void);

// Limpia la pantalla
void hmi_oled_clear(void);

// Establece el cursor en coordenadas (columna, fila) en píxeles
void hmi_oled_set_cursor(uint8_t x, uint8_t y);

// Imprime texto en la posición actual
void hmi_oled_print(const char *text);

// Imprime texto con formato (usa printf)
void hmi_oled_printf(const char *format, ...);

// Actualiza el display (envía el buffer a la pantalla)
void hmi_oled_display(void);

// Apaga la pantalla (modo bajo consumo)
void hmi_oled_off(void);

// Enciende la pantalla
void hmi_oled_on(void);

#endif
