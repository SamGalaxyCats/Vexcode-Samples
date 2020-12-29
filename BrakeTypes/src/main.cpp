/*----------------------------------------------------------------------------*/
/*                                                                            */
/*    Module:       main.cpp                                                  */
/*    Author:       C:\Users\sambo                                            */
/*    Created:      Sat Dec 26 2020                                           */
/*    Description:  V5 project                                                */
/*                                                                            */
/*----------------------------------------------------------------------------*/

// ---- START VEXCODE CONFIGURED DEVICES ----
// Robot Configuration:
// [Name]               [Type]        [Port(s)]
// Controller1          controller                    
// bLeftDrive           motor         3               
// bRightDrive          motor         4               
// fRightDrive          motor         17              
// fLeftDrive           motor         18              
// ---- END VEXCODE CONFIGURED DEVICES ----

#include "vex.h"

using namespace vex;

void changeBrakeType(vex::brakeType type)
{
  bLeftDrive.setStopping(type);
  bRightDrive.setStopping(type);
  fLeftDrive.setStopping(type);
  fRightDrive.setStopping(type);
}

int main() 
{
  // Initializing Robot Configuration. DO NOT REMOVE!
  vexcodeInit();
  bLeftDrive.setVelocity(95, percent);
  bRightDrive.setVelocity(95, percent);
  fLeftDrive.setVelocity(95, percent);
  fRightDrive.setVelocity(95, percent);
  //Showing off the braketypes. You'll need a little bit of space for this one, and you might need to reconfigure the motors here.
  //keep the names, but change the ports and reversedness all you want.
  //As per all my code, bLeft = back left, fRight = front right.

  //Side note - I got lazy and made a method to avoid having to write "bLeftDrive.setStopping(brake);" and its variants 500 times,
  //but you don't have to make a method for this. Typically, you want to set the brakeTypes at the start of the program before any while loops.
  //Also, if you use setStopping, that motor will always stop with the brakeType you choose until you change it again. 
  //lines like "bRightDrive.stop(hold);" will ignore the set brakeType without changing it, so if you only want to hold once and brake the rest
  //of the time, this is the solution.

  //Fair warning, it'll go forward a bunch, change brakeType, go back the same distance, change brakeType, and go forward again.
  waitUntil(Controller1.ButtonA.pressing());
  changeBrakeType(brake); //Set the brake type of all motors to brake.
  //Brake is the default brakeType. It's kind of like slamming a car brake, where you'll come to a complete and sudden stop.
  //AFTER the motor stops, the brain CUTS POWER to the stopped motor.
  //Once the robot has stopped, try to push it around. 
  bLeftDrive.spinFor(forward, 360, degrees, false);
  fLeftDrive.spinFor(forward, 360, degrees, false);
  fRightDrive.spinFor(forward, 360, degrees, false);
  bRightDrive.spinFor(forward, 360, degrees, true);
  waitUntil(Controller1.ButtonA.pressing());
  changeBrakeType(hold); //Set the brake type of all motors to hold.
  //Hold is perfect for lifts, and pretty much only lifts. When a motor stops with hold, it's not going anywhere.
  //It uses an automated PID loop to stay in the exact spot it stopped. If you use this on a lift, turn your robot off before you try to reset it,
  //because you have to overpower a motor to move it if you don't, and overpowering a motor is horrible for any motor, wire, or port. 
  //(Note: the motor will obviously move when the code says to, because when it's not stopped brakeTypes don't apply.)
  //Try pushing the robot once it stops. 
  bLeftDrive.spinFor(forward, -360, degrees, false);
  fLeftDrive.spinFor(forward, -360, degrees, false);
  fRightDrive.spinFor(forward, -360, degrees, false);
  bRightDrive.spinFor(forward, -360, degrees, true);
  waitUntil(Controller1.ButtonA.pressing());
  changeBrakeType(coast);//Set the brake type of all motors to coast.
  //I'm not sure what coast could be used for yet, but maybe someone else will have a reason to use it.
  //When a motor stops with brakeType coast, it just cuts power to the motor. It's kind of like coasting on a bike.
  //Try this one on a smooth surface and just watch. It's the smoothest possible stop, 
  //but also the one that puts you the furthest from where you stopped. If you use it on a lift, the lift will probably collapse every time it stops.
  bLeftDrive.spinFor(forward, 360, degrees, false);
  fLeftDrive.spinFor(forward, 360, degrees, false);
  fRightDrive.spinFor(forward, 360, degrees, false);
  bRightDrive.spinFor(forward, 360, degrees, true);
  waitUntil(Controller1.ButtonA.pressing());
}
