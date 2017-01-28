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

float in[] = { 100.00, 100.39, 100.78, 101.17, 101.56, 101.95, 102.34, 102.73, 103.12, 103.51,
               103.90, 104.29, 104.68, 105.07, 105.46, 105.85, 106.24, 106.63, 107.02, 107.40,
               107.79, 108.18, 108.57, 108.96, 109.35, 109.73, 110.12, 110.51, 110.90, 111.29,
               111.67, 112.06, 112.45, 112.83, 113.22, 113.61, 114.00, 114.38, 114.77, 115.15,
               115.54, 115.93, 116.31, 116.70, 117.08, 117.47, 117.86, 118.24, 118.63, 119.01,
               119.40, 119.78, 120.17, 120.55, 120.94, 121.32, 121.71, 122.09, 122.47, 122.86,
               123.24, 123.63, 124.01, 124.39, 124.78, 125.16, 125.54, 125.93, 126.31, 126.69,
               127.08, 127.46, 127.84, 128.22, 128.61, 128.99, 129.37, 129.75, 130.13, 130.52 };
               
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
    // double temp = (Rpt - 100) / 0.385;

    int temp =  MultiMap(Rpt,in,80);
  
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

 float MultiMap(float val, float* _in, uint8_t size)
{
  // calculate if value is out of range 
  if (val < _in[0] ) return -99.99;
  if (val > _in[size-1] ) return 99.99;

  //  search for 'value' in _in array to get the position No.
  uint8_t pos = 0;
  while(val > _in[pos]) pos++;  

  // handles the 'rare' equality case
  if (val == _in[pos]) return pos;

  float r1 = _in[pos-1];
  float r2 = _in[pos];
  int c1 = pos-1;
  int c2 = pos;

 return c1 + (val - r1) / (r2-r1) * (c2-c1);
}
