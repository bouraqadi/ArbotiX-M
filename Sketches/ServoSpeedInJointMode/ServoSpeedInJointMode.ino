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
const int MinPosition = 300;
const int MaxPosition = 700;

void setup() {
  Serial.begin(9600);
  Serial.println("");   
  Serial.println("#### Serial Communication Established.");
  checkVoltage();
  Serial.println("#### WARNING: Ensure servo with ID 1 can freely rotate");
  Serial.println("Type 'R' once you are ready"); 
  while(!isReady());
  Serial.println("#### Initializing servo");
  ensureServoInJointMode();
  moveServoToPosition(MinPosition);
  Serial.println("... DONE. ");
}

void ensureServoInJointMode(){
  if (dxlGetMode(ServoID) == 1){
      Serial.println("Servo is already in 'joint' mode.");
      return;   
  }
  Serial.println("Switching the servo to 'joint' mode."); 
  dxlSetJointMode(ServoID, 0, 1023);   
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

void moveServoToPosition(int position){
  dxlSetGoalPosition(ServoID, position);
  waitServoPositionStablize(ServoID); 
}

// 0 = Top speed in joint mode
const int RPMs[] = {10, 20, 30, 40, 50, 0};
const int SpeedsCount = 6;
int CurrentSpeedIndex = 0;
const float RpmToSpeedValueRatio = 0.111; //for AX servos 0.111, for MX servos 0.114
unsigned long LastMotionTime = 0;
const unsigned long millisecondsBetweenMotions = 1000;

void loop() {
  unsigned long currentTime = millis();
  if(currentTime - LastMotionTime < millisecondsBetweenMotions){
    return;
  }
  Serial.println("#### Speed Changed ");
  Serial.print("- Goal Speed RPMs: ");
  if(RPMs[CurrentSpeedIndex] == 0){
    Serial.println("Full speed"); 
  } else{
    Serial.println(RPMs[CurrentSpeedIndex]);
  }
  int speedValue = RPMs[CurrentSpeedIndex] / RpmToSpeedValueRatio; 
  Serial.print("- Goal Speed value: ");
  Serial.println(speedValue);
  dxlSetGoalSpeed(ServoID, speedValue);
  moveServoToPosition(MaxPosition);
  delay(500);
  moveServoToPosition(MinPosition);
  CurrentSpeedIndex++;
  if(CurrentSpeedIndex >= SpeedsCount){
    CurrentSpeedIndex = 0;
  }
  LastMotionTime = millis(); 
}

void waitServoPositionStablize(int servoID){
  while(dxlGetMoving(servoID)){
    Serial.print("- Current speed value: ");
    Serial.println(readCurrentSpeed());
    delay(200);
  }
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

