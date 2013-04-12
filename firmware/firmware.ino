#include <SoftwareSerial.h>

#define SOFTWARE_RX_PIN 2
#define SOFTWARE_TX_PIN 3

#define LED_RED   6
#define LED_BLUE  9
#define LED_GREEN 5

#define RESET_PIN     7
#define CONNECTED_PIN 4

#define SWITCH_A 10
#define SWITCH_B 8

uint8_t expectedTransmitACK[5] = {0x02, 0x80, 0x01, 0x00, 0x81};
uint8_t expectedRecieveACK[3] = {0x02, 0x20, 0xDD};

uint8_t count = 0;
long lastNotifyTime = millis();

SoftwareSerial mySerial(SOFTWARE_RX_PIN, SOFTWARE_TX_PIN);

void setLED(uint8_t R, uint8_t G, uint8_t B) {
  analogWrite(LED_RED,    R);
  analogWrite(LED_GREEN,  G);
  analogWrite(LED_BLUE,   B);
}

void read_ble(uint8_t buffer[]) {
  // Zero the buffer
  for(int x = 0; x < 64; x++) {
    buffer[x] = 0x00;
  }

  // Make sure we got all the data
  delay(50);

  // Read all the data   
  for(int x=0; x<64; x++) {
    buffer[x] = mySerial.read();
  }  
}

void BLE_Send(char *str){
  setLED(0,255,0);  // All good!
  
  boolean error = false;

  int exOR_result = 0xFD;
  int dataLength = 0;

  // Calculate the length of the message
  for(dataLength=0; str[dataLength]!=0; dataLength++);

  // We can handle a maximum of 20 bytes, so bail if it's too long
  if(dataLength > 20) {
    setLED(0,0,0);
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
  
  // Wait for a reply
  delay(50);
  
  uint8_t buffer[64];
  
  // Read reply and check for a positive ACKnowledgement
  if (mySerial.available() != 5) {
    error = true;
  } 
  else {
    
    read_ble(buffer);
    
    for (uint8_t i = 0; i < sizeof(expectedTransmitACK); i++) {
      if (buffer[i] != expectedTransmitACK[i]) {
        error = true; 
      }
    }
  }
  
  // Handle a data send failure
  if(error) {
    Serial.println("Error sending data to the BLE module.");
    dump_buffer(buffer);
    error_flash();
    return;
  }

  setLED(0,0,0);
}

// Recieve a message from the BLE shield
int BLE_Recieve(char* inbuffer) {
  uint8_t buffer[64];
  boolean error = false;
  
  setLED(0,0,255);

  // Zero the buffer
  for(int x = 0; x < 64; x++) {
    buffer[x] = 0x00;
    inbuffer[x] = 0x00;
  }

  // Make sure we got all the data
  delay(50);

  // Read all the data   
  read_ble(buffer);  
  
  // Make sure the header is as expected
  for (int i = 0; i < 3; i++) {
    if (buffer[i] != expectedRecieveACK[i]) {
        error = true;
    }
  }
  
  setLED(0, 0, 0);
  
  // Handle recieve errors. TODO: XOR Checksum
  if (error == true) {
    Serial.println("Error receiving data from the BLE module.");
    dump_buffer(buffer);
    error_flash();
    return 0;
  }
  
  // Copy the payload bytes to the provided char array
  int length = buffer[3];
  char payload[length];
  for (int x = 0; x < length; x++) {
    inbuffer[x] = (char)buffer[4 + x];
  }
  
  return length;
}

// Debugging
void dump_buffer(uint8_t buffer[]) {
  for (int x=0; x < 64; x++) {
    Serial.print(buffer[x], HEX);
    Serial.print(" ");
  }
  Serial.println();
}

// Flash the RED Led
void error_flash() {
 for (int x=0; x < 4; x++) {
    setLED(255,0,0);
    delay(75);    
    setLED(0,0,0);
    delay(75);
  }
}


void setup() { 
  //Initialize serial and wait for port to open:
  pinMode(SOFTWARE_RX_PIN, INPUT);
  pinMode(SOFTWARE_TX_PIN, OUTPUT);
  mySerial.begin(9600);
  mySerial.listen();

  Serial.begin(115200);

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

  // While we are connected, send a test sequence every second
  if(digitalRead(CONNECTED_PIN) && (millis() - lastNotifyTime > 1000)) {
    setLED(20,20,20);  // white: connected  
    
    char message[20];
    snprintf(message, 20, "Connected %is", count);
    BLE_Send(message);

    Serial.println(message);
    count++;
    
    lastNotifyTime = millis();
    setLED(0,0,0);
  }
  
  // Check to see if we've received any data from the BLE shield
  if (mySerial.available() > 0) {
    char buffer[64];
    int length = BLE_Recieve(buffer);
    Serial.print("Message recieved: ");
    Serial.println(buffer);
  }

  if(digitalRead(SWITCH_A) == LOW) {
    setLED(0,20,0); //green: switch a pressed
    BLE_Send("Button A!");
  }
  else if(digitalRead(SWITCH_B) == LOW) {
    setLED(0,0,20); // blue: switch b pressed
    BLE_Send("Button B!");
  }
  else if(digitalRead(CONNECTED_PIN) == LOW) {
    setLED(127,0,0); 
  }
  else {
    setLED(0,0,0); 
  }
  
  
}

