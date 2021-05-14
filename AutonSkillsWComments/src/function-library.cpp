#include "function-library.h"
#include "vex.h"

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*\
|*^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^*|
|*                                                                                                         */
//driveDistance methods

//Drive ticks at degreesPerSecond dps. By using the spinFor method, we don't have to rely on time, but we also
//don't have to worry about slight discrepancies in motor rotations having a large overall effect.
void Robot::driveDistance(double ticks, double degreesPerSecond)
{
  printf("Pre-Drive Facing: %f\n", accella.rotation()); //Print the measured angle before you drive so we can compare to when we finish.
  bLeftDrive.spinFor(forward, ticks, degrees, degreesPerSecond, dps, false); //Each motor spins forward for ticks degrees at degreesPerSecond degrees per second.
  fLeftDrive.spinFor(forward, ticks, degrees, degreesPerSecond, dps, false); //Negative degreesPerSecond will go backward.
  bRightDrive.spinFor(forward, ticks, degrees, degreesPerSecond, dps, false); //The false means the motors won't wait until one has reached its target to move the others.
  fRightDrive.spinFor(forward, ticks, degrees, degreesPerSecond, dps, true); //This waits until it's done to move on because the other motos have been activated.

  //If the front right motor reaches its target before the others, wait until the others finish.
  while(bLeftDrive.isSpinning() || fLeftDrive.isSpinning() || bRightDrive.isSpinning() || fRightDrive.isSpinning())
  {
    vex::task::sleep(5); //Wait 0.005 seconds between checks, a short enough time to stop on the dot but long enough to keep the brain from burning itself out.
  } 
  //Print positional information, that way if there's some kind of fluke or consistant error we have lots of data to go off of.
  printf("Drive Pos: BL %f | FL %f | BR %f | FR %f\nFacing: %f\n", bLeftDrive.position(degrees), fLeftDrive.position(degrees), bRightDrive.position(degrees), fRightDrive.position(degrees), accella.rotation());
}

//Drive ticks at degreesPerSecond dps with a time limit, so that the robot can't get stuck.
//Say it runs into a wall, or a tower, and it can't go any further. Without some sort of timeout, it will just keep driving into that wall,
//And never move on to the next step. This method prevents that.
void Robot::driveDistance(double ticks, double degreesPerSecond, double timeLimit)
{
  printf("Pre-Drive Facing: %f\n", accella.rotation());
  bLeftDrive.spinFor(forward, ticks, degrees, degreesPerSecond, dps, false);
  fLeftDrive.spinFor(forward, ticks, degrees, degreesPerSecond, dps, false);
  bRightDrive.spinFor(forward, ticks, degrees, degreesPerSecond, dps, false);
  fRightDrive.spinFor(forward, ticks, degrees, degreesPerSecond, dps, false); //Use the loop to make sure we can quit if we run into something.

  double timeStamp = Brain.Timer.value(); //Get the time before the loop starts
  while(!bLeftDrive.isDone() || !fLeftDrive.isDone() || !bRightDrive.isDone() || !fRightDrive.isDone()) //Same logic as the previous method.
  {
    if((Brain.Timer.value() - timeStamp) > timeLimit) //Calculate how much time has passed since you started driving. If it's over time limit, exit the loop.
    {
      printf("Timeout triggered, did we hit a wall?\n"); //Print an alert that the robot never reached its goal and instead ran out of time.
      break; //Exit the loop, skip straight to bLeftDrive.stop();
    }
  }
  bLeftDrive.stop(); //Stop the motors.
  fLeftDrive.stop();
  bRightDrive.stop();
  fRightDrive.stop();
  printf("Drive Pos: BL %f | FL %f | BR %f | FR %f\nFacing: %f\n", bLeftDrive.position(degrees), fLeftDrive.position(degrees), bRightDrive.position(degrees), fRightDrive.position(degrees), accella.rotation()); //Print positional info
}

