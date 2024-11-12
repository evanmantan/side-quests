#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <NewPing.h>

#define IN_TRIG 26
#define IN_ECHO 25
#define OUT_TRIG 17
#define OUT_ECHO 16

void idleState();
void visitorIn();
void visitorOut();

NewPing inSensor(IN_TRIG, IN_ECHO);
NewPing outSensor(OUT_TRIG, OUT_ECHO);

LiquidCrystal_I2C lcd(0x27, 16, 2);
int visitorCount = 0;
int state = 0; 
int inDistanceRef = 0;
int outDistanceRef = 0;
int inDistance = 0;
int outDistance = 0;
bool helper = false;

const int threshold = 10; // Reading threshold before detecting person (in cm)
const int timeout = 5000; // Timeout to implement WDT

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Hello, ESP32!");
  lcd.init();
  lcd.backlight();
  // set cursor to first column, first row
  lcd.setCursor(0, 0);
  // print message
  lcd.print("Calibrating");
  for (int i = 0; i < 10; i++) {
    inDistanceRef += inSensor.ping_cm();
    outDistanceRef += outSensor.ping_cm();
    delay(500);
    if ((i+1) % 3 == 0) lcd.print(".");
  }
  inDistanceRef = inDistanceRef / 10 - threshold;
  outDistanceRef = outDistanceRef / 10 - threshold;

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Total Visitor:");
}

void loop() {
  lcd.setCursor(0,1);
  lcd.print(visitorCount);

  inDistance = inSensor.ping_cm();
  outDistance = outSensor.ping_cm();

  switch (state) {
    case 0:
      idleState();
      break;
    case 1:
      visitorIn();
      break;
    case 2:
      visitorOut();
      break;
  }
}

void idleState() {
  if (outDistance < outDistanceRef) state = 1;
  else if (inDistance < inDistanceRef) state = 2;
}

void visitorIn() {
  if (inDistance < inDistanceRef) helper = true;
  else if (helper == true) {
    state = 0;
    helper = false;
    visitorCount += 1;
  }
}

void visitorOut() {
  if (outDistance < outDistanceRef) helper = true;
  else if (helper == true) {
    state = 0;
    helper = false;
    visitorCount -= 1;
  }
}
