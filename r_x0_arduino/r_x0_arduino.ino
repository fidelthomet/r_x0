#include <Servo.h>

// Constants
#define DRAW 0
#define TRANSMIT 1
#define CALLIBRATE 2

#define LEFT LOW
#define RIGHT HIGH
#define BACK HIGH
#define FORWARD LOW

// Pins
#define MODE_BTN 7
#define ACTION_BTN 8

#define MODE_LED 11
#define ACTION_LED 6

#define ST1DIR 2
#define ST1STEP 3
#define ST2DIR 4
#define ST2STEP 5

#define SERVO 9

#define BUFFER_SIZE 20 // actual size of the buffer for integer values: (numberOfValsToRead*6)+(numberOfValsToRead-1)

Servo servo;
byte mode = DRAW;
bool modeBtnState = true;
bool actionBtnState = true;

unsigned long time;
bool penDown = false;
bool readSerial = false;

const float step_mm = (76 * PI) / 1600;
const float step_deg = (240 * PI) / 360 / step_mm;
 
char incommingBuffer[BUFFER_SIZE]; // buffer to store incomming values
char incomming; // primary buffer to store single incommning bytes
int incommingCounter = 0; // counter for counting the positions inside the buffer

void setup() {
  pinMode(ACTION_BTN, INPUT_PULLUP);
  pinMode(MODE_BTN, INPUT_PULLUP);

  pinMode(ACTION_LED, OUTPUT);
  pinMode(MODE_LED, OUTPUT);

  pinMode(ST1DIR, OUTPUT);
  pinMode(ST1STEP, OUTPUT);
  pinMode(ST2DIR, OUTPUT);
  pinMode(ST2STEP, OUTPUT);

  servo.attach(9);
  Serial.begin(9600);
}

void loop() {
  time = millis();
  updateMode();

  bool newActionBtnState = digitalRead(ACTION_BTN);
  bool action = newActionBtnState && !actionBtnState;
  actionBtnState = newActionBtnState;

  switch (mode) {
  case DRAW:
    draw(action);
    break;
  case TRANSMIT:
    transmit(action);
    break;
  case CALLIBRATE:
    callibrate(action);
    break;
  }
}

void updateMode() {
  bool newModeBtnState = digitalRead(MODE_BTN);
  if (newModeBtnState && !modeBtnState) {
    mode = (mode + 1) % 3;
  }
  modeBtnState = newModeBtnState;
}

void draw(bool action) {
  analogWrite(MODE_LED, 255);
}

void transmit(bool action) {
  int led = int((time / 100) % 2);
  analogWrite(MODE_LED, led * 255);

  if (readSerial) {
    while (Serial.available() > 0) {
      incomming = Serial.read(); // read single incommning bytes
 
      if (incomming != '\r') {
        incommingBuffer[incommingCounter++] = incomming; // go on the next position in the buffer
      } else {
        incommingBuffer[incommingCounter] = '&#092;&#048;'; // set the last byte to NULL to sign it for the string operators
   
        char *a = strtok(incommingBuffer, ",.;"); // split the string after delimiters into tokens
        char *b = strtok(NULL, ",.;"); // ...
        char *c = strtok(NULL, ",.;"); // ...
        //char *d = strtok(NULL, ",.;"); // add another line if needed

        if (String(a) == "STOP") {
          digitalWrite(ACTION_LED, HIGH);
          Serial.println("SUCCESS");
          readSerial = false;
        } else {
          int firstValue = atoi(a); // convert the strings into integers
          int secondValue = atoi(b); // ...
          int thirdValue = atoi(c); // ...
          //fourthValue = atoi(d); // add another line if needed    
          
  
          Serial.print(firstValue); // debugging
          Serial.print("\t");
          Serial.print(secondValue);
          Serial.print("\t");
          Serial.print(thirdValue);
          Serial.print("\t");
          Serial.println(); // send a carriage return for debugging
        }
        incommingCounter = 0; // reset the counter
        memset(incommingBuffer, 0, BUFFER_SIZE); //overwrite the incommingBuffer
      }
//      char type = Serial.read();
//      char dir = Serial.read();
//      char value = Serial.parseInt();
//      Serial.println("--");
//      Serial.print(value);
//      
//
//      if (Serial.read() == '\n') {
//        Serial.println(type);
//        Serial.println("--");
//        Serial.println(dir);
//        Serial.println("--");
//        Serial.print(value);
//        Serial.println();
//        readSerial = false;
//        digitalWrite(ACTION_LED, HIGH);
//      }
    }

    //Serial.print("END");
  }

  if (action) {
    digitalWrite(ACTION_LED, LOW);
    Serial.println("START");
    readSerial = true;
  }
}

void callibrate(bool action) {
  int led = abs(256 - int((time / 2) % 512));
  analogWrite(MODE_LED, led);

  if (action) {
    penDown = !penDown;
    if (penDown) {
      raisePen();
    } else {
      lowerPen();
    }
  }
}

void rotate (bool dir, float deg) {
  digitalWrite(ST1DIR, dir);
  digitalWrite(ST2DIR, dir);

  const int steps = round(deg * step_deg);

  for (int i = 0; i < steps; i++){
    digitalWrite(ST1STEP, HIGH);
    digitalWrite(ST2STEP, HIGH);
    delay(1);

    digitalWrite(ST1STEP, LOW);
    digitalWrite(ST2STEP, LOW);
    delay(1);
  }
}

void move (bool dir, float mm) {
  digitalWrite(ST1DIR, dir);
  digitalWrite(ST2DIR, !dir);

  const int steps = round(mm / step_mm);

  for (int i = 0; i < steps; i++) {
    digitalWrite(ST1STEP, HIGH);
    digitalWrite(ST2STEP, HIGH);
    delay(1);
    digitalWrite(ST1STEP, LOW);
    digitalWrite(ST2STEP, LOW);
    delay(1);
  }
}

void raisePen () {
  servo.write(20);
}

void lowerPen () {
  servo.write(0);
}
