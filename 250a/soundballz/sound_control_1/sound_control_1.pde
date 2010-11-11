#include <SoftwareSerial.h>

//
// soundgin_predefined
//
// cycles through all of the predefined sounds on the soundgin
 
SoftwareSerial mySerial = SoftwareSerial(2, 3);

void playDefault(int preDefined)
{
 mySerial.print(27, BYTE);
 mySerial.print(75, BYTE);
 mySerial.print(preDefined, BYTE);
}

void setup() {
 mySerial.begin(9600);
 pinMode(13, OUTPUT);

 pinMode(2, INPUT);
 pinMode(3, OUTPUT);

 delay(1000);
}

void loop()  {
 for (int i=0; i<=30; i++)   
 {     
     digitalWrite(13, HIGH);
     delay(100);
     digitalWrite(13, LOW);
     
     playDefault(i);
     delay(1000);
 } 
}
