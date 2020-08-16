#include "BluezService.h"

#include <algorithm>

BluezService::BluezService() : conn(DBUS_BUS_SYSTEM), object_manager(&conn, "org.bluez", "/") {
    object_manager.InterfacesAdded = [&](std::string path, SimpleDBus::Holder options) { add_path(path, options); };
    object_manager.InterfacesRemoved = [&](std::string path, SimpleDBus::Holder options) {
        remove_path(path, options);
    };
}

BluezService::~BluezService() {
    // std::cout << "Destroying BluezService" << std::endl;
    conn.remove_match("type='signal',sender='org.bluez'");
}

void BluezService::init() {
    conn.init();
    object_manager.GetManagedObjects(true);  // Feed the objects via callback.

    conn.add_match("type='signal',sender='org.bluez'");
}

void BluezService::run_async() {
    SimpleDBus::Message message = conn.read_write_pop();
    if (!message.is_valid()) {
        return;
    }

    switch (message.get_type()) {
        case SimpleDBus::MessageType::SIGNAL:
            process_received_signal(message);
            break;

        default:
            break;
    }
}

void BluezService::process_received_signal(SimpleDBus::Message& message) {
    std::string path = message.get_path();

    if (path == "/org/freedesktop/DBus") return;

    if (object_manager.process_received_signal(message)) return;

    for (auto& [path, adapter] : adapters) {
        if (adapter->process_received_signal(message)) return;
    }
}

void BluezService::add_path(std::string path, SimpleDBus::Holder options) {
    int path_elements = std::count(path.begin(), path.end(), '/');

    switch (path_elements) {
        case 2:
            agent.reset(new BluezAgent(path, options));
            break;
        case 3:
            adapters.emplace(std::make_pair(path, new BluezAdapter(&conn, path, options)));
            break;
        default:
            // Propagate the paths downwards until someone claims it.
            for (auto& [adapter_path, adapter] : adapters) {
                if (adapter->add_path(path, options)) return;
            }
            break;
    }
}

void BluezService::remove_path(std::string path, SimpleDBus::Holder options) {
    // TODO: Do we need to notify the objects that they are being deleted?
    int path_elements = std::count(path.begin(), path.end(), '/');

    switch (path_elements) {
        case 2:
            break;
        case 3:
            // ! FIXME: There's the possibility that individual interfaces might get removed.
            adapters.erase(path);
            break;
        default:
            // Propagate the paths downwards until someone claims it.
            for (auto& [adapter_path, adapter] : adapters) {
                if (adapter->remove_path(path, options)) return;
            }
            break;
    }
}

std::shared_ptr<BluezAdapter> BluezService::get_first_adapter() {
    std::shared_ptr<BluezAdapter> return_value = nullptr;

    if (!adapters.empty()) {
        return_value = adapters.begin()->second;
    }

    return return_value;
}
