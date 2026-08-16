#ifndef CONFIG_GLOBAL_H
#define CONFIG_GLOBAL_H

// ---------- PINES ----------
#define PIN_LED 2
#define DEBUG_UART 0

// ---------- CONFIGURACIONES DEL SISTEMA ----------
#define MAX_TAGS 16        // Cantidad Maxima de Tags
#define WDT_TIMEOUT_SEC 10 // Tiempo para el watch dog timer
// ---------- PRIORIDADES DE TAREAS ----------
#define PRIORITY_TEST_WRITER 2
#define PRIORITY_TEST_READER 2
#define PRIORITY_TEST_LED 1

#endif // CONFIG_GLOBAL_H
