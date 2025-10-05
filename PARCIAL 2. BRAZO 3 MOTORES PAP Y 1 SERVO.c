#include <PARCIAL 2. BRAZO 3 MOTORES PAP Y 1 SERVO.h>


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
#use standard_io(d)
#use standard_io(e)
/////////////////////////

/////////ENTRADAS////////////////////////////////////////
///////////////////////////////////////////////////
/////pulsadores//////////
#define save               pin_c0
#define start              pin_c1
#define parada_emergencia  pin_c2

#define limit_switch_1     pin_c3
#define limit_switch_2     pin_d0
#define limit_switch_3     pin_d1
#define limit_switch_4     pin_d2
////////pines analogicos//////////////
#define adc_motor1         pin_a0   //  
#define adc_motor2         pin_a1   //
#define adc_motor3         pin_a2   //    
#define adc_servo          pin_a3   //
      
#define stop               pin_e2

////////////////SALIDAS///////////////////////
/////////////////////////////////////
//#define led0              pin_d0
//#define led1              pin_d1
//#define led2              pin_d2
#define led_on            pin_d3
#define led_off           pin_d4
#define pwm_servo         pin_d5
#define pwm_derecho_dc    pin_d6
#define pwm_izquierdo_dc  pin_d7

//////  pin_b0
//////  pin_b1
//////  pin_b2       MOTOR1
//////  pin_b3

//////  pin_b4
//////  pin_b5
//////  pin_b6       MOTOR2
//////  pin_b7

//////  pin_c4
//////  pin_c5
//////  pin_c6       MOTOR3
//////  pin_c7



//////////////////////////////////



///////////////ESTADOS//////////////////
/////////////////////////////////////
#define posicionar_home_automaticamente           0
#define posicionar_posicion1                      1
#define posicionar_posicion2                   3
#define posicionar_posicion3                 5
#define posicionar_posicion4                  7
#define posicionar_posicion5                      9
#define esperar_1                 2
#define esperar_2                 4
#define esperar_3                   6
#define esperar_4                          8
#define esperar_5                        10
#define ejecutar_posicion1            11
#define ejecutar_posicion2              12
#define ejecutar_posicion3            13
#define ejecutar_posicion4         14 
#define ejecutar_posicion5         15

/////////////////////////////////////////////////


//// Prototipos de Funciones 
void initial ();
void elegir_estado_actual();
void ejecutar_estado_actual();
void MOVER_MOTOR1();
void MOVER_MOTOR2();
void MOVER_MOTOR3();
void MOVER_SERVO();
void DESBASTAR();
void PARO();
//////////////////////////////////

//////Variables a usar//////
int duty_servo;

int count1;
int16 adc;                        //toma el valor de 10bits del ADC
//int16 adc_escalado;                // divide para ajustar el valor
int speed;
int16 adc_escalado1;                // divide para ajustar el valor
int16 adc_escalado2;                // divide para ajustar el valor
int16 adc_escalado3;                // divide para ajustar el valor
int16 adc_escalado4;                // divide para ajustar el valor
int16 posicion1;                   // posicion a lazo abierto del motor 1 
int16 posicion2;                   // posicion a lazo abierto del motor 2 
int16 posicion3;                   // posicion a lazo abierto del motor 3 
//int reg_desplazamiento;           // registro para ir dezplazando los bits para mover los motores
int reg_desplazamiento1;           // registro para ir dezplazando los bits para mover los motores
int reg_desplazamiento2;           // registro para ir dezplazando los bits para mover los motores
int reg_desplazamiento3;           // registro para ir dezplazando los bits para mover los motores
int16 time;                        //tiempo en milisegundos para cada pulso al motor
int estado_actual;              
int16 reg_memory_1_1;                  // registro para registrar la posicion a lazo abierto del motor 1
int16 reg_memory_2_1;                  // registro para registrar la posicion a lazo abierto del motor 2
int16 reg_memory_3_1;                  // registro para registrar la posicion a lazo abierto del motor 3
int   reg_memory_4_1=0;
int16 reg_memory_1_2;                  // registro para registrar la posicion a lazo abierto del motor 1
int16 reg_memory_2_2;                  // registro para registrar la posicion a lazo abierto del motor 2
int16 reg_memory_3_2;                  // registro para registrar la posicion a lazo abierto del motor 3
int   reg_memory_4_2=0;
int16 reg_memory_1_3;                  // registro para registrar la posicion a lazo abierto del motor 1
int16 reg_memory_2_3;                  // registro para registrar la posicion a lazo abierto del motor 2
int16 reg_memory_3_3;                  // registro para registrar la posicion a lazo abierto del motor 3
int   reg_memory_4_3=0;
int16 reg_memory_1_4;                  // registro para registrar la posicion a lazo abierto del motor 1
int16 reg_memory_2_4;                  // registro para registrar la posicion a lazo abierto del motor 2
int16 reg_memory_3_4;                  // registro para registrar la posicion a lazo abierto del motor 3
int   reg_memory_4_4=0;
int16 reg_memory_1_5;                  // registro para registrar la posicion a lazo abierto del motor 1
int16 reg_memory_2_5;                  // registro para registrar la posicion a lazo abierto del motor 2
int16 reg_memory_3_5;                  // registro para registrar la posicion a lazo abierto del motor 3
int   reg_memory_4_5=0;

int1 alcanza_1;
int1 alcanza_2;
int1 alcanza_3;
int1 velocidad_dc_derecho = 0;
int1 velocidad_dc_izquierdo = 0;
int1 avanzar=0;

