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
#define ldpwm 11  // Left drive PWM
#define ldp 48    // Left drive positive
#define ldn 49    // Left drive negative
#define rdpwm 12
#define rdp 50
#define rdn 51
#define kpwm 13   // Kick PWM
#define kp 52     // Kick positive
#define kn 53     // Kick negative
//#define ppwm      // Pull motor PWM
//#define pp        // Pull positive
//#define pn        // Pull negative
//#define lpwm      // Lift pwm
//#define lp        // Lift positive
//#define ln        // Lift negative

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

  //Serial.print("Transmitter channel 2 (left stick y-axis):");
  //Serial.println(xmch2) ;
  //Serial.print("Transmitter channel 3 (right stick y-axis):");
  //Serial.println(xmch3) ;
  //Serial.print("Transmitter channel 5:");
  //Serial.println(xmch5) ;
  //Serial.print("Transmitter channel 6:");
  //Serial.println(xmch6) ;
  //Serial.print("Transmitter channel 7:");
  //Serial.println(xmch7) ;
  //delay(200) ;  //Like wait() in C/C++. Parameter in milliseconds. Adjust as necessary.
 

  // Mapping motor movement to transmitter stick values.
  // Going nested with this. Seems to improve performance.
  // Constraints help even out the motor performance when the robot is backing up. Without this constraint the motors stop behaving when I reverse the motor, especially at the lowest end.
 // lmm = map(xmch2, 1040, 1881, -255, 255) ; //Left motor moves
 // rmm = map(xmch3, 1040, 1881, -255, 255) ; //Right motor moves
  lmm = constrain(map(xmch2, 1040, 1881, -255, 255), -100, 255 ) ;
  rmm = constrain(map(xmch3, 1040, 1881, -255, 255), -100, 255 ) ;

  // Serial stuff to test out lmm...
  //Serial.print("Left Stick = \n") ;
  //Serial.print(lmm) ;
  //Serial.print("Right stick = \n") ;
  //Serial.print(rmm) ;
  //delay(200) ;

/*
 * I am now going to create 3 modes of operation with if, else if, and else statements. I don't know if there's a better way to do this, but it will do for now.
 * The way it works is the MC will have the thumbsticks control:
 * the drive wheels if both AUX switches are off (read: xmch5 = xmch6 < 1466)
 * the throw/kick if xmch5 is on and xmch6 is off (read: xmch5 > 1466 && xmch6 < 1466)
 * the lift if both xmch5 and xmch6 are on (read: xmch5 = xmch6 > 1466)
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
    // STOP the left drive motor
    digitalWrite(ldp, LOW) ;
    digitalWrite(ldn, LOW) ;
    analogWrite(ldpwm, 0) ;
    // STOP the right drive motor
    digitalWrite(rdp, LOW) ;
    digitalWrite(rdn, LOW) ;
    analogWrite(rdpwm, 0) ;
    // STOP Kick spin
    digitalWrite(kp, LOW) ;
    digitalWrite(kn, LOW) ;
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
      digitalWrite(kp, LOW) ;
      digitalWrite(kn, LOW) ;
      analogWrite(kpwm, 0) ;

      // Now I'm going to send signals to left motor driver
      if(lmm > 10){
          digitalWrite(ldp, HIGH) ; // "+" on driver
          digitalWrite(ldn, LOW) ; // "-" on driver
          analogWrite(ldpwm, lmm) ; // This is pulse width! Send to appropriate spot on driver or nothing will happen!
        }
      if(lmm < -10){
          digitalWrite(ldp, LOW) ;
          digitalWrite(ldn, HIGH) ;
          analogWrite(ldpwm, lmm) ;
        }
        // Now I want to make a spot where there is absolutely no movement at all in the motor. I'll tell the arduino to feed the motors no current if the transmitter vals are within a certain domain.
      if(-10<=lmm && lmm<=10){
          digitalWrite(ldp, LOW) ;
          digitalWrite(ldn, HIGH) ;
          analogWrite(ldpwm, 0) ;
        }
    
      // And now for the right motor
      if(rmm > 10){
          digitalWrite(rdp, HIGH) ;
          digitalWrite(rdn, LOW) ;
          analogWrite(rdpwm, rmm) ;
        }
      if(rmm < -10){
          digitalWrite(rdp, LOW) ;
          digitalWrite(rdn, HIGH) ;
          analogWrite(rdpwm, rmm) ;
        }
        // Same deal as above with the left motor. I'm making a spot where there is absolutely no movement at all in the motor. I'll tell the arduino to feed the motors no current if the transmitter vals are within a certain domain.
      if(-10<=rmm && rmm<=10){
          digitalWrite(rdp, LOW) ;
          digitalWrite(rdn, HIGH) ;
          analogWrite(rdpwm, 0) ;
        }
  }
/**************************************************************************************************
 *                  THE KICK AND THROW                                                            *
 * ************************************************************************************************
*/
    else if(lfbs == true && rfts == false){
       Serial.print("Kick and Throw mode\n") ;
       
          // STOP the left drive motor
          digitalWrite(ldp, LOW) ;
          digitalWrite(ldn, LOW) ;
          analogWrite(ldpwm, 0) ;
          // STOP the right drive motor
          digitalWrite(rdp, LOW) ;
          digitalWrite(rdn, LOW) ;
          analogWrite(rdpwm, 0) ;
        
       if(lmm > 10){
          digitalWrite(kp, HIGH) ;
          digitalWrite(kn, LOW) ;
          analogWrite(kpwm, lmm) ;
          }
       if(lmm < -10){
          digitalWrite(kp, LOW) ;
          digitalWrite(kn, HIGH) ;
          analogWrite(kpwm, lmm) ;
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
