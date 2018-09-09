#include <Servo.h>

// Constants
#define DRAW 0
#define TRANSMIT 1
#define CALLIBRATE 2

#define OFF 0
#define ON 1
#define BLINK 2
#define FADE 3

#define BUFFER_SIZE 27 // actual size of the buffer for integer values: (numberOfValsToRead*6)+(numberOfValsToRead-1)

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

unsigned int commands[200][4];

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
  Serial.begin(57600);

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
    penDown = false;
    readSerial = false;
    drawing = false;
    command = 0;
    incommingCounter = 0;

    mode = (mode + 1) % 3;
  }
  modeBtnState = newModeBtnState;
}

void draw(bool action) {
  light(MODE_LED, ON);

  if (action) {
    drawing = !drawing;
  }

  if (drawing) {
    light(ACTION_LED, ON);
    Serial.println("Drawing");
    Serial.print(commands[command][0]);
    Serial.print(commands[command][1]);
    Serial.print(commands[command][2]);
    Serial.println(commands[command][3]);
    if (commands[command][0] == 0 && commands[command][1] == 0 && commands[command][2] == 0 && commands[command][3] == 0) {
      Serial.println("Stop Drawing");
      raisePen();
      drawing = false;
      command = 0;
    } else {
      if (commands[command][0] == 1) {
        lowerPen();
      } else {
        raisePen();
      }
      // delay(250);

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
  } else {
    light(ACTION_LED, OFF);
  }
}

void transmit(bool action) {
  light(MODE_LED, BLINK);

  // if (readSerial) {

  while (Serial.available() > 0) {
    light(ACTION_LED, ON);
    incomming = Serial.read(); // read single incommning bytes

    if (incomming != '\r') {
      incommingBuffer[incommingCounter++] = incomming; // go on the next position in the buffer
    } else {
      incommingBuffer[incommingCounter] = '&#092;&#048;'; // set the last byte to NULL to sign it for the string operators

      int a = atoi(strtok(incommingBuffer, ",.;"));
      int b = atoi(strtok(NULL, ",.;"));
      int c = atoi(strtok(NULL, ",.;"));
      int d = atoi(strtok(NULL, ",.;"));

      commands[command][0] = a;
      commands[command][1] = b;
      commands[command][2] = c;
      commands[command][3] = d;
      Serial.println(command++);

      if (a == 0 && b == 0 & c == 0 & d == 0) {
        command = 0;
        light(ACTION_LED, OFF);
      }
      incommingCounter = 0; // reset the counter
      memset(incommingBuffer, 0, BUFFER_SIZE); //overwrite the incommingBuffer
    }
  }
  light(ACTION_LED, OFF);
}

void callibrate(bool action) {
  light(MODE_LED, FADE);

  if (action) {
    penDown = !penDown;
    light(ACTION_LED, ON);
    if (penDown) {
      raisePen();
    } else {
      lowerPen();
    }
    delay(250);
    light(ACTION_LED, OFF);
  }
}

void light(byte led, byte lightMode) {
  int brightness;
  switch (lightMode) {
    case OFF:
      digitalWrite(led, LOW);
      break;
    case ON:
      digitalWrite(led, HIGH);
      break;
    case BLINK:
      brightness = int((time / 100) % 2);
      analogWrite(MODE_LED, brightness * 255);
      break;
    case FADE:
      brightness = abs(256 - int((time / 2) % 512));
      analogWrite(led, brightness);
      break;
  }
}

void raisePen () {
  servo.write(20);
}

void lowerPen () {
  servo.write(0);
}

// #define BUFFER_SIZE 27 // actual size of the buffer for integer values: (numberOfValsToRead*6)+(numberOfValsToRead-1)
//
// char incommingBuffer[BUFFER_SIZE]; // buffer to store incomming values
// char incomming; // primary buffer to store single incommning bytes
// int incommingCounter = 0; // counter for counting the positions inside the buffer
// int command = 0;
//
// void setup() {
//   Serial.begin(57600);
// }
//
// void loop() {
//   while (Serial.available() > 0) {
//     incomming = Serial.read(); // read single incommning bytes
//
//     if (incomming != '\r') {
//       incommingBuffer[incommingCounter++] = incomming; // go on the next position in the buffer
//     } else {
//       incommingBuffer[incommingCounter] = '&#092;&#048;'; // set the last byte to NULL to sign it for the string operators
//
//       char *a = strtok(incommingBuffer, ",.;"); // split the string after delimiters into tokens
//       char *b = strtok(NULL, ",.;"); // ...
//       char *c = strtok(NULL, ",.;"); // ...
//       char *d = strtok(NULL, ",.;"); // add another line if needed
//
//       Serial.println(command++);
//
//       incommingCounter = 0; // reset the counter
//       memset(incommingBuffer, 0, BUFFER_SIZE); //overwrite the incommingBuffer
//     }
//   }
// }
