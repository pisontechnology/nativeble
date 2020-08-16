#include "NativeBleController.h"

#include <iostream>
#include <string>
#include <csignal>
#include <thread>

#define SCAN_TIMEOUT_MS 5000

#define NORDIC_UART_SERVICE_UUID "6e400001-b5a3-f393-e0a9-e50e24dcca9e"
#define NORDIC_UART_CHAR_RX "6e400002-b5a3-f393-e0a9-e50e24dcca9e"
#define NORDIC_UART_CHAR_TX "6e400003-b5a3-f393-e0a9-e50e24dcca9e"

NativeBLE::NativeBleController ble;

void signal_handler(int signal) { 
    if (signal == SIGINT) {
        std::cout << std::endl << "User quit program." << std::endl;
    }

    ble.disconnect();
    ble.dispose();
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    exit(signal);
}

/**
 * This program will scan for devices and ask you to choose a device to connect to.
 * After connecting, the output of the device will be printed to the terminal, and
 * messages can be sent to the device from the terminal.
 * 
 * Known issues:
 * If while typing, a message is received from the device, your message will be erased
 * from the screen but not from memory, so if you type 'enter' after receiving a message,
 * whatever you were in the process of typing will get sent, even if you can't see it.
 */ 
int main() {
    //Setup signal to disconnect and dispose ble when program is quit.
    std::signal(SIGINT, signal_handler);

    NativeBLE::CallbackHolder                   ble_events;
    std::vector<NativeBLE::DeviceDescriptor>    devices;

    // Setup callback functions
    ble_events.callback_on_device_disconnected = [](std::string msg) {
        std::cout << "Disconnected: " << msg << std::endl;
        return;
    };
    ble_events.callback_on_device_connected = []() {
        std::cout << "Connected" << std::endl;
    };
    ble_events.callback_on_scan_found = [&](NativeBLE::DeviceDescriptor device) {
        static int i = 1;
        std::cout << "\r" << i++ << " devices found.";
        fflush(stdout);
        devices.push_back(device);
    };
    ble_events.callback_on_scan_start = []() {
        std::cout << "Scanning for " << SCAN_TIMEOUT_MS << " ms..." << std::endl;
    };
    ble_events.callback_on_scan_stop = []() {
        std::cout << std::endl << "Scan complete." << std::endl;
    };

    ble.setup(ble_events);
    ble.scan_timeout(SCAN_TIMEOUT_MS);

    for (int i = 0; i < devices.size(); i++) {
        std::cout << "  " << i << ": " << devices[i].name << " (" << devices[i].address << ")" << std::endl;
    }

    std::cout << "Type index of device to connect to: ";
    int device;
    std::cin >> device;
    
    ble.connect(devices[device].address);

    // Setup notify for when data is received
    ble.notify(NORDIC_UART_SERVICE_UUID, NORDIC_UART_CHAR_TX, [&](const uint8_t* data, uint32_t length) {
        std::cout << "\r<" << devices[device].name << "> " << "(" << length << ") ";
        for (int i = 0; i < length; i++) { std::cout << data[i]; }
        std::cout << std::endl << " > ";
        fflush(stdout);
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    ble.read(NORDIC_UART_SERVICE_UUID, NORDIC_UART_CHAR_TX, [&](const uint8_t* data, uint32_t length) {
        std::cout << "\r<" << devices[device].name << "> " << "(" << length << ") ";
        for (int i = 0; i < length; i++) { std::cout << data[i]; }
        std::cout << std::endl << " > ";
        fflush(stdout);
    });

    // * Endless loop to send/receive BLE UART
    std::string message;
    while (true) {
        getline(std::cin, message);
        ble.write_request(NORDIC_UART_SERVICE_UUID, NORDIC_UART_CHAR_RX, message);
        std::cout << " > ";
    }

    ble.disconnect();
    ble.dispose();
}