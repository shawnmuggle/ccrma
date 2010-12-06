#include <SoftwareSerial.h>

SoftwareSerial mySerial = SoftwareSerial(2, 3);

volatile int accel_x = 0, prev_accel_x = 0, delta_accel_x = 0;
volatile int accel_y = 0, prev_accel_y = 0, delta_accel_y = 0;
volatile int accel_z = 0, prev_accel_z = 0, delta_accel_z = 0;
volatile unsigned int delta_accel_mag = 0;

/*
const int delta_accel_mags = 15;
int dev_mags[num_dev_mags];
int dev_mags_index = 0;
int dev_mags_sum = 0;
*/

int accel_baseline = 350;
int dev_x, dev_y, dev_z;
int deviation_from_freefall;

int num_freefall_ticks = 0;

const int num_dev_mags = 15;
int dev_mags[num_dev_mags];
int dev_mags_index = 0;
int dev_mags_sum = 0;

// TODO: optimize continuous tones (freefall & force applied), because i think that the arduino is losing responsiveness by doing so much softwareserial traffic
// TODO: tweak parameters for red ball in a separate program/array at top

// TO MAYBE DO: auto calibration for sensors


// NOTE: states' entrance conditions should be mutually exclusive. Make sure they are.

// STATES (fake enum)
const int FREEFALL = 0;
const int CATCH = 1;
const int FORCE_APPLIED = 2;
const int AT_REST = 3;

int current_state = AT_REST;
unsigned long ticks_in_state = 0;

int ticks_spent_maybe_in_freefall = 0;
const int ticks_required_before_freefall_state = 10;

int non_force_applied_ticks_during_force_applied = 0;
const int non_force_applied_ticks_allowed_in_force_applied_state = 100;

int note_played_counter = 0;

boolean checkIfInFreefallState(int deviation_from_freefall, int jerk)
{
  if (deviation_from_freefall < 120) {
    //Serial.print("ticks so far in freefall: ");
    //Serial.println(ticks_spent_maybe_in_freefall);
    ticks_spent_maybe_in_freefall = ticks_spent_maybe_in_freefall + 1;
    if (ticks_spent_maybe_in_freefall > ticks_required_before_freefall_state) {
      //Serial.println("RETURNING TRUE RE: ARE WE IN FREEFALL?");
      return true;
    }
  } else {
    ticks_spent_maybe_in_freefall = 0;
    return false;
  }
  
  return false;
}

void enterFreefallState()
{
  //Serial.println("ENTERING FREEFALL STATE");;

  note_played_counter = 0;

  current_state = FREEFALL;
  ticks_in_state = 0;
}

void stayInFreefallState()
{
  ++ticks_in_state;
  
  if (ticks_in_state % 100 == 0) {
    playNote(0, 65 + note_played_counter++);
  }
}

void leaveFreefallState()
{
}

boolean checkIfInCatchState(int deviation_from_freefall, int jerk)
{
/*
  if (current_state == FREEFALL) {
    Serial.print("CATCHING WITH JERK: ");
    Serial.println(jerk);
  }
*/
  if (jerk > 100 && current_state == FREEFALL) {
    return true;
  } else {
    return false;
  }
}

void enterCatchState()
{
  //Serial.println("ENTERING CATCH STATE");
  
  current_state = CATCH;
  ticks_in_state = 0;
  
  //playNote(0, 50);
  playNote(2, 100);
  delay(750);
}

void stayInCatchState()
{
  ++ticks_in_state;
}

void leaveCatchState()
{
}

boolean checkIfInForceAppliedState(int deviation_from_freefall, int jerk)
{
  if (deviation_from_freefall > 280) {
    non_force_applied_ticks_during_force_applied = 0;
    return true;
  } else {
    if (current_state == FORCE_APPLIED && non_force_applied_ticks_during_force_applied < non_force_applied_ticks_allowed_in_force_applied_state) {
      non_force_applied_ticks_during_force_applied += 1;
      //Serial.print("Non-forceful ticks during force_applied state: ");
      //Serial.println(non_force_applied_ticks_during_force_applied);
      return true;
    }
    
    return false;
  }
  
  return false;
}

void enterForceAppliedState()
{
  //Serial.println("ENTERING FORCE_APPLIED STATE");
  
  current_state = FORCE_APPLIED;
  ticks_in_state = 0;
  
  non_force_applied_ticks_during_force_applied = 0; // currently unused
}

