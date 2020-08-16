#pragma once

#include <dbus/dbus.h>

#include <stack>
#include <string>
#include <vector>

#include "Connection.h"
#include "Holder.h"

namespace SimpleDBus {

class Connection;
class Interface;

typedef enum {
    INVALID = DBUS_MESSAGE_TYPE_INVALID,
    METHOD_CALL = DBUS_MESSAGE_TYPE_METHOD_CALL,
    METHOD_RETURN = DBUS_MESSAGE_TYPE_METHOD_RETURN,
    ERROR = DBUS_MESSAGE_TYPE_ERROR,
    SIGNAL = DBUS_MESSAGE_TYPE_SIGNAL,
} MessageType;

class Message {
  private:
    friend class Connection;

    int indent;
    bool iter_initialized;

    Holder _extract_array(DBusMessageIter* iter);
    Holder _extract_dict(DBusMessageIter* iter);
    Holder _extract_generic(DBusMessageIter* iter);
    void _append_argument(DBusMessageIter* iter, Holder& argument, std::string signature);

  public:
    Message();
    Message(DBusMessage* msg);
    ~Message();

    bool is_valid();
    void append_argument(Holder argument, std::string signature);

    Holder extract();
    void extract_reset();
    bool extract_has_next();
    void extract_next();
    std::string to_string();

    std::string get_signature();
    std::string get_interface();
    std::string get_path();
    MessageType get_type();

    bool is_signal(std::string interface, std::string signal_name);

    DBusMessage* _msg;
    DBusMessageIter _iter;

    static Message create_method_call(std::string bus_name, std::string path, std::string interface,
                                      std::string method);
};

}  // namespace SimpleDBus