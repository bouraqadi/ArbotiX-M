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

void setup(){
  Serial.println("");   
  Serial.begin(9600);
  Serial.println("");   
  Serial.println("#### Serial Communication Established.");
}

unsigned long LastDisplayTime = 0;
const unsigned long MillisecondsBetweenDisplayRefresh = 2000;

void loop() {
  // put your main code here, to run repeatedly:
  unsigned long currentTime = millis();
  if(currentTime - LastDisplayTime < MillisecondsBetweenDisplayRefresh){
    return;
  }
  LastDisplayTime = currentTime; 
  checkVoltage();
}

const int NumberOfServos = 4;
const int ServoIDs[] = {1, 2, 3, 4};

void checkVoltage(){
  //ServoIDs is optional if servos IDs are consecutive starting at 1
  float voltage = dxlGetSystemVoltage(NumberOfServos, ServoIDs); 
  Serial.print ("##### System Voltage: ");
  Serial.print (voltage);
  Serial.println (" volts.");
  if (voltage >= 10.0){
    Serial.println("ALL GOOD: Voltage is above 10 V");
    return;
  }
  Serial.println("PROBLEM: Voltage level below minimum (10 V)");
  Serial.println("DANGER if you are using LiPO batteries!!!");
}

