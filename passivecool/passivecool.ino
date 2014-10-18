/*
  Bridge Servo controller
  Context: Arduino, for Yún
  
  Controls 2 servo motors
  
  Prepare your Yun's SD card with an empty folder in the SD root named 
  "arduino" and a subfolder of that named "www". 
  This will ensure that the Yún will create a link 
  to the SD to the "/mnt/sd" path.
 
  When you upload your sketch, these files will be placed in
  the /arduino/www/Servos folder on your SD card.
 
  You can then go to http://arduino.local/sd/Servos
  to see the output of this sketch.  

  created 14 December 2013
  by Jeff Hoefs
  */
  
#include <Bridge.h>
//#include <Console.h>
#include <YunServer.h>
#include <YunClient.h>
#include <Servo.h>
#include "ContinuousServo.h"

#define TIME_FOR_90 6000

#define MODE_AUTO 0
#define MODE_MANUAL 1

YunServer server;
Servo servo1;
ContinuousServo cservo1;

int position = 0;
float temperature = 0.0;
float humidity = 0.0;
int light_level = 0;

int current_mode = MODE_AUTO;


void setup() {
  
  servo1.attach(9);
  cservo1.attach(servo1);
  
  // Wakeup 
  cservo1.counterclockwise();
  delay(100);
  
  cservo1.clockwise();
  delay(100);
  
  cservo1.stop();

  pinMode(13, OUTPUT);

  // begin() is blocking and takes about 2 seconds to complete
  Bridge.begin();
//  Console.begin();

  flash13();
  
  // Listen for incoming connection only from localhost
  // (no one from the external network can connect)
  server.begin();
}

void flash13()
{
  for(int i=0; i<10; i++) 
  {
    delay(100);
    digitalWrite(13, HIGH);
    delay(100);
    digitalWrite(13, LOW);
  }

}

void loop() {
  // Get clients coming from the server
  YunClient client = server.accept();
  
  if (client) {
    process(client);
    client.stop();
  }
  
  delay(50);
}

void process(YunClient client) {
  if (client.available() > 0) {
    String command = client.readStringUntil('/');
    
    if (command.indexOf("state") == 0) {
      stateCommand(client);    
    } else if (command.indexOf("mode") == 0) {
      modeCommand(client);
    } else {
       client.print(F("Error: command "));
       client.print(command);
       client.println(F(" not defined"));
    }
  }
}

// expecting "/open/
int stateCommand(YunClient client) {
  int move = 0;
  if (client.peek() >= 0) {
      move = client.parseInt();
      
      if (move > 90) {
        cservo1.counterclockwise();
        delay(TIME_FOR_90); // todo move based on percentage of degrees
        cservo1.stop();
      } else if (move < 90) {
        cservo1.clockwise();
        delay(TIME_FOR_90);
        cservo1.stop();
      } else {
        // TOOD: center?
      }
      
  } else {
    client.print(F("{\"position\":"));
    client.print(position); 
    client.print(F(", "));
    client.print(F("\"temperature\":"));
    client.print(temperature); 
    client.print(F(", "));
    client.print(F("\"humidity\":"));
    client.print(humidity); 
    client.print(F(", "));
    client.print(F("\"light_level\":"));
    client.print(light_level); 
    client.println(F("}"));
  }
}

int modeCommand(YunClient client) {
    if (client.peek() < 0) { 
      client.print(F("{\"mode\":"));
      client.print(current_mode == MODE_AUTO ? F("auto") : F("manual"));
      return 0;
    } 

    String mode = client.readString();

    if (mode.indexOf(F("auto")) >= 0) {
      current_mode = MODE_AUTO;
      client.println(F("ok"));
    } else if (mode.indexOf(F("manual")) >= 0) {
      current_mode = MODE_MANUAL;
      client.println(F("ok"));
    } else {
      client.print(F("{error: \"unknown mode "));
      client.print(mode);
      client.println(F("\"}"));
    }

    return 1;
}



