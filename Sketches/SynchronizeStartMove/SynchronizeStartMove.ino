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

void setup() {
  Serial.begin(9600);
  Serial.println("");   
  Serial.println("#### Serial Communication Established.");
  checkVoltage();
  Serial.println("#### WARNING: Ensure ALL 4 servos can freely rotate");
  Serial.println("Type 'R' once you are ready"); 
  while(!isReady());
  dxlSetGoalSpeed(DXL_BROADCAST, 180); // Set speed to 20 RPM : slow enough to watch synchronized movements
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

unsigned long LastMotionTime = 0;
const unsigned long millisecondsBetweenMovements = 2000;

// We assume we have 4 servos with consecutive IDs from 1 to 4
const int MaxServoID = 4;
const int rightPositions[4][2] = {{1, 200}, {2, 300}, {3, 400}, {4, 500}};
const int leftPositions[4][2] = {{1, 1000}, {2, 900}, {3, 800}, {4, 700}};

bool shouldMoveToTheLeft = true;

void loop() {
  unsigned long currentTime = millis();
  if(currentTime - LastMotionTime < millisecondsBetweenMovements){
    return;
  }
  // Ensure servo 1 finished moving. All servos finish at the same time because movements are synchronized
  if(dxlGetMoving(1)){
    return;
  }
  if(shouldMoveToTheLeft){
    Serial.println("<<<< Moving all servos to the left");
    dxlSyncWritePosition(leftPositions, MaxServoID);
  }else{
    Serial.println(">>>> Moving all servos to the right");
    dxlSyncWritePosition(rightPositions, MaxServoID);
  }
  shouldMoveToTheLeft = !shouldMoveToTheLeft;
  LastMotionTime = millis(); 
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

