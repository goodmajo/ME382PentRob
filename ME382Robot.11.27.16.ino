/* 
 * RC Control of Arduino Mega knockoff-based robot for ME 382 
 * By Joel R. Goodman, October and November of 2016
 * goodmajo@oregonstate.edu
 * https://github.com/goodmajo/ME382PentRob
 * Supports L298N and IBT_2 motor drivers via MotorDrv class.
 * Everything is controlled with the y-axes of the thumbsticks. Aux switches on transmitter control modes of operation, which just change which motors the thumbsticks operate.
 */
 
#include <MotorDrv.h>  // Sweet custom class I made.
#include <Stepper.h>  // Stepper library. 

// NOTE: I use #define here rather than static const because it takes up zero memory. Probably makes no real difference in the long run, though.
// First the inputs.
#define xmch2_INPUT 2 
#define xmch3_INPUT 3
#define xmch5_INPUT 5 
#define xmch6_INPUT 6

// Define floor and ceiling values for the transmitter here. YOU MUST DETERMINE THESE ON YOUR OWN! They vary from transmitter to transmitter and receiver to receiver.
#define transMin 950
#define transMax 1700

// Now for the outputs.
#define ldpwml 10     // Left drive PWM (to LPWM on IBT_2)
#define ldpwmr 11     // Left drive PWM (to RPWM on IBT_2)
#define ldenl 30      // Left drive left enable
#define ldenr 31      // Left drive right enable

#define rdpwml 12     // Right drive PWM (to LPWM on IBT_2)
#define rdpwmr 13     // Right drive PWM (to RPWM on IBT_2)
#define rdenl  32     // Right drive left enable
#define rdenr  33     // Right drive right enable

#define kpwm 9        // Kick PWM
#define kp 22         // Kick positive
#define kn 23         // Kick negative

#define lpwm 8        // Lift motor PWM
#define lp 24         // Lift positive
#define ln 25         // Lift negative

#define pullIn1 40   // Pull Int1 on L298N
#define pullIn2 42   // Pull Int2 on L298N
#define pullIn3 44   // Pull Int3 on L298N
#define pullIn4 46   // Pull Int4 on L298N

// Initializing variables at the top like a good little programmer
// xmchn = "transmitter channel n". This is where I'll store transmitter channel values, which will then be changed into usable control values. 
int leftStick ;       // Left stick
int rightStick ;      // Right stick
int leftSwitch ;      // Left front bottom switch ("THRO HOLD")
int rightSwitch ;     // Right front top switch ("GEAR")

// Now for the stepper. This library is very well documented and I won't be using them in any revolutionary new fashion.
const int stepsPerRevolution = 96;
Stepper pull(stepsPerRevolution, pullIn1, pullIn2, pullIn3, pullIn4) ;
int pullMove ;        // This variable will store values I use to move the stepper that pulls the kick motor.

// These next variables are booleans that I'll use when switching modes
bool lfbs ;           // left front bottom switch
bool rfts ;           // right front top switch

MotorDrv robot ;      // Making a motor control object.

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
  pinMode(ldenl, OUTPUT) ;
  pinMode(ldenr, OUTPUT) ;

  // Right motor, sprint n' climb
  pinMode(rdpwml, OUTPUT) ;
  pinMode(rdpwmr, OUTPUT) ;
  pinMode(rdenl, OUTPUT) ;
  pinMode(rdenr, OUTPUT) ;

  // Motor that kicks n' throws
  pinMode(kp, OUTPUT) ;
  pinMode(kn, OUTPUT) ;
  pinMode(kpwm, OUTPUT) ;

  // Motor that lifts the weight
  pinMode(lp, OUTPUT) ;
  pinMode(ln, OUTPUT) ;
  pinMode(lpwm, OUTPUT) ;

  // Stepper, for the pull
  pinMode(pullIn1, OUTPUT) ;
  pinMode(pullIn2, OUTPUT) ;
  pinMode(pullIn3, OUTPUT) ;
  pinMode(pullIn4, OUTPUT) ;
  pull.setSpeed(60);
  
  Serial.print("Setup is complete!\n") ; // Confirms we've made it to the end without issue and prints affirmation to serial.

}

