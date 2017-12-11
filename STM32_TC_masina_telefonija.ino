#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 20, 4);

/*
 * A2 - Izlaz prema kondicioneru PWM-0-10VDC
 * A0 - Analogni ulaz - brzina obrtanja lire 0-3,3v
 * B0 - Ulaz frekventni sa enkodera 0-14400hz
 * B6 , B7  - Izlaz prema LCD displeju
 */

#include <Adafruit_MCP4725.h>
Adafruit_MCP4725 dac;
#define DAC_RESOLUTION    (12)

// period of pulse accumulation and serial output, milliseconds
#define MainPeriod 20

long previousMillis = 0; // will store last time of the cycle end
long previousMillis1 = 0;
volatile unsigned long duration=0; // accumulates pulse width
volatile unsigned int  pulsecount=0;

volatile unsigned long previousMicros=0;

unsigned int val;

int             Korak;
int             liraU; 
float           Obrt;

void setup()
{
  dac.begin(0x60);
  
  lcd.begin();
  lcd.backlight();
  Serial.begin(115200);
  
  pinMode(PA0, INPUT);
  
  pinMode(PA6, PWM);
  pinMode(PA2, PWM);  // Izlazni napon linijske brzine 14175Hz == 3,3V PWM
  pinMode(PB0, INPUT_PULLUP);
  pinMode(9, OUTPUT);
  attachInterrupt(PB0, myinthandler, RISING);

    lcd.setCursor(0, 0);
    lcd.print("LIRA[1/min]  ");
    lcd.setCursor(0, 1);
    lcd.print("LINIJA[m/min] ");
    lcd.setCursor(0, 2);
    lcd.print("KORAK[mm]  ");

    lcd.setCursor(0, 3);
    lcd.print("FREK[Hz]:  ");
  
}

void loop()
{
  
  unsigned long currentMillis = millis();
  
  if (currentMillis - previousMillis >= MainPeriod) 
  {
    previousMillis = currentMillis;   

 
    unsigned long _duration = duration;
    unsigned long _pulsecount = pulsecount;
   
    duration = 0; // clear counters
    pulsecount = 0;
    float Freq = 1e6 / float(_duration);
    Freq *= _pulsecount; // calculate F
    
    // output time and frequency data to RS232
    Serial.print(currentMillis);
    Serial.print(" "); 
    Serial.print(Freq);
    Serial.print(" "); 
    Serial.print(_pulsecount);
    Serial.print(" ");
    Serial.println(_duration);

    liraU = analogRead(PA0);     // 3,3V == 4095
    Obrt  = round ( liraU / 2.048 ) ;     // 4095 == 2000 obrtaja u minuti
    pwmWrite(PA6, liraU * 16);

    unsigned int Brz = map ( int(Freq), 0, 14175, 0, 1181);       //  14400 Hz = 1200 m/min, 14175hz = 1181m/min
              
    Korak = round ( ( float(Brz) * ( 1000 /2 ) ) / Obrt ) ;   //  Korak u mm
    Korak = constrain(Korak,0, 99999999);
    
    unsigned int BrzUout = map ( int(Freq), 0, 14175, 0, 65535);  // maksimalna brzina 14175hz = izlaz 10V
    BrzUout = constrain(BrzUout,0, 65535);
    pwmWrite(PA2, BrzUout);
   

//    int Val = map(int(Freq), 0,3600,0,4095);
//    dac.setVoltage(Val, false);

//    dac.setVoltage(int(Freq)/20 , false);
 //   analogWrite(PB9, int(Freq));
    
 unsigned long currentMillis1 = millis();
  if (currentMillis1 - previousMillis1 >= 500) 
  {
    previousMillis1 = currentMillis1;      
  
//    lcd.clear();

    lcd.setCursor(15, 0);
    lcd.print("     ");
    
    lcd.setCursor(15, 1);
    lcd.print("     ");

    lcd.setCursor(12, 2);
    lcd.print("        ");
    
    lcd.setCursor(12, 3);
    lcd.print("        ");
    
    
    lcd.setCursor(15, 0);
    lcd.print(int(Obrt));
    
    lcd.setCursor(15, 1);
    lcd.print(Brz);

    lcd.setCursor(12, 2);
    lcd.print(Korak);

    lcd.setCursor(12, 3);
    lcd.print(int(Freq));
    
   }
  }
}

void myinthandler() // interrupt handler
{
  unsigned long currentMicros = micros();
  duration += currentMicros - previousMicros;
  previousMicros = currentMicros;
  pulsecount++;

}

