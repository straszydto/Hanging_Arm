#include <Bluepad32.h>
#include <Servo.h>

//define controller
ControllerPtr myController = nullptr;

Servo panServo, tiltServo, headRServo, headLServo, eyeServo;

const int panPin=13, tiltPin=12, headRPin=14, headLPin=27, eyePin=20;

int panAngle = 90, eyeAngle = 90;

const int deadzone=200;

int outputInterval = 2000, moveInterval = 20;

unsigned long lastOutput = 0, lastMove=0;

void onConnectedController(ControllerPtr ctl) {
  Serial.println("Controller Connectected!");
  myController = ctl;
}

void onDisconnectedController(ControllerPtr ctl) {
  Serial.println("Controller Disconnected");
  myController = nullptr;
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  panServo.attach(panPin);
  panServo.write(panAngle);

  tiltServo.attach(tiltPin);
  tiltServo.write(90);

  headRServo.attach(headRPin);
  headLServo.attach(headLPin);
  headRServo.write(90);
  headLServo.write(90);

  eyeServo.attach(eyePin);
  eyeServo.write(eyeAngle);

  BP32.setup(&onConnectedController, &onDisconnectedController);

  Serial.println("Waiting for controller...");
}

void loop() {
  BP32.update();

  if(!myController || !myController->isConnected()) {
    delay(10);
    return;
  }

  unsigned long now = millis();

  int lsX = myController->axisX();
  int lsY = myController->axisY();
  int rsX = myController->axisRX();
  int rsY = myController->axisRY();
  int rt = myController->throttle();
  int lt = myController->brake();

  if (now-lastOutput >= outputInterval) {
    lastOutput=now;
    Serial.printf("Left Stick X: %4d, Y: %4d\nRight Stick X: %4d, Y: %4d\nRT: %4d, LT: %4d\n",
    lsX, lsY,
    rsX, rsY,
    rt, lt
    );

    Serial.printf("Pan Angle: %d\n", panAngle);
    Serial.printf("Eye Angle: %d\n", eyeAngle);
    
  }

  //Movement Section ever X miliseconds
  if (now-lastMove >= moveInterval) {
    lastMove=now;

    //Pan Movement
    if (lsX >= deadzone || lsX <= -deadzone) {
      panAngle += map(lsX, -512, 512, -5, 5);
      panAngle = constrain(panAngle, 0, 180);
      panServo.write(panAngle);
    }

    //tilt movement
    if (abs(lsY) > deadzone) {
      int move = map(lsY, -512, 512, 0, 180);
      tiltServo.write(move);
    } else {
      tiltServo.write(90);
    }

    if (abs(rsY) > deadzone) {
      int move = map(rsY, -512, 512, 0, 180);
      headRServo.write(move);
      headLServo.write(move);
    } else {
      headRServo.write(90);
      headLServo.write(90);
    }

    //Eye movement using RT/LT
    if (rt > deadzone && eyeAngle<180) {
      eyeAngle+=2;
      eyeServo.write(eyeAngle);
    } else if (lt > deadzone && eyeAngle>0) {
      eyeAngle-=2;
      eyeServo.write(eyeAngle);
    }
  }

  delay(10);
}