#pragma once
#include <string>
#include <functional>

#include "../base/Connection.h"
#include "../base/Holder.h"
#include "../base/Message.h"

namespace SimpleDBus {

class Holder;
class Connection;

class Properties {
  private:
    const std::string _interface;
    
    std::string _path;
    std::string _service;
    Connection* _conn;

  public:
    Properties(Connection* conn, std::string service, std::string path);
    ~Properties();

    // Names are made matching the ones from the DBus specification
    Holder Get(std::string interface, std::string name);
    Holder GetAll(std::string interface);
    void Set(std::string interface, std::string name, Holder value);

    std::function<void(std::string interface, Holder changed_properties, Holder invalidated_properties)> PropertiesChanged;

    bool process_received_signal(Message& message);
};

}  // namespace SimpleDBus