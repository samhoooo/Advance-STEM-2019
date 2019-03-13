// already a 7 millisecond delay from 9600 baud communication
// actually a 21 ms delay built in in actual testing
// see https://www.microchip.com/forums/m405110.aspx
const int ENABLE = 13;
const int SAIN1 = 11; // motor A
const int SAIN2 = 12;
const int SBIN1 = 10; // motor B
const int SBIN2 = 8;
const int SCIN1 = 3; // motor C
const int SCIN2 = 7;
const int SDIN1 = 2; // motor D
const int SDIN2 = 4;
volatile int speed2distanceratio = 4;
volatile int speedthres = 64;
volatile int distanceDegredation = 4;

int designatedSpeed = 255;
int designatedDistance = 128;



volatile boolean forwardB = true;
volatile int speedB = designatedSpeed;
volatile int distanceB = designatedSpeed;
volatile int targetB = 0;
volatile int currentB = 0;

volatile boolean forwardC = true;
volatile int speedC = designatedSpeed;
volatile int distanceC = designatedSpeed;
volatile int targetC = 0;
volatile int currentC = 0;

volatile boolean forwardA = true;
volatile int speedA = designatedSpeed;
volatile int distanceA = designatedSpeed;
volatile int targetA = 0;
volatile int currentA = 0;

volatile boolean forwardD = true;
volatile int speedD = designatedSpeed;
volatile int distanceD = designatedSpeed;
volatile int targetD = 0;
volatile int currentD = 0;

volatile boolean spindirB = false;
volatile boolean spindirC = false;
volatile boolean spindirA = true;
volatile boolean spindirD = true;

volatile boolean enableB = false;
volatile boolean enableC = false;
volatile boolean enableA = false;
volatile boolean enableD = false;

//Notes and Warnings
//The PWM outputs generated on pins 5 and 6 will have higher-than-expected duty cycles.
//This is because of interactions with the millis() and delay() functions,
//which share the same internal timer used to generate those PWM outputs.
//This will be noticed mostly on low duty-cycle settings (e.g. 0 - 10)
//and may result in a value of 0 not fully turning off the output on pins 5 and 6.
void trigB() {
  if (forwardB) {
    currentB++;
    if (currentB > targetB + distanceB) {
      distanceB /= distanceDegredation;
      if (distanceB < speedB / speed2distanceratio) {

        distanceB = max(distanceB, speedB / speed2distanceratio);
        speedB /= 2;
      }

      forwardB = !forwardB;
    }

  } else {
    currentB--;
    if (currentB < targetB - distanceB) {
      distanceB /= 4;
      if (distanceB < speedB / speed2distanceratio) {
        distanceB = max(distanceB, speedB / speed2distanceratio);
        speedB /= 2;
      }

      forwardB = !forwardB;
    }
  }
  digitalWrite(SBIN2, spindirB ^ forwardB);
  if (spindirB ^ forwardB) {
    analogWrite(SBIN1, 255 - speedB);
  } else {

    analogWrite(SBIN1, speedB);
  }

  if (speedB < speedthres) {
    digitalWrite(SBIN2, HIGH);
    digitalWrite(SBIN1, HIGH);
    enableB = false;
    speedB = designatedSpeed;
    distanceB = designatedDistance;
    targetB = 0;
    currentB = 0;
    detachInterrupt(digitalPinToInterrupt(21));
  }
}

void trigC() {
  if (forwardC) {
    currentC++;
    if (currentC > targetC + distanceC) {
      distanceC /= distanceDegredation;
      if (distanceC < speedC / speed2distanceratio) {

        distanceC = max(distanceC, speedC / speed2distanceratio);
        speedC /= 2;
      }

      forwardC = !forwardC;
    }

  } else {
    currentC--;
    if (currentC < targetC - distanceC) {
      distanceC /= distanceDegredation;
      if (distanceC < speedC / speed2distanceratio) {
        distanceC = max(distanceC, speedC / speed2distanceratio);
        speedC /= 2;
      }

      forwardC = !forwardC;
    }
  }
  digitalWrite(SCIN2, spindirC ^ forwardC);
  if (spindirC ^ forwardC) {
    analogWrite(SCIN1, 255 - speedC);
  } else {

    analogWrite(SCIN1, speedC);
  }

  if (speedC < speedthres) {
    digitalWrite(SCIN2, HIGH);
    digitalWrite(SCIN1, HIGH);
    enableC = false;
    speedC = designatedSpeed;
    distanceC = designatedDistance;
    targetC = 0;
    currentC = 0;
    detachInterrupt(digitalPinToInterrupt(20));
  }
}

