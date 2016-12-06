/*
 * 670ma rated per coil
 * VREF = max_current / 2 = 0.3V
 *
 *
 */
 #include <LiquidCrystal.h>
 LiquidCrystal lcd(7,8,9,10,11,12);

struct {
  uint32_t timelapseTime;  // Total duration of movement along slider for timelapse mode
  uint32_t continuousTime; // Total duration of movement along slider for continuous mode
  uint32_t LEDflashTime;   // LED blink rate indicates selected speed
} speed[] = {
   5 * 60 * 1000.0, 3000.0,  1000000L / 2, //  5 min tl, 3s c,   2 Hz blink
  10 * 60 * 1000.0, 6000.0,  1000000L,     // 10 min tl, 6s c,   1 Hz blink
  20 * 60 * 1000.0, 10000.0, 1000000L * 2, // 20 min tl, 10s c,  1/2 Hz blink
  60 * 60 * 1000.0, 15000.0, 1000000L * 3  // 60 min tl, 15s c,  1/3 Hz blink
};
#define N_SPEEDS (sizeof(speed) / sizeof(speed[0]))

#define REV_STEPS 200.0
#define STEP_MODE 0.5 //.5 for half, 1 for full
#define RAIL_LENGTH 337.0 //mm
#define MM_PER_REV 39.0 //mm
#define GEAR_RATIO 20.0 //step down ratio of gearing (1:GEAR_RATIO)

//Motor pins
#define PIN_ENABLE 6
#define PIN_STEP  5
#define PIN_DIRECTION 4
#define PIN_STEP_MODE 2

#define PIN_SELECT A0
#define PIN_START A1

#define PIN_TRACK_MODE 9
#define PIN_SHUTTER 3
#define PIN_LED 13
#define BAUD (9600)

//float rev_delay;
//float step_delay;
float ms_for_track;
long steps_per_track;
int step_increment = 1;
int direction_mode = LOW;

// button debouncing - records state
boolean debouncer[] = { false, false };


// Speed selection variables
static uint8_t  led;
uint8_t  speedIdx  = 0;
uint32_t startTime = 0;      // micros() value when slider movement started

int mode = 0;
int captures = 200;
int duration = 60;
int shutter_time = 0;

// Function prototypes
boolean button_pressed(int pin, boolean set_pressed = false);

void setup()
{
  Serial.begin(BAUD);
  pinMode(PIN_ENABLE, OUTPUT); // Enable
  pinMode(PIN_STEP, OUTPUT); // Step
  pinMode(PIN_DIRECTION, OUTPUT); // Dir
  pinMode(PIN_STEP_MODE, OUTPUT);
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_START, INPUT_PULLUP);
  pinMode(PIN_SELECT, INPUT_PULLUP);
  //pinMode(PIN_TRACK_MODE, INPUT_PULLUP);

  // analog pins should set to HIGH for pullup resistors
  digitalWrite(PIN_SELECT, HIGH);
  digitalWrite(PIN_START, HIGH);

  // set initial state
  digitalWrite(PIN_LED, HIGH);       // LED steady on during init
  digitalWrite(PIN_ENABLE, HIGH);    // disable power to motor
  digitalWrite(PIN_DIRECTION, direction_mode);
  if (STEP_MODE == 0.5) {
    digitalWrite(PIN_STEP_MODE, HIGH); //Set mode to 1/2 step
  }
  else if (STEP_MODE == 1) {
    digitalWrite(PIN_STEP_MODE, LOW); //Set mode to full step
  }

  steps_per_track = RAIL_LENGTH / MM_PER_REV * REV_STEPS / STEP_MODE * GEAR_RATIO;

  digitalWrite(PIN_LED, LOW);       // LED off init complete

  lcd.begin(16,2);
  lcd.clear();
}

int menu_step = -1;

void loop()
{
  if (menu_step < 1337) {
    menu();
    return;
  }

  //set direction
  digitalWrite(PIN_DIRECTION, direction_mode);

  if (mode == 0) //timelapse mode selected
  {
    timelapse();
  }
  else if (mode == 1) //continuous mode selected
  {
    continuous();
  }
  else if (mode == 2) //move left/right
  {
    duration = 20;
    continuous();
  }

  menu_step = -1;
}

