/* This is based on polling - would be nice to reimplements to use an eventing model */



/*
  LiquidCrystal Library - Hello World
 
 Demonstrates the use a 16x2 LCD display.  The LiquidCrystal
 library works with all LCD displays that are compatible with the
 Hitachi HD44780 driver. There are many of them out there, and you
 can usually tell them by the 16-pin interface.
 
 This sketch prints "Hello World!" to the LCD
 and shows the time.
 
  The circuit:
 * LCD RS pin to digital pin 12
 * LCD Enable pin to digital pin 11
 * LCD D4 pin to digital pin 5
 * LCD D5 pin to digital pin 4
 * LCD D6 pin to digital pin 3
 * LCD D7 pin to digital pin 2
 * LCD R/W pin to ground
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)
 
 Library originally added 18 Apr 2008
 by David A. Mellis
 library modified 5 Jul 2009
 by Limor Fried (http://www.ladyada.net)
 example added 9 Jul 2009
 by Tom Igoe
 modified 22 Nov 2010
 by Tom Igoe
 
 This example code is in the public domain.

 http://www.arduino.cc/en/Tutorial/LiquidCrystal
 */

// include the library code:
#include <LiquidCrystal.h>

//#define n 2


// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

typedef struct Machine {
  int pin;         // arduino pin for sensor input
  char* name;      // name of machine for display
  int threshold;   // minimum sensor input that represents a run state for this machine
  int postrun;     // seconds of extractor post run after machine stop
};


// Declare Pins
int relayPin = 9; // control relay connected to digital pin 9
int manPin = 7;   // manual switch connected to digital pin 8
const int extSenPin = A1;
int testPin = 8;
//Machine machines[n];
//int val = 0;     // variable to store the read value

#define M_SIZE = 2

Machine machines[] = { 
    (Machine){A2, "Planer ", 800, 5}, 
    (Machine){A3, "Bandsaw", 800, 5} 
    //(Machine){1, "Planer", 800, 5}, 
    //(Machine){2, "Bandsaw", 800, 5}
};

void setup() {
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  pinMode(relayPin, OUTPUT);      // sets the digital pin 9 as output
  pinMode(manPin, INPUT);      // sets the digital pin 8 as input
  pinMode(extSenPin, INPUT);
  pinMode(testPin, INPUT);
  
  //TODO - iterate over the machine array setting the pin mode for the sensor inputs.

  
  //machines = { {1, "Planer", 800, 5},{1, "Bandsaw", 800, 5}};
}

void loop() {
  DrawHomeScreen();
  //check for manual
  if (digitalRead(manPin)==HIGH){
    ManualOverride();
  }
  //loop over machine array
  for(int i=0;i<2;i++){
    if(machines[i].threshold < GetSensorValue(machines[i].pin)){
      Run(i);
    }
  }  
  //wait
  delay(500);  
}

void ManualOverride(){
     //loop checking status of manual pin
     int i=10;
     while(digitalRead(manPin)==HIGH){
       //turn on the extractor
       digitalWrite(relayPin, HIGH);
       //draw screen
       if(i==10){  //draw override screen every n iterations
         DrawManualOverrideScreen(GetCurrent(extSenPin));
         i=0;
       }
       i++;
       if(digitalRead(manPin)==HIGH)  //check pin again
         delay(250);  //wait
       else{
         //digitalWrite(relayPin, LOW);
         break; 
       } 
              
     }
     digitalWrite(relayPin, LOW);
     return;
} 

void Run(int index){ // parameter is the machine index of the running machine
  //loop checking status of sensor
   int i=10;
   while(machines[index].threshold < GetSensorValue(machines[index].pin)){
     //turn on the extractor
     digitalWrite(relayPin, HIGH);
     //draw screen
     if(i==10){  //draw override screen every n iterations
       DrawRunScreen(index);
       i=0;
     }
     i++;
     if(machines[index].threshold < GetSensorValue(machines[index].pin))  //check pin again
       delay(250);  //wait
     else{
       //digitalWrite(relayPin, LOW);
       break; 
     }      
   }
   //post run
   if (machines[index].postrun>0){
     // display the stop screen and count down the overrun seconds.
     for(int j=machines[index].postrun;j>0;j--){
        lcd.clear();  
        lcd.setCursor(0, 0);
        lcd.print(machines[index].name);
        lcd.print(" STOPPED ");
        lcd.setCursor(0, 1);
        lcd.print("Ext overrun ");
        lcd.print(j);
        lcd.print(" s");
        delay(1000);
     }
    // 
   }
   digitalWrite(relayPin, LOW);
   return;
   
}

void DrawHomeScreen(){
  lcd.clear();  
  lcd.setCursor(0, 0);
  lcd.print("AutoVac 10,000");  /// Add RTC and show clock?
  lcd.setCursor(0, 1);
  lcd.print("Extractor OFF");
  return;
}

void DrawRunScreen(int index){
  lcd.clear();  
  lcd.setCursor(0, 0);
  lcd.print(machines[index].name);
  lcd.print(" ON ");
  lcd.print(GetCurrent(machines[index].pin),1);
  lcd.print("A");
  lcd.setCursor(0, 1);
  lcd.print("Extract ON ");
  lcd.print(GetCurrent(extSenPin),1);
  lcd.print("A");
  return;
}

void DrawManualOverrideScreen(float c){
  lcd.clear();  
  lcd.setCursor(0, 0);
  lcd.print("MANUAL OVERRIDE");
  lcd.setCursor(0, 1);
  lcd.print("Extract ON ");
  lcd.print(c,1);
  lcd.print("A");  
  return;
}

int GetSensorDeviation(int pin){ //returns deviation form 512 which represents 2.5v from sensor i.e. 0 A
  /*if(digitalRead(testPin)==HIGH)
    return 900;
  else
    return 300;*/
  
  return abs(512-analogRead(pin));  //0-1023  
}

float GetCurrent(int pin){
  //double SensorVoltage = GetSensorValue(pin)*5/1023.0;  //
  //double Current = (SensorVoltage -2.5 )/0.100; 		 	//verify from Datasheet 20A=100, 5A=185
  
  double SensorVoltage = (((long)analogRead(pin) * 5000 / 1024) - 500 ) * 1000 / 133; 
  double Current = outputValue / 1000;
  
  return Current;
  
  //return 4.5;  
}

