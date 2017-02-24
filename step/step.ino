/*
 * 670ma rated per coil
 * VREF = max_current / 2 = 0.3V
 *
 *
 */
#include <Wire.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"

// for oled display
#define I2C_ADDRESS 0x3C

#define REV_STEPS 200.0
#define STEP_MODE 0.5 //.5 for half, 1 for full
#define RAIL_LENGTH 337.0 //mm
#define MM_PER_REV 39.0 //mm
#define GEAR_RATIO 21.5 //step down ratio of gearing (1:GEAR_RATIO)

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

SSD1306AsciiWire oled;

//float rev_delay;
//float step_delay;
float ms_for_track;
long steps_per_track;
int step_increment = 1;
int direction_mode = LOW;

// button debouncing - records state
boolean debouncer[] = { false, false };

// Speed selection variables
uint32_t startTime = 0;      // micros() value when slider movement started

int mode = 0;
int captures = 200;
int duration = 60;
int interval = 5;
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

  Wire.begin();
  oled.begin(&Adafruit128x64, I2C_ADDRESS);
  oled.set400kHz();
  oled.setFont(Adafruit5x7);
}

int menu_step = -1;

void loop()
{
  if (menu_step < 1337) {
    menu();
    return;
  }

  digitalWrite(PIN_LED, LOW);       // LED off init complete

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
      display_title("TemporalLapse v1\nMode?");
      display_setting("timelapse");
      menu_step = 0;
    case 0:
      if (button_pressed(PIN_SELECT))
      {
        if (mode == 0) {
          mode = 1;
          display_setting("continuous");
        }
        else if (mode == 1) {
          mode = 2;
          display_setting("move L/R");
        }
        else {
          mode = 0;
          display_setting("timelapse");
        }
      }
      else if (button_pressed(PIN_START)) {
        menu_step = mode * 100 + 1;
      }
      break;
    case 1: //timelapse mode
      display_title("Num Captures?");
      display_setting(captures);
      menu_step = 2;
    case 2:
      if (button_pressed(PIN_SELECT))
      {
        captures = (captures + 25) % 1000;
        display_setting(captures);
      }
      else if (button_pressed(PIN_START)) {
        menu_step = 3;
        display_title("Interval (s)?");
        display_setting(interval);
      }
      
      break;
    case 3:
      if (button_pressed(PIN_SELECT))
      {
        interval = (interval + 1) % 10;
        display_setting(interval);
      }
      else if (button_pressed(PIN_START)) {
        menu_step = 4;
        display_title("Shutter (sec)?\n  (0 for camera def)");
        display_setting(shutter_time);
      }
      break;
    case 4:
      if (button_pressed(PIN_SELECT))
      {
        shutter_time = (shutter_time + 1) % 30;
        display_setting(shutter_time);
      }
      else if (button_pressed(PIN_START)) {
        menu_step = 201;
      }
      break;

    case 101: //continuous mode
      display_title("Duration (sec)?");
      duration = 10;
      display_setting(duration);
      menu_step = 102;
    case 102:
      if (button_pressed(PIN_SELECT))
      {
        duration = (duration + 2) % 100;
        display_setting(duration);
      }
      else if (button_pressed(PIN_START)) {
        menu_step = 201;
      }
      break;

    case 201: //move left/right
      display_title("Direction?");
      display_setting("forward");
      menu_step = 202;
    case 202:
      if (button_pressed(PIN_SELECT))
      {
        if (direction_mode == LOW)
        {
          direction_mode = HIGH;
          display_setting("reverse");
        }
        else
        {
          direction_mode = LOW;
          display_setting("forward");
        }
      }
      else if (button_pressed(PIN_START)) {
        menu_step = 1335;
      }
      break;

    case 1335:
      menu_step = 1336;
      display_title("Start?");
    case 1336:
      if (button_pressed(PIN_SELECT))
      {
        menu_step = -1;
      }
      else if (button_pressed(PIN_START))
      {
        menu_step = 1337;
        display_title("Running...");
      }
      break;
  }
}

void display_title(char title[])
{
  oled.clear();
  oled.set1X();
  oled.println(title);
}

void display_setting(int setting)
{
  oled.clear(0, 128, 2, 8);
  oled.setCursor(0, 2);
  oled.set2X();
  oled.println(setting);
}

void display_setting(char setting[])
{
  oled.clear(0, 128, 2, 8);
  oled.setCursor(0, 2);
  oled.set2X();
  oled.println(setting);
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
  //unsigned long time_per_capture = (unsigned long)duration * 60UL * 1000UL * 1000UL / (unsigned long)captures;
  unsigned long time_per_capture = (unsigned long)interval * 1000UL * 1000UL;
  duration = interval * captures;
  
  int steps_per_capture = steps_per_track / captures;
  unsigned long capture_ms = shutter_time * 1000UL;
  unsigned long delay_ms = 0UL;
  if (capture_ms == 0)
  {
    capture_ms = 50UL;
  }

  Serial.print("duration / captures / interval: ");
  Serial.println(duration);
  Serial.println(captures);
  Serial.println(interval);

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

    delay_ms = max(((time_per_capture - (micros() - startTime)) / 1000.0 - steps_per_capture * 2) / 2.0, 0);
    Serial.print("Delay: ");
    Serial.print("         ");
    Serial.print(time_per_capture);
    Serial.print(" - ");
    Serial.println(micros() - startTime);
    Serial.println(delay_ms);
    delay(delay_ms);

    digitalWrite(PIN_ENABLE, LOW); // enable power to motor
    for (int steps=0; steps<steps_per_capture; steps++)
    {
      digitalWrite(PIN_STEP, HIGH);
      delay(1);
      digitalWrite(PIN_STEP, LOW);
      delay(1);
    }
    digitalWrite(PIN_ENABLE, HIGH); // disable power to motor

    //delay(500); //reduce vibration (theory)
    if (time_per_capture < (micros() - startTime)) {
      delay_ms = 50;
    }
    else {
      delay_ms = max((time_per_capture - (micros() - startTime)) / 1000.0, 0);
    }
    
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
