#include "BLEShield.h"
#include "Arduino.h"

#define SOFTWARE_RX_PIN 2
#define SOFTWARE_TX_PIN 3

#define LED_RED   6
#define LED_BLUE  9
#define LED_GREEN 5

#define RESET_PIN     7
#define CONNECTED_PIN 4

#define SWITCH_A 10
#define SWITCH_B 8

#define TACK 0
#define RACK 1
#define GARBAGE 2

uint8_t expectedTransmitACK[5] = {0x02, 0x80, 0x01, 0x00, 0x81};
uint8_t expectedRecieveACK[3] = {0x02, 0x20, 0xDD};

SoftwareSerial _bleSerial(SOFTWARE_RX_PIN, SOFTWARE_TX_PIN);

BLEShield::BLEShield(void) {
  pinMode(SOFTWARE_RX_PIN, INPUT);
  pinMode(SOFTWARE_TX_PIN, OUTPUT);
  
  // Set the switch pins to inputs
  pinMode(CONNECTED_PIN, INPUT);  
  pinMode(SWITCH_A, INPUT);
  pinMode(SWITCH_B, INPUT);

   // Enable the reset output
  digitalWrite(RESET_PIN, HIGH);
  pinMode(RESET_PIN, OUTPUT);

  // Reset the BLE module  
  digitalWrite(RESET_PIN, LOW);
  _delay_ms(100);
  digitalWrite(RESET_PIN, HIGH);

  _bleSerial.begin(9600);
  _bleSerial.listen();

}

void BLEShield::setLED(uint8_t R, uint8_t G, uint8_t B) {
  analogWrite(LED_RED,    R);
  analogWrite(LED_GREEN,  G);
  analogWrite(LED_BLUE,   B);
}

// // Debugging
void BLEShield::dumpBuffer(uint8_t buffer[]) {
  for (int x=0; x < 64; x++) {
    Serial.print(buffer[x], HEX);
    Serial.print(" ");
  }
  Serial.println();
}

// Flash the RED Led
void BLEShield::errorFlash() {
 for (int x=0; x < 4; x++) {
    setLED(255,0,0);
    _delay_ms(75);    
    setLED(0,0,0);
    _delay_ms(75);
  }
}

int BLEShield::parse(uint8_t buffer[]) {
  for (uint8_t x=0; x < 64; x++) {

    int tack = 0;
    int rack = 0;

    // Look for a TACK
    for (uint8_t y=0; y < 5; y++) {
      if (buffer[x + y] != expectedTransmitACK[y]) {
        tack++;
      }
    }

    // Look for a RACK
    for (uint8_t y=0; y < 3; y++) {
      if (buffer[x + y] != expectedRecieveACK[y]) {
        rack++;
      }
    }

    // End of data found
    if (buffer[x] == 0xFF) {
      continue;
    }

    if (rack == 0) return RACK;
    else if (tack == 0) return TACK;
    else return GARBAGE;
  }
  return GARBAGE;
}

void BLEShield::readBLE(uint8_t buffer[]) {
  // Zero the buffer
  for(int x = 0; x < 64; x++) {
    buffer[x] = 0xFF;
  }

  // Make sure we got all the data
  _delay_ms(50);

  // Read all the data
  for(int x = 0; x < 64 && _bleSerial.available(); x++) {
    buffer[x] = _bleSerial.read();
  }  
}

void BLEShield::print(const char *str){
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
  _bleSerial.flush();
  
  _bleSerial.write(0xFF); // Wake up
  _bleSerial.write(0xFF); // Wake up
  _bleSerial.write(0xFF); // Wake up 
  _bleSerial.write(0x02); // Start of Packet
  _bleSerial.write(0x20); // Device
  _bleSerial.write(0xDD); // Command (send)
  _bleSerial.write(dataLength); // Length
  exOR_result = exOR_result ^ dataLength;
  for(int counter=0; counter< dataLength; counter++){
    _bleSerial.write(str[counter]); // Payload
    exOR_result = exOR_result ^ str[counter];
  }
  _bleSerial.write(exOR_result); // XOR Checksum
  
  // Wait for a reply
  _delay_ms(100);
  
  uint8_t buffer[64];
  readBLE(buffer);

  // Handle a data send failure
  if(parse(buffer) != TACK) {
    Serial.println("Error sending data to the BLE module.");
    dumpBuffer(buffer);
    errorFlash();
    return;
  }

  setLED(0,0,0);
}

// Recieve a message from the BLE shield
int BLEShield::readChars(char* inbuffer) {
  uint8_t buffer[64];
  boolean error = false;
  
  setLED(0,0,255);

  // Zero the buffer
  for(int x = 0; x < 64; x++) {
    buffer[x] = 0x00;
    inbuffer[x] = 0x00;
  }

  // Make sure we got all the data
  _delay_ms(50);

  // Read all the data   
  readBLE(buffer);  
  int resp = parse(buffer);
  
  setLED(0, 0, 0);
  
  // Handle recieve errors. TODO: XOR Checksum
  if (resp != RACK) {
    Serial.println("Error receiving data from the BLE module.");
    dumpBuffer(buffer);
    errorFlash();
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

int BLEShield::available() {
  return _bleSerial.available();
}

bool BLEShield::connected() { 
  return digitalRead(CONNECTED_PIN);
}

bool BLEShield::readSwitchA() {    
  return !digitalRead(SWITCH_A);
}

bool BLEShield::readSwitchB() {    
  return !digitalRead(SWITCH_B);
}

