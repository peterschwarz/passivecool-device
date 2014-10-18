#include <Console.h>
#include <Servo.h>

int incomingByte;      // a variable to read incoming serial data into
Servo myServo;

void setup() {
  // initialize serial communication:
  Bridge.begin();
  myServo.attach(9);  
  Console.begin(); 

   while (!Console){
    ; // wait for Console port to connect.
  }
  Console.println("Passive Cool - O for open, C for close");
  
}

void loop() {
 
   if (Console.available() > 0) {
    // read the oldest byte in the serial buffer:
    incomingByte = Console.read();
    if (incomingByte == 'O') {
      openBlind();
    } 
   if (incomingByte == 'C') {
      closeBlind();
    }
 
  }
}

void openBlind() {
  myServo.writeMicroseconds(1700);
  delay(3000);
  myServo.writeMicroseconds(1515);
  delay(3000);
}

void closeBlind() {
   myServo.writeMicroseconds(1300);
   delay(3000);
   myServo.writeMicroseconds(1515);
   delay(3000);
}
