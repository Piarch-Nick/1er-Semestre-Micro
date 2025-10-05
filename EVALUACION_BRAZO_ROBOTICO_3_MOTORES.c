#include <EVALUACION_BRAZO_ROBOTICO_3_MOTORES.h>

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

/////////ENTRADAS////////////////////////////////////////
///////////////////////////////////////////////////
/////pulsadores//////////
#define guardar1           pin_c0
#define guardar2           pin_c1
#define guardar3           pin_c2
#define ejecutar           pin_c3
////////pines analogicos//////////////
#define adc_motor1         pin_a0
#define adc_motor2         pin_a1
#define adc_motor3         pin_a2



////////////////SALIDAS///////////////////////
/////////////////////////////////////
#define led0           pin_a3
#define led1           pin_a4

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


//      SERVOMOTOR = RA5
//////////////////////////////////



///////////////ESTADOS//////////////////
/////////////////////////////////////
#define posicionar_posicion1       0
#define posicionar_posicion2       1
#define posicionar_posicion3       2
#define esperar                    3
#define ejecutar_posicion1         4
#define ejecutar_posicion2         5
#define ejecutar_posicion3         6


/////////////////////////////////////////////////


//// Prototipos de Funciones 
void initial ();
void elegir_estado_actual();
void ejecutar_estado_actual();
void MOVER_MOTOR1();
void MOVER_MOTOR2();
void MOVER_MOTOR3();
void Modificar_Velocidad();
void Paro_Emergencia();
//////////////////////////////////

//////Variables a usar//////
int1 DUTY;
int count1;
int16 adc;                        //toma el valor de 10bits del ADC
//int16 adc_escalado;                // divide para ajustar el valor
int speed;
int16 adc_escalado1;                // divide para ajustar el valor
int16 adc_escalado2;                // divide para ajustar el valor
int16 adc_escalado3;                // divide para ajustar el valor
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

int16 reg_memory_1_2;                  // registro para registrar la posicion a lazo abierto del motor 1
int16 reg_memory_2_2;                  // registro para registrar la posicion a lazo abierto del motor 2
int16 reg_memory_3_2;                  // registro para registrar la posicion a lazo abierto del motor 3

int16 reg_memory_1_3;                  // registro para registrar la posicion a lazo abierto del motor 1
int16 reg_memory_2_3;                  // registro para registrar la posicion a lazo abierto del motor 2
int16 reg_memory_3_3;                  // registro para registrar la posicion a lazo abierto del motor 3

int1 alcanza_1;
int1 alcanza_2;
int1 alcanza_3;
/////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#INT_TIMER0
void interrupt()   // cada 100us entra
{
      if(count1 == 200)      //perido 20ms  //frecuencia 50hz
      {
            output_bit(pin_A5,1);
            count1=0;
      }
      else if (count1 < 200)
      {
           if(DUTY == 0) //DEFAULT
           {
             if(count1 == 6)   // DUTY DE 600us. SERVOMOTOR EN POSICION 0 GRADOS.  
             {
                  output_bit(pin_A5,0);
             }
           }
           else if(DUTY == 1)
           {
             if(count1 == 25)   // DUTY DE 2.5ms. SERVOMOTOR EN POSICION 180 GRADOS.
             {
                  output_bit(pin_A5,0);
             }
           }
              count1++;
      }
      set_timer0(6);
}
      
      
      
      
/////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////  PRINCIPAL   ////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void main()
{
   setup_timer_0(RTCC_INTERNAL | RTCC_8_BIT |RTCC_DIV_2);
   set_timer0(6);                  // desborda en 100us // cada tic es de 400ns
                                   
   disable_interrupts(GLOBAL);
   disable_interrupts(INT_TIMER0);
   
   setup_adc(adc_clock_internal);
   setup_adc_ports(sAN0 | sAN1 | sAN2 ); 
   initial();      // inicializar variables

   while(true)
   {
      elegir_estado_actual();    // maquina de mealy que, en funcion del estado actual y de las entradas decide cual es su estado futuro...
      ejecutar_estado_actual();  // ejecuta el estado correspodiente..
   }
}

















