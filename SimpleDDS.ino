

  
/*

 N6QW / KK6FUT Super Simple DDS
 
 This is a super simple DDS sketch to run a single band AD9850-based DDS.
 Kept super-super simple to make it easy for the non-software folks.
 
 Inputs: Rotary encoder
 Outputs: LCD display, DDS frequency

 Uses a a (16*2) LCD Display on the I2C Interface for TwRobot. 
 
 */

#include<stdlib.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h> //TwRobot

const double  bandStart = 7000000;  // start of 40m
const double bandEnd = 7300000; // end of 40m
double freq = bandStart;  // this is a variable (changes) - set it to the beginning of the band

// Set pin numbers for ADS9850
const int W_CLK = 2;
const int FQ_UD = 3;
const int DATAPIN = 4;
const int RESET = 5;

// Set pins for ROTARY ENCODER - we are NOT using rotary encoder switch, so omit for clarity
const int RotEncAPin = 11;
const int RotEncBPin = 12;

// Display library assumes use of A4 for clock, A5 for data. No code needed.

// Variables for Rotary Encoder handling
boolean OldRotEncA = true;
boolean RotEncA = true;
boolean RotEncB = true;

// Instantiate the LCD display...
LiquidCrystal_I2C lcd(0x027, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address TwRobot

void setup() {

  // Set up LCD
  lcd.begin(16,2);   // initialize the lcd for 16 chars 2 lines, turn on backlight TwRobot

  // Set up ROTARY ENCODER
  pinMode(RotEncAPin, INPUT);
  pinMode(RotEncBPin, INPUT);
  // set up pull-up resistors on inputs...  
  digitalWrite(RotEncAPin,HIGH);
  digitalWrite(RotEncBPin,HIGH);

  // Set up DDS
  pinMode(FQ_UD, OUTPUT);
  pinMode(W_CLK, OUTPUT);
  pinMode(DATAPIN, OUTPUT);
  pinMode(RESET, OUTPUT);
 // start up the DDS... 
  pulseHigh(RESET);
  pulseHigh(W_CLK);
  pulseHigh(FQ_UD); 
  // start the oscillator...
  send_frequency(freq);     
  display_frequency(freq);

}

void loop() {

  // Read the inputs...
  RotEncA = digitalRead(RotEncAPin);
  RotEncB = digitalRead(RotEncBPin); 

  // check the rotary encoder values
  if ((RotEncA == HIGH)&&(OldRotEncA == LOW)){
    // adjust frequency
    if (RotEncB == LOW) {
      freq=constrain(freq+10000,bandStart,bandEnd);
    } else {
      freq=constrain(freq-10000,bandStart,bandEnd);
    }
    OldRotEncA=RotEncA;  // store rotary encoder position for next go around
 
     // Now, update the LCD with frequency 
    display_frequency(freq); // push the frequency to LCD display
    send_frequency(freq);  // set the DDS to the new frequency  
    delay(400); // let the frequency/voltages settle out after changing the frequency
   }
  OldRotEncA=RotEncA; 
  // End of loop()
}



// subroutine to display the frequency...
void display_frequency(double frequency) {  
  lcd.setCursor(0, 0); //was 17
  if (frequency<10000000){
    lcd.print(" ");
  }  
  lcd.print(frequency/1e6,4);  
  lcd.print(" MHz");
}  


// Subroutine to generate a positive pulse on 'pin'...
void pulseHigh(int pin) {
  digitalWrite(pin, HIGH); 
  digitalWrite(pin, LOW); 
}

// calculate and send frequency code to DDS Module...
void send_frequency(double frequency) {
  int32_t freq = (frequency) * 4294967295/125000000;
  for (int b=0; b<4; b++, freq>>=8) {
    shiftOut(DATAPIN, W_CLK, LSBFIRST, freq & 0xFF);
  } 
  shiftOut(DATAPIN, W_CLK, LSBFIRST, 0x00);  
  pulseHigh(FQ_UD); 
}








