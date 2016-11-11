/*
 * RC Control of Arduino Mega knockoff-based robot for ME 382 by Joel R. Goodman
 * Aux switches on transmitter control modes of operation, which more or less just change which motors the thumbsticks operate.
 * 
 * IMPORTANT NOTES TO SELF:
 *  1. Not all pins on the Arduino are created equal. Just because it's an I/O pin doesn't mean it can send or receive PWM. Trust me, I learned this the hard way.
 *  2. Pre-defining input and output variables make it MUCH easier to switch pins later on down the line. Plus, it makes the code a little more portable.
 *  3. The intended transmitter for this project is a Turnigy 9x or OEM clone thereof. It's worth looking into whether or not other transmitters will work with this code.
 *  4. Some info on raw transmitter input that might be useful later on: 
 *      Transmitter sticks have a ceiling value range of........ 1501 - 1881
 *      Transmitter sticks are centered at about................ 1400 - 1500
 *      Transmitter sticks have a floor value range of.......... 1040 - 1399
 *      Two way toggles have floor value of..................... 1048
 *      Two way toggles have ceiling value of....................1883
 *      Knobs have a floor value of .............................1060
 *      Knobs have a ceiling value of............................1887
 *   5. As of now, the drive motors are controlled by L298Ns. Bigger, beefier drivers might be needed to push a 25 lb robot. 
 */
#include <MotorDrv.h> // Sweet custom class I made.
#include <Stepper.h> // Stepper library. 

// Defining inputs. It'll make it easier to change pins if needed. Probably won't use all of these but whatever.
#define xmch1_INPUT 1 
#define xmch2_INPUT 4 
#define xmch3_INPUT 5 
#define xmch4_INPUT 2 
#define xmch5_INPUT 31 
#define xmch6_INPUT 33 
#define xmch7_INPUT 35

// Now for the outputs.
#define ldpwm 11      // Left drive PWM
#define ldp 48        // Left drive positive
#define ldn 49        // Left drive negative
#define rdpwm 12
#define rdp 50
#define rdn 51
#define kpwm 9       // Kick PWM
#define kp 22         // Kick positive
#define kn 23         // Kick negative
#define ppwm 10        // Pull motor PWM
#define pp 24          // Pull positive
#define pn 25          // Pull negative
#define liftInt1 40   // Lift Int1 on L298N
#define liftInt2 41   // Lift Int2 on L298N
#define liftInt3 42   // Lift Int3 on L298N
#define liftInt4 43   // Lift Int4 on L298N

// Initializing variables at the top like a good little programmer
// xmchn = "transmitter channel n". This is where I'll store transmitter channel values, which will then be changed into usable control values. 
int xmch2 ; // Left stick
int xmch3 ; // Right stick
int xmch5 ; // Left front bottom switch ("THRO HOLD")
int xmch6 ; // Right front top switch ("GEAR")
int xmch7 ; // Right front knob ("HOV THRO"). I will use this to manually adjust max and min threshold, maybe. Might actually be dangerous. We'll see.

// Now for the stepper.
const int stepsPerRevolution = 200;  // This should not change for most of the steppers in our price range.
Stepper lift(stepsPerRevolution, liftInt1, liftInt2, liftInt3, liftInt4) ;
int liftstick ; // This will be used to store usable signals from the transmitter for the lift stepper

// These next variables are booleans that I'll use when switching modes
bool lfbs ; // left front bottom switch
bool rfts ; // right front top switch

MotorDrv MotorDrv ; // In order to use my sweet custom class you need to declare an instance of it. I dunno why. I will learn about this later.

