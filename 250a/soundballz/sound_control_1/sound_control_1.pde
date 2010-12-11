#include <SoftwareSerial.h>

//
// soundgin_predefined
//
// cycles through all of the predefined sounds on the soundgin
 
SoftwareSerial mySerial = SoftwareSerial(2, 3);

volatile int accel_x = 0, prev_accel_x = 0, delta_accel_x = 0;
volatile int accel_y = 0, prev_accel_y = 0, delta_accel_y = 0;
volatile int accel_z = 0, prev_accel_z = 0, delta_accel_z = 0;
volatile unsigned int delta_accel_mag = 0;
volatile unsigned int accel_mag = 0;

const int num_delta_accel_mags = 10;
volatile int delta_accel_mags[num_delta_accel_mags];
volatile int delta_accel_mags_index = 0;
volatile unsigned int delta_accel_mags_sum;

const int accel_baseline = 350;
volatile int dev_x, dev_y, dev_z;
volatile unsigned int deviation_from_freefall;


void playDefault(int preDefined)
{
 mySerial.print(27, BYTE);
 mySerial.print(75, BYTE);
 mySerial.print(preDefined, BYTE);
}

void resetSoundgin()
{
  mySerial.print(27, BYTE);
  mySerial.print(70, BYTE);
}

void setUpSoundgin()
{
  setAmplitude(127);
  
  // mixer a amplitude to 127 
  mySerial.print(27, BYTE);
  mySerial.print(1, BYTE);
  mySerial.print(8, BYTE);
  mySerial.print(127, BYTE);
  
  // chip amplitude to 127 
  mySerial.print(27, BYTE);
  mySerial.print(1, BYTE);
  mySerial.print(136, BYTE);
  mySerial.print(127, BYTE);

  // mix control, mix osc 1
  mySerial.print(27, BYTE);
  mySerial.print(1, BYTE);
  mySerial.print(0, BYTE);
  mySerial.print(1, BYTE);

  // osc a1 control, turn on
  mySerial.print(27, BYTE);
  mySerial.print(1, BYTE);
  mySerial.print(16, BYTE);
  mySerial.print(132, BYTE); // 10000100 = on, pulse

/*
  // osc a1 freq fine
  mySerial.print(27, BYTE);
  mySerial.print(1, BYTE);
  mySerial.print(17, BYTE);
  mySerial.print(126, BYTE);
 
  // osc a1 freq low
  mySerial.print(27, BYTE);
  mySerial.print(1, BYTE);
  mySerial.print(18, BYTE);
  mySerial.print(53, BYTE);
  
  // osc a1 freq high
  mySerial.print(27, BYTE);
  mySerial.print(1, BYTE);
  mySerial.print(19, BYTE);
  mySerial.print(7, BYTE);
*/ 
  
  // set up osc a1 envelope attack
  mySerial.print(27, BYTE);
  mySerial.print(1, BYTE);
  mySerial.print(29, BYTE);
  mySerial.print(240, BYTE); // 11110000 (loudest, fastest)
  
  // set up osc a1 envelope decay
  mySerial.print(27, BYTE);
  mySerial.print(1, BYTE);
  mySerial.print(30, BYTE);
  mySerial.print(9, BYTE); // 00001001 (half loudness, 750ms)
  
  // set up osc a1 envelope release
  mySerial.print(27, BYTE);
  mySerial.print(1, BYTE);
  mySerial.print(31, BYTE);
  mySerial.print(8, BYTE); // 00001000 (zero loudness, 300ms)
}

void playSomething()
{
  /*
  // load note into osc a1
  mySerial.print(27, BYTE);
  mySerial.print(72, BYTE);
  mySerial.print(60, BYTE);

  // play mixer a
  mySerial.print(27, BYTE);
  mySerial.print(80, BYTE);
  */

  // load note into osc a1 and play
  mySerial.print(27, BYTE);
  mySerial.print(88, BYTE);
  mySerial.print(70, BYTE);

/*  
 // load frequency
 mySerial.print(27, BYTE);
 mySerial.print(92, BYTE);
 mySerial.print(255, BYTE);
 mySerial.print(0, BYTE);
*/
}

