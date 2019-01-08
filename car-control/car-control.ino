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
const int speed = 30; // 0-255 (higher means faster)

void setup() {
  // Initialize digital pins
  pinMode(ENABLE, OUTPUT);
  pinMode(SAIN2, OUTPUT);
  pinMode(SBIN2, OUTPUT);
  pinMode(SCIN2, OUTPUT);
  pinMode(SDIN2, OUTPUT);

  // Initialize analog pins
  analogWrite(SAIN1, pwm0);
  analogWrite(SBIN1, pwm0);
  analogWrite(SCIN1, pwm0);
  analogWrite(SDIN1, pwm0);
  start();
}

void loop() {
  // illustration of car motor control functions
  forward();
  delay(1000);
  left();
  delay(1000);
  backward();
  delay(1000);
  right();
  delay(1000);
}

void move(char motor, String direction) {  
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
  digitalWrite(ENABLE, HIGH);
}

void forward() {
  move('A',"clockwise");
  move('B',"clockwise");
  move('C',"clockwise");
  move('D',"clockwise");
}

void backward() {
  move('A',"anti-clockwise");
  move('B',"anti-clockwise");
  move('C',"anti-clockwise");
  move('D',"anti-clockwise");
}

//
void left() {
  move('A',"anti-clockwise");
  move('C',"anti-clockwise");
  move('B',"clockwise");
  move('D',"clockwise");
}

void right() {
  move('B',"anti-clockwise");
  move('D',"anti-clockwise");
  move('A',"clockwise");
  move('C',"clockwise");
}
