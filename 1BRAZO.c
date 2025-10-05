#include <1BRAZO.h>

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
//////////////////////////////////


//////Variables a usar//////
int1 DUTY;
int count1;
int16 adc;                        //toma el valor de 10bits del ADC
int16 adc_escalado;                // divide para ajustar el valor
int16 posicion1;                   // posicion a lazo abierto del motor 1 
int16 posicion2;                   // posicion a lazo abierto del motor 2 
int posicion3;                   // posicion a lazo abierto del motor 3 
int reg_desplazamiento;           // registro para ir dezplazando los bits para mover los motores
int16 time;                        //tiempo en milisegundos para cada pulso al motor
int estado_actual;              
int16 reg_memory_1;                  // registro para registrar la posicion a lazo abierto del motor 1
int reg_memory_2;                  // registro para registrar la posicion a lazo abierto del motor 2
int reg_memory_3;







void main()
{
   setup_adc_ports(sAN0 | sAN1 | sAN2, VSS_VDD);
   setup_adc(ADC_CLOCK_INTERNAL | ADC_TAD_MUL_0 | ADC_LEGACY_MODE | ADC_THRESHOLD_INT_DISABLED);
   initial();      // inicializar variables
       output_bit(pin_b4,0);  // apagamos bits del motor2
                                 output_bit(pin_b5,0);
                                 output_bit(pin_b6,0);
                                 output_bit(pin_b7,0);
                                 output_bit(pin_c4,0);  // apagamos bits del motor3
                                 output_bit(pin_c5,0);
                                 output_bit(pin_c6,0);
                                 output_bit(pin_c7,0);
                                 output_bit(pin_a3,1);
                                   output_bit(pin_a4,1);
                                    reg_desplazamiento = 0b00001100;  // comenzar a mover el motor1
   while(TRUE)
   {
    setup_adc_ports(sAN0 | sAN1 | sAN2, VSS_VDD);
    setup_adc(ADC_CLOCK_INTERNAL);
                                 set_adc_channel(0);
                                 delay_us(100);
                                 adc = read_adc();
                                 adc_escalado = adc*2;    // como el motor es de 2048 pasos cada vuelta, escalamos los 1023 binarios a 2048 posiciones enteras...!!
                            
                             
                                   
                                
                                                                                          
                                 
                                 if (posicion1 == adc_escalado || posicion1 == adc_escalado +1 || posicion1 == adc_escalado -1 || posicion1 == adc_escalado -2 || posicion1 == adc_escalado +2 || posicion1 == adc_escalado -3 || posicion1 == adc_escalado +3) // si la posicion a lazo abierto es similar a la posicion del potenciometro todo se queda quieto..!!
                                       {
                                  
                                       }
                                 else if (posicion1 < adc_escalado)  // si la posicion del potenciometro es mayor a la posicion del motor a lazo abierto, haz que mueva a un lado... 
                                       {
                                           if (reg_desplazamiento == 0b00001100)////  1100 1001 0011 0110
                                             {                                    
                                                reg_desplazamiento = 0b00001001;
                                                posicion1 = posicion1 + 1;
                                                output_b (reg_desplazamiento);
                                                delay_ms(time);
                                             }
                                           else if (reg_desplazamiento == 0b00001001)////  1100 1001 0011 0110
                                             {
                                                reg_desplazamiento = 0b00000011;
                                                posicion1 = posicion1 + 1;
                                                output_b (reg_desplazamiento);
                                                delay_ms(time);
                                             }
                                           else
                                             {
                                                reg_desplazamiento = reg_desplazamiento << 1;         // aunque cambie el argumento del output_b() este no cambia por el lach. a menos que se llame.
                                                posicion1 = posicion1 + 1;
                                                output_b (reg_desplazamiento);
                                                delay_ms(time);
                                             }
                                       }
                                    
                                   else if (posicion1 > adc_escalado) // si la posicion del potenciometro es menor a la posicion del motor a lazo abierto, haz que mueva al otro lado... 
                                     {   
                                          if (reg_desplazamiento == 0b00000011)
                                             {
                                                reg_desplazamiento = 0b00001001;
                                                posicion1 = posicion1 - 1;
                                                output_b (reg_desplazamiento);
                                                delay_ms(time);
                                             }
                                          else if (reg_desplazamiento == 0b00001001)
                                             {
                                                reg_desplazamiento = 0b00001100;
                                                posicion1 = posicion1 - 1;
                                                output_b (reg_desplazamiento);
                                                delay_ms(time);
                                             }
                                          else
                                             {
                                                reg_desplazamiento = reg_desplazamiento >> 1;         // aunque cambie el argumento del output_b() este no cambia por el lach. a menos que se llame.
                                                posicion1 = posicion1 - 1;
                                                output_b (reg_desplazamiento);
                                                delay_ms(time);
                                             }      
                                    }      
   }
}
  

void initial ()
{
DUTY=0; //POR DEFAULT EL SERVO EN 0 GRADOS
count1 =0;
adc=0;
adc_escalado=0;
posicion1=0;
posicion2=0;
posicion3=0;
reg_memory_1=0;
reg_memory_2=0;
reg_memory_3=0;
reg_desplazamiento = 0b11000000;
time=20;
output_b(0b00000000);
output_bit(pin_c4,0);
output_bit(pin_c5,0);
output_bit(pin_c6,0);
output_bit(pin_c7,0);
output_bit(pin_A5,0);
estado_actual=0;
}






     
