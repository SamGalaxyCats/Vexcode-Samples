#ifndef FUNCTION_LIBRARY_H //If the function library isn't defined,
#define FUNCTION_LIBRARY_H //Define it:

class Robot
{
  public:
    void driveDistance(double ticks, double degreesPerSecond); //Look in the function-library.cpp file to see what these do!
    void driveDistance(double ticks, double degreesPerSecond, bool waitForComplete, bool doCoast);
    void driveDistance(double ticks, double degreesPerSecond, double timeLimit);
    void backOut(double intakeSpeed, double degreesPerSecond, double ticks);
    void turnDegrees(double degreees, double degreesPerSecond);
    void turnDegrees(double degreees, double degreesPerSecond, bool doCoast);
    void turnLeft(double degreees, double degreesPerSecond); 
    void turnRight(double degreees, double degreesPerSecond);
    void turnOnlyLeft(double degreees, double degreesPerSecond); //Not each function is used in the main. 
    void turnOnlyRight(double degreees, double degreesPerSecond);
    void turnOnlyLeftBack(double degreees, double degreesPerSecond);
    void turnOnlyRightBack(double degreees, double degreesPerSecond);
    void turnDistance(double ticksL, double speedL, double ticksR, double speedR);
    void turnDistance(double ticksL, double speedL, double ticksR, double speedR, bool waitForComplete);
    void turnDistance(double ticksL, double speedL, double ticksR, double speedR, bool waitForComplete, bool doCoast);
    void scoreBall(double objectRange, double timeLimit);
    void scoreBall(double objectRange, double timeLimit, double bufferTime);
    void score2Balls(double objectRange, double timeLimit);
};

#endif 