//Drive ticks at degreesPerSecond dps with the option to move to the next line after this method before it completes
//and the option to stop using coast, if it seems like it's too sudden a stop.
//We never used this one.
void Robot::driveDistance(double ticks, double degreesPerSecond, bool waitForComplete, bool doCoast)
{
  //If you want to brake with coast, set the brakeType of each drive motor to coast. Otherwise, move on.
  if(doCoast)
  {
    bLeftDrive.setStopping(coast);
    fLeftDrive.setStopping(coast);
    bRightDrive.setStopping(coast);
    fRightDrive.setStopping(coast);
  }

  //The last waitForComplete is variable so that if you want to start driving and do something else in the meantime, you can.
  bLeftDrive.spinFor(forward, ticks, degrees, degreesPerSecond, dps, false);
  fLeftDrive.spinFor(forward, ticks, degrees, degreesPerSecond, dps, false);
  bRightDrive.spinFor(forward, ticks, degrees, degreesPerSecond, dps, false);
  fRightDrive.spinFor(forward, ticks, degrees, degreesPerSecond, dps, waitForComplete);

  //Return Stopping to the default brakeType.
  bLeftDrive.setStopping(brake);
  fLeftDrive.setStopping(brake);
  bRightDrive.setStopping(brake);
  fRightDrive.setStopping(brake);
}
/*                                                                                                         *|
|*v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v*|
\*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*\
|*^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^*|
|*                                                                                                         */
//backOut methods.  Used for getting out of towers once you're in one.
//We only have one of these since it was developed when I was testing the 9th (middle) tower, and by that point the other 8 were already written.
//Back out of the tower at degreesPerSecond dps while running the intakes at -intakeSpeed/2 (so I could plug in the intakeSpeed variable from the main)
void Robot::backOut(double intakeSpeed, double degreesPerSecond, double ticks)
{
  lIntake.spin(forward, -intakeSpeed/2, rpm); //Outtake to avoid taking a ball out the tower
  rIntake.spin(forward, -intakeSpeed/2, rpm);
  driveDistance(ticks, degreesPerSecond); //Using a method within a method is completely viable and actually incredibly useful.
  lIntake.stop(); //Stop the intakes.
  rIntake.stop();
}
/*                                                                                                         *|
|*v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v*|
\*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*\
|*^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^*|
|*                                                                                                         */
//turnDegrees methods
//This family of methods uses the inertial sensor to turn in place to face an exact rotation.

//turn until you match the desired accella.rotation() value at degreesPerSecond. Should determine which way you need
//to turn to accomplish that and run the related method. If the robot is already there, it informs you of the redundancy.
void Robot::turnDegrees(double degreees, double degreesPerSecond)
{
  //I'm very bad at lefts and rights, so I had a program determine the shortest way to get where it was going.
  //degrees is already a key word, meaning you can't have a variable with the same name. Hence, degreees.
  if(accella.rotation() < degreees) //If you face left of where you need to go,
  {
    turnRight(degreees, degreesPerSecond); //Turn right.
  }
  else if(accella.rotation() > degreees) //If you're to the right of where you need to face,
  {
    turnLeft(degreees, degreesPerSecond); //Turn left.
  }
  else //If you're already there,
  {
    printf("There was a turn that didn't need to be here.\n"); //Print something to the terminal.
  }
}

//Same thing as above, but with the option to coast instead of break.
//Once again, we did not use this one.
void Robot::turnDegrees(double degreees, double degreesPerSecond, bool doCoast)
{
  //If you decide to brake using coast, set the brakeType of each wheel to coast.
  if(doCoast)
  {
    bLeftDrive.setStopping(coast);
    fLeftDrive.setStopping(coast);
    bRightDrive.setStopping(coast);
    fRightDrive.setStopping(coast);
  }

  if(accella.rotation() < degreees)
  {
    turnRight(degreees, degreesPerSecond); //After it runs that method, it returns to this method.
  }
  else if(accella.rotation() > degreees)
  {
    turnLeft(degreees, degreesPerSecond); //Think of it like layers. It goes down a layer here, then returns to this layer that we're in, reaches the end of this layer, and moves up to the main.
  }
  else
  {
    printf("There was a turn that didn't need to be here.\n");
  }

  //Return Stopping to the default brakeType.
  bLeftDrive.setStopping(brake);
  fLeftDrive.setStopping(brake);
  bRightDrive.setStopping(brake);
  fRightDrive.setStopping(brake);
}

