#include <SoftwareSerial.h>

char ble_ACK[6];
char sendDATA(char *str);

#define SOFTWARE_RX_PIN 2
#define SOFTWARE_TX_PIN 3


#define LED_RED   6
#define LED_BLUE  9
#define LED_GREEN 5

#define RESET_PIN     7
#define CONNECTED_PIN 4

#define SWITCH_A 10
#define SWITCH_B 8


SoftwareSerial mySerial(SOFTWARE_RX_PIN, SOFTWARE_TX_PIN);

void setLED(uint8_t R, uint8_t G, uint8_t B) {
  analogWrite(LED_RED,    R);
  analogWrite(LED_GREEN,  G);
  analogWrite(LED_BLUE,   B);
}

char sendDATA(char *str){
  int exOR_result = 0xFD;
  int str_length = 0;
  
  for(int i=0; i<6; i++) {
    ble_ACK[i] = 0;
  }

  // Assuming a null-terminated string, count it's length    
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

void setup() { 
 //Initialize serial and wait for port to open:
  mySerial.begin(9600);

//  Serial.begin(9600);

  // Set the switch pins to inputs
  pinMode(CONNECTED_PIN, INPUT);  
  pinMode(SWITCH_A, INPUT);
  pinMode(SWITCH_B, INPUT);

  // Disable the RGB led
  setLED(20,0,0);
  
  // Pull the BLE module out of reset
  pinMode(RESET_PIN, INPUT);
    
//  while (!Serial) {
//    ; // wait for serial port to connect. Needed for Leonardo only
//  }
}

void loop() {
  uint count = 0;
  char message[20];
    
  // While we are connected, send a test sequence
  while(digitalRead(CONNECTED_PIN)) {
    setLED(0,0,20);  // Blue: connected

    snprintf(message, 20, "Connected %is", count);
    sendDATA(message);
    count++;
    
    delay(1000);
  
//    // Why do this?
//    for(int i=0; i<6; i++) {
//      Serial.print(ble_ACK[i]);
//    }
  }
  setLED(20,0,0);   // Red: disconnected
}