void setup() {

  Serial.begin(57600) ; // Parameter is baud rate.

  // Reciever inputs go here.
  pinMode(xmch2_INPUT, INPUT) ; // Left Stick y-axis
  pinMode(xmch3_INPUT, INPUT) ; // Right Stick y-axis
  pinMode(xmch5_INPUT, INPUT) ; // Bottom left front switch, 2 way toggle.
  pinMode(xmch6_INPUT, INPUT) ; // Top right front switch, 2 way toggle.
  pinMode(xmch7_INPUT, INPUT) ; // Right front knob, big range of values
  
  // Left motor, sprint n' climb
  pinMode(ldp, OUTPUT) ;
  pinMode(ldn, OUTPUT) ;
  pinMode(ldpwm, OUTPUT) ;

  // Right motor, sprint n' climb
  pinMode(rdp, OUTPUT) ;
  pinMode(rdn, OUTPUT) ;
  pinMode(rdpwm, OUTPUT) ;

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
  xmch7 = pulseIn(xmch7_INPUT, HIGH, 100000) ;
  
  // The next block of code is for testing whether or not the MC is taking in pulse data from the receiver. Use in conjunction with serial monitor, dummy. And comment it out if you aren't actually using it.

  //Serial.print("Transmitter channel 7:");
  //Serial.println(xmch7) ;
  //delay(200) ;  //Like wait() in C/C++. Parameter in milliseconds. Adjust as necessary.

/*
 * I am now going to create 3 modes of operation with if, else if, and else statements. I don't know if there's a better way to do this, but it will do for now.
 * I'll make the booleans do their true/false thing to make this a little easier to read
 */
  
  if(xmch5 < 1466){lfbs = false;}
  else(lfbs = true) ;

  if(xmch6 < 1466){rfts = false;}
  else(rfts = true) ;

/*
 *********************************************************************************************** 
 *                  ROBOT OPERATION TIME!!!                                                    *
 *                  ROBOT OPERATION TIME!!!                                                    *
 *                  ROBOT OPERATION TIME!!!                                                    *
 *                  ROBOT OPERATION TIME!!!                                                    *
 ***********************************************************************************************
 */
 
 /*First, if the transmitter is turned off, we want to the motors to do nothing.
  * Before I put this if statement in, the drive motors would go apeshit if the system was on and the transmitter was off. 
  * What this does is tell the motors to do nothing if the thumbsticks are sending values of 0 to the receiver, which only happens if the transmitter is turned off. 
  */

 if(xmch2 == 0 && xmch3 == 0){
    Serial.print("Transmitter is not on!\n") ;
    // MotorDrv.motorKill( <PWM pin of motor you want to stop> )
    // STOP the drive motors
    MotorDrv.motorKill(ldpwm) ;
    MotorDrv.motorKill(rdpwm) ;
    // STOP the kick motor
    MotorDrv.motorKill(kpwm) ;
    
    }
 /*
  **********************************************************************************************
  *                 THE SPRINT AND CLIMB                                                       *
  **********************************************************************************************                 
  */

  else if(lfbs == false && rfts == false){
      Serial.print("Sprint and Climb mode\n") ;
      // STOP Kick spin
      MotorDrv.motorKill(kpwm) ;   
      
      // Spin the motors! Left stick is the kick, right stick is the pull. Reposting function usage below:
      // MotorDrv.L298N( <left transmitter channel>, <right transmitter channel>, <left pwm>, <left +>, <left ->, <right pwm>, <right +>, <right ->, <% of max possible motor strength>)
      MotorDrv.L298N(xmch2,xmch3,ldpwm,ldp,ldn,rdpwm,rdp,rdn,100) ;
     
  }

/**************************************************************************************************
 *                  THE KICK AND THROW                                                            *
 * ************************************************************************************************
*/

 else if(lfbs == true && rfts == false){
       Serial.print("Kick and Throw mode\n") ;

       // STOP drive motors
       MotorDrv.motorKill(ldpwm) ;
       MotorDrv.motorKill(rdpwm) ;

      // MotorDrv.L298N( <left transmitter channel>, <right transmitter channel>, <left pwm>, <left +>, <left ->, <right pwm>, <right +>, <right ->, <% of max possible motor strength>)
      MotorDrv.L298N(xmch2,xmch3,kpwm,kp,kn,ppwm,pp,pn,100) ;
          }   
    

/**************************************************************************************************
 *                  THE LIFT                                                                      *
 * ************************************************************************************************
*/
      else if(lfbs == false && rfts == true){
        Serial.print("Lift mode\n") ;
        
    // Mapping motor movement to transmitter stick values.
    // Constraints help even out the motor performance when the robot is backing up. Without this constraint the motors stop behaving when I reverse the motor, especially at the lowest end.
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
}
/************************************************************************************************** 
 *                  FOURTH MODE                                                                   *
 **************************************************************************************************
 */
 /*
        else(Serial.print("Nothing to see here\n")) ;

*/
