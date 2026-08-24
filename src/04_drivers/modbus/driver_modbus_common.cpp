#include "ModbusServerRTU.h"
#include "ModbusTypeDefs.h"
#include "config_global.h"
#include "data_dictionary.h"
#include "logger.h"
#include "protocols.h"

// Instancias de servidores (una por UART que usemos)
static ModbusServerRTU mbServer_uart1(2000);
static ModbusServerRTU mbServer_uart2(2000, RS485_DE_RE);

// ---------- Funciones auxiliares ----------
uint16_t readModbusRegister(uint16_t regIndex) {
  DataPoint_t dp;
  if (data_dict_get(regIndex, &dp) && dp.quality == QUALITY_GOOD) {
    return (uint16_t)(dp.value & 0xFFFF);
  }
  return 0;
}

void writeModbusRegister(uint16_t regIndex, uint16_t value) {
  data_dict_set(regIndex, value, QUALITY_GOOD);

  // Si es el registro de comando LED, actuamos directamente
  if (regIndex == (LED_COMMAND_REGISTER - 40001)) {
    digitalWrite(LED_PIN, value ? HIGH : LOW);
    log("Comando LED");
  }
  // Aquí podrías encolar un comando a otro driver según la tabla de mapeo
}

// ---------- Handlers Modbus ----------
ModbusMessage handleReadHolding(ModbusMessage request) {
  uint16_t address, words;
  request.get(2, address);
  request.get(4, words);

  ModbusMessage response;

  if ((address + words) > MAX_TAGS) {
    response.setError(request.getServerID(), request.getFunctionCode(),
                      ILLEGAL_DATA_ADDRESS);
    return response;
  }

  response.add(request.getServerID(), request.getFunctionCode(),
               (uint8_t)(words * 2));
  for (uint16_t i = 0; i < words; i++) {
    response.add(readModbusRegister(address + i));
  }
  return response;
}

ModbusMessage handleWriteSingle(ModbusMessage request) {
  uint16_t address, value;
  request.get(2, address);
  request.get(4, value);

  ModbusMessage response;

  if (address >= MAX_TAGS) {
    response.setError(request.getServerID(), request.getFunctionCode(),
                      ILLEGAL_DATA_ADDRESS);
    return response;
  }

  writeModbusRegister(address, value);

  response.add(request.getServerID(), request.getFunctionCode(), address,
               value);
  return response;
}

ModbusMessage handleWriteMultiple(ModbusMessage request) {
  uint16_t address, quantity;
  uint8_t byteCount;
  request.get(2, address);
  request.get(4, quantity);
  request.get(6, byteCount);

  ModbusMessage response;

  if ((address + quantity) > MAX_TAGS) {
    response.setError(request.getServerID(), request.getFunctionCode(),
                      ILLEGAL_DATA_ADDRESS);
    return response;
  }

  uint16_t offset = 7;
  for (uint16_t i = 0; i < quantity; i++) {
    uint16_t value;
    request.get(offset + i * 2, value);
    writeModbusRegister(address + i, value);
  }

  response.add(request.getServerID(), request.getFunctionCode(), address,
               quantity);
  return response;
}

// ---------- Inicialización genérica ----------
bool modbus_rtu_init(uint8_t uart_num, uint32_t baud, int8_t tx_pin,
                     int8_t rx_pin, uint32_t config, int8_t de_re_pin) {
  HardwareSerial *serial = nullptr;

  switch (uart_num) {
  case 1:
    serial = &Serial1;
    break;
  case 2:
    serial = &Serial2;
    break;
  default:

    log("UART no soportado para Modbus RTU");
    return false;
  }

  // Configurar pin DE/RE si es necesario (RS485)
  if (de_re_pin >= 0) {
    pinMode(de_re_pin, OUTPUT);
    digitalWrite(de_re_pin, LOW); // Iniciar en recepción
    // Nota: la librería eModbus se encargará de conmutar DE/RE automáticamente
    //       si le indicamos el pin al crear el servidor (depende de la
    //       versión). Si no lo soporta, tendremos que hacerlo manualmente en
    //       los handlers.
  }

  // Inicializar UART
  serial->begin(baud, config, rx_pin, tx_pin);

  // Obtener la instancia de servidor adecuada
  ModbusServerRTU *server = nullptr;
  if (uart_num == 1) {
    server = &mbServer_uart1;
  } else if (uart_num == 2) {
    server = &mbServer_uart2;
  }

  if (!server)
    return false;

  // Registrar workers (si no estaban registrados)
  // Lo hacemos solo una vez por instancia
  static bool uart1_workers_registered = false;
  static bool uart2_workers_registered = false;

  if (uart_num == 1 && !uart1_workers_registered) {
    server->registerWorker(RS232_ID, READ_HOLD_REGISTER, &handleReadHolding);
    server->registerWorker(RS232_ID, WRITE_HOLD_REGISTER, &handleWriteSingle);
    server->registerWorker(RS232_ID, Modbus::WRITE_MULT_REGISTERS,
                           &handleWriteMultiple);
    uart1_workers_registered = true;
  } else if (uart_num == 2 && !uart2_workers_registered) {
    server->registerWorker(RS485_ID, READ_HOLD_REGISTER, &handleReadHolding);
    server->registerWorker(RS485_ID, WRITE_HOLD_REGISTER, &handleWriteSingle);
    server->registerWorker(RS232_ID, Modbus::WRITE_MULT_REGISTERS,
                           &handleWriteMultiple);
    uart2_workers_registered = true;
  }

  // Arrancar el servidor (si no está ya corriendo)
  if (uart_num == 1) {
    server->begin(*serial, 1); // prioridad 5, core 1
  } else {
    server->begin(*serial, 1);
  }

  log("Servidor Modbus RTU en UART listo");

  return true;
}
