#pragma once

#include "simpledbus/SimpleDBus.h"

#include "interfaces/Adapter1.h"

#include "BluezDevice.h"

#include <memory>
#include <string>

class BluezAdapter : public Adapter1, public SimpleDBus::Properties {
  private:
    SimpleDBus::Connection* _conn;
    std::string _path;

    void add_interface(std::string interface_name, SimpleDBus::Holder options);

  public:
    BluezAdapter(SimpleDBus::Connection* conn, std::string path, SimpleDBus::Holder managed_interfaces);
    ~BluezAdapter();

    std::map<std::string, std::shared_ptr<BluezDevice>> devices;
    std::shared_ptr<BluezDevice> get_device(std::string mac_address);

    bool add_path(std::string path, SimpleDBus::Holder options);
    bool remove_path(std::string path, SimpleDBus::Holder options);
    bool process_received_signal(SimpleDBus::Message& message);

    // TODO: Add support for more complex filter types.
    void discovery_filter_transport_set(std::string value);

    std::function<void(std::string address, std::string name)> OnDeviceFound;
};
