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

// We assume we have a servo with ID 1
const int ServoID = 1;

void setup() {
  Serial.begin(9600);
  Serial.println("");   
  Serial.println("#### Serial Communication Established.");
  checkVoltage();
  Serial.println("#### WARNING: Ensure servo with ID 1 can freely rotate");
  Serial.println("Type 'R' once you are ready"); 
  while(!isReady());
  Serial.println("#### Initializing servo");
  ensureServoInWheelMode();
  Serial.println("... DONE. ");
}

void ensureServoInWheelMode(){
  if (dxlGetMode(ServoID) == 2){
      Serial.println("Servo is already in 'wheel' mode.");
      return;   
  }
  Serial.println("Switching the servo to 'wheel' mode."); 
  axSetWheelMode(ServoID);  //For MX servos use mxSetWheelMode()
}

boolean isReady(){
  delay(500); 
  if(Serial.available() == 0) {
    return false;}
  int inByte = Serial.read();
  if(inByte == 'r' || inByte == 'R'){
    return true;
  }
  return false;
}

 //Top speed for AX-12A is about 60 RPM
 //Values below 115 are counter clockwise speeds
 //Values beyon 115 are clockwise speeds
 //115 == 0
const int RPMs[] = {0, 30, 60};
const int SpeedsCount = 3;
int CurrentSpeedIndex = 0;
boolean ClockwiseRotation = true;
const float RpmToSpeedValueRatio = 0.111; //for AX servos 0.111, for MX servos 0.114
unsigned long LastSpeedChangeTime = 0;
const unsigned long millisecondsBetweenSpeeds = 2000;

void loop() {
  unsigned long currentTime = millis();
  if(currentTime - LastSpeedChangeTime < millisecondsBetweenSpeeds){
    return;
  }
  Serial.println("#### Speed Changed ");
  Serial.print("- Goal Speed RPMs: ");
  Serial.println(RPMs[CurrentSpeedIndex]);
  int speedValue;
  if(ClockwiseRotation){
    Serial.println("- Clockwise Rotation");
    speedValue =  1024 + RPMs[CurrentSpeedIndex] / RpmToSpeedValueRatio;
  }else{
    speedValue = RPMs[CurrentSpeedIndex] / RpmToSpeedValueRatio;
    Serial.println("- Counter Clockwise Rotation");
  }
  Serial.print("- Goal Speed value: ");
  Serial.println(speedValue);
  dxlSetGoalSpeed(ServoID, speedValue);
  CurrentSpeedIndex++;
  if(CurrentSpeedIndex >= SpeedsCount){
    CurrentSpeedIndex = 0;
    ClockwiseRotation = !ClockwiseRotation; //Change direction
  }
  LastSpeedChangeTime = millis(); 
}

int readCurrentSpeed(){
  int speedValue;
  int retryCount = 5;
  do{
    speedValue = dxlGetSpeed(ServoID);
    retryCount--;
  }while(speedValue < 0 || retryCount > 0);
  return speedValue;
}

void checkVoltage(){
  //ServoIDs is optional if servos IDs are consecutive starting at 1
  float voltage = dxlGetSystemVoltage(ServoID); 
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

