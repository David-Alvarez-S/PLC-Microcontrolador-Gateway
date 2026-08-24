#include "config_global.h"
#include "protocols.h"

void rs485_driver_init() {
  // UART2, 9600 baud, con pin DE/RE
  bool ok =
      modbus_rtu_init(2, 9600, RS485_TX, RS485_RX, SERIAL_8N1, RS485_DE_RE);
  if (!ok) {
    // Manejar error
  }
}
