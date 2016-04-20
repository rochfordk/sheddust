#include <LiquidCrystal.h>

#include <SM.h>
#include <State.h>

#define DEBUG 1

SM M(S1_idle_H, S1_idle_B);//create statemchine with initial head and body state functions

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
// Declare Pins
int relayPin = 9; // control relay connected to digital pin 9
int manPin = 7;   // manual switch connected to digital pin 8
const int extSenPin = A1;
int testPin = 8;
int led = 13;
int index =0;


typedef struct Machine {
  int pin;         // arduino pin for sensor input
  char* name;      // name of machine for display
  int threshold;   // minimum sensor input that represents a run state for this machine
  int postrun;     // seconds of extractor post run after machine stop
};

Machine machines[] = { 
    (Machine){A1, "Planer ", 80000, 5}, 
    (Machine){A3, "Bandsaw", 00000, 5} 
    //(Machine){1, "Planer", 80, 5}, 
    //(Machine){2, "Bandsaw", 80, 5}
};

//=====================
void setup(){
  Serial.begin(115200);
  
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  pinMode(relayPin, OUTPUT);      // sets the digital pin 9 as output
  pinMode(manPin, INPUT);      // sets the digital pin 8 as input
  pinMode(extSenPin, INPUT);
  pinMode(testPin, INPUT);
  pinMode(led, OUTPUT); 
  
  //TODO - iterate over the machine array setting the pin mode for the sensor inputs.

  
  //machines = { {1, "Planer", 800, 5},{1, "Bandsaw", 800, 5}};
 
 }//setup()

void loop(){
  EXEC(M);//run statemachine
}//loop()

State S1_idle_H(){//state head function, run only once at each entry

  //turn off extractor
  digitalWrite(relayPin, LOW);
  digitalWrite(led, LOW);
  
  //display home screen
  lcd.clear();  
  lcd.setCursor(0, 0);
  lcd.print("AutoVac 10,000");  /// Add RTC and show clock?
  lcd.setCursor(0, 1);
  lcd.print("Extractor OFF");
  
  #ifdef DEBUG
  Serial.println("S1_idle_H");//print message on each re-entry
  #endif
}

State S1_idle_B(){//state body function run constantly
  //
  #ifdef DEBUG
  Serial.println("S1_idle_B");//print message on each re-entry
  if(M.Timeout(5000)) M.Set(S3_run_H, S3_run_B);
  #endif
  index = 0;
  //if(M.Timeout(500)) M.Set(S1_idle_H, S1_idle_B);//re-enter state after 0,5s
  
  //while(digitalRead(manPin)==HIGH)
  
  if(digitalRead(manPin)==HIGH) M.Set(S2_manual_H, S2_manual_B);
  
  if(machineRunning()) M.Set(S3_run_H, S3_run_B);
  
}//

boolean machineRunning(){
  //iterate over the array of defined machines
  //if any machine is running, set index and return true; 
  for(int i=0;i<2;i++){
    //if(machines[i].threshold < GetSensorDeviation(machines[i].pin)){
    if(GetCurrent(machines[i].pin>0)){
      index = i;
      Serial.println("machine running");//print message on each re-entry
      return true;
    }
  }  
  return false;
}

State S2_manual_H(){//state head function, run only once at each entry
  //display manual screen
  lcd.clear();  
  lcd.setCursor(0, 0);
  lcd.print("MANUAL OVERRIDE");
  lcd.setCursor(0, 1);
  lcd.print("Extract ON ");
  //lcd.print(c,1);  
  //lcd.print(GetSensorDeviation(extSenPin));
  lcd.setCursor(15,1 );
  lcd.print("A"); 
  #ifdef DEBUG
  Serial.println("S2_manual_H");//print message on each re-entry
  #endif
}

