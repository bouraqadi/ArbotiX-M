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
int SelectedServoID = -1;

void setup() {
  Serial.begin(9600);
  Serial.println("");   
  Serial.println("#### Serial Communication Established.");
  checkVoltage();
  turnOffServoLed(DXL_BROADCAST); //Turn OFF all servo LEDs
  displaySelectServoMenu();
}

void loop() {
  if(SelectedServoID == -1){
    return selectServo();
  }
  controlServo();
}

void displaySelectServoMenu(){
  Serial.println("###########################");
  Serial.println("Please type in the ID (1-4) of the servo to control");
}

void selectServo(){
  if(Serial.available() == 0) { return ; }
  int inByte = Serial.read();
  if(inByte < 49 || inByte > 52){
     displaySelectServoMenu();
    return;
  }
  SelectedServoID = inByte - 48;
  turnOnServoLed(SelectedServoID);
  Serial.println("###########################");    
  Serial.print("Selected: ");
  displayServoPosition(SelectedServoID);
  Serial.println("Press a or z to move the servo back and forth");
  Serial.println("Press any other key to choose another servo");
}

void displayServoPosition(int servoID){
    Serial.print("Servo ");
    Serial.print(servoID);
    Serial.print(" at position ");
    Serial.println(getServoPosition(servoID));
}

int getServoPosition(int servoID){
  int position;
  for(int i = 0; i < 10; i++){
    position = dxlGetPosition(servoID);
    if(position > -1){
      return position;
    }
  }
  Serial.print("Failed reading position of servo ");
  Serial.println(servoID);
  return -1;
}

void controlServo(){
  if(Serial.available() == 0) { return ; }
  int inByte = Serial.read();
  int selectedServoPosition = getServoPosition(SelectedServoID);
   switch (inByte){
    case 'a' :
      selectedServoPosition += 10;
      break;
    case 'z' :
      selectedServoPosition -= 10;
      break;
    default:
     turnOffServoLed(SelectedServoID);
     SelectedServoID = -1;
     displaySelectServoMenu();
     return;   
    }
  Serial.print("- Setting goal position: ");
  Serial.print(selectedServoPosition);
  Serial.print(" for ");
  displayServoPosition(SelectedServoID);
  dxlSetGoalPosition(SelectedServoID, selectedServoPosition);
  waitServoPositionStablize(SelectedServoID);
  Serial.print("... DONE. ");
  displayServoPosition(SelectedServoID);
}

void waitServoPositionStablize(int servoID){
  while(dxlGetMoving(SelectedServoID)){
    delay(200);
  }
}

void turnOnServoLed(int servoID){
  dxlSetLED(servoID, true); 
}

void turnOffServoLed(int servoID){
   dxlSetLED(servoID, false);
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

