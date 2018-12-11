#include <Wire.h>
#include "RTClib.h"
#include <Servo.h>
 #define LED 9     //El LED esta conectado en el pin 9 
#define LDR A1     //El LDR esta conectador en el pin A0

int luz = 0;       
int valor_sensor = 0;       
int valor_limite = 120; 

const int outputPin = LED_BUILTIN;
bool state = false;
 Servo servMotor;
// RTC_DS1307 rtc;
RTC_DS3231 rtc;
const int LEDPi = 13;        // pin para el LED
const int PIRPi = 2;         // pin de entrada (for PIR sensor)
 
int pirStat = LOW;           // de inicio no hay movimiento
int va = 0;                  // estado del pin
const int led = 9;

const int LEDP = 8;
const int LDRPin = 3;

Servo servoMotor; 
void setup() {
  pinMode(LDR, INPUT);
  pinMode(led, OUTPUT);
  pinMode(LEDPi, OUTPUT); 
pinMode(LDRPin, INPUT);
   Serial.begin(9600);
   delay(1000);
  servoMotor.attach(5);
   servMotor.attach(7); 
   if (!rtc.begin()) {
      Serial.println(F("Couldn't find RTC"));
      while (1);
   }
 
   if (rtc.lostPower()) {
      rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
   }
   pinMode(LEDPi, OUTPUT); 
   pinMode(PIRPi, INPUT);
}
 
// Comprobar si esta programado el encendido
bool isScheduledON(DateTime date)
{
   int weekDay = date.dayOfTheWeek();
   float hours = date.hour() + date.minute() / 60.0;
 
   // De 09:30 a 11:30 y de 21:00 a 23:00
   bool hourCondition = (hours > 9.00 && hours < 18.40) || (hours > 21.00 && hours < 23.00);
 
   // Miercoles, Sabado o Domingo
   bool dayCondition = (weekDay == 2 || weekDay == 6 || weekDay == 0); 
   if (hourCondition && dayCondition)
   {
      return true;
   }
   return false;
}
 
void loop() {
  
valor_sensor = analogRead(LDR); 
  
   
   DateTime now = rtc.now();
 
   if (state == false && isScheduledON(now))      // Apagado y debería estar encendido
   {
      digitalWrite(outputPin, HIGH);
      state = true;
      Serial.print("Activado");
digitalWrite(led , HIGH);

   }
   else if (state == true && !isScheduledON(now))  // Encendido y deberia estar apagado
   {
      digitalWrite(outputPin, LOW);
      state = false;
      Serial.print("Desactivar");
   }
 
   delay(3000);

   va = digitalRead(PIRPi);
   if (va == HIGH)   //si está activado
   { 
      digitalWrite(LEDPi, HIGH);  //LED ON
      if (pirStat == LOW)  //si previamente estaba apagado
      {
        Serial.println("Sensor activado");
        pirStat = HIGH;
        servoMotor.write(180);
        delay(2000);
      }
   } 
   else   //si esta desactivado
   {
      digitalWrite(LEDPi, LOW); // LED OFF
      if (pirStat == HIGH)  //si previamente estaba encendido
      {
        Serial.println("Sensor parado");
        pirStat = LOW;
      }
   }
  luz = (5.0 * valor_sensor * 100.0)/1024.0; //Para entender esta formula visitar: http://programarfacil.com/podcast/48-sensor-de-temperatura-en-arduino/
  Serial.print(luz);  
  Serial.println(" Luz");             
  delay(300);                       
  
  if (luz <= valor_limite)   //Si el valor de luz es menor o igual que el valor limite
  {
    digitalWrite (LED, LOW);  //El led se apaga
    servMotor.write(90);
    delay(2000);
  }
  if (luz > valor_limite)   //Si es mayor que el valor limite
  {
    digitalWrite (LED, HIGH);  //El led se eniende
    servMotor.write(180);
    delay(2000);
  }
}
