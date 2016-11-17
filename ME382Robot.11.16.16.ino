/* 
 * RC Control of Arduino Mega knockoff-based robot for ME 382 by Joel R. Goodman.
 * goodmajo@oregonstate.edu
 * Now supports L298N and IBT_2 motor drivers via MotorDrv class.
 * Everything is controlled with the y-axes of the thumbsticks. Aux switches on transmitter control modes of operation, which just change which motors the thumbsticks operate.
 * 
 * Some info on raw transmitter input that might be useful later on: 
 *      Transmitter sticks have a top range of.................. 1501 - 1881
 *      Transmitter sticks are centered at about................ 1400 - 1500
 *      Transmitter sticks have a bottom value range of......... 1040 - 1399
 *      Two way toggles have floor value of..................... 1048
 *      Two way toggles have ceiling value of....................1883
 *      Knobs have a floor value of .............................1060
 *      Knobs have a ceiling value of............................1887
 */
 
#include <MotorDrv.h>  // Sweet custom class I made.
#include <Stepper.h>  // Stepper library. 

// NOTE: I use #define here rather than static const because it takes up zero memory. Probably makes no real difference in the long run, though.
// First the inputs.
#define xmch2_INPUT 4 
#define xmch3_INPUT 5
#define xmch5_INPUT 2 
#define xmch6_INPUT 3 

// Now for the outputs.
#define ldpwml 10     // Left drive PWM (to LPWM on IBT_2)
#define ldpwmr 11     // Left drive PWM (to RPWM on IBT_2)
#define ldenl 31      // Left drive left enable
#define ldenr 32      // Left drive right enable
#define rdpwml 12     // Right drive PWM (to LPWM on IBT_2)
#define rdpwmr 13     // Right drive PWM (to RPWM on IBT_2)
#define rdenl  33     // Right drive left enable
#define rdenr  34     // Right drive right enable
#define kpwm 9        // Kick PWM
#define kp 22         // Kick positive
#define kn 23         // Kick negative
#define ppwm 8        // Pull motor PWM
#define pp 24         // Pull positive
#define pn 25         // Pull negative
#define liftInt1 40   // Lift Int1 on L298N
#define liftInt2 42   // Lift Int2 on L298N
#define liftInt3 44   // Lift Int3 on L298N
#define liftInt4 46   // Lift Int4 on L298N

// Initializing variables at the top like a good little programmer
// xmchn = "transmitter channel n". This is where I'll store transmitter channel values, which will then be changed into usable control values. 
int xmch2 ; // Left stick
int xmch3 ; // Right stick
int xmch5 ; // Left front bottom switch ("THRO HOLD")
int xmch6 ; // Right front top switch ("GEAR")

// Now for the stepper. Documentation on these functions is very well documented and I won't be using them in any revolutionary new fashion.
const int stepsPerRevolution = 200;
Stepper lift(stepsPerRevolution, liftInt1, liftInt2, liftInt3, liftInt4) ;
int liftstick ;

// These next variables are booleans that I'll use when switching modes
bool lfbs ; // left front bottom switch
bool rfts ; // right front top switch

MotorDrv robot ; // Making a motor control object.

void setup() {

  Serial.begin(57600) ; // Parameter is baud rate.

  // Reciever inputs go here.
  pinMode(xmch2_INPUT, INPUT) ; // Left Stick y-axis
  pinMode(xmch3_INPUT, INPUT) ; // Right Stick y-axis
  pinMode(xmch5_INPUT, INPUT) ; // Bottom left front switch, 2 way toggle.
  pinMode(xmch6_INPUT, INPUT) ; // Top right front switch, 2 way toggle.
  
  // Left motor, sprint n' climb
  pinMode(ldpwml, OUTPUT) ;
  pinMode(ldpwmr, OUTPUT) ;

  // Right motor, sprint n' climb
  pinMode(rdpwml, OUTPUT) ;
  pinMode(rdpwmr, OUTPUT) ;

  // Motor that kicks n' throws
  pinMode(kp, OUTPUT) ;
  pinMode(kn, OUTPUT) ;
  pinMode(kpwm, OUTPUT) ;

  // Motor that pulls the other motor during kick n' throw
  pinMode(pp, OUTPUT) ;
  pinMode(pn, OUTPUT) ;
  pinMode(ppwm, OUTPUT) ;

  // Stepper, for the lift
  lift.setSpeed(60);
  
  Serial.print("Setup is complete!\n") ; // Confirms we've made it to the end without issue and prints affirmation to serial.

}

