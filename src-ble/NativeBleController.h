#pragma once

#include "NativeBleControllerTypes.h"

using namespace std::string_literals;  // enables s-suffix for std::string literals

namespace NativeBLE {

class NativeBleInternal;

class NativeBleController {
    // TODO Add getServices, getCharacteristicsForService.
    // TODO Add a callback to notify the user of disconnections.

  private:
    // * Pointer to object specific to platform (Windows, macOS, Linux)
    NativeBleInternal *internal;

  public:
    /**
     * NativeBleController::NativeBleController
     *
     * Constructor to initialize the internal object
     *
     */
    NativeBleController();

    /**
     * NativeBleController::~NativeBleController
     *
     * Destructor
     *
     */
    ~NativeBleController();

    /**
     * NativeBleController::setup
     *
     * Sets up the callback holder so that all functions have pointers to callbacks
     *
     * @param callback_holder: object containing a set of callback functions
     */
    void setup(CallbackHolder callback_holder);

    /**
     * NativeBleController::scan_start
     *
     * Starts an indefinite BLE scan (must be stopped by scan_stop())
     * While scanning, when a device is found, the callback for a device found will
     * be called from callback_holder.
     *
     */
    void scan_start();

    /**
     * NativeBleController::scan_stop
     *
     * Stops a BLE scan (started by scan_start())
     *
     */
    void scan_stop();

    /**
     * NativeBleController::scan_is_active
     *
     * Returns a boolean of whether there is a BLE scan active currently
     *
     * @return whether scan is active currently
     */
    bool scan_is_active();

    /**
     * NativeBleController::scan_timeout
     *
     * Starts a BLE scan for a prescribed amount of time and stops.
     * While scanning, when a device is found, the callback for a device found will
     * be called from callback_holder.
     *
     * @param timeout_ms: number of milliseconds to scan
     */
    void scan_timeout(int32_t timeout_ms);

    /**
     * NativeBleController::is_connected
     *
     * Returns a boolean of whether BLE is connected to a device currently
     *
     * @return whether BLE device is connected currently
     */
    bool is_connected();

    /**
     * NativeBleController::connect
     *
     * Connects to the provided BLE MAC address
     *
     * @param address: the MAC address in a 12 character string format
     */
    void connect(const BluetoothAddress &address);

    /**
     * NativeBleController::write_request
     *
     * Writes to the provided characteristic on the provided service using
     * a regular BLE WRITE command
     *
     * @param service: UUID of the BLE service
     * @param characteristic: UUID of the characteristic on the service
     * @param data: a string containing the data that should be written to the characteristic
     */
    void write_request(BluetoothUUID service, BluetoothUUID characteristic, DataChunk data);

    /**
     * NativeBleController::write_command
     *
     * Writes to the provided characteristic on the provided service using
     * a BLE WRITE_NO_RESPONSE command
     *
     * @param service: UUID of the BLE service
     * @param characteristic: UUID of the characteristic on the service
     * @param data: a string containing the data that should be written to the characteristic
     */
    void write_command(BluetoothUUID service, BluetoothUUID characteristic, DataChunk data);

    /**
     * NativeBleController::read
     *
     * Reads from the provided characteristic on the provided service
     *
     * @param service: UUID of the BLE service
     * @param characteristic: UUID of the characteristic on the service
     * @param callback_on_read: callback std::function for when the read is complete, providing
     *                          pointer to data and length of data
     */
    void read(BluetoothUUID service, BluetoothUUID characteristic,
              std::function<void(const uint8_t *data, uint32_t length)> callback_on_read);

    /**
     * NativeBleController::notify
     *
     * Enables notification for when the BLE device notifies the provided characteristic
     *
     * @param service: UUID of the BLE service
     * @param characteristic: UUID of the characteristic on the service
     * @param callback_on_notify: callback std::function for when a notification happens, providing
     *                            pointer to data and length of data
     */
    void notify(BluetoothUUID service, BluetoothUUID characteristic,
                std::function<void(const uint8_t *data, uint32_t length)> callback_on_notify);

    /**
     * NativeBleController::indicate
     *
     * Enables indication for when the BLE device notifies the provided characteristic
     *
     * @param service: UUID of the BLE service
     * @param characteristic: UUID of the characteristic on the service
     * @param callback_on_indicate: callback std::function for when an indication happens, providing
     *                              pointer to data and length of data
     */
    void indicate(BluetoothUUID service, BluetoothUUID characteristic,
                  std::function<void(const uint8_t *data, uint32_t length)> callback_on_indicate);

    /**
     * NativeBleController::disconnect
     *
     * Disconnects from BLE device
     *
     */
    void disconnect();

    /**
     * NativeBleController::dispose
     *
     * Dispose of any remaining pointers or information from NativeBleController
     *
     */
    void dispose();
};
}  // namespace NativeBLE