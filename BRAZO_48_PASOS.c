#include <BRAZO_48_PASOS.h>

//// configuraciones ////
#fuses nowdt    //desactiva perro guardian. es un temporizador que reinicia el microcontrolador si detecta que el programa se ha quedado atascado en un bucle infinito
#fuses hs         // para configurar el pic a frecuencias de clk mas de 8M Hz. 
#fuses noprotect  //permite que el código almacenado en el microcontrolador pueda ser leído por dispositivos externos, lo cual puede ser útil durante el desarrollo y la depuración, pero no es recomendable para productos finales donde se desea proteger la propiedad intelectual.
//#fuses nomclr     //El fusible NOMCLR desactiva la función de Master Clear (MCLR) del microcontrolador. Cuando NOMCLR está activado, el pin MCLR se convierte en una entrada digital normal en lugar de ser utilizado para reiniciar el microcontrolador1. Esto puede ser útil si necesitas más pines de entrada/salida y no necesitas la función de reinicio externo.
//#fuses novregen   // se utiliza para desactivar la función de Voltage Regulator Enable (VREGEN) en algunos microcontroladores PIC. Esta función controla el regulador de voltaje interno del microcontrolador.. Al usar #FUSES NOVREGEN, se desactiva esta función, lo que puede ser útil en situaciones donde se utiliza una fuente de alimentación externa regulada o cuando se desea reducir el consumo de energía
//////////////////////////

// Manejo de Puertos //
#use standard_io(a)
#use standard_io(b)
#use standard_io(c)
//#use standard_io(d)
/////////////////////////

/////////PIN_OUT///////////
//      ADC MOTOR1 = RA0
//      ADC MOTOR2 = RA1
//      ADC MOTOR3 = RA2

//      PULSADOR MOVER MOTOR1 = RC0
//      PULSADOR MOVER MOTOR2 = RC1
//      PULSADOR MOVER MOTOR3 = RC2
//      PULSADOR GUARDADO     = RC3

//      LED_INDICADOR0  = RA3
//      LED_INDICADOR1  = RA4

//      SERVOMOTOR = RA5
//////////////////////////////////

//// Prototipos de Funciones 
void initial ();
void elegir_estado_actual();
void ejecutar_estado_actual();
void MOVER_MOTOR1();
void MOVER_MOTOR2();
void MOVER_MOTOR3();
//////////////////////////////////

//////Variables a usar//////

float adc;                        //toma el valor de 10bits del ADC
int16 adc_escalado1;                // divide para ajustar el valor
int16 adc_escalado2;                // divide para ajustar el valor
int16 adc_escalado3;                // divide para ajustar el valor
int16 posicion1;                   // posicion a lazo abierto del motor 1 
int16 posicion2;                   // posicion a lazo abierto del motor 2 
int16 posicion3;                   // posicion a lazo abierto del motor 3 
int reg_desplazamiento1;           // registro para ir dezplazando los bits para mover los motores
int reg_desplazamiento2;           // registro para ir dezplazando los bits para mover los motores
int reg_desplazamiento3;           // registro para ir dezplazando los bits para mover los motores
int16 time;                        //tiempo en milisegundos para cada pulso al motor
int estado_actual;              
int16 reg_memory_1;                  // registro para registrar la posicion a lazo abierto del motor 1
int16 reg_memory_2;                  // registro para registrar la posicion a lazo abierto del motor 2
int16 reg_memory_3;                  // registro para registrar la posicion a lazo abierto del motor 3

/////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////  
      
/////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////  PRINCIPAL   ////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void main()
{
  
   setup_adc_ports(sAN0 | sAN1 | sAN2, VSS_VDD);
   setup_adc(ADC_CLOCK_INTERNAL | ADC_TAD_MUL_0 | ADC_LEGACY_MODE | ADC_THRESHOLD_INT_DISABLED);

   initial();      // inicializar variables

   while(true)
   {
      elegir_estado_actual();    // maquina de mealy que, en funcion del estado actual y de las entradas decide cual es su estado futuro...
      ejecutar_estado_actual();  // ejecuta el estado correspodiente..
   }
}