void loop() {

  // Code below reads the pulse width of each channel.
  xmch2 = pulseIn(xmch2_INPUT, HIGH, 100000) ; //Final number is timeout in microseconds. Adjust as necessary.
  xmch3 = pulseIn(xmch3_INPUT, HIGH, 100000) ;
  xmch5 = pulseIn(xmch5_INPUT, HIGH, 100000) ;
  xmch6 = pulseIn(xmch6_INPUT, HIGH, 100000) ;
  
/*
 * I am now going to create 3 modes of operation with if, else if, and else statements. I don't know if there's a better way to do this, but it will do for now.
 * I'll make the booleans do their true/false thing to make this a little easier to read
 */
  
  if(xmch5 < 1466){lfbs = false;}
  else(lfbs = true) ;

  if(xmch6 < 1466){rfts = false;}
  else(rfts = true) ;

/*
 ********************************************************************************************** 
 *                  ROBOT OPERATION TIME!!                                                    *
 *                  ROBOT OPERATION TIME!!                                                    *
 *                  ROBOT OPERATION TIME!!                                                    *
 *                  ROBOT OPERATION TIME!!                                                    *
 **********************************************************************************************
 */
 
 /* First, if the transmitter is turned off, we want to the motors to do nothing.
  * Before I put this if statement in, the drive motors would go apeshit if the system was on and the transmitter was off. 
  * What this does is tell the motors to do nothing if the thumbsticks are sending values of 0 to the receiver, which only happens if the transmitter is turned off. 
  */

  if(xmch2 == 0 && xmch3 == 0){
    Serial.print("Transmitter is not on!\n") ;
    // MotorDrv.motorKill( <PWM pin of motor you want to stop> )
    // STOP the drive motors
    robot.motorKill(ldpwml) ;
    robot.motorKill(ldpwmr) ;
    robot.motorKill(rdpwml) ;
    robot.motorKill(rdpwmr) ;
    // STOP the kick/throw and pull motors
    robot.motorKill(kpwm) ;
    robot.motorKill(ppwm) ;
    // STOP stepper
    lift.step(0);
    }
 /*
  **********************************************************************************************
  *                 THE SPRINT AND CLIMB                                                       *
  **********************************************************************************************                 
  */

  else if(lfbs == false && rfts == false){
    Serial.print("Sprint and Climb mode\n") ;
    // STOP the kick/throw and pull motors
    robot.motorKill(kpwm) ;
    robot.motorKill(ppwm) ;
    // STOP stepper
    lift.step(0);
     
    // Drive the robot!
    // MotorDrv.IBT2( <left transmitter channel>, <right transmitter channel>, <motor 1 Lpwm>, <motor 1 Rpwm>, <motor 1 L enable >, <motor 1 R enable>, <motor 2 Lpwm>, <motor 2 Rpwm>, motor 2 L enable >, <motor 2 R enable>, <% of max possible motor strength>)
    robot.IBT2( xmch2, xmch3, ldpwml, ldpwmr, ldenl, ldenr, rdpwml, rdpwmr, rdenl, rdenr, 100 ) ;     
    }

/**************************************************************************************************
 *                  THE KICK AND THROW                                                            *
 * ************************************************************************************************
*/

  else if(lfbs == true && rfts == false){
    Serial.print("Kick and Throw mode\n") ;
    // STOP drive motors
    robot.motorKill(ldpwml) ;
    robot.motorKill(ldpwmr) ;
    robot.motorKill(rdpwml) ;
    robot.motorKill(rdpwmr) ;
    // STOP stepper
    lift.step(0);

    // Spin the motors!
    // MotorDrv.L298N( <left transmitter channel>, <right transmitter channel>, <left pwm>, <left +>, <left ->, <right pwm>, <right +>, <right ->, <% of max possible motor strength>)
    robot.L298N( xmch2, xmch3, kpwm, kp, kn, ppwm, pp, pn, 100 ) ;
  }   
    

/**************************************************************************************************
 *                  THE LIFT                                                                      *
 * ************************************************************************************************
*/
  else if(lfbs == false && rfts == true){
    Serial.print("Lift mode\n") ;
    // STOP drive motors
    robot.motorKill(ldpwml) ;
    robot.motorKill(ldpwmr) ;
    robot.motorKill(rdpwml) ;
    robot.motorKill(rdpwmr) ;
    // STOP the kick/throw and pull motors
    robot.motorKill(kpwm) ;
    robot.motorKill(ppwm) ;
    
    // Mapping motor movement to transmitter stick values.
    liftstick = map(xmch2, 1040, 1880, -200, 200) ;
    
      if(liftstick > 10 || liftstick < -10){
      lift.step(liftstick);
      delay(500) ;
      }
      
      // Now I want to make a spot where there is absolutely no movement at all in the motor. I'll tell the arduino to feed the motors no current if the transmitter vals are within a certain domain.
      if(-10<=liftstick && liftstick<=10){
      lift.step(0);
      delay(500) ;
      }
  }
  
/************************************************************************************************** 
 *                  FOURTH MODE                                                                   *
 **************************************************************************************************
 */
 
  else(Serial.print("Nothing to see here\n")) ;

}
