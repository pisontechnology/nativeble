#pragma once
#include "NativeBleControllerTypes.h"

#include <map>
#include <set>
#include <string>

#include "winrt/Windows.Devices.Bluetooth.h"
#include "winrt/Windows.Devices.Bluetooth.Advertisement.h"
using namespace winrt::Windows::Devices::Bluetooth;
using namespace winrt::Windows::Devices::Bluetooth::GenericAttributeProfile;

namespace NativeBLE {

class NativeBleInternal {
  private:
    CallbackHolder callback_holder;
    BluetoothLEDevice device;
    bool scanning;
    std::set<NativeBLE::BluetoothAddress> detected_addresses;
    struct Advertisement::BluetoothLEAdvertisementWatcher scanner;
    std::map<std::string, std::map<std::string, GattCharacteristic>> characteristics_map;

    winrt::guid uuid_to_guid(const std::string &uuid);
    std::string guid_to_uuid(const winrt::guid &guid);
    GattCharacteristic *fetch_characteristic(const std::string &service_uuid, const std::string &characteristic_uuid);

    void disconnect_execute();

    BluetoothAddress format_mac_address(std::string); 

  public:
    NativeBleInternal(/* args */);
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
