int dustPin=0;
int dustVal=0;
 
int ledPower=2;
int delayTime=280;
int delayTime2=40;
float offTime=9680;

int cnt = 0;

#ifndef READVCC_CALIBRATION_CONST
#define READVCC_CALIBRATION_CONST 1126400L
#endif

void setup()
{
  Serial.begin(9600);
  pinMode(ledPower,OUTPUT);
  pinMode(4, OUTPUT);
}
 
void loop()
{
  // ledPower is any digital pin on the arduino connected to Pin 3 on the sensor
  float vcc = readVcc()/1000.0;
  float v_prop = vcc/5.0;
  digitalWrite(ledPower,LOW); // power on the LED
  delayMicroseconds(delayTime);
  dustVal=analogRead(dustPin); // read the dust value via pin 5 on the sensor
  delayMicroseconds(delayTime2);
  digitalWrite(ledPower,HIGH); // turn the LED off
  delayMicroseconds(offTime);
  float a_norm = dustVal/1023.0;
  float mgm3 = (2.5/3.5)*a_norm;

  if(cnt > 3)
  {
    Serial.print(dustVal);
    Serial.print(",");
    Serial.print(vcc);
    Serial.print(",");
    Serial.println(mgm3);
  }
  delay(3000);
  cnt++;
}

long readVcc() {
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