void loop() {

  // Code below reads the pulse width of each channel.
  leftStick = pulseIn(xmch2_INPUT, HIGH, 100000) ; //Final number is timeout in microseconds. Adjust as necessary.
  rightStick = pulseIn(xmch3_INPUT, HIGH, 100000) ;
  leftSwitch = pulseIn(xmch5_INPUT, HIGH, 100000) ;
  rightSwitch = pulseIn(xmch6_INPUT, HIGH, 100000) ;
  
  // The code below can be used to determine the top and bottom values for your thumbsticks and switches via serial monitor. It is probably a good idea to comment it out if you aren't using it.
  /*
  Serial.print("Left Stick = ") ;
  Serial.print(leftStick) ;
  Serial.print("\n") ;
  
  Serial.print("Right Stick = ") ;
  Serial.print(rightStick) ;
  Serial.print("\n") ;
  
  Serial.print("Left Switch = ") ;
  Serial.print(leftSwitch) ;
  Serial.print("\n") ;
  
  Serial.print("Right Switch = ") ;
  Serial.print(rightSwitch) ;
  Serial.print("\n") ;
  */
  
/*
 * I am now going to create 3 modes of operation with if, else if, and else statements. I don't know if there's a better way to do this, but it will do for now.
 * I'll make the booleans do their true/false thing to make this a little easier to read
 */
  
  if(leftSwitch < 1466){lfbs = false;}
  else(lfbs = true) ;

  if(rightSwitch < 1466){rfts = false;}
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

  if(leftStick == 0 && rightStick == 0){
    Serial.print("Transmitter is not on!\n") ;
    // robot.motorKill( <PWM pin of motor you want to stop> )
    // robot.stepperKill(<In1 pin on L298N>, <In2 pin on L298N>, <In3 pin on L298N>, <In4 pin on L298N>)
    // STOP the drive motors
    robot.motorKill(ldpwml) ;
    robot.motorKill(ldpwmr) ;
    robot.motorKill(rdpwml) ;
    robot.motorKill(rdpwmr) ;
    // STOP the kick/throw motors
    robot.motorKill(kpwm) ;
    // STOP the stepper
    robot.stepperKill(pullIn1, pullIn2, pullIn3, pullIn4) ;
    // STOP lift motor
    robot.motorKill(lpwm) ;
   
    }
 /*
  **********************************************************************************************
  *                 THE SPRINT AND CLIMB                                                       *
  **********************************************************************************************                 
  */

  else if(lfbs == false && rfts == false){
    Serial.print("Sprint and Climb mode\n") ;
    // STOP the kick/throw motor
    robot.motorKill(kpwm) ;
    // STOP the stepper
    robot.stepperKill(pullIn1, pullIn2, pullIn3, pullIn4) ;
    // STOP lift motor
    robot.motorKill(lpwm) ;
        
    // Drive the robot!
    // MotorDrv.IBT2( <left transmitter channel>, <right transmitter channel>, <motor 1 Lpwm>, <motor 1 Rpwm>, <motor 1 L enable >, <motor 1 R enable>, <motor 2 Lpwm>, <motor 2 Rpwm>, motor 2 L enable >, <motor 2 R enable>, <% of max possible motor strength>)
    robot.IBT2( transMin, transMax, leftStick, rightStick, ldpwml, ldpwmr, ldenl, ldenr, rdpwml, rdpwmr, rdenl, rdenr, 100 ) ;     
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
    // STOP lift motor
    robot.motorKill(lpwm) ;

    // Kick motor!
    // MotorDrv.L298Nsingle( <transmitter channel>, <pwm pin>, <+ pin>, <- pin>, <% of max possible motor strength>)
    robot.L298Nsingle( transMin, transMax, leftStick, kpwm, kp, kn, 100 ) ;

    // Pull the platform!
    // I am seriously limiting the steps taken by the stepper, just to start with. If it's too little, I can always step it up. Better safe then sorry!
    pullMove = map(rightStick, transMin, transMax, -stepsPerRevolution, stepsPerRevolution) ;
    if(pullMove > 10 || pullMove < -10){
    pull.step(pullMove) ;
    }
    else{
      robot.stepperKill(pullIn1, pullIn2, pullIn3, pullIn4) ;
    } ;
    
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
    // STOP the kick/throw motor
    robot.motorKill(kpwm) ;
    // STOP the stepper
    robot.stepperKill(pullIn1, pullIn2, pullIn3, pullIn4) ;

    // Start lifting!!!
    robot.L298Nsingle( transMin, transMax, leftStick, lpwm, lp, ln, 100) ;
    
  }
  
/************************************************************************************************** 
 *                  FOURTH MODE                                                                   *
 **************************************************************************************************
 */
 
  else{
    Serial.print("Nothing to see here\n");
    // STOP the drive motors
    robot.motorKill(ldpwml) ;
    robot.motorKill(ldpwmr) ;
    robot.motorKill(rdpwml) ;
    robot.motorKill(rdpwmr) ;
    // STOP the kick/throw motor
    robot.motorKill(kpwm) ;
    // STOP stepper
    robot.stepperKill(pullIn1, pullIn2, pullIn3, pullIn4) ;
    // STOP lift motor
    robot.motorKill(lpwm) ;
    }

}


