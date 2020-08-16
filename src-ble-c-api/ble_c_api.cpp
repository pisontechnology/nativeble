#include "ble_c_api.h"

#include <iostream>
#include <string>

#include "NativeBleController.h"

void* ble_construct() {
    void* ble_ptr = new NativeBLE::NativeBleController();
    return ble_ptr;
}

void ble_destruct(void* ble_ptr) {
    NativeBLE::NativeBleController* ble = (NativeBLE::NativeBleController*)ble_ptr;
    delete ble;
}

void ble_setup(void* ble_ptr, void (*callback_on_scan_start)(), void (*callback_on_scan_stop)(),
               void (*callback_on_scan_found)(const char*, const char*), void (*callback_on_device_connected)(),
               void (*callback_on_device_disconnected)(const char*)) {
    NativeBLE::NativeBleController* ble = (NativeBLE::NativeBleController*)ble_ptr;
    NativeBLE::CallbackHolder callback_holder;

    if (callback_on_scan_start != nullptr) {
        callback_holder.callback_on_scan_start = [=]() { callback_on_scan_start(); };
    }

    if (callback_on_scan_stop != nullptr) {
        callback_holder.callback_on_scan_stop = [=]() { callback_on_scan_stop(); };
    }

    if (callback_on_scan_found != nullptr) {
        callback_holder.callback_on_scan_found = [=](NativeBLE::DeviceDescriptor descriptor) {
            callback_on_scan_found(descriptor.name.c_str(), descriptor.address.c_str());
        };
    }

    if (callback_on_device_connected != nullptr) {
        callback_holder.callback_on_device_connected = [=]() { callback_on_device_connected(); };
    }

    if (callback_on_device_disconnected != nullptr) {
        callback_holder.callback_on_device_disconnected = [=](std::string reason) {
            callback_on_device_disconnected(reason.c_str());
        };
    }

    ble->setup(callback_holder);
}

void ble_scan_start(void* ble_ptr) {
    NativeBLE::NativeBleController* ble = (NativeBLE::NativeBleController*)ble_ptr;
    ble->scan_start();
}

void ble_scan_stop(void* ble_ptr) {
    NativeBLE::NativeBleController* ble = (NativeBLE::NativeBleController*)ble_ptr;
    ble->scan_stop();
}

bool ble_scan_is_active(void* ble_ptr) {
    NativeBLE::NativeBleController* ble = (NativeBLE::NativeBleController*)ble_ptr;
    return ble->scan_is_active();
}

void ble_scan_timeout(void* ble_ptr, int32_t timeout_ms) {
    NativeBLE::NativeBleController* ble = (NativeBLE::NativeBleController*)ble_ptr;
    ble->scan_timeout(timeout_ms);
}

void ble_connect(void* ble_ptr, const char* address_ptr) {
    NativeBLE::NativeBleController* ble = (NativeBLE::NativeBleController*)ble_ptr;
    std::string address(address_ptr);
    ble->connect(address);
}

void ble_write_request(void* ble_ptr, const char* service_ptr, const char* characteristic_ptr, const char* data_ptr,
                       uint32_t data_len) {
    NativeBLE::NativeBleController* ble = (NativeBLE::NativeBleController*)ble_ptr;
    std::string service(service_ptr);
    std::string characteristic(characteristic_ptr);
    std::string data(data_ptr, data_len);

    ble->write_request(service, characteristic, data);
}

void ble_write_command(void* ble_ptr, const char* service_ptr, const char* characteristic_ptr, const char* data_ptr,
                       uint32_t data_len) {
    NativeBLE::NativeBleController* ble = (NativeBLE::NativeBleController*)ble_ptr;
    std::string service(service_ptr);
    std::string characteristic(characteristic_ptr);
    std::string data(data_ptr, data_len);

    ble->write_command(service, characteristic, data);
}

void ble_read(void* ble_ptr, const char* service_ptr, const char* characteristic_ptr,
              void (*callback)(const uint8_t* data, uint32_t length)) {
    NativeBLE::NativeBleController* ble = (NativeBLE::NativeBleController*)ble_ptr;
    std::string service(service_ptr);
    std::string characteristic(characteristic_ptr);
    ble->read(service, characteristic, [=](const uint8_t* data, uint32_t length) { callback(data, length); });
}

void ble_notify(void* ble_ptr, const char* service_ptr, const char* characteristic_ptr,
                void (*callback)(const uint8_t* data, uint32_t length)) {
    NativeBLE::NativeBleController* ble = (NativeBLE::NativeBleController*)ble_ptr;
    std::string service(service_ptr);
    std::string characteristic(characteristic_ptr);
    ble->notify(service, characteristic, [=](const uint8_t* data, uint32_t length) { callback(data, length); });
}

void ble_indicate(void* ble_ptr, const char* service_ptr, const char* characteristic_ptr,
                  void (*callback)(const uint8_t* data, uint32_t length)) {
    NativeBLE::NativeBleController* ble = (NativeBLE::NativeBleController*)ble_ptr;
    std::string service(service_ptr);
    std::string characteristic(characteristic_ptr);
    ble->indicate(service, characteristic, [=](const uint8_t* data, uint32_t length) { callback(data, length); });
}

void ble_disconnect(void* ble_ptr) {
    NativeBLE::NativeBleController* ble = (NativeBLE::NativeBleController*)ble_ptr;
    ble->disconnect();
}
void ble_dispose(void* ble_ptr) {
    NativeBLE::NativeBleController* ble = (NativeBLE::NativeBleController*)ble_ptr;
    ble->dispose();
}