void trigA() {
  if (forwardA) {
    currentA++;
    if (currentA > targetA + distanceA) {
      distanceA /= distanceDegredation;
      if (distanceA < speedA / speed2distanceratio) {

        distanceA = max(distanceA, speedA / speed2distanceratio);
        speedA /= 2;
      }

      forwardA = !forwardA;
    }

  } else {
    currentA--;
    if (currentA < targetA - distanceA) {
      distanceA /= distanceDegredation;
      if (distanceA < speedA / speed2distanceratio) {
        distanceA = max(distanceA, speedA / speed2distanceratio);
        speedA /= 2;
      }

      forwardA = !forwardA;
    }
  }
  digitalWrite(SAIN2, spindirA ^ forwardA);
  if (spindirA ^ forwardA) {
    analogWrite(SAIN1, 255 - speedA);
  } else {

    analogWrite(SAIN1, speedA);
  }

  if (speedA < speedthres) {
    digitalWrite(SAIN2, HIGH);
    digitalWrite(SAIN1, HIGH);
    enableA = false;
    speedA = designatedSpeed;
    distanceA = designatedDistance;
    targetA = 0;
    currentA = 0;
    detachInterrupt(digitalPinToInterrupt(18));
  }
}

void trigD() {
  if (forwardD) {
    currentD++;
    if (currentD > targetD + distanceD) {
      distanceD /= distanceDegredation;
      if (distanceD < speedD / speed2distanceratio) {

        distanceD = max(distanceD, speedD / speed2distanceratio);
        speedD /= 2;
      }

      forwardD = !forwardD;
    }

  } else {
    currentD--;
    if (currentD < targetD - distanceD) {
      distanceD /= distanceDegredation;
      if (distanceD < speedD / speed2distanceratio) {
        distanceD = max(distanceD, speedD / speed2distanceratio);
        speedD /= 2;
      }

      forwardD = !forwardD;
    }
  }
  digitalWrite(SDIN2, spindirD ^ forwardD);
  if (spindirD ^ forwardD) {
    analogWrite(SDIN1, 255 - speedD);
  } else {

    analogWrite(SDIN1, speedD);
  }

  if (speedD < speedthres) {
    digitalWrite(SDIN2, HIGH);
    digitalWrite(SDIN1, HIGH);
    enableD = false;
    speedD = designatedSpeed;
    distanceD = designatedDistance;
    targetD = 0;
    currentD = 0;
    detachInterrupt(digitalPinToInterrupt(19));
  }
}

//detachInterrupt(digitalPinToInterrupt(pin))

void setup() {
  // initialize serial for debugging
  Serial.begin(2000000);
  Serial2.begin(9600);
  // Initialize digital pins
  pinMode(ENABLE, OUTPUT);
  pinMode(SAIN2, OUTPUT);
  pinMode(SBIN2, OUTPUT);
  pinMode(SCIN2, OUTPUT);
  pinMode(SDIN2, OUTPUT);

  start();



  while (true) {

    spindirB = false;
    spindirC = false;
    spindirA = true;
    spindirD = true;
    enableB = true;
    enableC = true;
    enableA = true;
    enableD = true;
    digitalWrite(SBIN2, !spindirB);
    digitalWrite(SBIN1, spindirB);
    targetB = 500;
    digitalWrite(SCIN2, !spindirC);
    digitalWrite(SCIN1, spindirC);
    targetC = 500;
    digitalWrite(SAIN2, !spindirA);
    digitalWrite(SAIN1, spindirA);
    targetA = 500;
    digitalWrite(SDIN2, !spindirD);
    digitalWrite(SDIN1, spindirD);
    targetD = 500;
    attachInterrupt(digitalPinToInterrupt(21), trigB, CHANGE);
    attachInterrupt(digitalPinToInterrupt(20), trigC, CHANGE);
    attachInterrupt(digitalPinToInterrupt(19), trigD, CHANGE);
    attachInterrupt(digitalPinToInterrupt(18), trigA, CHANGE);
    while (enableA || enableB || enableC || enableD) {}
    adjustCarToNorth(0);
    spindirB = true;
    spindirC = false;
    spindirA = true;
    spindirD = false;
    enableB = true;
    enableC = true;
    enableA = true;
    enableD = true;
    digitalWrite(SBIN2, !spindirB);
    digitalWrite(SBIN1, spindirB);
    targetB = 500;
    digitalWrite(SCIN2, !spindirC);
    digitalWrite(SCIN1, spindirC);
    targetC = 500;
    digitalWrite(SAIN2, !spindirA);
    digitalWrite(SAIN1, spindirA);
    targetA = 500;
    digitalWrite(SDIN2, !spindirD);
    digitalWrite(SDIN1, spindirD);
    targetD = 500;
    attachInterrupt(digitalPinToInterrupt(21), trigB, CHANGE);
    attachInterrupt(digitalPinToInterrupt(20), trigC, CHANGE);
    attachInterrupt(digitalPinToInterrupt(19), trigD, CHANGE);
    attachInterrupt(digitalPinToInterrupt(18), trigA, CHANGE);
    while (enableA || enableB || enableC || enableD) {}
    adjustCarToNorth(0);
    spindirB = true;
    spindirC = true;
    spindirA = false;
    spindirD = false;
    enableB = true;
    enableC = true;
    enableA = true;
    enableD = true;
    digitalWrite(SBIN2, !spindirB);
    digitalWrite(SBIN1, spindirB);
    targetB = 500;
    digitalWrite(SCIN2, !spindirC);
    digitalWrite(SCIN1, spindirC);
    targetC = 500;
    digitalWrite(SAIN2, !spindirA);
    digitalWrite(SAIN1, spindirA);
    targetA = 500;
    digitalWrite(SDIN2, !spindirD);
    digitalWrite(SDIN1, spindirD);
    targetD = 500;
    attachInterrupt(digitalPinToInterrupt(21), trigB, CHANGE);
    attachInterrupt(digitalPinToInterrupt(20), trigC, CHANGE);
    attachInterrupt(digitalPinToInterrupt(19), trigD, CHANGE);
    attachInterrupt(digitalPinToInterrupt(18), trigA, CHANGE);
    while (enableA || enableB || enableC || enableD) {}
    adjustCarToNorth(0);
    spindirB = false;
    spindirC = true;
    spindirA = false;
    spindirD = true;
    enableB = true;
    enableC = true;
    enableA = true;
    enableD = true;
    digitalWrite(SBIN2, !spindirB);
    digitalWrite(SBIN1, spindirB);
    targetB = 500;
    digitalWrite(SCIN2, !spindirC);
    digitalWrite(SCIN1, spindirC);
    targetC = 500;
    digitalWrite(SAIN2, !spindirA);
    digitalWrite(SAIN1, spindirA);
    targetA = 500;
    digitalWrite(SDIN2, !spindirD);
    digitalWrite(SDIN1, spindirD);
    targetD = 500;
    attachInterrupt(digitalPinToInterrupt(21), trigB, CHANGE);
    attachInterrupt(digitalPinToInterrupt(20), trigC, CHANGE);
    attachInterrupt(digitalPinToInterrupt(19), trigD, CHANGE);
    attachInterrupt(digitalPinToInterrupt(18), trigA, CHANGE);
    while (enableA || enableB || enableC || enableD) {}
    adjustCarToNorth(0);

  }


}

