# NativeBLE

Welcome to the NativeBLE tutorial!

This page will explain in-depth the code for the NativeBLE test application so that you, as the developer, can understand how to incorporate the Native BLE library into your own projects.

The source code test application can be found in `src-ble-test`, and after using CMake to build the library, the application will be compiled to the `bin` folder.

## An Overview
The purpose of the tester application is to expose the developer to a basic use-case of the Native BLE library. Although the library can be used to interface with any BLE device, in this test application we will interface with a Nordic UART characteristic. This can be on a Nordic BLE microcontroller, or can be emulated using the nRF Connect app on the Android Play Store.

In this tutorial we will use the nRF Connect app to interface with the test application.

## A Basic Explanation of BLE
BLE is a protocol designed to exchange information with various devices and sensors using very low energy, hence the name Bluetooth Low Energy. In a basic sense, the protocol involves a Central device (in our case the PC) and any Peripheral device (any sensor, device, app) that the Central device wants to get information from.

Each Peripheral has a set of services which each have a set of characteristics that have read and/or write properties. Characteristics that contain information that should be read by the Central will have read properties. When information needs to be sent to the Peripheral, the Central will write to a characteristic with a write property. Characteristics can also have both properties enabled.

Within the read properties there are:
- Read
    - allows the Central device to manually read the value of a characteristic at any time
- Notify
    - issues a notification to the Central device that the value of a characteristic has been updated
    - Central device will issue a response to Peripheral indicating it received the notification
- Indicate
    - issues a notification to the Central device that the value of a characteristic has been updated
    - no response is sent to Peripheral

Within the write properties there are:
* Write
    * Central will write to a characteristic of Peripheral and will expect a response from the Peripheral indicating the write was successful
* Write With No Response
    * Central will write to a characteristic of Peripheral and will not expect a response

## Test Application Code
In this section, we will go through the code of the Test Application to understand how to use the Native BLE library.

### Setup
First, we need to declare a `NativeBleController` object from the `NativeBLE` namespace. We can declare this in the global namespace so that it's a static variable.
```
NativeBLE::NativeBleController ble;
```


We also want to define some constants. We will define a constant for how long we want to scan for devices, `SCAN_TIMEOUT_MS` and also constants for the UUIDs of the Nordic UART Service and its TX and RX characteristics.
```
#define SCAN_TIMEOUT_MS 5000

#define NORDIC_UART_SERVICE_UUID "6e400001-b5a3-f393-e0a9-e50e24dcca9e"
#define NORDIC_UART_CHAR_RX "6e400002-b5a3-f393-e0a9-e50e24dcca9e"
#define NORDIC_UART_CHAR_TX "6e400003-b5a3-f393-e0a9-e50e24dcca9e"
```

Now, we want to start setting up our `NativeBleController` object, `ble`. We'll start with declaring the set of callback lambdas that `ble` will use. We'll also declare an array of `DeviceDescriptor`s that will contain a list of all the devices discovered after scanning.
```
NativeBLE::CallbackHolder ble_events;
std::vector<NativeBLE::DeviceDescriptor> devices;
```

The `callback_on_device_disconnected` passes the reason for disconnect as a string. We will simply print this.
> The exact message will differ on each platform because Windows, Linux, and macOS differ in their disconnection messages.
```
ble_events.callback_on_device_disconnected = [](std::string msg) {
    std::cout << "Disconnected: " << msg << std::endl;
    return;
};
```

The `callback_on_device_connected` is called when a Peripheral is connected and its services and characteristics are discovered.
```
ble_events.callback_on_device_connected = []() {
    std::cout << "Connected" << std::endl;
};
```

The `callback_on_scan_found` is called whenever a new device is discovered during a scan. We want to save these devices to an array that we can use to remember the MAC address of the device we want to connect to. We also print to the screen how many devices have been found so far.
> CoreBluetooth, which runs on Apple devices (macOS, iOS, iPadOS) do not report Bluetooth MAC address. Instead they internally map the addresses to a UUID which is exposed to the developer. So, on Apple devices, `device.address` will provide this UUID and not the Bluetooth MAC address that other platforms show.
```
ble_events.callback_on_scan_found = [&](NativeBLE::DeviceDescriptor device) {
    devices.push_back(device);
    std::cout << "\r" << devices.size() << " devices found.";
    fflush(stdout);
};
```

