#pragma comment(lib, "windowsapp")
#include "NativeBleInternal.h"

#include "winrt/Windows.Devices.Bluetooth.Advertisement.h"
#include "winrt/Windows.Devices.Bluetooth.GenericAttributeProfile.h"
#include "winrt/Windows.Foundation.h"
#include "winrt/Windows.Storage.Streams.h"
#include "winrt/base.h"

#include <algorithm>
#include <iomanip>
#include <set>
#include <sstream>
#include <vector>

#include <iostream>

using namespace NativeBLE;

NativeBleInternal::NativeBleInternal() : device(nullptr), scanning(false) {}

NativeBleInternal::~NativeBleInternal() {}

void NativeBleInternal::setup(CallbackHolder callback_holder) {
    this->callback_holder = callback_holder;
    winrt::init_apartment();
}

void NativeBleInternal::scan_start() {
    detected_addresses.clear();
    scanner = Advertisement::BluetoothLEAdvertisementWatcher();
    scanner.ScanningMode(Advertisement::BluetoothLEScanningMode::Active);
    scanner.Received([&](const auto& w, const Advertisement::BluetoothLEAdvertisementReceivedEventArgs args) {
        DeviceDescriptor descriptor;
        std::stringstream helper;
        helper << std::hex << args.BluetoothAddress();
        descriptor.name = winrt::to_string(args.Advertisement().LocalName());
        descriptor.address = helper.str();
        if (descriptor.name.length() > 0 && detected_addresses.find(descriptor.address) == detected_addresses.end()) {
            // If the detected address has not been seen before, add it to the list and notify the callback.
            detected_addresses.insert(descriptor.address);
            callback_holder.callback_on_scan_found(descriptor);
        }
    });
    scanner.Start();
    scanning = true;
    callback_holder.callback_on_scan_start();
}

void NativeBleInternal::scan_stop() {
    if (scanning) {
        scanner.Stop();
        callback_holder.callback_on_scan_stop();
        scanning = false;
    }
}

bool NativeBleInternal::scan_is_active() { return scanning; }

void NativeBleInternal::scan_timeout(int32_t timeout_ms) {
    this->scan_start();
    std::this_thread::sleep_for(std::chrono::milliseconds(timeout_ms));
    this->scan_stop();
}

bool NativeBleInternal::is_connected() { 
    if (device != nullptr && device.ConnectionStatus() == BluetoothConnectionStatus::Connected) {
        return true;
    }
    return false;
}

void NativeBleInternal::connect(const BluetoothAddress& address) {
    if (scanning) {
        this->scan_stop();
    }

    std::string addr = format_mac_address(address);

    uint64_t ble_address = strtoull(addr.c_str(), nullptr, 16);
    device = BluetoothLEDevice::FromBluetoothAddressAsync(ble_address).get();
    device.ConnectionStatusChanged([=](const BluetoothLEDevice device, const auto args) {
        if (device.ConnectionStatus() == BluetoothConnectionStatus::Connected) {
        } else {
            this->disconnect_execute();
            callback_holder.callback_on_device_disconnected("Disconnected from device.");
        }
    });

    // We'll now cache all services and characteristics, because if not the underlying objects will be garbage
    // collected.
    auto services_result = device.GetGattServicesAsync(BluetoothCacheMode::Uncached).get();
    if (services_result.Status() == GenericAttributeProfile::GattCommunicationStatus::Success) {
        auto gatt_services = services_result.Services();
        for (GattDeviceService&& service : gatt_services) {
            // For each service...
            auto characteristics_result = service.GetCharacteristicsAsync(BluetoothCacheMode::Uncached).get();
            if (characteristics_result.Status() == GattCommunicationStatus::Success) {
                auto gatt_characteristics = characteristics_result.Characteristics();
                for (GattCharacteristic&& characteristic : gatt_characteristics) {
                    // For each characteristic...
                    // Store the underlying object pointer in a map.
                    std::string service_uuid = guid_to_uuid(service.Uuid());
                    std::string characteristic_uuid = guid_to_uuid(characteristic.Uuid());
                    characteristics_map[service_uuid].emplace(characteristic_uuid, characteristic);
                }
            } else {
                this->disconnect_execute();
                callback_holder.callback_on_device_disconnected("Could not discover characteristics for service.");
                return;
            }
        }
    } else {
        this->disconnect_execute();
        callback_holder.callback_on_device_disconnected("Could not discover services for device.");
        return;
    }

    callback_holder.callback_on_device_connected();
}

