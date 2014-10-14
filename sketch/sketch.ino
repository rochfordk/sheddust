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

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

int relayPin = 9; // control relay connected to digital pin 9
int manPin = 8;   // manual switch connected to digital pin 8
int extSenPin = 10;
//int val = 0;     // variable to store the read value

typedef struct {
  byte pin : 6;
  byte mode : 1;
  byte state : 1;
} Machine;

void setup() {
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  pinMode(relayPin, OUTPUT);      // sets the digital pin 9 as output
  pinMode(manPin, INPUT);      // sets the digital pin 8 as input
  pinMode(extSenPin, INPUT);
}

void loop() {
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  //lcd.clear();  
  //lcd.setCursor(0, 1);
  // print the number of seconds since reset:
  //lcd.print(millis()/1000);
  //delay(2000);
  
  //val = digitalRead(manPin);   // read the input pin
      // sets the LED to the button's value
  
  if (digitalRead(manPin)==HIGH){
    //close the relay
    digitalWrite(relayPin, HIGH);
    //Display Manual Screen
    lcd.clear();  
    lcd.setCursor(0, 0);
    lcd.print("MANUAL OVERRIDE");
    lcd.setCursor(0, 1);
    lcd.print("Extract ON  5.4A");
    //delay(4000);
  }
  
  //Display Home Screen
  lcd.clear();  
  lcd.setCursor(0, 0);
  lcd.print("AutoVac 10,000");
  lcd.setCursor(0, 1);
  lcd.print("Extractor OFF");
  delay(3000);
  //Display Run Screen
  lcd.clear();  
  lcd.setCursor(0, 0);
  lcd.print("Planer  ON 10.2A");
  lcd.setCursor(0, 1);
  lcd.print("Extract ON  5.4A");
  delay(3000);
  //Display OVER-Run Screen
  lcd.clear();  
  lcd.setCursor(0, 0);
  //lcd.print("Bandsaw ON 10.2A");
  lcd.print("Planer  0FF ");
  lcd.setCursor(0, 1);
  lcd.print("Extract o/run 5s");
  delay(3000);
  
  
}

void ManualOverride(){
     //loop checking status of manual pin
     int i=0;
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
       else
         break;       
     }
} 

void Run(int index){ // parameter is the machine index of the running machine

}

void DrawHomeScreen(){
  
}

void DrawRunScreen(int index){
  lcd.clear();  
  lcd.setCursor(0, 0);
  lcd.print("Planer");
  lcd.print(" ON ");
  lcd.print(GetCurrent(),1);
  lcd.print("A");
  lcd.setCursor(0, 1);
  lcd.print("Extract ON ");
  lcd.print(GetCurrent(extSenPin),1);
  lcd.print("A");
}

void DrawManualOverrideScreen(float c){
  lcd.clear();  
  lcd.setCursor(0, 0);
  lcd.print("MANUAL OVERRIDE");
  lcd.setCursor(0, 1);
  lcd.print("Extract ON ");
  lcd.print(c,1);
  lcd.print("A");
  
}


float GetCurrent(int pin){
  return 4.5;
  
}

