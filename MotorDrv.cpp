/*
 * This is a library to deal with Arduino interfacing with the L298N H-bridge module. 
 * By Joel R. Goodman, October 2016.
 * goodmajo@oregonstate.edu
 */
#include <Arduino.h>
#include <MotorDrv.h>
#include <math.h> // I need this to round intensity values

	int ls ;
    int rs ;
    
MotorDrv::L298N(int ltc, int rtc, int lpwm, int lpos, int lneg, int rpwm, int rpos, int rneg, float intensity ){

    intensity = (intensity * .01) ; // Turning intensity into an easy to use multiplier
    // Mapping motor movement to transmitter stick values.
    // Constraints help even out the motor performance when the robot is backing up. Without this constraint the motors stop behaving when I reverse the motor, especially at the lowest end.
    ls = constrain(map(ltc, 1040, 1881, -255, 255), round(-100*intensity), round(255*intensity) ) ;
    rs = constrain(map(rtc, 1040, 1881, -255, 255), round(-100*intensity), round(255*intensity) ) ;
    
    if(ls > 10){
          digitalWrite(lpos, HIGH) ; // "+" on driver
          digitalWrite(lneg, LOW) ; // "-" on driver
          analogWrite(lpwm, ls) ; // This is pulse width! Send to appropriate spot on driver or nothing will happen!
        }
      if(ls < -10){
          digitalWrite(lpos, LOW) ;
          digitalWrite(lneg, HIGH) ;
          analogWrite(lpwm, ls) ;
        }
        // Now I want to make a spot where there is absolutely no movement at all in the motor. I'll tell the arduino to feed the motors no current if the transmitter vals are within a certain domain.
      if(-10<=ls && ls<=10){
          digitalWrite(lpos, LOW) ;
          digitalWrite(lneg, HIGH) ;
          analogWrite(lpwm, 0) ;
        }
    
      // And now for the right motor
      if(rs > 10){
          digitalWrite(rpos, HIGH) ;
          digitalWrite(rneg, LOW) ;
          analogWrite(rpwm, rs) ;
        }
      if(rs < -10){
          digitalWrite(rpos, LOW) ;
          digitalWrite(rneg, HIGH) ;
          analogWrite(rpwm, rs) ;
        }
        // Same deal as above with the left motor. I'm making a spot where there is absolutely no movement at all in the motor. I'll tell the arduino to feed the motors no current if the transmitter vals are within a certain domain.
      if(-10<=rs && rs<=10){
          digitalWrite(rpos, LOW) ;
          digitalWrite(rneg, HIGH) ;
          analogWrite(rpwm, 0) ;
        }
  }

MotorDrv::motorKill(int PWMpintoKill){
		analogWrite(PWMpintoKill, 0) ;
	}
