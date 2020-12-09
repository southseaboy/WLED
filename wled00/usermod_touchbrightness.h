//
//  usermod_touchbrightness.h
//  github.com/aircoookie/WLED
//
//  Created by Justin Kühner on 14.09.2020.
//  Copyright © 2020 NeariX. All rights reserved.
//  https://github.com/NeariX67/
//  Discord: @NeariX#4799
//
//  modified by Chris Rich on 20.11.2020
//  to include 2nd touch pin for preset cycling
//  unsure of JK's license conditions so will not publish this
//

#pragma once

#include "wled.h"

#define threshold 40                    //Increase value if touches falsely accur. Decrease value if actual touches are not recognized
#define touchPin0 T0                     //T0 = D4 / GPIO4
#define touchPin1 T1                     //T1 = GPIO0 
//Define the 5 brightness levels
//Long press to turn off / on
#define brightness1 51
#define brightness2 102
#define brightness3 153
#define brightness4 204
#define brightness5 255


#ifdef ESP32


class TouchBrightnessControl : public Usermod {
  private:
    unsigned long lastTime = 0;         //Interval

    unsigned long lastTouch0 = 0;        //Timestamp of last Touch0
    unsigned long lastRelease0 = 0;      //Timestamp of last Touch0 release
    boolean released0 = true;            //current Touch0 state (touched/released)
    uint16_t touchReading0 = 0;          //sensor reading, maybe use uint8_t???
    uint16_t touchDuration0 = 0;         //duration of last touch0

    unsigned long lastTouch1 = 0;        //Timestamp of last Touch1
    unsigned long lastRelease1 = 0;      //Timestamp of last Touch1 release
    boolean released1 = true;            //current Touch1 state (touched/released)
    uint16_t touchReading1 = 0;          //sensor reading, maybe use uint8_t???
    uint16_t touchDuration1 = 0;         //duration of last touch1
 
    uint16_t presetIndex = 0;
 
  public:
  
    void setup() {
      lastTouch0 = millis();
      lastRelease0 = millis();
      lastTime = millis();
      lastTouch1 = millis();
      lastRelease1 = millis();
    }

    void loop() {
      if (millis() - lastTime >= 50) {                           //Check every 50ms if a touch occurs
        lastTime = millis();
        touchReading0 = touchRead(touchPin0);                      //Read touch sensor on pin T0 (GPIO4 / D4)
        touchReading1 = touchRead(touchPin1);                      //Read touch sensor on pin T1 (GPIO0 )

        if(touchReading0 < threshold && released0) {               //Touch started
          released0 = false;
          lastTouch0 = millis();
        }
        else if(touchReading0 >= threshold && !released0) {        //Touch released
          released0 = true;
          lastRelease0 = millis();
          touchDuration0 = lastRelease0 - lastTouch0;               //Calculate duration
        }

        if(touchReading1 < threshold && released1) {               //Touch started
          released1 = false;
          lastTouch1 = millis();
        }
        else if(touchReading1 >= threshold && !released1) {        //Touch released
          released1 = true;
          lastRelease1 = millis();
          touchDuration1 = lastRelease1 - lastTouch1;               //Calculate duration
        }

        //Serial.println(touchDuration);

        // Toucpin 0 - brightness and on/off toggle
        if(touchDuration0 >= 800 && released0) {                   //Toggle power if button press is longer than 800ms
          touchDuration0 = 0;                                     //Reset touch duration to avoid multiple actions on same touch
          toggleOnOff();
          colorUpdated(2);                                       //Refresh values
        }
        else if(touchDuration0 >= 100 && released0) {              //Switch to next brightness if touch is between 100 and 800ms
          touchDuration0 = 0;                                     //Reset touch duration to avoid multiple actions on same touch
          if(bri < brightness1) {
            bri = brightness1;
          } else if(bri >= brightness1 && bri < brightness2) {
            bri = brightness2;
          } else if(bri >= brightness2 && bri < brightness3) {
            bri = brightness3;
          } else if(bri >= brightness3 && bri < brightness4) {
            bri = brightness4;
          } else if(bri >= brightness4 && bri < brightness5) {
            bri = brightness5;
          } else if(bri >= brightness5) {
            bri = brightness1;
          }
          colorUpdated(2);                                       //Refresh values
        }
        // Touchpin 1 - cycles presets
        if(touchDuration1 >= 100 && released1) {              //Switch to next preset if touch is > 100ms
          touchDuration1 = 0;

          presetIndex ++ ;
           if(presetIndex > 8) {
             presetIndex = 1;
           }
          applyPreset(presetIndex);
          colorUpdated(2);
        }  

      }
    }
};

#endif