#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x3F, 16, 2);

#include <Adafruit_MCP4725.h>
Adafruit_MCP4725 dac;
#define DAC_RESOLUTION    (12)

// period of pulse accumulation and serial output, milliseconds
#define MainPeriod 50

long previousMillis = 0; // will store last time of the cycle end
long previousMillis1 = 0;
volatile unsigned long duration=0; // accumulates pulse width
volatile unsigned int  pulsecount=0;
volatile unsigned int  obrtcount=0;
volatile unsigned long previousMicros=0;

unsigned int val;

void setup()
{
  dac.begin(0x60);
  
  lcd.begin();
  lcd.backlight();
  Serial.begin(115200); 
  pinMode(0, INPUT_PULLUP);
  pinMode(9, OUTPUT);
  attachInterrupt(0, myinthandler, RISING);

  lcd.setCursor(0, 0);
  lcd.print("FREQ: ");
  lcd.setCursor(0, 1);
  lcd.print("OBRT: ");
}

void loop()
{
  
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= MainPeriod) 
  {
    previousMillis = currentMillis;   

 
    unsigned long _duration = duration;
    unsigned long _pulsecount = pulsecount;
    unsigned long _obrtcount = obrtcount/360;
    duration = 0; // clear counters
    pulsecount = 0;
    float Freq = 1e6 / float(_duration);
    Freq *= _pulsecount; // calculate F
    
    // output time and frequency data to RS232
    Serial.print(currentMillis);
    Serial.print(" "); // separator!
    Serial.print(Freq);
    Serial.print(" "); 
    Serial.print(_pulsecount);
    Serial.print(" ");
    Serial.println(_duration);
    Serial.print(" ");
    Serial.println(_obrtcount);

   int Val = map(int(Freq), 0,3600,0,4095);
    dac.setVoltage(Val, false);

//    dac.setVoltage(int(Freq)/20 , false);
 //   analogWrite(PB9, int(Freq));
    
 unsigned long currentMillis1 = millis();
  if (currentMillis1 - previousMillis1 >= 200) 
  {
    previousMillis1 = currentMillis1;      
  
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("FREQ: ");
    lcd.setCursor(0, 1);
    lcd.print("OBRT: ");

    
    lcd.setCursor(7, 0);
    lcd.print(Freq);
    
    lcd.setCursor(7, 1);
    lcd.print(_obrtcount);
   }
  }
}

void myinthandler() // interrupt handler
{
  unsigned long currentMicros = micros();
  duration += currentMicros - previousMicros;
  previousMicros = currentMicros;
  pulsecount++;
  obrtcount++;
}

