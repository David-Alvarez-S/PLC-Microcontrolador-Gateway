#include "hmi_config_nvs.h"
#include "config_global.h"
#include <Arduino.h>
#include <Preferences.h>

// Instancia de Preferences (espacio de nombres "gateway")
static Preferences preferences;

// Variables globales de configuración
char eth_ip_str[16] = ETH_IP;
char eth_mask_str[16] = ETH_MASK;
char eth_gw_str[16] = ETH_GW;
char eth_dns_str[16] = ETH_DNS;
uint32_t uart_baud = 115200;

// Nombres de claves en NVS
static const char *NVS_NAMESPACE = "gateway";
static const char *KEY_IP = "eth_ip";
static const char *KEY_MASK = "eth_mask";
static const char *KEY_GW = "eth_gw";
static const char *KEY_DNS = "eth_dns";
static const char *KEY_UART_BAUD = "uart_baud";

void nvs_load_config(void) {
  preferences.begin(NVS_NAMESPACE, true); // modo lectura

  // Si existen claves, cargar; si no, mantener valores por defecto
  if (preferences.isKey(KEY_IP)) {
    strlcpy(eth_ip_str, preferences.getString(KEY_IP).c_str(),
            sizeof(eth_ip_str));
  }
  if (preferences.isKey(KEY_MASK)) {
    strlcpy(eth_mask_str, preferences.getString(KEY_MASK).c_str(),
            sizeof(eth_mask_str));
  }
  if (preferences.isKey(KEY_GW)) {
    strlcpy(eth_gw_str, preferences.getString(KEY_GW).c_str(),
            sizeof(eth_gw_str));
  }
  if (preferences.isKey(KEY_DNS)) {
    strlcpy(eth_dns_str, preferences.getString(KEY_DNS).c_str(),
            sizeof(eth_dns_str));
  }
  if (preferences.isKey(KEY_UART_BAUD)) {
    uart_baud = preferences.getUInt(KEY_UART_BAUD, uart_baud);
  }

  preferences.end();
}

void nvs_save_config(void) {
  preferences.begin(NVS_NAMESPACE, false); // modo escritura

  preferences.putString(KEY_IP, eth_ip_str);
  preferences.putString(KEY_MASK, eth_mask_str);
  preferences.putString(KEY_GW, eth_gw_str);
  preferences.putString(KEY_DNS, eth_dns_str);
  preferences.putUInt(KEY_UART_BAUD, uart_baud);

  preferences.end();
}