void menu()
{
  switch(menu_step)
  {
    case -1:
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("TemporalLapse v1");
      lcd.setCursor(0,1);
      lcd.print("Mode? timelapse");
      menu_step = 0;
    case 0:
      if (button_pressed(PIN_SELECT))
      {
        lcd.setCursor(0,1);
        lcd.print("Mode? ");
        if (mode == 0) {
          mode = 1;
          lcd.print("continuous");
        }
        else if (mode == 1) {
          mode = 2;
          lcd.print("move L/R  ");
        }
        else {
          mode = 0;
          lcd.print("timelapse ");
        }
      }
      else if (button_pressed(PIN_START)) {
        menu_step = mode * 100 + 1;
      }
      break;
    case 1: //timelapse mode
      lcd.clear();
      lcd.print("Num Captures?");
      lcd.setCursor(0,1);
      lcd.print(captures);
      menu_step = 2;
    case 2:
      if (button_pressed(PIN_SELECT))
      {
        captures = (captures + 25) % 1000;
        lcd.setCursor(0,1);
        lcd.print(captures);
        lcd.print("   ");
      }
      else if (button_pressed(PIN_START)) {
        menu_step = 3;
        lcd.clear();
        lcd.print("Duration (min)?");
        lcd.setCursor(0,1);
        lcd.print(duration);
      }
      break;
    case 3:
      if (button_pressed(PIN_SELECT))
      {
        duration = (duration + 10) % 300;
        lcd.setCursor(0,1);
        lcd.print(duration);
        lcd.print("   ");
      }
      else if (button_pressed(PIN_START)) {
        menu_step = 4;
        lcd.clear();
        lcd.print("Shutter (sec)?");
        lcd.setCursor(0,1);
        lcd.print(shutter_time);
        if (!shutter_time)
          lcd.print(" (camera def)");
      }
      break;
    case 4:
      if (button_pressed(PIN_SELECT))
      {
        shutter_time = (shutter_time + 1) % 30;
        lcd.setCursor(0,1);
        lcd.print(shutter_time);
        if (!shutter_time)
          lcd.print(" (camera def)");
        lcd.print("              ");
      }
      else if (button_pressed(PIN_START)) {
        menu_step = 201;
      }
      break;

    case 101: //continuous mode
      lcd.clear();
      lcd.print("Duration (sec)?");
      lcd.setCursor(0,1);
      duration = 10;
      lcd.print(duration);
      menu_step = 102;
    case 102:
      if (button_pressed(PIN_SELECT))
      {
        duration = (duration + 2) % 100;
        lcd.setCursor(0,1);
        lcd.print(duration);
        lcd.print("   ");
      }
      else if (button_pressed(PIN_START)) {
        menu_step = 201;
      }
      break;

    case 201: //move left/right
      lcd.clear();
      lcd.print("Direction?");
      lcd.setCursor(0,1);
      lcd.print("forward");
      menu_step = 202;
    case 202:
      if (button_pressed(PIN_SELECT))
      {
        lcd.setCursor(0,1);
        if (direction_mode == LOW)
        {
          direction_mode = HIGH;
          lcd.print("reverse");
        }
        else
        {
          direction_mode = LOW;
          lcd.print("forward");
        }
        lcd.print(" ");
      }
      else if (button_pressed(PIN_START)) {
        menu_step = 1335;
      }
      break;

    case 1335:
      menu_step = 1336;
      lcd.clear();
      lcd.print("Start?");
    case 1336:
      if (button_pressed(PIN_SELECT))
      {
        menu_step = -1;
        lcd.clear();
      }
      else if (button_pressed(PIN_START)) {
        menu_step = 1337;
        lcd.clear();
        lcd.print("Running...");
      }
      break;
  }
}


boolean button_pressed(int pin, boolean set_pressed)
{
  int pressed = !digitalRead(pin) || set_pressed;

  if (!pressed) {
    debouncer[pin % 14] = 0; // % 14 assumes buttons are on analog pins
    return 0;
  }
  else if (pressed && !debouncer[pin % 14]) {
    debouncer[pin % 14] = 1;
    return 1;
  }

  return 0;
}

void timelapse()
{
  unsigned long time_per_capture = (unsigned long)duration * 60UL * 1000UL * 1000UL / (unsigned long)captures;
  
  int steps_per_capture = steps_per_track / captures;
  unsigned long capture_ms = shutter_time * 1000UL;
  unsigned long delay_ms = 0UL;
  if (capture_ms == 0)
  {
    capture_ms = 10UL;
  }

  Serial.print("duration and captures: ");
  Serial.println(duration);
  Serial.println(captures);

  Serial.print("time_per_capture: ");
  Serial.println(time_per_capture);

  Serial.print("steps_per_capture: ");
  Serial.println(steps_per_capture);

  for (int capture=0; capture<captures; capture++)
  {
    startTime = micros();
    //TODO watch out for overflow ~70 min since program begin
    //     this should be handled by subtracting the current time from the start time
    //     as it causes a reverse overflow.

    Serial.print("Begin capture ");
    Serial.println(capture);

    // Trigger the shutter
    digitalWrite(PIN_SHUTTER, HIGH);
    delay(capture_ms);
    digitalWrite(PIN_SHUTTER, LOW);

    delay_ms = ((time_per_capture - (micros() - startTime)) / 1000.0 - steps_per_capture * 11) / 2.0;
    Serial.print("Delay: ");
    Serial.println(delay_ms);
    delay(delay_ms);

    digitalWrite(PIN_ENABLE, LOW); // enable power to motor
    for (int steps=0; steps<steps_per_capture; steps++)
    {
      digitalWrite(PIN_STEP, HIGH);
      delay(1);
      digitalWrite(PIN_STEP, LOW);
      delay(10);
    }
    digitalWrite(PIN_ENABLE, HIGH); // disable power to motor

    //delay(500); //reduce vibration (theory)
    delay_ms = (time_per_capture - (micros() - startTime)) / 1000.0;
    Serial.print("Delay 2: ");
    Serial.print("         ");
    Serial.print(time_per_capture);
    Serial.print(" - ");
    Serial.println(micros() - startTime);
    Serial.println(delay_ms);
    delay(delay_ms);
  }
}

void continuous()
{
  ms_for_track = duration * 1000.0;
  float step_delay = ms_for_track / steps_per_track;
  Serial.print("Revolutions: ");
  Serial.println(RAIL_LENGTH / MM_PER_REV);

  Serial.println("Begin");
  digitalWrite(PIN_ENABLE, LOW); // enable power to motor

  for (int steps=0; steps<steps_per_track; steps++)
  {
      digitalWrite(PIN_STEP, HIGH); // Output high
      delay(1);
      digitalWrite(PIN_STEP, LOW); // Output low
      delay(step_delay);
      if (button_pressed(PIN_START)) {
        digitalWrite(PIN_ENABLE, HIGH); // disable power to motor
        return;
      }
  }

  digitalWrite(PIN_ENABLE, HIGH); // disable power to motor
}
