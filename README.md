#ME382PentRob
It is very important that anyone that reads this and looks at my code understand that I DO NOT KNOW WHAT I'M DOING HERE. I've never programmed for this type of thing before, and this is my attempt at learning. If you know about Arduino programming and want to tell me how wrong I am about any of this stuff, please do not hesitate to do so.

  Joel Goodman
  goodmajo@oregonstate.edu

##10 - 31 - 2016##
* I'm using a function called DCmotor to take all the controller inputs, turn them into values the driver can use, and then make the motors do their thing. This seems to me like it will improve the overall tidiness of the code and make it easier to write.
* I'm going to make a function that easily stops a pair of motors, probably once construction of the robot commences.
* I'm trying my hand at making a library to do what these functions I'm writing do. It's not quite there yet, but it's close.
* I still do not know how to use git. I'm working on this.

## 11 - 10 - 2016 ##
This time around there were pretty big changes. It took me forever to get these changes done because of school ,waiting for parts, etc, but I'm finally ready to post new stuff.
* I made a class (MotorDrv) to deal with driving the motors. So far I just have functions to kill the motors (motorKill) and to drive DC motors with the (L298N) module. I'll add more motor drivers to this class as I'm able to get my hands on them. In any event, the code is a tad less monolithic now and *much* easier to read.
* I started using a stepper to perform one function. For that I just use the stepper library everyone uses. Again, if I can figure out how they work I'll add steppers to my MotorDrv class.

## 11 - 11 - 2016 ##
I added support for the BTS7960 (aka IBT_2) motor drivers I'm using to control the drive motors. It's very easy to use. Keep in mind that these drivers do not require a command to the enable pins like the L298N. Instead, I'm connecting my enable pins to the a bus that is connected to the Arduino's 5V out pin.

-Joel