/////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#INT_TIMER0
void interrupt()   // cada 100us entra
{
      if(count1 == 200)      //perido 20ms  //frecuencia 50hz
      {
            output_bit(pwm_servo,1);
            output_bit(pwm_derecho_dc,0);
            output_bit(pwm_derecho_dc,0);
            
            
            if(velocidad_dc_derecho == 1)  // indica que queremos girar derecha motor dc
                  {
                          output_bit(pwm_derecho_dc,1);
                  }
            else if(velocidad_dc_izquierdo == 1) // indica que queremos girar izquierda motor dc
                  {
                          output_bit(pwm_izquierdo_dc,1);
                  }       
            count1=0;
      }
      
      else if (count1 < 200)
      {
           if(count1 == duty_servo) //DEFAULT
           {
             output_bit(pwm_servo,0);
           }
           
           else if(count1 == 100) // duty del 50% para el giro a la derecha motor dc
           {
             output_bit(pwm_derecho_dc,0);
           }
           
           else if(count1 == 199) // duty del 100% para el giro a la izquierda motor dc
           {
             output_bit(pwm_izquierdo_dc,0);
           }
              count1++;
      }
      //set_timer0(6);
      set_timer0(70); // colocamos esta carga quiza para cuadrar los retardos
      
}
      
      
      
      
/////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////  PRINCIPAL   ////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void main()
{
   setup_timer_0(RTCC_INTERNAL | RTCC_8_BIT |RTCC_DIV_2);
   set_timer0(6);                  // desborda en 100us // cada tic es de 400ns
                                   
   enable_interrupts(GLOBAL);
   enable_interrupts(INT_TIMER0);
   
   setup_adc(ADC_CLOCK_INTERNAL);
   setup_adc_ports(AN0_AN1_AN2_AN3_AN4);
   
   initial();      // inicializar variables

   while(true)
   {
      elegir_estado_actual();    // maquina de mealy que, en funcion del estado actual y de las entradas decide cual es su estado futuro...
      ejecutar_estado_actual();  // ejecuta el estado correspodiente..
   }
}

















void initial ()
{
duty_servo=0; //POR DEFAULT EL SERVO EN 0 GRADOS
count1 =0;
adc=0;
//adc_escalado=0;
posicion1=0;
posicion2=0;
posicion3=0;
reg_memory_1_1=0;
reg_memory_2_1=0;
reg_memory_3_1=0;

reg_memory_1_2=0;
reg_memory_2_2=0;
reg_memory_3_2=0;

reg_memory_1_3=0;
reg_memory_2_3=0;
reg_memory_3_3=0;

reg_memory_1_4=0;
reg_memory_2_4=0;
reg_memory_3_4=0;

reg_memory_1_5=0;
reg_memory_2_5=0;
reg_memory_3_5=0;

reg_desplazamiento1 = 0b00000011;  // comenzar a mover el motor1
reg_desplazamiento2 = 0b00110000;  // comenzar a mover el motor1
reg_desplazamiento3 = 0b11000000;  // comenzar a mover el motor1
time=15;
output_b(0b00000000);
output_bit(pin_c4,0);
output_bit(pin_c5,0);
output_bit(pin_c6,0);
output_bit(pin_c7,0);

estado_actual=posicionar_home_automaticamente; //estado_0
alcanza_1=0;
alcanza_2=0;
alcanza_3=0;
speed=0;
avanzar = 0;
}
  



