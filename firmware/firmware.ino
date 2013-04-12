#include <SoftwareSerial.h>

#define expectedACKLength 5
uint8_t expectedACK[expectedACKLength] = {0x02, 0x80, 0x01, 0x00, 0x81};

#define BLE_ACK_LENGTH 6
char bleACK[BLE_ACK_LENGTH];

int transmitCount;
int receiveErrorCount;

#define MAXIMUM_RECEIVE_ERRORS_FOR_FAIL 2
#define MINIMUM_TRANSMISSIONS_FOR_PASS 3

#define SOFTWARE_RX_PIN 2
#define SOFTWARE_TX_PIN 3


#define LED_RED   6
#define LED_BLUE  9
#define LED_GREEN 5

#define RESET_PIN     7
#define CONNECTED_PIN 4

#define SWITCH_A 8
#define SWITCH_B 10


SoftwareSerial mySerial(SOFTWARE_RX_PIN, SOFTWARE_TX_PIN);

void setLED(uint8_t R, uint8_t G, uint8_t B) {
  analogWrite(LED_RED,    R);
  analogWrite(LED_GREEN,  G);
  analogWrite(LED_BLUE,   B);
}

void sendDATA(char *str){
  if(transmitCount >= MINIMUM_TRANSMISSIONS_FOR_PASS) {
    setLED(0,255,0);  // All good!
  }
  
  int exOR_result = 0xFD;
  int dataLength = 0;
  

  // Calculate the length of the message
  for(dataLength=0; str[dataLength]!=0; dataLength++);
  
  // We can handle a maximum of 20 bytes, so bail if it's too long
  if(dataLength > 20) {
    return;
  }
  
  // Flush the software serial buffer, in case there was stuff in it
  mySerial.flush();
  
  mySerial.write(0xFF);
  mySerial.write(0xFF);
  mySerial.write(0xFF);  
  mySerial.write(0x02);
  mySerial.write(0x20);
  mySerial.write(0xDD);
  mySerial.write(dataLength);
  exOR_result = exOR_result ^ dataLength;
  for(int counter=0; counter< dataLength; counter++){
    mySerial.write(str[counter]);
    exOR_result = exOR_result ^ str[counter];
  }
  mySerial.write(exOR_result);

  int receivedBytes = mySerial.readBytes(bleACK, 6); // Ack is 5, Nack is 6
  
  if(receivedBytes != 5) {
    receiveErrorCount++;
  }
  else {
    for (uint8_t i = 0; i < expectedACKLength; i++) {
      if ((uint8_t)bleACK[i] != expectedACK[i]) {
        receiveErrorCount++;
      }
    }
  }
  
  while(receiveErrorCount >= MAXIMUM_RECEIVE_ERRORS_FOR_FAIL) {
    setLED(255,0,0);
    delay(500);    
    setLED(0,0,0);
    delay(500);
  }

  char buff[30];
  snprintf(buff, 30, "%i, %x,%x,%x,%x,%x\n", receivedBytes,
                 bleACK[0],bleACK[1],bleACK[2],bleACK[3],bleACK[4]);
  Serial.print(buff);
  
  transmitCount++;
}

void setup() { 
 //Initialize serial and wait for port to open:
  pinMode(SOFTWARE_RX_PIN, INPUT);
  pinMode(SOFTWARE_TX_PIN, OUTPUT);
  mySerial.begin(9600);
  mySerial.listen();
  mySerial.setTimeout(1000);
  receiveErrorCount = 0;

  Serial.begin(9600);

  // Set the switch pins to inputs
  pinMode(CONNECTED_PIN, INPUT);  
  pinMode(SWITCH_A, INPUT);
  pinMode(SWITCH_B, INPUT);

  // Disable the RGB led
  setLED(20,0,0);
  
  // Enable the reset output
  digitalWrite(RESET_PIN, HIGH);
  pinMode(RESET_PIN, OUTPUT);

  // Reset the BLE module  
  digitalWrite(RESET_PIN, LOW);
  delay(100);
  digitalWrite(RESET_PIN, HIGH);  

//  while (!Serial) {
//    ; // wait for serial port to connect. Needed for Leonardo only
//  }
}

void loop() {
  unsigned int count = 0;
  char message[20];
    
  // While we are connected, send a test sequence
  while(digitalRead(CONNECTED_PIN)) {
    setLED(20,20,20);  // white: connected
    
    snprintf(message, 20, "Connected %is", count);
    sendDATA(message);
    count++;
    delay(1000);
  }
  
  if(digitalRead(SWITCH_A) == 0) {
    setLED(0,20,0); //green: switch a pressed
  }
  else if(digitalRead(SWITCH_B) == 0) {
    setLED(0,0,20); // blue: switch b pressed
  }
  else {
    setLED(20,0,0);   // Red: disconnected
  }
}
