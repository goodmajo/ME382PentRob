/*
 * MotorDrv.h <<HEADER>>
 * This is a library to deal with Arduino interfacing with the L298N H-bridge module, the IBT_2 half-bridge module, and killing current to motors that you don't want to move. 
 * By Joel R. Goodman, October 2016
 * goodmajo@oregonstate.edu 
*/

#include <Arduino.h>

#ifndef MotorDrv_h
#define MotorDrv_h


class MotorDrv{

	public:
		L298N(int ltc, int rtc, int lpwm, int lpos, int lneg, int rpwm, int rpos, int rneg, float intensity) ;
		L298Nsingle(int L298Ntc, int pwm, int pos, int neg, float intensity) ;
		IBT2(int ls, int rs, int m1pwml, int m1pwmr, int m1enl, int m1enr, int m2pwml, int m2pwmr, int m2enl, int m2enr, float intensity ) ;
		IBT2single(int IBT2tc, int pwml, int pwmr, int enl, int enr, float intensity ) ;
		motorKill(int pwmPinToKill) ;
		stepperKill(int in1, int in2, int in3, int in4);
	
	private:
		int rs ;
		int ls ;
		int L298Ncontrol ;
		int IBT2control ;

};

#endif
