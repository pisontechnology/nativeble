#include "BluezAgent.h"

#include <iostream>

BluezAgent::BluezAgent(std::string path, SimpleDBus::Holder options) : _path(path) {
    // std::cout << "Creating BluezAgent" << std::endl;
}

BluezAgent::~BluezAgent() {
    // std::cout << "Destroying BluezAgent" << std::endl;
}

bool BluezAgent::process_received_signal(SimpleDBus::Message& message) {
    if (message.get_path() == _path) {
        return true;
    }
    return false;
}