void initial ()
{
adc=0;
posicion1=0;
posicion2=0;
posicion3=0;
reg_memory_1=0;
reg_memory_2=0;
reg_memory_3=0;
reg_desplazamiento1 = 0b00000011;  // comenzar a mover el motor1
reg_desplazamiento2 = 0b00110000;  // comenzar a mover el motor1
reg_desplazamiento3 = 0b11000000;  // comenzar a mover el motor1
time=20;
output_b(0b00000000);
output_bit(pin_c4,0);
output_bit(pin_c5,0);
output_bit(pin_c6,0);
output_bit(pin_c7,0);
output_bit(pin_A5,0);
estado_actual=0;
}
  



void elegir_estado_actual()
{
   if (estado_actual == 0 && input(pin_c0) == 0)
         {
            estado_actual = estado_actual;
         }
   else if (estado_actual == 0 && input(pin_c0) == 1)  // si se presiona C0 A uno
      {                       // Debe estar el motor1 colocado en posicion 0. 
         estado_actual = 1;   // Estado de mover el motor1 a la posicion que se quiere guardar...
       //  delay_ms(10);        // Rebote.....
    //   posicion1 = 0;     // posicion actual es la inicializacion a 0. entiendes zahi?
      }
 //  else if (estado_actual == 1 && input(pin_c3) == 1)   // boton de guardar presionado
 //     {
 //        estado_actual = 2; // estado de memorizar posicion del motor 1
 //        reg_memory_1 = posicion1;
 //     }
   else if (estado_actual == 2 && input(pin_c1) == 1)  // si se presiona C1 A uno
      {                                      // Debe estar el motor2 colocado en posicion 0.
         estado_actual = 3;            // Estado de mover el motor2 a la posicion que se quiere guardar...
         posicion2 = 0;     // posicion actual es la inicializacion a 0. entiendes zahi?
      }
   else if (estado_actual == 3 && input(pin_c3) == 1)  // boton de guardar presionado
      {
         estado_actual = 4; // estado de memorizar posicion del motor 2
         reg_memory_2 = posicion2;
      }
   else if (estado_actual == 4 && input(pin_c2) == 1)  // si se presiona C2 A uno
      {                                   // Debe estar el motor3 colocado en posicion 0.
         estado_actual = 5;            // Estado de mover el motor3 a la posicion que se quiere guardar...
         posicion3 = 0;     // posicion actual es la inicializacion a 0. entiendes zahi?
      }
   else if (estado_actual == 5 && input(pin_c3) == 1)  // boton de guardar presionado
      {
         estado_actual = 6; // estado de memorizar posicion del motor 3
         reg_memory_3 = posicion3;
      }
   else 
        estado_actual = estado_actual;  
}




