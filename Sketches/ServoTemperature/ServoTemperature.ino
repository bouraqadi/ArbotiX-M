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

void setup() {
  Serial.begin(9600);
  Serial.println("");   
  Serial.println("#### Serial Communication Established.");
  checkVoltage();
}

unsigned long LastDisplayTime = 0;
const unsigned long millisecondsBetweenDisplay = 2000;

void loop() {
  unsigned long currentTime = millis();
  if(currentTime - LastDisplayTime < millisecondsBetweenDisplay){
    return;
  }
  displayAllServoTemperatures();
  LastDisplayTime = millis(); 
}


void displayAllServoTemperatures(){
  Serial.println("#### Temperature in degrees Celsius");
  for(int id = 1; id <= MaxServoID ; id++){
    displayTemperatureForServo(id);
  }
}

void displayTemperatureForServo(int servoID){
    float currentTemperature = dxlGetTemperature(servoID);
    if(currentTemperature == -1){
      Serial.print("- No servo found with ID: ");
      Serial.println(servoID);
      return;
    }
    float maxTemperature = dxlGetTempLimit(servoID);
    Serial.print("- Servo ");
    Serial.print(servoID);
    Serial.print(": ");
    Serial.print(currentTemperature);
    Serial.print(" - Max: ");
    Serial.println(maxTemperature);
    if(currentTemperature/maxTemperature >= 1){
      Serial.println("DANGER: Temperature is way too high!!!");
      Serial.println("Turn off your servo NOW !!!");
      return;
    }
    if(currentTemperature/maxTemperature >= 0.8){
      Serial.println("WARNING: Temperature is high!!!");
      return;
    }
    Serial.println("OK: Temperature is way below the maximum");
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

