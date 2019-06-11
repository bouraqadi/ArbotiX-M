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
#include <BioloidController.h>

BioloidController controller = BioloidController(1000000); //1000000 = bauds for control

void setup() {
  Serial.begin(9600);
  Serial.println("");   
  Serial.println("#### Serial Communication Established.");
  checkVoltage();
  Serial.println("#### WARNING: Ensure ALL 4 servos can freely rotate");
  Serial.println("Type 'R' once you are ready"); 
  while(!isReady());
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

// We assume we have 4 servos with consecutive IDs from 1 to 4
const int MaxServoID = 4;
const PROGMEM unsigned int rightPositions[] = {MaxServoID, 200, 300, 400, 500};
const PROGMEM unsigned int leftPositions[] = {MaxServoID, 1000, 900, 800, 700};

bool shouldMoveToTheLeft = true;

void loop() {
  if(shouldMoveToTheLeft){
    Serial.println("<<<< Moving all servos to the left");
    moveTo(leftPositions, 1000);
  }else{
    Serial.println(">>>> Moving all servos to the right");
    moveTo(rightPositions, 3000);
  }
  shouldMoveToTheLeft = !shouldMoveToTheLeft;
}

void moveTo(unsigned int * pose, int motionDurationInMilliseconds){
    delay(100); // Recommanded pause
    controller.loadPose(pose);   // load the pose from FLASH, into the nextPose buffer
    controller.readPose();       // read in current servo positions to the curPose buffer
    controller.interpolateSetup(motionDurationInMilliseconds); 
    while(controller.interpolating > 0){  // do this while we have not reached our new pose
        controller.interpolateStep();     // move servos, if necessary. 
        delay(1);
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