void stayInForceAppliedState()
{
  ++ticks_in_state;
  if (ticks_in_state % 100 == 0) {
    playNote(0, 40);
  }
}

void leaveForceAppliedState()
{
}

boolean checkIfInAtRestState(int deviation_from_freefall, int jerk)
{
  if (deviation_from_freefall <= 280 && deviation_from_freefall >= 110) {
    return true;
  } else {
    return false;
  }
}

void enterAtRestState()
{
  //Serial.println("ENTERING AT_REST STATE");
  
  current_state = AT_REST;
  ticks_in_state = 0;
}

void stayInAtRestState()
{
  ++ticks_in_state;
}

void leaveAtRestState()
{
}

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
  // chip amplitude to 127 
  mySerial.print(27, BYTE);
  mySerial.print(1, BYTE);
  mySerial.print(136, BYTE);
  mySerial.print(127, BYTE);
  
  // mixer a amplitude to 127 
  mySerial.print(27, BYTE);
  mySerial.print(1, BYTE);
  mySerial.print(8, BYTE);
  mySerial.print(127, BYTE);

  // mix control, mix osc 1
  mySerial.print(27, BYTE);
  mySerial.print(1, BYTE);
  mySerial.print(0, BYTE);
  mySerial.print(7, BYTE); // 00000111 = all 3 oscillators on

  // osc a1 control, turn on
  mySerial.print(27, BYTE);
  mySerial.print(1, BYTE);
  mySerial.print(16, BYTE);
  mySerial.print(132, BYTE); // 10000100 = on, pulse

  // osc a2 control, turn on
  mySerial.print(27, BYTE);
  mySerial.print(1, BYTE);
  mySerial.print(32, BYTE);
  mySerial.print(128, BYTE); // 10000000 = on, sine

  // osc a3 control, turn on
  mySerial.print(27, BYTE);
  mySerial.print(1, BYTE);
  mySerial.print(48, BYTE);
  mySerial.print(132, BYTE); // 10000100 = on, pulse

  // osc a1 amplitude to 127
  mySerial.print(27, BYTE);
  mySerial.print(1, BYTE);
  mySerial.print(24, BYTE);
  mySerial.print(127, BYTE); 

  // osc a2 amplitude to 127
  mySerial.print(27, BYTE);
  mySerial.print(1, BYTE);
  mySerial.print(40, BYTE);
  mySerial.print(127, BYTE); 

  // osc a3 amplitude to 127
  mySerial.print(27, BYTE);
  mySerial.print(1, BYTE);
  mySerial.print(56, BYTE);
  mySerial.print(127, BYTE); 

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
  mySerial.print(9, BYTE); // 00001001 (zero loudness, 750ms)
  
  // set up osc a1 envelope release
  mySerial.print(27, BYTE);
  mySerial.print(1, BYTE);
  mySerial.print(31, BYTE);
  mySerial.print(8, BYTE); // 00001000 (zero loudness, 300ms)
  
  
  // set up osc a3 envelope attack
  mySerial.print(27, BYTE);
  mySerial.print(1, BYTE);
  mySerial.print(61, BYTE);
  mySerial.print(240, BYTE); // 11110000 (loudest, fastest)  

  // set up osc a3 envelope decay
  mySerial.print(27, BYTE);
  mySerial.print(1, BYTE);
  mySerial.print(62, BYTE);
  mySerial.print(9, BYTE); // 00001001 (zero loudness, 750ms)
}

void playSomething()
{
  // load note into osc a1 and play
  mySerial.print(27, BYTE);
  mySerial.print(88, BYTE);
  mySerial.print(70, BYTE);
}

/*
// amp should be between 0 and 15
void playNote(int osc, int pitch, int amp)
{
  int attack_amp_reg[3] = {29, 45, 61};
  int note_reg[3] = {88, 89, 90};

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
*/

void playNote(int osc, int pitch)
{
  int note_reg[3] = {88, 89, 90};
  
  // load note into osc a1 and play
  mySerial.print(27, BYTE);
  mySerial.print(note_reg[osc], BYTE);
  mySerial.print(pitch, BYTE);
}


