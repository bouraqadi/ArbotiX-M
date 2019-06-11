/*    Copyright 2018 Noury Bouraqadi (https://noury.tech)
 *  
 *    This program is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 */
#include <ax12.h> //ax12 library allows to send DYNAMIXEL commands

// We assume we have 4 servos with consecutive IDs from 1 to 4
const int MaxServoID = 4;
int CurrentServoID = 1;

void setup() {
  Serial.begin(9600);
  Serial.println("");   
  Serial.println("#### Serial Communication Established.");
  checkVoltage();
  turnOffServoLed(DXL_BROADCAST); //Turn OFF all servo LEDs
  turnOnServoLed(CurrentServoID);
}

unsigned long LastDisplayTime = 0;
const unsigned long millisecondsBetweenBlinks = 1000;

void loop() {
  unsigned long currentTime = millis();
  if(currentTime - LastDisplayTime < millisecondsBetweenBlinks){
    return;
  }
  LastDisplayTime = currentTime; 
  turnOffServoLed(CurrentServoID);
  CurrentServoID++;
  if(CurrentServoID > MaxServoID){
    CurrentServoID = 1;
  }
  turnOnServoLed(CurrentServoID);
  delay(100); //Give servos some time to refresh
  displayAllServoLedStatuses();
}

void turnOnServoLed(int servoID){
  dxlSetLED(servoID, true); 
}

void turnOffServoLed(int servoID){
   dxlSetLED(servoID, false);
}

bool isServoLedOn(int servoID){
  return dxlGetLed(servoID);
}

void displayAllServoLedStatuses(){
  Serial.println("---------------");
  for(int id = 1; id <= MaxServoID ; id++){
    Serial.print("LED for servo ");
    Serial.print(id);
    Serial.print(" is: ");
    if(isServoLedOn(id)){
      Serial.println("ON");
    }else{
      Serial.println("OFF");
    }
  }
}

void checkVoltage(){
  //ServoIDs is optional if servos IDs are consecutive starting at 1
  float voltage = dxlGetSystemVoltage(MaxServoID); 
  Serial.print ("##### System Voltage: ");
  Serial.print (voltage);
  Serial.println (" volts.");
  if (voltage >= 10.0){
    Serial.println("ALL GOOD: Voltage is above 10 V");
    return;
  }
  Serial.println("PROBLEM: Voltage level below minimum (10 V)");
  Serial.println("DANGER if you are using LiPO batteries!!!");
  Serial.println("Program stopped");
  while(true); //Infinite loop to avoid executing instructions that require more battery power
}