void elegir_estado_actual()
{
   if (estado_actual == posicionar_home_automaticamente && avanzar == 1) 
         {
            estado_actual = posicionar_posicion1; 
         }
   else if (estado_actual == posicionar_posicion1 && input(save) == 1)  // boton de guardar presionado
      {                       
         estado_actual = esperar_1;
         reg_memory_1_1 = posicion1;
         reg_memory_2_1 = posicion2;
         reg_memory_3_1 = posicion3;
         reg_memory_4_1 = duty_servo;
      }
    else if (estado_actual == posicionar_posicion1 && input(start) == 1)  // boton de guardar presionado
      {                       
         estado_actual = ejecutar_posicion1;
         reg_memory_1_1 = posicion1;
         reg_memory_2_1 = posicion2;
         reg_memory_3_1 = posicion3;
         reg_memory_4_1 = duty_servo;
         alcanza_1=0;
         alcanza_2=0;
         alcanza_3=0;
      }
   else if (estado_actual == esperar_1 && input(save) == 0)   // boton de guardar no presionado
      {
         estado_actual = posicionar_posicion2; 
      }
   else if (estado_actual == posicionar_posicion2 && input(save) == 1)  // boton de guardar presionado
      {                                    
         estado_actual = esperar_2;            
         reg_memory_1_2 = posicion1;
         reg_memory_2_2 = posicion2;
         reg_memory_3_2 = posicion3;
         reg_memory_4_2 = duty_servo;
      }
   else if (estado_actual == posicionar_posicion2 && input(start) == 1)  // boton de guardar presionado
      {                       
         estado_actual = ejecutar_posicion1;
         reg_memory_1_1 = posicion1;
         reg_memory_2_1 = posicion2;
         reg_memory_3_1 = posicion3;
         reg_memory_4_1 = duty_servo;
         alcanza_1=0;
         alcanza_2=0;
         alcanza_3=0;
      }
   else if (estado_actual == esperar_2 && input(save) == 0)  // boton de guardar no presionado
      {
         estado_actual = posicionar_posicion3; 
      } 
    else if (estado_actual == posicionar_posicion3 && input(save) == 1)  // boton de guardar no presionado
      {
         estado_actual = esperar_3; 
         reg_memory_1_3 = posicion1;
         reg_memory_2_3 = posicion2;
         reg_memory_3_3 = posicion3;
         reg_memory_4_3 = duty_servo;
      } 
    else if (estado_actual == posicionar_posicion3 && input(start) == 1)  // boton de guardar presionado
      {                       
         estado_actual = ejecutar_posicion1;
         reg_memory_1_1 = posicion1;
         reg_memory_2_1 = posicion2;
         reg_memory_3_1 = posicion3;
         reg_memory_4_1 = duty_servo;
         alcanza_1=0;
         alcanza_2=0;
         alcanza_3=0;
      }
   else if (estado_actual == esperar_3 && input(save) == 0)  // boton de guardar no presionado
      {
         estado_actual = posicionar_posicion4; 
      } 
    else if (estado_actual == posicionar_posicion4 && input(save) == 1)  // boton de guardar no presionado
      {
         estado_actual = esperar_4; 
         reg_memory_1_4 = posicion1;
         reg_memory_2_4 = posicion2;
         reg_memory_3_4 = posicion3;
         reg_memory_4_4 = duty_servo;
      }  
     else if (estado_actual == posicionar_posicion4 && input(start) == 1)  // boton de guardar presionado
      {                       
         estado_actual = ejecutar_posicion1;
         reg_memory_1_1 = posicion1;
         reg_memory_2_1 = posicion2;
         reg_memory_3_1 = posicion3;
         reg_memory_4_1 = duty_servo;
         alcanza_1=0;
         alcanza_2=0;
         alcanza_3=0;
      }
    else if (estado_actual == esperar_4 && input(save) == 0)  // boton de guardar no presionado
      {
         estado_actual = posicionar_posicion5; 
      } 
    else if (estado_actual == posicionar_posicion5 && input(save) == 1)  // boton de guardar no presionado
      {
         estado_actual = esperar_5; 
         reg_memory_1_5 = posicion1;
         reg_memory_2_5 = posicion2;
         reg_memory_3_5 = posicion3;
         reg_memory_4_5 = duty_servo;
      }
     else if (estado_actual == posicionar_posicion5 && input(start) == 1)  // boton de guardar presionado
      {                       
         estado_actual = ejecutar_posicion1;
         reg_memory_1_1 = posicion1;
         reg_memory_2_1 = posicion2;
         reg_memory_3_1 = posicion3;
         reg_memory_4_1 = duty_servo;
         alcanza_1=0;
         alcanza_2=0;
         alcanza_3=0;
      }
     else if (estado_actual == esperar_5 && input(start) == 1)  // boton de guardar no presionado
      {
         estado_actual = ejecutar_posicion1; 
         alcanza_1=0;
         alcanza_2=0;
         alcanza_3=0;                          
      }
      
      else if (estado_actual == ejecutar_posicion1 && alcanza_1 == 1 && alcanza_2 == 1 && alcanza_3 == 1)  
      {
         DESBASTAR();   // al moverse a posicion guardada va a la sub-rutina debastar()
         delay_ms(1000);
         estado_actual = ejecutar_posicion2; 
         alcanza_1 = 0;
         alcanza_2 = 0;
         alcanza_3 = 0;
      }
          else if (estado_actual == ejecutar_posicion2 && alcanza_1 == 1 && alcanza_2 == 1 && alcanza_3 == 1)  
      {
      DESBASTAR();   // al moverse a posicion guardada va a la sub-rutina debastar()
      delay_ms(1000);
         estado_actual = ejecutar_posicion3; 
         alcanza_1 = 0;
         alcanza_2 = 0;
         alcanza_3 = 0;
      }
           else if (estado_actual == ejecutar_posicion3 && alcanza_1 == 1 && alcanza_2 == 1 && alcanza_3 == 1)  
      {
      DESBASTAR();   // al moverse a posicion guardada va a la sub-rutina debastar()
         delay_ms(1000);
         estado_actual = ejecutar_posicion4; 
         alcanza_1 = 0;
         alcanza_2 = 0;
         alcanza_3 = 0;
      }
       else if (estado_actual == ejecutar_posicion4 && alcanza_1 == 1 && alcanza_2 == 1 && alcanza_3 == 1)  
      {
      DESBASTAR();   // al moverse a posicion guardada va a la sub-rutina debastar()
         delay_ms(1000);
         estado_actual = ejecutar_posicion5; 
         alcanza_1 = 0;
         alcanza_2 = 0;
         alcanza_3 = 0;
      }
       else if (estado_actual == ejecutar_posicion5 && alcanza_1 == 1 && alcanza_2 == 1 && alcanza_3 == 1)  
      {
      DESBASTAR();   // al moverse a posicion guardada va a la sub-rutina debastar()
         delay_ms(1000);
         estado_actual = ejecutar_posicion1; 
         alcanza_1 = 0;
         alcanza_2 = 0;
         alcanza_3 = 0;
      }
   else 
        estado_actual = estado_actual;  
}




