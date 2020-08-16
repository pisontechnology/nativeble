#pragma once

#include "simpledbus/SimpleDBus.h"

#include "interfaces/Device1.h"

#include "BluezGattService.h"

#include <memory>
#include <string>

class BluezDevice : public Device1, public SimpleDBus::Properties {
  private:
    SimpleDBus::Connection* _conn;
    std::string _path;

    std::map<std::string, std::shared_ptr<BluezGattService>> gatt_services;
    void add_interface(std::string interface_name, SimpleDBus::Holder options);

  public:
    BluezDevice(SimpleDBus::Connection* conn, std::string path, SimpleDBus::Holder options);
    ~BluezDevice();

    bool add_path(std::string path, SimpleDBus::Holder options);
    bool remove_path(std::string path, SimpleDBus::Holder options);
    bool process_received_signal(SimpleDBus::Message& message);

    void connect();
    void disconnect();

    std::shared_ptr<BluezGattService> get_service(std::string service_uuid);
    std::shared_ptr<BluezGattCharacteristic> get_characteristic(std::string service_uuid, std::string char_uuid);
};
