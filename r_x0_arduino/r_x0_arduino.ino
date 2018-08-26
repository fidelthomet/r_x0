#include <Servo.h>

// Constants
#define DRAW 0
#define TRANSMIT 1
#define CALLIBRATE 2

#define LEFT LOW
#define RIGHT HIGH
#define BACK HIGH
#define FORWARD LOW

#define BUFFER_SIZE 20 // actual size of the buffer for integer values: (numberOfValsToRead*6)+(numberOfValsToRead-1)

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

Servo servo;
byte mode = DRAW;
bool modeBtnState = true;
bool actionBtnState = true;

unsigned int commands[100][4];

unsigned long time;
bool penDown = false;
bool readSerial = false;
bool drawing = false;
int command = 0;

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

  commands[0][0] = 1;
  commands[0][1] = 1;
  commands[0][2] = 0;
  commands[0][3] = 800;

  commands[1][0] = 0;
  commands[1][1] = 1;
  commands[1][2] = 1;
  commands[1][3] = 400;

  commands[2][0] = 1;
  commands[2][1] = 1;
  commands[2][2] = 0;
  commands[2][3] = 800;

  commands[3][0] = 0;
  commands[3][1] = 0;
  commands[3][2] = 0;
  commands[3][3] = 0;
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

  if (action) {
    drawing = !drawing;
  }

  if (drawing) {
    // int c[4] = commands[command];
    if (commands[command][0] == 0 && commands[command][1] == 0 && commands[command][2] == 0 && commands[command][3] == 0) {
      raisePen();
      drawing = false;
      command = 0;
    } else {
      if (commands[command][0] == 1) {
        lowerPen();
      } else {
        raisePen();
      }
      delay(250);

      digitalWrite(ST1DIR, commands[command][1]);
      digitalWrite(ST2DIR, commands[command][2]);

      for (int i = 0; i < commands[command][3]; i++){
        digitalWrite(ST1STEP, HIGH);
        digitalWrite(ST2STEP, HIGH);
        delay(1);

        digitalWrite(ST1STEP, LOW);
        digitalWrite(ST2STEP, LOW);
        delay(1);
      }
      command++;
    }
  }
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
        char *d = strtok(NULL, ",.;"); // add another line if needed

        if (String(a) == "STOP") {
          digitalWrite(ACTION_LED, HIGH);
          Serial.println("SUCCESS");
          readSerial = false;
          command = 0;
        } else {
          commands[command][0] = atoi(a); // convert the strings into integers
          commands[command][1] = atoi(b); // ...
          commands[command][2] = atoi(c); // ...
          commands[command][3] = atoi(d); // add another line if needed
          command++;
        }
        incommingCounter = 0; // reset the counter
        memset(incommingBuffer, 0, BUFFER_SIZE); //overwrite the incommingBuffer
      }
    }
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