void initial ()
{
DUTY=0; //POR DEFAULT EL SERVO EN 0 GRADOS
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
reg_desplazamiento1 = 0b00000011;  // comenzar a mover el motor1
reg_desplazamiento2 = 0b00110000;  // comenzar a mover el motor1
reg_desplazamiento3 = 0b11000000;  // comenzar a mover el motor1
time=15;
output_b(0b00000000);
output_bit(pin_c4,0);
output_bit(pin_c5,0);
output_bit(pin_c6,0);
output_bit(pin_c7,0);

estado_actual=posicionar_posicion1; //estado_0
alcanza_1=0;
alcanza_2=0;
alcanza_3=0;
speed=0;
}
  



void elegir_estado_actual()
{
   if (estado_actual == posicionar_posicion1 && input(guardar1) == 0)
         {
            estado_actual = estado_actual;
         }
   else if (estado_actual == posicionar_posicion1 && input(guardar1) == 1)  // boton de guardar1 presionado
      {                       
         estado_actual = posicionar_posicion2;   
         reg_memory_1_1 = posicion1;
         reg_memory_2_1 = posicion2;
         reg_memory_3_1 = posicion3;
      }
   else if (estado_actual == posicionar_posicion2 && input(guardar2) == 1)   // boton de guardar2 presionado
      {
         estado_actual = posicionar_posicion3; 
         reg_memory_1_2 = posicion1;
         reg_memory_2_2 = posicion2;
         reg_memory_3_2 = posicion3;
      }
   else if (estado_actual == posicionar_posicion3 && input(guardar3) == 1)  // boton de guardar3 presionado
      {                                      // Debe estar el motor2 colocado en posicion 0.
         estado_actual = esperar;            // Estado de esperar
         reg_memory_1_3 = posicion1;
         reg_memory_2_3 = posicion2;
         reg_memory_3_3 = posicion3;
      }
   else if (estado_actual == esperar && input(ejecutar) == 1)  // boton de EJECUTAR
      {
         estado_actual = ejecutar_posicion1; 
         alcanza_1=0;
         alcanza_2=0;
         alcanza_3=0;                          
         enable_interrupts(GLOBAL);
         enable_interrupts(INT_TIMER0);
         set_timer0(6);                  // desborda en 100us // cada tic es de 400ns 
      }
      else if (estado_actual == ejecutar_posicion1 && alcanza_1 == 1 && alcanza_2 == 1 && alcanza_3 == 1)  
      {
         DUTY=0;    //0 GRADOS
         delay_ms(1000);
         estado_actual = ejecutar_posicion2; 
         alcanza_1 = 0;
         alcanza_2 = 0;
         alcanza_3 = 0;
      }
          else if (estado_actual == ejecutar_posicion2 && alcanza_1 == 1 && alcanza_2 == 1 && alcanza_3 == 1)  
      {
      delay_ms(1000);
         estado_actual = ejecutar_posicion3; 
         alcanza_1 = 0;
         alcanza_2 = 0;
         alcanza_3 = 0;
      }
           else if (estado_actual == ejecutar_posicion3 && alcanza_1 == 1 && alcanza_2 == 1 && alcanza_3 == 1)  
      {
         DUTY=1;
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
         case posicionar_posicion1:   //Estado cero, el usuario tiene la libertad y la obligacion de llevar los 3 motores a posicion cero.
                                 output_bit(led0,0);
                                 output_bit(led1,0);
  
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
      
                  break;
       
         case posicionar_posicion2:  // Estado de mover motor1 a la posicion de guardado. Debio ser inicializado a posicion 0...
                                 output_bit(led0,1);
                                 output_bit(led1,0);//////LEDS ...
 
                                 
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
      
                  break;
         
         case posicionar_posicion3:
                                 output_bit(led0,0);
                                 output_bit(led1,1);

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
      
                  break;
         
          case esperar: 
                                 output_toggle(led0);
                                 output_toggle(led1);
                                 delay_ms(100);
///// Estado de espera......
                        
                  break; 
           
           case ejecutar_posicion1:
                                 output_bit(led0,0);
                                 output_bit(led1,0);
                                 Modificar_Velocidad();
                                 Paro_Emergencia();
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
      
                  break;
            
            
            
        case ejecutar_posicion2:
                                 output_bit(led0,1);
                                 output_bit(led1,0);
                                 Modificar_Velocidad();
                                 Paro_Emergencia();
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
      
                  break;
                                    
          case ejecutar_posicion3:
          
                                 output_bit(led0,1);
                                 output_bit(led1,1);
                                 Modificar_Velocidad();
                                 Paro_Emergencia();
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
                                 
                                 
      
                  break;
                  
                
                  
      }
}
    /*
                
         case 6:         // Estado de mover los motores a la posicion inicial y a la posicion guardada y repetir eso.
        //////////////////////////////////////////////
                                 emergencia();
                                 Modificar_Velocidad();
     /////////////////////////////////////////////
                                 output_bit(pin_A3,0);
                                 output_bit(pin_A4,1);

         
                 //////////////////INICIALIZAR MOTOR 1///////////////////////////////////
                                 output_bit(pin_b4,0);  // apagamos bits del motor2
                                 output_bit(pin_b5,0);
                                 output_bit(pin_b6,0);
                                 output_bit(pin_b7,0);
                                 output_bit(pin_c4,0);  // apagamos bits del motor3
                                 output_bit(pin_c5,0);
                                 output_bit(pin_c6,0);
                                 output_bit(pin_c7,0);         
                                 reg_desplazamiento = 0b00000011;
                                 
                                          for (int16 w1 = reg_memory_1 ; w1 > 0 ; w1=w1-1)
                                          {
                                
                                              if (reg_desplazamiento == 0b00000011)
                                             {
                                                reg_desplazamiento = 0b00001001;
                                                output_b (reg_desplazamiento);
                                                delay_ms(time);
                                             }
                                          else if (reg_desplazamiento == 0b00001001)
                                             {
                                                reg_desplazamiento = 0b00001100;
                                                output_b (reg_desplazamiento);
                                                delay_ms(time);
                                             }
                                          else
                                             {
                                                reg_desplazamiento = reg_desplazamiento >> 1;        
                                                output_b (reg_desplazamiento);
                                                delay_ms(time);  
                                                
                                                 //////////////////////////////////////////////
                                                 emergencia();
                                                 Modificar_Velocidad();
                                                      /////////////////////////////////////////////
                                             }
                                          }
                                
                  ////////////////////INICIALIZAR MOTOR 2//////////////////////////////
                                 output_bit(pin_b0,0);  // apagamos bits del motor1
                                 output_bit(pin_b1,0);
                                 output_bit(pin_b2,0);
                                 output_bit(pin_b3,0);
                                 output_bit(pin_c4,0);  // apagamos bits del motor3
                                 output_bit(pin_c5,0);
                                 output_bit(pin_c6,0);
                                 output_bit(pin_c7,0);         
                                 reg_desplazamiento = 0b00110000;
                                 
                                          for (int16 w2 = reg_memory_2 ; w2 > 0 ; w2=w2-1)
                                          {
                                             if (reg_desplazamiento == 0b00110000)
                                             {
                                                reg_desplazamiento = 0b10010000;
                                                output_b (reg_desplazamiento);
                                                delay_ms(time);
                                             }
                                          else if (reg_desplazamiento == 0b10010000)
                                             {
                                                reg_desplazamiento = 0b11000000;
                                                output_b (reg_desplazamiento);
                                                delay_ms(time);
                                             }
                                          else
                                             {
                                                reg_desplazamiento = reg_desplazamiento >> 1;         
                                                output_b (reg_desplazamiento);
                                                delay_ms(time);
                                                
                                                 //////////////////////////////////////////////
                                              emergencia();
                                               Modificar_Velocidad();
                                                    /////////////////////////////////////////////
                                             }      
                                          }
                                          
                 ////////////////////INICIALIZAR MOTOR 3//////////////////////////////
                                 output_bit(pin_b0,0);  // apagamos bits del motor1
                                 output_bit(pin_b1,0);
                                 output_bit(pin_b2,0);
                                 output_bit(pin_b3,0);
                                 output_bit(pin_b4,0);  // apagamos bits del motor2
                                 output_bit(pin_b5,0);
                                 output_bit(pin_b6,0);
                                 output_bit(pin_b7,0);         
                                 reg_desplazamiento = 0b11000000;
                                 
                                          for (int16 w3 = reg_memory_3 ; w3 > 0 ; w3=w3-1)
                                          {
                                              if (reg_desplazamiento == 0b11000000)////  1100 1001 0011 0110
                                             {                                    
                                                reg_desplazamiento = 0b01100000;
                                                output_bit(pin_c7,0);
                                                output_bit(pin_c6,1);
                                                output_bit(pin_c5,1);
                                                output_bit(pin_c4,0);
                                                delay_ms(time);
                                             }
                                           else if (reg_desplazamiento == 0b01100000)////  1100 1001 0011 0110
                                             {
                                                reg_desplazamiento = 0b00110000;
                                                output_bit(pin_c7,0);
                                                output_bit(pin_c6,0);
                                                output_bit(pin_c5,1);
                                                output_bit(pin_c4,1);
                                                delay_ms(time);
                                             }
                                             else if (reg_desplazamiento == 0b00110000)////  1100 1001 0011 0110
                                             {
                                                reg_desplazamiento = 0b10010000;
                                                output_bit(pin_c7,1);
                                                output_bit(pin_c6,0);
                                                output_bit(pin_c5,0);
                                                output_bit(pin_c4,1);
                                                delay_ms(time);
                                             }
                                             else if (reg_desplazamiento == 0b10010000)////  1100 1001 0011 0110
                                             {
                                                reg_desplazamiento = 0b11000000;
                                                output_bit(pin_c7,1);
                                                output_bit(pin_c6,1);
                                                output_bit(pin_c5,0);
                                                output_bit(pin_c4,0);
                                                delay_ms(time);
                                                
                                                 //////////////////////////////////////////////
                                                  emergencia();
                                                   Modificar_Velocidad();
                                                   /////////////////////////////////////////////
                                             }
                                          }                       
                                
                                
                         //////////////RETARDO///////////////
                                delay_ms(1000);
                                
                        ////////////////////////////////////////// 
                                
               /////////////////////IR A LA POSICION GUARDADA DEL MOTOR1/////////////////////////////////////
               
                                 output_bit(pin_b4,0);  // apagamos bits del motor2
                                 output_bit(pin_b5,0);
                                 output_bit(pin_b6,0);
                                 output_bit(pin_b7,0);
                                 output_bit(pin_c4,0);  // apagamos bits del motor3
                                 output_bit(pin_c5,0);
                                 output_bit(pin_c6,0);
                                 output_bit(pin_c7,0);
                                 
                                 reg_desplazamiento = 0b00001100;  // comenzar a mover el motor1
               
                                for (int16 w1 = 0 ; w1 < reg_memory_1 ; w1++)
                                          {
                                                                                          
                                           if (reg_desplazamiento == 0b00001100)////  1100 1001 0011 0110
                                             {                                    
                                                reg_desplazamiento = 0b00001001;
                                                output_b (reg_desplazamiento);
                                                delay_ms(time);
                                             }
                                           else if (reg_desplazamiento == 0b00001001)////  1100 1001 0011 0110
                                             {
                                                reg_desplazamiento = 0b00000011;
                                                output_b (reg_desplazamiento);
                                                delay_ms(time);
                                             }
                                           else
                                             {
                                                reg_desplazamiento = reg_desplazamiento << 1;         // aunque cambie el argumento del output_b() este no cambia por el lach. a menos que se llame.
                                                output_b (reg_desplazamiento);
                                                delay_ms(time);
                                                
                                                 //////////////////////////////////////////////
                                             emergencia();
                                              Modificar_Velocidad();
                                                    /////////////////////////////////////////////
                                             }
                                       }
                                       
                        /////////////////////IR A LA POSICION GUARDADA DEL MOTOR2/////////////////////////////////////
              
                                 output_bit(pin_b0,0);  // apagamos bits del motor1
                                 output_bit(pin_b1,0);
                                 output_bit(pin_b2,0);
                                 output_bit(pin_b3,0);
                                 output_bit(pin_c4,0);  // apagamos bits del motor3
                                 output_bit(pin_c5,0);
                                 output_bit(pin_c6,0);
                                 output_bit(pin_c7,0);
                                 
                                 reg_desplazamiento = 0b1100000;  // comenzar a mover el motor2
                               
                                for (int16 w2 = 0 ; w2 < reg_memory_2 ; w2++)
                                 {
                                                                                          
                                           if (reg_desplazamiento == 0b11000000)////  1100 1001 0011 0110
                                             {                                    
                                                reg_desplazamiento = 0b10010000;
                                                output_b (reg_desplazamiento);
                                                delay_ms(time);
                                             }
                                           else if (reg_desplazamiento == 0b10010000)////  1100 1001 0011 0110
                                             {
                                                reg_desplazamiento = 0b00110000;
                                                output_b (reg_desplazamiento);
                                                delay_ms(time);
                                             }
                                           else
                                             {
                                                reg_desplazamiento = reg_desplazamiento << 1;         // aunque cambie el argumento del output_b() este no cambia por el lach. a menos que se llame.
                                                output_b (reg_desplazamiento);
                                                delay_ms(time);
                                                 //////////////////////////////////////////////
                                                       emergencia();
                                                        Modificar_Velocidad();
                                                  /////////////////////////////////////////////
                                             }
                                       }
                                       
                                       
                       /////////////////////IR A LA POSICION GUARDADA DEL MOTOR3/////////////////////////////////////
                                 
                                 output_bit(pin_b0,0);  // apagamos bits del motor1
                                 output_bit(pin_b1,0);
                                 output_bit(pin_b2,0);
                                 output_bit(pin_b3,0);
                                 output_bit(pin_b4,0);  // apagamos bits del motor2
                                 output_bit(pin_b5,0);
                                 output_bit(pin_b6,0);
                                 output_bit(pin_b7,0);
                                 
                                 reg_desplazamiento = 0b1100000;  // comenzar a mover el motor3
                               
                                for (int16 w3 = 0 ; w3 < reg_memory_3 ; w3++)
                                 {
                                           if (reg_desplazamiento == 0b11000000)////  1100 1001 0011 0110
                                             {                                    
                                                reg_desplazamiento = 0b10010000;
                                                output_bit(pin_c7,1);
                                                output_bit(pin_c6,0);
                                                output_bit(pin_c5,0);
                                                output_bit(pin_c4,1);
                                                delay_ms(time);
                                             }
                                           else if (reg_desplazamiento == 0b10010000)////  1100 1001 0011 0110
                                             {
                                                reg_desplazamiento = 0b00110000;
                                                output_bit(pin_c7,0);
                                                output_bit(pin_c6,0);
                                                output_bit(pin_c5,1);
                                                output_bit(pin_c4,1);
                                                delay_ms(time);
                                             }
                                             else if (reg_desplazamiento == 0b00110000)////  1100 1001 0011 0110
                                             {
                                                reg_desplazamiento = 0b01100000;
                                                output_bit(pin_c7,0);
                                                output_bit(pin_c6,1);
                                                output_bit(pin_c5,1);
                                                output_bit(pin_c4,0);
                                                delay_ms(time);
                                             }
                                             else if (reg_desplazamiento == 0b01100000)////  1100 1001 0011 0110
                                             {
                                                reg_desplazamiento = 0b11000000;
                                                output_bit(pin_c7,1);
                                                output_bit(pin_c6,1);
                                                output_bit(pin_c5,0);
                                                output_bit(pin_c4,0);
                                                delay_ms(time);
                                                 //////////////////////////////////////////////
                                                emergencia();
                                                 Modificar_Velocidad();
                                                     /////////////////////////////////////////////
                                             }
                                       }
              //////////////////////RETARDO///////////////////////////
                                  delay_ms(1000);
              ///////////////////////////////////////////////
             
        ////////////////SERVOMOTOR CUANDO LOS 3 MOTORES ESTAN EN SUS POSICIONES/////////////////////
                   
                 setup_timer_0(RTCC_INTERNAL | RTCC_8_BIT |RTCC_DIV_2);
                 set_timer0(6);                  // desborda en 100us // cada tic es de 400ns       
                 enable_interrupts(GLOBAL);
                 enable_interrupts(INT_TIMER0);
             
                 DUTY=1;          //SERVO SE MUEVE A 180 GRADOS
                 delay_ms(2000);
             
                 DUTY = 0;        //SERVO SE MUEVE A 0 GRADOS DE NUEVO
                 delay_ms(2000);
                 
                 disable_interrupts(GLOBAL);
                 disable_interrupts(INT_TIMER0);
                 
                 break;
             */                      
        
