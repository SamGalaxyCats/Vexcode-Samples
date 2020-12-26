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
// ---- END VEXCODE CONFIGURED DEVICES ----

#include "vex.h"

using namespace vex;

int main() 
{
  // Initializing Robot Configuration. DO NOT REMOVE!
  //This snippet is so useful for so many things. You can toggle different speeds, increment or decrement values, etc,
  //all without having to worry about that element of the code moving faster than you can press a button.
  vexcodeInit();
  bool canToggle = true;
  bool isToggled = false;
  int timesBPressed = 0;
  int timesAPressed = 0;
  std::string message = ("Off");
  Controller1.Screen.clearScreen(); //clear the brain screen
  while(true)
  {
    //if you can toggle and the toggle button is being pressed,
    if(canToggle && Controller1.ButtonA.pressing())
    {
      canToggle = false; //Set canToggle to false. That way, your toggle can only trigger once per button press.
      timesAPressed++; //Increment timesAPressed.
      //Actual toggling happens here:
      if(isToggled) //if it was toggled,
      {
        isToggled = false; //now it's not.
        message = ("Off");
      }
      else if(!isToggled) //if it wasn't toggled,
      {
        isToggled = true; //now it is.
        message = ("On ");
      }
    }
    else if(canToggle && Controller1.ButtonB.pressing())
    {
      canToggle = false; //Set canToggle to false. That way, your toggle can only trigger once per button press.
      timesBPressed++; //increment timesBPressed.
    }
    //If NONE OF THE BUTTONS that affect canToggle are being pressed, you can toggle again. 
    else if(!canToggle && !(Controller1.ButtonA.pressing() || Controller1.ButtonB.pressing()))
    {
      canToggle = true; //This re-enables toggling once you've let go of the buttons. That way you only toggle once per button press.
    }

    Controller1.Screen.setCursor(1, 1);
    Controller1.Screen.print(message.c_str());
    Controller1.Screen.setCursor(2, 1);
    Controller1.Screen.print("You've pressed A %d times", timesAPressed);
    Controller1.Screen.setCursor(3, 1);
    Controller1.Screen.print("You've pressed B %d times", timesBPressed);
  }
}
