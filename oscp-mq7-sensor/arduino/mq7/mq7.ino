#include <SPI.h>
#include "CS_MQ7.h"

int csPin = 4;
int pin_input = 2;

int coCicles = 0;
int val = 0;

bool debug = false;

CS_MQ7 *sensor;

unsigned long startMillis;

void setup() 
{
    sensor = new CS_MQ7(csPin);
    Serial.begin(9600);
    startMillis = millis();
}

void loop() 
{
    sensor->CoPwrCycler();
    
    unsigned int gasLevel = analogRead(pin_input);
    float time = (millis() - startMillis) / 1000.0;
    
    float voltage = gasLevel * (5.0/1023.0);
    if(debug) 
    {
        Serial.print(time);
        Serial.print(",");
        Serial.print(voltage);
        Serial.print(",");
        Serial.print(gasLevel);
        Serial.print(",");
        if(sensor->CurrentState() == false)
        {
          Serial.println("1.4v");
        }
        else
        {
          Serial.println("5v");
        }
    }
    delay(100);
}
