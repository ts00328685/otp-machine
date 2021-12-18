#include <Servo.h>

Servo servo1;  // create servo object to control a servo
Servo servo2;

boolean flag = false;

void setup() {
  servo1.attach(1);  // attaches the servo on pin 9 to the servo object
  servo2.attach(2);

  // read this pin to trigger motor
  pinMode(A1, INPUT);

  // camera
  pinMode(A0, OUTPUT);
  digitalWrite(A0, LOW);

  pressPasswordBtn();
}

void loop() {
  delay(300);
  handleServo();
}

void handleServo() {
  int a1Val = digitalRead(A1);

  for (int i = 1; i <= 5; i++) {
    delay(200);
    a1Val = digitalRead(A1);
  }

  if (a1Val == 1) {
    flag = true;
  }

  if (flag) {

    pressPowerBtn();
    pressPasswordBtn();
    pressPasswordBtn();

    digitalWrite(A0, HIGH);
    delay(2000);
    digitalWrite(A0, LOW);

    flag = false;
    delay(12000);
  }
}

void pressPowerBtn() {

  // SERVO 2
  servo2.write(96);
  delay(150);

  servo2.write(45);
  delay(150);

  servo2.write(96);
  delay(500);
}


void pressPasswordBtn() {

  // SERVO 1

  // lift up
  servo1.write(82);

  // bottom right default #149
  servo1.write(130);
  delay(150);

  // lift up
  servo1.write(82);
  delay(600);

  // bottom left  default #35
  servo1.write(40);
  delay(150);

  // lift up
  servo1.write(82);
  delay(600);

}
