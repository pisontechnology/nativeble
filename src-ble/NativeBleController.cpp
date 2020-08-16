#include "NativeBleController.h"
#include "NativeBleInternal.h"

#include <iostream>

using namespace NativeBLE;

NativeBleController::NativeBleController() : internal(nullptr) { internal = new NativeBleInternal(); }

NativeBleController::~NativeBleController() {
    if (internal != nullptr) {
        delete internal;
    }
}

void NativeBleController::setup(CallbackHolder callback_holder) { internal->setup(callback_holder); }

void NativeBleController::scan_start() { internal->scan_start(); }

void NativeBleController::scan_stop() { internal->scan_stop(); }

bool NativeBleController::scan_is_active() { return internal->scan_is_active(); }

void NativeBleController::scan_timeout(int32_t timeout_ms) { internal->scan_timeout(timeout_ms); }

bool NativeBleController::is_connected() { return internal->is_connected(); }

void NativeBleController::connect(const BluetoothAddress& address) { internal->connect(address); }

void NativeBleController::write_request(BluetoothUUID service, BluetoothUUID characteristic, DataChunk data) {
    internal->write_request(service, characteristic, data);
}

void NativeBleController::write_command(BluetoothUUID service, BluetoothUUID characteristic, DataChunk data) {
    internal->write_command(service, characteristic, data);
}

void NativeBleController::read(BluetoothUUID service, BluetoothUUID characteristic,
                               std::function<void(const uint8_t* data, uint32_t length)> callback_on_read) {
    internal->read(service, characteristic, callback_on_read);
}

void NativeBleController::notify(BluetoothUUID service, BluetoothUUID characteristic,
                                 std::function<void(const uint8_t* data, uint32_t length)> callback_on_notify) {
    internal->notify(service, characteristic, callback_on_notify);
}

void NativeBleController::indicate(BluetoothUUID service, BluetoothUUID characteristic,
                                   std::function<void(const uint8_t* data, uint32_t length)> callback_on_indicate) {
    internal->indicate(service, characteristic, callback_on_indicate);
}

void NativeBleController::disconnect() { internal->disconnect(); }

void NativeBleController::dispose() { internal->dispose(); }