The `callback_on_scan_start` and `callback_on_scan_stop` are called when scanning starts and stops, respectively.
```
ble_events.callback_on_scan_start = []() {
    std::cout << "Scanning for " << SCAN_TIMEOUT_MS << " ms..." << std::endl;
};
ble_events.callback_on_scan_stop = []() {
    std::cout << std::endl << "Scan complete." << std::endl;
};
```

Now that we have initialized the callbacks, we can pass them into the `setup` method of `ble`.
> Note that the callbacks can also be left empty when initializing `ble`.
```
ble.setup(ble_events);
```

This completes the setup portion of the `NativeBleController`. Now we can use use `ble` to scan, connect, write and read.

### Scanning and Connecting to Device
To start scanning for devices, we call the `scan_timeout` method.
> This method is blocking, which means that it will not complete until `SCAN_TIMEOUT_MS` milliseconds has passed. In this case that is 5000 milliseconds. You can use `scan_start` and `scan_stop` to implement a non-blocking scanning operation as well.
```
ble.scan_timeout(SCAN_TIMEOUT_MS);
```

Our `callback_on_scan_found` will populate the `devices` array we declared earlier, so when the scanning finishes, we can print out this list in a readable form for the user to choose which device to connect to.
```
for (int i = 0; i < devices.size(); i++) {
    std::cout << "  " << i ": " devices[i].name << " (" << devices[i].address << ")" << std::endl;
}
std::cout << "Type index of device to connect to: ";
int device;
std::cin >> device;
```

Now to connect to the desired device, we pass its address into the `connect` method.
> This method is blocking and will not return until the device is connected and the services and characteristics are discovered.
```
ble.connect(devices[device].address);
```

Now we are ready to enable communication with the BLE device. Remember, in this example we are writing and reading to the Nordic UART Service, but when working with other devices, like a BLE Heart Rate monitor, you would be reading/writing to its services.

### Enabling Communication
In order to receive notifications on whether a characteristic's value has been updated on the device we are connected to, we need to call the `notify` method and provide a callback lambda for when the characteristic's value is updated. In our callback, we add some formatting to show the message from the device on the screen.
> We can also call the `indicate` method with the same parameters to enable indications.
```
ble.notify(NORDIC_UART_SERVICE_UUID, NORDIC_UART_CHAR_TX, [7](const uint8_t* data, uint32_t length) {
    std::cout << "\r" << devices[device].name << "> ";
    for (int i = 0; i < length; i++) { std::cout << data[i]' }
    std::cout << std::endl; << " > ";
    fflush(stdout);
});
```

After enabling notifications, we want to create a loop that the application will stay in so that we can send messages to the device. Our loop will read in all characters that are typed until an ENTER, and send these characters to the peripheral device's RX Characteristic.
> Here, `write_request` is used, which is a write with response operation, as described in A Basic Explanation of BLE. We can also use `write_command` with the same parameters to issue a write without response operation.
```
std::string message;
while (true) {
    getline(std::cin, message);
    ble.write_request(NORDIC_UART_SERVICE_UUID, NORDIC_UART_CHAR_RX, message);
    std::cout << " > ";
}
```

At this point, your app will be able to talk to a peripheral device that has the Nordic UART Service with the TX and RX characteristic. The last thing we will do is add some safe practices for disconnecting from the BLE device when your app is closed.

### Disconnecting
To close this test application after starting, you can use the Ctrl-C command. However, we want to add some code that will properly disconnect and dispose of our `NativeBleController` object before closing the application, to ensure that the Bluetooth hardware on your device is released appropriately.

We will add a signal handler in our code to handle this when the program closes, using the `std::signal` type. First, let's create our signal handler.
```
void signal_handler(int signal) {
    if (signal == SIGINT) {
        std::cout << std::endl << "User quit program." << std::endl;
    }
    ble.disconnect();
    ble.dispose();
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    exit(signal);
}
```

The `sleep_for` is to wait so that the `callback_on_device_disconnected` is called and the disconnect message is printed before the program closes.

The full source code for this test application can be found in the `src-ble-test` folder in the repository.

