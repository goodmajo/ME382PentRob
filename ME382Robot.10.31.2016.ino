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
 *   5. As of now, the drive motors are controlled by L298Ns. Bigger, beefier, drivers might be needed to push a 25 lb robot. 
 */
 
#include <math.h> // I need this to round intensity values

// Defining inputs. It'll make it easier to change pins if needed. Probably won't use all of these but whatever.
#define xmch1_INPUT 1 
#define xmch2_INPUT 4 
#define xmch3_INPUT 5 
#define xmch4_INPUT 2 
#define xmch5_INPUT 31 
#define xmch6_INPUT 33 
//#define xmch7_INPUT 35 
//#define xmch8_INPUT 8

// Now for the outputs.
#define ldpwm 11      // Left drive PWM
#define ldp 48        // Left drive positive
#define ldn 49        // Left drive negative
#define rdpwm 12
#define rdp 50
#define rdn 51
#define kpwm 13       // Kick PWM
#define kp 52         // Kick positive
#define kn 53         // Kick negative
//#define ppwm         // Pull motor PWM
//#define pp           // Pull positive
//#define pn           // Pull negative
//#define brpwm        // Ball retention PWM
//#define brp          // Ball retention positive
//#define bpn          // Ball retention negative
//#define liftpwm      // Lift pwm
//#define liftp        // Lift positive
//#define liftn        // Lift negative

// Initializing variables at the top like a good little programmer
// xmchn = "transmitter channel n". This is where I'll store transmitter channel values, which will then be changed into usable control values. 
int xmch2 ; // Left stick
int xmch3 ; // Right stick
int xmch5 ; // Left front bottom switch ("THRO HOLD")
int xmch6 ; // Right front top switch ("GEAR")
//int xmch7 ; // Right front knob ("HOV THRO"). You probably don't need this, dude.

int lmm ; // Left motor movements
int rmm ; // Right motor movements

// These next variables are booleans that I'll use when switching modes
bool lfbs ; // left front bottom switch
bool rfts ; // right front top switch

void setup() {

  Serial.begin(57600) ; // Parameter is baud rate.

  // Reciever inputs go here.
  pinMode(xmch2_INPUT, INPUT) ; // Left Stick y-axis
  pinMode(xmch3_INPUT, INPUT) ; // Right Stick y-axis
  pinMode(xmch5_INPUT, INPUT) ; // Bottom left front switch, 2 way toggle.
  pinMode(xmch6_INPUT, INPUT) ; // Top right front switch, 2 way toggle.
  //pinMode(xmch7_INPUT, INPUT) ; // Right front knob, big range of values
  
  // Left motor, sprint n' climb
  pinMode(ldp, OUTPUT) ;
  pinMode(ldn, OUTPUT) ;
  pinMode(ldpwm, OUTPUT) ;

  // Right motor, sprint n' climb
  pinMode(rdp, OUTPUT) ;
  pinMode(rdn, OUTPUT) ;
  pinMode(rdpwm, OUTPUT) ;

  // Motor 1, Kick n' throw
  pinMode(kp, OUTPUT) ;
  pinMode(kn, OUTPUT) ;
  pinMode(kpwm, OUTPUT) ;

  Serial.print("Setup is complete!\n") ; // Confirms we've made it to the end without issue and prints affirmation to serial.

}
  
void loop() {

  // Code below reads the pulse width of each channel.
  xmch2 = pulseIn(xmch2_INPUT, HIGH, 100000) ; //Final number is timeout in microseconds. Adjust as necessary.
  xmch3 = pulseIn(xmch3_INPUT, HIGH, 100000) ;
  xmch5 = pulseIn(xmch5_INPUT, HIGH, 100000) ;
  xmch6 = pulseIn(xmch6_INPUT, HIGH, 100000) ;
  //xmch7 = pulseIn(xmch7_INPUT, HIGH, 100000) ;
  
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
    // STOP the drive motors
    analogWrite(ldpwm, 0) ;
    analogWrite(rdpwm, 0) ;
    // STOP the kick motor
    analogWrite(kpwm, 0) ;
    }
 /*
  **********************************************************************************************
  *                 THE SPRINT AND CLIMB                                                       *
  **********************************************************************************************                 
  */
  else if(lfbs == false && rfts == false){
      Serial.print("Sprint and Climb mode\n") ;
      // STOP Kick spin
      analogWrite(kpwm, 0) ;
      //DCmotor( <left transmitter channel>, <right transmitter channel>, <left pwm>, <left +>, <left ->, <right pwm>, <right +>, <right ->, <% of max possible motor strength>)
     DCmotor(xmch2,xmch3,ldpwm,ldp,ldn,rdpwm,rdp,rdn,100) ;
  }
/**************************************************************************************************
 *                  THE KICK AND THROW                                                            *
 * ************************************************************************************************
*/
    else if(lfbs == true && rfts == false){
       Serial.print("Kick and Throw mode\n") ;

       // STOP drive motors
       analogWrite(ldpwm, 0) ;
       analogWrite(rdpwm, 0) ;
       
       if(lmm > 10){
          digitalWrite(kp, HIGH) ;
          digitalWrite(kn, LOW) ;
          analogWrite(kpwm, 0) ;
          }
       if(lmm < -10){
          digitalWrite(kp, LOW) ;
          digitalWrite(kn, HIGH) ;
          analogWrite(kpwm, 0) ;
          }
          // No movement at all in the motor.
       if(-10<=lmm && lmm<=10){
            digitalWrite(kp, LOW) ;
            digitalWrite(kn, HIGH) ;
            analogWrite(kpwm, 0) ;
          }   
    }
/**************************************************************************************************
 *                  THE LIFT                                                                      *
 * ************************************************************************************************
*/
      else if(lfbs == false && rfts == true){
        Serial.print("Lift mode\n") ;
        
        }
/************************************************************************************************** 
 *                  FOURTH MODE                                                                   *
 **************************************************************************************************
 */
        else(Serial.print("Nothing to see here\n")) ;
}

/*
 *  <<<DCmotor function>>>
 *  Inputs are:
 *    * control values from transmitter
 *    * pwm pin assignments
 *    * positive and negative pin assignments
 *    
 *  Outputs are:
 *    * values sent to L298N used to drive motors
 *  
 *  This function will move motors back and forth, in forward and reverse based on transmitter values. It will also include a "dead zone", where motors do nothing. It is meant to deal with one L298N at a time. In other words, there will be one instance of this function in use per L298N motor driver, and many can be used simultaneously.
 *
 *  Usage : L298NDCmotor( <left transmitter channel>, <right transmitter channel>, <left pwm>, <left +>, <left ->, <right pwm>, <right +>, <right ->, <% of max possible motor strength>)
 */

void DCmotor(int ltc, int rtc, int lpwm, int lpos, int lneg, int rpwm, int rpos, int rneg, float intensity){

    int ls ;
    int rs ;
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
          analogWrite(lpwm, ls) ;
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
