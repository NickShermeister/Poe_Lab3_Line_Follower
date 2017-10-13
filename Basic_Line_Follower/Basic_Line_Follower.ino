//  POE Lab 3
//  Remy Boudousquie & Nicholas Sherman 
//  10/12/2017

//Include needed libraries
#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"

//Define variables
int sensorPinLeft = A0;   // select the input pin for the left IR sensor
int sensorValueLeft = 0;  // variable to store the value coming from the left sensor
int sensorPinRight = A1;  // select the input pin for the right IR sensor
int sensorValueRight = 0; // variable to store the value coming from the right sensor
int ByteReceived = -1;    // variable that holds what bytes are received from serial
int motorSpeed = 24;      //The starting motor speed when cruising directly forward
int speedLeft = 0;        //The speed of the left wheel
int speedRight = 0;       //The speed of the right wheel
int newSpeed = 0;         //The new speed before max is taken into account
int bound = 350;          //The boundary that constitutes being on a line
int newBound = 0;        //The new bound before max is taken into account

// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield();

// Set up motors
Adafruit_DCMotor *leftMotor = AFMS.getMotor(1);
Adafruit_DCMotor *rightMotor = AFMS.getMotor(2);


//Setup Function
void setup() {
  Serial.begin(9600); //Begin the serial so we can read from it/print to it
  AFMS.begin();  // create with the default frequency 1.6KHz
  printHelp();   // print the key
  printOutput(); // print what is printed in each of the four columns
}


void loop() {
  
  //Stay still, nonverbose output
  if (ByteReceived == '0') {
    rightMotor->setSpeed(0);
    leftMotor->setSpeed(0);
    sensorValueLeft = analogRead(sensorPinLeft);
    sensorValueRight = analogRead(sensorPinRight);
  }

  //Stay still, verbose output
  else if (ByteReceived == '1') //Stay still.
  {
    rightMotor->setSpeed(0);
    leftMotor->setSpeed(0);
    sensorValueLeft = analogRead(sensorPinLeft);
    sensorValueRight = analogRead(sensorPinRight);
    printSensorInputs();
  }

  //Go forward, nonverbose output
  else if (ByteReceived == '2')
  {
    forward();
  }

  //Go forward, verbose output
  else if (ByteReceived == '3')
  {
    forward();
    printSensorInputs();
  }

  //If there is something to be read from the serial port:
  if (Serial.available() > 0)
  {
    //Read in the byte and assign it to ByteReceived
    ByteReceived = Serial.read();
    Serial.println();    // End the line

    //Make sure the motors are stopped
      rightMotor->setSpeed(0);
      leftMotor->setSpeed(0);
    
    //print the information of what the different inputs do.
    if (ByteReceived == '4') {
      printHelp();
    }
    
    //print what each of the outputs are when running on verbose
    else if (ByteReceived == '5') {
      printOutput();
    }
    
    //change the speed (max of 254)
    else if (ByteReceived == '6') 
    {   
      Serial.println("What do you want the new speed to be (0-254)?");
      //Wait until you have another input
      while (!(Serial.available())) {
        delay(20);
      }
      
      //Set the input to an int
      ByteReceived = Serial.parseInt();
      newSpeed = int(ByteReceived);
      
      //Set the motor speed so it is within the legal bounds.
      motorSpeed = newSpeed % 255;
    }
    
    //change bound for what constitutes being on the line (max 899)
    else if (ByteReceived == '7') 
    {
      Serial.println("What do you want the new bound to be (0-899)?");
      //Wait until there's another input
      while (!(Serial.available())) {
        delay(20);
      }
      
      //Get the int that was sent via serial
      ByteReceived = Serial.parseInt();
      Serial.println();
      newBound = int(ByteReceived);
      
      //Change the bound, maxing at 899.
      bound = newBound % 900;
    }
  }
 
  delay(20);  //delay so as not to have the arduino run at its full speed (there is no point)
}

//The function that the arduino will run when going "forward"
void forward() {
  //Get the sensor values
  sensorValueLeft = analogRead(sensorPinLeft);
  sensorValueRight = analogRead(sensorPinRight);

  //Check to see if both of the values are greater than the bound (if the follower is going over the finish line), as defined by "bound"
  if (sensorValueLeft > bound && sensorValueRight > bound) {
    //Have both go the normal motor speed and in the forward direction
    speedLeft = motorSpeed;
    speedRight = motorSpeed;
    rightMotor->run(FORWARD);
    leftMotor->run(FORWARD);
  }
  
  //Check to see if only the right sensor is over the tape, as defined by "bound"
  else if (bound < sensorValueRight) {
    //Have the left motor go normal speed and the right motor go at 1/4 the speed in reverse to allow for better turning radius
    speedLeft = motorSpeed;
    speedRight = -motorSpeed / 4;
    rightMotor->run(BACKWARD);
    leftMotor->run(FORWARD);
  }
  
  //Check to see if only the left sensor is over the tape, as defined by "bound"
  else if (bound < sensorValueLeft) {
    //Have the right motor go normal speed and the left motor go at 1/4 the speed in reverse to allow for better turning radius
    speedLeft = -motorSpeed / 4;
    speedRight = motorSpeed;
    leftMotor->run(BACKWARD);
    leftMotor->run(FORWARD);
  }
  
  //If neither sensor is over the tape, as defined by "bound"
  else {
    //Have both motors go the normal speed in the forward direction
    speedLeft = motorSpeed;
    speedRight = motorSpeed;
    rightMotor->run(FORWARD);
    leftMotor->run(FORWARD);
  }
  
  //Set the motor speeds; the absolute value is taken so the speeds are valid speeds.
  rightMotor->setSpeed(abs(speedRight));
  leftMotor->setSpeed(abs(speedLeft));
}

//Print the help key
void printHelp() {
  Serial.println("The key for changing how the line follower works:");
  Serial.println("0: Stay still, nonverbose output.");
  Serial.println("1: Stay still, verbose output.");
  Serial.println("2: Go forward, nonverbose output.");
  Serial.println("3: Go forward, verbose output.");
  Serial.println("4: Print the help log (this) out again.");
  Serial.println("5: Print which values are in which columns.");
  Serial.println("6: Change the motor speed.");
  Serial.println("7: Change what the bound is for being considered on the line.");
  
}


//Print what gets printed where
void printOutput(){
  Serial.println("LeftSensor \tRightSensor \tleftMotor \trightMotor");
}


//The function to print the sensor values and motor speeds (kept it in here so we aren't always printing).
void printSensorInputs() {
  Serial.print(sensorValueLeft);
  Serial.print("\t\t");
  Serial.print(sensorValueRight);
  Serial.print("\t\t");
  Serial.print(speedLeft);
  Serial.print("\t\t");
  Serial.print(speedRight);
  Serial.println("");
}
