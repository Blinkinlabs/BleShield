#include <SoftwareSerial.h>

#define LED_COUNT 32

#define LED_MOSI_PIN PORTB5
#define LED_SCK_PIN PORTB3

char ble_ACK[6];
char sendDATA(char *str);

SoftwareSerial mySerial(3, 2); // RX, TX

#define LED_GREEN 9
#define LED_RED 6
#define LED_BLUE 5

// For BLE Shield - Clock is PB3, MOSI is PB5
//void send_single_byte(uint8_t c)
//{
//  for(uint8_t i = 0; i < 8; i++) {
//    PORTB = (((c >> (7 - i)) & 0x01) << LED_MOSI_PIN);
//    PORTB = (((c >> (7 - i)) & 0x01) << LED_MOSI_PIN) | _BV(LED_SCK_PIN);
//  }
//} 

void send_single_byte(uint8_t c)
{
  for(uint8_t i = 0; i < 8; i++) {
    // For BLE Shield - Clock is PB3, MOSI is PB5
    if(c >> (7 - i) & 0x01) {
      bitSet(PORTB, LED_MOSI_PIN);
    }
    else {
      bitClear(PORTB, LED_MOSI_PIN);      
    }
    
    bitSet(PORTB, LED_SCK_PIN);
    bitClear(PORTB, LED_SCK_PIN);
  }
}

void send_pixel(uint8_t red, uint8_t green, uint8_t blue) {
  send_single_byte(0x80 | red);
  send_single_byte(0x80 | green);
  send_single_byte(0x80 | blue);
}

void setup() { 
 //Initialize serial and wait for port to open:
  mySerial.begin(9600);
  Serial.begin(9600);
  
  bitSet(DDRB, DDB3);
  bitSet(DDRB, DDB5);
    
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
} 

int j = 0;
int f = 0;
int k = 0;

void color_loop() {
  float brightness = random(100,100)/100.0;
  
  for (uint8_t i = 0; i < LED_COUNT; i++) {
    uint8_t red =   64*(1+sin(i/2.0 + j/4.0       ))*brightness;
    uint8_t green = 64*(1+sin(i/1.0 + f/9.0  + 2.1))*brightness;
    uint8_t blue =  64*(1+sin(i/3.0 + k/14.0 + 4.2))*brightness;
    
    send_pixel(red, green, blue);
  }
  
  j = j + random(1,2);
  f = f + random(1,2);
  k = k + random(1,2);
}

void loop() { 
//  sendDATA("TEST");
//  delay(1000);
//  sendDATA("TEST2");
//  delay(1000);
//  
//  /*for(int i=0; i<6; i++)
//    Serial.print(ble_ACK[i]);
//  delay(4000);*/

   color_loop();
   send_single_byte(0x00);
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
}
