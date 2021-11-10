/* Ruler 1         2         3         4         5         6         7        */

/********************************** Headers ***********************************/

/* ------------------------ Inclusion of Std Headers ------------------------ */

#include <Arduino.h>  // Se importan las librerias necesarias
#include <stdlib.h>
#include <math.h> 
#include "my_PWM.h"   


typedef enum {  // Se definen los estados
  HIGH_LEVEL,
  LOW_LEVEL 
} PWM_STATE_T;

struct ANALOG_WRITE_T { // Se define la estructura oculta
  int   previous_value;
  int   pin_out;
  int   previous_time;
  int   time_HI, time_LO;
  PWM_STATE_T state;
};


ANALOG_WRITE_T *New_Object(){ // Funcion que crea un espacio de memoria oculto al usuario
  
  return (ANALOG_WRITE_T *)malloc( sizeof( ANALOG_WRITE_T ) );
}

void MyAnalogWriteSetup(ANALOG_WRITE_T *fsm, int value, int pin, int frecuency){  // Funcion de configuracion para los PWM
  fsm -> previous_value = value; // Ciclo util
  fsm -> pin_out = pin; // Pin a usar
  fsm -> previous_time = micros();  // Toma de iempo
  fsm -> time_HI = (value * 1000000) / (frecuency*255); // Calculo de tiempo de subida
  fsm -> time_LO =  (1000000 / frecuency) - fsm -> time_HI; // Calculo de tiempo de bajada
  fsm -> state = HIGH_LEVEL;  //Se inicia estado en High
  pinMode( pin, OUTPUT);  // Se define el pin como salida
  digitalWrite( pin, HIGH); // S establece el pin en alto
}




void MyAnalogWrite(ANALOG_WRITE_T *fsm, int value, int frecuency){  // Maquina de estados de los PWM
  switch (fsm -> state){
    case HIGH_LEVEL:
      if(micros() - fsm -> previous_time >= fsm -> time_HI){  // Si se cumple el tiempo entra al if
        digitalWrite(fsm -> pin_out, LOW);  // Se establece el pin en bajo
        fsm -> previous_time = micros();  // Se toma un nuevo tiempo
        fsm -> state = LOW_LEVEL; // Se cambia de estado
      }

    break;
    
    case LOW_LEVEL:
      if(micros() - fsm -> previous_time >= fsm -> time_LO){  // Si se cumple el tiempo entra al if
        digitalWrite(fsm -> pin_out, HIGH); // Se establece el pin en alto
        fsm -> previous_time = micros();  // Se toma un nuevo tiempo
        if(value != fsm -> previous_value){ // Si no se cumple el tiempo entra al if
          fsm -> time_HI = (value * 1000000) / (frecuency*255); // Se calcula el tiempo en alto
          fsm -> time_LO = (1000000 / frecuency) - fsm -> time_HI;  // Se calcula el tiempo en bajo
          fsm -> previous_value = value;  // Se toma el ciclo util
        }
        fsm -> state = HIGH_LEVEL; // Se cambia de estado
      }
    break;
  }
}