void loop() {

}

void stop() {
  digitalWrite(ENABLE, LOW);
}

void start() {
  // Initialize analog pins
  analogWrite(SAIN1, 0);
  analogWrite(SBIN1, 0);
  analogWrite(SCIN1, 0);
  analogWrite(SDIN1, 0);

  digitalWrite(ENABLE, HIGH);
}

int compasstranslate(int compass, int translate) {
  return (compass + translate) % 3600;

}
int degerr(int input) {
  if (input < 1800) {
    return input;
  } else {
    return 3600 - input;
  }
}



int getCompass() {
  char valueints[8];
  int degree = 0;
  int counter = 0;
  int value = 0;

  Serial2.write(0x31);
  if (Serial2.available()) {
    Serial2.read();
  }
  while (true) {
    if (Serial2.available()) {
      valueints[counter] = Serial2.read();
      counter = (counter + 1) % 8;
      if (!counter) {
        degree = (valueints[2] - 48) * 1000 + (valueints[3] - 48) * 100 + (valueints[4] - 48) * 10 + (valueints[6] - 48);
        return degree;
      }
    }
  }

}


void adjustCarToNorth(int angle) {

  int trial = 0;
  int compassDirection = compasstranslate(getCompass(), angle);
  int compassDiff;
  bool clockwise;
  int targetspeed;
  while (compassDirection != 0) {

    compassDirection = compasstranslate(getCompass(), angle);

    compassDiff = degerr(compassDirection);
    //    clockwise = false;
    clockwise = compassDirection > 1800;
    //    if (compassDirection > 1800) {
    //      compassDiff = 3600 - compassDirection;
    //      clockwise = true;
    //    }
    targetspeed = 130;
    if (compassDiff < 500) {
      targetspeed = map(compassDiff, 0, 500, 30, 130);
    }
    trial++;

    if (trial > 150) {
      selfRotationAntiClockwise(160);
      compassDirection = 1;
      delay(75);
      trial = 0;
    }
    targetspeed = min(targetspeed, trial * 5);
    if (clockwise) {
      selfRotationAntiClockwise(targetspeed);
    } else {
      selfRotationClockwise(targetspeed);
    }
  }
}





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



void move(char motor, String direction, int speed) {
  if (speed < 0) {
    if (direction.equals("clockwise")) {
      move2(motor, "anti-clockwise", -speed);
    } else if (direction.equals("anti-clockwise")) {
      move2(motor, "clockwise", -speed);
    } else {
      return;
    }
  } else {
    move2(motor, direction, speed);
  }
}


void move2(char motor, String direction, int speed) {
  boolean modeAD;
  boolean modeBC;
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
