/*
  AnalogReadSerial
 Reads an analog input on pin 0, prints the result to the serial monitor 
 
 This example code is in the public domain.
 */
 
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
volatile byte encoder0Pos = 0;
volatile int encoder0PinALast = LOW;
volatile int n = LOW;
int valNew = 0;
int valOld = 0;
volatile int m = LOW;

int keyPin1 = 4;
int keyPin2 = 5;
int keyPin3 = 6;
int keyPin4 = 7;
int keyPin5 = 8;
int keyPin6 = 9;

volatile byte keyValue1;
volatile byte keyValue2;
volatile byte keyValue3;
volatile byte keyValue4;
volatile byte keyValue5;
volatile byte keyValue6;


volatile byte sensorValue1 = 0;
volatile byte sensorValue2 = 0;
volatile byte sensorValue3 = 0;


void setup()
{
  pinMode (encoder0PinA,INPUT); 
  pinMode (encoder0PinB,INPUT);

  pinMode (keyPin1,INPUT);
  pinMode (keyPin2,INPUT);
  pinMode (keyPin3,INPUT);
  pinMode (keyPin4,INPUT);
  pinMode (keyPin5,INPUT);
  pinMode (keyPin6,INPUT);

  Serial.begin (9600);
  attachInterrupt(1, CountA, CHANGE);
  attachInterrupt(0, StateB, FALLING);
}

void loop()
{
  encoder0PinALast = n;
  valNew = encoder0Pos;
  if (valNew != valOld) {
    // Consider using Serial.print(encoder0Pos, DEC) instead
    valOld = valNew;
  }
  sensorValue1 = analogRead(A0);
  sensorValue2 = analogRead(A1);
  sensorValue3 = analogRead(A2);
  
  keyValue1 = digitalRead(keyPin1);
  keyValue2 = digitalRead(keyPin2);
  keyValue3 = digitalRead(keyPin3);
  keyValue4 = digitalRead(keyPin4);
  keyValue5 = digitalRead(keyPin5);
  keyValue6 = digitalRead(keyPin6);
  
  Serial.print(sensorValue1);
  Serial.print(sensorValue2);
  Serial.print(sensorValue3);
  
  Serial.print(encoder0Pos);

  Serial.print(keyValue1);
  Serial.print(keyValue2);
  Serial.print(keyValue3);
  Serial.print(keyValue4);
  Serial.print(keyValue5);
  Serial.print(keyValue6);

  Serial.println("");
  //delay(10);
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
