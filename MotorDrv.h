/*
 * MotorDrv.h <<HEADER>>
 * This is a library to deal with Arduino interfacing with the L298N H-bridge module, the IBT_2 half-bridge module, and killing current to motors that you don't want to move. 
 * By Joel R. Goodman, October and November of 2016
 * goodmajo@oregonstate.edu
 * https://github.com/goodmajo/ME382PentRob
*/

#include <Arduino.h>

#ifndef MotorDrv_h
#define MotorDrv_h


class MotorDrv{

	public:
		void L298N(int stickFloor, int stickCeiling, int ltc, int rtc, int lpwm, int lpos, int lneg, int rpwm, int rpos, int rneg, float intensity ) ;
		void L298Nsingle(int stickFloor, int stickCeiling, int tc, int pwm, int pos, int neg, float intensity) ;
		void IBT2(int stickFloor, int stickCeiling, int ls, int rs, int m1pwml, int m1pwmr, int m1enl, int m1enr, int m2pwml, int m2pwmr, int m2enl, int m2enr, float intensity ) ;
		void IBT2single(int stickFloor, int stickCeiling, int tc, int pwml, int pwmr, int enl, int enr, float intensity ) ;
		void motorKill(int pwmPinToKill) ;
		void stepperKill(int in1, int in2, int in3, int in4);
	
	private:
		int rs ;
		int ls ;
		int control ;
};

#endif
