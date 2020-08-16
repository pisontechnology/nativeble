#include "Connection.h"

#include <chrono>
#include <iostream>
#include <thread>

using namespace SimpleDBus;

Connection::Connection(DBusBusType dbus_bus_type) : _dbus_bus_type(dbus_bus_type) {}

Connection::~Connection() {
    // In order to prevent a crash on any third party environment
    // we need to flush the connection queue.
    SimpleDBus::Message message;
    do {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        message = read_write_pop();
    } while (message.is_valid());
    // ---------------------------------------------------------

    dbus_error_free(&err);
    dbus_connection_unref(conn);
}

void Connection::init() {
    dbus_error_init(&err);
    conn = dbus_bus_get(_dbus_bus_type, &err);
    if (dbus_error_is_set(&err)) {
        std::cout << "ERROR: " << err.name << " " << err.message << std::endl;
        dbus_error_free(&err);
    }
}

void Connection::add_match(std::string rule) {
    dbus_bus_add_match(conn, rule.c_str(), &err);
    dbus_connection_flush(conn);
    if (dbus_error_is_set(&err)) {
        std::cout << "ERROR: " << err.name << " " << err.message << std::endl;
        dbus_error_free(&err);
    }
}

void Connection::remove_match(std::string rule) {
    dbus_bus_remove_match(conn, rule.c_str(), &err);
    dbus_connection_flush(conn);
    if (dbus_error_is_set(&err)) {
        std::cout << "ERROR: " << err.name << " " << err.message << std::endl;
        dbus_error_free(&err);
    }
}

Message Connection::read_write_pop() {
    // Non blocking read of the next available message
    dbus_connection_read_write(conn, 0);
    DBusMessage* msg = dbus_connection_pop_message(conn);
    if (msg == nullptr) {
        return Message();
    } else {
        return Message(msg);
    }
}

uint32_t Connection::send(Message& msg) {
    uint32_t msg_serial = 0;
    bool success = dbus_connection_send(conn, msg._msg, &msg_serial);

    if (!success) {
        std::cout << "ERROR: Could not send message." << std::endl;
    } else {
        dbus_connection_flush(conn);
    }
    return msg_serial;
}

Message Connection::send_with_reply_and_block(Message& msg) {
    DBusMessage* msg_tmp = dbus_connection_send_with_reply_and_block(conn, msg._msg, -1, &err);

    if (dbus_error_is_set(&err)) {
        std::cout << "ERROR: " << err.name << " " << err.message << std::endl;
        dbus_error_free(&err);
        return Message();
    } else {
        return Message(msg_tmp);
    }
}