void ejecutar_estado_actual()
{
   switch (estado_actual)
      {
      
         case posicionar_home_automaticamente:
         
                                                    //    reg_desplazamiento1 = 0b00000011;  // comenzar a mover el motor1
                                                    //    reg_desplazamiento2 = 0b00110000;  // comenzar a mover el motor2
                                                    //    reg_desplazamiento3 = 0b11000000;  // comenzar a mover el motor3
                            
                                time = 25; //se garantiza velocidad lenta para los paso a paso.
                               output_bit(led_on,1);
                                 output_bit(led_off,1);
                              //  duty_servo = 5     // 500us ===> 0 grados
                              //  duty_servo = 15    // 1.5ms ===> 90 grados
                              //  duty_servo = 25    // 2.5ms ===> 180 grados
                               
                                velocidad_dc_derecho = 0;              // apagamos motor dc
                                velocidad_dc_izquierdo = 0;            // apagamos motor dc
                                duty_servo = 5;    // 500us ===> 0 grados  // inicializamos el servo a 0.
                               
                               while(input(limit_switch_1) == 0) // si no se ha presionado limit_switch_1, mueve el motor 1
                                    {
                                          if (reg_desplazamiento1 == 0b00001100)////  1100 1001 0011 0110
                                             {                                    
                                                reg_desplazamiento1 = 0b00001001;
                                                output_b (reg_desplazamiento1);
                                                delay_ms(time);
                                             }
                                           else if (reg_desplazamiento1 == 0b00001001)////  1100 1001 0011 0110
                                             {
                                                reg_desplazamiento1 = 0b00000011;
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
                                                output_b (reg_desplazamiento1);
                                                delay_ms(time);
                                             }
                                    }
                                    
                                    delay_ms(500); /// luego de inicializar el Motor1, se sigue con los siguientes:
                        
                                   
                 //MODIFICAMOS EL SENTIDO INTERCAMBIAMOS
                               while(input(limit_switch_2) == 0) // si no se ha presionado limit_switch_2, mueve el motor2
                                    {
                                          if (reg_desplazamiento2 == 0b00110000)
                                             {
                                                reg_desplazamiento2 = 0b10010000;
                                                output_b (reg_desplazamiento2);
                                                delay_ms(time);
                                             }
                                          else if (reg_desplazamiento2 == 0b10010000)
                                             {
                                                reg_desplazamiento2 = 0b11000000;
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
                                                reg_desplazamiento2 = reg_desplazamiento2 >> 1;         // aunque cambie el argumento del output_b() este no cambia por el lach. a menos que se llame.
                                                output_b (reg_desplazamiento2);
                                                delay_ms(time);
                                             }  
                                    } 
                             
                                   
                         //MODIFICAMOS EL SENTIDO INTERCAMBIAMOS
                              while(input(limit_switch_3) == 0) // si no se ha presionado limit_switch_3, mueve el motor3
                                    {
                                           if (reg_desplazamiento3 == 0b11000000)////  1100 1001 0011 0110
                                             {                                    
                                                reg_desplazamiento3 = 0b01100000;
                                                output_bit(pin_c4,0);
                                                output_bit(pin_c5,1);
                                                output_bit(pin_c6,1);
                                                output_bit(pin_c7,0);
                                                delay_ms(time);
                                             }
                                           else if (reg_desplazamiento3 == 0b01100000)////  1100 1001 0011 0110
                                             {
                                                reg_desplazamiento3 = 0b00110000;
                                                output_bit(pin_c4,0);
                                                output_bit(pin_c5,0);
                                                output_bit(pin_c6,1);
                                                output_bit(pin_c7,1);
                                                delay_ms(time);
                                             }
                                             else if (reg_desplazamiento3 == 0b00110000)////  1100 1001 0011 0110
                                             {
                                                reg_desplazamiento3 = 0b10010000;
                                                output_bit(pin_c4,1);
                                                output_bit(pin_c5,0);
                                                output_bit(pin_c6,0);
                                                output_bit(pin_c7,1);
                                                delay_ms(time);
                                             }
                                             else if (reg_desplazamiento3 == 0b10010000)////  1100 1001 0011 0110
                                             {
                                                reg_desplazamiento3 = 0b11000000;
                                                output_bit(pin_c4,1);
                                                output_bit(pin_c5,1);
                                                output_bit(pin_c6,0);
                                                output_bit(pin_c7,0);
                                                delay_ms(time);
                                             }
                                    }
                            while(input(limit_switch_4) == 0) // si no se ha presionado limit_switch_4, mueve el servomotor
                                {
                                    duty_servo = 5;    // 500us ===> 0 grados
                                }
         
                  avanzar = 1;
                  break;
         //Luego de inicializar los 4 motores avanzamos a guardar las 5 posiciones:
         // los 4 potenciometros deben estar en cero.
         
         case posicionar_posicion1:   // el usuario tiene la libertad de llevar los 3 motores a posicion arbitraria
                                
                                 output_bit(led_on,1);
                                 output_bit(led_off,0);
  
                       ////////////////////////////////////////////////////////////////////////*// 
                                 set_adc_channel(0);               //potenciometro del motor1
                                 delay_us(100);
                                 adc = read_adc(ADC_START_AND_READ);
                                 adc_escalado1 = adc*2;    // como el motor es de 2048 pasos cada vuelta en "paso normal", escalamos los 1023 binarios a 2048 posiciones enteras...!!       
                                 MOVER_MOTOR1();
                                 
                      ////////////////////////////////////////////////////////////////////////////////////////
                      
                      ///////////////////////////////////////////////////////////////////////////////////////
                                 set_adc_channel(1);       //Potenciometro del motor 2
                                 delay_us(100);
                                 adc = read_adc(ADC_START_AND_READ);
                                 adc_escalado2 = adc*2;    // como el motor es de 2048 pasos cada vuelta, escalamos los 1023 binarios a 512 posiciones enteras...!!
                                 MOVER_MOTOR2();
                              
                       //////////////////////////////////////////////////////////////////////////////////////////
                     
                       ////////////////////////////////////////////////////////////////////////////////////////
                                 set_adc_channel(2);       //Potenciometro del motor 3
                                 delay_us(100);
                                 adc = read_adc(ADC_START_AND_READ);
                                 adc_escalado3 = adc*2;    // como el motor es de 2048 pasos cada vuelta, escalamos los 1023 binarios a 2048 posiciones enteras...!!
                                 MOVER_MOTOR3();
                       /////////////////////////////////////////////////////////////////////////////////////////
                       
                       ////////////////////////////////////////////////////////////////////////////////////////          
                                 set_adc_channel(3);       //Potenciometro del servomotor
                                 delay_us(100);
                                 adc = read_adc(ADC_START_AND_READ);
                                 adc_escalado4 = adc/40;    //escalamos los 1023 binarios a 25 para el duty del servo...!!
                                 MOVER_SERVO();
                      ////////////////////////////////////////////////////////////////////////////////////////////      
      
                  break;
       
         case esperar_1: 
                               
                                 output_bit(led_on,0);
                                 output_bit(led_off,1);
                                 delay_ms(100);
///// Estado de espera......
                        
                  break; 
                  
         case posicionar_posicion2:
                                
                                 
                                 output_bit(led_on,1);
                                 output_bit(led_off,0);
 
                                 ////////////////////////////////////////////////////////////////////////*// 
                                 set_adc_channel(0);               //potenciometro del motor1
                                 delay_us(100);
                                 adc = read_adc(ADC_START_AND_READ);
                                 adc_escalado1 = adc*2;    // como el motor es de 2048 pasos cada vuelta, escalamos los 1023 binarios a 2048 posiciones enteras...!!       
                                 MOVER_MOTOR1();
                                 
                      ////////////////////////////////////////////////////////////////////////////////////////
                      
                      ///////////////////////////////////////////////////////////////////////////////////////
                                 set_adc_channel(1);       //Potenciometro del motor 2
                                 delay_us(100);
                                 adc = read_adc();
                                 adc_escalado2 = adc*2;    // como el motor es de 2048 pasos cada vuelta, escalamos los 1023 binarios a 512 posiciones enteras...!!
                                 MOVER_MOTOR2();
                              
                       //////////////////////////////////////////////////////////////////////////////////////////
                     
                       ////////////////////////////////////////////////////////////////////////////////////////
                                 set_adc_channel(2);       //Potenciometro del motor 3
                                 delay_us(100);
                                 adc = read_adc();
                                 adc_escalado3 = adc*2;    // como el motor es de 2048 pasos cada vuelta, escalamos los 1023 binarios a 2048 posiciones enteras...!!
                                 MOVER_MOTOR3();
      
                      ////////////////////////////////////////////////////////////////////////////////////////          
                                 set_adc_channel(3);       //Potenciometro del servomotor
                                 delay_us(100);
                                 adc = read_adc(ADC_START_AND_READ);
                                 adc_escalado4 = adc/40;    // como el motor es de 2048 pasos cada vuelta, escalamos los 1023 binarios a 2048 posiciones enteras...!!
                                 MOVER_SERVO();
                      ////////////////////////////////////////////////////////////////////////////////////////////      
      
                  break;
         
           case esperar_2: 
                               
                                 output_bit(led_on,0);
                                 output_bit(led_off,1);
                                 delay_ms(100);
///// Estado de espera......
                        
                  break; 
                  
         case posicionar_posicion3:
         
                               
                                 output_bit(led_on,1);
                                 output_bit(led_off,0);

                                 set_adc_channel(0);               //potenciometro del motor1
                                 delay_us(100);
                                 adc = read_adc(ADC_START_AND_READ);
                                 adc_escalado1 = adc*2;    // como el motor es de 2048 pasos cada vuelta, escalamos los 1023 binarios a 2048 posiciones enteras...!!       
                                 MOVER_MOTOR1();
                                 
                      ////////////////////////////////////////////////////////////////////////////////////////
                      
                      ///////////////////////////////////////////////////////////////////////////////////////
                                 set_adc_channel(1);       //Potenciometro del motor 2
                                 delay_us(100);
                                 adc = read_adc();
                                 adc_escalado2 = adc*2;    // como el motor es de 2048 pasos cada vuelta, escalamos los 1023 binarios a 512 posiciones enteras...!!
                                 MOVER_MOTOR2();
                              
                       //////////////////////////////////////////////////////////////////////////////////////////
                     
                       ////////////////////////////////////////////////////////////////////////////////////////
                                 set_adc_channel(2);       //Potenciometro del motor 3
                                 delay_us(100);
                                 adc = read_adc();
                                 adc_escalado3 = adc*2;    // como el motor es de 2048 pasos cada vuelta, escalamos los 1023 binarios a 2048 posiciones enteras...!!
                                 MOVER_MOTOR3();
      
                     ////////////////////////////////////////////////////////////////////////////////////////
                                 set_adc_channel(3);       //Potenciometro del servomotor
                                 delay_us(100);
                                 adc = read_adc(ADC_START_AND_READ);
                                 adc_escalado4 = adc/40;    // como el motor es de 2048 pasos cada vuelta, escalamos los 1023 binarios a 2048 posiciones enteras...!!
                                 MOVER_SERVO();
                      ////////////////////////////////////////////////////////////////////////////////////////////      
      
                  break;
         
           case esperar_3: 
                               
                                 output_bit(led_on,0);
                                 output_bit(led_off,1);
                                 delay_ms(100);
///// Estado de espera......
                        
                  break; 
                  
         case posicionar_posicion4:
         
                               
                                 output_bit(led_on,1);
                                 output_bit(led_off,0);

                                 set_adc_channel(0);               //potenciometro del motor1
                                 delay_us(100);
                                 adc = read_adc(ADC_START_AND_READ);
                                 adc_escalado1 = adc*2;    // como el motor es de 2048 pasos cada vuelta, escalamos los 1023 binarios a 2048 posiciones enteras...!!       
                                 MOVER_MOTOR1();
                                 
                      ////////////////////////////////////////////////////////////////////////////////////////
                      
                      ///////////////////////////////////////////////////////////////////////////////////////
                                 set_adc_channel(1);       //Potenciometro del motor 2
                                 delay_us(100);
                                 adc = read_adc();
                                 adc_escalado2 = adc*2;    // como el motor es de 2048 pasos cada vuelta, escalamos los 1023 binarios a 512 posiciones enteras...!!
                                 MOVER_MOTOR2();
                              
                       //////////////////////////////////////////////////////////////////////////////////////////
                     
                       ////////////////////////////////////////////////////////////////////////////////////////
                                 set_adc_channel(2);       //Potenciometro del motor 3
                                 delay_us(100);
                                 adc = read_adc();
                                 adc_escalado3 = adc*2;    // como el motor es de 2048 pasos cada vuelta, escalamos los 1023 binarios a 2048 posiciones enteras...!!
                                 MOVER_MOTOR3();
      
                     ////////////////////////////////////////////////////////////////////////////////////////
                                 set_adc_channel(3);       //Potenciometro del servomotor
                                 delay_us(100);
                                 adc = read_adc(ADC_START_AND_READ);
                                 adc_escalado4 = adc/40;    // como el motor es de 2048 pasos cada vuelta, escalamos los 1023 binarios a 2048 posiciones enteras...!!
                                 MOVER_SERVO();
                      ////////////////////////////////////////////////////////////////////////////////////////////      
      
                  break;
                 
                  case esperar_4:
                               
                                 output_bit(led_on,0);
                                 output_bit(led_off,1);
                                 delay_ms(100);
///// Estado de espera......
                        
                  break;  
                  
                  case posicionar_posicion5:
         
                               
                                 output_bit(led_on,1);
                                 output_bit(led_off,0);

                                 set_adc_channel(0);               //potenciometro del motor1
                                 delay_us(100);
                                 adc = read_adc(ADC_START_AND_READ);
                                 adc_escalado1 = adc*2;    // como el motor es de 2048 pasos cada vuelta, escalamos los 1023 binarios a 2048 posiciones enteras...!!       
                                 MOVER_MOTOR1();
                                 
                      ////////////////////////////////////////////////////////////////////////////////////////
                      
                      ///////////////////////////////////////////////////////////////////////////////////////
                                 set_adc_channel(1);       //Potenciometro del motor 2
                                 delay_us(100);
                                 adc = read_adc();
                                 adc_escalado2 = adc*2;    // como el motor es de 2048 pasos cada vuelta, escalamos los 1023 binarios a 512 posiciones enteras...!!
                                 MOVER_MOTOR2();
                              
                       //////////////////////////////////////////////////////////////////////////////////////////
                     
                       ////////////////////////////////////////////////////////////////////////////////////////
                                 set_adc_channel(2);       //Potenciometro del motor 3
                                 delay_us(100);
                                 adc = read_adc();
                                 adc_escalado3 = adc*2;    // como el motor es de 2048 pasos cada vuelta, escalamos los 1023 binarios a 2048 posiciones enteras...!!
                                 MOVER_MOTOR3();
      
                     ////////////////////////////////////////////////////////////////////////////////////////
                                 set_adc_channel(3);       //Potenciometro del servomotor
                                 delay_us(100);
                                 adc = read_adc(ADC_START_AND_READ);
                                 adc_escalado4 = adc/40;    // como el motor es de 2048 pasos cada vuelta, escalamos los 1023 binarios a 2048 posiciones enteras...!!
                                 MOVER_SERVO();
                      ////////////////////////////////////////////////////////////////////////////////////////////      
      
                  break;
        
             case esperar_5: 
                               
                                 output_bit(led_on,0);
                                 output_bit(led_off,1);
                                 delay_ms(100);
///// Estado de espera......
                        
                  break; 
                  
           case ejecutar_posicion1:
                              
                                  time = 12; //se garantiza velocidad rapida para los paso a paso.
                                 output_bit(led_on,1);
                                 output_bit(led_off,0);
                                 PARO();
                       ////////////////////////////////////////////////////////////////////////*// 
                                 adc_escalado1 = reg_memory_1_1;    // como el motor es de 2048 pasos cada vuelta, escalamos los 1023 binarios a 2048 posiciones enteras...!!       
                                 MOVER_MOTOR1();
                                 
                      ////////////////////////////////////////////////////////////////////////////////////////
                      
                      ///////////////////////////////////////////////////////////////////////////////////////
                                 adc_escalado2 = reg_memory_2_1;    // como el motor es de 2048 pasos cada vuelta, escalamos los 1023 binarios a 512 posiciones enteras...!!
                                 MOVER_MOTOR2();
                              
                       //////////////////////////////////////////////////////////////////////////////////////////
                     
                       ////////////////////////////////////////////////////////////////////////////////////
                                 adc_escalado3 = reg_memory_3_1;    // como el motor es de 2048 pasos cada vuelta, escalamos los 1023 binarios a 2048 posiciones enteras...!!
                                 MOVER_MOTOR3();
                                 
                                 duty_servo = reg_memory_4_1;
      
                  break;
            
            
            
        case ejecutar_posicion2:
                                 output_bit(led_on,1);
                                 output_bit(led_off,0);
                                 PARO();
                  ////////////////////////////////////////////////////////////////////////*// 
                                 adc_escalado1 = reg_memory_1_2;    // como el motor es de 2048 pasos cada vuelta, escalamos los 1023 binarios a 2048 posiciones enteras...!!       
                                 MOVER_MOTOR1();
                                 
                      ////////////////////////////////////////////////////////////////////////////////////////
                      
                      ///////////////////////////////////////////////////////////////////////////////////////
                                 adc_escalado2 = reg_memory_2_2;    // como el motor es de 2048 pasos cada vuelta, escalamos los 1023 binarios a 512 posiciones enteras...!!
                                 MOVER_MOTOR2();
                              
                       //////////////////////////////////////////////////////////////////////////////////////////
                     
                       ////////////////////////////////////////////////////////////////////////////////////
                                 adc_escalado3 = reg_memory_3_2;    // como el motor es de 2048 pasos cada vuelta, escalamos los 1023 binarios a 2048 posiciones enteras...!!
                                 MOVER_MOTOR3();
                                 
                                  duty_servo = reg_memory_4_2;
                                    
                  break;
                                    
          case ejecutar_posicion3:
          
                                 output_bit(led_on,1);
                                 output_bit(led_off,0);
                                 PARO();
                  ////////////////////////////////////////////////////////////////////////*// 
                                 adc_escalado1 = reg_memory_1_3;    // como el motor es de 2048 pasos cada vuelta, escalamos los 1023 binarios a 2048 posiciones enteras...!!       
                                 MOVER_MOTOR1();
                                 
                      ////////////////////////////////////////////////////////////////////////////////////////
                      
                      ///////////////////////////////////////////////////////////////////////////////////////
                                 adc_escalado2 = reg_memory_2_3;    // como el motor es de 2048 pasos cada vuelta, escalamos los 1023 binarios a 512 posiciones enteras...!!
                                 MOVER_MOTOR2();
                              
                       //////////////////////////////////////////////////////////////////////////////////////////
                     
                       ////////////////////////////////////////////////////////////////////////////////////
                                 adc_escalado3 = reg_memory_3_3;    // como el motor es de 2048 pasos cada vuelta, escalamos los 1023 binarios a 2048 posiciones enteras...!!
                                 MOVER_MOTOR3();
                                 
                                 duty_servo = reg_memory_4_3;                 
      
                  break;
                  
                case ejecutar_posicion4:
          
                                 output_bit(led_on,1);
                                 output_bit(led_off,0);
                                 PARO();
                  ////////////////////////////////////////////////////////////////////////*// 
                                 adc_escalado1 = reg_memory_1_4;    // como el motor es de 2048 pasos cada vuelta, escalamos los 1023 binarios a 2048 posiciones enteras...!!       
                                 MOVER_MOTOR1();
                                 
                      ////////////////////////////////////////////////////////////////////////////////////////
                      
                      ///////////////////////////////////////////////////////////////////////////////////////
                                 adc_escalado2 = reg_memory_2_4;    // como el motor es de 2048 pasos cada vuelta, escalamos los 1023 binarios a 512 posiciones enteras...!!
                                 MOVER_MOTOR2();
                              
                       //////////////////////////////////////////////////////////////////////////////////////////
                     
                       ////////////////////////////////////////////////////////////////////////////////////
                                 adc_escalado3 = reg_memory_3_4;    // como el motor es de 2048 pasos cada vuelta, escalamos los 1023 binarios a 2048 posiciones enteras...!!
                                 MOVER_MOTOR3();
                                 
                                 duty_servo = reg_memory_4_4;                 
      
                  break;
                  
              case ejecutar_posicion5:
          
                                 output_bit(led_on,1);
                                 output_bit(led_off,0);
                                 PARO();
                  ////////////////////////////////////////////////////////////////////////*// 
                                 adc_escalado1 = reg_memory_1_5;    // como el motor es de 2048 pasos cada vuelta, escalamos los 1023 binarios a 2048 posiciones enteras...!!       
                                 MOVER_MOTOR1();
                                 
                      ////////////////////////////////////////////////////////////////////////////////////////
                      
                      ///////////////////////////////////////////////////////////////////////////////////////
                                 adc_escalado2 = reg_memory_2_5;    // como el motor es de 2048 pasos cada vuelta, escalamos los 1023 binarios a 512 posiciones enteras...!!
                                 MOVER_MOTOR2();
                              
                       //////////////////////////////////////////////////////////////////////////////////////////
                     
                       ////////////////////////////////////////////////////////////////////////////////////
                                 adc_escalado3 = reg_memory_3_5;    // como el motor es de 2048 pasos cada vuelta, escalamos los 1023 binarios a 2048 posiciones enteras...!!
                                 MOVER_MOTOR3();
                                 
                                 duty_servo = reg_memory_4_5;                 
      
                  break;
                
                  
      }
}
    