void setup() {
  
  for (int i = 0; i < num_dev_mags; i++) {
    dev_mags[i] = 0; 
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
  
  int num_readings = 0;
  unsigned long x_sum = 0, y_sum = 0, z_sum = 0;
  unsigned long now = millis();
  unsigned long future = now + 10 * 1000;
  while(millis() < future) {
    x_sum += analogRead(A0);
    y_sum += analogRead(A1);
    z_sum += analogRead(A2);
    num_readings++;
  }
  int x_average = x_sum / num_readings;
  int y_average = y_sum / num_readings;
  int z_average = z_sum / num_readings;
  accel_baseline = (x_average + y_average + z_average) / 3;
  //Serial.print("ACCELEROMETER BASELINE GUESS: ");
  //Serial.println(accel_baseline);
  playNote(0, 60);
  delay(100);
  playNote(0, 72);
  delay(100);
  playNote(0, 84);
  delay(100);
}

void loop()  {
  prev_accel_x = accel_x;
  prev_accel_y = accel_y;
  prev_accel_z = accel_z;
  
  accel_x = analogRead(A0);
  accel_y = analogRead(A1);
  accel_z = analogRead(A2);
  
  /*
  Serial.print("accel_x: ");
  Serial.println(accel_x);
  Serial.print("accel_y: ");
  Serial.println(accel_y);
  Serial.print("accel_z: ");
  Serial.println(accel_z);
  Serial.println("-----");
  */

  // JERK DETECTION
  delta_accel_x = accel_x - prev_accel_x;
  delta_accel_y = accel_y - prev_accel_y;
  delta_accel_z = accel_z - prev_accel_z;
  
  //delta_accel_mag = delta_accel_x * delta_accel_x + delta_accel_y * delta_accel_y + delta_accel_z * delta_accel_z;
  // FAKIN IT with l1 norms y'all
  delta_accel_mag = abs(delta_accel_x) + abs(delta_accel_y) + abs(delta_accel_z);
    
  //Serial.print("delta accel mag: " );
  //Serial.println(delta_accel_mag);
  
  // average DEVIATION FROM FREEFALL over past n ticks
  dev_x = accel_x - accel_baseline;
  dev_y = accel_y - accel_baseline;
  dev_z = accel_z - accel_baseline;
  
  //deviation_from_freefall = sqrt(dev_x * dev_x + dev_y * dev_y + dev_z * dev_z);
  // FAKIN IT with l1 norms y'all
  deviation_from_freefall = abs(dev_x) + abs(dev_y) + abs(dev_z);
  
  dev_mags[dev_mags_index] = deviation_from_freefall;

  dev_mags_index++;
  dev_mags_index %= num_dev_mags;
  
  //Serial.print("DEV from freefall: ");
  //Serial.println(deviation_from_freefall);

  dev_mags_sum = 0;
  for (int i = 0; i < num_dev_mags; i++) {
    dev_mags_sum = dev_mags_sum + dev_mags[i];
  }


  //Serial.print("Dev mags avg: ");
  //Serial.println(dev_mags_sum / num_dev_mags);
  //delay(100);

  boolean enter_catch = checkIfInCatchState(deviation_from_freefall /*dev_mags_sum / num_dev_mags*/, delta_accel_mag);
  boolean enter_freefall = checkIfInFreefallState(deviation_from_freefall /*dev_mags_sum / num_dev_mags*/, delta_accel_mag);
  boolean enter_force_applied = checkIfInForceAppliedState(deviation_from_freefall /*dev_mags_sum / num_dev_mags*/, delta_accel_mag);
  boolean enter_at_rest = checkIfInAtRestState(deviation_from_freefall /*dev_mags_sum / num_dev_mags*/, delta_accel_mag);
  
  // TODO: Figure out how to call "leave state" for the current state here
  
/*
  if (enter_catch == true) {
    Serial.println("enter catch");
  }
  if (enter_freefall == true) {
    Serial.println("enter freefall");
  }
  if (enter_force_applied == true) {
    Serial.println("enter force_applied");
  }
  if (enter_at_rest == true) {
    Serial.println("enter at_rest");
  }
  
  Serial.println("------");
*/

  
  if (enter_catch && current_state != CATCH) {
    enterCatchState();
  }
  else if (enter_freefall && current_state != FREEFALL) {
    enterFreefallState();
  }
  else if (enter_force_applied && current_state != FORCE_APPLIED) {
    enterForceAppliedState();
  }
  else if (enter_at_rest && !enter_force_applied && current_state != AT_REST) {
    enterAtRestState();
  }

  switch (current_state) {
    case CATCH:
      stayInCatchState();
      break;
    case FREEFALL:
      stayInFreefallState();
      break;
    case FORCE_APPLIED:
      stayInForceAppliedState();
      break;
    case AT_REST:
      stayInAtRestState();
      break;
    default:
      break;
  }

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
