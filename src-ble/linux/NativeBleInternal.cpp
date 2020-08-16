#include "NativeBleInternal.h"
#include <sstream>

using namespace NativeBLE;

NativeBleInternal::NativeBleInternal() : async_thread(nullptr), async_thread_active(true) {
    bluez_service.init();
    adapter = bluez_service.get_first_adapter();
    if (adapter == nullptr) {
        // No adapter available.
    } else {
        async_thread = new std::thread(&NativeBleInternal::async_thread_function, this);
    }
}

NativeBleInternal::~NativeBleInternal() {
    async_thread_active = false;
    while (!async_thread->joinable()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    async_thread->join();
    delete async_thread;
}

void NativeBleInternal::setup(CallbackHolder callback_holder) { this->callback_holder = callback_holder; }

void NativeBleInternal::scan_start() {
    if (adapter == nullptr) return;
    for (auto& [device_path, dev] : adapter->devices) {
        DeviceDescriptor descriptor;
        descriptor.address = dev->get_address();
        descriptor.name = dev->get_name();
        if (dev->get_name() != "") {
            callback_holder.callback_on_scan_found(descriptor);
        }
    }
    adapter->OnDeviceFound = [&](std::string address, std::string name) {
        DeviceDescriptor descriptor;
        descriptor.address = address;
        descriptor.name = name;
        if (name != "") {
            callback_holder.callback_on_scan_found(descriptor);
        }
    };
    adapter->discovery_filter_transport_set("le");
    adapter->StartDiscovery();
    callback_holder.callback_on_scan_start();
}

void NativeBleInternal::scan_stop() {
    if (adapter == nullptr) return;
    adapter->StopDiscovery();
    callback_holder.callback_on_scan_stop();
}

bool NativeBleInternal::scan_is_active() {
    if (adapter == nullptr) return false;
    return adapter->is_discovering();
}

void NativeBleInternal::scan_timeout(int32_t timeout_ms) {
    this->scan_start();
    std::this_thread::sleep_for(std::chrono::milliseconds(timeout_ms));
    this->scan_stop();
}

bool NativeBleInternal::is_connected() { return device != nullptr ? true : false; }

void NativeBleInternal::connect(const BluetoothAddress& address) {
    BluetoothAddress addr = format_mac_address(address);

    if (adapter == nullptr) return;
    device = adapter->get_device(address);
    if (device != nullptr) {
        device->OnConnected = [&]() { callback_holder.callback_on_device_connected(); };
        device->Connect();
    }
}

void NativeBleInternal::write_request(BluetoothUUID service_uuid, BluetoothUUID characteristic_uuid, DataChunk& data) {
    if (device == nullptr) return;
    auto characteristic = device->get_characteristic(service_uuid, characteristic_uuid);
    if (characteristic != nullptr) {
        characteristic->write_request((const uint8_t*)data.c_str(), data.length());
    }
}

void NativeBleInternal::write_command(BluetoothUUID service_uuid, BluetoothUUID characteristic_uuid, DataChunk& data) {
    if (device == nullptr) return;
    auto characteristic = device->get_characteristic(service_uuid, characteristic_uuid);
    if (characteristic != nullptr) {
        characteristic->write_command((const uint8_t*)data.c_str(), data.length());
    }
}

void NativeBleInternal::read(BluetoothUUID service_uuid, BluetoothUUID characteristic_uuid,
                             std::function<void(const uint8_t* data, uint32_t length)> callback_on_read) {
    if (device == nullptr) return;
    auto characteristic = device->get_characteristic(service_uuid, characteristic_uuid);
    if (characteristic != nullptr) {
        SimpleDBus::Holder result = characteristic->ReadValue(SimpleDBus::Holder());
        auto result_array = result.get_array();

        uint8_t* result_data = new uint8_t[result_array.size()];
        for (int i = 0; i < result_array.size(); i++) {
            result_data[i] = result_array[i].get_byte();
        }
        delete[] result_data;
        callback_on_read(result_data, result_array.size());
    }
}

void NativeBleInternal::notify(BluetoothUUID service_uuid, BluetoothUUID characteristic_uuid,
                               std::function<void(const uint8_t* data, uint32_t length)> callback_on_notify) {
    if (device == nullptr) return;
    auto characteristic = device->get_characteristic(service_uuid, characteristic_uuid);
    if (characteristic != nullptr) {
        characteristic->ValueChanged = [callback_on_notify](std::vector<uint8_t>& new_value) {
            callback_on_notify(&new_value[0], new_value.size());
        };
        characteristic->StartNotify();
    }
}

void NativeBleInternal::indicate(BluetoothUUID service_uuid, BluetoothUUID characteristic_uuid,
                                 std::function<void(const uint8_t* data, uint32_t length)> callback_on_indicate) {
    if (device == nullptr) return;
    auto characteristic = device->get_characteristic(service_uuid, characteristic_uuid);
    if (characteristic != nullptr) {
        characteristic->ValueChanged = [callback_on_indicate](std::vector<uint8_t>& new_value) {
            callback_on_indicate(&new_value[0], new_value.size());
        };
        characteristic->StartNotify();
    }
}

void NativeBleInternal::disconnect() {
    if (device == nullptr) return;
    device->Disconnect();
}

void NativeBleInternal::dispose() {}

void NativeBleInternal::async_thread_function() {
    while (async_thread_active) {
        bluez_service.run_async();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

BluetoothAddress NativeBleInternal::format_mac_address(std::string address) {
    if ((int)address.find(' ') > 0) {
        address.replace(address.begin(), address.end(), ' ', ':');
    }
    if ((int)address.find('-') > 0) {
        address.replace(address.begin(), address.end(), '-', ':');
    }

    if ((int)address.find(':') < 0) {
        std::stringstream formatted_addr;
        formatted_addr << address[0] << address[1];
        for (int i = 0; i < address.length(); i += 2) {
            formatted_addr << ":" << address[i] << address[i + 1];
        }

        return (BluetoothAddress)formatted_addr.str();
    }
    return (BluetoothAddress)address;
}