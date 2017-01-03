/* Demonstration sketch for PCF8574T I2C LCD Backpack 
Uses library from https://bitbucket.org/fmalpartida/new-liquidcrystal/downloads GNU General Public License, version 3 (GPL-3.0) */
#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C  lcd(0x27,2,1,0,4,5,6,7); // 0x27 is the I2C bus address for an unmodified backpack

// known resistance in voltage divider
int Rref = 410;
int Vin = 5;
int count = 0;
double VoutSum = 0.000000;

void setup() {
  // activate LCD module
  lcd.begin (16,2); // for 16 x 2 LCD module
  lcd.setBacklightPin(3,POSITIVE);
  lcd.setBacklight(HIGH);
  
  Serial.begin(9600);
}
void loop() {
  
  int sensorOut = analogRead(A0);
  
  double Vout = sensorOut * (5.0 / 1023.0);
  VoutSum += Vout;
  count++;

  if(count == 20){
    double finalVout = VoutSum/20;
    double Rpt = (finalVout * Rref) / (Vin - finalVout);
    double temp = (Rpt - 100) / 0.385;
  
    lcd.home ();                // set cursor to 0,0
    lcd.print("Voltage: ");
    lcd.print(finalVout, 5);
    lcd.print(" V");
    lcd.setCursor (0,1);        // go to start of 2nd line
    lcd.print("Temp: ");
    lcd.print(temp);
    lcd.print(" C");

    count =0;
    VoutSum = 0.000000;
  }
  delay(100);
}
