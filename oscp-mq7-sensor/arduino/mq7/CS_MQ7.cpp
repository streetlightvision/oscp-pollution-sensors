#include "CS_MQ7.h"

CS_MQ7::CS_MQ7(int csPin)
{
    _cs_pin = csPin;
    
    SPI.begin();
    
    pinMode(_cs_pin, OUTPUT);
	
    time = 0;
    currTime = 0;
    prevTime = 0;
    currCoPwrTimer = 0;
    CoPwrState = V_LOW;
    currCoPwrTimer = 500;	
    startMillis = millis();
}

void CS_MQ7::set_r(int val)
{
    digitalWrite(_cs_pin,LOW);
    if(CoPwrState == V_LOW)
    {
        SPI.transfer(0);
        SPI.transfer(V_LOW);
    }
    else
    {
        SPI.transfer(0);
        SPI.transfer(V_HIGH);
    }
    digitalWrite(_cs_pin,HIGH);
}

void CS_MQ7::CoPwrCycler()
{    
    currTime = millis();

    if (currTime - prevTime > currCoPwrTimer)
    {
        prevTime = currTime;
        
        if(CoPwrState == V_LOW)
        {
          
            unsigned int gasLevel = analogRead(2);
            float time = (millis() - startMillis) / 1000.0;
    
            float voltage = gasLevel * (readVcc()/1000.0/1023.0);
            
            if(time > 50) // ignore first entry
            {
                Serial.print(time);
                Serial.print(",");
                Serial.print(voltage);
                Serial.print(",");
                Serial.println(gasLevel);
            }
            
            CoPwrState = V_HIGH;
            currCoPwrTimer = 60000;  //60 seconds at 5v
        }
        else
        {
          CoPwrState = V_LOW;
          currCoPwrTimer = 90000;  //90 seconds at 1.4v
        }
        set_r(CoPwrState);
    }
}

boolean CS_MQ7::CurrentState()
{	
    if(CoPwrState == V_LOW)
    {
        return false; 
    }
    else
    {
        return true;
    }
}

long CS_MQ7::readVcc() {
  long result;
  
  //not used on emonTx V3 - as Vcc is always 3.3V - eliminates bandgap error and need for calibration http://harizanov.com/2013/09/thoughts-on-avr-adc-accuracy/

  #if defined(__AVR_ATmega168__) || defined(__AVR_ATmega328__) || defined (__AVR_ATmega328P__)
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);  
  #elif defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__) || defined(__AVR_AT90USB1286__)
  ADMUX = _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  ADCSRB &= ~_BV(MUX5);   // Without this the function always returns -1 on the ATmega2560 http://openenergymonitor.org/emon/node/2253#comment-11432
  #elif defined (__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
  ADMUX = _BV(MUX5) | _BV(MUX0);
  #elif defined (__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
  ADMUX = _BV(MUX3) | _BV(MUX2);
	
  #endif


  #if defined(__AVR__) 
  delay(2);                                        // Wait for Vref to settle
  ADCSRA |= _BV(ADSC);                             // Convert
  while (bit_is_set(ADCSRA,ADSC));
  result = ADCL;
  result |= ADCH<<8;
  result = READVCC_CALIBRATION_CONST / result;  //1100mV*1024 ADC steps http://openenergymonitor.org/emon/node/1186
  return result;
 #elif defined(__arm__)
  return (3300);                                  //Arduino Due
 #else 
  return (3300);                                  //Guess that other un-supported architectures will be running a 3.3V!
 #endif
}
