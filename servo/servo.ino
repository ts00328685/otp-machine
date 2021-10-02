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

}

void loop() {

  delay(1000);
  handleServo();

}

void handleServo() {
  int a1Val = digitalRead(A1);

  for (int i = 1; i <= 5; i++) {
    delay(300);
    a1Val = digitalRead(A1);
  }

  if (a1Val == 1) {
    flag = true;
  }

  if (flag) {

    int val = 0;

    // SERVO 2
    for (int i = 1; i <= 2; i++ ) {
      val = map(550, 0, 1023, 0, 180);     // scale it to use it with the servo (value between 0 and 180)
      servo2.write(val);                  // sets the servo position according to the scaled value
      delay(500);

      val = map(260, 0, 1023, 0, 180);     // scale it to use it with the servo (value between 0 and 180)
      servo2.write(val);                  // sets the servo position according to the scaled value
      delay(250);                           // waits for the servo to get there

      val = map(550, 0, 1023, 0, 180);     // scale it to use it with the servo (value between 0 and 180)
      servo2.write(val);                  // sets the servo position according to the scaled value
      delay(1000);                           // waits for the servo to get there
    }

    // SERVO 1
    for (int i = 1; i <= 2; i++ ) {
      // reads the value of the potentiometer (value between 0 and 1023)
      val = map(470, 0, 1023, 0, 180);     // scale it to use it with the servo (value between 0 and 180)
      servo1.write(val);                  // sets the servo position according to the scaled value

      val = map(850, 0, 1023, 0, 180);     // scale it to use it with the servo (value between 0 and 180)
      servo1.write(val);                  // sets the servo position according to the scaled value
      delay(200);                           // waits for the servo to get there

      val = map(470, 0, 1023, 0, 180);     // scale it to use it with the servo (value between 0 and 180)
      servo1.write(val);                  // sets the servo position according to the scaled value
      delay(1000);

      val = map(200, 0, 1023, 0, 180);     // scale it to use it with the servo (value between 0 and 180)
      servo1.write(val);                  // sets the servo position according to the scaled value
      delay(200);

      val = map(470, 0, 1023, 0, 180);     // scale it to use it with the servo (value between 0 and 180)
      servo1.write(val);                  // sets the servo position according to the scaled value
      delay(1000);

    }


    digitalWrite(A0, HIGH);
    delay(2000);
    digitalWrite(A0, LOW);

    flag = false;
    delay(12000);
  }
}
