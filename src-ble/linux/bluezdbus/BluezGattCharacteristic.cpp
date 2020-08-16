#include "BluezGattCharacteristic.h"

#include <iostream>

BluezGattCharacteristic::BluezGattCharacteristic(SimpleDBus::Connection* conn, std::string path,
                                                 SimpleDBus::Holder managed_interfaces)
    : _conn(conn), _path(path), GattCharacteristic1{conn, path}, Properties{conn, "org.bluez", path} {
    // std::cout << "Creating BluezGattCharacteristic: " << path << std::endl;

    Properties::PropertiesChanged = [&](std::string interface, SimpleDBus::Holder changed_properties,
                                        SimpleDBus::Holder invalidated_properties) {
        // std::cout << "(" << _path << ") PropertiesChanged" << std::endl;
        // std::cout << changed_properties.represent() << std::endl;
        // std::cout << invalidated_properties.represent() << std::endl;
        // // TODO: Handle the changed property.

        if (interface == "org.bluez.GattCharacteristic1") {
            GattCharacteristic1::set_options(changed_properties, invalidated_properties);
        } else {
        }
    };

    auto managed_interface = managed_interfaces.get_dict();
    for (auto& [iface, options] : managed_interface) {
        add_interface(iface, options);
    }
}

BluezGattCharacteristic::~BluezGattCharacteristic() {
    // std::cout << "Destroying BluezGattCharacteristic" << std::endl;
}

bool BluezGattCharacteristic::process_received_signal(SimpleDBus::Message& message) {
    if (message.get_path() == _path) {
        if (Properties::process_received_signal(message)) return true;
        std::cout << message.to_string() << std::endl;
    }
    return false;
}

void BluezGattCharacteristic::add_interface(std::string interface_name, SimpleDBus::Holder options) {
    // std::cout << interface_name << std::endl;
    if (interface_name == "org.bluez.GattCharacteristic1") {
        GattCharacteristic1::set_options(options);
    } else {
    }
}

bool BluezGattCharacteristic::add_path(std::string path, SimpleDBus::Holder options) {
    int path_elements = std::count(path.begin(), path.end(), '/');
    if (path.rfind(_path, 0) == 0) {
        if (path_elements == 7) {
            // TODO: Characteristics also have Descriptors, although I'm not sure we need them.
            // std::cout << "New path: " << path << std::endl << options.represent() << std::endl;
        } else {
        }

        return true;
    }
    return false;
}

bool BluezGattCharacteristic::remove_path(std::string path, SimpleDBus::Holder options) { return false; }