void ejecutar_estado_actual()
{
   switch (estado_actual)
      {
         case 0:   //Estado cero, el usuario tiene la libertad y la obligacion de llevar los 3 motores a posicion cero.
                                 output_bit(pin_A3,1);
                                 output_bit(pin_A4,0);
  
                       ////////////////////////////////////////////////////////////////////////
                                 set_adc_channel(0);               //potenciometro del motor1
                                 delay_us(100);
                                 adc = read_adc(ADC_START_AND_READ);
                                 adc_escalado1 = adc*(48./1023.);    // como el motor es de 2048 pasos cada vuelta, escalamos los 1023 binarios a 2048 posiciones enteras...!!       
                                 MOVER_MOTOR1();
                                 
                      ////////////////////////////////////////////////////////////////////////////////////////
                      
                      ///////////////////////////////////////////////////////////////////////////////////////
                                 set_adc_channel(1);       //Potenciometro del motor 2
                                 delay_us(100);
                                 adc = read_adc(ADC_START_AND_READ);
                                 adc_escalado2 = adc*(48./1023.);    // como el motor es de 2048 pasos cada vuelta, escalamos los 1023 binarios a 512 posiciones enteras...!!
                                 MOVER_MOTOR2();
                          
                       //////////////////////////////////////////////////////////////////////////////////////////
                       
                       ////////////////////////////////////////////////////////////////////////////////////////
                                 set_adc_channel(2);       //Potenciometro del motor 3
                                 delay_us(100);
                                 adc = read_adc();
                                 adc_escalado3 = adc*(48./1023.);    // como el motor es de 2048 pasos cada vuelta, escalamos los 1023 binarios a 2048 posiciones enteras...!!
                                 MOVER_MOTOR3();
      
                  break;
       
         case 1:  // Estado de mover motor1 a la posicion de guardado. Debio ser inicializado a posicion 0...
                                 output_bit(pin_A3,1);
                                 output_bit(pin_A4,1);//////LEDS ...
 
                                 
                                 set_adc_channel(0);               //potenciometro del motor1
                                 delay_us(100);
                                 adc = read_adc();
                                 adc_escalado1 = adc*2;    // como el motor es de 2048 pasos cada vuelta, escalamos los 1023 binarios a 2048 posiciones enteras...!!
                                 MOVER_MOTOR1();
      
                  break;
                          
         }
}


void MOVER_MOTOR1()
{
                           
                                 if (posicion1 == adc_escalado1 || posicion1 == adc_escalado1 +1 || posicion1 == adc_escalado1 -1 ) // si la posicion a lazo abierto es similar a la posicion del potenciometro todo se queda quieto..!!
                                       {
                                  
                                       }
                                 else if (posicion1 < adc_escalado1)  // si la posicion del potenciometro es mayor a la posicion del motor a lazo abierto, haz que mueva a un lado... 
                                       {
                                           if (reg_desplazamiento1 == 0b00001100)////  1100 1001 0011 0110
                                             {                                    
                                                reg_desplazamiento1 = 0b00001001;
                                                posicion1 = posicion1 + 1;
                                                output_b (reg_desplazamiento1);
                                                delay_ms(time);
                                             }
                                           else if (reg_desplazamiento1 == 0b00001001)////  1100 1001 0011 0110
                                             {
                                                reg_desplazamiento1 = 0b00000011;
                                                posicion1 = posicion1 + 1;
                                                output_b (reg_desplazamiento1);
                                                delay_ms(time);
                                             }
                                             else if (reg_desplazamiento1 == 0b00000000)////  1100 1001 0011 0110
                                             {
                                                reg_desplazamiento1 = 0b00000011;
                                                delay_ms(time);
                                             }
                                           else
                                             {
                                                reg_desplazamiento1 = reg_desplazamiento1 << 1;         // aunque cambie el argumento del output_b() este no cambia por el lach. a menos que se llame.
                                                posicion1 = posicion1 + 1;
                                                output_b (reg_desplazamiento1);
                                                delay_ms(time);
                                             }
                                       }
                                    
                                   else if (posicion1 > adc_escalado1) // si la posicion del potenciometro es menor a la posicion del motor a lazo abierto, haz que mueva al otro lado... 
                                     {   
                                          if (reg_desplazamiento1 == 0b00000011)
                                             {
                                                reg_desplazamiento1 = 0b00001001;
                                                posicion1 = posicion1 - 1;
                                                output_b (reg_desplazamiento1);
                                                delay_ms(time);
                                             }
                                          else if (reg_desplazamiento1 == 0b00001001)
                                             {
                                                reg_desplazamiento1 = 0b00001100;
                                                posicion1 = posicion1 - 1;
                                                output_b (reg_desplazamiento1);
                                                delay_ms(time);
                                             }
                                           else if (reg_desplazamiento1 == 0b00000000)////  1100 1001 0011 0110
                                             {
                                                reg_desplazamiento1 = 0b00001100;
                                                delay_ms(time);
                                             }
                                          else
                                             {
                                                reg_desplazamiento1 = reg_desplazamiento1 >> 1;         // aunque cambie el argumento del output_b() este no cambia por el lach. a menos que se llame.
                                                posicion1 = posicion1 - 1;
                                                output_b (reg_desplazamiento1);
                                                delay_ms(time);
                                             }      
                                    }
}
                      ////////////////////////////////////////////////////////////////////////////////////////
