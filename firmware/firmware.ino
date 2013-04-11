#include <SoftwareSerial.h>

#define BLE_ACK_LENGTH 6
char bleACK[BLE_ACK_LENGTH];
char sendDATA(char *str);

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

char sendDATA(char *str){
  int exOR_result = 0xFD;
  int str_length = 0;
  
  for(int i=0; i<BLE_ACK_LENGTH; i++) {
    bleACK[i] = 0;
  }

  // Assuming a null-terminated string, count it's length    
  for(str_length=0; str[str_length]!=0; str_length++);
  
  int bytes;
  
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
    
//    // Read some bytes. I mean, read them!
//    for(uint8_t i = 0; i < 5; i++) {
//      int c = mySerial.read();
//      if(c != -1) {
//        ble_ACK[i] = c;
//      }
//    }

    mySerial.setTimeout(1000);
    bytes = mySerial.readBytes(bleACK, 6); // Ack is 5, Nack is 6
  }
  
  
  // Check the ble ACK
//  #define expectedACKLength 5
//  uint8_t expectedACK[expectedACKLength] = {0x02, 0x80, 0x01, 0x00, 0x81};
//  for (uint8_t i = 0; i < expectedACKLength; i++) {
//    Serial.write(expectedACK[i]);
//    if ((uint8_t)bleACK[i] != expectedACK[i]) {
//      setLED(255,255,0);
//      delay(500);
//      setLED(0,0,0);
//      delay(500);
//      setLED(255,255,0);
//      delay(500);
//      setLED(0,0,0);
//      delay(500);
//      
//      char buff[30];
//      snprintf(buff, 30, "%i, %x,%x,%x,%x,%x\n", bytes,
//                     bleACK[0],bleACK[1],bleACK[2],bleACK[3],bleACK[4]);
//      Serial.write(buff);
//    }
//  }
}

void setup() { 
 //Initialize serial and wait for port to open:
  pinMode(SOFTWARE_RX_PIN, INPUT);
  pinMode(SOFTWARE_TX_PIN, OUTPUT);
  mySerial.begin(9600);
  mySerial.listen();

  Serial.begin(9600);

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
