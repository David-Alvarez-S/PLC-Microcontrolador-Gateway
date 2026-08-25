#ifndef PROTOCOLS_H
#define PROTOCOLS_H

#include "config_global.h"

// ---------- MODBUS RTU ----------
bool modbus_rtu_init(uint8_t uart_num, uint32_t baud, int8_t tx_pin,
                     int8_t rx_pin, uint32_t config, int8_t de_re_pin);
void rs485_driver_init(void);
void rs232_driver_init(void);
// ---------- MODBUS TCP ----------
bool modbus_tcp_init(void);
// ---------- UART ----------
void uart_gateway_init(void);
#endif // PROTOCOLS_H
