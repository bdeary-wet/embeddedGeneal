# myDev
This is just bits an pieces of useful code. Some work together to form the
base for simple embedded systems where an RTOS is not needed.  Somewhat geared to
cortex level arm processors but easily adaptable to most micros.

I use CEEDLING with GCC to do unit testing and off target development.
CEEDLING is a great wrapper around the Unity unit test framework with CMock and CException.
Last seen at [throwtheswitch.org](http://www.throwtheswitch.org/ceedling/)  
Nice tutorial is [Here](https://dmitryfrank.com/articles/unit_testing_embedded_c_applications)

## Sub projects

#### crc8
crc8 functions

#### fastswitch
C macro wrapper and copy function for foreground objects that are updated
in a lower priority task and fast switched by changing the main pointer.
Allows setting multiple copies to allow copies of the pointer to operate 
for periods of time after switch.

#### optable
sorted table lookup designed for sparse opcode decoding. Uses bsearch. 
Need to add the option for non-sparse table lookup switch which would just be 
an array dereference.

####  qmacs.h
C queue macros for simple fifos

#### generalQueue
Make a queue of any type.  
Also includes functions for making a Pool of any type. The pool entry can be
returned as a special wrapper type or as just a pointer to your object. In either 
case the object can be returned to the pool with only the original returned pointer
so pooled items can be passed around, or added to queues. There are also optional 
destructor functions that can be applied to any pool item returned. There are also
iterator functions for applying a function to all objects in a pool, all active 
objects or all inactive objects.


#### state
General State machine with timed states, target state, and error state callbacks

#### swtimer
simple unsigned roll-over timer functions for 16 and 32 bit timers.

#### swTimers
more complex service for addind lists of pooled timers to a time source.

#### taskService and taskServiceClass
A prioritized task service that supports cooperative multitasking.

#### timercb
Callback on software timer package, uses swtimer.


