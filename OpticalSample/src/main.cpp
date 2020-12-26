/*----------------------------------------------------------------------------*/
/*                                                                            */
/*    Module:       main.cpp                                                  */
/*    Author:       C:\Users\sambo                                            */
/*    Created:      Wed Dec 23 2020                                           */
/*    Description:  V5 project                                                */
/*                                                                            */
/*----------------------------------------------------------------------------*/

// ---- START VEXCODE CONFIGURED DEVICES ----
// Robot Configuration:
// [Name]               [Type]        [Port(s)]
// eye                  optical       1               
// Controller1          controller                    
// ---- END VEXCODE CONFIGURED DEVICES ----

#include "vex.h"

using namespace vex;

int main() 
{
  // Initializing Robot Configuration. DO NOT REMOVE!
  vexcodeInit();
  //This code shows off some of the functions of the optical sensor. 
  //For those who haven't, I highly recommend taking the lid off the Brain, opening the Device Info menu (you can go back by 
  //pressing the power button once), picking the optical sensor, and just waving some objects in front of it.  You can watch it detect stuff,
  //set the mode (I have yet to explore gesture mode, but from what I've read it doesn't work), and press the bottom left corner to adjust the LED brightness.

  bool canToggle = true; //for use later in the code.
  int currentLightPercent = 0; //to keep track of how bright the LED is set.
  while(true)
  {
    if(eye.isNearObject())
    {
      Brain.Screen.setCursor(1, 1);
      Brain.Screen.print("I see an object! Its color value is ");
      Brain.Screen.setPenColor(eye.color()); //Sets the pen color to the color the optical sensor detects.
      Brain.Screen.print(eye.color());
    }
    else
    {
      Brain.Screen.setCursor(1, 1);
      Brain.Screen.print("I'm not seeing anything nearby.                  ");
    }
    Brain.Screen.setPenColor(white);
    Brain.Screen.setCursor(2, 1);
    Brain.Screen.print("The ambient light levels are ");
    Brain.Screen.print(eye.brightness()); // As far as I can tell, the built-in LED affects this value, but the value itself is TINY.
    Brain.Screen.setCursor(3, 1);
    Brain.Screen.print("The hue is ");
    Brain.Screen.print(eye.hue()); 

    //increment or decrement the light level by 10% by pressing buttons A or B respectively. This logic makes you press it once per value change.
    if(canToggle && Controller1.ButtonA.pressing())
    {
      canToggle = false;
      currentLightPercent += 10;
    }
    else if(canToggle && Controller1.ButtonB.pressing())
    {
      canToggle = false;
      currentLightPercent -= 10;
    }
    else if(!canToggle && !(Controller1.ButtonA.pressing() || Controller1.ButtonB.pressing()))
    {
      canToggle = true;
    }

    //make sure to not exceed 100% or go below 0%.
    if(currentLightPercent > 100)
    {
      //PSA: When I first tested the optical sensor, I left it on 100% brightness the whole time, and the sensor got hot, 
      //so for future reference, don't leave the LED on at 100% for extended periods of time.
      currentLightPercent = 100; 
    }
    else if(currentLightPercent <= 0)
    {
      currentLightPercent = 0;
      eye.setLight(ledState::off); //turns the LED off if the brightness is 0% anyway.
    }
    else
    {
      eye.setLight(ledState::on);
    }
    eye.setLightPower(currentLightPercent, percent);
  }
}