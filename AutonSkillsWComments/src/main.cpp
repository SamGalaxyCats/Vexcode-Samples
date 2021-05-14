/*----------------------------------------------------------------------------*/
/*                                                                            */
/*    Module:       main.cpp                                                  */
/*    Author:       VEX                                                       */
/*    Created:      Thu Sep 26 2019                                           */
/*    Description:  Competition Template                                      */
/*                                                                            */
/*----------------------------------------------------------------------------*/

// ---- START VEXCODE CONFIGURED DEVICES ----
// Robot Configuration:
// [Name]               [Type]        [Port(s)]
// Controller1          controller                    
// bLeftDrive           motor         11              
// bRightDrive          motor         19              
// fLeftDrive           motor         1               
// fRightDrive          motor         9               
// lIntake              motor         15              
// rIntake              motor         14              
// conveyL              motor         3               
// conveyR              motor         5               
// accella              inertial      18              
// dist                 distance      13              
// ---- END VEXCODE CONFIGURED DEVICES ----

#include "vex.h"
#include "function-library.h"

using namespace vex;

// A global instance of competition
competition Competition;

// define your global instances of motors and other devices here

/*---------------------------------------------------------------------------*/
/*                          Pre-Autonomous Functions                         */
/*                                                                           */
/*  You may want to perform some actions before the competition starts.      */
/*  Do them in the following function.  You must return from this function   */
/*  or the autonomous and usercontrol tasks will not be started.  This       */
/*  function is only called once after the V5 has been powered on and        */
/*  not every time that the robot is disabled.                               */
/*---------------------------------------------------------------------------*/

