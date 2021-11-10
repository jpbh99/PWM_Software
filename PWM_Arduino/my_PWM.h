#ifdef __cplusplus 
 extern "C" {
#endif

#ifndef	MY_PWM_H
#define MY_PWM_H
typedef struct ANALOG_WRITE_T ANALOG_WRITE_T; // Se define la estructura 


ANALOG_WRITE_T   *New_Object(); // Se definen las funciones que se van a usar
void MyAnalogWriteSetup(ANALOG_WRITE_T *fsm, int value, int pin, int frecuency);
void MyAnalogWrite(ANALOG_WRITE_T *fsm, int value, int frecuency);

#endif /* MY_PWM_H */

#ifdef __cplusplus
}
#endif
