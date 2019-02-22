// pins
const int ENABLE = 13;
const int SAIN1 = 11; // motor A
const int SAIN2 = 12;
const int SBIN1 = 10; // motor B
const int SBIN2 = 8;
const int SCIN1 = 6;  // motor C
const int SCIN2 = 7;
const int SDIN1 = 5;  // motor D
const int SDIN2 = 4;

// PWM values
const int pwm0 = 0; //PWM 0%
//const int speed = 30; // 0-255 (higher means faster)

// used to getCompass value
char valueBytes[8];
int degree = 0;
int counter = 0;
byte value = 0;


void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);

  // Initialize digital pins
  pinMode(ENABLE, OUTPUT);
  pinMode(SAIN2, OUTPUT);
  pinMode(SBIN2, OUTPUT);
  pinMode(SCIN2, OUTPUT);
  pinMode(SDIN2, OUTPUT);

  start();
}

void loop() {
  // illustration of car motor control functions
  //  forward();
  //  delay(1000);
  //  left();
  //  delay(1000);
  //  backward();
  //  delay(1000);
  //  right();
  //  delay(1000);
  //  selfRotationClockwise();
  //  delay(1000);

  adjustCarToNorth();
  forward();
  delay(5000);
}

//void rotateCarToNorth() {
//  int compassDirection = getCompass();
//  while (compassDirection > 0 && compassDirection < 359) {
//    selfRotationClockwise();
//    compassDirection = getCompass();
//    Serial.println(compassDirection);
//  }
//}

void adjustCarToNorth() {
  int compassDirection = getCompass();
  int compassDiff;
  bool clockwise;
  int targetspeed;
  while (compassDirection != 0) {
    compassDirection = getCompass();
    compassDiff = compassDirection;
    clockwise = false;
    if (compassDirection > 180) {
      compassDiff = 360 - compassDirection;
      clockwise = true;
    }
    targetspeed = 120;
    if (compassDiff < 60) {
      targetspeed=map(compassDiff, 0, 60, 20, 50);
    }
    if (clockwise) {
      selfRotationAntiClockwise(targetspeed);
    } else {
      selfRotationClockwise(targetspeed);
    }
  }
}
void move(char motor, String direction, int speed) {
  int modeAD;
  int modeBC;
  int pwmAD;
  int pwmBC;

  if (direction.equals("clockwise")) {
    modeAD = HIGH;
    modeBC = LOW;
    pwmAD = 255 - speed;
    pwmBC = 0 + speed;
  } else if (direction.equals("anti-clockwise")) {
    modeAD = LOW;
    modeBC = HIGH;
    pwmAD = 0 + speed;
    pwmBC = 255 - speed;
  } else {
    return;
  }

  switch (motor) {
    case 'A':
      digitalWrite(SAIN2, modeAD);
      analogWrite(SAIN1, pwmAD);
      break;
    case 'D':
      digitalWrite(SDIN2, modeAD);
      analogWrite(SDIN1, pwmAD);
      break;
    case 'B':
      digitalWrite(SBIN2, modeBC);
      analogWrite(SBIN1, pwmBC);
      break;
    case 'C':
      digitalWrite(SCIN2, modeBC);
      analogWrite(SCIN1, pwmBC);
      break;
    default:
      break;
  }
}

void stop() {
  digitalWrite(ENABLE, LOW);
}

void start() {
  // Initialize analog pins
  analogWrite(SAIN1, pwm0);
  analogWrite(SBIN1, pwm0);
  analogWrite(SCIN1, pwm0);
  analogWrite(SDIN1, pwm0);

  digitalWrite(ENABLE, HIGH);
}

void forward() {
  move('A', "clockwise", 30);
  move('B', "clockwise", 30);
  move('C', "clockwise", 30);
  move('D', "clockwise", 30);
}
//
//void backward() {
//  move('A',"anti-clockwise");
//  move('B',"anti-clockwise");
//  move('C',"anti-clockwise");
//  move('D',"anti-clockwise");
//}
//
////
//void left() {
//  move('A',"anti-clockwise");
//  move('C',"anti-clockwise");
//  move('B',"clockwise");
//  move('D',"clockwise");
//}
//
//void right() {
//  move('B',"anti-clockwise");
//  move('D',"anti-clockwise");
//  move('A',"clockwise");
//  move('C',"clockwise");
//}

void selfRotationClockwise(int speed) {
  move('C', "anti-clockwise", speed);
  move('D', "anti-clockwise", speed);
  move('A', "clockwise", speed);
  move('B', "clockwise", speed);
}

void selfRotationAntiClockwise(int speed) {
  move('A', "anti-clockwise", speed);
  move('B', "anti-clockwise", speed);
  move('C', "clockwise", speed);
  move('D', "clockwise", speed);
}

int getCompass() {

  value = 0;

  Serial1.write(0x31);
  if (Serial1.available()) {
    Serial1.read();
  }
  while (value == 0) {
    if (Serial1.available()) {
      valueBytes[counter] = Serial1.read();
      counter = (counter + 1) % 8;
      if (counter == 0) {
        degree = (valueBytes[2] - 48) * 100 + (valueBytes[3] - 48) * 10 + (valueBytes[4] - 48);
        value = 1;
      }
    }
  }
  return degree;
}