// amp should be between 0 and 15
void playNote(int pitch, int amp)
{
  //setAmplitude(amp);
  // set up osc a1 envelope attack
  mySerial.print(27, BYTE);
  mySerial.print(1, BYTE);
  mySerial.print(29, BYTE);
  mySerial.print(amp << 4, BYTE); // 11110000 (loudest, fastest)
  
  // load note into osc a1 and play
  mySerial.print(27, BYTE);
  mySerial.print(88, BYTE);
  mySerial.print(pitch, BYTE);
}

void setAmplitude(byte amp)
{
  // osc a1 amplitude to 127
  mySerial.print(27, BYTE);
  mySerial.print(1, BYTE);
  mySerial.print(24, BYTE);
  mySerial.print(amp, BYTE); 
}

void setup() {
  for (int i = 0; i < num_delta_accel_mags; i++) {
    delta_accel_mags[i] = 0;
  }
  
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);

  //Serial.begin(9600);

  mySerial.begin(9600);
  pinMode(13, OUTPUT);

  pinMode(2, INPUT);
  pinMode(3, OUTPUT);

  delay(1000);
 
  resetSoundgin();
  delay(1000);
  setUpSoundgin();
  delay(1000);
}

void loop()  {
/*
  playNote(92, 15);
  delay(1000);
  playNote(91, 14);
  delay(750);
  playNote(89, 13);
  delay(550);
  playNote(87, 12);
  delay(400);
  playNote(85, 11);
  delay(300);
  playNote(84, 10);
  delay(250);
  playNote(82, 9);
  delay(225);
  playNote(80, 8);
  delay(212);
  playNote(79, 7);
  delay(200);
  playNote(79, 7);
  delay(175);
  playNote(77, 6);
  delay(150);
  playNote(75, 5);
  delay(125);
  playNote(75, 5);
  delay(100);
  playNote(73, 4);
  delay(75);
  playNote(73, 4);
  delay(50);
  playNote(72, 3);
  delay(50);
  playNote(70, 2);
  delay(50);
  playNote(70, 1);
  delay(1000);
  */
  
  prev_accel_x = accel_x;
  prev_accel_y = accel_y;
  prev_accel_z = accel_z;
  
  accel_x = analogRead(A0);
  accel_y = analogRead(A1);
  accel_z = analogRead(A2);
/*
  // FREEFALL DETECTION
  dev_x = accel_x - accel_baseline;
  dev_y = accel_y - accel_baseline;
  dev_z = accel_z - accel_baseline;
  
  deviation_from_freefall = dev_x * dev_x + dev_y * dev_y + dev_z * dev_z;
  if (deviation_from_freefall < 400) {
    playSomething();
  }
  Serial.print("dev: ");
  Serial.println(deviation_from_freefall);
*/

  // JERK DETECTION
  delta_accel_x = accel_x - prev_accel_x;
  delta_accel_y = accel_y - prev_accel_y;
  delta_accel_z = accel_z - prev_accel_z;
  
  delta_accel_mag = delta_accel_x * delta_accel_x + delta_accel_y * delta_accel_y + delta_accel_z * delta_accel_z;

  
  if (delta_accel_mag > 10000) {
    playSomething();
  }
/*
  delta_accel_mags_sum -= delta_accel_mags[delta_accel_mags_index];
  delta_accel_mags[delta_accel_mags_index] = delta_accel_mag;
  delta_accel_mags_sum += delta_accel_mags[delta_accel_mags_index];
  
  delta_accel_mags_index++;
  delta_accel_mags_index %= num_delta_accel_mags;

  accel_mag = accel_x * accel_x + accel_y * accel_y + accel_z * accel_z;

  byte amp = 127 * (delta_accel_mags_sum / 400000.0);
  //setAmplitude(amp);
  
  //Serial.print("delta accel mag sum: ");
  //Serial.println(delta_accel_mags_sum);
  //delay(100);
  */
  /*
 for (int i=0; i<=30; i++)   
 {     
     digitalWrite(13, HIGH);
     delay(100);
     digitalWrite(13, LOW);
     
     playDefault(i);
     delay(1000);
 }
 */
}
