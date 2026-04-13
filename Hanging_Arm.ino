#include <Bluepad32.h>

//define controller
ControllerPtr myController = nullptr;


void onConnectedController(ControllerPtr ctl) {
  Serial.println("Controller Connectected!");
  myController = ctl;
}

void onDisconnectedController(ControllerPtr ctl) {
  Serial.println("Controller Disconnected");
  myController = nullptr;
}

unsigned long lastOutput = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);

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
  if (now-lastOutput >= 2500) {
    lastOutput=now;
    Serial.printf("Let Stick X: %4d, Y: %4d\nRight Stick X: %4d, Y: %4d\nRT: %4d, LT: %4d\n", 
    myController->axisX(),
    myController->axisY(),
    myController->axisRX(),
    myController->axisRY(),
    myController->throttle(),
    myController->brake()
    );
  }

}