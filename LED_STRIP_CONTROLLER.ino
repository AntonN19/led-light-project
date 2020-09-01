/**
 * Project Name: Arduino - LED light Strip controller 
 * Name: LED_STRIP_CONTROLLER
 * Description: A bluetooth controlled LED Strip with an integrated PIR Sensor
 * 
 * @author Anton Nikulsin
 * @version: 1.1 7/12/2020
 */
#include <SoftwareSerial.h>


SoftwareSerial bleMod(2,3); //RX, TX
const byte RED = 9;   // pin controlling red leds
const byte GREEN = 5; // pin controlling green leds
const byte BLUE = 6;  // pin controlling blue leds
const byte PIR = 10;  // PIR Sensor
int data[4] = {0,0,0,1}; //int array used to store RGB value sent from smartphone and pir sensor control
                         //initially set with pir sensor on
byte prevState = 0;   //stores previous state of PIR sensor

void setup() {
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);
  bleMod.begin(9600);
  Serial.begin(9600);
  
  sendCommand("AT+ROLE0");              //Set as peripheral
  sendCommand("AT+UUID0xFF00");         //Change UUID to FF00
  sendCommand("AT+CHAR0xFF01");         //Change character UUID to FF01
  sendCommand("AT+NAMELED-Controller"); //Change BLE chip Name to LED-Controller
  
}

/**
 * Sends a AT command to the BLE chip
 * 
 * @param command The command being sent to the BLE chip
 */
void sendCommand(const char * command){
  bleMod.println(command);
  delay(100);
  
  char reply[100];
  int i = 0;
  while (bleMod.available()){
    reply[i] = bleMod.read();
    i++;
  }
  reply[i] = '\0';
  Serial.println(reply);
}

/**
 * Reads message sent to BLE chip from smartphone and store it in a int array
 */
void readMod(){
  int i = 0;
  while(bleMod.available()){
    data[i] = bleMod.read();
    i++;
  }
}

/**
 * 
 */
void usePIR(){
  byte state = digitalRead(PIR);
  if(state == 1 && state != prevState) {
    for(int i = 0; i <= 150; i++){
      analogWrite(RED, i);
      analogWrite(GREEN, i);
      analogWrite(BLUE, i);
      delay(20);
      readMod();                       //check for PIR value and break loop if its not equal to one
      if(data[3] != 1) break;
    }
    for(int i = 0; i <= 1200; i++){
      delay(100);
      readMod();                       //check for PIR value and break loop if its not equal to one
      if(data[3] != 1) break;
    }
  prevState = state;
  } else if(state == 0 && state != prevState){
    for(int i = 150; i >= 0; i--){
      analogWrite(RED, i);
      analogWrite(GREEN, i);
      analogWrite(BLUE, i);
      delay(20);
      readMod();                      //check for PIR value and break loop if its not equal to one
      if(data[3] != 1) break;
    }
    prevState = state;
  }
}

void loop() {
  readMod();
  if(data[3] == 0){
    analogWrite(RED, data[0]);
    analogWrite(GREEN, data[1]);
    analogWrite(BLUE, data[2]);
    delay(100);
  } else if(data[3] == 1){
    while(data[3] == 1){
      usePIR();
    }
  }
}
