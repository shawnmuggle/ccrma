// Firmware for haptics lab
// Put together by Edgar Berdahl
// October, 2010
//
//
// For controlling the motor
// PWM (orange) on pin D9, and direction (yellow) on pin D13
int dirPin = 13;
int PWMpin = 9;


// The code for reading the encoder is modified from
// http://www.arduino.cc/playground/Main/RotaryEncoders
//
// --- Fast encoder reading: using just interrupts ---
//
// "I also had to face with the problem of reading encoder
// signals and, after many trials, I'm happy to let you know 
// a new way to deal with them, inspired by all the previous 
// suggestions. I tried it on AMT encoder and it work really 
// good. Unlikely, the other methods failed, the counting rate 
// was too fast. To escape reading Arduino's port programming, 
// I thought it may be even faster just using the interrupt 
// pins."
//
// The encoder phases should be connected to digital pins D2 and D3.
#define encoder0PinA  2
#define encoder0PinB  3

volatile int encoder0Pos = 0;


// The white wire for the force sensing resitor (FSR) used as
// a pressure sensor at the end of the force stick it connected
// to analog input A7
#define FSRpin        7




int inputTorque;

void setup()
{
  //turn on pullup resistors
  digitalWrite(encoder0PinA, HIGH);
  digitalWrite(encoder0PinB, HIGH);
  
  pinMode(encoder0PinA,INPUT); 
  pinMode(encoder0PinB,INPUT);
  //To speed up even more, you may define manually the ISRs
  // encoder A channel on interrupt 0 (arduino's pin 2)
  attachInterrupt(0, doEncoderA, RISING);

  Serial.begin(57600);
  
  pinMode (PWMpin, OUTPUT);
  pinMode (dirPin, OUTPUT);
  
  // These lines may are reported to make delay() and millis() act
  // strangely, although I have not observed this
  //// TCCR1B = TCCR1B & 0b11111000 | 0x02;  // PWM rate 3900Hz
  TCCR1B = TCCR1B & 0b11111000 | 0x01;  // PWM rate 31250Hz
}




void loop()
{
  int tmp, FSRread, torque;
  char newTorque;
  
  
  // Receive the torque (i.e. rotational force) command from Pd
  // via the serial port and convert it into direction and PWM
  // commands.
  newTorque=0;
  while (Serial.available() > 0) {
    torque=Serial.read();
    newTorque=1;
  }
  if (newTorque){
    if (torque > 127)
      torque = torque - 256;   // convert unsigned back to signed
      inputTorque = torque;
    if (torque > 0) {
      digitalWrite(dirPin, HIGH);
    } else {
      digitalWrite(dirPin, LOW);
      torque=torque*-1;
    }
    analogWrite(PWMpin, torque*2);
  }
  
  
  // Now prepare for sending output back to Pd.
  // First send one 0xFF header byte (ASCII 255) for sync
  Serial.write(0xFF);
  
  
  // Send the encoder value using two bytes
  tmp = 0x0F80 & encoder0Pos;
  tmp = tmp >> 7;
  Serial.write((char)tmp);  // First the most significant bits (MSB)
  tmp = 0x007F & encoder0Pos;
  Serial.write((char)tmp);  // then the least significant bits (LSB)
  
  
  FSRread = analogRead(FSRpin);
  tmp = 0x0380 & FSRread;
  tmp = tmp >> 7;
  Serial.write((char)tmp);  // First the most significant bits (MSB)
  tmp = 0x007F & FSRread;
  Serial.write((char)tmp);  // then the least significant bits (LSB)
  
  
  // Wait for 1ms so as not to overrun the serial interface or Pd's event scheduler
  delay(1);
}




// Interrupt routine is called if phase A just hit a rising edge (i.e. went high)
void doEncoderA()    
{
  digitalRead(encoder0PinB) ? encoder0Pos--:  encoder0Pos++;
}
