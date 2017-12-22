# myDev
This is just bits an pieces of useful code. Some work together to form the
base for simple embedded systems where an RTOS is not needed.

I use CEEDLING with GCC to do unit testing and off target development.
CEEDLING is a great wrapper around the Unity unit test framework with CMock and CException.
Last seen at http://www.throwtheswitch.org/ceedling/

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

#### state
General State machine with timed states, target state, and error state callbacks

#### swtimer
simple unsigned roll-over timer functions for 16 and 32 bit timers.

#### tasker
General background loop tasker to call task functions in response to event flags.
Provides several processing functions for a single tasker object to process 
in a prioritized or round-robin manner. 

#### timercb
Callback on software timer package, uses swtimer.


