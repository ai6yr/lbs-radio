     
/*

 N6QW / KK6FUT Super Simple DDS - Si5351 version
 2015 Feb 02
 
 This is a super simple DDS sketch to run a single band AD9850-based DDS.
 Kept super-super simple to make it easy for the non-software folks.
 
 Inputs: Rotary encoder
 Outputs: LCD display, DDS frequency

 Uses a a (16*2) LCD Display on the I2C Interface for TwRobot. 
 Using the Si5351 breakout board from Adafruit.
 
 */

#include<stdlib.h>
#include <Wire.h>
//#include <LiquidCrystal_I2C.h> //twrobot
#include "Jsi5351.h"

#define IF           0L

Si5351 si5351;

boolean keystate = 0;
boolean changed_f = 0;
int old_vfo = 0;
int old_bfo = 0;
int val = 0; // used for dip

const uint32_t bandStart = 7000000L;  // start of 40m
const uint32_t bandEnd = 7300000L; // end of 40m
const uint32_t bandInit = 7100000L; // where to initially set the frequency

volatile uint32_t vfo = bandInit ;  // this is a variable (changes) - set it to the beginning of the band
volatile uint32_t radix = 1000; // how much to change the frequency by, clicking the rotary encoder will change this.
volatile uint32_t LSB = 8998500L;
volatile uint32_t USB = 9001500L;
volatile uint32_t bfo = 8998500L; // or LSB later make it selectable with the SSB Select Switch

int blinkpos = 3; // position for blinking cursor

// Set pin numbers for ADS9850
const int W_CLK = 2;
const int FQ_UD = 3;
const int DATAPIN = 4;
const int RESET = 5;

// Set pins for ROTARY ENCODER - we are NOT using rotary encoder switch, so omit for clarity
const int RotEncAPin = 10;
const int RotEncBPin = 11;
const int RotEncSwPin = A3;

// Display library assumes use of A4 for clock, A5 for data. No code needed.

// Variables for Rotary Encoder handling
boolean OldRotEncA = true;
boolean RotEncA = true;
boolean RotEncB = true;
boolean RotEncSw = true;

// Instantiate the LCD display...

//LiquidCrystal_I2C lcd(0x027, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address TwRobot
//LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x20 for a 16 chars and 2 line display

void setup() {

  // Set up LCD
   lcd.begin(16,2);   // initialize the lcd for 16 chars 2 lines, turn on backlight TwRobot
  //lcd.init();
    // Print a message to the LCD.
 // lcd.backlight();
  lcd.setCursor(0, 1);
  lcd.print(" N6QW & KK6FUT");

  // Set up ROTARY ENCODER
  pinMode(RotEncAPin, INPUT);
  pinMode(RotEncBPin, INPUT);
  pinMode(RotEncSwPin, INPUT);
  // set up pull-up resistors on inputs...  
  digitalWrite(RotEncAPin,HIGH);
  digitalWrite(RotEncBPin,HIGH);
  digitalWrite(RotEncSwPin,HIGH); 
  
  
  // Start serial and initialize the Si5351
  si5351.init(SI5351_CRYSTAL_LOAD_8PF);
  //si5351.set_correction(200);
  delay(1000);
  // Set CLK0 to output vfo  frequency with a fixed PLL frequency
  si5351.set_pll(SI5351_PLL_FIXED, SI5351_PLLA);
  si5351.set_freq(vfo , SI5351_PLL_FIXED, SI5351_CLK0);
  //set power
  si5351.drive_strength(SI5351_CLK0, SI5351_DRIVE_8MA);
  si5351.drive_strength(SI5351_CLK2, SI5351_DRIVE_8MA);
  
  // start the oscillator...
  send_frequency(vfo);     
  display_frequency(vfo);

}

void loop() {

  // Read the inputs...
  RotEncA = digitalRead(RotEncAPin);
  RotEncB = digitalRead(RotEncBPin); 
  RotEncSw = digitalRead(RotEncSwPin);

  // check the rotary encoder values
  if ((RotEncA == HIGH)&&(OldRotEncA == LOW)){
    // adjust frequency
    if (RotEncB == LOW) { 
      vfo=constrain(vfo+radix,bandStart,bandEnd);
    } else {
      vfo=constrain(vfo-radix,bandStart,bandEnd);
    }
    OldRotEncA=RotEncA;  // store rotary encoder position for next go around
 
     // Now, update the LCD with frequency 
    display_frequency(vfo); // push the frequency to LCD display
    send_frequency(vfo);  // set the DDS to the new frequency  
   // delay(400); // let the frequency/voltages settle out after changing the frequency
   }
  
  // check for the click of the rotary encoder 
  if (RotEncSw==LOW){
    // if user clicks rotary encoder, change the size of the increment
    // use an if then loop, but this could be more elegantly done with some shifting math
    
    if (radix == 10000) {
       radix = 100;
    } else if (radix == 1000) {
       radix = 10000;
    } else if (radix == 100) {
       radix = 1000;
    } else if (radix == 10) {
       radix = 100;
     } else { // this is either 100 or we goofed somewhere else, so set it back to the big change
       radix = 10000;
    }
  }    
  
  OldRotEncA=RotEncA; 
  // End of loop()
}



// subroutine to display the frequency...
void display_frequency(uint32_t frequency) {  
  lcd.noBlink();
  lcd.setCursor(0, 0); //was 17
  if (frequency<10000000){
    lcd.print(" ");
  }  
  lcd.print(frequency/(uint32_t)100000,4);  
  lcd.print(" MHz");

}  


// Subroutine to generate a positive pulse on 'pin'...
void pulseHigh(int pin) {
  digitalWrite(pin, HIGH); 
  digitalWrite(pin, LOW); 
}

// calculate and send frequency code to Si5351...
void send_frequency(uint32_t frequency) {
  vfo = frequency;
  si5351.set_freq(frequency , SI5351_PLL_FIXED, SI5351_CLK0);
 // si5351.drive_strength(SI5351_CLK0, SI5351_DRIVE_8MA);

  Serial.print(vfo);
  Serial.print("   ");
  Serial.print(bfo);
  Serial.print("  ");
  Serial.print(vfo - bfo);
  Serial.print("  ");



}








