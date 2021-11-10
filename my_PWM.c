/* Ruler 1         2         3         4         5         6         7        */

/********************************** Headers ***********************************/

/* ------------------------ Inclusion of Std Headers ------------------------ */

#include <Arduino.h>
#include <stdlib.h>
#include <math.h> 
#include "my_PWM.h"   


typedef enum {
  HIGH_LEVEL,
  LOW_LEVEL 
} PWM_STATE_T;

struct ANALOG_WRITE_T {
  int   previous_value;
  int   pin_out;
  int   previous_time;
  int   time_HI, time_LO;
  PWM_STATE_T state;
};


ANALOG_WRITE_T *New_Object(){
  
  return (ANALOG_WRITE_T *)malloc( sizeof( ANALOG_WRITE_T ) );
}

void MyAnalogWriteSetup(ANALOG_WRITE_T *fsm, int value, int pin, int frecuency){
  fsm -> previous_value = value;
  fsm -> pin_out = pin;
  fsm -> previous_time = micros();
  fsm -> time_HI = (value * 1000000) / (frecuency*255);  
  fsm -> time_LO =  (1000000 / frecuency) - fsm -> time_HI;
  fsm -> state = HIGH_LEVEL;
  pinMode( pin, OUTPUT);
  digitalWrite( pin, HIGH);
}




void MyAnalogWrite(ANALOG_WRITE_T *fsm, int value, int frecuency){
  switch (fsm -> state){
    case HIGH_LEVEL:
      if(micros() - fsm -> previous_time >= fsm -> time_HI){
        digitalWrite(fsm -> pin_out, LOW);
        fsm -> previous_time = micros();
        fsm -> state = LOW_LEVEL;
      }

    break;
    
    case LOW_LEVEL:
      if(micros() - fsm -> previous_time >= fsm -> time_LO){
        digitalWrite(fsm -> pin_out, HIGH);
        fsm -> previous_time = micros();
        if(value != fsm -> previous_value){
          fsm -> time_HI = (value * 1000000) / (frecuency*255);
          fsm -> time_LO = (1000000 / frecuency) - fsm -> time_HI;
          fsm -> previous_value = value;
        }
        fsm -> state = HIGH_LEVEL;
      }
    break;
  }
}