void MOVER_MOTOR2()
{
                      ///////////////////////////////////////////////////////////////////////////////////////
                               
                            
                                 
                             
                                                                                          
                                 
                                 if (posicion2 == adc_escalado2 || posicion2 == adc_escalado2 +1 || posicion2 == adc_escalado2 -1 || posicion2 == adc_escalado2 -2 || posicion2 == adc_escalado2 +2) // si la posicion a lazo abierto es similar a la posicion del potenciometro todo se queda quieto..!!
                                       {
                                  
                                       }
                                 else if (posicion2 < adc_escalado2)  // si la posicion del potenciometro es mayor a la posicion del motor a lazo abierto, haz que mueva a un lado... 
                                       {
                                           if (reg_desplazamiento2 == 0b11000000)////  1100 1001 0011 0110
                                             {                                    
                                                reg_desplazamiento2 = 0b10010000;
                                                posicion2 = posicion2 + 1;
                                                output_b (reg_desplazamiento2);
                                                delay_ms(time);
                                             }
                                           else if (reg_desplazamiento2 == 0b10010000)////  1100 1001 0011 0110
                                             {
                                                reg_desplazamiento2 = 0b00110000;
                                                posicion2 = posicion2 + 1;
                                                output_b (reg_desplazamiento2);
                                                delay_ms(time);
                                             }
                                             else if (reg_desplazamiento2 == 0b00000000)////  1100 1001 0011 0110
                                             {
                                                reg_desplazamiento2 = 0b00110000;
                                                delay_ms(time);
                                             }
                                           else
                                             {
                                                reg_desplazamiento2 = reg_desplazamiento2 << 1;         // aunque cambie el argumento del output_b() este no cambia por el lach. a menos que se llame.
                                                posicion2 = posicion2 + 1;
                                                output_b (reg_desplazamiento2);
                                                delay_ms(time);
                                             }
                                       }
                                    
                                   else if (posicion2 > adc_escalado2) // si la posicion del potenciometro es menor a la posicion del motor a lazo abierto, haz que mueva al otro lado... 
                                     {   
                                          if (reg_desplazamiento2 == 0b00110000)
                                             {
                                                reg_desplazamiento2 = 0b10010000;
                                                posicion2 = posicion2 - 1;
                                                output_b (reg_desplazamiento2);
                                                delay_ms(time);
                                             }
                                          else if (reg_desplazamiento2 == 0b10010000)
                                             {
                                                reg_desplazamiento2 = 0b11000000;
                                                posicion2 = posicion2 - 1;
                                                output_b (reg_desplazamiento2);
                                                delay_ms(time);
                                             }
                                          else
                                             {
                                                reg_desplazamiento2 = reg_desplazamiento2 >> 1;         // aunque cambie el argumento del output_b() este no cambia por el lach. a menos que se llame.
                                                posicion2 = posicion2 - 1;
                                                output_b (reg_desplazamiento2);
                                                delay_ms(time);
                                             }      
                                    }
}
                       //////////////////////////////////////////////////////////////////////////////////////////
