#include <Bluepad32.h>
#include <Servo.h>

//define controller
ControllerPtr myController = nullptr;

Servo panServo, eyeServo;

const int panPin=18, eyePin=19;

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

    Serial.printf("Eye Angle: %d", eyeAngle);
  }

  //Movement Section ever X miliseconds
  if (now-lastMove >= moveInterval) {
    lastmove=now;

    //Pan Movement
    if (abs(lsX) >= deadzone) {
      panAngle += map(lsX, -512, 512, 0, 180)
      panAngle = constrain(panAngle, 0, 180)
      panServo.write(panAngle);
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