// Run a A4998 Stepstick from an Arduino UNO.
// Paul Hurley Aug 2015

/*
 * 670ma rated per coil
 * VREF = max_current / 2 = 0.3V
 * 
 * 
 */


#define REV_STEPS 200.0
#define STEP_MODE 0.5 //.5 for half, 1 for full
#define RAIL_LENGTH 337.0 //mm
#define MM_PER_REV 39.0 //mm
//#define MS_FOR_TRACK 6000 //ms, how long the entire track run should take for continuous mode

//Motor pins
#define PIN_ENABLE 6
#define PIN_STEP  5
#define PIN_DIRECTION 4
#define PIN_STEP_MODE 7

#define PIN_START 8
#define PIN_TRACK_MODE 9
#define PIN_SPEED 10
#define PIN_SHUTTER 13
#define BAUD (9600)

float rev_delay;
float step_delay;
float ms_for_track;
int steps_per_track;
int steps_track_offset = 0;
int step_increment = 1;
boolean button_start_is_pressed = 0;
boolean needs_reset = 0;

void setup() 
{
  Serial.begin(BAUD);
  pinMode(PIN_ENABLE, OUTPUT); // Enable
  pinMode(PIN_STEP, OUTPUT); // Step
  pinMode(PIN_DIRECTION, OUTPUT); // Dir
  pinMode(PIN_STEP_MODE, OUTPUT);
  pinMode(PIN_START, INPUT_PULLUP);
  pinMode(PIN_TRACK_MODE, INPUT_PULLUP);
  pinMode(PIN_SPEED, INPUT_PULLUP);

  digitalWrite(PIN_ENABLE, HIGH); // disable power to motor
  digitalWrite(PIN_DIRECTION, LOW); // Set Dir high
  if (STEP_MODE == 0.5) {
    digitalWrite(PIN_STEP_MODE, HIGH); //Set mode to 1/2 step
  }
  else if (STEP_MODE == 1) {
    digitalWrite(PIN_STEP_MODE, LOW); //Set mode to full step
  }

  steps_per_track = RAIL_LENGTH / MM_PER_REV * REV_STEPS / STEP_MODE;
}

void loop()
{
  //int do_not_start = digitalRead(PIN_START);
  int track_mode = 0;
  int track_speed;
  int num_steps;
  
  track_mode = digitalRead(PIN_TRACK_MODE); //1 for continuous, 0 for timelapse
  track_speed = digitalRead(PIN_SPEED); //1 for slow, 0 for fast
  
  if (start_stop_pressed())
  {
    Serial.println("Start!");
    Serial.println(start_stop_pressed());
    if (!track_mode) //timelapse mode selected
    {
      step_delay = 10;
      rev_delay = 5000;
      timelapse();
    }
  
    if (track_mode) //continuous mode selected
    {
      ms_for_track = track_speed ? 6000.0 : 3000.0;
      num_steps = steps_per_track;
      step_delay = ms_for_track / num_steps;
      rev_delay = 0;
      Serial.print("Step delay: ");
      Serial.println(step_delay);
      continuous(num_steps);
    }

    needs_reset = 1;
    Serial.print("Steps: ");
    Serial.println(steps_track_offset);
  }

  delay(200);
}

boolean start_stop_pressed()
{
  int pressed = !digitalRead(PIN_START);

  if (!pressed) {
    button_start_is_pressed = 0;
    return 0;
  }
  else if (pressed && !button_start_is_pressed) {
    button_start_is_pressed = 1;
    return 1;
  }

  return 0;
}

void continuous(int steps)
{
  Serial.println("Begin"); 
  digitalWrite(PIN_ENABLE, LOW); // enable power to motor

  Serial.print("Revolutions: ");
  Serial.println(RAIL_LENGTH / MM_PER_REV);
  
  for (int revs=0; revs<RAIL_LENGTH / MM_PER_REV * REV_STEPS / STEP_MODE; revs++)
  {
//    for (int x=0; x<REV_STEPS / STEP_MODE; x++)
//    {
      digitalWrite(PIN_STEP, HIGH); // Output high
      delay(1);
      digitalWrite(PIN_STEP, LOW); // Output low
      delay(step_delay);
      steps_track_offset += step_increment;
      if (start_stop_pressed()) {
        digitalWrite(PIN_ENABLE, HIGH); // disable power to motor
        return;
      } 
//    }
  }

  digitalWrite(PIN_ENABLE, HIGH); // disable power to motor
}

void timelapse() 
{
  Serial.println("Begin"); 

  //for (int revs=0; revs<RAIL_LENGTH / MM_PER_REV; revs++)
  for (int revs=0; revs<1; revs++)
  {
    Serial.println("Step for a full revolution");
    digitalWrite(PIN_ENABLE, LOW); // enable power to motor
    
    for (int x=0; x<REV_STEPS / STEP_MODE; x++)
    {
      digitalWrite(PIN_STEP, HIGH); // Output high
      digitalWrite(PIN_STEP, LOW); // Output low
      delay(step_delay);
      steps_track_offset++;
    }
  
    digitalWrite(PIN_ENABLE, HIGH); // disable power to motor
    
    // Trigger the shutter in the middle of the wait
    delay(rev_delay / 2 - 10);
    digitalWrite(PIN_SHUTTER, HIGH); // Output low
    delay(10);
    digitalWrite(PIN_SHUTTER, LOW); // Output low
    Serial.println("Shutter triggered");
    delay(rev_delay / 2);
  }
}
