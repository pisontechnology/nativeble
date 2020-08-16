#pragma once

#include <atomic>
#include <thread>

#include "NativeBleControllerTypes.h"
#include "bluezdbus/BluezService.h"

namespace NativeBLE {

class NativeBleInternal {
  private:
    BluezService bluez_service;
    std::shared_ptr<BluezAdapter> adapter;
    std::shared_ptr<BluezDevice> device;

    std::thread *async_thread;
    volatile bool async_thread_active = true;
    void async_thread_function();

    CallbackHolder callback_holder;
    BluetoothAddress format_mac_address(std::string address);

  public:
    NativeBleInternal();
    ~NativeBleInternal();

    void setup(CallbackHolder callback_holder);

    void scan_start();
    void scan_stop();
    bool scan_is_active();
    void scan_timeout(int32_t timeout_ms);

    bool is_connected();

    void connect(const BluetoothAddress &address);

    void write_request(BluetoothUUID service, BluetoothUUID characteristic, DataChunk &data);
    void write_command(BluetoothUUID service, BluetoothUUID characteristic, DataChunk &data);

    void read(BluetoothUUID service, BluetoothUUID characteristic,
              std::function<void(const uint8_t *data, uint32_t length)> callback_on_read);
    void notify(BluetoothUUID service, BluetoothUUID characteristic,
                std::function<void(const uint8_t *data, uint32_t length)> callback_on_notify);
    void indicate(BluetoothUUID service, BluetoothUUID characteristic,
                  std::function<void(const uint8_t *data, uint32_t length)> callback_on_indicate);

    void disconnect();
    void dispose();
};

}  // namespace NativeBLE