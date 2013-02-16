#include "BLEShield.h"

BLEShield::BLEShield() {
	mySerial.begin(9600);
}

char sendData(char *str) {
	int exOR_result = 0xFD;
	int str_length = 0;
	for(int i=0; i<6; i++)
    ble_ACK[i] = 0;
    
	for(str_length=0; str[str_length]!=0; str_length++);
	if(str_length <= 20){
	    mySerial.write(0xFF);
	    mySerial.write(0xFF);
	    mySerial.write(0xFF);  
	    mySerial.write(0x02);
	    mySerial.write(0x20);
	    mySerial.write(0xDD);
	    mySerial.write(str_length);
	    exOR_result = exOR_result ^ str_length;
	    
	    for(int counter=0; counter< str_length; counter++){
	      mySerial.write(str[counter]);
	      exOR_result = exOR_result ^ str[counter];
	    }
	    
	    mySerial.write(exOR_result);
	    mySerial.setTimeout(100);
	    mySerial.readBytes(ble_ACK, 6);
	}
}