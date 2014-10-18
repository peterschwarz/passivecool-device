#ifndef CONTINUOUS_SERVO_H
#define CONTINUOUS_SERVO_H

#include <inttypes.h>
#include <Servo.h>

class ContinuousServo
{
  
public: 
    ContinuousServo();

    void attach(Servo &s);   

    void stop();

    void counterclockwise();

    void clockwise();           // attach the given pin to the next free channel, sets pinMode, returns channel number or 0 if failure

private:
    Servo *servo;

};

#endif /* CONTINUOUS_SERVO_H */
