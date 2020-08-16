#include "Device1.h"

#include <iostream>

Device1::Device1(SimpleDBus::Connection* conn, std::string path) : _conn(conn), _path(path) {
    // std::cout << "Creating org.bluez.Device1: " << path << std::endl;
}

Device1::~Device1() {}

void Device1::add_option(std::string option_name, SimpleDBus::Holder value) {
    // std::cout << "\t" << option_name << std::endl;

    if (option_name == "Address") {
        _address = value.get_string();
    } else if (option_name == "Name") {
        _name = value.get_string();
    } else if (option_name == "Connected") {
        _connected = value.get_boolean();
        if (_connected && OnConnected) {
            OnConnected();
        } else if (!_connected && OnDisconnected) {
            OnDisconnected();
        }
    }
}

void Device1::remove_option(std::string option_name) {}

void Device1::Connect() {
    // std::cout << "org.bluez.Device1 Connect" << std::endl;
    auto msg = SimpleDBus::Message::create_method_call("org.bluez", _path, "org.bluez.Device1", "Connect");
    _conn->send_with_reply_and_block(msg);
}

void Device1::Disconnect() {
    // std::cout << "org.bluez.Device1 Disconnect" << std::endl;
    auto msg = SimpleDBus::Message::create_method_call("org.bluez", _path, "org.bluez.Device1", "Disconnect");
    _conn->send_with_reply_and_block(msg);
}

std::string Device1::get_name() { return _name; }

std::string Device1::get_address() { return _address; }

bool Device1::is_connected() { return _connected; }