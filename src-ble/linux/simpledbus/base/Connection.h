#pragma once

#include <dbus/dbus.h>
#include "Message.h"

namespace SimpleDBus {

class Message;

class Connection {
  private:
    ::DBusBusType _dbus_bus_type;

  public:
    Connection(::DBusBusType dbus_bus_type);
    ~Connection();

    void init();

    void add_match(std::string rule);
    void remove_match(std::string rule);

    Message read_write_pop();

    uint32_t send(Message &msg);
    Message send_with_reply_and_block(Message &msg);

    ::DBusConnection *conn;  // TODO: Make private
    ::DBusError err;         // TODO: Make private
};

}  // namespace SimpleDBus