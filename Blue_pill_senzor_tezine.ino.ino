#include <Q2HX711.h>
#include <Wire.h>


const byte hx711_data_pin = PA11;
const byte hx711_clock_pin =PA12;

 #define izlazPWM    PA1

  Q2HX711 hx711(hx711_data_pin, hx711_clock_pin);

void setup() {
 
  pinMode(izlazPWM, PWM);
  Serial.begin(19200);

}

void loop() {
  
  
 Serial.println((hx711.read()/100.0-83200)*24-2700);

  pwmWrite(izlazPWM, (hx711.read()/100.0-83200)*24-2700);

  delay(2);
}
