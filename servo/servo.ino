#include <Servo.h>

Servo servo1;  // create servo object to control a servo
Servo servo2;
Servo servo3;

boolean flag = false;

void setup() {
  servo1.attach(1);  // attaches the servo on pin 9 to the servo object
  servo2.attach(2);
  servo3.attach(3);

  // read this pin to trigger motor
  pinMode(A1, INPUT);

  // camera
  pinMode(A0, OUTPUT);
  digitalWrite(A0, LOW);

  resetMotor();
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
    pressPowerBtn();
    pressPasswordBtn3();
    pressPasswordBtn1();

    digitalWrite(A0, HIGH);
    delay(2000);
    digitalWrite(A0, LOW);

    flag = false;
    delay(10000);
  }
}

void resetMotor() {
  // SERVO 2
  servo2.write(96);
  // lift up
  servo1.write(82);
  // lift up
  servo3.write(82);

}

void pressPowerBtn() {

  // SERVO 2
  servo2.write(96);

  // press down
  servo2.write(30);
  delay(150);

  servo2.write(96);
  delay(350);
}


void pressPasswordBtn1() {

  // SERVO 1

  // lift up
  servo1.write(82);

  // 3
  // bottom right default #149
  servo1.write(145);
  delay(170);

  // lift up
  servo1.write(82);
  delay(380);

  // 3
  // bottom right default #149
  servo1.write(145);
  delay(170);

  // lift up
  servo1.write(82);
  delay(380);

    // 3
  // bottom right default #149
  servo1.write(145);
  delay(170);

  // lift up
  servo1.write(82);
  delay(380);
  
//  // 9
//  // bottom left  default #35
//  servo1.write(50);
//  delay(150);
//
//  // lift up
//  servo1.write(82);
//  delay(600);

}


void pressPasswordBtn3() {

  // SERVO 3

  //  // lift up
  //  servo3.write(82);
  //
  //  // 7
  //  // bottom right default #149
  //  servo3.write(130);
  //  delay(150);

  // lift up
  servo3.write(82);

  // 1
  // bottom left  default #35
  servo3.write(37);
  delay(170);

  // lift up
  servo3.write(82);
  delay(380);

}
