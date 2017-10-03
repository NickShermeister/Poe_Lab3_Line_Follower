#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"

int sensorPinLeft = A0;    // select the input pin for the potentiometer
int sensorValueLeft = 0;  // variable to store the value coming from the sensor
int sensorPinRight = A1;
int sensorValueRight = 0;
int ByteReceived = 0;
int i = 0;

// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
// Or, create it with a different I2C address (say for stacking)
// Adafruit_MotorShield AFMS = Adafruit_MotorShield(0x61); 

// Select which 'port' M1, M2, M3 or M4. In this case, M1
Adafruit_DCMotor *myMotor = AFMS.getMotor(1);
// You can also make another motor on port M2
//Adafruit_DCMotor *myOtherMotor = AFMS.getMotor(2);

void setup() {

  Serial.begin(9600);

  AFMS.begin();  // create with the default frequency 1.6KHz
  //AFMS.begin(1000);  // OR with a different frequency, say 1KHz
  
  // Set the speed to start, from 0 (off) to 255 (max speed)
  myMotor->setSpeed(150);
  myMotor->run(FORWARD);
  // turn on motor
  myMotor->run(RELEASE);
}

void loop() {

  if(ByteReceived == '0')
  {
  sensorValueLeft = analogRead(sensorPinLeft);
  Serial.println("Value on left: ");
  Serial.println(sensorValueLeft);

  sensorValueRight = analogRead(sensorPinRight);
  Serial.println("Value on right: ");
  Serial.println(sensorValueRight);

  delay(500);
  }

  else if(ByteReceived == '1')
  {
    myMotor->run(FORWARD);
    for (i=0; i<255; i++) {
      myMotor->setSpeed(i);  
      delay(10);
    }
    for (i=255; i!=0; i--) {
    myMotor->setSpeed(i);  
    delay(10);
    }
  }

  else if(ByteReceived == '2')
  {
    myMotor->run(BACKWARD);
  for (i=0; i<255; i++) {
    myMotor->setSpeed(i);  
    delay(10);
  }
  for (i=255; i!=0; i--) {
    myMotor->setSpeed(i);  
    delay(10);
  }
  }

  if (Serial.available() > 0)
  {
    ByteReceived = Serial.read();
    Serial.print(ByteReceived);   
    Serial.print("        ");      
    Serial.print(ByteReceived, HEX);
    Serial.print("       ");     
    Serial.print(char(ByteReceived));
    
    Serial.println();    // End the line

  // END Serial Available
  }

}