void NativeBleInternal::write_request(BluetoothUUID service_uuid, BluetoothUUID characteristic_uuid, DataChunk& data) {
    winrt::guid service_guid = uuid_to_guid(service_uuid);
    winrt::guid characteristic_guid = uuid_to_guid(characteristic_uuid);

    GattCharacteristic* gatt_characteristic = fetch_characteristic(service_uuid, characteristic_uuid);
    uint32_t gatt_characteristic_prop = (uint32_t)gatt_characteristic->CharacteristicProperties();

    if (gatt_characteristic != nullptr &&
        (gatt_characteristic_prop & (uint32_t)GattCharacteristicProperties::Write) != 0) {
        auto writer = winrt::Windows::Storage::Streams::DataWriter();
        std::vector<uint8_t> data_buffer;
        for (int i = 0; i < data.length(); i++) {
            data_buffer.push_back(data[i]);
        }
        writer.WriteBytes(data_buffer);

        auto status =
            gatt_characteristic->WriteValueAsync(writer.DetachBuffer(), GattWriteOption::WriteWithResponse).get();
        if (status == GenericAttributeProfile::GattCommunicationStatus::Success) {
        }
    }
}

void NativeBleInternal::write_command(BluetoothUUID service_uuid, BluetoothUUID characteristic_uuid, DataChunk& data) {
    winrt::guid service_guid = uuid_to_guid(service_uuid);
    winrt::guid characteristic_guid = uuid_to_guid(characteristic_uuid);

    GattCharacteristic* gatt_characteristic = fetch_characteristic(service_uuid, characteristic_uuid);
    uint32_t gatt_characteristic_prop = (uint32_t)gatt_characteristic->CharacteristicProperties();

    if (gatt_characteristic != nullptr &&
        (gatt_characteristic_prop & (uint32_t)GattCharacteristicProperties::WriteWithoutResponse) != 0) {
        auto writer = winrt::Windows::Storage::Streams::DataWriter();
        std::vector<uint8_t> data_buffer;
        for (int i = 0; i < data.length(); i++) {
            data_buffer.push_back(data[i]);
        }
        writer.WriteBytes(data_buffer);

        auto status =
            gatt_characteristic->WriteValueAsync(writer.DetachBuffer(), GattWriteOption::WriteWithoutResponse).get();
        if (status == GenericAttributeProfile::GattCommunicationStatus::Success) {
        }
    }
}

void NativeBleInternal::read(BluetoothUUID service_uuid, BluetoothUUID characteristic_uuid,
                             std::function<void(const uint8_t* data, uint32_t length)> callback_on_read) {
    winrt::guid service_guid = uuid_to_guid(service_uuid);
    winrt::guid characteristic_guid = uuid_to_guid(characteristic_uuid);

    GattCharacteristic* gatt_characteristic = fetch_characteristic(service_uuid, characteristic_uuid);
    uint32_t gatt_characteristic_prop = (uint32_t)gatt_characteristic->CharacteristicProperties();

    if (gatt_characteristic != nullptr) {
        auto result = gatt_characteristic->ReadValueAsync().get();

        if (result.Status() == GenericAttributeProfile::GattCommunicationStatus::Success) {
            auto reader = winrt::Windows::Storage::Streams::DataReader::FromBuffer(result.Value());
            uint32_t recv_size = reader.UnconsumedBufferLength();
            std::vector<uint8_t> recv_buffer(recv_size);
            reader.ReadBytes(recv_buffer);
            callback_on_read(recv_buffer.data(), recv_size);
        }
    }
}

void NativeBleInternal::notify(BluetoothUUID service_uuid, BluetoothUUID characteristic_uuid,
                               std::function<void(const uint8_t* data, uint32_t length)> callback_on_notify) {
    GattCharacteristic* gatt_characteristic = fetch_characteristic(service_uuid, characteristic_uuid);
    uint32_t gatt_characteristic_prop = (uint32_t)gatt_characteristic->CharacteristicProperties();

    if (gatt_characteristic != nullptr &&
        (gatt_characteristic_prop & (uint32_t)GattCharacteristicProperties::Notify) != 0) {
        auto status = gatt_characteristic
                          ->WriteClientCharacteristicConfigurationDescriptorAsync(
                              GattClientCharacteristicConfigurationDescriptorValue::Notify)
                          .get();
        if (status == GenericAttributeProfile::GattCommunicationStatus::Success) {
            gatt_characteristic->ValueChanged(
                [=](const GattCharacteristic sender, const GattValueChangedEventArgs args) {
                    auto reader = winrt::Windows::Storage::Streams::DataReader::FromBuffer(args.CharacteristicValue());
                    uint32_t recv_size = reader.UnconsumedBufferLength();
                    std::vector<uint8_t> recv_buffer(recv_size);
                    reader.ReadBytes(recv_buffer);
                    callback_on_notify(recv_buffer.data(), recv_size);
                });
        }
    }
}

