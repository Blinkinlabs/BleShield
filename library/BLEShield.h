#ifndef BLEShield_H
#define BLEShield_H

#include <SoftwareSerial.h>

class BLEShield {
public:
	BLEShield();
	char sendData(char *str);

private:
	SoftwareSerial mySerial(3, 2); // RX, TX
	char ble_ACK[6];

};


#endif // BLEShield_H