/*
void emergencia()
{
           //////////////////INICIALIZAR MOTOR 1///////////////////////////////////
                                  if (input(pin_c0) == 1) ///////boton de emergencia/////
                                  {
                                 reg_desplazamiento = 0b00000011;
                                 
                                          for (int16 w1 = reg_memory_1 ; w1 > 0 ; w1=w1-1)
                                          {
                                
                                              if (reg_desplazamiento == 0b00000011)
                                             {
                                                reg_desplazamiento = 0b00001001;
                                                output_b (reg_desplazamiento);
                                                delay_ms(time);
                                             }
                                          else if (reg_desplazamiento == 0b00001001)
                                             {
                                                reg_desplazamiento = 0b00001100;
                                                output_b (reg_desplazamiento);
                                                delay_ms(time);
                                             }
                                          else
                                             {
                                                reg_desplazamiento = reg_desplazamiento >> 1;        
                                                output_b (reg_desplazamiento);
                                                delay_ms(time);  
                                                
                                             }
                                          }
                                
                  ////////////////////INICIALIZAR MOTOR 2//////////////////////////////
                              
                                 reg_desplazamiento = 0b00110000;
                                 
                                          for (int16 w2 = reg_memory_2 ; w2 > 0 ; w2=w2-1)
                                          {
                                             if (reg_desplazamiento == 0b00110000)
                                             {
                                                reg_desplazamiento = 0b10010000;
                                                output_b (reg_desplazamiento);
                                                delay_ms(time);
                                             }
                                          else if (reg_desplazamiento == 0b10010000)
                                             {
                                                reg_desplazamiento = 0b11000000;
                                                output_b (reg_desplazamiento);
                                                delay_ms(time);
                                             }
                                          else
                                             {
                                                reg_desplazamiento = reg_desplazamiento >> 1;         
                                                output_b (reg_desplazamiento);
                                                delay_ms(time);
                                               
                                             }      
                                          }
                                          
                 ////////////////////INICIALIZAR MOTOR 3//////////////////////////////
                                
                                 reg_desplazamiento = 0b11000000;
                                 
                                          for (int16 w3 = reg_memory_3 ; w3 > 0 ; w3=w3-1)
                                          {
                                              if (reg_desplazamiento == 0b11000000)////  1100 1001 0011 0110
                                             {                                    
                                                reg_desplazamiento = 0b01100000;
                                                output_bit(pin_c7,0);
                                                output_bit(pin_c6,1);
                                                output_bit(pin_c5,1);
                                                output_bit(pin_c4,0);
                                                delay_ms(time);
                                             }
                                           else if (reg_desplazamiento == 0b01100000)////  1100 1001 0011 0110
                                             {
                                                reg_desplazamiento = 0b00110000;
                                                output_bit(pin_c7,0);
                                                output_bit(pin_c6,0);
                                                output_bit(pin_c5,1);
                                                output_bit(pin_c4,1);
                                                delay_ms(time);
                                             }
                                             else if (reg_desplazamiento == 0b00110000)////  1100 1001 0011 0110
                                             {
                                                reg_desplazamiento = 0b10010000;
                                                output_bit(pin_c7,1);
                                                output_bit(pin_c6,0);
                                                output_bit(pin_c5,0);
                                                output_bit(pin_c4,1);
                                                delay_ms(time);
                                             }
                                             else if (reg_desplazamiento == 0b10010000)////  1100 1001 0011 0110
                                             {
                                                reg_desplazamiento = 0b11000000;
                                                output_bit(pin_c7,1);
                                                output_bit(pin_c6,1);
                                                output_bit(pin_c5,0);
                                                output_bit(pin_c4,0);
                                                delay_ms(time);
                                                
                                        
                                             }
                                          }
                                          while(1)
                                          {}
                                  }
}
*/

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

void Modificar_Velocidad()
{
       set_adc_channel(0);               //potenciometro 1 para variar velocidad.      10 <time< 25 
       delay_us(100);
       adc = read_adc();        // 0-1024
       speed = adc/30;   // 0-34
       if(speed > 9)
       {
            time = speed;
       }
       else
       { time = 9;}
}







void Paro_Emergencia()
{
      if(input(pin_c0) == 1)
      {
            while(input(pin_c3) == 0)
            { output_bit(pin_a3,0);
              output_bit(pin_a4,0);
              }
            
      }
}


