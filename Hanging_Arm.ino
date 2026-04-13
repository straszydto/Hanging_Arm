#include <Bluepad32.h>
#include <Servo.h>

//define controller
ControllerPtr myController = nullptr;

Servo eyeServo;

const int eyePin=18;

int eyeAngle = 90;

const int deadzone=200;

void onConnectedController(ControllerPtr ctl) {
  Serial.println("Controller Connectected!");
  myController = ctl;
}

void onDisconnectedController(ControllerPtr ctl) {
  Serial.println("Controller Disconnected");
  myController = nullptr;
}

unsigned long lastOutput = 0, lastMove=0;

void setup() {
  Serial.begin(115200);
  delay(1000);

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

  int rt = myController->throttle();
  int lt = myController->brake();

  if (now-lastOutput >= 2500) {
    lastOutput=now;
    Serial.printf("Battery: %f, Let Stick X: %4d, Y: %4d\nRight Stick X: %4d, Y: %4d\nRT: %4d, LT: %4d\n",
    myController->battery(),
    myController->axisX(),
    myController->axisY(),
    myController->axisRX(),
    myController->axisRY(),
    myController->throttle(),
    myController->brake()
    );

    Serial.printf("Eye Angle: %d", eyeAngle);
  }

  if (now-lastMove >=200) {
    now=lastMove;
    if (rt > deadzone && eyeAngle<180) {
      eyeAngle+=2;
      eyeServo.write(eyeAngle);
    } else if (lt > deadzone && eyeAngle>0) {
      eyeAngle-=2;
      eyeServo.write(eyeAngle);
    }
  }
  //Eye Movement
  delay(10);
}