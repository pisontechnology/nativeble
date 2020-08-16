#pragma once

#include <cstdint>

#ifdef _WIN32
#define EXPORT_SYMBOL __declspec(dllexport)
#else
#define EXPORT_SYMBOL
#endif

#ifdef __cplusplus
extern "C" {
#endif

EXPORT_SYMBOL void* ble_construct();
EXPORT_SYMBOL void ble_destruct(void* ble_ptr);

EXPORT_SYMBOL void ble_setup(void* ble_ptr, void (*callback_on_scan_start)(), void (*callback_on_scan_stop)(),
                             void (*callback_on_scan_found)(const char*, const char*),
                             void (*callback_on_device_connected)(),
                             void (*callback_on_device_disconnected)(const char*));

EXPORT_SYMBOL void ble_scan_start(void* ble_ptr);
EXPORT_SYMBOL void ble_scan_stop(void* ble_ptr);
EXPORT_SYMBOL bool ble_scan_is_active(void* ble_ptr);
EXPORT_SYMBOL void ble_scan_timeout(void* ble_ptr, int32_t timeout_ms);

EXPORT_SYMBOL void ble_connect(void* ble_ptr, const char* address_ptr);

EXPORT_SYMBOL void ble_write_request(void* ble_ptr, const char* service, const char* characteristic, const char* data,
                                     uint32_t data_len);
EXPORT_SYMBOL void ble_write_command(void* ble_ptr, const char* service, const char* characteristic, const char* data,
                                     uint32_t data_len);

EXPORT_SYMBOL void ble_read(void* ble_ptr, const char* service, const char* characteristic,
                            void (*callback_on_read)(const uint8_t* data, uint32_t length));
EXPORT_SYMBOL void ble_notify(void* ble_ptr, const char* service, const char* characteristic,
                              void (*callback_on_notify)(const uint8_t* data, uint32_t length));
EXPORT_SYMBOL void ble_indicate(void* ble_ptr, const char* service, const char* characteristic,
                                void (*callback_on_indicate)(const uint8_t* data, uint32_t length));

EXPORT_SYMBOL void ble_disconnect(void* ble_ptr);
EXPORT_SYMBOL void ble_dispose(void* ble_ptr);

#ifdef __cplusplus
}
#endif