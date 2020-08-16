#include "GattCharacteristic1.h"

#include <iostream>

GattCharacteristic1::GattCharacteristic1(SimpleDBus::Connection* conn, std::string path) : _conn(conn), _path(path) {
    // std::cout << "Creating org.bluez.GattCharacteristic1: " << path << std::endl;
}

GattCharacteristic1::~GattCharacteristic1() {}

void GattCharacteristic1::add_option(std::string option_name, SimpleDBus::Holder value) {
    if (option_name == "UUID") {
        _uuid = value.get_string();
    } else if (option_name == "Value") {
        _value.clear();
        auto value_array = value.get_array();
        for (auto& elem : value_array) {
            _value.push_back(elem.get_byte());
        }
        if (ValueChanged) {
            ValueChanged(_value);
        }
    }
}

void GattCharacteristic1::remove_option(std::string option_name) {}

std::string GattCharacteristic1::get_uuid() { return _uuid; }

std::vector<uint8_t> GattCharacteristic1::get_value() { return _value; }

void GattCharacteristic1::StartNotify() {
    // std::cout << "org.bluez.GattCharacteristic1 StartNotify" << std::endl;
    auto msg = SimpleDBus::Message::create_method_call("org.bluez", _path, "org.bluez.GattCharacteristic1",
                                                       "StartNotify");
    _conn->send_with_reply_and_block(msg);
}
void GattCharacteristic1::StopNotify() {
    // std::cout << "org.bluez.GattCharacteristic1 StoptNotify" << std::endl;
    auto msg = SimpleDBus::Message::create_method_call("org.bluez", _path, "org.bluez.GattCharacteristic1",
                                                       "StopNotify");
    _conn->send_with_reply_and_block(msg);
}

void GattCharacteristic1::WriteValue(SimpleDBus::Holder value, SimpleDBus::Holder options) {
    // std::cout << "org.bluez.GattCharacteristic1 WriteValue" << std::endl;
    auto msg = SimpleDBus::Message::create_method_call("org.bluez", _path, "org.bluez.GattCharacteristic1",
                                                       "WriteValue");
    msg.append_argument(value, "ay");
    msg.append_argument(options, "a{sv}");
    _conn->send_with_reply_and_block(msg);
}

SimpleDBus::Holder GattCharacteristic1::ReadValue(SimpleDBus::Holder options) {
    // std::cout << "org.bluez.GattCharacteristic1 ReadValue" << std::endl;
    auto msg = SimpleDBus::Message::create_method_call("org.bluez", _path, "org.bluez.GattCharacteristic1",
                                                       "ReadValue");
    msg.append_argument(options, "a{sv}");
    SimpleDBus::Message reply_msg = _conn->send_with_reply_and_block(msg);
    SimpleDBus::Holder value = reply_msg.extract();
    return value;
}

void GattCharacteristic1::write_request(const uint8_t* data, uint16_t length) {
    SimpleDBus::Holder value = SimpleDBus::Holder::create_array();
    for (uint16_t i = 0; i < length; i++) {
        value.array_append(SimpleDBus::Holder::create_byte(data[i]));
    }
    SimpleDBus::Holder options = SimpleDBus::Holder::create_dict();
    options.dict_append("type", SimpleDBus::Holder::create_string("request"));
    WriteValue(value, options);
}

void GattCharacteristic1::write_command(const uint8_t* data, uint16_t length) {
    SimpleDBus::Holder value = SimpleDBus::Holder::create_array();
    for (uint16_t i = 0; i < length; i++) {
        value.array_append(SimpleDBus::Holder::create_byte(data[i]));
    }
    SimpleDBus::Holder options = SimpleDBus::Holder::create_dict();
    options.dict_append("type", SimpleDBus::Holder::create_string("command"));
    WriteValue(value, options);
}
