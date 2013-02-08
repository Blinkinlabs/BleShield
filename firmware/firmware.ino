#include <SoftwareSerial.h>

char ble_ACK[6];
char sendDATA(char *str);

SoftwareSerial mySerial(3, 2); // RX, TX

void setup() { 
 //Initialize serial and wait for port to open:
  mySerial.begin(9600);
  Serial.begin(9600);

  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
} 

void loop() { 
  sendDATA("TEST");
  delay(1000);
  sendDATA("TEST2");
  delay(1000);
  
  /*for(int i=0; i<6; i++)
    Serial.print(ble_ACK[i]);
  delay(4000);*/
}

char sendDATA(char *str){
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
}378.49