State S2_manual_B(){//state body function run constantly
  #ifdef DEBUG
  Serial.println("S2_manual_B");//print message on each re-entry
  #endif
  // run extractor
  digitalWrite(relayPin, HIGH);
  digitalWrite(led, HIGH);
  // Draw extractor current
  lcd.setCursor(12,1 );
  //Print average measurement over 10 smamples
  /*float avg=0;
  for(int i=0; i<10; i++){
    avg+=GetCurrent(extSenPin);
    delay(10);
  }
  float displayVal=avg/10;
  lcd.print(displayVal,1); 
  */
  //test for manual call
  if(digitalRead(manPin)==LOW) M.Set(S1_idle_H, S1_idle_B);
  // test for manual pin (should we test for machine run too or go to orun via idle?)
  //if(M.Timeout(500)) M.Set(S1_idle_H, S1_idle_B);//re-enter state after 0,5s
}//

State S3_run_H(){//state head function, run only once at each entry
  //get the index of the running machine
  //int index=0;

  //display run screen
  lcd.clear();  
  lcd.setCursor(0, 0);
  lcd.print(machines[index].name);
  lcd.print(" ON ");
  lcd.print(GetCurrent(machines[index].pin),1);  //TODO: Move this to the state body
  lcd.print("A");
  lcd.setCursor(0, 1);
  lcd.print("Extract ON ");
  lcd.print(GetCurrent(extSenPin),1); //TODO: Move this to the state body
  lcd.print("A");
  //lcd.print(GetSensorDeviation(A1));
  #ifdef DEBUG
  Serial.println("S3_run_H");//print message on each re-entry
  #endif
}

State S3_run_B(){//state body function run constantly
  // run extractor until (all?) machine(s?) stopped
  digitalWrite(relayPin, HIGH);
  digitalWrite(led, HIGH);
  #ifdef DEBUG
  Serial.println("S3_run_B");//print message on each re-entry
  delay(5000);
  #endif
  // test for machine current and go to overrun if low
  if(GetCurrent(machines[index].pin)==0) M.Set(S4_overrun_H, S4_overrun_B);
  
  //if(M.Timeout(500)) M.Set(S1_idle_H, S1_idle_B);//re-enter state after 0,5s
}//

State S4_overrun_H(){//state head function, run only once at each entry
  //display overrun screen
  //int index =0;
  
  lcd.clear();  
  lcd.setCursor(0, 0);
  lcd.print(machines[index].name);
  lcd.print(" STOPPED ");
  lcd.setCursor(0, 1);
  lcd.print("Ext overrun ");
       
  #ifdef DEBUG
  Serial.println("S4_overrun_H");//print message on each re-entry
  #endif
}

State S4_overrun_B(){//state body function run constantly
  // run extractor for designated time
  digitalWrite(relayPin, HIGH);
  digitalWrite(led, HIGH);
  // test for manual pin (should we test for machine run too or go to orun via idle?)
  if(M.Timeout(machines[index].postrun*1000)) M.Set(S1_idle_H, S1_idle_B); // return to idle after specified overrun time
  
  //reset index to -1
}//

/*
States: Idle, Manual, Run, Overrun

*/

int GetSensorDeviation(int pin){ //returns deviation form 512 which represents 2.5v from sensor i.e. 0 A
  /*if(digitalRead(testPin)==HIGH)
    return 900;
  else
    return 300;*/
  
  return abs(512-analogRead(pin));  //0-1023  
  //return 80;
}

float GetCurrent(int pin){
  //double SensorVoltage = GetSensorValue(pin)*5/1023.0;  //
  //double Current = (SensorVoltage -2.5 )/0.100; 		 	//verify from Datasheet 20A=100, 5A=185
  
  /*double SensorVoltage = (((long)analogRead(pin) * 5000 / 1024) - 500 ) * 1000 / 133; 
  double Current = SensorVoltage / 1000;
  */
  //return Current;
  
  //double SensorVoltage =  analogRead(pin)*5.0/1023.0;
  //double Current = (SensorVoltage -2.5 )/0.100; 
  
  //verage = abs(average) + abs((0.048875855327468 * analogRead(A0) -25) / 1000);
  
  float a =((float) analogRead(pin) / 512.0 - 1.0) * 2.5 / 2 * 20;
  if (a<2){
    a=0;
  }
  return abs(a);
  //return 4.5;  
}