//Left wheels reverse, as degrees need to go down. This method is called by turnDegrees, but can be used on its own.
//Turns left at degreesPerSecond until accella.rotation() no longer exceeds degrees, at which point it stops.
void Robot::turnLeft(double degreees, double degreesPerSecond)
{
  bLeftDrive.spin(forward, -degreesPerSecond, dps); //Instead of spinning for an amount, just have each motor spin.
  fLeftDrive.spin(forward, -degreesPerSecond, dps);
  bRightDrive.spin(forward, degreesPerSecond, dps);
  fRightDrive.spin(forward, degreesPerSecond, dps);

  //The logic behind having it face a direction instead of turning an amount was pretty important.
  //If the robot gets off course or drifts while it drives, and we tell it to turn an amount, the problems amplify and the autonomous fails.
  //If it gets off course and we tell it to face a direction, it has a chance to correct itself, meaning we can mess up one portion of the autonomous without failing the rest.
  while(accella.rotation() > degreees)
  {
    vex::task::sleep(10);
  }
  bLeftDrive.stop(); //When you tell them to spin, you have to tell them to stop, too.
  bRightDrive.stop();
  fRightDrive.stop();
  bRightDrive.stop();

  //Print positional information in case an error occurs so we have data to work with.
  printf("Pos: BL %f | FL %f | BR %f | FR %f\nFacing: %f\n", bLeftDrive.position(degrees), fLeftDrive.position(degrees), bRightDrive.position(degrees), fRightDrive.position(degrees), accella.rotation());
}

//Right wheels reverse, as degrees need to go up. This method is called by turnDegrees, but can be used on its own.
//Turns right at degreesPerSecond until accella.rotation() exceeds degrees, at which point it stops.
void Robot::turnRight(double degreees, double degreesPerSecond)
{
  //Same logic as before, but in reverse.
  bLeftDrive.spin(forward, degreesPerSecond, dps);
  fLeftDrive.spin(forward, degreesPerSecond, dps);
  bRightDrive.spin(forward, -degreesPerSecond, dps);
  fRightDrive.spin(forward, -degreesPerSecond, dps);

  //Another thing to note is that we use accella.rotation() instead of accella.heading().
  //Heading can only ever be 0-360, whereas rotation counts how much you've turned (positive or negative).
  //If you turn until accella.heading() > 361, you will spin infinitely. If you turn until accella.rotation() > 361, you'll be able to stop.
  while(accella.rotation() < degreees)
  {
    vex::task::sleep(10);
  }
  bLeftDrive.stop();
  bRightDrive.stop();
  fRightDrive.stop();
  bRightDrive.stop();
  printf("Pos: BL %f | FL %f | BR %f | FR %f\nFacing: %f\n", bLeftDrive.position(degrees), fLeftDrive.position(degrees), bRightDrive.position(degrees), fRightDrive.position(degrees), accella.rotation());
}

/*                                                                                                         *|
|*v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v*|
\*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*\
|*^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^*|
|*                                                                                                         */
//TurnOnly methods
//Used to turn only one set of wheels, for when we aren't turning in place.  
//Only used to make the first two turns, but has potential to use for precision scoring.

//TurnOnlyLeft
void Robot::turnOnlyLeft(double degreees, double degreesPerSecond)
{
  bLeftDrive.spin(forward, degreesPerSecond, dps); //Only activate one set of wheels.
  fLeftDrive.spin(forward, degreesPerSecond, dps);

  while(accella.rotation() < degreees)
  {
    vex::task::sleep(10);
  }
  bLeftDrive.stop();
  fLeftDrive.stop();

  //Print positional information.
  printf("Pos: BL %f | FL %f | BR %f | FR %f\nFacing: %f\n", bLeftDrive.position(degrees), fLeftDrive.position(degrees), bRightDrive.position(degrees), fRightDrive.position(degrees), accella.rotation());
}

//TurnOnlyRight
void Robot::turnOnlyRight(double degreees, double degreesPerSecond)
{
  //These can only be used to turn forward.
  bRightDrive.spin(forward, degreesPerSecond, dps);
  fRightDrive.spin(forward, degreesPerSecond, dps);

  while(accella.rotation() > degreees)
  {
    vex::task::sleep(10);
  }
  bRightDrive.stop();
  fRightDrive.stop();

  printf("Pos: BL %f | FL %f | BR %f | FR %f\nFacing: %f\n", bLeftDrive.position(degrees), fLeftDrive.position(degrees), bRightDrive.position(degrees), fRightDrive.position(degrees), accella.rotation());
}

//TurnOnlyLeftBack
void Robot::turnOnlyLeftBack(double degreees, double degreesPerSecond)
{
  //Because going backward changes the direction of the turn, a separate method is needed to go backward.
  bLeftDrive.spin(forward, -degreesPerSecond, dps);
  fLeftDrive.spin(forward, -degreesPerSecond, dps);
  while(accella.rotation() > degreees)
  {
    vex::task::sleep(10);
  }
  bLeftDrive.stop();
  fLeftDrive.stop();

  printf("Pos: BL %f | FL %f | BR %f | FR %f\nFacing: %f\n", bLeftDrive.position(degrees), fLeftDrive.position(degrees), bRightDrive.position(degrees), fRightDrive.position(degrees), accella.rotation());
}

