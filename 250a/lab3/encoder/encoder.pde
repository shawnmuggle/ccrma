// Thank you Karl
// This was taken from http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1185064568

// Connect pin 2 of the Arduino to Phase A of the schematic shown on
// http://cm-wiki.stanford.edu/wiki/Sensors#Quadrature_Encoders
// Connect pin 3 of the Arduino to Phase B, and you're ready to send out the position
// of the encoder over the serial bus.  Of course, the encoder always starts at 0
// when the program on the arduino is restarted.
//
// You can read the serial values with serial monitor.


int encoder0PinA = 2;
int encoder0PinB = 3;
volatile int encoder0Pos = 0;
volatile int encoder0PinALast = LOW;
volatile int n = LOW;
int valNew = 0;
int valOld = 0;
volatile int m = LOW;

void setup()
{
  pinMode (encoder0PinA,INPUT); 
  pinMode (encoder0PinB,INPUT);
  Serial.begin (9600);
  attachInterrupt(1, CountA, CHANGE);
  attachInterrupt(0, StateB, FALLING);
}

void loop()
{
  encoder0PinALast = n;
  valNew = encoder0Pos;
  if (valNew != valOld) {
    Serial.write(encoder0Pos);
    // Consider using Serial.print(encoder0Pos, DEC) instead
    valOld = valNew;
  }

}

void CountA()
{
  n = digitalRead(encoder0PinA); 
  if ((encoder0PinALast == LOW) && (n == HIGH)) { 
    if (m == LOW) { 
      encoder0Pos--; 
    } 
    else { 
      encoder0Pos++; 
    } 
  }
}
void StateB()
{
  m = digitalRead(encoder0PinB);
}
