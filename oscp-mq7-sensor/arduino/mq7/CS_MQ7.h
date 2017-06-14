#ifndef CS_MQ7_h
#define CS_MQ7_h

#define V_HIGH 0
#define V_LOW 122
//#define V_HIGH 40
//#define V_LOW 120

#ifndef READVCC_CALIBRATION_CONST
#define READVCC_CALIBRATION_CONST 1126400L
#endif

#include <SPI.h>
#import <Arduino.h>

class CS_MQ7{

	public:
		
		CS_MQ7(int csPin);
		void CoPwrCycler();
		boolean CurrentState();
                void set_r(int val);
                long readVcc();
		
		unsigned long time;
		unsigned long currTime;
		unsigned long prevTime;
		unsigned long currCoPwrTimer;

                unsigned long startMillis;

		int CoPwrState;
		
	private:
		int _cs_pin;
	};

#endif
