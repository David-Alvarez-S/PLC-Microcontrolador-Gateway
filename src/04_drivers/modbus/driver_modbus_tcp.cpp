#include <Arduino.h>
#include <Ethernet.h>
#include <SPI.h>

#include "ModbusServerTCPasync.h"
#include "ModbusTypeDefs.h"

#include "config_global.h"
#include "data_dictionary.h"
#include "logger.h"
#include "protocols.h"

using namespace Modbus;

// ---------- Instancias globales ----------
ModbusServerTCPasync mbServerTCP;

// ---------- Funciones Auxiliares (definidas en driver_modbus_common.cpp)
// ----------
extern uint16_t readModbusRegister(uint16_t regIndex);
extern void writeModbusRegister(uint16_t regIndex, uint16_t value);

// ---------- Handlers Modbus TCP ----------
ModbusMessage handleReadHoldingTCP(ModbusMessage request) {
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

ModbusMessage handleWriteSingleTCP(ModbusMessage request) {
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

ModbusMessage handleWriteMultipleTCP(ModbusMessage request) {
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

  for (uint16_t i = 0; i < quantity; i++) {
    uint16_t value;
    request.get(7 + i * 2, value);
    writeModbusRegister(address + i, value);
  }

  response.add(request.getServerID(), request.getFunctionCode(), address,
               quantity);
  return response;
}

// ---------- Inicialización Modbus TCP ----------
bool modbus_tcp_init(void) {
  // 1. Hardware Reset del W5500
  pinMode(W5500_RST, OUTPUT);
  digitalWrite(W5500_RST, LOW);
  delay(10);
  digitalWrite(W5500_RST, HIGH);
  delay(100);

  // 2. Inicializar bus SPI
  SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);

  // 3. Configurar Ethernet
  uint8_t mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
  IPAddress ip, gateway, subnet, dns;
  ip.fromString(ETH_IP);
  subnet.fromString(ETH_MASK);
  gateway.fromString(ETH_GW);
  dns.fromString(ETH_DNS);

  Ethernet.init(CS_W5500);
  Ethernet.begin(mac, ip, dns, gateway, subnet);

  delay(1000);

  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    log("Error: Controlador W5500 no detectado");
    return false;
  }

  log("Ethernet W5500 listo");

  // 4. Registrar los workers
  mbServerTCP.registerWorker(1, READ_HOLD_REGISTER, &handleReadHoldingTCP);
  mbServerTCP.registerWorker(1, WRITE_HOLD_REGISTER, &handleWriteSingleTCP);
  mbServerTCP.registerWorker(1, WRITE_MULT_REGISTERS, &handleWriteMultipleTCP);

  // 5. Iniciar Servidor TCP
  mbServerTCP.start(502, 4, 2000);

  log("Servidor Modbus TCP activo en el puerto 502");
  return true;
}
