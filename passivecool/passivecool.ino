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
  
  modified by Peter Schwarz and Baskin Tapkan
   IoT Hack Day MN - October 2014  
   */
  
#include <Bridge.h>
//#include <Console.h>
#include <YunServer.h>
#include <YunClient.h>
#include <Servo.h>
#include <DHT.h>
#include "ContinuousServo.h"

#define TIME_FOR_90 6144

#define SERVO_PIN 9

#define DHTPIN 2

#define DHTTYPE DHT22

// initialize DHT sensor for normal 16 MHz Arduino
DHT dht(DHTPIN, DHTTYPE);

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
  
  servo1.attach(SERVO_PIN);
  cservo1.attach(servo1);

  pinMode(13, OUTPUT);

  // begin() is blocking and takes about 2 seconds to complete
  Bridge.begin();
//  Console.begin();

  flash13();
  
  dht.begin();
  
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

void updateSensorReadings() {

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float updated_humidity;
  updated_humidity = dht.readHumidity();

  if(!isnan(updated_humidity) && updated_humidity != NAN) {
    humidity = updated_humidity;
  }

  // Read temperature as Fahrenheit
  temperature = dht.readTemperature(true);
  
  int light_sensor1, light_sensor2;

  light_sensor1 = analogRead(0);
  light_sensor2 = analogRead(1);

  light_level = (light_sensor1 + light_sensor2) / 2;  
}

void calculatePosition()
{
    int new_position = map(light_level, 0, 1023, 0, 90);

    if (abs(new_position - position) > 4) {
      updatePosition(new_position);
    }
}

void updatePosition(int new_position) {
  int time_to_move;

  if (new_position == position) {
      return;
  }
  
  if (new_position < position) {
    time_to_move = TIME_FOR_90 * ((position - new_position) / 90.0);
    openBlind(time_to_move);
  } else {
    time_to_move = TIME_FOR_90 * ((new_position - position) / 90.0);
    closeBlind(time_to_move);
  }
  position = new_position;
}

void loop() {

  updateSensorReadings();

  if (current_mode == MODE_AUTO) {
    calculatePosition();
  }

  // Get clients coming from the server
  YunClient client = server.accept();
  
  if (client) {
    process(client);
    client.stop();
  }
  
  delay(1000);

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

int closeBlind(int move_time) {
  cservo1.clockwise();
  delay(move_time); // todo move based on percentage of degrees
  cservo1.stop();
}

int openBlind(int move_time) {
  cservo1.counterclockwise();
  delay(move_time);
  cservo1.stop();
}

// expecting "/open/
int stateCommand(YunClient client) {
  int move = 0;
  if (client.peek() >= 0) {
    move = constrain(client.parseInt(), 0, 90);
    updatePosition(move);
  } else {
    sendState(client);
  }
}

int modeCommand(YunClient client) {
    if (client.peek() < 0) { 
      sendMode(client);

      return 0;
    } 

    String mode = client.readStringUntil('/');

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

void sendState(YunClient client) {
  client.print(F("{\"position\":"));
  client.print(position + 90); 
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


void sendMode(YunClient client) {
  client.print(F("{\"mode\": \""));
  client.print(current_mode == MODE_AUTO ? F("auto") : F("manual"));
  client.println(F("\"}"));
}