void MOVER_MOTOR1()
{
                           
                                 if (posicion1 == adc_escalado1 || posicion1 == adc_escalado1 +1 || posicion1 == adc_escalado1 -1 || posicion1 == adc_escalado1 +2 || posicion1 == adc_escalado1 -2 || posicion1 == adc_escalado1 +3 || posicion1 == adc_escalado1 -3 || posicion1 == adc_escalado1 +4 || posicion1 == adc_escalado1 -4 || posicion1 == adc_escalado1 +5 || posicion1 == adc_escalado1 -5) // si la posicion a lazo abierto es similar a la posicion del potenciometro todo se queda quieto..!!
                                       {
                                             alcanza_1=1;
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
                                if (posicion2 == adc_escalado2 || posicion2 == adc_escalado2 +1 || posicion2 == adc_escalado2 -1 || posicion2 == adc_escalado2 -2 || posicion2 == adc_escalado2 +2 || posicion2 == adc_escalado2 -3 || posicion2 == adc_escalado2 +3 || posicion2 == adc_escalado2 -4 || posicion2 == adc_escalado2 +4 || posicion2 == adc_escalado2 -5 || posicion2 == adc_escalado2 +5) // si la posicion a lazo abierto es similar a la posicion del potenciometro todo se queda quieto..!!
                                       {
                 
                                             alcanza_2=1;

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
                                                reg_desplazamiento2 = 0b11000000;
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
                                           else if (reg_desplazamiento2 == 0b00000000)////  1100 1001 0011 0110
                                             {
                                                reg_desplazamiento2 = 0b00110000;
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
                          
                                 if (posicion3 == adc_escalado3 || posicion3 == adc_escalado3 +1 || posicion3 == adc_escalado3 -1 || posicion3 == adc_escalado3 -2 || posicion3 == adc_escalado3 +2 || posicion3 == adc_escalado3 -3 || posicion3 == adc_escalado3 +3 || posicion3 == adc_escalado3 -4 || posicion3 == adc_escalado3 +4 || posicion3 == adc_escalado3 -5 || posicion3 == adc_escalado3 +5) // si la posicion a lazo abierto es similar a la posicion del potenciometro todo se queda quieto..!!
                                       {
                                          alcanza_3=1;
                                       }
                                 else if (posicion3 < adc_escalado3)  // si la posicion del potenciometro es mayor a la posicion del motor a lazo abierto, haz que mueva a un lado... 
                                       {
                                           if (reg_desplazamiento3 == 0b11000000)////  1100 1001 0011 0110
                                             {                                    
                                                reg_desplazamiento3 = 0b10010000;
                                                posicion3 = posicion3 + 1;
                                                output_bit(pin_c4,1);
                                                output_bit(pin_c5,0);
                                                output_bit(pin_c6,0);
                                                output_bit(pin_c7,1);
                                                delay_ms(time);
                                             }
                                           else if (reg_desplazamiento3 == 0b10010000)////  1100 1001 0011 0110
                                             {
                                                reg_desplazamiento3 = 0b00110000;
                                                posicion3 = posicion3 + 1;
                                                output_bit(pin_c4,0);
                                                output_bit(pin_c5,0);
                                                output_bit(pin_c6,1);
                                                output_bit(pin_c7,1);
                                                delay_ms(time);
                                             }
                                             else if (reg_desplazamiento3 == 0b00110000)////  1100 1001 0011 0110
                                             {
                                                reg_desplazamiento3 = 0b01100000;
                                                posicion3 = posicion3 + 1;
                                                output_bit(pin_c4,0);
                                                output_bit(pin_c5,1);
                                                output_bit(pin_c6,1);
                                                output_bit(pin_c7,0);
                                                delay_ms(time);
                                             }
                                             else if (reg_desplazamiento3 == 0b01100000)////  1100 1001 0011 0110
                                             {
                                                reg_desplazamiento3 = 0b11000000;
                                                posicion3 = posicion3 + 1;
                                                output_bit(pin_c4,1);
                                                output_bit(pin_c5,1);
                                                output_bit(pin_c6,0);
                                                output_bit(pin_c7,0);
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
                                                output_bit(pin_c4,0);
                                                output_bit(pin_c5,1);
                                                output_bit(pin_c6,1);
                                                output_bit(pin_c7,0);
                                                delay_ms(time);
                                             }
                                           else if (reg_desplazamiento3 == 0b01100000)////  1100 1001 0011 0110
                                             {
                                                reg_desplazamiento3 = 0b00110000;
                                                posicion3 = posicion3 - 1;
                                                output_bit(pin_c4,0);
                                                output_bit(pin_c5,0);
                                                output_bit(pin_c6,1);
                                                output_bit(pin_c7,1);
                                                delay_ms(time);
                                             }
                                             else if (reg_desplazamiento3 == 0b00110000)////  1100 1001 0011 0110
                                             {
                                                reg_desplazamiento3 = 0b10010000;
                                                posicion3 = posicion3 - 1;
                                                output_bit(pin_c4,1);
                                                output_bit(pin_c5,0);
                                                output_bit(pin_c6,0);
                                                output_bit(pin_c7,1);
                                                delay_ms(time);
                                             }
                                             else if (reg_desplazamiento3 == 0b10010000)////  1100 1001 0011 0110
                                             {
                                                reg_desplazamiento3 = 0b11000000;
                                                posicion3 = posicion3 - 1;
                                                output_bit(pin_c4,1);
                                                output_bit(pin_c5,1);
                                                output_bit(pin_c6,0);
                                                output_bit(pin_c7,0);
                                                delay_ms(time);
                                             }
                                    }
}

void MOVER_SERVO()
{
   //5-25
   if (adc_escalado4 < 5)
   {adc_escalado4 = 5;}
   if(adc_escalado4 >25)
   {adc_escalado4 = 25;}
   
   duty_servo = adc_escalado4;
}



void DESBASTAR()
{
    output_bit(led_on,1);
    output_bit(led_off,0);
    delay_ms(1000);
    velocidad_dc_derecho = 1;   // activamos el motor  la derecha al 50%;
    velocidad_dc_izquierdo = 0;   // desactivamos el giro del motor a la izquierda al 100%;
    duty_servo = 5;
   
                // movemos el servo a 90 grados para bajar la herramient de corte
                
    for(int i=0 ; i<20 ; i++)  
    {
         duty_servo++;
         delay_ms(200);   // 4 seg incrementando el duty del servo para ir bajando
    }
    delay_ms(1000);   //esperamos para desbastar bien
    velocidad_dc_derecho = 0;   // desactivamos el motor  la derecha al 50%;
    velocidad_dc_izquierdo = 1;   // activamos el giro del motor a la izquierda al 100%;
    
    for(int i=0 ; i<20 ; i++)  
    {
         duty_servo--;
         delay_ms(200);   // 4 seg  para ir subiendo
    }
    velocidad_dc_derecho = 0;   // desactivamos el motor  la derecha al 50%;
    velocidad_dc_izquierdo = 0;   // desactivamos el giro del motor a la izquierda al 100%;
    delay_ms(1000);   //esperamos...

}

void PARO()
{
   if (input(stop) == 1)
   {
         while(1)
         {
         
         output_bit(led_on,0);
         output_bit(led_off,1);
         velocidad_dc_derecho = 0;   // desactivamos el motor  la derecha al 50%;
         velocidad_dc_izquierdo = 0;   // desactivamos el giro del motor a la izquierda al 100%;
         duty_servo = 5; // se coloca arriba
         delay_ms(10);
         
         }  
   }
}

