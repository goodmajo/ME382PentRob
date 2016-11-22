/* 
 * MotorDrv.cpp <<C++ functions>>
 * This is a library to deal with Arduino interfacing with the L298N H-bridge module, the IBT_2 half-bridge module, and killing current to motors that you don't want to move. 
 * By Joel R. Goodman, October 2016
 * goodmajo@oregonstate.edu 
 */
#include <Arduino.h>
#include <MotorDrv.h>
#include <math.h>		// I need this to round intensity values

    int ls ;
    int rs ;
    int control ;
    
MotorDrv::L298N(int ltc, int rtc, int lpwm, int lpos, int lneg, int rpwm, int rpos, int rneg, float intensity ){
    intensity = (intensity * .01) ; // Turning intensity into an easy to use multiplier
    
    // Mapping motor movement to transmitter stick values.
    // Constraints help even out the motor performance when the robot is backing up. Without this constraint the motors stop behaving when I reverse the motor, especially at the lowest end.
    ls = constrain(map(ltc, 1040, 1881, -255, 255), round(-100*intensity), round(255*intensity) ) ;
    rs = constrain(map(rtc, 1040, 1881, -255, 255), round(-100*intensity), round(255*intensity) ) ;
    
    if(ls > 10){
          digitalWrite(lpos, HIGH) ;		// "+" on driver
          digitalWrite(lneg, LOW) ;		// "-" on driver
          analogWrite(lpwm, ls) ;		// This is pulse width! Send to appropriate spot on driver or nothing will happen!
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

// This controls *half* of a L298N. For when you just want to drive one motor.
// L298Nsingle(<transmitter channel>,<PWM pin>, <positive pin>, <negative pin>, <% of max motor strength>)
MotorDrv::L298Nsingle(int L298Ntc, int pwm, int pos, int neg, float L298Nintensity ){
    intensity = (intensity * .01) ; // Turning intensity into an easy to use multiplier
    
    // Mapping motor movement to transmitter stick values.
    // Constraints help even out the motor performance when the robot is backing up. Without this constraint the motors stop behaving when I reverse the motor, especially at the lowest end.
    L298Ncontrol = constrain(map(L298Ntc, 1040, 1881, -255, 255), round(-100*L298Nintensity), round(255*L298Nintensity) ) ;
    
    if(control > 10){
          digitalWrite(pos, HIGH) ;		// "+" on driver
          digitalWrite(neg, LOW) ;		// "-" on driver
          analogWrite(pwm, L298Ncontrol) ;		// This is pulse width! Send to appropriate spot on driver or nothing will happen!
        }
      if(control < -10){
          digitalWrite(pos, LOW) ;
          digitalWrite(neg, HIGH) ;
          analogWrite(pwm, L298Ncontrol) ;
        }
        // Now I want to make a spot where there is absolutely no movement at all in the motor. I'll tell the arduino to feed the motors no current if the transmitter vals are within a certain domain.
      if(-10<=control && control<=10){
          digitalWrite(lpos, LOW) ;
          digitalWrite(lneg, HIGH) ;
          analogWrite(lpwm, 0) ;
        }

  }

// This controls two DC motors using two  BTS7960 43 amp motor drivers. 
// Usage: MotorDrv.IBT( <left transmitter channel>, <right transmitter channel>, <motor 1 Lpwm>, <motor 1 Rpwm>, <motor 1 L enable >, <motor 1 R enable>, <motor 2 Lpwm>, <motor 2 Rpwm>, motor 2 L enable >, <motor 2 R enable>, <% of max possible motor strength>)
MotorDrv::IBT2(int ls, int rs, int m1pwml, int m1pwmr, int m1enl, int m1enr, int m2pwml, int m2pwmr, int m2enl, int m2enr, float intensity ){

    intensity = (intensity * .01) ; // Turning intensity into an easy to use multiplier
    
    // Mapping motor movement to transmitter stick values.
    // Constraints help even out the motor performance when the robot is backing up. Without this constraint the motors stop behaving when I reverse the motor, especially at the lowest end.
    ls = constrain(map(ls, 1040, 1881, -255, 255), round(-150*intensity), round(255*intensity) ) ;
    rs = constrain(map(rs, 1040, 1881, -255, 255), round(-150*intensity), round(255*intensity) ) ;
	
	  // Forward movement (Motor #1)
      if(ls > 10){
          analogWrite(m1pwml, ls) ; // This is pulse width! Send to appropriate spot on driver or nothing will happen!
          analogWrite(m1pwmr, 0) ;
          digitalWrite(m1enl, HIGH) ;
          digitalWrite(m1enr, HIGH) ;
        }
      // Backward Movement (Motor #1)
      if(ls < -10){
          analogWrite(m1pwml, 0) ;
          analogWrite(m1pwmr, ls) ;
          digitalWrite(m1enl, HIGH) ;
          digitalWrite(m1enr, HIGH) ;
        }
      // Now I want to make a spot where there is absolutely no movement at all in the motor. I'll tell the arduino to feed the motors no current if the transmitter vals are within a certain domain. (Motor #1)
      if(-10<=ls && ls<=10){
          analogWrite(m1pwml, 0) ;
          analogWrite(m1pwmr, 0) ;
          digitalWrite(m1enl, LOW) ;
          digitalWrite(m1enr, LOW) ;
        }

	  // Now for motor #2. Same story of the stuff above for motor #1
      if(rs > 10){
          analogWrite(m2pwml, ls) ;
          analogWrite(m2pwmr, 0) ;
          digitalWrite(m2enl, HIGH) ;
          digitalWrite(m2enr, HIGH) ;
        }
      if(rs < -10){
          analogWrite(m2pwml, 0) ;
          analogWrite(m2pwmr, ls) ;
          digitalWrite(m2enl, HIGH) ;
          digitalWrite(m2enr, HIGH) ;
        }
      if(-10<=rs && rs<=10){
          analogWrite(m2pwml, 0) ;
          analogWrite(m2pwmr, 0) ;
          digitalWrite(m2enl, LOW) ;
          digitalWrite(m2enr, LOW) ;
        }

  }

// When you only want to control one motor with one IBT2 module.
// IBT2single(<transmitter channel>, <left side PWM pin>,<right side PWM pin>,<left side enable pin>,<right side enable pin>,<% of max motor strength>)
MotorDrv::IBT2single(int tc, int pwml, int pwmr, int enl, int enr, float IBT2intensity ){

    intensity = (intensity * .01) ; // Turning intensity into an easy to use multiplier
    
    // Mapping motor movement to transmitter stick values.
    // Constraints help even out the motor performance when the robot is backing up. Without this constraint the motors stop behaving when I reverse the motor, especially at the lowest end.
    IBT2control = constrain(map(tc, 1040, 1881, -255, 255), round(-150*IBT2intensity), round(255*IBT2intensity) ) ;
	
	  // Forward movement (Motor #1)
      if(ls > 10){
          analogWrite(pwml, IBT2control) ; // This is pulse width! Send to appropriate spot on driver or nothing will happen!
          analogWrite(pwmr, 0) ;
          digitalWrite(enl, HIGH) ;
          digitalWrite(enr, HIGH) ;
        }
      // Backward Movement (Motor #1)
      if(ls < -10){
          analogWrite(pwml, 0) ;
          analogWrite(pwmr, IBT2control) ;
          digitalWrite(enl, HIGH) ;
          digitalWrite(enr, HIGH) ;
        }
      // Now I want to make a spot where there is absolutely no movement at all in the motor. I'll tell the arduino to feed the motors no current if the transmitter vals are within a certain domain. (Motor #1)
      if(-10<=ls && ls<=10){
          analogWrite(pwml, 0) ;
          analogWrite(pwmr, 0) ;
          digitalWrite(enl, LOW) ;
          digitalWrite(enr, LOW) ;
        }
        
	  // If you wanna stop a motor from moving once you cut off all control signals, this is the way. Input is simply pwm pin of the motor you want to stop.
MotorDrv::motorKill(int pwmPinToKill){
		analogWrite(pwmPinToKill, 0) ;
	}
