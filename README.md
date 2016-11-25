#ME382PentRob
I'm using this README to document changes and updates to this project. The latest and greatest is at the top and the old stuff is at the bottom.

It is very important that anyone that reads this and looks at my code understand that *I DO NOT KNOW WHAT I'M DOING HERE*. I've never programmed for this type of thing before, and this is my attempt at learning. If you know about Arduino programming or C++ and want to tell me how wrong I am about any of this stuff, please do not hesitate to do so.

  Joel Goodman
  
  goodmajo@oregonstate.edu

## 11 - 25 - 2016 ##
* Happy Thanksgiving!
* If anyone is actually paying attention to the arduino code I'm writing, the version I uploaded today is more or less what I anticipate will end up being the final version. If you have questions about how the library works and how to use its functions, this code represents a good example of that.
* There were some errors in the library with the new single-motor functions, but I fixed them. This would have rendered even double-motor functions useless so if you were having problems compiling, they should be fixed now.
* I added a function to the MotorDrv class to kill a stepper. This currently doesn't fit in with the rest of the class because it I don't make any attempt to move steppers with any of its functions. However, I needed something that would stop current to my stepper and it was cleaner to add it to the class than stick a function at the end of the .ino file. If I ever have the time, I'll add full support for steppers to the class. For now, I'm happy just stopping them from bursting into flames.
* I rearranged this README so now it's in reverse chronological order.
* **Please**, if you are trying to use this code or the MotorDrv class and it gives you an error when you're trying to compile or upload, **let me know ASAP so I can fix it ASAP**. As the end of this term and the deadline for this project approaches I don't have time to test the code as much as I want to, and I would sincerely appreciate any heads up that something is wrong so I can fix it early. I can't help you troubleshoot your robot or its control system (I regretfully don't have the time this late in the game), but I will happily address any issues with my own code.
* I still don't know how to use git. I have every version of everything I've done for this project so anyone wants old, dusty versions of my code, let me know and I'll hook you up.

## 11 - 22 - 2016 ##
The MotorDrv class now supports driving individual motors instead of just pairs of motors. I can't believe it took me so long to decide it was a good idea to include this kind of support. The usage of these new, radical, revolutionary functions (/s) is well commented in the code. At this point I doubt I'll change the library much (if at all) prior to the end of this term. My robot's code will likely change quite a lot but any further changes I make to the library will just be minor tweaks.

## 11 - 19 - 2016 ##
The MotorDrv class works with the BTS7960. I changed the enabling and it's in good shape now. The current version of this library might be the final one for all purposes relating to this particular robot.

## 11 - 17 - 2017 ##
I have changed the MotorDrv class to have the drive motors enable only when they're moving. I have not tested this yet, but I have no reason to believe it won't work. The new IBT2 function usage is described in the comments.

## 11 - 15 - 2016 ##
I've uploaded a slightly more up to date version of everything, plus a zip file of the library. My priority at this point is changing the way the IBT2 function handles enabling each side of the driver. I am starting to think that having both sides enabled all the time is not wise. I fried an Arduino mega today (!!!) and this might be at the root of that unfortunate occurance. In any event, more to come as soon as I get a replacement in the mail.

## 11 - 11 - 2016 ##
I added support for the BTS7960 (aka IBT_2) motor drivers I'm using to control the drive motors. It's very easy to use. Keep in mind that these drivers do not require a command to the enable pins like the L298N. Instead, I'm connecting my enable pins to a bus that is connected to the Arduino's 5V out pin.

## 11 - 10 - 2016 ##
This time around there were pretty big changes. It took me forever to get these changes done because of school ,waiting for parts, etc, but I'm finally ready to post new stuff.
* I made a class (MotorDrv) to deal with driving the motors. So far I just have functions to kill the motors (motorKill) and to drive DC motors with the (L298N) module. I'll add more motor drivers to this class as I'm able to get my hands on them. In any event, the code is a tad less monolithic now and *much* easier to read.
* I started using a stepper to perform one function. For that I just use the stepper library everyone uses. Again, if I can figure out how they work I'll add steppers to my MotorDrv class.

## 10 - 31 - 2016 ##
* I'm using a function called DCmotor to take all the controller inputs, turn them into values the driver can use, and then make the motors do their thing. This seems to me like it will improve the overall tidiness of the code and make it easier to write.
* I'm going to make a function that easily stops a pair of motors, probably once construction of the robot commences.
* I'm trying my hand at making a library to do what these functions I'm writing do. It's not quite there yet, but it's close.
* I still do not know how to use git. I'm working on this.