//TurnOnlyRightBack
void Robot::turnOnlyRightBack(double degreees, double degreesPerSecond)
{
  //This is currently the only one of these methods that we use. The rest are just there in case we need them.
  bRightDrive.spin(forward, -degreesPerSecond, dps);
  fRightDrive.spin(forward, -degreesPerSecond, dps);

  while(accella.rotation() < degreees)
  {
    vex::task::sleep(10);
  }
  bRightDrive.stop();
  fRightDrive.stop();

  printf("Pos: BL %f | FL %f | BR %f | FR %f\nFacing: %f\n", bLeftDrive.position(degrees), fLeftDrive.position(degrees), bRightDrive.position(degrees), fRightDrive.position(degrees), accella.rotation());
}
/*                                                                                                         *|
|*v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v*|
\*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/



/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*\
|*^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^*|
|*                                                                                                         */
//turnDistance methods
//useful for turning only one side, turning with speed discrepancies, etc. I highly recommend using a turnDegrees
//method soon afterward to ensure that rotational positioning is maintained. A distance sensor could be useful to do 
//some odometry here.
//These were initially used with the initial turn, but then I wrote turnOnlyRightBack(), which was significantly better in that use case.

//The left motors drive ticksL at speedL while the right motors drive ticksR at speedR.
void Robot::turnDistance(double ticksL, double speedL, double ticksR, double speedR)
{
  //A separate speed for left and right motors. You can do equal and opposite for just turning, set one of them to zero, or make them different.
  bLeftDrive.spinFor(forward, ticksL, degrees, speedL, dps, false);
  fLeftDrive.spinFor(forward, ticksL, degrees, speedL, dps, false);
  bRightDrive.spinFor(forward, ticksR, degrees, speedR, dps, false);
  fRightDrive.spinFor(forward, ticksR, degrees, speedR, dps, false);

  //Wait for all the motors to finish before you exit the code.
  while(bLeftDrive.isSpinning() || fLeftDrive.isSpinning() || bRightDrive.isSpinning() || fRightDrive.isSpinning())
  {
    vex::task::sleep(5);
  } 

  printf("Pos: BL %f | FL %f | BR %f | FR %f\nFacing: %f\n", bLeftDrive.position(degrees), fLeftDrive.position(degrees), bRightDrive.position(degrees), fRightDrive.position(degrees), accella.rotation());
}

void Robot::turnDistance(double ticksL, double speedL, double ticksR, double speedR, bool waitForComplete)
{
  bLeftDrive.spinFor(forward, ticksL, degrees, speedL, dps, false);
  fLeftDrive.spinFor(forward, ticksL, degrees, speedL, dps, false);
  bRightDrive.spinFor(forward, ticksR, degrees, speedR, dps, false);
  fRightDrive.spinFor(forward, ticksR, degrees, speedR, dps, waitForComplete);

  //in the event that the left wheels have to go further or faster than the right AND waitForComplete is true, 
  //wait for them. Uses && instead of || so that if only one condition is false, it exits the loop.
  if((!bLeftDrive.isDone() && !fLeftDrive.isDone()) && waitForComplete)
  {
    while(!bLeftDrive.isDone() && !fLeftDrive.isDone())
    {
      vex::task::sleep(20);
    }
  }

  printf("Pos: BL %f | FL %f | BR %f | FR %f\nFacing: %f\n", bLeftDrive.position(degrees), fLeftDrive.position(degrees), bRightDrive.position(degrees), fRightDrive.position(degrees), accella.rotation());
}

//All of the above turnDistance stuff, but with the option to do coast. I thought it might be necessary to eliminate jerk, but we ultimately had more drift. 
void Robot::turnDistance(double ticksL, double speedL, double ticksR, double speedR, bool waitForComplete, bool doCoast)
{
  if(doCoast)
  {
    bLeftDrive.setStopping(coast);
    fLeftDrive.setStopping(coast);
    bRightDrive.setStopping(coast);
    fRightDrive.setStopping(coast);
  }

  bLeftDrive.spinFor(forward, ticksL, degrees, speedL, dps, false);
  fLeftDrive.spinFor(forward, ticksL, degrees, speedL, dps, false);
  bRightDrive.spinFor(forward, ticksR, degrees, speedR, dps, false);
  fRightDrive.spinFor(forward, ticksR, degrees, speedR, dps, waitForComplete);

  //in the event that the left wheels have to go further or faster than the right AND waitForComplete is true, 
  //wait for them.
  if((!bLeftDrive.isDone() || !fLeftDrive.isDone()) && waitForComplete)
  {
    while(!bLeftDrive.isDone() || !fLeftDrive.isDone())
    {
      vex::task::sleep(20);
    }
  }

  //Return Stopping to the default brakeType.
  bLeftDrive.setStopping(brake);
  fLeftDrive.setStopping(brake);
  bRightDrive.setStopping(brake);
  fRightDrive.setStopping(brake);

  printf("Pos: BL %f | FL %f | BR %f | FR %f\nFacing: %f\n", bLeftDrive.position(degrees), fLeftDrive.position(degrees), bRightDrive.position(degrees), fRightDrive.position(degrees), accella.rotation());
}

