#include "ContinuousServo.h"

ContinuousServo::ContinuousServo()
{
    
}

void ContinuousServo::attach(Servo &s) 
{
    servo = &s;
}

void ContinuousServo::stop()
{
    servo->writeMicroseconds(1515);
}

void ContinuousServo::counterclockwise()
{
    servo->writeMicroseconds(1700);   
}

void ContinuousServo::clockwise()
{
    servo->writeMicroseconds(1300);   
}
