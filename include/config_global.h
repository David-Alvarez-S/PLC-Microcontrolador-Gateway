#ifndef CONFIG_GLOBAL_H
#define CONFIG_GLOBAL_H

#include <Arduino.h>
// ---------- PINES ----------
#define GATEWAY_UART_NUM 0 // UART0 - Debug/ Salida UART
#define RS232_UART_NUM 1   // UART1 - RS232 (MAX3232)
#define RS485_UART_NUM 2   // UART2 - RS485 (MAX485)
// RS232 (MAX3232)
#define RS232_TX 17
#define RS232_RX 16
// RS485 (MAX485)
#define RS485_TX 14
#define RS485_RX 34
#define RS485_DE_RE 32
// SPI
#define CS_W5500 25
#define CS_MCP2515 26
#define CS_SHIFTREGISTER 27
#define CS_SALIDA_GATEWAY 33
// Buttons
#define BUTTON_LEFT 4
#define BUTTON_RIGHT 36
// ---------- CONFIGURACIONES DEL SISTEMA ----------
// DEBUG MODE
#define DEBUG_MODE        // Para mostrar Logs en pantalla
#define MAX_TAGS 16       // Cantidad Maxima de Tags
#define WDT_TIMEOUT_SEC 7 // Tiempo para el watch dog timer
#define RS232_ID 1
#define RS485_ID 2
#define LED_PIN 5
#define LED_COMMAND_REGISTER 40010

// ---------- PRIORIDADES DE TAREAS ----------
#define PRIORITY_TEST_WRITER 2
#define PRIORITY_TEST_READER 2
#define PRIORITY_MODBUS 5

// Ethernet (W5500)
#define MY_ETH_PHY_TYPE 1
#define MY_ETH_PHY_ADDR 1
#define ETH_IP "192.168.1.100"
#define ETH_MASK "255.255.255.0"
#define ETH_GW "192.168.1.1"
#define ETH_DNS "8.8.8.8"

// Pin de reset del W5500
#define W5500_RST 13

// Pines SPI (por si necesitás ser explícito)
#define SPI_SCK 18
#define SPI_MISO 19
#define SPI_MOSI 23

#define I2C0_SDA 21
#define I2C0_SCL 22

#endif // CONFIG_GLOBAL_H
