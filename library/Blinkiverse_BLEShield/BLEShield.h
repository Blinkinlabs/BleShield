#ifndef BLEShield_h
#define BLEShield_h

#include <SoftwareSerial.h>
#include <util/delay.h>

class BLEShield {
public:
	BLEShield();
	void print(const char *str);
	void print(String str);
	int readChars(char* inbuffer);
	int available();
	bool connected();
	bool readSwitchA();
	bool readSwitchB();
	
	void setLED(uint8_t R, uint8_t G, uint8_t B);
	void errorFlash();

private:
	char ble_ACK[6];

	void readBLE(uint8_t buffer[]);
	void dumpBuffer(uint8_t buffer[]);
	int parse(uint8_t buffer[]);

};



#endif // BLEShield_h