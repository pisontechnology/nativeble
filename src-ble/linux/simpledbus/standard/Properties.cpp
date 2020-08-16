#include "Properties.h"

#include "../base/Message.h"

#include <iostream>

using namespace SimpleDBus;

Properties::Properties(Connection* conn, std::string service, std::string path)
    : _conn(conn), _service(service), _path(path), _interface("org.freedesktop.DBus.Properties") {}

Properties::~Properties() {}

// Names are made matching the ones from the DBus specification
Holder Properties::Get(std::string interface, std::string name) {
    Message query_msg = Message::create_method_call(_service, _path, _interface, "Get");

    Holder h_interface = Holder::create_string(interface.c_str());
    query_msg.append_argument(h_interface, "s");

    Holder h_name = Holder::create_string(name.c_str());
    query_msg.append_argument(h_name, "s");

    Message reply_msg = _conn->send_with_reply_and_block(query_msg);
    Holder result = reply_msg.extract();
    return result;
}

Holder Properties::GetAll(std::string interface) {
    Message query_msg = Message::create_method_call(_service, _path, _interface, "GetAll");

    Holder h_interface = Holder::create_string(interface.c_str());
    query_msg.append_argument(h_interface, "s");

    Message reply_msg = _conn->send_with_reply_and_block(query_msg);
    Holder result = reply_msg.extract();
    return result;
}

void Properties::Set(std::string interface, std::string name, Holder value) {
    Message query_msg = Message::create_method_call(_service, _path, _interface, "Set");

    Holder h_interface = Holder::create_string(interface.c_str());
    query_msg.append_argument(h_interface, "s");

    Holder h_name = Holder::create_string(name.c_str());
    query_msg.append_argument(h_name, "s");

    query_msg.append_argument(value, "v");

    _conn->send_with_reply_and_block(query_msg);
}

bool Properties::process_received_signal(Message& message) {
    if (message.get_path() == _path && message.is_signal(_interface, "PropertiesChanged")) {
        // std::cout << message.to_string() << std::endl;
        Holder interface = message.extract();
        message.extract_next();
        Holder changed_properties = message.extract();
        message.extract_next();
        Holder invalidated_properties = message.extract();
        if (PropertiesChanged) {
            PropertiesChanged(interface.get_string(), changed_properties, invalidated_properties);
        }
        return true;
    }
    return false;
}