/*                                                                                                         *|
|*v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v*|
\*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/



/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*\
|*^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^*|
|*                                                                                                         */
//scoreBall methods
//Once you reach a tower, these are used to score the ball using the distance sensor alongside a preventative timeout.

//Attempt to score a ball for timeLimit seconds.
void Robot::scoreBall(double objectRange, double timeLimit)
{
  bool ballFlag = false; //Set a varaible named ballFlag to false.
  conveyL.spin(forward); //Activate conveyors.
  conveyR.spin(forward);
  double timeStamp = Brain.Timer.value(); //get the time at the start of the action so you can compare it to the later time.

  //In restrospect, it would've been simpler to say:
  //while((!ballFlag || (dist.objectDistance(inches) < objectRange)) && (Brain.Timer.value()-timeStamp > time limit))
  //Same logic but much more concise. Live and learn, I guess.
  while(!ballFlag || (dist.objectDistance(inches) < objectRange)) 
  {
    vex::task::sleep(10);
    //If the distance sensor reads a value that is less than objectRange away, something is passing through the conveyor.
    if(dist.objectDistance(inches) < objectRange)
    {
      ballFlag = true; //It then trips this flag by setting ballFlag to true. Once the object leaves the vision sensor's range, it sees that ballFlag is true and no objects are near, and exits the loop.
    }
    if((Brain.Timer.value()-timeStamp) > timeLimit) //If you go over the time limit and nothing happens, you likely didn't get a ball.
    {
      break; //Exit the loop so you aren't stuck at the tower.
    }
  }
  ballFlag = false; //Untrip the flag.
  vex::task::sleep(200); //Score for an extra 0.2 seconds because the ball may have cleared the distance sensor but not made it into the tower.
  conveyL.stop(); //Stop the conveyor.
  conveyR.stop();
}

//scoreBall but with variable buffer time after you score the ball.
void Robot::scoreBall(double objectRange, double timeLimit, double bufferTime)
{
  bool ballFlag = false;
  conveyL.spin(forward);
  conveyR.spin(forward);
  double timeStamp = Brain.Timer.value();
  while(!ballFlag || (dist.objectDistance(inches) < objectRange))
  {
    vex::task::sleep(10);
    if(dist.objectDistance(inches) < objectRange)
    {
      ballFlag = true;
    }
    if((Brain.Timer.value()-timeStamp) > timeLimit)
    {
      break;
    }
  }
  ballFlag = false;
  vex::task::sleep(bufferTime); //This is the only difference between this and the previous program. Sometimes that extra 0.1s is all it takes, but I didn't want to mess with tried-and-true towers.
  conveyL.stop();
  conveyR.stop();
}

//Score 2 balls, only used in the 2nd tower, but worked amazingly well.
void Robot::score2Balls(double objectRange, double timeLimit)
{
  bool ballFlag = false;
  conveyL.spin(forward);
  conveyR.spin(forward);
  double timeStamp = Brain.Timer.value();
  while(!ballFlag || (dist.objectDistance(inches) < objectRange))
  {
    vex::task::sleep(10);
    if(dist.objectDistance(inches) < objectRange)
    {
      ballFlag = true;
    }
    if((Brain.Timer.value()-timeStamp) > timeLimit)
    {
      break;
    }
  }
  ballFlag = false; //After you've scored one ball, reset the ballFlag so you can tell when you've scored another.
  while(!ballFlag || (dist.objectDistance(inches) < objectRange))
  {
    vex::task::sleep(10);
    if(dist.objectDistance(inches) < objectRange)
    {
      ballFlag = true;
    }
    if((Brain.Timer.value()-timeStamp) > timeLimit)
    {
      break;
    }
  }
  ballFlag = false;
  vex::task::sleep(200);
  conveyL.stop();
  conveyR.stop();
}
/*                                                                                                         *|
|*v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v-v*|
\*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/