void NativeBleInternal::indicate(BluetoothUUID service_uuid, BluetoothUUID characteristic_uuid,
                                 std::function<void(const uint8_t* data, uint32_t length)> callback_on_indicate) {
    GattCharacteristic* gatt_characteristic = fetch_characteristic(service_uuid, characteristic_uuid);
    uint32_t gatt_characteristic_prop = (uint32_t)gatt_characteristic->CharacteristicProperties();

    if (gatt_characteristic != nullptr &&
        (gatt_characteristic_prop & (uint32_t)GattCharacteristicProperties::Indicate) != 0) {
        auto status = gatt_characteristic
                          ->WriteClientCharacteristicConfigurationDescriptorAsync(
                              GattClientCharacteristicConfigurationDescriptorValue::Indicate)
                          .get();
        if (status == GenericAttributeProfile::GattCommunicationStatus::Success) {
            gatt_characteristic->ValueChanged(
                [=](const GattCharacteristic sender, const GattValueChangedEventArgs args) {
                    auto reader = winrt::Windows::Storage::Streams::DataReader::FromBuffer(args.CharacteristicValue());
                    uint32_t recv_size = reader.UnconsumedBufferLength();
                    std::vector<uint8_t> recv_buffer(recv_size);
                    reader.ReadBytes(recv_buffer);
                    callback_on_indicate(recv_buffer.data(), recv_size);
                });
        }
    }
}

void NativeBleInternal::disconnect_execute() {
    if (device != nullptr) {
        auto services = device.GetGattServicesAsync().get().Services();
        for (GattDeviceService s : services) {
            s.Close();
        }
        device.Close();
        characteristics_map.clear();
        device = nullptr;
    }
}

void NativeBleInternal::disconnect() {
    if (is_connected()) {
        disconnect_execute();
        callback_holder.callback_on_device_disconnected("Manually disconnected");
    }
}

void NativeBleInternal::dispose() {}

// ----- Start of auxiliary functions. -----

GattCharacteristic* NativeBleInternal::fetch_characteristic(const std::string& service_uuid,
                                                            const std::string& characteristic_uuid) {
    if (characteristics_map.count(service_uuid) == 1) {
        if (characteristics_map[service_uuid].count(characteristic_uuid) == 1) {
            return &characteristics_map[service_uuid].at(characteristic_uuid);
        }
    }
    return nullptr;
}

winrt::guid NativeBleInternal::uuid_to_guid(const std::string& uuid) {
    // TODO: Add proper cleanup / validation
    std::stringstream helper;
    for (int i = 0; i < uuid.length(); i++) {
        if (uuid[i] != '-') {
            helper << uuid[i];
        }
    }
    std::string clean_uuid = helper.str();
    winrt::guid guid;
    uint64_t* data4_ptr = (uint64_t*)guid.Data4;

    guid.Data1 = std::strtoul(clean_uuid.substr(0, 8).c_str(), nullptr, 16);
    guid.Data2 = std::strtoul(clean_uuid.substr(8, 4).c_str(), nullptr, 16);
    guid.Data3 = std::strtoul(clean_uuid.substr(12, 4).c_str(), nullptr, 16);
    *data4_ptr = _byteswap_uint64(std::strtoull(clean_uuid.substr(16, 16).c_str(), nullptr, 16));

    return guid;
}

std::string NativeBleInternal::guid_to_uuid(const winrt::guid& guid) {
    std::stringstream helper;

    for (uint32_t i = 0; i < 4; i++) {
        // * NOTE: We're performing a byte swap!
        helper << std::hex << std::setw(2) << std::setfill('0') << (int)((uint8_t*)&guid.Data1)[3 - i];
    }
    helper << '-';
    for (uint32_t i = 0; i < 2; i++) {
        // * NOTE: We're performing a byte swap!
        helper << std::hex << std::setw(2) << std::setfill('0') << (int)((uint8_t*)&guid.Data2)[1 - i];
    }
    helper << '-';
    for (uint32_t i = 0; i < 2; i++) {
        // * NOTE: We're performing a byte swap!
        helper << std::hex << std::setw(2) << std::setfill('0') << (int)((uint8_t*)&guid.Data3)[1 - i];
    }
    helper << '-';
    for (uint32_t i = 0; i < 2; i++) {
        helper << std::hex << std::setw(2) << std::setfill('0') << (int)guid.Data4[i];
    }
    helper << '-';
    for (uint32_t i = 0; i < 6; i++) {
        helper << std::hex << std::setw(2) << std::setfill('0') << (int)guid.Data4[2 + i];
    }
    return helper.str();
}

// Format all mac addresses to Windows format (e.g. abcdef123456)
BluetoothAddress NativeBleInternal::format_mac_address(std::string address) {
    std::string new_addr;
    for (int i = 0; i < address.length(); i++) {
        if (address[i] >= 'A' && address[i] <= 'F') {
            new_addr.push_back(address[i] - 32); //cast to lower case
        } else if ((address[i] >= 'a' && address[i] <= 'f') || (address[i] >= '0' && address[i] <= '9')) {
            new_addr.push_back(address[i]); //add characters that are only valid to a MAC address
        }
    }

    return (BluetoothAddress) new_addr;
}
