#include "config_global.h"
#include "protocols.h"

void rs232_driver_init() {
  // UART1, 9600 baud, sin pin DE/RE
  bool ok = modbus_rtu_init(1, 9600, RS232_TX, RS232_RX, SERIAL_8N1, -1);
  if (!ok) {
    // Manejar error
  }
}
