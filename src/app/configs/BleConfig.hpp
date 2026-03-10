#pragma once
#include <cstdint>

/* ============================================================
 * DEVICE CONFIG
 * ============================================================ */

#define BLE_CONF_DEVICE_NAME "J1-Prosthesis"
static const uint32_t BLE_CONF_PASSKEY = 123456;

/* ============================================================
 * SECURITY CONFIGURATION
 * ============================================================ */

/*
 * Bonding  -> Salva as chaves após pareamento
 * MITM     -> Proteção contra ataque Man-in-the-Middle
 * Secure   -> Usa LE Secure Connections (ECDH)
 */
#define BLE_CONF_BONDING  true
#define BLE_CONF_MITM     false     // Desativado para facilitar testes com LightBlue
#define BLE_CONF_SECURE   true

/*
 * IO Capabilities:
    BLE_HS_IO_DISPLAY_ONLY: Show pin on display, 
    BLE_HS_IO_KEYBOARD_ONLY: Input pin via keyboard, 
    BLE_HS_IO_NO_INPUT_OUTPUT: No input or output capabilities, 
    BLE_HS_IO_KEYBOARD_DISPLAY: Both input and output capabilities (display and keyboard) 
    BLE_HS_IO_NO_INPUT_OUTPUT: No input or output capabilities (default for most devices - just works without user interaction)
*/
#define BLE_CONF_SET_SECURITY_IO_CAPS BLE_HS_IO_NO_INPUT_OUTPUT


/* ============================================================
 * CUSTOM SERVICE (128-bit UUID válido)
 * ============================================================ */

/*
 * UUID 128-bit válido (formato padrão BLE)
 */
static constexpr const char* BLE_CONF_MAIN_SERVICE_UUID =
    "12345678-1234-1234-1234-1234567890ab";

static constexpr const char* BLE_CONF_LED_UUID =
    "12345678-1234-1234-1234-1234567890ac";

/* ============================================================
 * BATTERY SERVICE (Padrão BLE)
 * ============================================================ */

/*
 * Serviço padrão Battery Service
 */
#define BLE_CONF_BATTERY_SERVICE_UUID  0x180F

/*
 * Characteristic padrão Battery Level
 */
#define BLE_CONF_BATTERY_LEVEL_UUID    0x2A19



