# NativeBLE

The purpose of NativeBLE is to add full-fledged BLE capability without the need to worry about specific platform compatibility issues. The API is designed to work on Windows, Linux, and macOS and allows you to write the same code for all three platforms.

The API is very simple and easy to use, giving developers the ability to interact with any BLE device. This library will allow you to:
* Manage connections to BLE devices
* Read and write to a characteristic
* Subscribe to notifications or indications for characteristics

The provided tester application shows how the same code can be compiled and run on any platform.

For more information, read our [TUTORIAL](TUTORIAL.md).

## Code Structure
The code is organized as follows:
* `src-ble`
    * An abstracted API that is platform-less (`NativeBleController`)
    * Internal code for each platform (`NativeBleInternal`)
* `src-ble-c-api`
    * Wrapper in standard C for the API
* `src-ble-test`
    * A tester application that uses the compiled `src-ble` library

## Build Instructions
We have specific scripts to compile the library on each platform. By default, the C wrapper and the tester application will also be built. The flags to enable each of these can be found in `CMakeLists.txt`.

All binaries will be placed in the `bin` folder.

### Windows
* Install Visual Studio 2019, selecting the C++ toolchain.
* Install CMake from https://cmake.org/
* Run `.\toolchains\windows\windows_compile.bat` from Powershell.
  * Use `-c` or `-clean` to perform a full rebuild of the project.
  * Use `-a=<ARCH>` or `-arch=<ARCH>` to build the libraries for a specific architecture. (Supported values are `x86` and `x64`.)

### Linux
* Run `./toolchains/linux/linux_install.sh` to install the OS dependencies. This should be required only once.
* Run `./toolchains/linux/linux_compile.sh` to build the project.
  * Use `-c` or `-clean` to perform a full rebuild of the project.

### macOS
* Install Homebrew from https://brew.sh/
* Run `brew install cmake` to install CMake.
* Run `./toolchains/macos/macos_compile.sh` to build the project.
  * Use `-c` or `-clean` to perform a full rebuild of the project.
The library will be compiled into a `.dylib` and saved into `bin/darwin`.

## License
All components within this project that have not been bundled from external creators, are licensed under the terms of the [MIT Licence](LICENCE.md).

## Important things about the macOS platform
Apple's Bluetooth API for its platforms macOS, iOS, and iPadOS is CoreBluetooth. NativeBLE uses CoreBluetooth for the macOS platform, and in turn, this library can be used for the other Apple platforms as well, without any major changes.

### Maximum Message Size
In BLE, there is a term known as MTU (Maximum Transmission Unit). This defines the largest size a message can be that is transferred from the Peripheral to the Central. On most platforms, this number is set to 244 bytes, however this number differs on various Apple devices. Below is a list of some Apple devices and their MTU.

- MacBook Pro 2016 and older without Touchbar: 104 bytes
- MacBook Pro 2016 and newer with Touchbar: 244 bytes
- iOS devices: 185 bytes

It is important to respect these numbers when developing an application with Apple devices, because sending a message larger than the MTU size will result in those bytes being clipped off without any indication. Unfortunately, CoreBluetooth does not expose any methods for identifying what the MTU of the device it is running on is, so this information must be passed to it from the peripheral. 

CoreBluetooth will always negotiate the highest possible MTU at the beginning of a connection. For example, if a peripheral supports an MTU of 244 bytes and your MacBook supports an MTU of 104 bytes, CoreBluetooth on your Mac will choose 104 because it is the highest possible MTU supported by both parties.

### MAC Addresses and UUIDs
In an effort to protect privacy, CoreBluetooth does not expose the MAC address of a device to a user. Instead, it randomizes the MAC address to a UUID (Universal Unique Identifier) that is exposed to the user. Because of this, you cannot manually input a Bluetooth MAC address into the `connect` method of `NativeBleController` on an Apple device. Instead, you will need to scan for devices and find the UUID of the desired device to connect to. This UUID will be passed into the `connect` method.

This will only be a problem if your application has hard-coded a Bluetooth MAC address to connect to. If you are scanning for devices, on Apple devices, the returned devices from the scan will have their address field filled with the UUID of the device and not the Bluetooth MAC address. This UUID can be used for the `connect` method.