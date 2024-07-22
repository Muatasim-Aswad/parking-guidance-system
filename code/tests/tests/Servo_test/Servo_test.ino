/*

*/
#include <Servo.h>
Servo myservo;
int pos = 0;

void setup() {
   myservo.attach(9);
   myservo.write(0);
}

void loop() {
  OpenEntrance();
  delay(10000);
  CloseEntrance();
  delay(10000);
    
}

void OpenEntrance ()
{
  for (pos=0; pos <= 90; pos += 1) { 
    // in steps of 1 degree
    myservo.write(pos);              
    delay(20);                       
  }
}

void CloseEntrance ()
{   
  for (pos=90; pos >= 0; pos -= 1) { 
    myservo.write(pos);              
    delay(15);                       
  }
}
