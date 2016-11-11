/*
 * This is a library to deal with Arduino interfacing with the L298N H-bridge module. 
 * By Joel R. Goodman, October 2016
 * goodmajo@oregonstate.edu 
*/

#include <Arduino.h>

#ifndef MotorDrv_h
#define MotorDrv_h


class MotorDrv{

	public:
		L298N(int ltc, int rtc, int lpwm, int lpos, int lneg, int rpwm, int rpos, int rneg, float intensity) ;
		motorKill(int PWMpintoKill) ;
	
	private:
		int rs ;
		int ls ;

};

#endif
