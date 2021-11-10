#include "my_PWM.h" // Se incluye el .C creado

int f = 1;  // Se define la frecuencia
ANALOG_WRITE_T *object_0 = New_Object();  // Se crean los objetos, que serian los PWM
ANALOG_WRITE_T *object_1 = New_Object();  
ANALOG_WRITE_T *object_2 = New_Object();  
  
void setup() {
    MyAnalogWriteSetup(object_0, 255, 2, f);  // Se hace la configuracion los tres PWM
    MyAnalogWriteSetup(object_1, 127, 3, f);
    MyAnalogWriteSetup(object_2, 50, 4, f);

} 


void loop() {
    MyAnalogWrite(object_0, 123, f);  // Empiezan las salidas PWM
    MyAnalogWrite(object_1, 127, f);
    MyAnalogWrite(object_1, 127, f);

} 
