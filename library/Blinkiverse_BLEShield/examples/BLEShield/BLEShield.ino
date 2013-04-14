#include <SoftwareSerial.h>
#include <BLEShield.h>

uint8_t count = 0;
long lastNotifyTime = millis();

BLEShield bluetooth;

void setup() { 
  
  Serial.begin(115200);
  
  // Standby - Red LED
  bluetooth.setLED(20,0,0);

  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

}

void loop() {

  // While we are connected, send a test sequence every second
  if(bluetooth.connected() && (millis() - lastNotifyTime > 1000)) {
    
    // Make the LED white
    bluetooth.setLED(20,20,20);
  
    // Format the message
    char message[20];
    snprintf(message, 20, "Connected %is", count);
    
    // Send the message to the bluetooth module and the serial port
    bluetooth.print(message);
    Serial.println(message);
    
    // Increment the count, turn off the LED, reset the timer
    count++;
    bluetooth.setLED(0,0,0);
    lastNotifyTime = millis();

  }

  // Check to see if we've received any data from the BLE shield
  if (bluetooth.available() > 0) {
    
    // Create a buffer to hold the received data
    char buffer[64];
    
    // Read the data
    bluetooth.readChars(buffer);
    
    // Print the data to the serial port
    Serial.print("Message recieved: ");
    Serial.println(buffer);
  }
  
  // Check to see if the switches are pressed
  if(bluetooth.readSwitchA()) {
    bluetooth.print("Button A!");
  }
  else if(bluetooth.readSwitchB()) {
    bluetooth.print("Button B!");
  }
  else if(bluetooth.connected()) {
    // Standby - Red LED
    bluetooth.setLED(127,0,0); 
  }
  else {
    // Not connected - LED off
    bluetooth.setLED(0,0,0); 
  }
}
