#ifndef HMI_CONFIG_NVS_H
#define HMI_CONFIG_NVS_H

#include <stdint.h>

// Variables globales de configuración (se cargan desde NVS)
extern char eth_ip_str[16];
extern char eth_mask_str[16];
extern char eth_gw_str[16];
extern char eth_dns_str[16];
extern uint32_t uart_baud;

// Carga la configuración desde NVS (o valores por defecto si no existe)
void nvs_load_config(void);

// Guarda la configuración actual en NVS
void nvs_save_config(void);

#endif
