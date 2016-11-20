#include <Keypad.h>
#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C  lcd(0x27,2,1,0,4,5,6,7); // 0x27 is the I2C bus address for an unmodified backpack

/* http://tronixstuff.com/2014/09/24/tutorial-serial-pcf8574-backpacks-hd44780-compatible-lcd-modules-arduino/ */

const byte ROWS = 4; // Four rows
const byte COLS = 4; // Three columns
// Define the Keymap
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
/*
6-Green      10-Yellow
7-Blue      11-Orange
8-Violet      12-Red
9-Grey      13-Brown
*/
// Connect keypad ROW0, ROW1, ROW2 and ROW3 to these Arduino pins.
byte rowPins[ROWS] = { 9, 8, 7, 6 };
// Connect keypad COL0, COL1 and COL2 to these Arduino pins.
byte colPins[COLS] = { 10, 11, 12, 13 }; 

// Create the Keypad
Keypad kpd = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

unsigned char read_data= 0;
unsigned char lcd_lsb = 0;
unsigned char lcd_msb = 0;
unsigned char num_step = 0;
unsigned char time_step[24] = { 0 };
unsigned char temp_step[24] = { 0 };
unsigned char val= 0;
unsigned char i = 0;
unsigned char j = 0;
int motor_pin = 5;

float in[] = { 100.00, 100.39, 100.78, 101.17, 101.56, 101.95, 102.34, 102.73, 103.12, 103.51,
               103.90, 104.29, 104.68, 105.07, 105.46, 105.85, 106.24, 106.63, 107.02, 107.40,
               107.79, 108.18, 108.57, 108.96, 109.35, 109.73, 110.12, 110.51, 110.90, 111.29,
               111.67, 112.06, 112.45, 112.83, 113.22, 113.61, 114.00, 114.38, 114.77, 115.15,
               115.54, 115.93, 116.31, 116.70, 117.08, 117.47, 117.86, 118.24, 118.63, 119.01,
               119.40, 119.78, 120.17, 120.55, 120.94, 121.32, 121.71, 122.09, 122.47, 122.86,
               123.24, 123.63, 124.01, 124.39, 124.78, 125.16, 125.54, 125.93, 126.31, 126.69,
               127.08, 127.46, 127.84, 128.22, 128.61, 128.99, 129.37, 129.75, 130.13, 130.52 };

// known resistance in voltage divider
int R1 = 217;

void setup()
{
  Serial.begin(9600);
  pinMode(5, OUTPUT);
  delay(500);
  // activate LCD module
  lcd.begin (16,2); // for 16 x 2 LCD module
  lcd.setBacklightPin(3,POSITIVE);
  lcd.setBacklight(HIGH);
  lcd.cursor();
  lcd.setCursor(0,0);
  lcd.print("Enter steps: ");
  lcd.setCursor(13,0);
  lcd_msb = get_data();
  lcd.print(lcd_msb);
  lcd.setCursor(14,0);
  lcd_lsb = get_data();
  lcd.print(lcd_lsb); 
  num_step = (lcd_msb*10) + lcd_lsb;
  delay(1500);
  lcd_msb = 0;
  lcd_lsb = 0;
  Serial.println(num_step);
  for(i=1; i <= num_step; i++)
  {
     lcd.clear();
     lcd.setCursor(0,0);
     lcd.print("Enter time : ");
     lcd.setCursor(10,0);
     lcd.print(i);
     lcd.setCursor(13,0);
     lcd_msb = get_data();
     lcd.print(lcd_msb);
     lcd.setCursor(14,0);
     lcd_lsb = get_data();
     lcd.print(lcd_lsb);
     time_step[i] = (lcd_msb*10) + lcd_lsb;
     lcd_msb = 0;
     lcd_lsb = 0;
     lcd.setCursor(0,1);
     lcd.print("Time  => ");
     lcd.setCursor(5,1);
     lcd.print(i);
     lcd.setCursor(8,1);
     lcd.print(time_step[i]);
     delay(1500);

     lcd.clear();
     lcd.setCursor(0,0);
     lcd.print("Enter temp : ");
     lcd.setCursor(10,0);
     lcd.print(i);
     lcd.setCursor(13,0);
     lcd_msb = get_data();
     lcd.print(lcd_msb);
     lcd.setCursor(14,0);
     lcd_lsb = get_data();
     lcd.print(lcd_lsb);
     temp_step[i] = (lcd_msb*10) + lcd_lsb;
     lcd_msb = 0;
     lcd_lsb = 0;
     lcd.setCursor(0,1);
     lcd.print("Temp  => ");
     lcd.setCursor(5,1);
     lcd.print(i);
     lcd.setCursor(8,1);
     lcd.print(temp_step[i]);
     delay(1500);
  }
     lcd.clear();
     lcd.setCursor(0,0);
     lcd.print("<- input done ->");
     delay(1500);
}

void loop()
{
  for(i=1; i <= num_step; i++)
  {
    for(j=1; j <= time_step[i]; j++)
     {  
       if(temp_step[i] != read_temp())
       {
        motor_on();
       }
       else
       {
        motor_off();
       }
       delay(1000);
     }
  }
  while(1);
}

void motor_on(void)
{
  digitalWrite(5, HIGH);                  
}

void motor_off(void)
{
  digitalWrite(5, LOW);                  
}

int read_temp(void)
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("CurrentTemp: ");
  int pt100 = analogRead(A0);
  float Vout = pt100 * (5.0 / 1023.0);
  float R2 = R1 * 1/(5.0/Vout - 1);
  int cel =  MultiMap(R2,in,80);
  lcd.setCursor(12,0);
  lcd.print(cel);
  Serial.println(cel);
  //delay(1000);
  return cel;
  //while(1); 
}
char get_data(void)
{
 char key = 'X';
  do
  {
    key = kpd.getKey();
  }while(!key);
  Serial.println(key);
  return key - 48;
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

