// pins
const int ENABLE = 13;
const int SAIN1 = 12; // motor A
const int SAIN2 = 11;
const int SBIN1 = 10; // motor B
const int SBIN2 = 8;
const int SCIN1 = 6;  // motor C
const int SCIN2 = 7;
const int SDIN1 = 5;  // motor D
const int SDIN2 = 4;

// PWM values
const int pwm50 = 127;  // PWM 50%
const int pwm80 = 204; // PWM 80%

void setup() {
  // put your setup code here, to run once:
  pinMode(ENABLE, OUTPUT);
  pinMode(SAIN1, OUTPUT);
  pinMode(SAIN2, OUTPUT);
}

void loop() {
  start();

  // illustration of car motor control functions
  forward();
  delay(1000);
  backward();
  delay(1000);
  left();
  delay(1000);
  right();
}

void clockwise(int pin1, int pin2) {
  digitalWrite(pin2, LOW);
  analogWrite(pin1, pwm50); // pwm ratio 50%
}

void antiClockwise(int pin1, int pin2) {
  digitalWrite(pin2, HIGH);
  analogWrite(pin1, pwm80);
}

void stop() {
  digitalWrite(ENABLE, LOW);
}

void start() {
  digitalWrite(ENABLE, HIGH);
}

void forward() {
  clockwise(SAIN1, SAIN2);
  clockwise(SBIN1, SBIN2);
  clockwise(SCIN1, SCIN2);
  clockwise(SDIN1, SDIN2);
}

void backward() {
  antiClockwise(SAIN1, SAIN2);
  antiClockwise(SBIN1, SBIN2);
  antiClockwise(SCIN1, SCIN2);
  antiClockwise(SDIN1, SDIN2);
}

void left() {
  antiClockwise(SAIN1, SAIN2);
  antiClockwise(SCIN1, SCIN2);
  clockwise(SBIN1, SBIN2);
  clockwise(SDIN1, SDIN2);
}

void right() {
  clockwise(SAIN1, SAIN2);
  clockwise(SCIN1, SCIN2);
  antiClockwise(SBIN1, SBIN2);
  antiClockwise(SDIN1, SDIN2);
}
