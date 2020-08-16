#import <CoreBluetooth/CoreBluetooth.h>
#import <Foundation/Foundation.h>
#import <functional>

@interface Macable:NSObject<CBCentralManagerDelegate, CBPeripheralDelegate>
{
    CBCentralManager *centralManager;
    dispatch_queue_t dispatchQueue;
}

@property (readonly, getter=isPeripheralReady) BOOL peripheralReady;
@property (strong) CBPeripheral *peripheral;
@property std::function<void(CBPeripheral*, NSDictionary*, NSNumber*)> onScanFound;
@property std::function<void()> onConnected;
@property std::function<void(NSString*)> onDisconnected;
@property std::function<void(NSData*)> onCharacteristicUpdated;

/**
 * [Macable setup]
 * 
 * Initialize the CoreBluetooth driver
 *
 */
-(void) setup;

/**
 * [Macable startScan]
 * 
 * Begin scanning indefinitely for BLE devices
 * Each time a peripheral is found onScanFound will be called with the peripheral's information
 *
 */
-(void) startScan;

/**
 * [Macable stopScan]
 * 
 * Stop scanning
 *
 */
-(void) stopScan;

/**
 * [Macable connect]
 * 
 * Will connect to the peripheral saved in @property peripheral and attaches Macable as delegate
 * for peripheral events
 * ! Must save to property by calling [Macable setPeripheral:] !
 * onConnected will be called when the peripheral is connected and all services and characteristics
 * are populated
 *
 */
-(void) connect;

/**
 * [Macable getPeripheralWithUUID:]
 * 
 * Returns a CBPeripheral matching the provided UUID if it is found, nil otherwise
 *
 * @param uuid: the uuid of the desired peripheral
 * @return CBPeripheral object with provided UUID
 *
 */
-(CBPeripheral *) getPeripheralWithUUID:(NSUUID *)uuid;

/**
 * [Macable disconnect]
 * 
 * Disconnects from the peripheral saved in @property peripheral and detaches Macable as delegate
 * for peripheral events.
 * onDisconnected will be called when the peripheral disconnects
 *
 */
-(void) disconnect;

/**
 * [Macable readValueOnCharacteristic:service:]
 * 
 * Read the value on the provided characteristic and service. onCharacteristicUpdated will
 * be called with the value if read is successful
 *
 * @param characteristic_uuid: the uuid of the characteristic
 * @param service_uuid: the uuid of the service
 *
 */
-(void) readValueOnCharacteristic:(CBUUID*)characteristic_uuid service:(CBUUID*)service_uuid;

/**
 * [Macable subscribeToNotificationsOnCharacteristic:service:]
 * 
 * Subscribe to notifications on the provided characteristic and service. onCharacteristicUpdated will
 * be called with the value whenever the characteristic is updated
 *
 * @param characteristic_uuid: the uuid of the characteristic
 * @param service_uuid: the uuid of the service
 *
 */
-(void) subscribeToNotificationsOnCharacteristic:(CBUUID *)characteristic_uuid service:(CBUUID *)service_uuid;

/**
 * [Macable subscribeToIndicationsOnCharacteristic:service:]
 * 
 * Subscribe to indications on the provided characteristic and service. onCharacteristicUpdated will
 * be called with the value whenever the characteristic is updated
 *
 * @param characteristic_uuid: the uuid of the characteristic
 * @param service_uuid: the uuid of the service
 *
 */
-(void) subscribeToIndicationsOnCharacteristic:(CBUUID *)characteristic_uuid service:(CBUUID *)service_uuid;

/**
 * [Macable writeData:toCharacteristic:service:]
 * 
 * Writes data to provided characteristic and service using a WriteWithResponse command
 *
 * @param data: data to be written
 * @param characteristic_uuid: the uuid of the characteristic
 * @param service_uuid: the uuid of the service
 *
 */
-(void) writeData:(NSData*)data toCharacteristic:(CBUUID*)characteristic_uuid service:(CBUUID*)service_uuid;

/**
 * [Macable writeNoResponseData:toCharacteristic:service]
 * 
 * Writes data to provided characteristic using a WriteWithoutResponse command
 *
 * @param data: data to be written
 * @param characteristic_uuid: the uuid of the characteristic
 * @param service_uuid: the uuid of the service
 *
 */
-(void) writeNoResponseData:(NSData*)data toCharacteristic:(CBUUID*)characteristic_uuid service:(CBUUID*)service_uuid;

@end