void pre_auton(void) 
{
  // Initializing Robot Configuration. DO NOT REMOVE!
  vexcodeInit();
  accella.calibrate(); //Calibrate the inertial sensor. Must happen before the program runs. Don't move the robot once you hit run.
  rIntake.setBrake(brake);
  lIntake.setBrake(brake);
  conveyL.setBrake(brake);
  conveyR.setBrake(brake);
  bLeftDrive.setStopping(brake);
  fLeftDrive.setStopping(brake);
  bRightDrive.setStopping(brake);
  fRightDrive.setStopping(brake);
  conveyL.setVelocity(450, rpm); //Give the conveyors a default velocity. 450rpm = 75% max speed
  conveyR.setVelocity(450, rpm);
  bLeftDrive.setRotation(0, degrees);
  fLeftDrive.setRotation(0, degrees);
  bRightDrive.setRotation(0, degrees);
  fRightDrive.setRotation(0, degrees);
  // All activities that occur before the competition starts
  // Example: clearing encoders, setting servo positions, ...
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                              Autonomous Task                              */
/*                                                                           */
/*  This task is used to control your robot during the autonomous phase of   */
/*  a VEX Competition.                                                       */
/*                                                                           */
/*  You must modify the code to add your own robot specific commands here.   */
/*---------------------------------------------------------------------------*/

void autonomous(void) 
{
  Robot driveSystem; //Create an object to run Robot methods from function-library.h
  double intakeSpeed = 540; 
  printf("Run start\n"); //At the start of the program, print this to the terminal so we can differentiate between runs.
  while(accella.isCalibrating()) //Don't do anything until the inertial sensor finishes calibrating.
  {
    //Calibrating the inertial sensor is absolutely necessary to do at the start of each run.
    //if the robot is moved during calibration, it will calibrate incorrectly and make very obviously wrong turns.
    //Calibration begins at the start of the program during the preauton. When you run skills autonomous without a competition switch,
    //You will lose ~5 seconds because of this necessary sleep. No time will be lost when this is run through a competition switch.
    vex::task::sleep(10); //Check if it's done every 0.01 seconds.
  }

  //Score 1st tower
  driveSystem.turnOnlyRightBack(21.5, 300);
  lIntake.spin(forward, -90, percent); //Run intakes in reverse to open the plexiglass ramp
  rIntake.spin(forward, -90, percent); 
  conveyL.spin(forward, 90, percent); //Run conveyor to open conveyor ramp and score the ball sitting ontop of it.
  conveyR.spin(forward, 90, percent);
  vex::task::sleep(1000); //It takes 1 second to flip out.
  conveyL.stop(); //Stop running everything after that second.
  conveyR.stop();
  lIntake.stop();
  rIntake.stop();
  printf("\n2nd tower\n"); //Print that you have started the 2nd tower to the terminal so that the terminal is easy to read.

  //We had to add a second turn after a very brief distance to pick up the next ball, or we would be turning into the tower.
  driveSystem.driveDistance(100, 250); //Move a skosh forward
  driveSystem.turnOnlyRightBack(30, 300); //Turn slightly.

  //Turn to next ball
  lIntake.spin(forward, intakeSpeed, rpm); //Turn intakes on
  rIntake.spin(forward, intakeSpeed, rpm);
  driveSystem.driveDistance(500, 600); //Drive to ball
  conveyL.spin(forward, 300, rpm); //Activate conveyor at 50% speed to get the ball up a little bit
  conveyR.spin(forward, 300, rpm); //This brief pause makes room for the second ball.
  vex::task::sleep(300); //0.3 seconds of 50% speed intakes is enough.
  conveyL.stop(); //Stop conveyors, leave intakes on.
  conveyR.stop();

  //get ANOTHER ball
  //driveSystem.turnDegrees(34, 150); //We took out this turn because most of the time the robot was already lined up.
  driveSystem.driveDistance(750, 600); //Drive to the next ball.
  conveyL.spin(forward, 300, rpm); //Once again, pause to intake the ball a little.
  conveyR.spin(forward, 300, rpm); //That helps the ball get all the way into the intakes.
  vex::task::sleep(250); //Only 0.25 seconds this time.
  conveyL.stop(); //Stop the conveyors.
  conveyR.stop();

  //Go to 2nd tower and score ball
  driveSystem.turnDegrees(-78, 300); //turn to face the tower
  driveSystem.driveDistance(750, 500, 2); //Drive to the tower.
  lIntake.stop(); //Stop intaking, we don't want the bottom ball.
  rIntake.stop();
  driveSystem.score2Balls(6, 2.5); //Score both balls. 

  //back out
  lIntake.spin(forward, -intakeSpeed/2, rpm); //Outtake at half default speed so that you don't take any balls with you.
  rIntake.spin(forward, -intakeSpeed/2, rpm);
  driveSystem.driveDistance(-200, 300); //Back out slowly.
  lIntake.stop(); //Stop outtaking
  rIntake.stop();

  //go for 3rd tower
  printf("\n3rd tower\n"); //Print that we're going for the 3rd tower.
  driveSystem.turnDegrees(-10, 300); //turn to face next ball
  lIntake.spin(forward, intakeSpeed, rpm); //intake
  rIntake.spin(forward, intakeSpeed, rpm);
  driveSystem.driveDistance(950, 600); //get ball
  driveSystem.turnDegrees(-35, 300); //face tower
  driveSystem.driveDistance(400, 400, 1.5); //Go to tower
  lIntake.stop(); //Stop intaking
  rIntake.stop();
  driveSystem.scoreBall(6, 2, 300); //Score the third tower with variable buffer time. We weren't always there long enough.

  //back out
  lIntake.spin(forward, -intakeSpeed/4, rpm); //This process could theoretically be its own method, since it's the same steps with different numbers each time.
  rIntake.spin(forward, -intakeSpeed/4, rpm);
  driveSystem.driveDistance(-350, 600);
  lIntake.stop();
  rIntake.stop();

  //4th tower
  printf("\n4th tower\n");
  driveSystem.turnDegrees(82, 250); //face next ball
  lIntake.spin(forward, intakeSpeed, rpm); //intake
  rIntake.spin(forward, intakeSpeed, rpm);
  driveSystem.driveDistance(1050, 600); //get ball
  driveSystem.turnDegrees(10, 300); //face tower
  driveSystem.driveDistance(300, 600, 2); //go to tower
  lIntake.stop(); //Stop intaking
  rIntake.stop();
  driveSystem.scoreBall(6, 2, 300); //Score at the 4th tower

  //back out
  lIntake.spin(forward, -intakeSpeed/4, rpm);
  rIntake.spin(forward, -intakeSpeed/4, rpm);
  driveSystem.driveDistance(-225, 300); 
  lIntake.stop();
  rIntake.stop();

  //5th tower
  printf("\n5th tower\n");
  driveSystem.turnDegrees(93, 300); //face ball
  lIntake.spin(forward, intakeSpeed, rpm);
  rIntake.spin(forward, intakeSpeed, rpm);
  driveSystem.driveDistance(1000, 600); //get ball
  driveSystem.turnDegrees(50, 300); //face tower
  driveSystem.driveDistance(600, 600, 2); //go to tower
  lIntake.stop();
  rIntake.stop();
  driveSystem.scoreBall(6, 2, 300); //score

  //back out
  lIntake.spin(forward, -intakeSpeed/4, rpm);
  rIntake.spin(forward, -intakeSpeed/4, rpm);
  driveSystem.driveDistance(-400, 600); 
  lIntake.stop();
  rIntake.stop();

  //6th tower
  printf("\n6th tower\n");
  driveSystem.turnDegrees(199.5, 300); //face ball (200 was too much and 199 wasn't enough)
  lIntake.spin(forward, intakeSpeed, rpm);
  rIntake.spin(forward, intakeSpeed, rpm);
  driveSystem.driveDistance(1300, 600); //get ball
  driveSystem.turnDegrees(92, 300); //face tower
  driveSystem.driveDistance(850, 600, 2); //go to tower
  lIntake.stop();
  rIntake.stop();
  driveSystem.scoreBall(6, 2); //score

  //back out
  lIntake.spin(forward, -intakeSpeed/2, rpm);
  rIntake.spin(forward, -intakeSpeed/2, rpm);
  driveSystem.driveDistance(-175, 300); //back out
  lIntake.stop();
  rIntake.stop();

  //7th tower
  printf("\n7th tower\n");
  driveSystem.turnDegrees(170.5, 300); //turn to face next ball
  lIntake.spin(forward, intakeSpeed, rpm);
  rIntake.spin(forward, intakeSpeed, rpm);
  driveSystem.driveDistance(1050, 650); //get ball
  driveSystem.turnDegrees(140, 300); //face tower
  driveSystem.driveDistance(500, 400, 1); //go to tower
  lIntake.stop();
  rIntake.stop();
  driveSystem.scoreBall(6, 2, 300); //score

  //back out
  lIntake.spin(forward, -intakeSpeed/4, rpm);
  rIntake.spin(forward, -intakeSpeed/4, rpm);
  driveSystem.driveDistance(-450, 600);
  lIntake.stop();
  rIntake.stop();

  //8th tower
  printf("\n8th tower\n"); 
  driveSystem.turnDegrees(263, 300); //face ball
  lIntake.spin(forward, intakeSpeed, rpm);
  rIntake.spin(forward, intakeSpeed, rpm);
  driveSystem.driveDistance(990, 600); //get ball
  driveSystem.turnDegrees(190, 300); //face tower
  driveSystem.driveDistance(250, 400, 1.5); //go to tower
  lIntake.stop();
  rIntake.stop();
  driveSystem.scoreBall(6, 2, 300); //score

  //The fabled 9th tower (middle tower)
  printf("\n9th tower\n"); //Because of the aforementioned calibration time loss, we didn't always get to test this. It seems inconsistent with whether it scores.
  driveSystem.backOut(intakeSpeed, 300, -300); //back out
  driveSystem.turnDegrees(367, 300); //face ball
  lIntake.spin(forward, intakeSpeed, rpm);
  rIntake.spin(forward, intakeSpeed, rpm);
  driveSystem.driveDistance(450, 600); //get ball
  conveyL.spin(forward); //pull the ball up
  conveyR.spin(forward); //that way we can outtake later.
  vex::task::sleep(400);
  conveyL.stop(); //Don't run the conveyor for too long, though.
  conveyR.stop();

  //Poke out
  driveSystem.turnDegrees(357, 150); //face tower
  lIntake.spin(forward, -intakeSpeed, rpm); //outtake to ensure that we poke out
  rIntake.spin(forward, -intakeSpeed, rpm);
  driveSystem.driveDistance(500, 600, 1.2); //get to the tower
  bRightDrive.spin(forward, -25, percent); //Angle yourself to line up with the tower
  fRightDrive.spin(forward, -25, percent);
  driveSystem.scoreBall(6, 2); //Try to score the ball for 2 seconds
  bRightDrive.stop(); //Brake, don't break.
  fRightDrive.stop();
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                              User Control Task                            */
/*                                                                           */
/*  This task is used to control your robot during the user control phase of */
/*  a VEX Competition.                                                       */
/*                                                                           */
/*  You must modify the code to add your own robot specific commands here.   */
/*---------------------------------------------------------------------------*/

void usercontrol(void) {
  // User control code here, inside the loop
  while (1) {
    // This is the main execution loop for the user control program.
    // Each time through the loop your program should update motor + servo
    // values based on feedback from the joysticks.

    // ........................................................................
    // Insert user code here. This is where you use the joystick values to
    // update your motors, etc.
    // ........................................................................

    wait(20, msec); // Sleep the task for a short amount of time to
                    // prevent wasted resources.
  }
}

//
// Main will set up the competition functions and callbacks.
//
int main() {
  // Set up callbacks for autonomous and driver control periods.
  Competition.autonomous(autonomous);
  Competition.drivercontrol(usercontrol);

  // Run the pre-autonomous function.
  pre_auton();

  // Prevent main from exiting with an infinite loop.
  while (true) {
    wait(100, msec);
  }
}