void MOVER_MOTOR3()                       
                       ////////////////////////////////////////////////////////////////////////////////////////
{                             
                                
                                 
                                                                                          
                                 
                                 if (posicion3 == adc_escalado3 || posicion3 == adc_escalado3 +1 || posicion3 == adc_escalado3 -1 || posicion3 == adc_escalado3 -2 || posicion3 == adc_escalado3 +2 || posicion3 == adc_escalado3 -3 || posicion3 == adc_escalado3 +3) // si la posicion a lazo abierto es similar a la posicion del potenciometro todo se queda quieto..!!
                                       {
                                  
                                       }
                                 else if (posicion3 < adc_escalado3)  // si la posicion del potenciometro es mayor a la posicion del motor a lazo abierto, haz que mueva a un lado... 
                                       {
                                           if (reg_desplazamiento3 == 0b11000000)////  1100 1001 0011 0110
                                             {                                    
                                                reg_desplazamiento3 = 0b10010000;
                                                posicion3 = posicion3 + 1;
                                                output_bit(pin_c7,1);
                                                output_bit(pin_c6,0);
                                                output_bit(pin_c5,0);
                                                output_bit(pin_c4,1);
                                                delay_ms(time);
                                             }
                                           else if (reg_desplazamiento3 == 0b10010000)////  1100 1001 0011 0110
                                             {
                                                reg_desplazamiento3 = 0b00110000;
                                                posicion3 = posicion3 + 1;
                                                output_bit(pin_c7,0);
                                                output_bit(pin_c6,0);
                                                output_bit(pin_c5,1);
                                                output_bit(pin_c4,1);
                                                delay_ms(time);
                                             }
                                             else if (reg_desplazamiento3 == 0b00110000)////  1100 1001 0011 0110
                                             {
                                                reg_desplazamiento3 = 0b01100000;
                                                posicion3 = posicion3 + 1;
                                                output_bit(pin_c7,0);
                                                output_bit(pin_c6,1);
                                                output_bit(pin_c5,1);
                                                output_bit(pin_c4,0);
                                                delay_ms(time);
                                             }
                                             else if (reg_desplazamiento3 == 0b01100000)////  1100 1001 0011 0110
                                             {
                                                reg_desplazamiento3 = 0b11000000;
                                                posicion3 = posicion3 + 1;
                                                output_bit(pin_c7,1);
                                                output_bit(pin_c6,1);
                                                output_bit(pin_c5,0);
                                                output_bit(pin_c4,0);
                                                delay_ms(time);
                                             }
                                             else if (reg_desplazamiento3 == 0b00000000)////  1100 1001 0011 0110
                                             {
                                                reg_desplazamiento3 = 0b11000000;
                                          
                                                delay_ms(time);
                                             }
                                          
                                           
                                       }
                                    
                                   else if (posicion3 > adc_escalado3) // si la posicion del potenciometro es menor a la posicion del motor a lazo abierto, haz que mueva al otro lado... 
                                     {   
                                          if (reg_desplazamiento3 == 0b11000000)////  1100 1001 0011 0110
                                             {                                    
                                                reg_desplazamiento3 = 0b01100000;
                                                posicion3 = posicion3 - 1;
                                                output_bit(pin_c7,0);
                                                output_bit(pin_c6,1);
                                                output_bit(pin_c5,1);
                                                output_bit(pin_c4,0);
                                                delay_ms(time);
                                             }
                                           else if (reg_desplazamiento3 == 0b01100000)////  1100 1001 0011 0110
                                             {
                                                reg_desplazamiento3 = 0b00110000;
                                                posicion3 = posicion3 - 1;
                                                output_bit(pin_c7,0);
                                                output_bit(pin_c6,0);
                                                output_bit(pin_c5,1);
                                                output_bit(pin_c4,1);
                                                delay_ms(time);
                                             }
                                             else if (reg_desplazamiento3 == 0b00110000)////  1100 1001 0011 0110
                                             {
                                                reg_desplazamiento3 = 0b10010000;
                                                posicion3 = posicion3 - 1;
                                                output_bit(pin_c7,1);
                                                output_bit(pin_c6,0);
                                                output_bit(pin_c5,0);
                                                output_bit(pin_c4,1);
                                                delay_ms(time);
                                             }
                                             else if (reg_desplazamiento3 == 0b10010000)////  1100 1001 0011 0110
                                             {
                                                reg_desplazamiento3 = 0b11000000;
                                                posicion3 = posicion3 - 1;
                                                output_bit(pin_c7,1);
                                                output_bit(pin_c6,1);
                                                output_bit(pin_c5,0);
                                                output_bit(pin_c4,0);
                                                delay_ms(time);
                                             }
                                    }
}

