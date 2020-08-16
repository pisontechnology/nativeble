#import "macable.h"

int servicesCount;

@implementation Macable
-(id) init {
    self = [super init];

    if (self) {
        _peripheralReady = NO;
        _onScanFound = [](CBPeripheral*, NSDictionary*, NSNumber*) {};
        _onConnected = []() {};
        _onDisconnected = [](NSString*) {};
        _onCharacteristicUpdated = [](NSData*) {};
    }

    return self;
}

-(void) setup {
    dispatchQueue = dispatch_queue_create("CBqueue", DISPATCH_QUEUE_SERIAL);
    centralManager = [[CBCentralManager alloc]initWithDelegate:self queue:dispatchQueue];
    _peripheralReady = NO;
}

-(void) startScan {
    [centralManager scanForPeripheralsWithServices:nil options:nil];
}

-(void) stopScan {
    [centralManager stopScan];
}

-(void) connect {
    if (_peripheral != nil) {
        _peripheral.delegate = self;
        [centralManager connectPeripheral:_peripheral options:nil];
    }
}

-(CBPeripheral*) getPeripheralWithUUID:(NSUUID*)uuid {
    NSArray* peripherals = [centralManager retrievePeripheralsWithIdentifiers:@[uuid]];

    if ([peripherals count] > 0) {
        return [peripherals objectAtIndex:0];
    }

    return nil;
}

-(void) disconnect {
    if (_peripheral != nil) {
        [centralManager cancelPeripheralConnection:_peripheral];
    }
}

-(void) readValueOnCharacteristic:(CBUUID*)characteristic_uuid service:(CBUUID*)service_uuid {
    for (CBService* service in _peripheral.services) {
        if ([service.UUID isEqual:service_uuid]) {
            for (CBCharacteristic* characteristic in service.characteristics) {
                if ([characteristic.UUID isEqual:characteristic_uuid]) {
                    if ((characteristic.properties & CBCharacteristicPropertyRead) == CBCharacteristicPropertyRead) {
                        [_peripheral readValueForCharacteristic:characteristic];
                        break;
                    }
                    return;
                }
            }
            break;
        }
    }
}

-(void) subscribeToNotificationsOnCharacteristic:(CBUUID*)characteristic_uuid service:(CBUUID*)service_uuid {
    for (CBService* service in _peripheral.services) {
        if ([service.UUID isEqual:service_uuid]) {
            for (CBCharacteristic* characteristic in service.characteristics) {
                if ([characteristic.UUID isEqual:characteristic_uuid]) {
                    if ((characteristic.properties & CBCharacteristicPropertyNotify) == CBCharacteristicPropertyNotify) {
                        [_peripheral setNotifyValue:YES forCharacteristic:characteristic];
                        break;
                    }
                    return;
                }
            }
            break;
        }
    }
}

-(void) subscribeToIndicationsOnCharacteristic:(CBUUID*)characteristic_uuid service:(CBUUID*)service_uuid {
    for (CBService* service in _peripheral.services) {
        if ([service.UUID isEqual:service_uuid]) {
            for (CBCharacteristic* characteristic in service.characteristics) {
                if ([characteristic.UUID isEqual:characteristic_uuid]) {
                    if ((characteristic.properties & CBCharacteristicPropertyIndicate) == CBCharacteristicPropertyIndicate) {
                        [_peripheral setNotifyValue:YES forCharacteristic:characteristic];
                        break;
                    }
                    return;
                }
            }
            break;
        }
    }
}

-(void) writeData:(NSData*)data toCharacteristic:(CBUUID*)characteristic_uuid service:(CBUUID*)service_uuid {
    //TODO: Add breaks and returns to stop looping once found
    for (CBService* service in _peripheral.services) {
        if ([service.UUID isEqual:service_uuid]) {
            for (CBCharacteristic* characteristic in service.characteristics) {
                if ([characteristic.UUID isEqual:characteristic_uuid]) {
                    if ((characteristic.properties & CBCharacteristicPropertyWrite) == CBCharacteristicPropertyWrite) {
                        [_peripheral writeValue:data forCharacteristic:characteristic type:CBCharacteristicWriteWithResponse];
                    }
                }
            }
        }
    }
}

-(void) writeNoResponseData:(NSData*)data toCharacteristic:(CBUUID*)characteristic_uuid service:(CBUUID*)service_uuid {
    //TODO: Add breaks and returns to stop looping once found
    for (CBService* service in _peripheral.services) {
        if ([service.UUID isEqual:service_uuid]) {
            for (CBCharacteristic* characteristic in service.characteristics) {
                if ([characteristic.UUID isEqual:characteristic_uuid]) {
                    if ((characteristic.properties & CBCharacteristicPropertyWriteWithoutResponse) == CBCharacteristicPropertyWriteWithoutResponse) {
                        [_peripheral writeValue:data forCharacteristic:characteristic type:CBCharacteristicWriteWithoutResponse];
                    }
                }
            }
        }
    }
}

-(void) centralManager:(CBCentralManager*)central didDiscoverPeripheral:(CBPeripheral*)device
        advertisementData:(NSDictionary*)advertisementData RSSI:(NSNumber*)RSSI {
    self.onScanFound(device, advertisementData, RSSI);
}

-(void) centralManagerDidUpdateState:(CBCentralManager*)central {}

-(void) centralManager:(CBCentralManager*)central didConnectPeripheral:(CBPeripheral*)device {
    [device discoverServices:nil];
}

-(void) centralManager:(CBCentralManager*)central didDisconnectPeripheral:(CBPeripheral*)device 
        error:(NSError*)error {
    _peripheral.delegate = nil;

    _peripheralReady = NO;
    if (error.localizedDescription == nil) {
        self.onDisconnected(@"Manually disconnected.");
    } else {
        self.onDisconnected(error.localizedDescription);
    }
}

-(void) peripheral:(CBPeripheral*)peripheral didUpdateValueForCharacteristic:(CBCharacteristic*)characteristic
    error:(NSError*)error {
    self.onCharacteristicUpdated(characteristic.value);
}

-(void) peripheral:(CBPeripheral*)peripheral didDiscoverServices:(NSError*)error {
    if (error == nil) {
        servicesCount = [peripheral.services count];
        for (CBService* service in peripheral.services) {
            [peripheral discoverCharacteristics:nil forService:service];
        }
    }
}

-(void) peripheral:(CBPeripheral*)peripheral didDiscoverCharacteristicsForService:(CBService*)service
    error:(NSError*)error {

    servicesCount--;

    if (error == nil && servicesCount == 0) {
        _peripheralReady = YES;
        self.onConnected();
    }
}


@end